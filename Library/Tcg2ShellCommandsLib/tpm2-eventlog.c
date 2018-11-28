/*
 * Show the event logs
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
#include <IndustryStandard/TcpaAcpi.h>
#include <IndustryStandard/UefiTcgPlatform.h>
#include <Protocol/DevicePathToText.h>

EFI_STATUS Tcg2GetCapability(EFI_TCG2_BOOT_SERVICE_CAPABILITY *Capability, UINT8 *CapabilitySize);

#define EFI_TCG2_BOOT_HASH_ALG_MASK (TREE_BOOT_HASH_ALG_SHA1 | TREE_BOOT_HASH_ALG_SHA256 | TREE_BOOT_HASH_ALG_SHA384 | TREE_BOOT_HASH_ALG_SHA512)

#define TREE_BOOT_HASH_ALG_MASK (TREE_BOOT_HASH_ALG_SHA1|TREE_BOOT_HASH_ALG_SHA256|TREE_BOOT_HASH_ALG_SHA384|TREE_BOOT_HASH_ALG_SHA512)

#define TREE_EVENT_LOG_FORMAT_MASK (TREE_EVENT_LOG_FORMAT_TCG_1_2)

#define EFI_TCG2_EVENT_LOG_FORMAT_MASK (EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2 | EFI_TCG2_EVENT_LOG_FORMAT_TCG_2)

static CHAR16* DevicePathToStr(EFI_DEVICE_PATH_PROTOCOL *Dp)
{
    EFI_STATUS Status;
    CHAR16 *String = NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevicePathToText = NULL;
    Status = gBS->LocateProtocol(&gEfiDevicePathToTextProtocolGuid, NULL, &DevicePathToText);
    if(!EFI_ERROR(Status))
    {
         String = DevicePathToText->ConvertDevicePathToText(Dp, FALSE, FALSE);
    }
    return String;
}

static EFI_TCG2_BOOT_SERVICE_CAPABILITY* GetCapability(VOID)
{
	EFI_STATUS Status;
	UINTN TpmCapabilitySize = 0;
    EFI_TCG2_BOOT_SERVICE_CAPABILITY *TpmCapability = NULL;

	Status = Tcg2GetCapability(NULL, (UINT8 *)&TpmCapabilitySize);
    if(!EFI_ERROR(Status))
    {
        Status = gBS->AllocatePool(EfiBootServicesData, TpmCapabilitySize, &TpmCapability);
        if(!EFI_ERROR(Status))
        {
            Status = Tcg2GetCapability(TpmCapability, (UINT8 *)&TpmCapabilitySize);
            if (EFI_ERROR(Status)) 
            {
                gBS->FreePool(TpmCapability);
                return NULL;
            }
        }
    }
	return TpmCapability;
}

static EFI_TCG2_EVENT_ALGORITHM_BITMAP GetActivePcrBanks(EFI_TCG2_BOOT_SERVICE_CAPABILITY *TpmCapability)
{
	EFI_TCG2_EVENT_ALGORITHM_BITMAP Banks = 0;

	if (TpmCapability->StructureVersion.Major == 1 && TpmCapability->StructureVersion.Minor == 0) 
    {
		TREE_BOOT_SERVICE_CAPABILITY *TrEECapability;

		TrEECapability = (TREE_BOOT_SERVICE_CAPABILITY *)&TpmCapability;
		Banks = TrEECapability->HashAlgorithmBitmap & TREE_BOOT_HASH_ALG_MASK;
	} 
    else if (TpmCapability->StructureVersion.Major == 1 && TpmCapability->StructureVersion.Minor == 1)
		Banks = TpmCapability->ActivePcrBanks & EFI_TCG2_BOOT_HASH_ALG_MASK;
	else
		ShellPrintEx(-1, -1, L"Unsupported structure version: %d.%d\n", (UINT8)TpmCapability->StructureVersion.Major, (UINT8)TpmCapability->StructureVersion.Minor);

	return Banks;
}

static EFI_TCG2_EVENT_LOG_BITMAP GetSupportedEventLogFormat(EFI_TCG2_BOOT_SERVICE_CAPABILITY *TpmCapability)
{
	EFI_TCG2_EVENT_LOG_BITMAP Format = 0;

	if (TpmCapability->StructureVersion.Major == 1 && TpmCapability->StructureVersion.Minor == 0) 
    {
		TREE_BOOT_SERVICE_CAPABILITY *TrEECapability;

		TrEECapability = (TREE_BOOT_SERVICE_CAPABILITY *)TpmCapability;
		Format = TrEECapability->SupportedEventLogs & TREE_BOOT_HASH_ALG_MASK;
	} 
    else if (TpmCapability->StructureVersion.Major == 1 && TpmCapability->StructureVersion.Minor == 1)
		Format = TpmCapability->SupportedEventLogs & EFI_TCG2_EVENT_LOG_FORMAT_MASK;
	else
		ShellPrintEx(-1, -1, L"Unsupported structure version: %d.%d\n", (UINT8)TpmCapability->StructureVersion.Major, (UINT8)TpmCapability->StructureVersion.Minor);

	return Format;
}

static CHAR16 *GetEventTypeName(TCG_EVENTTYPE EventType)
{
	CHAR16 *Name;

	switch (EventType) 
    {
        case EV_POST_CODE:
            Name = L"Post code";
            break;
        case EV_NO_ACTION:
            Name = L"No action";
            break;
        case EV_SEPARATOR:
            Name = L"Separator";
            break;
        case EV_S_CRTM_CONTENTS:
            Name = L"S-CRTM contents";
            break;
        case EV_S_CRTM_VERSION:
            Name = L"S-CRTM version";
            break;
        case EV_CPU_MICROCODE:
            Name = L"CPU microcode";
            break;
        case EV_TABLE_OF_DEVICES:
            Name = L"Table of devices";
            break;
        case EV_EFI_EVENT_BASE:
            Name = L"EFI event base";
            break;
        case EV_EFI_VARIABLE_DRIVER_CONFIG:
            Name = L"EFI variable driver config";
            break;
        case EV_EFI_VARIABLE_BOOT:
            Name = L"EFI variable boot";
            break;
        case EV_EFI_BOOT_SERVICES_APPLICATION:
            Name = L"EFI boot services application";
            break;
        case EV_EFI_BOOT_SERVICES_DRIVER:
            Name = L"EFI boot services driver";
            break;
        case EV_EFI_RUNTIME_SERVICES_DRIVER:
            Name = L"EFI runtime services driver";
            break;
        case EV_EFI_GPT_EVENT:
            Name = L"EFI GPT event";
            break;
        case EV_EFI_ACTION:
            Name = L"EFI action";
            break;
        case EV_EFI_PLATFORM_FIRMWARE_BLOB:
            Name = L"EFI platform firmware blob";
            break;
        case EV_EFI_HANDOFF_TABLES:
            Name = L"EFI handoff tables";
            break;
        case EV_EFI_VARIABLE_AUTHORITY:
            Name = L"EFI variable authority";
            break;
        default:
            Name = L"Unknown";
	}

	return Name;
}

static VOID ShowString(CHAR16 *String, UINTN StringLength)
{
    EFI_STATUS Status;
    CHAR16 *Buffer = NULL;

    Status = gBS->AllocatePool(EfiBootServicesData, sizeof(CHAR16) * (StringLength + 1), &Buffer);
    if(!EFI_ERROR(Status))
    {
        gBS->CopyMem(Buffer, String, StringLength * sizeof(CHAR16));
        Buffer[StringLength] = 0;
        ShellPrintEx(-1, -1, Buffer);

        Status = gBS->FreePool(Buffer);
    }
}

static VOID ShowAsciiString(CHAR8 *String, UINT32 StringLength)
{
    EFI_STATUS Status;
    CHAR8 *Buffer = NULL;
    Status = gBS->AllocatePool(EfiBootServicesData, sizeof(CHAR8) * (StringLength + 1), &Buffer);
    if(!EFI_ERROR(Status))
    {

        gBS->CopyMem(Buffer, String, StringLength * sizeof(CHAR8));
        Buffer[StringLength] = 0;

        ShellPrintEx(-1, -1, L"%a", Buffer);

        Status = gBS->FreePool(Buffer);
    }
}

static VOID ShowHexString(UINT8 *Binary, UINTN BinarySize)
{
	for (UINT32 Index = 0; Index < BinarySize; ++Index)
		ShellPrintEx(-1, -1, L"%02x", Binary[Index]);
}

static VOID ShowVariableLog(VOID *Binary, UINTN BinarySize)
{
	EFI_VARIABLE_DATA_TREE *VarLog;

	VarLog = (EFI_VARIABLE_DATA_TREE *)Binary;

	ShellPrintEx(-1, -1, L"    Variable name: %g\n", &VarLog->VariableName);
	ShellPrintEx(-1, -1, L"    Unicode name length: %lld-byte\n", VarLog->UnicodeNameLength * sizeof(CHAR16));
	ShellPrintEx(-1, -1, L"    Variable data length: %lld-byte\n", VarLog->VariableDataLength);

	ShellPrintEx(-1, -1, L"    Unicode name: ");
	ShowString(VarLog->UnicodeName, (UINTN)VarLog->UnicodeNameLength);
	ShellPrintEx(-1, -1, L"\n");

	ShellPrintEx(-1, -1, L"    Variable data:\n");
	if (VarLog->VariableDataLength) 
    {
		ShellPrintEx(-1, -1, L"      ");
		UINT8 *VariableData = (UINT8 *)VarLog + sizeof(*VarLog) -
				      sizeof(VarLog->UnicodeName) -
				      sizeof(VarLog->VariableData) +
				      VarLog->UnicodeNameLength;
		ShowHexString(VariableData, (UINTN)VarLog->VariableDataLength);
		ShellPrintEx(-1, -1, L"\n");
	}
}

static VOID ShowImageLog(VOID *Binary, UINTN BinarySize)
{
	EFI_IMAGE_LOAD_EVENT *ImageLog;
    CHAR16 *String = NULL;

	ImageLog = (EFI_IMAGE_LOAD_EVENT *)Binary;

	ShellPrintEx(-1, -1, L"    Image location: 0x%llx\n", ImageLog->ImageLocationInMemory);
	ShellPrintEx(-1, -1, L"    Image length: %d-byte\n", ImageLog->ImageLengthInMemory);
	ShellPrintEx(-1, -1, L"    Image link time address: 0x%x\n", ImageLog->ImageLinkTimeAddress);
	ShellPrintEx(-1, -1, L"    Device path length: %d-byte\n", ImageLog->LengthOfDevicePath);

	if (ImageLog->LengthOfDevicePath) 
    {
		ShellPrintEx(-1, -1, L"    Device path:\n");
		ShellPrintEx(-1, -1, L"      ");
        String = DevicePathToStr(ImageLog->DevicePath);
        if(String != NULL)
        {
            ShellPrintEx(-1, -1, String);
            gBS->FreePool(String);
        }
		ShellPrintEx(-1, -1, L"\n");
	}
}

static VOID ShowEventData(TCG_EVENTTYPE EventType, UINT8 *EventData, UINTN EventDataSize)
{
	ShellPrintEx(-1, -1, L"  Event Data:\n");

	if (EventType == EV_POST_CODE || EventType == EV_S_CRTM_VERSION || EventType == EV_SEPARATOR) 
    {
		ShellPrintEx(-1, -1, L"    ");
		ShowHexString(EventData, EventDataSize);
		ShellPrintEx(-1, -1, L"\n");
	} 
    else if (EventType == EV_EFI_VARIABLE_DRIVER_CONFIG || EventType == EV_EFI_VARIABLE_BOOT)
		ShowVariableLog(EventData, EventDataSize);
	else if (EventType == EV_EFI_BOOT_SERVICES_APPLICATION || EventType == EV_EFI_BOOT_SERVICES_DRIVER || EventType == EV_EFI_RUNTIME_SERVICES_DRIVER)
		ShowImageLog(EventData, EventDataSize);
	else 
    {
		ShellPrintEx(-1, -1, L"    ");
		ShowHexString(EventData, EventDataSize);
		ShellPrintEx(-1, -1, L"\n");
	}
}

static UINTN
GetTcgDigestAlgorithmLength(TPM_ALG_ID AlgId)
{
	UINTN Length;

	switch (AlgId) {
	case TPM_ALG_SHA1:
		Length = SHA1_DIGEST_SIZE;
		break;
	case TPM_ALG_SHA256:
		Length = SHA256_DIGEST_SIZE;
		break;
	case TPM_ALG_SHA384:
		Length = SHA384_DIGEST_SIZE;
		break;
	case TPM_ALG_SHA512:
		Length = SHA512_DIGEST_SIZE;
		break;
	case TPM_ALG_SM3_256:
		Length = SM3_256_DIGEST_SIZE;
		break;
	default:
		Length = 0;
	}

	return Length;
}

static CHAR16 *
GetTcgAlgorithmName(TPM_ALG_ID AlgId)
{
	CHAR16 *Name;

	switch (AlgId) {
	case TPM_ALG_SHA1:
		Name = L"SHA1";
		break;
	case TPM_ALG_AES:
		Name = L"AES";
		break;
	case TPM_ALG_KEYEDHASH:
		Name = L"KEYEDHASH";
		break;
	case TPM_ALG_SHA256:
		Name = L"SHA256";
		break;
	case TPM_ALG_SHA384:
		Name = L"SHA384";
		break;
	case TPM_ALG_SHA512:
		Name = L"SHA512";
		break;
	case TPM_ALG_NULL:
		Name = L"NULL";
		break;
	case TPM_ALG_SM3_256:
		Name = L"SM3-256";
		break;
	case TPM_ALG_SM4:
		Name = L"SM4";
		break;
	case TPM_ALG_RSASSA:
		Name = L"RSASSA";
		break;
	case TPM_ALG_RSAES:
		Name = L"RSAES";
		break;
	case TPM_ALG_RSAPSS:
		Name = L"RSAPSS";
		break;
	case TPM_ALG_OAEP:
		Name = L"OAEP";
		break;
	case TPM_ALG_ECDSA:
		Name = L"ECDSA";
		break;
	case TPM_ALG_ECDH:
		Name = L"ECDH";
		break;
	case TPM_ALG_ECDAA:
		Name = L"ECDAA";
		break;
	case TPM_ALG_SM2:
		Name = L"SM2";
		break;
	case TPM_ALG_ECSCHNORR:
		Name = L"ECSCHNORR";
		break;
	case TPM_ALG_ECMQV:
		Name = L"ECMQV";
		break;
	case TPM_ALG_KDF1_SP800_56a:
		Name = L"KDF1_SP800_56a";
		break;
	case TPM_ALG_KDF2:
		Name = L"KDF2";
		break;
	case TPM_ALG_KDF1_SP800_108:
		Name = L"KDF1_SP800_108";
		break;
	case TPM_ALG_ECC:
		Name = L"ECC";
		break;
	case TPM_ALG_SYMCIPHER:
		Name = L"SYMCIPHER";
		break;
	case TPM_ALG_CTR:
		Name = L"CTR";
		break;
	case TPM_ALG_OFB:
		Name = L"OFB";
		break;
	case TPM_ALG_CBC:
		Name = L"CBC";
		break;
	case TPM_ALG_CFB:
		Name = L"CFB";
		break;
	case TPM_ALG_ECB:
		Name = L"ECB";
		break;
	default:
		Name = L"Unknown";
	}

	return Name;
}

static VOID ShowTcg2EventLogs(EFI_PHYSICAL_ADDRESS StartEntry, EFI_PHYSICAL_ADDRESS LastEntry)
{
	ShellPrintEx(-1, -1, L"TCG 2.0 Normal Event Logs:\n");

	TCG_PCR_EVENT *Event = (TCG_PCR_EVENT *)(UINTN)StartEntry;
	ShellPrintEx(-1, -1, L"  Event Log Header: %d-byte\n", Event->EventSize);

	TCG_EfiSpecIDEventStruct *EventLogHeader;
	EventLogHeader = (TCG_EfiSpecIDEventStruct *)Event->Event;

	ShellPrintEx(-1, -1, L"    Signature: ");
	ShowAsciiString(EventLogHeader->signature, sizeof(EventLogHeader->signature));
	ShellPrintEx(-1, -1, L"\n");

	ShellPrintEx(-1, -1, L"    Platform class: 0x%x (%s)\n",
	      EventLogHeader->platformClass,
	      EventLogHeader->platformClass == TCG_PLATFORM_TYPE_CLIENT ?
	      L"Client platform" :
	      (EventLogHeader->platformClass == TCG_PLATFORM_TYPE_SERVER
	       ? L"Server platform" : L"dirty"));
	ShellPrintEx(-1, -1, L"    Spec version: %d.%d errata %d\n", EventLogHeader->specVersionMajor, EventLogHeader->specVersionMinor, EventLogHeader->specErrata);
	ShellPrintEx(-1, -1, L"    UINTN size: %d-byte\n", EventLogHeader->uintnSize * sizeof(UINT32));
#if 0
	ShellPrintEx(-1, -1, L"    Number of algorithms: %d\n", EventLogHeader->numberOfAlgorithms);

	for (UINTN Idx = 0; Idx < EventLogHeader->numberOfAlgorithms; ++Idx) 
    {
		TCG_EfiSpecIdEventAlgorithmSize *Alg;

		Alg = EventLogHeader->digestSizes + Idx;
		ShellPrintEx(-1, -1, L"      Digest algorithm ID: 0x%x, %d-byte (%s)\n",
		      Alg->algorithmId, Alg->digestSize,
		      GetTcgAlgorithmName(Alg->algorithmId));
	}

	UINT8 *Pointer = (UINT8 *)(EventLogHeader->digestSizes + EventLogHeader->numberOfAlgorithms);
	UINT8 VendorInfoSize = *Pointer++;
	ShellPrintEx(-1, -1, L"    Vendor info size: %d-byte\n", VendorInfoSize);

	ShellPrintEx(-1, -1, L"    Vendor info: ");
	ShowHexString(Pointer, VendorInfoSize);
	ShellPrintEx(-1, -1, L"\n");
#endif
	UINTN Index = 1;
	TCG_PCR_EVENT2 *Event2;

	Event2 = (TCG_PCR_EVENT2 *)((UINT8 *)(Event + 1) + Event->EventSize);

	while ((EFI_PHYSICAL_ADDRESS)Event2 < LastEntry) 
    {
		ShellPrintEx(-1, -1, L"[%02d]\n", Index);
		ShellPrintEx(-1, -1, L"  PCR Index: %d\n", Event2->PCRIndex);
		ShellPrintEx(-1, -1, L"  Event Type: 0x%x (%s)\n", Event2->EventType, GetEventTypeName(Event2->EventType));

		UINT8 *Pointer = (UINT8 *)Event2->Digest.digests;

		ShellPrintEx(-1, -1, L"  Digests: %d\n", Event2->Digest.count);
		for (UINTN Idx = 0; Idx < Event2->Digest.count; ++Idx) 
        {
			TPMT_HA *Digest = (TPMT_HA *)Pointer;
			UINTN DigestAlgLength;
			CHAR16 *DigestAlgName;

			DigestAlgLength = GetTcgDigestAlgorithmLength(Digest->hashAlg);
			DigestAlgName = GetTcgAlgorithmName(Digest->hashAlg);

			ShellPrintEx(-1, -1, L"    %s: ", DigestAlgName);
			ShowHexString((UINT8*)&(Digest->digest), DigestAlgLength);
			ShellPrintEx(-1, -1, L"\n");

			Pointer += sizeof(TPMT_HA) + DigestAlgLength;
		}

		UINT32 EventDataSize = *(UINT32 *)Pointer;
		Pointer += sizeof(UINT32);
		ShellPrintEx(-1, -1, L"  Event Size: %d-byte\n", EventDataSize);

		ShowEventData(Event2->EventType, Pointer, EventDataSize);

		Event2 = (TCG_PCR_EVENT2 *)(Pointer + EventDataSize);
		++Index;

        ShellPromptForResponse(ShellPromptResponseTypeAnyKeyContinue, L"Press any key to display the next log\n", NULL);
	}
}

static VOID ShowTcg1EventLogs(EFI_PHYSICAL_ADDRESS StartEntry, EFI_PHYSICAL_ADDRESS LastEntry)
{
	TCG_PCR_EVENT *Event = (TCG_PCR_EVENT *)(UINTN)StartEntry;
	UINTN Index = 1;

	ShellPrintEx(-1, -1, L"TCG 1.2 Normal Event Logs:\n");
	while ((EFI_PHYSICAL_ADDRESS)Event <= LastEntry) 
    {
		ShellPrintEx(-1, -1, L"[%02d]\n", Index);
		ShellPrintEx(-1, -1, L"  PCR Index: %d\n", Event->PCRIndex);
		ShellPrintEx(-1, -1, L"  Event Type: 0x%x (%s)\n", Event->EventType, GetEventTypeName(Event->EventType));
		ShellPrintEx(-1, -1, L"  Digest: ");
		for (UINTN Idx = 0; Idx < sizeof(Event->Digest); ++Idx)
			ShellPrintEx(-1, -1, L"%02x", Event->Digest.digest[Idx]);

		ShellPrintEx(-1, -1, L"\n");
		ShellPrintEx(-1, -1, L"  Event Size: %d-byte\n", Event->EventSize);
		ShowEventData(Event->EventType, Event->Event, Event->EventSize);

        // sizeof(TCG_PCR_EVENT) contains a single byte of Event->Digest data, so subtract 1 byte of event data
		Event = (TCG_PCR_EVENT *)((UINT8 *)(Event + 1) + Event->EventSize - 1);
		++Index;

        ShellPromptForResponse(ShellPromptResponseTypeAnyKeyContinue, L"Press any key to display the next log\n", NULL);
	}
}

static VOID ShowEventLogs(EFI_TCG2_EVENT_LOG_BITMAP Format, EFI_PHYSICAL_ADDRESS StartEntry, EFI_PHYSICAL_ADDRESS LastEntry)
{
	if (Format == EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2)
		ShowTcg1EventLogs(StartEntry, LastEntry);
	else
		ShowTcg2EventLogs(StartEntry, LastEntry);
}

EFI_STATUS Tpm2EventLog(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *Systab)
{
    EFI_STATUS Status;

	EFI_TCG2_BOOT_SERVICE_CAPABILITY *TpmCapability;
	EFI_TCG2_PROTOCOL *Tcg2;
	EFI_TCG2_EVENT_LOG_BITMAP SupportedFormats;

	TpmCapability = GetCapability();
	if (!TpmCapability)
		return EFI_UNSUPPORTED;


	SupportedFormats = GetSupportedEventLogFormat(TpmCapability);
	gBS->FreePool(TpmCapability);
	if (!SupportedFormats)
		return EFI_UNSUPPORTED;


	Status = gBS->LocateProtocol(&gEfiTcg2ProtocolGuid, NULL, (VOID**)&Tcg2);
    if(!EFI_ERROR(Status))
    {

        EFI_TCG2_EVENT_LOG_BITMAP FormatList[] = {EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2, EFI_TCG2_EVENT_LOG_FORMAT_TCG_2};
        for (UINTN Index = 0; Index < (sizeof(FormatList) / sizeof(EFI_TCG2_EVENT_LOG_BITMAP)); ++Index) 
        {
            if (!(SupportedFormats & FormatList[Index]))
                continue;

            EFI_PHYSICAL_ADDRESS StartEntry;
            EFI_PHYSICAL_ADDRESS LastEntry;
            BOOLEAN Truncated;

            Status = Tcg2->GetEventLog(Tcg2, FormatList[Index], &StartEntry, &LastEntry, &Truncated);
            if(!EFI_ERROR(Status))
            {

                if (StartEntry == LastEntry) 
                {
                    CONST CHAR16 *Ver[] = {L"1.2", L"2.0"};

                    ShellPrintEx(-1, -1, L"Skip the empty TCG %s event log area\n", Ver[Index]);
                    continue;
                }

                ShowEventLogs(FormatList[Index], StartEntry, LastEntry);

                if (Truncated)
                    ShellPrintEx(-1, -1, L"The event log area is truncated\n");
            }
        }
    }
	return Status;
}