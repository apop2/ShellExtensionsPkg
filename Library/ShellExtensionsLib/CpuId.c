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
#include <Library/ShellLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>


extern EFI_HANDLE gShellExtensionsLibHiiHandle;

STATIC CONST SHELL_PARAM_ITEM ParamList[] = {
  {NULL, TypeMax}
  };



BOOLEAN IsDisplayable(UINT8 Character)
{
    if(Character >= 0x20 && Character <= 0x7E)
        return TRUE;
    return FALSE;
}

BOOLEAN IsAlphaNumberic(UINT8 Character)
{
    if( Character >= '0' && Character <= '9')
        return TRUE;
    else if(Character >= 'A' && Character <= 'Z')
        return TRUE;
    else if(Character >= 'a' && Character <= 'z')
        return TRUE;
    
    return FALSE;
}

SHELL_STATUS EFIAPI ShellCommandCpuId(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;
    LIST_ENTRY *Package;
    CHAR16 *ProblemParam;
    CONST CHAR16 *Temp;

    UINT64 Input = (UINTN)-1;
    UINT64 SubInput = (UINTN)-1;

    UINT32 Eax;
    UINT32 Ebx;
    UINT32 Ecx;
    UINT32 Edx;
    UINTN i = 0;

    Status = ShellCommandLineParse(ParamList, &Package, &ProblemParam, TRUE);
    if(EFI_ERROR(Status))
    {
        Print(L"Problem Param! %s\n", ProblemParam);
        gBS->FreePool(ProblemParam);
        return SHELL_INVALID_PARAMETER;
    }
    if(ShellCommandLineGetCount(Package) == 1)
    {
        ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_GEN_TOO_FEW), gShellExtensionsLibHiiHandle, L"cpuid");  
        return SHELL_INVALID_PARAMETER;
    }
    if(ShellCommandLineGetCount(Package) > 3 ) 
    {
        ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_GEN_TOO_MANY), gShellExtensionsLibHiiHandle, L"cpuid");  
        return SHELL_INVALID_PARAMETER;
    }

    Temp = ShellCommandLineGetRawValue(Package, 1);
    if(Temp != NULL) 
    {
        Status = ShellConvertStringToUint64(Temp, &Input, TRUE, TRUE);
    }

    Temp = ShellCommandLineGetRawValue(Package, 2);
    if(Temp != NULL) 
    {
        Status = ShellConvertStringToUint64(Temp, &SubInput, TRUE, TRUE);
    }

    if(SubInput != (UINTN)-1)
    {
        AsmCpuidEx((UINT32)Input, (UINT32)SubInput, &Eax, &Ebx, &Ecx, &Edx);
        ShellPrintEx(-1,-1, L"CPUID EAX=0x%08x ECX=0x%08x\n", Input, SubInput);
    }
    else
    {
        AsmCpuid((UINT32)Input, &Eax, &Ebx, &Ecx, &Edx);
        ShellPrintEx(-1,-1, L"CPUID EAX=0x%08x\n", Input);
    }
    ShellPrintEx(-1, -1, L"  Eax = 0x%08x    \"", Eax);
    for(i = 0; i < 4; i++)
    {
        if( IsDisplayable(*(((UINT8*)&Eax) + i)) )
            ShellPrintEx(-1, -1, L"%c", *(((UINT8*)&Eax) + i) );
        else
            ShellPrintEx(-1, -1, L".");
    }
    ShellPrintEx(-1, -1, L"\"\n");

    ShellPrintEx(-1, -1, L"  Ebx = 0x%08x    \"", Ebx);
    for(i = 0; i < 4; i++)
    {
        if(IsDisplayable (*(((UINT8*)&Ebx) + i) ) )
            ShellPrintEx(-1, -1, L"%c", *(((UINT8*)&Ebx) + i));
        else
            ShellPrintEx(-1, -1, L".");
    }
    ShellPrintEx(-1, -1, L"\"\n");

    ShellPrintEx(-1, -1, L"  Ecx = 0x%08x    \"", Ecx);
    for(i = 0; i < 4; i++)
    {
        if( IsDisplayable(*(((UINT8*)&Ecx) + i)) )
            ShellPrintEx(-1, -1, L"%c", *(((UINT8*)&Ecx) + i));
        else
            ShellPrintEx(-1, -1, L".");
    }
    ShellPrintEx(-1, -1, L"\"\n");

    ShellPrintEx(-1, -1, L"  Edx = 0x%08x    \"", Edx);
    for(i = 0; i < 4; i++)
    {
        if( IsDisplayable(*(((UINT8*)&Edx) + i)) )
            ShellPrintEx(-1, -1, L"%c", *(((UINT8*)&Edx) + i));
        else
            ShellPrintEx(-1, -1, L".");
    }
    ShellPrintEx(-1, -1, L"\"\n");
    return EFI_SUCCESS;
}