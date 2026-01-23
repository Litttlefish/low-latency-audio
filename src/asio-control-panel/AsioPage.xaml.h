// Copyright (c) Yamaha Corporation
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Microsoft Low-Latency Audio driver project.
// Further information: https://aka.ms/asio
// ============================================================================
// ASIO is a registered trademark of Steinberg Media Technologies GmbH.

#pragma once

#include "AsioPage.g.h"

namespace winrt::USBAsioControlPanel::implementation
{
    struct AsioPage : AsioPageT<AsioPage>
    {
        AsioPage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent

            this->Loaded({this, &AsioPage::Page_Loaded});
        }


        winrt::USBAsioControlPanel::AsioViewModel ViewModel() { return m_viewModel; }

        foundation::IAsyncAction Page_Loaded(_In_ foundation::IInspectable const& sender, _In_ xaml::RoutedEventArgs const& e);

        foundation::IAsyncAction OnDeviceChangedAsyncAsioViewModel();
        foundation::IAsyncAction OkButtonClickedAsyncAsioViewModel();

        void DisposeAsioViewModel();
    
    private:
        winrt::USBAsioControlPanel::AsioViewModel m_viewModel;
        bool m_initialized = false;


    };
}

namespace winrt::USBAsioControlPanel::factory_implementation
{
    struct AsioPage : AsioPageT<AsioPage, implementation::AsioPage>
    {
    };
}
