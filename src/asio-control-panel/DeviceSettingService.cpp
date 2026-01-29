// Copyright (c) Yamaha Corporation
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Microsoft Low-Latency Audio driver project.
// Further information: https://aka.ms/asio
// ============================================================================
// ASIO is a registered trademark of Steinberg Media Technologies GmbH.

#include "pch.h"

_Use_decl_annotations_
concurrency::task<HRESULT> DeviceSettingService::SetDevicesSampleRateAsync(HANDLE handle, uint32_t sampleRate)
{
    return concurrency::create_task([=]
    {
        //OutputDebugString(L" Set Devices SmapleRate Start\n");

        HRESULT result = S_OK;

        KSPROPERTY ksprop{};
        ksprop.Set = KSPROPSETID_LowLatencyAudio;
        ksprop.Flags = KSPROPERTY_TYPE_SET;
        ksprop.Id = static_cast<int>(KsPropertyUACLowLatencyAudio::ChangeSampleRate);
        uint32_t val = sampleRate;

        result = SyncIoctl(handle, IOCTL_KS_PROPERTY, &ksprop, sizeof(KSPROPERTY), &val, sizeof(val), nullptr);

        //OutputDebugString(L" Set Devices SmapleRate End\n");

        return result;
    });
}

_Use_decl_annotations_
concurrency::task<HRESULT> DeviceSettingService::GetDevicesSampleRateAsync(HANDLE handle, uint32_t *sampleRate)
{
    return concurrency::create_task([=]
        {
            //OutputDebugString(L" Get Devices SmapleRate Start\n");

            HRESULT result = S_OK;

            UAC_AUDIO_PROPERTY audioProp = {};
            if (SUCCEEDED(FilterGetUacAudioProperty(handle, &audioProp)))
            {
                *sampleRate = static_cast<uint32_t>(audioProp.SampleRate);
            }
            else
            {
                result = S_FALSE;
            }

            //OutputDebugString(L" Get Devices SmapleRate End\n");

            return result;
        });
}

_Use_decl_annotations_
concurrency::task<HRESULT> DeviceSettingService::SetAsioBufferSizeAsync(HANDLE handle, uint16_t bufferSize)
{
    return concurrency::create_task([=]
    {
        //OutputDebugString(L" Set Asio Buffer Size Start\n");

        HRESULT result = S_OK;
        KSPROPERTY ksprop{};
        ksprop.Set = KSPROPSETID_LowLatencyAudio;
        ksprop.Flags = KSPROPERTY_TYPE_SET;
        ksprop.Id = static_cast<int>(KsPropertyUACLowLatencyAudio::SetBufferPeriod);
        ULONG val = bufferSize;

        result = SyncIoctl(handle, IOCTL_KS_PROPERTY, &ksprop, sizeof(KSPROPERTY), &val, sizeof(val), nullptr);

        //OutputDebugString(L" Set Asio Buffer Size End\n");

        return result;
    });
}

_Use_decl_annotations_
concurrency::task<HRESULT> DeviceSettingService::GetAsioBufferSizeAsync(HANDLE handle, uint16_t *bufferSize)
{
    return concurrency::create_task([=]
    {
        //OutputDebugString(L" Get Asio Buffer Size Start\n");

        HRESULT result = S_OK;

        uint16_t val = 0;
        if (SUCCEEDED(FilterGetAsioBufferSize(handle, &val)))
        {
            *bufferSize = val;
        }
        else
        {
            result = S_FALSE;
        }

        //OutputDebugString(L" Get Asio Buffer Size End\n");

        return result;
    });
}

_Use_decl_annotations_
concurrency::task<HRESULT> DeviceSettingService::SetAsioDevicePathAsync(HANDLE handle, std::wstring &devicePath)
{
    return concurrency::create_task([handle, &devicePath]
    {

        HRESULT result = S_OK;

        KSPROPERTY ksprop{};
        ksprop.Set = KSPROPSETID_LowLatencyAudio;
        ksprop.Flags = KSPROPERTY_TYPE_SET;
        ksprop.Id = toInt(KsPropertyUACLowLatencyAudio::SetAsioDevice);

        LPCWSTR asioDevicePath = devicePath.c_str();
        ULONG asioDevicePathSize = static_cast<ULONG>(devicePath.size() * sizeof(wchar_t));

        result = SyncIoctl(handle, IOCTL_KS_PROPERTY, &ksprop, sizeof(KSPROPERTY), (PVOID)asioDevicePath, asioDevicePathSize, nullptr);

        return result;
    });
}

_Use_decl_annotations_
concurrency::task<HRESULT> DeviceSettingService::GetAsioDevicePathAsync(HANDLE handle, std::wstring &devicePath)
{
    return concurrency::create_task([handle, &devicePath]
    {
        HRESULT result = S_OK;

        KSPROPERTY ksprop{};
        ksprop.Set = KSPROPSETID_LowLatencyAudio;
        ksprop.Flags = KSPROPERTY_TYPE_GET;
        ksprop.Id = toInt(KsPropertyUACLowLatencyAudio::GetAsioDevice);

        std::wstring asioDevicePath;
        asioDevicePath.resize(MAX_PATH);
        ULONG asioDevicePathSize = static_cast<ULONG>(asioDevicePath.size() * sizeof(wchar_t));
        ULONG bytesReturned = 0;
        
        result = SyncIoctl(handle, IOCTL_KS_PROPERTY, &ksprop, sizeof(KSPROPERTY), asioDevicePath.data(), asioDevicePathSize, &bytesReturned);

        if (bytesReturned)
        {
            asioDevicePath.resize((bytesReturned / sizeof(wchar_t))-1);
        }
        devicePath = asioDevicePath;

        OutputDebugString((L"GetAsioDevicePathAsync() " + winrt::to_hstring(devicePath.c_str()) + L"\n").c_str());

        return result;

    });
}


