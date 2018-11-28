/*
 * Show the TPM capability information
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

EFI_STATUS Tcg2GetCapability(EFI_TCG2_BOOT_SERVICE_CAPABILITY *Capability, UINT8 *CapabilitySize);

#define EFI_TCG2_BOOT_HASH_ALG_MASK (TREE_BOOT_HASH_ALG_SHA1 | TREE_BOOT_HASH_ALG_SHA256 | TREE_BOOT_HASH_ALG_SHA384 | TREE_BOOT_HASH_ALG_SHA512)

#define TREE_BOOT_HASH_ALG_MASK (TREE_BOOT_HASH_ALG_SHA1|TREE_BOOT_HASH_ALG_SHA256|TREE_BOOT_HASH_ALG_SHA384|TREE_BOOT_HASH_ALG_SHA512)

#define TREE_EVENT_LOG_FORMAT_MASK (TREE_EVENT_LOG_FORMAT_TCG_1_2)

#define EFI_TCG2_EVENT_LOG_FORMAT_MASK (EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2 | EFI_TCG2_EVENT_LOG_FORMAT_TCG_2)

EFI_STATUS Tpm2Capability(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *Systab)
{
	UINT8 TpmCapabilitySize = 0;
	EFI_STATUS Status;

	Status = Tcg2GetCapability(NULL, &TpmCapabilitySize);
	if (EFI_ERROR(Status))
		return Status;

	EFI_TCG2_BOOT_SERVICE_CAPABILITY *TpmCapability = NULL;
    Status = gBS->AllocatePool(EfiBootServicesData, TpmCapabilitySize, &TpmCapability);
    if(!EFI_ERROR(Status))
    {
        Status = Tcg2GetCapability(TpmCapability, &TpmCapabilitySize);
        if(!EFI_ERROR(Status))
        {

            if (TpmCapability->StructureVersion.Major == 1 &&
                    TpmCapability->StructureVersion.Minor == 0) {
                TREE_BOOT_SERVICE_CAPABILITY *TrEECapability;

                TrEECapability = (TREE_BOOT_SERVICE_CAPABILITY *)TpmCapability;

                ShellPrintEx(-1, -1, L"Structure Size: %d-byte\n",
                      (UINT8)TrEECapability->Size);

                ShellPrintEx(-1, -1, L"Structure Version: %d.%d\n",
                      (UINT8)TrEECapability->StructureVersion.Major,
                      (UINT8)TrEECapability->StructureVersion.Minor);

                ShellPrintEx(-1, -1, L"Protocol Version: %d.%d\n",
                      (UINT8)TrEECapability->ProtocolVersion.Major,
                      (UINT8)TrEECapability->ProtocolVersion.Minor);

                UINT32 Hash = TrEECapability->HashAlgorithmBitmap;
                ShellPrintEx(-1, -1, L"Supported Hash Algorithm: 0x%x (%s%s%s%s%s)\n",
                      Hash,
                      Hash & TREE_BOOT_HASH_ALG_SHA1 ? L"SHA-1" : L"",
                      Hash & TREE_BOOT_HASH_ALG_SHA256 ? L" SHA-256" : L"",
                      Hash & TREE_BOOT_HASH_ALG_SHA384 ? L" SHA-384" : L"",
                      Hash & TREE_BOOT_HASH_ALG_SHA512 ? L" SHA-512" : L"",
                      (Hash & ~TREE_BOOT_HASH_ALG_MASK) || !Hash ? L" dirty" : L"");

                EFI_TCG2_EVENT_LOG_BITMAP Format = TrEECapability->SupportedEventLogs;
                ShellPrintEx(-1, -1, L"Supported Event Log Format: 0x%x (%s%s%s)\n", Format,
                      Format & TREE_EVENT_LOG_FORMAT_TCG_1_2 ?
                      L"TCG1.2" : L"",
                      (Format & ~TREE_EVENT_LOG_FORMAT_MASK) || !Format ?
                      L" dirty" : L"");

                ShellPrintEx(-1, -1, L"TrEE Present: %s\n",
                      TrEECapability->TrEEPresentFlag ?
                      L"True" : L"False");

                ShellPrintEx(-1, -1, L"Max Command Size: %d-byte\n",
                      TrEECapability->MaxCommandSize);

                ShellPrintEx(-1, -1, L"Max Response Size: %d-byte\n",
                      TrEECapability->MaxResponseSize);

                ShellPrintEx(-1, -1, L"Manufacturer ID: 0x%x\n",
                      TrEECapability->ManufacturerID);
            } else if (TpmCapability->StructureVersion.Major == 1 &&
                    TpmCapability->StructureVersion.Minor == 1) {
                ShellPrintEx(-1, -1, L"Structure Size: %d-byte\n",
                      TpmCapability->Size);

                ShellPrintEx(-1, -1, L"Structure Version: %d.%d\n",
                      TpmCapability->StructureVersion.Major,
                      TpmCapability->StructureVersion.Minor);

                ShellPrintEx(-1, -1, L"Protocol Version: %d.%d\n",
                      TpmCapability->ProtocolVersion.Major,
                      TpmCapability->ProtocolVersion.Minor);

                UINT32 Hash = TpmCapability->HashAlgorithmBitmap;
                ShellPrintEx(-1, -1, L"Supported Hash Algorithm: 0x%x (%s%s%s%s%s%s)\n",
                      Hash,
                      Hash & EFI_TCG2_BOOT_HASH_ALG_SHA1 ? L"SHA-1" : L"",
                      Hash & EFI_TCG2_BOOT_HASH_ALG_SHA256 ? L" SHA-256" : L"",
                      Hash & EFI_TCG2_BOOT_HASH_ALG_SHA384 ? L" SHA-384" : L"",
                      Hash & EFI_TCG2_BOOT_HASH_ALG_SHA512 ? L" SHA-512" : L"",
                      Hash & EFI_TCG2_BOOT_HASH_ALG_SM3_256 ? L" SM3-256" : L"",
                      (Hash & ~EFI_TCG2_BOOT_HASH_ALG_MASK) || !Hash ?
                      L" dirty" : L"");

                EFI_TCG2_EVENT_LOG_BITMAP Format = TpmCapability->SupportedEventLogs;
                ShellPrintEx(-1, -1, L"Supported Event Log Format: 0x%x (%s%s%s)\n", Format,
                      Format & EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2 ?
                      L"TCG1.2" : L"",
                      Format & EFI_TCG2_EVENT_LOG_FORMAT_TCG_2 ?
                      L" TCG2.0" : L"",
                      (Format & ~EFI_TCG2_EVENT_LOG_FORMAT_MASK) || !Format ?
                      L" dirty" : L"");

                ShellPrintEx(-1, -1, L"TPM Present: %s\n",
                      TpmCapability->TPMPresentFlag ?
                      L"True" : L"False");

                ShellPrintEx(-1, -1, L"Max Command Size: %d-byte\n",
                      TpmCapability->MaxCommandSize);

                ShellPrintEx(-1, -1, L"Max Response Size: %d-byte\n",
                      TpmCapability->MaxResponseSize);

                ShellPrintEx(-1, -1, L"Manufacturer ID: 0x%x\n",
                      TpmCapability->ManufacturerID);

                ShellPrintEx(-1, -1, L"Number of PCR Banks: %d%s\n",
                      TpmCapability->NumberOfPCRBanks,
                      !TpmCapability->NumberOfPCRBanks ? L"(dirty)" : L"");

                EFI_TCG2_EVENT_ALGORITHM_BITMAP Bank = TpmCapability->ActivePcrBanks;
                ShellPrintEx(-1, -1, L"Active PCR Banks: 0x%x (%s%s%s%s%s%s)\n",
                      Bank,
                      Bank & EFI_TCG2_BOOT_HASH_ALG_SHA1 ? L"SHA-1" : L"",
                      Bank & EFI_TCG2_BOOT_HASH_ALG_SHA256 ? L" SHA-256" : L"",
                      Bank & EFI_TCG2_BOOT_HASH_ALG_SHA384 ? L" SHA-384" : L"",
                      Bank & EFI_TCG2_BOOT_HASH_ALG_SHA512 ? L" SHA-512" : L"",
                      Bank & EFI_TCG2_BOOT_HASH_ALG_SM3_256 ? L" SM3-256" : L"",
                      (Bank & ~EFI_TCG2_BOOT_HASH_ALG_MASK) || !Bank ?
                      L" dirty" : L"");
            } else {
                ShellPrintEx(-1, -1, L"Unsupported structure version: %d.%d\n",
                      TpmCapability->StructureVersion.Major,
                      TpmCapability->StructureVersion.Minor);

                Status = EFI_UNSUPPORTED;
            }
        }
        gBS->FreePool(TpmCapability);
    }

	return Status;
}