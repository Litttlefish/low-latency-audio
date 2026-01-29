// Copyright (c) Yamaha Corporation
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Microsoft Low-Latency Audio driver project.
// Further information: https://aka.ms/asio
// ============================================================================

#pragma once

#ifndef STRING_UTILITY_H
#define STRING_UTILITY_H

#include <format>

namespace Utility
{
	inline winrt::hstring GuidToHStringWithBraces(winrt::guid sourceGuid)
	{
		auto formatted = std::format(
            L"{{{:08x}-{:04x}-{:04x}-{:02x}{:02x}-{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}}}",
			sourceGuid.Data1,
			sourceGuid.Data2,
			sourceGuid.Data3,
			sourceGuid.Data4[0],
			sourceGuid.Data4[1],
			sourceGuid.Data4[2],
			sourceGuid.Data4[3],
			sourceGuid.Data4[4],
			sourceGuid.Data4[5],
			sourceGuid.Data4[6],
			sourceGuid.Data4[7]
			);

		return winrt::hstring{ formatted };
	}

};

#endif