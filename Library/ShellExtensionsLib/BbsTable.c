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
#include <Library/ShellLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/HandleParsingLib.h>
#include <Protocol/LegacyBios.h>

#include <Library/BaseLib.h>

extern EFI_HANDLE gShellExtensionsLibHiiHandle;


SHELL_STATUS EFIAPI ShellCommandBbsTable(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;
    EFI_LEGACY_BIOS_PROTOCOL *LegacyBios = NULL;

    HDD_INFO *HddInfo = NULL;
    UINT16 HddCount = 0;
    BBS_TABLE *BbsTable = NULL;
    UINT16 BbsCount = 0;
    UINTN i = 0;


    Status = gBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid, NULL, (VOID**)&LegacyBios);
    if(EFI_ERROR(Status))
    {
        ShellPrintEx(-1, -1, L"Unable to locate the Legacy Bios Protocol\n");
    }
    else
    {
        Status = LegacyBios->GetBbsInfo(LegacyBios, &HddCount, &HddInfo, &BbsCount, &BbsTable);
        if(EFI_ERROR(Status))
        {
            ShellPrintEx(-1, -1, L"Call to GetBbsInfo failed with %r\n", Status);
        }
        else
        {
            for(i = 0; i < BbsCount; i++)
            {
                if(BbsTable[i].BootPriority != BBS_IGNORE_ENTRY)
                {
                    CHAR8 *MfgStr = (CHAR8*)(UINTN)((BbsTable[i].MfgStringSegment << 4) + BbsTable[i].MfgStringOffset);
                    CHAR8 *DescStr = (CHAR8*)(UINTN)((BbsTable[i].DescStringSegment << 4) + BbsTable[i].DescStringOffset);

                    ShellPrintEx(-1, -1, L"Entry %d\n", i);
                    ShellPrintEx(-1, -1, L"  Priority: 0x%04x B%03d/D%02d/F%01d\n", BbsTable[i].BootPriority, BbsTable[i].Bus, BbsTable[i].Device, BbsTable[i].Function);
                    ShellPrintEx(-1, -1, L"  Class: 0x%02x Subclass: 0x%02x ", BbsTable[i].Class, BbsTable[i].SubClass);
                        
                    switch(BbsTable[i].DeviceType)
                    {
                        case BBS_FLOPPY:
                            ShellPrintEx(-1, -1, L"Floppy ");
                            break;

                        case BBS_HARDDISK:
                            ShellPrintEx(-1, -1, L"Hard Drive ");
                            break;

                        case BBS_CDROM:
                            ShellPrintEx(-1, -1, L"CDROM ");
                            break;

                        case BBS_PCMCIA:
                            ShellPrintEx(-1, -1, L"PCMCIA ");
                            break;

                        case BBS_USB:
                            ShellPrintEx(-1, -1, L"USB ");
                            break;

                        case BBS_EMBED_NETWORK:
                            ShellPrintEx(-1, -1, L"Embedded NIC ");
                            break;

                        case BBS_BEV_DEVICE:
                            ShellPrintEx(-1, -1, L"BEV device ");
                            break;

                        default:
                            ShellPrintEx(-1, -1, L"Unknown type ");
                            break;
                    }
                    if(MfgStr != NULL && MfgStr[0] != '\0')
                        ShellPrintEx(-1, -1, L"Mfg: \"%a\" ", MfgStr);

                    if(DescStr != NULL && DescStr[0] != '\0')
                        ShellPrintEx(-1, -1, L"Description: \"%a\" ", DescStr);

                    ShellPrintEx(-1, -1, L"\n\n");
                }
            }
        }
    }

    return EFI_SUCCESS;
}
