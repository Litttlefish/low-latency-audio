// Copyright (c) Yamaha Corporation
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Microsoft Low-Latency Audio driver project.
// Further information: https://aka.ms/asio
// ============================================================================
// ASIO is a registered trademark of Steinberg Media Technologies GmbH.

#include "pch.h"

concurrency::task<std::vector<std::shared_ptr<DevicePropInfo>>> DeviceEnumerationService::GetControlledDevicesAsync()
//concurrency::task<std::vector<winrt::hstring>> DeviceEnumerationService::GetControlledDevicesAsync()
{
    // Yamaha: It's fine to change this back to using the PnP code you had before instead of using
    // Windows.Devices.Enumeration. However, ensure all strings used are wide strings everywhere, 
    // not _T()

	return concurrency::create_task([]
	{
        std::vector<std::shared_ptr<DevicePropInfo>> results{};
        //std::vector<winrt::hstring> results{};

        winrt::guid classGuid{KSCATEGORY_AUDIO};

        // additional properties to request

        winrt::hstring deviceInterfaceSelector(
            L"System.Devices.InterfaceClassGuid:=\"" + Utility::GuidToHStringWithBraces(classGuid) + "\" AND "
            L"System.Devices.InterfaceEnabled:= System.StructuredQueryType.Boolean#True"
        );

        auto additionalProps = winrt::single_threaded_vector<winrt::hstring>();

        additionalProps.Append(L"System.Devices.DeviceManufacturer");
        additionalProps.Append(L"System.Devices.Manufacturer");

        auto ksFilterDevices = DeviceInformation::FindAllAsync(
            deviceInterfaceSelector,
            additionalProps,
            DeviceInformationKind::DeviceInterface
        ).get();

        uint32_t validDeviceCount{ 0 };

        for (auto const& ksFilterDevice : ksFilterDevices)
        {

            //OutputDebugString(L"** Device: ");
            //OutputDebugString(ksFilterDevice.Id().c_str());
            //OutputDebugString(L"\n");
            //msg = L"** Device: " + winrt::to_hstring(ksFilterDevice.Id().c_str()) + L"\n";
            OutputDebugString((L"** Device: " + winrt::to_hstring(ksFilterDevice.Id()) + L"\n").c_str());

            wil::unique_handle filterHandle;
            UAC_AUDIO_PROPERTY audioProp = {};

            // open the filter handle
            if (!SUCCEEDED(FilterInstantiate(ksFilterDevice.Id().c_str(), &filterHandle)))
            {
                //OutputDebugString(L"Unable to instantiate filter.\n\r");
                continue;
            }

            // get the audio KS property. This is only for UAC, so needs to be adapted
            // if additional types of audio devices are added to the scope of this
            // ASIO control panel code
            if (SUCCEEDED(FilterGetUacAudioProperty(filterHandle.get(), &audioProp)))
            {
                //OutputDebugString(L"Retrieved UAC2 property from filter.\n\r");

                // TODO: FOllow the logic in the original example. It appears to fold together interfaces on the same device

                uint16_t bufferSize = UAC_DEFAULT_ASIO_BUFFER_SIZE;
                if (!SUCCEEDED(FilterGetAsioBufferSize(filterHandle.get(), &bufferSize)))
                {
                    bufferSize = UAC_DEFAULT_ASIO_BUFFER_SIZE;
                }

                if (audioProp.OutputAsioChannels != 0 || audioProp.InputAsioChannels != 0)
                {
                    auto propInfo = std::make_shared<DevicePropInfo>();

                    propInfo->Index = validDeviceCount;
                    propInfo->DeviceHandle = std::move(filterHandle);
                    propInfo->AudioProp = audioProp;
//                  propInfo->Name = std::wstring{ ksFilterDevice.Name() };
                    propInfo->Name = audioProp.ProductName;
                    propInfo->DevicePath = ksFilterDevice.Id();
                    propInfo->BufferSize = bufferSize;

                    //auto dev = std::make_shared<UAC_AUDIO_PROPERTY>();

                    results.push_back(std::move(propInfo));
                    //results.push_back(ksFilterDevice.Name());

                    ++validDeviceCount;
                }
            }
        }

        //OutputDebugString(L"** Returning enumeration results. Thread id:");
        //OutputDebugString(std::to_wstring(GetCurrentThreadId()).c_str());
        //OutputDebugString(L"\n");
        OutputDebugString((L"** Returning enumeration results. Thread id:" + std::to_wstring(GetCurrentThreadId()) + L"\n").c_str());

        return results;
    });

}