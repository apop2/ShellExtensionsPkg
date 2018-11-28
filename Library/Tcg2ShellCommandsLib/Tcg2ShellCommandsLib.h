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
#ifndef _TCG_2_SHELL_COMANDS_LIB_H
#define _TCG_2_SHELL_COMANDS_LIB_H
SHELL_STATUS EFIAPI DumpEtet(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable);
SHELL_STATUS EFIAPI FakeEtet(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable);
SHELL_STATUS EFIAPI Tpm2Capability(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable);
SHELL_STATUS EFIAPI Tpm2EfiInfo(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable);
SHELL_STATUS EFIAPI Tpm2EventLog(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable);


#endif