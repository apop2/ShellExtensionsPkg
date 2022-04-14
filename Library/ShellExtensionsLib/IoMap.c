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
#include <PiDxe.h>
#include <Library/ShellLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/HandleParsingLib.h>
SHELL_STATUS EFIAPI ShellCommandIoMap(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_STATUS Status;

    EFI_GCD_IO_SPACE_DESCRIPTOR *GcdIo = NULL;
    UINTN Count;
    UINTN i = 0;

    UINTN HandleNumber = 0;
    
    Status = gDS->GetIoSpaceMap(&Count, &GcdIo);
    if(!EFI_ERROR(Status))
    {
        ShellPrintEx(-1, -1, L"Io Type                 Base Address       Length             DeviceHandle\n");
        for(i = 0; i < Count; i++)
        {
            if(GcdIo[i].ImageHandle != NULL)
            {
                switch(GcdIo[i].GcdIoType)
                {
                    case EfiGcdIoTypeNonExistent:
                        ShellPrintEx(-1, -1, L"EfiGcdIoTypeNonExistent ");
                        break;
                        
                    case EfiGcdIoTypeReserved:
                        ShellPrintEx(-1, -1, L"EfiGcdIoTypeReserved    ");
                        break;
                        
                    case EfiGcdIoTypeIo:
                        ShellPrintEx(-1, -1, L"EfiGcdIoTypeIo          ");
                        break;
                    default:
                        break;
                }
                ShellPrintEx(-1, -1, L"0x%016p 0x%016p ", GcdIo[i].BaseAddress, GcdIo[i].Length);

                if(GcdIo[i].DeviceHandle != NULL)
                {
                    HandleNumber = ConvertHandleToHandleIndex(GcdIo[i].DeviceHandle);
                    ShellPrintEx(-1, -1, L"%x", HandleNumber);
                }
                    
                Print(L"\n");
            }
        }
    }
	return EFI_SUCCESS;
}


