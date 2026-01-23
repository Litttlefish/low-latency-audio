// Copyright (c) Yamaha Corporation
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Microsoft Low-Latency Audio driver project.
// Further information: https://aka.ms/asio
// ============================================================================
// ASIO is a registered trademark of Steinberg Media Technologies GmbH.

#include "pch.h"
#include "AsioViewModel.h"
#include "AsioViewModel.g.cpp"

#include <winrt/Windows.System.h>

namespace winrt::USBAsioControlPanel::implementation
{
    AsioViewModel::AsioViewModel()
    {
        OutputDebugString(L"ViewModel Constructor\n");
    }

    AsioViewModel::~AsioViewModel()
    {
        OutputDebugString(L"Viewmodel Destructor\n");
    }

    _Use_decl_annotations_
    void AsioViewModel::Initialize(winrt::Microsoft::UI::Dispatching::DispatcherQueue const & queue)
    {
        OutputDebugString(L"AsioViewModel::Initialize()\n");

        m_dispatcherQueue = queue;

        auto weakThis = this->get_weak();
        m_timer = m_dispatcherQueue.CreateTimer();
        m_timer.Interval(std::chrono::seconds(1));
        m_tickToken = m_timer.Tick([weakThis](auto const &, auto const &) -> winrt::fire_and_forget {
            //OutputDebugString(L"OnTimer start\n");
            if (auto self = weakThis.get())
            {
                try
                {
                    co_await self->OnTimer();
                }
                catch (winrt::hresult_error const& ex)
                {
                    OutputDebugString((L"OnTimer:WinRT Exception: " + ex.message()).c_str());
                }
                catch (std::exception const& ex)
                {
                    OutputDebugString((L"OnTimer:std::exception: " + winrt::to_hstring(ex.what())).c_str());
                }
                catch (...)
                {
                    OutputDebugString(L"OnTimer:Unknown exception occurred in Tick handler.");
                }
            }
            else
            {
                OutputDebugString(L"OnTimer:weakThis.get() NG\n");
            }
            //OutputDebugString(L"OnTimer:end\n");
        });
        m_timer.Start();

    }

    _Use_decl_annotations_
    void AsioViewModel::RaisePropertyChanged(winrt::hstring const& property)
    {
        xaml::Data::PropertyChangedEventArgs args(property);

        m_propertyChanged(*this, args);
    }

    _Use_decl_annotations_
    winrt::event_token AsioViewModel::PropertyChanged(xaml::Data::PropertyChangedEventHandler const& handler)
    {
        return m_propertyChanged.add(handler);
    }

    _Use_decl_annotations_
    void AsioViewModel::PropertyChanged(winrt::event_token const& token) noexcept
    {
        return m_propertyChanged.remove(token);
    }



    //         
    _Use_decl_annotations_
    int AsioViewModel::getBufferCoefficient(_In_ uint32_t sampleRate)
    {
        int bufferCoefficient = 1;
        if (sampleRate > 50000 && sampleRate < 99999)
        {
            bufferCoefficient = 2;
        }
        if (sampleRate > 100000 && sampleRate < 199999)
        {
            bufferCoefficient = 4;
        }
        if (sampleRate > 200000 && sampleRate < 399999)
        {
            bufferCoefficient = 8;
        }
        if (sampleRate > 400000)
        {
            bufferCoefficient = 16;
        }
        return bufferCoefficient;
    }

