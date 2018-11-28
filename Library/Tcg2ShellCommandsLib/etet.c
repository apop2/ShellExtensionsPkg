/*
 * ETET common functions
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

EFI_STATUS EtetLocateAddress(VOID **Table)
{
	EFI_GUID Guid = EFI_TCG2_FINAL_EVENTS_TABLE_GUID;
	EFI_STATUS Status;

    Status = EfiGetSystemConfigurationTable(&Guid, (VOID**)Table);
	if (EFI_ERROR(Status)) {
		ShellPrintEx(-1, -1, L"Unable to find ETET: %r\n", Status);
		return Status;
	}

	return Status;
}