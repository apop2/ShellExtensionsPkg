/*
 * Detect TPM device status 
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
#include <Protocol/TrEEProtocol.h>

EFI_STATUS EtetLocateAddress(VOID **Table);
EFI_STATUS Tcg2GetCapability(EFI_TCG2_BOOT_SERVICE_CAPABILITY *Capability, UINT8 *CapabilitySize);

static VOID DetectETET(VOID)
{
	VOID *Table;
	EFI_STATUS Status = EtetLocateAddress(&Table);

	if (!EFI_ERROR(Status))
		ShellPrintEx(-1, -1, L"ETET is detected\n");
}

EFI_STATUS Tpm2EfiInfo(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *Systab)
{
	EFI_STATUS Status;

	EFI_TCG2_PROTOCOL *Tcg2;
	Status = gBS->LocateProtocol(&gEfiTcg2ProtocolGuid, NULL, (VOID**)&Tcg2);
    if(!EFI_ERROR(Status))
    {
        ShellPrintEx(-1, -1, L"EFI TCG2 protocol is installed\n");

        UINT8 TpmCapabilitySize = 0;
        Status = Tcg2GetCapability(NULL, &TpmCapabilitySize);
        if (EFI_ERROR(Status))
            return Status;

        EFI_TCG2_BOOT_SERVICE_CAPABILITY *TpmCapability;
        Status = gBS->AllocatePool(EfiBootServicesData, TpmCapabilitySize, &TpmCapability);
        if (!TpmCapability)
            return Status;

        Status = Tcg2GetCapability(TpmCapability, &TpmCapabilitySize);
        if (EFI_ERROR(Status)) {
            gBS->FreePool(TpmCapability);
            return Status;
        }

        if (!EFI_ERROR(Status)) {
            UINT8 Major = TpmCapability->StructureVersion.Major;
            UINT8 Minor = TpmCapability->StructureVersion.Minor;

            if (Major == 1 && Minor == 0) {
                TREE_BOOT_SERVICE_CAPABILITY *TrEECapability;

                TrEECapability = (TREE_BOOT_SERVICE_CAPABILITY *)TpmCapability;
                if (TrEECapability->TrEEPresentFlag)
                    ShellPrintEx(-1, -1, L"TPM 2.0 (TrEE) device is present\n");
                else
                    ShellPrintEx(-1, -1, L"TPM 2.0 (TrEE) device is absent\n");
            } else if (Major == 1 && Minor == 1) {
                if (TpmCapability->TPMPresentFlag)
                    ShellPrintEx(-1, -1, L"TPM 2.0 device is present\n");
                else
                    ShellPrintEx(-1, -1, L"TPM 2.0 device is absent\n");
            } else
                ShellPrintEx(-1, -1, L"Unsupported capability structure version: "
                      L"%d.%d\n", Major, Minor);
        }
    }
	DetectETET();

	return EFI_SUCCESS;
}