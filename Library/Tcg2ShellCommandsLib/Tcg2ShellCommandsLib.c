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
#include <Tcg2ShellCommandsLib.h>
#include <Tcg2ShellCommandsLibStrDefs.h>

static CHAR16 gTcg2ShellCommandsLib[] = L"Tcg2ShellCommandsLib";
EFI_HANDLE gTcg2CommandsLibHiiHandle = NULL;
extern EFI_HANDLE   mDpHiiHandle;

static EFI_GUID gTcg2ShellCommandsLibGuid = {0x513f8dea, 0x5dd2, 0x4081, {0xba, 0x7a, 0xd3, 0xce, 0x72, 0x95, 0x2f, 0x67}};

CONST CHAR16* EFIAPI ShellCommandsGetFileName(VOID)
{
    return (gTcg2ShellCommandsLib);
}

EFI_STATUS EFIAPI Tcg2ShellCommandsLibConstructor(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    gTcg2CommandsLibHiiHandle = HiiAddPackages (&gTcg2ShellCommandsLibGuid, ImageHandle, Tcg2ShellCommandsLibStrings, NULL);
    if(gTcg2CommandsLibHiiHandle != NULL)
    {
        ShellCommandRegisterCommandName(L"DumpEtet",        DumpEtet,       ShellCommandsGetFileName, 0, L"", FALSE, gTcg2CommandsLibHiiHandle, STRING_TOKEN(STR_DUMP_ETET));
        ShellCommandRegisterCommandName(L"Tpm2Capability",  Tpm2Capability, ShellCommandsGetFileName, 0, L"", FALSE, gTcg2CommandsLibHiiHandle, STRING_TOKEN(STR_DUMP_ETET));
        ShellCommandRegisterCommandName(L"Tpm2EfiInfo",     Tpm2EfiInfo,    ShellCommandsGetFileName, 0, L"", FALSE, gTcg2CommandsLibHiiHandle, STRING_TOKEN(STR_DUMP_ETET));
        ShellCommandRegisterCommandName(L"Tpm2EventLog",    Tpm2EventLog,   ShellCommandsGetFileName, 0, L"", FALSE, gTcg2CommandsLibHiiHandle, STRING_TOKEN(STR_DUMP_ETET));
    }
    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI Tcg2ShellCommandsLibDestructor (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    if (gTcg2CommandsLibHiiHandle != NULL)
    {
        HiiRemovePackages(gTcg2CommandsLibHiiHandle);
    }
    return (EFI_SUCCESS);
}
