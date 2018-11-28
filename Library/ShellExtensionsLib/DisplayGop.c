/**
    Copyright 2018 Aaron Pop

    Redistribution and use in source and binary forms, with or without modification, 
        are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
        list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice, 
        this list of conditions and the following disclaimer in the documentation 
        and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its contributors may 
        be used to endorse or promote products derived from this software without 
        specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
    OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/
#include <ShellBase.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/EdidActive.h>
#include <Library/UefiLib.h>
#include <Library/ShellLib.h>
#include <Library/HandleParsingLib.h>
#include <Library/UefiBootServicesTableLib.h>   
#include <Library/DevicePathLib.h>

STATIC CONST SHELL_PARAM_ITEM ParamList[] = {
  {NULL, TypeMax}
  };


VOID SetGopDeviceMode(EFI_HANDLE Device, UINT32 NewMode)
{
    EFI_STATUS Status;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop = NULL;

    Status = gBS->HandleProtocol(Device, &gEfiGraphicsOutputProtocolGuid, &Gop);
    if(!EFI_ERROR(Status))
    {
        if(NewMode > Gop->Mode->MaxMode)
            ShellPrintEx(-1, -1, L"Mode %d is greater than max mode %d\n", NewMode, Gop->Mode->MaxMode);
        else
            Status = Gop->SetMode(Gop, NewMode);
    }
    else
        ShellPrintEx(-1, -1, L"Handle does not contain a Graphics Output Protocol Instance\n");
}

VOID DisplayGopDeviceInfo(EFI_HANDLE Device)
{
    EFI_STATUS Status;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop = NULL;

	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *ModeInfo;
	UINTN ModeSize = 0;
    UINT32 i;

    EFI_DEVICE_PATH_PROTOCOL *Dp = NULL;

    Status = gBS->HandleProtocol(Device, &gEfiGraphicsOutputProtocolGuid, &Gop);
    if(!EFI_ERROR(Status)) 
    {
        i = (UINT32)ConvertHandleToHandleIndex(Device);
        ShellPrintEx(-1, -1, L"Handle %x\n", i);

        Status = gBS->HandleProtocol(Device, &gEfiDevicePathProtocolGuid, &Dp);
        if(!EFI_ERROR(Status))
        {
            CHAR16 *Str = ConvertDevicePathToText(Dp, TRUE, FALSE);
            ShellPrintEx(-1, -1, L"  %s\n", Str);
        }
        else
        {
            ShellPrintEx(-1, -1, L"Virtual Device\n");
        }

        for(i = 0; i < Gop->Mode->MaxMode; i++)
        {
            Status = Gop->QueryMode(Gop, i, &ModeSize, &ModeInfo);
            if(!EFI_ERROR(Status))
            {
                ShellPrintEx(-1, -1, L"    Mode %-2x: %-5d x %-5d", i, ModeInfo->HorizontalResolution, ModeInfo->VerticalResolution);
                if(i == Gop->Mode->Mode)
                    ShellPrintEx(-1, -1, L" *");
                ShellPrintEx(-1, -1, L"\n");
                gBS->FreePool(ModeInfo);
            }
        }

        ShellPrintEx(-1, -1, L"\n");
    }
    else
        ShellPrintEx(-1, -1, L"Handle does not contain a Graphics Output Protocol Instance\n");
}

VOID DisplayAllGopDevices(VOID)
{
    EFI_STATUS Status;
	EFI_HANDLE *HandleBuffer = NULL;
	UINTN HandleCount = 0;
	UINTN i = 0;

    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiGraphicsOutputProtocolGuid, NULL, &HandleCount, &HandleBuffer);
	if(!EFI_ERROR(Status)) 
    {
		for(i = 0; i < HandleCount; i++) 
        {
            DisplayGopDeviceInfo(HandleBuffer[i]);
		}
	}
    else
        ShellPrintEx(-1, -1, L"No Graphics Output Devices Found\n");
}

SHELL_STATUS EFIAPI ShellCommandDisplayGop(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_STATUS Status;

    LIST_ENTRY *Package;
    CHAR16 *ProblemParam;
    UINT64 HandleNumber = (UINT64)(-1);
    UINT64 NewMode = (UINT64)(-1);
    CONST CHAR16 *Temp;

    Status = ShellCommandLineParse(ParamList, &Package, &ProblemParam, TRUE);
    if(EFI_ERROR(Status))  
    { 
        Print(L"Problem Param! %s\n", ProblemParam);
        gBS->FreePool(ProblemParam);
        return SHELL_INVALID_PARAMETER;
    }

    if(ShellCommandLineGetCount(Package) > 3)
    {
        return SHELL_INVALID_PARAMETER;
    }

    Temp = ShellCommandLineGetRawValue(Package, 1);
    if(Temp != NULL) 
    {
        Status = ShellConvertStringToUint64(Temp, &HandleNumber, TRUE, TRUE);
    }

    Temp = ShellCommandLineGetRawValue(Package, 2);
    if(Temp != NULL) 
    {
        Status = ShellConvertStringToUint64(Temp, &NewMode, TRUE, TRUE);
    }

    if(NewMode != (-1))
    {
        SetGopDeviceMode(ConvertHandleIndexToHandle((UINT32)HandleNumber), (UINT32)NewMode);
    }
    else if(HandleNumber != (UINT64)(-1))
        DisplayGopDeviceInfo(ConvertHandleIndexToHandle((UINTN)HandleNumber));
    else
    {
        DisplayAllGopDevices();
    }
    return EFI_SUCCESS;
}