    foundation::IAsyncAction AsioViewModel::LoadDevicesAsync()
    {
        try
        {
            winrt::apartment_context ui_thread;

            auto devicePropInfos = co_await DeviceEnumerationService::GetControlledDevicesAsync();

            std::wstring devicePath = L"";

            if(devicePropInfos.size())
            {
                HANDLE handle = devicePropInfos.at(0)->DeviceHandle.get();
                HRESULT result = co_await DeviceSettingService::GetAsioDevicePathAsync(handle, devicePath);
                if (result != S_OK)
                {
                    OutputDebugString(L"LoadDevicesAsync() GetAsioDevicePathAsync() error.\n");
                    devicePath = L"";
                }
            }

            co_await ui_thread;

            RebuildComboboxItems_Devices(devicePropInfos, devicePath);
            RebuildComboboxItems_SampleRate();
            InitComboboxItems_BufferSize();
        }
        catch (winrt::hresult_error const & ex)
        {
            OutputDebugString((L"LoadDevicesAsync() WinRT Exception: " + ex.message()).c_str());
        }
        catch (std::exception const & ex)
        {
            OutputDebugString((L"LoadDevicesAsync() Catching Standard Exceptions" + winrt::to_hstring(ex.what())).c_str());
        }
        catch (...)
        {
            OutputDebugString(L"LoadDevicesAsync() Catching Other Exceptions\n");
        }

        co_return;
    }

    _Use_decl_annotations_
    void AsioViewModel::LoadSampleRates(ULONG supported)
    {
        // 11025,22050,32000,44100,48000,88200,96000,176400,192000,352800,384000,705600,768000

        if (supported & 0x00001000) { m_sampleRates.Append(768000); }
        if (supported & 0x00000800) { m_sampleRates.Append(705600); }
        if (supported & 0x00000400) { m_sampleRates.Append(384000); }
        if (supported & 0x00000200) { m_sampleRates.Append(352800); }
        if (supported & 0x00000100) { m_sampleRates.Append(192000); }
        if (supported & 0x00000080) { m_sampleRates.Append(176400); }
        if (supported & 0x00000040) { m_sampleRates.Append(96000); }
        if (supported & 0x00000020) { m_sampleRates.Append(88200); }
        if (supported & 0x00000010) { m_sampleRates.Append(48000); }
        if (supported & 0x00000008) { m_sampleRates.Append(44100); }
        if (supported & 0x00000004) { m_sampleRates.Append(32000); }
        if (supported & 0x00000002) { m_sampleRates.Append(22050); }
        if (supported & 0x00000001) { m_sampleRates.Append(11025); }
    }

    _Use_decl_annotations_
    void AsioViewModel::LoadBufferSizes(int bufferCoefficient)
    {
        // 2048,1536,1024,768,512,384,256,192,128,96,64,48,32,24,16,12,8,4,0

        m_bufferSizes.Append(static_cast<uint16_t>(2048 * bufferCoefficient));
        m_bufferSizes.Append(static_cast<uint16_t>(1536 * bufferCoefficient));
        m_bufferSizes.Append(static_cast<uint16_t>(1024 * bufferCoefficient));
        m_bufferSizes.Append(static_cast<uint16_t>(768 * bufferCoefficient));
        m_bufferSizes.Append(static_cast<uint16_t>(512 * bufferCoefficient));
        m_bufferSizes.Append(static_cast<uint16_t>(384 * bufferCoefficient));
        m_bufferSizes.Append(static_cast<uint16_t>(256 * bufferCoefficient));
        m_bufferSizes.Append(static_cast<uint16_t>(192 * bufferCoefficient));
        m_bufferSizes.Append(static_cast<uint16_t>(128 * bufferCoefficient));
        m_bufferSizes.Append(static_cast<uint16_t>(96 * bufferCoefficient));
        m_bufferSizes.Append(static_cast<uint16_t>(64 * bufferCoefficient));
        m_bufferSizes.Append(static_cast<uint16_t>(48 * bufferCoefficient));
        m_bufferSizes.Append(static_cast<uint16_t>(32 * bufferCoefficient));
#if _DEBUG
        m_bufferSizes.Append(static_cast<uint16_t>(24 * bufferCoefficient));
        m_bufferSizes.Append(static_cast<uint16_t>(16 * bufferCoefficient));
        m_bufferSizes.Append(static_cast<uint16_t>(12 * bufferCoefficient));
        m_bufferSizes.Append(static_cast<uint16_t>(8 * bufferCoefficient));
        m_bufferSizes.Append(static_cast<uint16_t>(4 * bufferCoefficient));
//      m_bufferSizes.Append(0);
#endif
    }

