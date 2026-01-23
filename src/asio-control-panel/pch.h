// Copyright (c) Yamaha Corporation
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Microsoft Low-Latency Audio driver project.
// Further information: https://aka.ms/asio
// ============================================================================
// ASIO is a registered trademark of Steinberg Media Technologies GmbH.

#pragma once

#include <windows.h>
#include <unknwn.h>
#include <restrictederrorinfo.h>
#include <hstring.h>

// Undefine GetCurrentTime macro to prevent
// conflict with Storyboard::GetCurrentTime
#undef GetCurrentTime

// Base Windows SDK
#include <pplawait.h>

#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Graphics.h>
#include <winrt/Windows.UI.Xaml.Interop.h> // for frame navigation / typename

using namespace winrt::Windows::Devices::Enumeration;
namespace foundation = winrt::Windows::Foundation;
namespace collections = winrt::Windows::Foundation::Collections;
namespace graphics = winrt::Windows::Graphics;

// WinUI 3 / Windows App SDK

#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Microsoft.UI.Xaml.Interop.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Navigation.h>
#include <winrt/Microsoft.UI.Xaml.Shapes.h>

using namespace winrt::Microsoft::UI::Windowing;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Navigation;

namespace xaml = winrt::Microsoft::UI::Xaml;



// WIL

#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result_macros.h>
#include <wil/cppwinrt_helpers.h>
#include <wil/registry.h>
#include <wil/registry_helpers.h>

// KS and USB

#include <devioctl.h>
#include <ks.h>
#include <ksmedia.h>
#include <avrt.h>

// shared with driver code

#include "UAC_User.h" // definitions and settings shared with the other projects

// Local KS code adapted from the MIDI 2.0 project. This needs access to values in UAC_User.h

#include "KsCommon.h"

// This app and driver

#include "string_utility.h"


#include "DriverSettings.h"
#include "AsioViewModel.h"
#include "DeviceEnumerationService.h"
#include "DeviceSettingService.h"
// #include "Messenger.h"

#include "AsioPage.xaml.h"
#include "AboutPage.xaml.h"
#include "MainWindow.xaml.h"

