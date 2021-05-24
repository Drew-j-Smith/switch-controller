#include "SoundDecider.h"

static double dotProduct(const std::vector<float> & v) {
    double total = 0.0f;
    for (int i = 0; i < v.size(); i++) {
        total += v[i] * v[i];
    }
    return total;
}

static double dotProduct(const std::vector<float> & v1, const std::vector<float> & v2) {
    double total = 0.0;
    int max = v1.size() < v2.size() ? v1.size() : v2.size();
    for (int i = 0; i < max; i++) {
        total += v1[i] * v2[i];
    }
    return total;
}

static std::vector<float> scalarMultiplication(const std::vector<float> & v, const float scale) {
    std::vector<float> v2(v.size());
    for (int i = 0; i < v.size(); i++) {
        v2[i] = v[i] * scale;
    }
    return v2;
}

static std::vector<float> vectorSubtraction(const std::vector<float> & v1, const std::vector<float> & v2) {
    std::vector<float> v3(v1.size() < v2.size() ? v1.size() : v2.size());
    for (int i = 0; i < v3.size(); i++) {
        v3[i] = v1[i] - v2[i];
    }
    return v3;
}

static std::vector<float> vectorSubtraction(const std::vector<float> & v, const float scalar) {
    std::vector<float> v3(v.size());
    for (int i = 0; i < v3.size(); i++) {
        v3[i] = v[i] - scalar;
    }
    return v3;
}

static double vectorMean(const std::vector<float> & v) {
    double sum = 0;
    for (int i = 0; i < v.size(); i++) {
        sum += v[i];
    }
    return sum / v.size();
}

static float conjugate(const fftwf_complex & num) {
    return num[0] * num[0] + num[1] * num[1];
}

static std::vector<float> findFrequencies(std::shared_ptr<fftwPlan> plan, const std::vector<float> & samples) {
    for (long long i = 0; i < plan->getInSize(); i++) {
        plan->getIn()[i] = samples[i];
    }

    plan->execute();

    std::vector<float> frequencies(plan->getOutSize());
    for (long long i = 0; i < frequencies.size(); i++) {
        //freq gap = 1/(dt*N)
        frequencies[i] = (conjugate(plan->getOut()[i])) / plan->getInSize();
    }
    return frequencies;
}

SoundDecider::SoundDecider(const boost::property_tree::ptree & tree) {
    try {
        name = tree.get<std::string>("name");
        matchAudio = AudioFile<float>(tree.get<std::string>("filename"));
        matchThreshold = tree.get<double>("match threshold");
    }
    catch (boost::property_tree::ptree_error & e) {
        std::cerr << e.what() << "\n";
        std::cerr << "Error loading SoundDecider";
    }

    plan = std::make_shared<fftwPlan>(matchAudio.getNumSamplesPerChannel());

    matchFrequencies = findFrequencies(plan, matchAudio.samples[0]);

    matchValue.store(0);
}

void SoundDecider::update(std::vector<float> soundData) {
    auto testFrequencies = findFrequencies(plan, soundData);

    // least square approx.
    double vectorScale = dotProduct(matchFrequencies, testFrequencies) / dotProduct(matchFrequencies);
    std::vector<float> expected = scalarMultiplication(matchFrequencies, vectorScale);

    // error calculation (R squared)
    double mean = vectorMean(testFrequencies);
    double error = 1 - dotProduct(vectorSubtraction(testFrequencies, expected)) / dotProduct(vectorSubtraction(testFrequencies, mean));

    matchValue.store(error);
    // std::cout << "Least Square Approx.: " << vectorScale << " * original vector" << std::endl;
    // std::cout << "R Squared: " << error << std::endl;
}

int SoundDecider::nextListIndex() const {
    return matchValue.load() > matchThreshold;
}