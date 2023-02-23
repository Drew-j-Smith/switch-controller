#include "SoundEvent.h"

#include "FFmpeg/AudioFrameSink.h"
#include "FFmpeg/FFmpegRecorder.h"

std::vector<float>
SoundEvent::findFrequencies(const std::vector<float> &samples) const {
    for (uint64_t i = 0; i < fftwSize; i++) {
        fftwIn[i] = samples[i];
    }

    fftwf_execute(*fftwPlan);

    std::vector<float> frequencies(fftwSize / 2 + 1);
    for (std::size_t i = 0; i < frequencies.size(); i++) {
        // freq gap = 1/(dt*N)
        frequencies[i] =
            std::conj(fftwOut[i]).real() / static_cast<float>(fftwSize);
    }
    return frequencies;
}

SoundEvent::SoundEvent(const std::string &filename, double matchThreshold,
                       AudioFrameSink *audioFrameSink) {
    this->matchThreshold = matchThreshold;
    this->audioFrameSink = audioFrameSink;

    std::vector<std::unique_ptr<FFmpegFrameSink>> sinks;
    sinks.push_back(std::make_unique<AudioFrameSink>(AV_CH_LAYOUT_MONO,
                                                     AV_SAMPLE_FMT_S16, 48000));
    auto audioSink = sinks[0].get();

    av_log_set_level(AV_LOG_QUIET);

    FFmpegRecorder recorder("", filename, {}, sinks);
    recorder.joinThread();

    std::vector<uint8_t> rawData;
    audioSink->getData(rawData);
    matchAudio = {};
    for (std::size_t i = 0; i < rawData.size(); i += 4) {
        float f;
        std::memcpy(&f, &rawData[i], 4);
        matchAudio.push_back(f);
    }

    fftwSize = matchAudio.size();
    fftwIn.resize(fftwSize);
    fftwOut.resize(fftwSize / 2 + 1);
    auto fftwf_plan_ptr = new fftwf_plan;
    *fftwf_plan_ptr = fftwf_plan_dft_r2c_1d(
        static_cast<int>(fftwSize), fftwIn.data(),
        reinterpret_cast<fftwf_complex *>(fftwOut.data()), FFTW_MEASURE);
    fftwPlan = {fftwf_plan_ptr, fftwf_deleter()};
    matchFrequencies = findFrequencies(matchAudio);
    matchValue.store(0);
}

uint8_t SoundEvent::value() const {
    audioFrameSink->getData(audioData);
    std::vector<float> soundData = {};
    for (std::size_t i = 0; i < audioData.size(); i += 4) {
        float f;
        std::memcpy(&f, &audioData[i], 4);
        soundData.push_back(f);
    }

    auto testFrequencies = findFrequencies(soundData);

    // let w be a subspace of reals^len(matchFrequencies)
    // defined by the basis vector: matchFrequencies.
    // The following algorithm finds the closest vector
    // to testFrequencies in the subspace w
    // using least square approximation

    std::vector<float> temp;
    std::transform(matchFrequencies.begin(), matchFrequencies.end(),
                   testFrequencies.begin(), temp.begin(),
                   [](float f1, float f2) { return f1 * f2; });
    float matchDotProductTest = std::accumulate(temp.begin(), temp.end(), 0.f);

    float matchDotProductMatch =
        std::reduce(matchFrequencies.begin(), matchFrequencies.end(), 0.f,
                    [](float f1, float f2) { return f1 + f2 * f2; });

    float vectorScale = matchDotProductTest / matchDotProductMatch;

    std::vector<float> leastSquareApprox;
    std::transform(matchFrequencies.begin(), matchFrequencies.end(),
                   leastSquareApprox.begin(),
                   [&](float f) { return f * vectorScale; });

    // R squared comparing leastSquareApprox to testFrequencies

    std::transform(testFrequencies.begin(), testFrequencies.end(),
                   leastSquareApprox.begin(), temp.begin(),
                   [](float f1, float f2) {
                       float diff = f1 - f2;
                       return diff * diff;
                   });
    float testMinusApprox = std::accumulate(temp.begin(), temp.end(), 0.f);

    float mean = std::reduce(testFrequencies.begin(), testFrequencies.end(),
                             0.f, [&](float f1, float f2) {
                                 return f1 + f2 / static_cast<float>(
                                                      testFrequencies.size());
                             });

    std::transform(testFrequencies.begin(), testFrequencies.end(), temp.begin(),
                   [&](float f) {
                       float diff = f - mean;
                       return diff * diff;
                   });
    float testMinusMean = std::accumulate(temp.begin(), temp.end(), 0.f);
    float rSquaredError = 1 - testMinusApprox / testMinusMean;

    spdlog::debug("Least Square Approx.: {} * original vector\nR Squared: {}",
                  vectorScale, rSquaredError);
    return rSquaredError > matchThreshold;
}