#include "SoundEvent.h"

#include "FFmpeg/AudioFrameSink.h"
#include "FFmpeg/FFmpegRecorder.h"

std::vector<float>
SoundEvent::findFrequencies(const std::vector<float> &samples) const {
    for (long long i = 0; i < fftwSize; i++) {
        fftwIn[i] = samples[i];
    }

    fftwf_execute(*fftwPlan);

    std::vector<float> frequencies(fftwSize / 2 + 1);
    for (unsigned long long i = 0; i < frequencies.size(); i++) {
        // freq gap = 1/(dt*N)
        frequencies[i] = std::conj(fftwOut[i]).real() / fftwSize;
    }
    return frequencies;
}

SoundEvent::SoundEvent(const std::string &filename, double matchThreshold,
                       std::shared_ptr<AudioFrameSink> audioFrameSink) {
    this->matchThreshold = matchThreshold;
    this->audioFrameSink = audioFrameSink;

    std::vector<std::shared_ptr<FFmpegFrameSink>> sinks;
    std::shared_ptr<AudioFrameSink> audioSink =
        std::make_shared<AudioFrameSink>(AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S16,
                                         48000);
    sinks.push_back(audioSink);

    av_log_set_level(AV_LOG_QUIET);

    FFmpegRecorder ffmpegRecorder("", filename, {}, sinks);
    ffmpegRecorder.start();
    audioSink->waitForInit();
    ffmpegRecorder.join();
    ffmpegRecorder.stop();

    std::vector<uint8_t> rawData;
    audioSink->getData(rawData);
    matchAudio = std::vector<float>((float *)rawData.data(),
                                    (float *)(rawData.data() + rawData.size()));

    fftwSize = (int)matchAudio.size();
    fftwIn.resize(fftwSize);
    fftwOut.resize(fftwSize / 2 + 1);
    *fftwPlan = fftwf_plan_dft_r2c_1d(
        fftwSize, fftwIn.data(), (fftwf_complex *)fftwOut.data(), FFTW_MEASURE);
    matchFrequencies = findFrequencies(matchAudio);
    matchValue.store(0);
}

uint8_t SoundEvent::value() const {
    audioFrameSink->getData(audioData);
    std::vector<float> soundData(
        (float *)audioData.data(),
        (float *)(audioData.data() + audioData.size()));

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

    float mean = std::reduce(
        testFrequencies.begin(), testFrequencies.end(), 0.f,
        [&](float f1, float f2) { return f1 + f2 / testFrequencies.size(); });

    std::transform(testFrequencies.begin(), testFrequencies.end(), temp.begin(),
                   [&](float f) {
                       float diff = f - mean;
                       return diff * diff;
                   });
    float testMinusMean = std::accumulate(temp.begin(), temp.end(), 0.f);
    float rSquaredError = 1 - testMinusApprox / testMinusMean;

    return rSquaredError > matchThreshold;
    // std::cout << "Least Square Approx.: " << vectorScale << " * original
    // vector" << std::endl; std::cout << "R Squared: " << error << std::endl;
}