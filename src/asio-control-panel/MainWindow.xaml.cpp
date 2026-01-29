// Copyright (c) Yamaha Corporation
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Microsoft Low-Latency Audio driver project.
// Further information: https://aka.ms/asio
// ============================================================================
// ASIO is a registered trademark of Steinberg Media Technologies GmbH.

#include "pch.h"

#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include <microsoft.ui.xaml.window.h>
#include <winrt/Windows.System.h>
#include <CommCtrl.h>
#include <dbt.h>

#include "winrt/Microsoft.UI.Xaml.Controls.h"
#include "winrt/Windows.ApplicationModel.Resources.h"

using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Windows::ApplicationModel::Resources;


winrt::fire_and_forget deviceChangeAsync(_In_ DWORD_PTR dwRefData)
{
    OutputDebugString(L"MainWindow deviceChangeAsync()\n");

    try
    {
        auto window = reinterpret_cast<winrt::USBAsioControlPanel::implementation::MainWindow*>(dwRefData);

        if (window->MainContentFrame())
        {
            auto asioPage = window->MainContentFrame().Content().try_as<winrt::USBAsioControlPanel::AsioPage>();
            if (asioPage)
            {
                co_await asioPage.OnDeviceChangedAsyncAsioViewModel();
            }
        }
    }
    catch (winrt::hresult_error const & ex)
    {
        OutputDebugString((L"deviceChangeAsync() WinRT Exception: " + ex.message()).c_str());
    }
    catch (std::exception const & ex)
    {
        OutputDebugString((L"deviceChangeAsync() Catching Standard Exceptions"  + winrt::to_hstring(ex.what())).c_str());
    }
    catch (...)
    {
        OutputDebugString(L"deviceChangeAsync() Catching Other Exceptions\n");
    }
}

LRESULT CALLBACK deviceChangeProc(
    _In_ HWND hwnd,
    _In_ UINT uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam,
    _In_ UINT_PTR uIdSubclass,
    _In_ DWORD_PTR dwRefData)
{
    UNREFERENCED_PARAMETER(uIdSubclass);

    if (uMsg == WM_DEVICECHANGE)
    {
        deviceChangeAsync(dwRefData);
    }

    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}


// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::USBAsioControlPanel::implementation
{
    MainWindow::MainWindow()
    {
        // Xaml objects should not call InitializeComponent during construction.
        // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent

        const int width{ 480 };
        const int height{ 400 };

        auto presenter = OverlappedPresenter::CreateForDialog();

        presenter.PreferredMinimumWidth(width);
        presenter.PreferredMinimumHeight(height);
        presenter.PreferredMaximumWidth(width);
        presenter.PreferredMaximumHeight(height);

        presenter.IsMaximizable(false);
        presenter.IsResizable(false);

        auto appWindow = this->AppWindow();

        this->ExtendsContentIntoTitleBar(true);


        auto displayArea = DisplayArea::GetFromWindowId(appWindow.Id(), DisplayAreaFallback::Nearest);

        if (displayArea != nullptr)
        {
            // center on the screen
            graphics::RectInt32 newLocationAndSize;

            newLocationAndSize.X = (displayArea.WorkArea().Width - width) / 2;
            newLocationAndSize.Y = (displayArea.WorkArea().Height - height) / 2;
            newLocationAndSize.Width = width;
            newLocationAndSize.Height = height;
                
            appWindow.MoveAndResize(newLocationAndSize);
        }
        else
        {
            // just resize
            winrt::Windows::Graphics::SizeInt32 newSize;
            newSize.Height = height;
            newSize.Width = width;

            appWindow.Resize(newSize);
        }

        appWindow.SetPresenter(presenter);

        this->Title(L"Configure Windows ASIO Driver");

        this->Closed({this, &MainWindow::OnClosed});

    }

    _Use_decl_annotations_
    void MainWindow::NavigationView_ItemInvoked(
        xaml::Controls::NavigationView const & sender, 
        xaml::Controls::NavigationViewItemInvokedEventArgs const & args)
    {
        foundation::IInspectable tag = args.InvokedItemContainer().Tag();
        winrt::hstring tagString = unbox_value<winrt::hstring>(tag);

        xaml::Navigation::FrameNavigationOptions options;

        if (sender.PaneDisplayMode() == xaml::Controls::NavigationViewPaneDisplayMode::Top)
        {
            options.IsNavigationStackEnabled(false);
        }

        if (tagString == L"ASIO")
        {
            MainContentFrame().NavigateToType(
                xaml_typename<winrt::USBAsioControlPanel::AsioPage>(),
                nullptr,
                nullptr
            );
        }
        else if (tagString == L"About")
        {
            MainContentFrame().NavigateToType(               
                xaml_typename<winrt::USBAsioControlPanel::AboutPage>(), 
                nullptr,                         
                nullptr
            );
        }
    }

    _Use_decl_annotations_
    void MainWindow::NavigationView_Loaded(
        foundation::IInspectable const& sender, 
        xaml::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

   //     MainContentFrame().Navigate(xaml_typename<winrt::USBAsioControlPanel::AsioPage>(), *this);

        auto windowNative = this->m_inner.try_as<::IWindowNative>();
        if (windowNative)
        {
            HWND hwnd = nullptr;
            windowNative->get_WindowHandle(&hwnd);
            if (hwnd)
            {
                SetWindowSubclass(hwnd, deviceChangeProc, 1, reinterpret_cast<DWORD_PTR>(this));
            }
        }
    }

    _Use_decl_annotations_
    foundation::IAsyncAction MainWindow::OKButton_Click(
        foundation::IInspectable const & sender, 
        xaml::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        auto strong_this{get_strong()};

        try
        {
            winrt::apartment_context ui_thread;
            if (MainContentFrame())
            {
                auto asioPage = MainContentFrame().Content().try_as<winrt::USBAsioControlPanel::AsioPage>();
                if (asioPage)
                {
                    co_await asioPage.OkButtonClickedAsyncAsioViewModel();
                }
            }
            co_await ui_thread;
            strong_this->Close();
        }
        catch (winrt::hresult_error const & ex)
        {
            OutputDebugString((L"OKButton_Click() WinRT Exception: " + ex.message()).c_str());
        }
        catch (std::exception const & ex)
        {
            OutputDebugString((L"OKButton_Click() Catching Standard Exceptions" + winrt::to_hstring(ex.what())).c_str());
        }
        catch (...)
        {
            OutputDebugString(L"OKButton_Click() Catching Other Exceptions\n");
        }
    }

    _Use_decl_annotations_
    void MainWindow::CancelButton_Click(
        foundation::IInspectable const& sender, 
        xaml::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->Close();
    }

    _Use_decl_annotations_
    void MainWindow::OnClosed(
        foundation::IInspectable const & sender,
        xaml::WindowEventArgs const & args)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);
        
        OutputDebugString(L"MainWindow::OnClosed()\n");

        if (MainContentFrame())
        {
            auto asioPage = MainContentFrame().Content().try_as<winrt::USBAsioControlPanel::AsioPage>();
            if (asioPage)
            {
                OutputDebugString(L"MainWindow::OnClosed() call DisposeAsioViewModel()\n");
                asioPage.DisposeAsioViewModel();
            }
        }

        auto windowNative = this->m_inner.try_as<::IWindowNative>();
        if (windowNative)
        {
            HWND hwnd = nullptr;
            windowNative->get_WindowHandle(&hwnd);
            if (hwnd)
            {
                OutputDebugString(L"MainWindow::OnClosed() call RemoveWindowSubclass()\n");
                RemoveWindowSubclass(hwnd, deviceChangeProc, 1);
            }
        }

        MainNavView().SelectedItem(nullptr);
        MainNavView().Content(nullptr);
        MainNavView().MenuItems().Clear();
        MainNavView().DataContext(nullptr);

        OutputDebugString(L"MainWindow::OnClosed() End.\n");
    }

} // namespace winrt::USBAsioControlPanel::implementation


