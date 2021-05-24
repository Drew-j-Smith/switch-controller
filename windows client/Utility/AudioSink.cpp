#include "AudioSink.h"

static bool SetIfZeroElseReturnEqual (long long & o1, long long o2) {
    if (o1 == 0) {
        o1 = o2;
        return true;
    }
    return o1 == o2;
}

HRESULT AudioSink::SetFormat(WAVEFORMATEX* pwfx) {
    if (!SetIfZeroElseReturnEqual(audioInfo.numChannels, pwfx->nChannels)
        || !SetIfZeroElseReturnEqual(audioInfo.numSamplesPerSec, pwfx->nSamplesPerSec)
        || !SetIfZeroElseReturnEqual(audioInfo.bitsPerSample, pwfx->wBitsPerSample)
        || audioInfo.bitsPerSample != 32) {
        std::cerr << "Unsupported Sound Format\n";
        return -1;
    }
    
    if (numSamples == 0)
        numSamples = audioInfo.numSamplesPerSec * duration;

    data.resize(audioInfo.numChannels);
    for (int i = 0; i < data.size(); i++)
        data[i].resize(numSamples);

    
    return 1;
};

HRESULT AudioSink::CopyData(BYTE* pData, UINT32 numFramesAvailable, BOOL* bDone) { 
    if (pData == NULL) {
        return 1;
    }

    *bDone = finished.load();

    std::lock_guard<std::mutex> gaurd(mMutex);
    for (int i = 0; i < numFramesAvailable; i++) {
        for (int j = 0; j < audioInfo.numChannels; j++)
            data[j][currentSample] = *(float*)&pData[i * audioInfo.bitsPerSample * audioInfo.numChannels / 8 + j * 4];
        currentSample++;
        if (currentSample == numSamples)
            currentSample = 0;
    }


    
    return 1;
};

//-----------------------------------------------------------
// Record an audio stream from the default audio capture
// device. The RecordAudioStream function allocates a shared
// buffer big enough to hold one second of PCM audio data.
// The function uses this buffer to stream data from the
// capture device. The main loop runs every 1/20 second.
//-----------------------------------------------------------

// REFERENCE_TIME time units per second and per millisecond

#define REFTIMES_PER_SEC  500000
#define REFTIMES_PER_MILLISEC  500

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { cleanup(); return hres; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

HRESULT AudioSink::RecordAudioStreamLoopback() {
    HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;

    UINT32 packetLength = 0;
    BOOL bDone = FALSE;
    BYTE *pData;
    DWORD flags;

    hr = CoInitialize(NULL);
    EXIT_ON_ERROR(hr)

    hr = CoCreateInstance(
           CLSID_MMDeviceEnumerator, NULL,
           CLSCTX_ALL, IID_IMMDeviceEnumerator,
           (void**)&pEnumerator);
    EXIT_ON_ERROR(hr)

    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    EXIT_ON_ERROR(hr)

    hr = pDevice->Activate(
                    IID_IAudioClient, CLSCTX_ALL,
                    NULL, (void**)&pAudioClient);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetMixFormat(&pwfx);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->Initialize(
                         AUDCLNT_SHAREMODE_SHARED,
                         AUDCLNT_STREAMFLAGS_LOOPBACK,
                         hnsRequestedDuration,
                         0,
                         pwfx,
                         NULL);
    EXIT_ON_ERROR(hr)

    // Get the size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetService(
                         IID_IAudioCaptureClient,
                         (void**)&pCaptureClient);
    EXIT_ON_ERROR(hr)

    // Notify the audio sink which format to use.
    hr = SetFormat(pwfx);
    EXIT_ON_ERROR(hr)

    // Calculate the actual duration of the allocated buffer.
    hnsActualDuration = (double)REFTIMES_PER_SEC *
                     bufferFrameCount / pwfx->nSamplesPerSec;

    hr = pAudioClient->Start();  // Start recording.
    EXIT_ON_ERROR(hr)

    // Each loop fills about half of the shared buffer.
    while (bDone == FALSE)
    {
        // Sleep for half the buffer duration.
        Sleep(hnsActualDuration/REFTIMES_PER_MILLISEC/2);

        hr = pCaptureClient->GetNextPacketSize(&packetLength);
        EXIT_ON_ERROR(hr)

        while (packetLength != 0)
        {
            // Get the available data in the shared buffer.
            hr = pCaptureClient->GetBuffer(
                                   &pData,
                                   &numFramesAvailable,
                                   &flags, NULL, NULL);
            EXIT_ON_ERROR(hr)

            if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
            {
                pData = NULL;  // Tell CopyData to write silence.
            }

            // Copy the available capture data to the audio sink.
            hr = CopyData(pData, numFramesAvailable, &bDone);
            EXIT_ON_ERROR(hr)

            hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
            EXIT_ON_ERROR(hr)

            hr = pCaptureClient->GetNextPacketSize(&packetLength);
            EXIT_ON_ERROR(hr)
        }
    }

    hr = pAudioClient->Stop();  // Stop recording.
    EXIT_ON_ERROR(hr)
    
    return 1;
}