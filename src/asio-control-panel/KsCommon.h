// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Microsoft Low-Latency Audio driver project.
// Further information: https://aka.ms/asio
// ============================================================================
// This file was adapted from the Windows MIDI Services Project
// Further information https://aka.ms/midi

#pragma once

HRESULT
SyncIoctl(
    _In_ HANDLE     hHandle,
    _In_ ULONG      ulIoctl,
    _In_ PVOID      pvInBuffer,
    _In_ ULONG      cbInBuffer,
    _In_opt_ PVOID      pvOutBuffer,
    _In_ ULONG      cbOutBuffer,
    _In_opt_ PULONG pulBytesReturned
);

HRESULT
PinPropertySimple(
    _In_ HANDLE  filter,
    _In_ ULONG   pinId,
    _In_ REFGUID guidPropertySet,
    _In_ ULONG   property,
    _In_ PVOID   value,
    _In_ ULONG   valueSize
);

HRESULT
PinPropertyAllocate(
    _In_ HANDLE       filter,
    _In_ ULONG        pinId,
    _In_ REFGUID      guidPropertySet,
    _In_ ULONG        property,
    _Out_ PVOID *     value,
    _Out_opt_ ULONG * valueSize
);

HRESULT
FilterInstantiate(
    _In_z_ const WCHAR * filterName,
    _In_ HANDLE *        filterHandle
);

HRESULT
FilterGetUacAudioProperty(
    _In_ HANDLE                 filter,
    _In_ UAC_AUDIO_PROPERTY*    audioProperty
);

HRESULT
FilterGetAsioBufferSize(
    _In_ HANDLE                 filter,
    _In_ uint16_t*              bufferSize
);

//HRESULT
//InstantiateMidiPin(
//    _In_ HANDLE        filter,
//    _In_ ULONG         pinId,
//    _In_ MidiTransport transport,
//    _In_ HANDLE *      pinHandle
//);
