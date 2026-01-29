// Copyright (c) Yamaha Corporation
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Microsoft Low-Latency Audio driver project.
// Further information: https://aka.ms/asio
// ============================================================================
// ASIO is a registered trademark of Steinberg Media Technologies GmbH.

#pragma once

// if this is going to be used in Xaml binding, it needs to be turned into a WinRT type
// by defining it in IDL. It's often easier to have a type that is specific to binding
// that points to a type with non-WinRT types like handles via parallel maps and a key or index
struct DevicePropInfo
{
  public:
    LONG                Index=-1;
    std::wstring        Name{};
    std::wstring        DevicePath{};
    wil::unique_handle  DeviceHandle{};
    //HANDLE              DeviceHandle=nullptr;
    //ULONG               CurrentSupportedSampleRate=0;
    //ULONG               CurrentSampleRate=0;
    //UACSampleFormat     CurrentSampleFormat= UACSampleFormat::UAC_SAMPLE_FORMAT_PCM;
    UAC_AUDIO_PROPERTY  AudioProp{};

    //bool                IsDevice=false;

    //LONG                AsioIndex=-1;

    uint16_t            BufferSize{};

    //auto operator<=>(const DevicePropInfo &) const = default;
    auto operator == (const DevicePropInfo& other) const
    {
        return std::tie(Index, Name, DevicePath, AudioProp.SampleRate, BufferSize)
            == std::tie(other.Index, other.Name, other.DevicePath, other.AudioProp.SampleRate, other.BufferSize);
    }

private:

};

class DeviceEnumerationService
{
  public:
    // method to enumerate devices
    static concurrency::task<std::vector<std::shared_ptr<DevicePropInfo>>> GetControlledDevicesAsync();

  private:





};