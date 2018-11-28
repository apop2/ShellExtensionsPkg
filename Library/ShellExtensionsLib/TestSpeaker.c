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
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IoLib.h>


SHELL_STATUS EFIAPI ShellCommandTestSpeaker(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    UINT16 freq[]  = { 0x9121, 0x8609, 0x8126, 0x7670, 0x7239, 0x6833, 0x6449, 0x6087, 0x5746, 0x5423, 0x5119, 0x4831, 0x4560, 0x4304, 0x4063, 0x3834, 0x3619, 0x3416, 0x3224, 0x3043, 0x2873, 0x2711, 0x2559, 0x2415, 0x2280, 0x2152, 0x2031, 0x1917, 0x1809, 0x1715, 0x1612, 0x1521, 0x1436, 0x1355, 0x1292, 0x1207, 0x1140 };
    UINT16 number = sizeof(freq) / sizeof(int);
    UINT16 i = 0;

    IoWrite8(0x43, 0xb6); 
    for( i = 0; i < number; i++)
    {
        IoWrite8(0x42, (unsigned char)freq[i]);
        IoWrite8(0x42, (unsigned char)(freq[i] >> 8) );

        IoWrite8(0x61, IoRead8(0x61)|3);
        gBS->Stall(1000);
        IoWrite8(0x61,IoRead8(0x61)&0xfc);
    }
    return EFI_SUCCESS;
}