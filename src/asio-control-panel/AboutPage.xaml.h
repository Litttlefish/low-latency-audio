// Copyright (c) Yamaha Corporation
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Microsoft Low-Latency Audio driver project.
// Further information: https://aka.ms/asio
// ============================================================================
// ASIO is a registered trademark of Steinberg Media Technologies GmbH.

#pragma once

#include "AboutPage.g.h"

namespace winrt::USBAsioControlPanel::implementation
{
    struct AboutPage : AboutPageT<AboutPage>
    {
        AboutPage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

    private:

    };
}

namespace winrt::USBAsioControlPanel::factory_implementation
{
    struct AboutPage : AboutPageT<AboutPage, implementation::AboutPage>
    {
    };
}
