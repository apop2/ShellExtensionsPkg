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

#include <Library/BaseLib.h>

extern EFI_HANDLE gShellExtensionsLibHiiHandle;

STATIC CONST SHELL_PARAM_ITEM ParamList[] = {
  {L"-w", TypeValue},
  {NULL, TypeMax}
  };

SHELL_STATUS EFIAPI ShellCommandMsr(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;
    LIST_ENTRY *Package;
    CHAR16 *ProblemParam;
    CONST CHAR16 *Temp;
    UINT64 Msr = (UINTN)-1;
    UINT64 Value = (UINTN)-1;
    UINT64 Width = 64;

    Status = ShellCommandLineParse(ParamList, &Package, &ProblemParam, TRUE);
    if(EFI_ERROR(Status))
    {
        Print(L"Problem Param! %s\n", ProblemParam);
        gBS->FreePool(ProblemParam);
        return SHELL_INVALID_PARAMETER;
    }

    if(ShellCommandLineGetCount(Package) == 1)
    {
        ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_GEN_TOO_FEW), gShellExtensionsLibHiiHandle, L"msr");  
        return SHELL_INVALID_PARAMETER;
    }

    if(ShellCommandLineGetCount(Package) > 3) 
    {
        ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_GEN_TOO_MANY), gShellExtensionsLibHiiHandle, L"msr");  
        return SHELL_INVALID_PARAMETER;
    }

    Temp = ShellCommandLineGetRawValue(Package, 1);
    if(Temp != NULL) 
    {
        Status = ShellConvertStringToUint64(Temp, &Msr, TRUE, TRUE);
    }

    Temp = ShellCommandLineGetRawValue(Package, 2);
    if(Temp != NULL) 
    {
        Status = ShellConvertStringToUint64(Temp, &Value, TRUE, TRUE);
    }

    Temp = ShellCommandLineGetValue(Package, L"-w");
    if (Temp != NULL) 
    {
        Status = ShellConvertStringToUint64(Temp, &Width, FALSE, TRUE);
    }

    if(Msr != (UINTN)-1)
    {
        if(Width == 64)
        {
            if(Value != (UINTN)-1)
            {
                AsmWriteMsr64((UINT32)Msr, (UINT64)Value);
            }
            else
            {
                ShellPrintEx(-1, -1, L" 0x%016p\n", AsmReadMsr64((UINT32)Msr));
            }
        }
        else if(Width == 32)
        {        
            if(Value != (UINTN)-1)
            {
                AsmWriteMsr32((UINT32)Msr, (UINT32)Value);
            }
            else
            {
                ShellPrintEx(-1, -1, L" 0x%08x\n", AsmReadMsr64((UINT32)Msr));
            }
        }
        else
            ShellPrintEx(-1, -1, L"Unknown width - %d\n", Width);
    }

    return EFI_SUCCESS;
}
