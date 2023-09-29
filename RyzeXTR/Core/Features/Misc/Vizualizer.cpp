#include "Vizualizer.h"
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <iostream>
#include <deque>
using namespace cfg::vizualizer;

#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

#define volumeThreshold 0.65f

void vizualizer::Initialize() {

    HRESULT hr;
    IMMDeviceEnumerator* pEnumerator = NULL;
    IMMDevice* pDevice = NULL;
    IAudioMeterInformation* pMeterInfo = NULL;

    CoInitialize(NULL);

    // Get enumerator for audio endpoint devices.
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);

    // Get peak meter for default audio-rendering device.
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    hr = pDevice->Activate(__uuidof(IAudioMeterInformation), CLSCTX_ALL, NULL, (void**)&pMeterInfo);

    float flVolume = 0;
    // this grabs the main thread so need to be running in a separated thread
    // this function is relatively CPU heavy, needs further optimization

    std::deque<float> deqVolumeNumbers{};
    float flPeakVolume{};
    while (!cfg::bDoUnload) {

        if (!cfg::vizualizer::bEnabled)
            continue;

        pMeterInfo->GetPeakValue(&flVolume);

        if (bDynamic) {

            if (std::find(deqVolumeNumbers.begin(), deqVolumeNumbers.end(), flPeakVolume) == deqVolumeNumbers.end())
                flPeakVolume = flVolume;

            deqVolumeNumbers.push_front(flVolume);
            for (const float& it : deqVolumeNumbers)
                if (it > flPeakVolume)
                    flPeakVolume = it;

            float flThreshold = volumeThreshold / flPeakVolume;

            if (flVolume < volumeThreshold && flVolume > 0.1f)
                flVolume *= flThreshold;

            if (flVolume >= volumeThreshold - flSensitivity)
                bDoEffect = true;

            while (deqVolumeNumbers.size() > 32)
                deqVolumeNumbers.pop_back();
        }
        else {

            if (flVolume >= flStaticThreshold)
                bDoEffect = true;
        }

        Sleep(15);
    }

    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pDevice)
    SAFE_RELEASE(pMeterInfo)
    CoUninitialize();

    return;
}

void vizualizer::Draw(CBaseEntity* pLocal) {

    if (!cfg::vizualizer::bEnabled)
        return;

    if (pLocal && bDoEffect) {

        pLocal->HealthShotBoost() = i::GlobalVars->flCurrentTime + 0.2f;
        bDoEffect = false;
    }
}