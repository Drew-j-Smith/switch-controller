#include "SoundDecider.h"

#include "FFmpeg/AudioFrameSink.h"
#include "FFmpeg/FFmpegRecorder.h"

static double dotProduct(const std::vector<float> &v) {
    double total = 0.0f;
    for (int i = 0; i < (int)v.size(); i++) {
        total += v[i] * v[i];
    }
    return total;
}

static double dotProduct(const std::vector<float> &v1,
                         const std::vector<float> &v2) {
    double total = 0.0;
    int max = (int)(v1.size() < v2.size() ? v1.size() : v2.size());
    for (int i = 0; i < max; i++) {
        total += v1[i] * v2[i];
    }
    return total;
}

static std::vector<float> scalarMultiplication(const std::vector<float> &v,
                                               const double scale) {
    std::vector<float> v2(v.size());
    for (int i = 0; i < (int)v.size(); i++) {
        v2[i] = (float)(v[i] * scale);
    }
    return v2;
}

static std::vector<float> vectorSubtraction(const std::vector<float> &v1,
                                            const std::vector<float> &v2) {
    std::vector<float> v3(v1.size() < v2.size() ? v1.size() : v2.size());
    for (int i = 0; i < (int)v3.size(); i++) {
        v3[i] = v1[i] - v2[i];
    }
    return v3;
}

static std::vector<float> vectorSubtraction(const std::vector<float> &v,
                                            const double scalar) {
    std::vector<float> v3(v.size());
    for (int i = 0; i < (int)v3.size(); i++) {
        v3[i] = (float)(v[i] - scalar);
    }
    return v3;
}

static double vectorMean(const std::vector<float> &v) {
    double sum = 0;
    for (int i = 0; i < (int)v.size(); i++) {
        sum += v[i];
    }
    return sum / v.size();
}

std::vector<float>
SoundDecider::findFrequencies(const std::vector<float> &samples) {
    for (long long i = 0; i < fftwSize; i++) {
        fftwIn[i] = samples[i];
    }

    fftwf_execute(fftwPlan);

    std::vector<float> frequencies(fftwSize / 2 + 1);
    for (unsigned long long i = 0; i < frequencies.size(); i++) {
        // freq gap = 1/(dt*N)
        frequencies[i] = std::conj(fftwOut[i]).real() / fftwSize;
    }
    return frequencies;
}

SoundDecider::SoundDecider(const boost::property_tree::ptree &tree) {
    std::string filename;
    try {
        name = tree.get<std::string>("name");
        filename = tree.get<std::string>("filename");
        matchThreshold = tree.get<double>("match threshold");
    } catch (boost::property_tree::ptree_error &e) {
        std::cerr << "Error loading SoundDecider.\n";
        std::cerr << "\tError: \"" << e.what() << "\"\n";
        std::cerr << "\tAre fields [\"name\", \"filename\", \"match "
                     "threshold\"] missing?\n";
        throw;
    }

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
    fftwPlan = fftwf_plan_dft_r2c_1d(
        fftwSize, fftwIn.data(), (fftwf_complex *)fftwOut.data(), FFTW_MEASURE);
    matchFrequencies = findFrequencies(matchAudio);
    matchValue.store(0);
}

void SoundDecider::update(std::vector<float> soundData) {
    auto testFrequencies = findFrequencies(soundData);

    // least square approx.
    double vectorScale = dotProduct(matchFrequencies, testFrequencies) /
                         dotProduct(matchFrequencies);
    std::vector<float> expected =
        scalarMultiplication(matchFrequencies, vectorScale);

    // error calculation (R squared)
    double mean = vectorMean(testFrequencies);
    double error =
        1 - dotProduct(vectorSubtraction(testFrequencies, expected)) /
                dotProduct(vectorSubtraction(testFrequencies, mean));

    matchValue.store(error);
    // std::cout << "Least Square Approx.: " << vectorScale << " * original
    // vector" << std::endl; std::cout << "R Squared: " << error << std::endl;
}

int SoundDecider::nextListIndex() const {
    return matchValue.load() > matchThreshold;
}