/*
 * Dump the content of ETET
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

EFI_STATUS EtetLocateAddress(VOID **Table);

static EFI_STATUS DumpEtetV1(EFI_TCG2_FINAL_EVENTS_TABLE *Table)
{
	ShellPrintEx(-1, -1, L"ETET Structure Version 1\n");
	ShellPrintEx(-1, -1, L"  Number of Events: %lld\n", Table->NumberOfEvents);

	return EFI_SUCCESS;
}

EFI_STATUS
DumpEtet(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *Systab)
{

	EFI_TCG2_FINAL_EVENTS_TABLE *Table;

	EFI_STATUS Status = EtetLocateAddress((VOID **)&Table);
	if (EFI_ERROR(Status))
		return Status;

	UINT64 Version = Table->Version;

	if (!Version || Version >
			EFI_TCG2_FINAL_EVENTS_TABLE_VERSION) {
		ShellPrintEx(-1, -1, L"Unsupported ETET (version %llx)\n", Version);
		return EFI_UNSUPPORTED;
	}

	if (Version == EFI_TCG2_FINAL_EVENTS_TABLE_VERSION)
		DumpEtetV1(Table);

	return EFI_SUCCESS;
}