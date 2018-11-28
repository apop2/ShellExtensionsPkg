/*
 * Fake ETET EFI application
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

EFI_STATUS Tcg2LocateProtocol(EFI_TCG2_PROTOCOL **Tcg2);

static EFI_STATUS InstallETET(VOID)
{
    EFI_TCG2_FINAL_EVENTS_TABLE *Table = NULL;

	EFI_STATUS Status;
	
    Status = gBS->AllocatePages(EfiRuntimeServicesData, sizeof(EFI_TCG2_FINAL_EVENTS_TABLE) + 0x1000, (VOID**)&Table);
	if (EFI_ERROR(Status)) {
		ShellPrintEx(-1, -1, L"Unable to allocate memory for ETET table: %r\n", Status);
		return Status;
	}

    Table->Version = EFI_TCG2_EVENT_HEADER_VERSION;
    Table->NumberOfEvents = 0;

    Status = gBS->InstallConfigurationTable(&gEfiTcg2FinalEventsTableGuid, Table);
	if (EFI_ERROR(Status)) {
		ShellPrintEx(-1, -1, L"Unable to install ETET table: %r\n", Status);
        gBS->FreePool(Table);
		return Status;
	}

	return EFI_SUCCESS;
}

/*
 * According to TCG EFI Protocol Specification for TPM 2.0 family,
 * all events generated after the invocation of EFI_TCG2_GET_EVENT_LOG
 * shall be stored in an instance of an EFI_CONFIGURATION_TABLE aka
 * EFI TCG 2.0 final events table. Hence, it is necessary to trigger the
 * internal switch through calling get_event_log() in order to allow
 * to retrieve the logs from OS runtime.
 */
static EFI_STATUS
TriggerETET(EFI_TCG2_EVENT_LOG_FORMAT Format)
{
	EFI_TCG2_PROTOCOL *Tcg2;
	EFI_STATUS Status;

	Status = Tcg2LocateProtocol(&Tcg2);;
	if (EFI_ERROR(Status))
		return Status;

	EFI_PHYSICAL_ADDRESS Start;
	EFI_PHYSICAL_ADDRESS End;
	BOOLEAN Truncated;

    return Tcg2->GetEventLog(Tcg2, Format, &Start, &End, &Truncated);
}

EFI_STATUS FakeEtet(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *Systab)
{
	EFI_TCG2_FINAL_EVENTS_TABLE *Table;

	EFI_STATUS Status = EtetLocateAddress((VOID **)&Table);
	if (!EFI_ERROR(Status)) {
		UINT64 Version = Table->Version;

		if (!Version || Version > EFI_TCG2_FINAL_EVENTS_TABLE_VERSION) 
        {
			ShellPrintEx(-1, -1, L"Unsupported ETET (version %llx)\n", Version);
			return EFI_UNSUPPORTED;
		}

		ShellPrintEx(-1, -1, L"ETET (version %lld, number of events %lld) detected\n", Version, Table->NumberOfEvents);
		ShellPrintEx(-1, -1, L"Skip to install a fake ETET\n");

		/*
		 * ETET may be not triggered to start logging the events
		 * if the number of events recorded is still zero.
		 */
		if (!Table->NumberOfEvents)
			goto trigger;

		return EFI_SUCCESS;
	}

	Status = InstallETET();
	if (EFI_ERROR(Status)) {
		ShellPrintEx(-1, -1, L"Unable to install the fake ETET: %r\n", Status);
		return Status;
	}

	ShellPrintEx(-1, -1, L"Fake ETET installed\n");

trigger:
	Status = TriggerETET(EFI_TCG2_EVENT_LOG_FORMAT_TCG_2);
	if (!EFI_ERROR(Status))
		ShellPrintEx(-1, -1, L"ETET triggered\n");
	else
		ShellPrintEx(-1, -1, L"Unable to trigger ETET\n");

	return Status;
}