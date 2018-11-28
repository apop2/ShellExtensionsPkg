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
#ifndef _SHELL_EXTENSIONS_LIB_H_
#define _SHELL_EXTENSIONS_LIB_H_
SHELL_STATUS EFIAPI ShellCommandDisplayEdid(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable);
SHELL_STATUS EFIAPI ShellCommandDisplayGop(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable);
SHELL_STATUS EFIAPI ShellCommandMsr(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable);
SHELL_STATUS EFIAPI ShellCommandCpuId(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable);
SHELL_STATUS EFIAPI ShellCommandTestSpeaker(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable);
SHELL_STATUS EFIAPI ShellCommandBbsTable(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable);
SHELL_STATUS EFIAPI ShellCommandColorScreen(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable);
SHELL_STATUS EFIAPI ShellCommandIoMap(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable);
SHELL_STATUS EFIAPI ShellCommandDumpHobs(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable);

extern EFI_HANDLE gShellExtensionsLibHiiHandle;

#endif