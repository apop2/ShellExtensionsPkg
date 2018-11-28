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
#include <Library/HiiLib.h>
#include <Library/ShellCommandLib.h>
#include <ShellExtensionsLib.h>
#include <ShellExtensionsLibStrDefs.h>
static CHAR16 gShellExtensionsFileName[] = L"ShellExtensionsLib";
EFI_HANDLE gShellExtensionsLibHiiHandle = NULL;

static EFI_GUID gShellExtensionsGuid = { 0x656e26aa, 0xd5e9, 0x4963, { 0xb1, 0x55, 0x3f, 0xdb, 0x1c, 0x95, 0xac, 0x34 } };

CONST CHAR16* EFIAPI ShellExtensionsGetFileName(VOID)
{
    return (gShellExtensionsFileName);
}

EFI_STATUS EFIAPI ShellExtensionsLibConstructor(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    gShellExtensionsLibHiiHandle = HiiAddPackages (&gShellExtensionsGuid, ImageHandle, ShellExtensionsLibStrings, NULL);
    if(gShellExtensionsLibHiiHandle != NULL)
    {
        ShellCommandRegisterCommandName(L"DisplayGop",   ShellCommandDisplayGop,    ShellExtensionsGetFileName, 0, L"", FALSE, gShellExtensionsLibHiiHandle, STRING_TOKEN(STR_GET_DISPLAY_GOP_HELP));
        ShellCommandRegisterCommandName(L"DisplayEdid", ShellCommandDisplayEdid,    ShellExtensionsGetFileName, 0, L"", FALSE, gShellExtensionsLibHiiHandle, STRING_TOKEN(STR_GET_DISPLAY_EDID_HELP));

        ShellCommandRegisterCommandName(L"Msr",                 ShellCommandMsr,    ShellExtensionsGetFileName, 0, L"", FALSE, gShellExtensionsLibHiiHandle, STRING_TOKEN(STR_GET_MSR_HELP));
        ShellCommandRegisterCommandName(L"CpuId",             ShellCommandCpuId,    ShellExtensionsGetFileName, 0, L"", FALSE, gShellExtensionsLibHiiHandle, STRING_TOKEN(STR_GET_CPUID_HELP));
        ShellCommandRegisterCommandName(L"TestSpeaker", ShellCommandTestSpeaker,    ShellExtensionsGetFileName, 0, L"", FALSE, gShellExtensionsLibHiiHandle, STRING_TOKEN(STR_GET_TEST_SPEAKER_HELP));
        ShellCommandRegisterCommandName(L"BbsTable",       ShellCommandBbsTable,    ShellExtensionsGetFileName, 0, L"", FALSE, gShellExtensionsLibHiiHandle, STRING_TOKEN(STR_GET_BBS_TABLE_HELP));
        ShellCommandRegisterCommandName(L"ColorBar",    ShellCommandColorScreen,    ShellExtensionsGetFileName, 0, L"", FALSE, gShellExtensionsLibHiiHandle, STRING_TOKEN(STR_GET_COLORBAR_HELP));
        ShellCommandRegisterCommandName(L"IoMap",             ShellCommandIoMap,    ShellExtensionsGetFileName, 0, L"", FALSE, gShellExtensionsLibHiiHandle, STRING_TOKEN(STR_GET_IO_MAP_HELP));

        ShellCommandRegisterCommandName(L"DumpHobs",       ShellCommandDumpHobs,    ShellExtensionsGetFileName, 0, L"", FALSE, gShellExtensionsLibHiiHandle, STRING_TOKEN(STR_GET_DUMP_HOBS_HELP));
    }
    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI ShellExtensionsLibDestructor (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    if (gShellExtensionsLibHiiHandle != NULL)
    {
        HiiRemovePackages(gShellExtensionsLibHiiHandle);
    }
    return (EFI_SUCCESS);
}
