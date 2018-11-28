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
#include <Library/HandleParsingLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/EdidActive.h>
#include <Library/ShellLib.h>
#include <Library/HandleParsingLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

STATIC CONST SHELL_PARAM_ITEM ParamList[] = {
  {NULL, TypeMax}
  };


VOID ParseAndPrintEdidInfo(EFI_HANDLE Handle, EFI_EDID_ACTIVE_PROTOCOL *EdidActive)
{
	UINTN i = 0;
    UINTN HorizontalVideo = 0;
    UINTN VerticalVideo = 0;
    UINTN Freq = 0;

    if(EdidActive != NULL && EdidActive->SizeOfEdid != 0) 
    {
        i = ConvertHandleToHandleIndex(Handle);
        ShellPrintEx(-1, -1, L"Handle %x\n", i);

        ShellPrintEx(-1, -1, L"  Established Timings\n");

        // Established timings I
        if(EdidActive->Edid[0x23] & 0x80)
            ShellPrintEx(-1, -1, L"    720 x 400 @ 70Hz\n");
        if(EdidActive->Edid[0x23] & 0x40)
            ShellPrintEx(-1, -1, L"    720 x 400 @ 88Hz \n");
        if(EdidActive->Edid[0x23] & 0x20)
            ShellPrintEx(-1, -1, L"    640 x 480 @ 60Hz \n");
        if(EdidActive->Edid[0x23] & 0x10)
            ShellPrintEx(-1, -1, L"    640 x 480 @ 67Hz \n");
        if(EdidActive->Edid[0x23] & 0x08)
            ShellPrintEx(-1, -1, L"    640 x 480 @ 72Hz \n");
        if(EdidActive->Edid[0x23] & 0x04)
            ShellPrintEx(-1, -1, L"    640 x 480 @ 75Hz \n");
        if(EdidActive->Edid[0x23] & 0x02)
            ShellPrintEx(-1, -1, L"    800 x 600 @ 56Hz \n");
        if(EdidActive->Edid[0x23] & 0x01)
            ShellPrintEx(-1, -1, L"    800 x 600 @ 60Hz \n");
        
        // Established timings II
        if(EdidActive->Edid[0x24] & 0x80)
            ShellPrintEx(-1, -1, L"    800 x 600 @ 72Hz \n");
        if(EdidActive->Edid[0x24] & 0x40)
            ShellPrintEx(-1, -1, L"    800 x 600 @ 75Hz \n");
        if(EdidActive->Edid[0x24] & 0x20)
            ShellPrintEx(-1, -1, L"    832 x 624 @ 75Hz \n");
        if(EdidActive->Edid[0x24] & 0x10)
            ShellPrintEx(-1, -1, L"    1024 x 768 @ 87Hz(I) \n");
        if(EdidActive->Edid[0x24] & 0x08)
            ShellPrintEx(-1, -1, L"    1024 x 768 @ 60Hz \n");
        if(EdidActive->Edid[0x24] & 0x04)
            ShellPrintEx(-1, -1, L"    1024 x 768 @ 70Hz \n");
        if(EdidActive->Edid[0x24] & 0x02)
            ShellPrintEx(-1, -1, L"    1024 x 768 @ 75Hz \n");
        if(EdidActive->Edid[0x24] & 0x01)
            ShellPrintEx(-1, -1, L"    1280 x 1024 @ 75Hz \n");
        
        // mfg timings 
        if(EdidActive->Edid[0x25] & 0x80)
            ShellPrintEx(-1, -1, L"    1152 x 870 @ 75Hz \n");

        ShellPrintEx(-1, -1, L"\n  Standard Timings\n");
        for(i = 0x26; i < 0x36; i = i + 2)
        {
            if(EdidActive->Edid[i] == 0)
                continue;

            HorizontalVideo = (EdidActive->Edid[i] + 31) * 8;
            if( (EdidActive->Edid[i+1] & 0xC0) == 0xC0)
                VerticalVideo = (HorizontalVideo * 9)/16;
            if( (EdidActive->Edid[i+1] & 0x80) == 0x80)
                VerticalVideo = (HorizontalVideo * 4)/5;
            if( (EdidActive->Edid[i+1] & 0x40) == 0x40)
                VerticalVideo = (HorizontalVideo * 3)/4;
            if( (EdidActive->Edid[i+1] & 0x00) == 0x00)
                VerticalVideo = (HorizontalVideo * 10)/16;

            Freq = (EdidActive->Edid[i+1] & 0x3F) + 60;

            ShellPrintEx(-1, -1, L"    Standard Timing: %d x %d @ %dHz \n", HorizontalVideo, VerticalVideo, Freq);
        }

        ShellPrintEx(-1, -1, L"\n  Detailed Timings\n");
        for(i = 0x36; i < 0x84; i = i + 0x18)
        {	
            // Check that it is a detailed descriptor (first word is non-zero), otherwise ignore 
            if((EdidActive->Edid[i+0] != 0x00) || (EdidActive->Edid[i+1] != 0x00) )
            {
                HorizontalVideo = ((EdidActive->Edid[i+4] & 0xF0) << 4) + EdidActive->Edid[i+02];
                VerticalVideo = ((EdidActive->Edid[i+7] & 0xF0) << 4) + EdidActive->Edid[i+05];

                if(i == 0x36)
                    ShellPrintEx(-1, -1, L"    Detailed Timing 1 (Native Resolution) : %d x %d \n", HorizontalVideo, VerticalVideo);
                else if(i == 0x48)
                    ShellPrintEx(-1, -1, L"    Detailed Timing 2 : %d x %d \n", HorizontalVideo, VerticalVideo);
                else if(i == 0x5A)
                    ShellPrintEx(-1, -1, L"    Detailed Timing 3 : %d x %d \n", HorizontalVideo, VerticalVideo);
                else if(i == 0x6C)
                    ShellPrintEx(-1, -1, L"    Detailed Timing 4 : %d x %d \n", HorizontalVideo, VerticalVideo);
            }
        }
        ShellPrintEx(-1, -1, L"\n");
    } 
    else 
    {
        ShellPrintEx(-1, -1, L"Edid information was not valid\n");
    }
}