    _Use_decl_annotations_
    void AsioViewModel::RebuildComboboxItems_Devices(const std::vector<std::shared_ptr<DevicePropInfo>>& devices, std::wstring devicePath)
    {
        OutputDebugString((L"RebuildComboboxItems_Devices() devciePath:" + winrt::to_hstring(devicePath.c_str()) + L"\n").c_str());

        m_devices.Clear();

        int index = 0;
        int selected = 0;
        for (auto const & device : devices)
        {
            winrt::hstring name{device->Name};
            m_devices.Append(name);
            OutputDebugString((L"** m_devices add:" + winrt::to_hstring(name.c_str()) + L"\n").c_str());

            if (!devicePath.empty())
            {
                if (device->DevicePath == devicePath)
                {
                    OutputDebugString(L"RebuildComboboxItems_Devices() Matches the devicepath\n");
                    selected = index;
                }
            }

            index++;
        }

        // temp
        if (m_devices.Size() == 0)
        {
#if _DEBUG
            m_devices.Append(L"Debug: No devices found");
#else
            selected = -1;
#endif
        }
        else
        {
            OutputDebugString((L"RebuildComboboxItems_Devices() selected:" + std::to_wstring(selected) + L"\n").c_str());
            //DeviceSelectedIndex(selected);
        }
        m_devicePropInfos = devices;

        m_deviceSelectedIndex = selected;

        //RaisePropertyChanged(L"Devices");
        RaisePropertyChanged(L"DeviceSelectedIndex");
    }

    void AsioViewModel::RebuildComboboxItems_SampleRate()
    {
        auto devices = m_devicePropInfos;
        auto selected = m_deviceSelectedIndex;

        if (selected >= 0 && selected < devices.size())
        {

            auto supportedSampleRate = devices.at(selected)->AudioProp.SupportedSampleRate;
            m_sampleRates.Clear();
            LoadSampleRates(supportedSampleRate);

            const auto currentSampleRate = devices.at(selected)->AudioProp.SampleRate;
            uint32_t index = 0;
            if (!m_sampleRates.IndexOf(currentSampleRate, index))
            {
                index = m_sampleRates.Size();
            }
            SampleRateSelectedIndex(index);
            RaisePropertyChanged(L"SampleRateSelectedIndex");
        }
        else
        {
            m_sampleRates.Clear();
        }
    }

    void AsioViewModel::InitComboboxItems_BufferSize()
    {
        auto devices = m_devicePropInfos;
        auto selected = m_deviceSelectedIndex;

        if (selected >= 0 && selected < devices.size())
        {
            auto currentSampleRate = m_sampleRates.GetAt(SampleRateSelectedIndex());
            auto bufferCoefficient = getBufferCoefficient(currentSampleRate);
            auto index = BufferSizeSelectedIndex();

            m_bufferSizes.Clear();
            LoadBufferSizes(bufferCoefficient);

            uint16_t value = static_cast<uint16_t>(devices.at(selected)->BufferSize * bufferCoefficient);
            uint32_t defaultIndex = 0;
            if (m_bufferSizes.IndexOf(value, defaultIndex))
            {
                index = defaultIndex;
            }
            BufferSizeSelectedIndex(index);
            RaisePropertyChanged(L"BufferSizeSelectedIndex");
        }
        else
        {
            m_bufferSizes.Clear();
        }
    }

    void AsioViewModel::RebuildComboboxItems_BufferSize()
    {
        auto devices = m_devicePropInfos;
        auto selected = m_deviceSelectedIndex;

        if (selected >= 0 && selected < devices.size())
        {
            auto currentSampleRate = m_sampleRates.GetAt(SampleRateSelectedIndex());
            auto bufferCoefficient = getBufferCoefficient(currentSampleRate);
            auto index = BufferSizeSelectedIndex();

            m_bufferSizes.Clear();

            LoadBufferSizes(bufferCoefficient);
            BufferSizeSelectedIndex(index);
            RaisePropertyChanged(L"BufferSizeSelectedIndex");
        }
        else
        {
            m_bufferSizes.Clear();
        }
    }

