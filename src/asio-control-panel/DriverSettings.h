// Copyright (c) Yamaha Corporation
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Microsoft Low-Latency Audio driver project.
// Further information: https://aka.ms/asio
// ============================================================================

#pragma once


class DriverSettings
{
public:


	UAC_SET_FLAGS_CONTEXT Flags;





    static HRESULT Load(_Inout_ int& BufferSize, _Inout_ std::wstring& devicePath);
    static HRESULT Save(_In_ int BufferSize, _In_ std::wstring devicePath);

private:

};