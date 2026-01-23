// Copyright (c) Yamaha Corporation
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Microsoft Low-Latency Audio driver project.
// Further information: https://aka.ms/asio
// ============================================================================
// ASIO is a registered trademark of Steinberg Media Technologies GmbH.

#pragma once

#include "MainWindow.g.h"

namespace winrt::USBAsioControlPanel::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();


    private:
        Frame m_appWindowFrame{};

     //   void OnWindowClosed(AppWindow const & sender, IInspectable const &);
      public:
        void NavigationView_ItemInvoked(_In_ xaml::Controls::NavigationView const& sender, _In_ xaml::Controls::NavigationViewItemInvokedEventArgs const & args);

        void NavigationView_Loaded(_In_ foundation::IInspectable const& sender, _In_ xaml::RoutedEventArgs const& e);
        foundation::IAsyncAction OKButton_Click(_In_ foundation::IInspectable const & sender, _In_ xaml::RoutedEventArgs const & e);
        void CancelButton_Click(_In_ foundation::IInspectable const& sender, _In_ xaml::RoutedEventArgs const& e);
        void OnClosed(_In_ foundation::IInspectable const & sender, _In_ xaml::WindowEventArgs const & Args);
    };
}

namespace winrt::USBAsioControlPanel::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
