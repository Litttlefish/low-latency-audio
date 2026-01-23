// Copyright (c) Yamaha Corporation
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Microsoft Low-Latency Audio driver project.
// Further information: https://aka.ms/asio
// ============================================================================
// ASIO is a registered trademark of Steinberg Media Technologies GmbH.

#include "pch.h"


_Use_decl_annotations_
HRESULT DriverSettings::Load(int & bufferSize, std::wstring &devicePath)
{
    HKEY    hKey;
    LONG    result = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows USB ASIO", 0, KEY_READ, &hKey);

    if (result == ERROR_SUCCESS)
    {
        {
            DWORD value = 0;
            DWORD value_length = sizeof(value);
            result = RegQueryValueExW(hKey, L"PeriodFrames", nullptr, nullptr, reinterpret_cast<LPBYTE>(&value), &value_length);
            if (result == ERROR_SUCCESS)
            {
                bufferSize = value;
            }
        }

        {
            DWORD value_length = 0;
            result = RegQueryValueExW(hKey, L"AsioDevice", nullptr, nullptr, nullptr, &value_length);
            if (result == ERROR_SUCCESS)
            {
                std::wstring value(value_length / sizeof(wchar_t), L'\0');
                result = RegQueryValueExW(hKey, L"AsioDevice", nullptr, nullptr, reinterpret_cast<LPBYTE>(&value[0]), &value_length);
                if (result == ERROR_SUCCESS)
                {
                    size_t nullPos = value.find(L'\0');
                    if (nullPos != std::wstring::npos)
                    {
                        value.resize(nullPos);
                    }
                    devicePath = value;
                }
            }
        }

        RegCloseKey(hKey);
    }


	return E_FAIL;
}

_Use_decl_annotations_
HRESULT DriverSettings::Save(int bufferSize, std::wstring devicePath)
{
    HKEY hKey;
    LONG result = RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows USB ASIO", 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr);

    if (result == ERROR_SUCCESS)
    {
        auto strings = reinterpret_cast<const BYTE *>(devicePath.c_str());
        size_t size = devicePath.size() * sizeof(wchar_t);
        RegSetValueExW(hKey, L"AsioDevice", 0, REG_SZ, strings, static_cast<DWORD>(size));

        DWORD value = 0;
        RegSetValueExW(hKey, L"BufferFlagsSetting", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&value), sizeof(value));
        value = bufferSize;
        RegSetValueExW(hKey, L"PeriodFrames", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&value), sizeof(value));
        value = 0xffffffff;
        RegSetValueExW(hKey, L"BasePeriod", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&value), sizeof(value));
        value = 0xffffffff;
        RegSetValueExW(hKey, L"IsoOffset", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&value), sizeof(value));
        value = 0xffffffff;
        RegSetValueExW(hKey, L"BulkBuffer", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&value), sizeof(value));

        value = 0;
        RegSetValueExW(hKey, L"BufferFlags", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&value), sizeof(value));
        value = 0;
        RegSetValueExW(hKey, L"ClassicFramesPerIrp", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&value), sizeof(value));
        value = 0;
        RegSetValueExW(hKey, L"ClassicFramesPerIrp2", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&value), sizeof(value));
        value = 0;
        RegSetValueExW(hKey, L"OutBufferOperationOffset", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&value), sizeof(value));
        value = 0;
        RegSetValueExW(hKey, L"InBufferOperationOffset", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&value), sizeof(value));
        value = 0;
        RegSetValueExW(hKey, L"BulkPacketsPerIrp", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&value), sizeof(value));
        value = 0;
        RegSetValueExW(hKey, L"OutBulkOperationOffset", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&value), sizeof(value));
        value = 2;
        RegSetValueExW(hKey, L"LatencyMode", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&value), sizeof(value));
        value = 1;
        RegSetValueExW(hKey, L"DropoutDetection", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&value), sizeof(value));

        RegCloseKey(hKey);
    }



	return E_FAIL;
}