    foundation::IAsyncAction AsioViewModel::OnDeviceChangedAsync()
    {
        OutputDebugString(L"WM_DEVICECHANGE detected\n");

        auto weakThis = this->get_weak();
        if (auto self = weakThis.get())
        {
            auto strong = self;

            //msg = L"OnDeviceChangedAsync() m_devicePropInfos size():" + std::to_wstring(strong->m_devicePropInfos.size()) + L"\n";
            //OutputDebugString(msg.c_str());

            auto displayedDevices = strong->m_devicePropInfos;
            auto displayedIndex = strong->m_deviceSelectedIndex;

            OutputDebugString((L"OnDeviceChangedAsync() displayedDevices size():" + std::to_wstring(displayedDevices.size()) + L"\n").c_str());
            OutputDebugString((L"OnDeviceChangedAsync() deviceSelectedIndex:" + std::to_wstring(displayedIndex) + L"\n").c_str());

            auto currentDevices = co_await DeviceEnumerationService::GetControlledDevicesAsync();

            if (!std::equal(displayedDevices.begin(), displayedDevices.end(), currentDevices.begin(), currentDevices.end()))
            {
                if (strong->m_dispatcherQueue)
                {
                    std::wstring devicePath = L"";
                    if (displayedIndex >= 0 && displayedIndex < displayedDevices.size())
                    {
                        devicePath = displayedDevices.at(displayedIndex)->DevicePath;
                        OutputDebugString((L"OnDeviceChangedAsync() devciePath:" + winrt::to_hstring(devicePath.c_str()) + L"\n").c_str());

                        strong->m_dispatcherQueue.TryEnqueue([currentDevices, devicePath, strong] {
                            OutputDebugString(L"DeviceChangedMessage Recived Devices combobox update\n");
                            if (!strong->m_isDisposed)
                            {
                                strong->RebuildComboboxItems_Devices(currentDevices, devicePath);
                                strong->RebuildComboboxItems_SampleRate();
                                strong->RebuildComboboxItems_BufferSize();
                            }
                        });
                    }
                    else
                    {
                        OutputDebugString(L"OnDeviceChangedAsync() No device selected\n");

                        if (currentDevices.size())
                        {
                            HANDLE handle = currentDevices.at(0)->DeviceHandle.get();
                            HRESULT result = S_OK;
                            result = co_await DeviceSettingService::GetAsioDevicePathAsync(handle, devicePath);
                            if (result != S_OK)
                            {
                                devicePath = L"";
                            }
                        }

                        strong->m_dispatcherQueue.TryEnqueue([currentDevices, devicePath, strong] {
                            OutputDebugString(L"DeviceChangedMessage Recived Devices combobox update\n");
                            if (!strong->m_isDisposed)
                            {
                                strong->RebuildComboboxItems_Devices(currentDevices, devicePath);
                                strong->RebuildComboboxItems_SampleRate();
                                strong->InitComboboxItems_BufferSize();
                            }
                        });
                    }
                }
            }
            else
            {
                OutputDebugString(L"*** No change in device information ***\n");
            }
        }
        co_return;
    }

