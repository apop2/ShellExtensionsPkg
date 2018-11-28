/*
 * EFI TCG2 common functions
 *
 * Copyright (c) 2016, Wind River Systems, Inc.
 * All rights reserved.
 *
 * See "LICENSE" for license terms.
 *
 * Author:
 *      Jia Zhang <zhang.jia@linux.alibaba.com>
 */

#include <ShellBase.h>
#include <Library/ShellLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/Tcg2Protocol.h>

EFI_STATUS Tcg2GetCapability(EFI_TCG2_BOOT_SERVICE_CAPABILITY *Capability, UINT8 *CapabilitySize)
{
	EFI_TCG2_PROTOCOL *Tcg2;
	EFI_STATUS Status;

    Status = gBS->LocateProtocol(&gEfiTcg2ProtocolGuid, NULL, (VOID**)&Tcg2);
	if (EFI_ERROR(Status)) {
		ShellPrintEx(-1, -1, L"Unable to locate EFI TCG2 Protocol: %r\n", Status);
		return Status;
	}

	EFI_TCG2_BOOT_SERVICE_CAPABILITY TpmCapability;

	if (!*CapabilitySize)
		TpmCapability.Size = sizeof(TpmCapability);
	else
		TpmCapability.Size = *CapabilitySize;

	Status = Tcg2->GetCapability( Tcg2, &TpmCapability);
	if (EFI_ERROR(Status)) {
		ShellPrintEx(-1, -1, L"Unable to get the TPM capability: %r\n", Status);
		if (Status == EFI_BUFFER_TOO_SMALL)
			ShellPrintEx(-1, -1, L"Only %d-byte is filled in the capability "
			      L"structure\n", TpmCapability.Size);
		return Status;
	}

	if (!*CapabilitySize)
		*CapabilitySize = TpmCapability.Size;
	else if (Capability) {
		UINTN CopySize = TpmCapability.Size;

		if (*CapabilitySize < CopySize)
			CopySize = *CapabilitySize;

        gBS->CopyMem( Capability, &TpmCapability, CopySize);
	}

	return Status;
}