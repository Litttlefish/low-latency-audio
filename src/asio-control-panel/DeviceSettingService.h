// Copyright (c) Yamaha Corporation
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Microsoft Low-Latency Audio driver project.
// Further information: https://aka.ms/asio
// ============================================================================
// ASIO is a registered trademark of Steinberg Media Technologies GmbH.

#pragma once

class DeviceSettingService
{
  public:

      static concurrency::task<HRESULT> SetDevicesSampleRateAsync(_In_ HANDLE handle, _In_ uint32_t sampleRate);
      static concurrency::task<HRESULT> GetDevicesSampleRateAsync(_In_ HANDLE handle, _Inout_ uint32_t *sampleRate);

      static concurrency::task<HRESULT> SetAsioBufferSizeAsync(_In_ HANDLE handle, _In_ uint16_t bufferSize);
      static concurrency::task<HRESULT> GetAsioBufferSizeAsync(_In_ HANDLE handle, _Inout_ uint16_t *bufferSize);

      static concurrency::task<HRESULT> SetAsioDevicePathAsync(_In_ HANDLE handle, _In_ std::wstring &devicePath);
      static concurrency::task<HRESULT> GetAsioDevicePathAsync(_In_ HANDLE handle, _Inout_ std::wstring &devicePath);

private:





};
