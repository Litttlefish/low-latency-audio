// Copyright (c) Yamaha Corporation
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Microsoft Low-Latency Audio driver project.
// Further information: https://aka.ms/asio
// ============================================================================
// ASIO is a registered trademark of Steinberg Media Technologies GmbH.

#include "pch.h"
#include "AsioPage.xaml.h"
#if __has_include("AsioPage.g.cpp")
#include "AsioPage.g.cpp"
#endif


// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::USBAsioControlPanel::implementation
{
	_Use_decl_annotations_
	foundation::IAsyncAction AsioPage::Page_Loaded(foundation::IInspectable const& sender, xaml::RoutedEventArgs const& e)
	{
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
        if (!m_initialized)
        {
            OutputDebugString(L"Page ViewModel Initialize\n");

            m_initialized = true;

            auto queue = this->DispatcherQueue();
            ViewModel().Initialize(queue);

            // make sure this class is still around when we return
            auto strong_this{get_strong()};

            co_await ViewModel().LoadDevicesAsync();
        }

		co_return;
	}

    foundation::IAsyncAction AsioPage::OnDeviceChangedAsyncAsioViewModel()
    {
        if (ViewModel())
        {
            co_await ViewModel().OnDeviceChangedAsync();
        }
    }

    foundation::IAsyncAction AsioPage::OkButtonClickedAsyncAsioViewModel()
    {
        if (ViewModel())
        {
            co_await ViewModel().OkButtonClickedAsync();
        }
    }

    void AsioPage::DisposeAsioViewModel()
    {
        if (ViewModel())
        {
            ViewModel().Dispose();
        }
    }

}

