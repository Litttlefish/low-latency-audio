// Copyright (c) Yamaha Corporation
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Microsoft Low-Latency Audio driver project.
// Further information: https://aka.ms/asio
// ============================================================================
// ASIO is a registered trademark of Steinberg Media Technologies GmbH.

#pragma once
#include "AsioViewModel.g.h"

#include "DeviceEnumerationService.h"

namespace winrt::USBAsioControlPanel::implementation
{
    struct AsioViewModel : AsioViewModelT<AsioViewModel>
    {
        AsioViewModel();
        ~AsioViewModel();
        void Initialize(_In_ winrt::Microsoft::UI::Dispatching::DispatcherQueue const & queue);

        collections::IObservableVector<winrt::hstring> Devices() { return m_devices; }

        collections::IObservableVector<uint16_t> BufferSizes() { return m_bufferSizes; }
        collections::IObservableVector<uint32_t> SampleRates() { return m_sampleRates; }
        collections::IObservableVector<uint16_t> SampleSizes() { return m_sampleSizes; }


        int32_t DeviceSelectedIndex(){ return m_deviceSelectedIndex; }
        void DeviceSelectedIndex(_In_ int32_t index)
        {
            auto devices = m_devicePropInfos;
            auto selected = m_deviceSelectedIndex;
            if (selected != index)
            {
                if (index >= 0 && index < devices.size())
                {
                    m_deviceSelectedIndex = index;

                    RebuildComboboxItems_SampleRate();
                    InitComboboxItems_BufferSize();
                }
            }
        }

        int32_t SampleRateSelectedIndex(){ return m_sampleRateSelectedIndex; }
        void SampleRateSelectedIndex(_In_ int32_t index)
        {
            if (m_sampleRateSelectedIndex != index)
            {
                m_sampleRateSelectedIndex = index;
                if (index >= 0)
                {
                    RebuildComboboxItems_BufferSize();
                }
            }
        }

        int32_t BufferSizeSelectedIndex(){ return m_bufferSizeSelectedIndex; }
        void BufferSizeSelectedIndex(_In_ int32_t index){ m_bufferSizeSelectedIndex = index; }

        winrt::event_token PropertyChanged(_In_ xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(_In_ winrt::event_token const& token) noexcept;

        foundation::IAsyncAction LoadDevicesAsync();

        foundation::IAsyncAction OnDeviceChangedAsync();
        foundation::IAsyncAction OkButtonClickedAsync();
        void Dispose();

    private:
        winrt::Microsoft::UI::Dispatching::DispatcherQueue m_dispatcherQueue{ nullptr };
        winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer m_timer{nullptr};
        winrt::event_token m_tickToken{};



        winrt::event<xaml::Data::PropertyChangedEventHandler> m_propertyChanged{};

        collections::IObservableVector<winrt::hstring> m_devices{winrt::multi_threaded_observable_vector<winrt::hstring>()};

        collections::IObservableVector<uint16_t> m_bufferSizes { winrt::multi_threaded_observable_vector<uint16_t>() };
        collections::IObservableVector<uint16_t> m_sampleSizes { winrt::multi_threaded_observable_vector<uint16_t>() };
        collections::IObservableVector<uint32_t> m_sampleRates { winrt::multi_threaded_observable_vector<uint32_t>() };

        int32_t m_deviceSelectedIndex = -1;
        int32_t m_sampleRateSelectedIndex = -1;
        int32_t m_bufferSizeSelectedIndex = -1;

        std::vector<std::shared_ptr<DevicePropInfo>> m_devicePropInfos{};

        void LoadSampleRates(_In_ ULONG supported = 0xffffffff);
        void LoadBufferSizes(_In_ int bufferCoefficient = 1);

        void RaisePropertyChanged(_In_ winrt::hstring const& property);

        int getBufferCoefficient(_In_ uint32_t sampleRate);

        void RebuildComboboxItems_Devices(_In_ const std::vector<std::shared_ptr<DevicePropInfo>>& devices, _In_ std::wstring devicePath);
        void RebuildComboboxItems_SampleRate();
        void RebuildComboboxItems_BufferSize();
        void InitComboboxItems_BufferSize();

        foundation::IAsyncAction OnTimer();

        bool m_isDisposed = false;

    };
}

namespace winrt::USBAsioControlPanel::factory_implementation
{
    struct AsioViewModel : AsioViewModelT<AsioViewModel, implementation::AsioViewModel>
    {
    };
}