    foundation::IAsyncAction AsioViewModel::OkButtonClickedAsync()
    {
        OutputDebugString(L"OK Button Click detected\n");

        auto weakThis = this->get_weak();
        if (auto self = weakThis.get())
        {
            auto strong = self;

            auto devices = strong->m_devicePropInfos;
            auto selectedIndex = strong->m_deviceSelectedIndex;

            if (selectedIndex >= 0 && selectedIndex < devices.size())
            {
                HANDLE handle = devices.at(selectedIndex)->DeviceHandle.get();

                auto devicePath = devices.at(selectedIndex)->DevicePath;
                OutputDebugString((L"OkButtonClickedAsync() devciePath:" + winrt::to_hstring(devicePath.c_str()) + L"\n").c_str());

                auto sampleRate = strong->m_sampleRates.GetAt(strong->SampleRateSelectedIndex());

                auto bufferSize = strong->m_bufferSizes.GetAt(strong->BufferSizeSelectedIndex());
                auto bufferCoefficient = strong->getBufferCoefficient(sampleRate);
                //DriverSettings::Save(bufferSize / bufferCoefficient, devicePath);
                co_await DeviceSettingService::SetAsioDevicePathAsync(handle, devicePath);
                co_await DeviceSettingService::SetDevicesSampleRateAsync(handle, sampleRate);
                co_await DeviceSettingService::SetAsioBufferSizeAsync(handle, static_cast<uint16_t>(bufferSize / bufferCoefficient));

                int32_t index{ 0 };

                for (auto const& device : devices)
                {
                    if (selectedIndex != index)
                    {
                        OutputDebugString((L"OkButtonClickedAsync() others devciePath:" + winrt::to_hstring(device->DevicePath.c_str()) + L"\n").c_str());
                        handle = device->DeviceHandle.get();
                        sampleRate = device->AudioProp.SampleRate;
                        bufferSize = device->BufferSize;
                        co_await DeviceSettingService::SetDevicesSampleRateAsync(handle, sampleRate);
                        co_await DeviceSettingService::SetAsioBufferSizeAsync(handle, bufferSize);
                    }
                    ++index;
                }
            }
        }
        co_return;
    }

    foundation::IAsyncAction AsioViewModel::OnTimer()
    {
        //OutputDebugString(L"OnTimer Start\n");

        auto weakThis = this->get_weak();
        if (auto self = weakThis.get())
        {
            auto strong = self;

            auto previousDevices = strong->m_devicePropInfos;
            auto previousIndex = strong->m_deviceSelectedIndex;

            ULONG previousSampleRate = 0;
            std::wstring devicePath = L"";
            if (previousIndex >= 0 && previousIndex < previousDevices.size())
            {
                previousSampleRate = previousDevices.at(previousIndex)->AudioProp.SampleRate;
                //OutputDebugString((L"OnTimer() previousSampleRate:" + std::to_wstring(previousSampleRate) + L"\n").c_str());

                devicePath = previousDevices.at(previousIndex)->DevicePath;

                HANDLE handle = previousDevices.at(previousIndex)->DeviceHandle.get();

                uint32_t currentSampleRate = 0;
                HRESULT result = co_await DeviceSettingService::GetDevicesSampleRateAsync(handle, &currentSampleRate);
                //OutputDebugString((L"OnTimer() currentSampleRate:" + std::to_wstring(currentSampleRate) + L"\n").c_str());

                if(result == S_OK && previousSampleRate != currentSampleRate)
                {
                    OutputDebugString(L"OnTimer Display update\n");

                    auto currentDevices = co_await DeviceEnumerationService::GetControlledDevicesAsync();
 
                    if (strong->m_dispatcherQueue)
                    {
                        strong->m_dispatcherQueue.TryEnqueue([currentDevices, devicePath, weakThis] {
                            //OutputDebugString(L"OnTimer update start\n");
                            if (auto self = weakThis.get())
                            {
                                if (!self->m_isDisposed)
                                {
                                    self->RebuildComboboxItems_Devices(currentDevices, devicePath);
                                    self->RebuildComboboxItems_SampleRate();
                                    self->RebuildComboboxItems_BufferSize();
                                }
                            }
                            //OutputDebugString(L"OnTimer update end\n");
                        });
                    }
                }
            }
            else
            {
                // OutputDebugString(L"OnTimer() previousIndex error\n");
            }
        }

        //OutputDebugString(L"OnTimer End\n");

        co_return;
    }

    void AsioViewModel::Dispose()
    {
        OutputDebugString(L"AsioViewModel::Dispose()\n");

        m_isDisposed = true;

        if (m_timer)
        {
            OutputDebugString(L"Polling timer stop\n");
            m_timer.Stop();
            m_timer.Tick(m_tickToken);
            m_timer = nullptr;
        }
    }



} // namespace winrt::USBAsioControlPanel::implementation
