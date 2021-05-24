#ifndef AUDIO_SINK_H
#define AUDIO_SINK_H

#include "pch.h"

#include <Windows.h>
#include <mmdeviceapi.h>
#include <mmreg.h>
#include <Audioclient.h>

#define SAFE_RELEASE(punk)  \
            if ((punk) != NULL)  \
            { (punk)->Release(); (punk) = NULL; }

class AudioSink
{
public:
    struct AudioInfo {
        long long numChannels;
        long long numSamplesPerSec;
        long long bitsPerSample;
    };
private:
    IMMDeviceEnumerator *pEnumerator = NULL;
    IMMDevice *pDevice = NULL;
    IAudioClient *pAudioClient = NULL;
    IAudioCaptureClient *pCaptureClient = NULL;
    WAVEFORMATEX *pwfx = NULL;

    std::mutex mMutex;

    std::vector<std::vector<float>> data;

    long long numSamples = 0;
    double duration = 0;
    long long currentSample = 0;
    std::atomic<bool> finished = false;

    AudioInfo audioInfo;

public:
    AudioSink(long long numSamples, AudioInfo audioInfo) : audioInfo(audioInfo) { this->numSamples = numSamples; }
    AudioSink(double duration, AudioInfo audioInfo) : audioInfo(audioInfo) { this->duration = duration; }

    AudioInfo getAudioInfo() { return audioInfo; };
    std::vector<std::vector<float>> getData() {
        std::vector<std::vector<float>> copy(audioInfo.numChannels);
        std::lock_guard<std::mutex> gaurd(mMutex);
        for (int i = 0; i < copy.size(); i++) {
            copy[i].resize(numSamples);
            std::copy(data[i].begin() + currentSample, data[i].end(), copy[i].begin());
            std::copy(data[i].begin(), data[i].begin() + currentSample, copy[i].end() - currentSample);
        }
        return copy; //todo
    }

    void setFinished(bool finished) { this->finished.store(finished); }

    HRESULT RecordAudioStreamLoopback();
private:
    HRESULT SetFormat(WAVEFORMATEX* pwfx);
    HRESULT CopyData(BYTE* pData, UINT32 numFramesAvailable, BOOL* bDone);

    void cleanup() {
        CoTaskMemFree(pwfx);
        SAFE_RELEASE(pEnumerator)
        SAFE_RELEASE(pDevice)
        SAFE_RELEASE(pAudioClient)
        SAFE_RELEASE(pCaptureClient)
        CoUninitialize();
        std::cerr << "cleanup\n";
    }
};

#endif