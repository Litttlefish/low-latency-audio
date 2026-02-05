// Copyright (c) Microsoft Corporation.
// Copyright (c) Yamaha Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Microsoft Low-Latency Audio driver project.
// Further information: https://aka.ms/asio
// ============================================================================
// ASIO is a trademark and software of Steinberg Media Technologies GmbH

/*++

Module Name:

    InterruptDataMessage.h

Abstract:

    Defines the process to respond to an interrupt data message.

Environment:

    Kernel-mode Driver Framework

--*/

#ifndef _INTERRUPT_DATA_MESSAGE_H_
#define _INTERRUPT_DATA_MESSAGE_H_

#include <windef.h>
#include <ks.h>

#include "public.h"
#include "UAC_User.h"

EXTERN_C_START
#include "usbdi.h"
#include "usbdlib.h"
#include <wdfusb.h>

__drv_maxIRQL(PASSIVE_LEVEL)
PAGED_CODE_SEG
NTSTATUS USBAudioAcxDriverStartInterruptDataReception(
    _In_ PDEVICE_CONTEXT deviceContext
);

__drv_maxIRQL(PASSIVE_LEVEL)
PAGED_CODE_SEG
NTSTATUS USBAudioAcxDriverStopInterruptDataReception(
    _In_ PDEVICE_CONTEXT deviceContext
);

__drv_maxIRQL(DISPATCH_LEVEL)
NONPAGED_CODE_SEG
EVT_WDF_USB_READER_COMPLETION_ROUTINE USBAudioAcxDriverEvtInterruptDataMessageCompletionRoutine;

__drv_maxIRQL(DISPATCH_LEVEL)
NONPAGED_CODE_SEG
EVT_WDF_USB_READERS_FAILED USBAudioAcxDriverEvtInterruptDataMessageCompletionRoutineFailed;

EXTERN_C_END

#endif // #ifndef _INTERRUPT_DATA_MESSAGE_H_