SHELL_STATUS EFIAPI ShellCommandDisplayEdid(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_STATUS Status;
	EFI_HANDLE *HandleBuffer = NULL;
	UINTN HandleCount = 0;
	UINTN i = 0;
	EFI_EDID_ACTIVE_PROTOCOL *EdidActive = NULL;


    LIST_ENTRY *Package;
    CHAR16 *ProblemParam;
    UINT64 HandleNumber = (UINT64)(-1);
    CONST CHAR16 *Temp;

    Status = ShellCommandLineParse(ParamList, &Package, &ProblemParam, TRUE);
    if(EFI_ERROR(Status))  
    { 
        gBS->FreePool(ProblemParam);
        return SHELL_INVALID_PARAMETER;
    }

    if(ShellCommandLineGetCount(Package) > 2)
    {
        return SHELL_INVALID_PARAMETER;
    }

    Temp = ShellCommandLineGetRawValue(Package, 1);
    if(Temp != NULL) 
    {
        Status = ShellConvertStringToUint64(Temp, &HandleNumber, TRUE, TRUE);
        Status = gBS->HandleProtocol(ConvertHandleIndexToHandle((UINT32)HandleNumber), &gEfiEdidActiveProtocolGuid, &EdidActive);
        if(!EFI_ERROR(Status)) 
        {
            ParseAndPrintEdidInfo(ConvertHandleIndexToHandle((UINT32)HandleNumber), EdidActive);
        }
        else
            ShellPrintEx(-1, -1, L"Handle %s did not conain an EDID protocol\n", Temp);
    }
    else
    {
        Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiEdidActiveProtocolGuid, NULL, &HandleCount, &HandleBuffer);
        if(!EFI_ERROR(Status)) 
        {
            for(i = 0; i < HandleCount; i++) 
            {
                Status = gBS->HandleProtocol(HandleBuffer[i], &gEfiEdidActiveProtocolGuid, &EdidActive);
                if(!EFI_ERROR(Status)) 
                {
                    ParseAndPrintEdidInfo(HandleBuffer[i], EdidActive);
                }
            }
            gBS->FreePool(HandleBuffer);
        }
        if(EFI_ERROR(Status))
            ShellPrintEx(-1, -1, L"No EdidActive Protocols found\n");
    }
    
	return EFI_SUCCESS;
}


