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
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>


#define GET_POOL_HOB_DATA(HobStart) \
  (VOID *)(*(UINT8 **)&(HobStart) + sizeof (EFI_HOB_TYPE_MEMORY_POOL))

#define GET_POOL_HOB_DATA_SIZE(HobStart) \
  (UINT16)(GET_HOB_LENGTH (HobStart) - sizeof (EFI_HOB_TYPE_MEMORY_POOL))

extern EFI_HANDLE gShellExtensionsLibHiiHandle;


STATIC CONST SHELL_PARAM_ITEM ParamList[] = {
  {L"-t", TypeValue},
  {L"-g", TypeValue},
  {L"-r", TypeFlag},
  {L"-raw", TypeFlag},
  {L"-v", TypeFlag},
  {L"-verbose", TypeFlag},
  {NULL, TypeMax}
  };

static UINTN mGuidDataLen[] = {8, 4, 4, 4, 12};
static BOOLEAN IsValidGuidString(IN CONST CHAR16 *String)
{
    CONST CHAR16  *Walker;
    CONST CHAR16  *PrevWalker;
    UINTN         Index;

    if (String == NULL)
        return FALSE;

    Walker      = String;
    PrevWalker  = String;
    Index       = 0;

    while (Walker != NULL && *Walker != CHAR_NULL) 
    {
        if ( (*Walker >= '0' && *Walker <= '9') || (*Walker >= 'a' && *Walker <= 'f') || (*Walker >= 'A' && *Walker <= 'F') ) 
            Walker++;
        else 
        {
            if (*Walker == L'-' && (((UINTN)Walker - (UINTN)PrevWalker) / sizeof (CHAR16)) == mGuidDataLen[Index]) 
            {
                Walker++;
                PrevWalker = Walker;
                Index++;
            } 
            else 
                return FALSE;
        }
    }

    if ((((UINTN)Walker - (UINTN)PrevWalker) / sizeof (CHAR16)) == mGuidDataLen[Index]) 
        return TRUE;
    else 
        return FALSE;
}

static UINTN HexCharToDecimal(IN CHAR16 Char)
{
    if (Char >= '0' && Char <= '9') 
        return Char - L'0';
    else if (Char >= 'a' && Char <= 'f')
        return Char - L'a' + 10;
    else
        return Char - L'A' + 10;
}

static EFI_STATUS ConvertStrToGuid(IN  CONST CHAR16 *String, OUT GUID *Guid)
{
    CONST CHAR16  *Walker;
    UINT8         TempValue;
    UINTN         Index;

    if (String == NULL || !IsValidGuidString (String))
        return EFI_UNSUPPORTED;

    Index = 0;

    Walker = String;
    Guid->Data1 = (UINT32)StrHexToUint64 (Walker);

    Walker += 9;
    Guid->Data2 = (UINT16)StrHexToUint64 (Walker);

    Walker += 5;
    Guid->Data3 = (UINT16)StrHexToUint64 (Walker);

    Walker += 5;
    while (Walker != NULL && *Walker != CHAR_NULL) 
    {
        if (*Walker == L'-') 
        {
            Walker++;
        } 
        else 
        {
            TempValue = (UINT8)HexCharToDecimal (*Walker);
            TempValue = (UINT8)LShiftU64 (TempValue, 4);
            Walker++;

            TempValue += (UINT8)HexCharToDecimal (*Walker);
            Walker++;

            Guid->Data4[Index] = TempValue;
            Index++;
        }
    }
    return EFI_SUCCESS;
}

SHELL_STATUS EFIAPI PrintRawHob(EFI_PEI_HOB_POINTERS Hob)
{
    UINT16 HobSize = GET_HOB_LENGTH(Hob.Raw);
    UINT8 *HobData = Hob.Raw;
    UINT16 i = 0;
    for(i = 0; i < HobSize; i++)
    {
        if( i == 0)
            ShellPrintEx(-1, -1, L"  ");
        else if( i != 0 && (i % 16) == 0)
            ShellPrintEx(-1, -1, L"\n  ");
        else if(i != 0 && (i % 8) == 0)
            ShellPrintEx(-1, -1, L"-");
        else
            ShellPrintEx(-1, -1, L" ");
        ShellPrintEx(-1, -1, L"%02x", HobData[i]);
    }
    ShellPrintEx(-1, -1, L"\n");

    return EFI_SUCCESS;
}

SHELL_STATUS EFIAPI PrintSingleHob(BOOLEAN VerboseFlag, BOOLEAN RawFlag, EFI_PEI_HOB_POINTERS Hob)
{
    switch( GET_HOB_TYPE(Hob) )
    {
        case EFI_HOB_TYPE_HANDOFF:
            ShellPrintEx(-1, -1, L"EFI_HOB_HANDOFF_INFO_TABLE\n");
            if(RawFlag)
            {
                PrintRawHob(Hob);
            }
            else
            {
                ShellPrintEx(-1, -1, L"  Version = 0x%04x\n  BootMode = 0x%04x\n  EfiMemoryTop = 0x%016p\n  EfiMemoryBottom = 0x%016p\n, EfiFreeMemoryTop = 0x%016p\n  EfiFreeMemoryBottom = 0x%016p\n  EfiEndOfHobList = 0x%016p\n", Hob.HandoffInformationTable->Version, Hob.HandoffInformationTable->BootMode, Hob.HandoffInformationTable->EfiMemoryTop, Hob.HandoffInformationTable->EfiMemoryBottom, Hob.HandoffInformationTable->EfiFreeMemoryTop, Hob.HandoffInformationTable->EfiFreeMemoryBottom, Hob.HandoffInformationTable->EfiEndOfHobList);
            }
            break;

        case EFI_HOB_TYPE_MEMORY_ALLOCATION:
            ShellPrintEx(-1, -1, L"EFI_HOB_MEMORY_ALLOCATION_HEADER\n");
            if(RawFlag)
            {
                PrintRawHob(Hob);
            }
            else
            {
                ShellPrintEx(-1, -1, L"  Allocation Guid = %g\n  MemoryBaseAddres = 0x%016p\n  MemoryLength = 0x%016p\n  MemoryType = 0x%04x\n", Hob.MemoryAllocation->AllocDescriptor.Name, Hob.MemoryAllocation->AllocDescriptor.MemoryBaseAddress, Hob.MemoryAllocation->AllocDescriptor.MemoryLength, Hob.MemoryAllocation->AllocDescriptor.MemoryType);
            }
            break;

        case EFI_HOB_TYPE_RESOURCE_DESCRIPTOR:
            ShellPrintEx(-1, -1, L"EFI_HOB_RESOURCE_DESCRIPTOR\n");
            if(RawFlag)
            {
                PrintRawHob(Hob);
            }
            else
            {
                ShellPrintEx(-1, -1, L"  Owner = %g\n  ResourceType = 0x%04x\n  ResourceAttribute = 0x%04x\n  PhysicalStart = 0x%016p\n  ResourceLength = 0x%016p\n", Hob.ResourceDescriptor->Owner, Hob.ResourceDescriptor->ResourceType, Hob.ResourceDescriptor->ResourceAttribute, Hob.ResourceDescriptor->PhysicalStart, Hob.ResourceDescriptor->ResourceLength);
            }
            break;

        case EFI_HOB_TYPE_GUID_EXTENSION:
        {
            UINT16 GuidHobSize = GET_GUID_HOB_DATA_SIZE(Hob.Guid);
            UINT8 *GuidHobData = GET_GUID_HOB_DATA(Hob.Guid);
            UINT16 i = 0;

            ShellPrintEx(-1, -1, L"EFI_HOB_GUID_TYPE\n");
            if(RawFlag)
            {
                PrintRawHob(Hob);
            }
            else
            {
                ShellPrintEx(-1, -1, L"  Guid = %g\n", Hob.Guid->Name);
                if(VerboseFlag)
                {
                    for(i = 0; i < GuidHobSize; i++)
                    {   
                        if( i == 0)
                            ShellPrintEx(-1, -1, L"  ");
                        else if( i != 0 && (i % 16) == 0)
                            ShellPrintEx(-1, -1, L"\n  ");
                        else if(i != 0 && (i % 8) == 0)
                            ShellPrintEx(-1, -1, L"-");
                        else
                            ShellPrintEx(-1, -1, L" ");
                        ShellPrintEx(-1, -1, L"%02x", GuidHobData[i]);
                    }
                    ShellPrintEx(-1, -1, L"\n");
                }
            }
            break;
        }

        case EFI_HOB_TYPE_FV:
            ShellPrintEx(-1, -1, L"EFI_HOB_FIRMWARE_VOLUME\n");
            if(RawFlag)
            {
                PrintRawHob(Hob);
            }
            else
            {
                ShellPrintEx(-1, -1, L"  BaseAddress = 0x%016p\n  Length = 0x%016p\n", Hob.FirmwareVolume->BaseAddress, Hob.FirmwareVolume->Length );
            }
            break;

        case EFI_HOB_TYPE_CPU:
            ShellPrintEx(-1, -1, L"EFI_HOB_CPU\n");
            if(RawFlag)
            {
                PrintRawHob(Hob);
            }
            else
            {
                ShellPrintEx(-1, -1, L"  SizeOfMemorySpace = 0x%02x\n  SizeOfIoSpace = 0x%02x\n", Hob.Cpu->SizeOfMemorySpace, Hob.Cpu->SizeOfIoSpace);
            }
            break;


        case EFI_HOB_TYPE_MEMORY_POOL:
            ShellPrintEx(-1, -1, L"EFI_HOB_MEMORY_POOL\n");
            if(RawFlag)
            {
                PrintRawHob(Hob);
            }
            else
            {
                ShellPrintEx(-1, -1, L"  Address = 0x%016p\n  Size = 0x%04x\n",  GET_POOL_HOB_DATA(Hob.Pool), GET_POOL_HOB_DATA_SIZE(Hob.Pool));
            }
            break;

        case EFI_HOB_TYPE_FV2:
            ShellPrintEx(-1, -1, L"EFI_HOB_FIRMWARE_VOLUME2\n");
            if(RawFlag)
            {
                PrintRawHob(Hob);
            }
            else
            {
                ShellPrintEx(-1, -1, L"  BaseAddress = 0x%016p\n  Length = 0x%016p\n  FvName = %g\n  FileName = %g", Hob.FirmwareVolume2->BaseAddress, Hob.FirmwareVolume2->Length, Hob.FirmwareVolume2->FvName, Hob.FirmwareVolume2->FileName);
            }
            break;


        case EFI_HOB_TYPE_UEFI_CAPSULE:
            ShellPrintEx(-1, -1, L"EFI_HOB_UEFI_CAPSULE\n");
            if(RawFlag)
            {
                PrintRawHob(Hob);
            }
            else
            {
                ShellPrintEx(-1, -1, L"  BaseAddress = 0x%016p\n  Length = 0x%016p\n", Hob.Capsule->BaseAddress, Hob.Capsule->Length);
            }
            break;

        default:
            ShellPrintEx(-1, -1, L"Unknown Hob - %d\n",  GET_HOB_TYPE(Hob));
            if(VerboseFlag)
            {
                PrintRawHob(Hob);
            }
            break;
    }
    return EFI_SUCCESS;
}


SHELL_STATUS EFIAPI DumpSpecificHobTypes(BOOLEAN VerboseFlag, BOOLEAN RawFlag, UINTN HobType)
{
	EFI_STATUS Status;
    EFI_PEI_HOB_POINTERS Hob;

    Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID**)&Hob.Raw);
    if(EFI_ERROR(Status))
    {
        ShellPrintEx(-1, -1, L"Hob System Configuration Table was not found\n");
    }
    else
    {
        for( ; !END_OF_HOB_LIST(Hob); Hob.Raw = GET_NEXT_HOB(Hob) )
        {

            if( GET_HOB_TYPE(Hob) == HobType)
            {
                PrintSingleHob(VerboseFlag, RawFlag, Hob);
            }
        }
    }
    return Status;
}

SHELL_STATUS DumpSpecificHobGuid(BOOLEAN VerboseFlag, BOOLEAN RawFlag, EFI_GUID *HobGuid)
{
	EFI_STATUS Status;
    EFI_PEI_HOB_POINTERS Hob;

    Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID**)&Hob.Raw);
    if(EFI_ERROR(Status))
    {
        ShellPrintEx(-1, -1, L"Hob System Configuration Table was not found\n");
    }
    else
    {
        for( ; !END_OF_HOB_LIST(Hob); Hob.Raw = GET_NEXT_HOB(Hob) )
        {

            if( GET_HOB_TYPE(Hob) == EFI_HOB_TYPE_GUID_EXTENSION)
            {
                if( CompareMem(&(Hob.Guid->Name), HobGuid, sizeof(EFI_GUID)) == 0)
                {
                    PrintSingleHob(VerboseFlag, RawFlag, Hob);
                }
            }
        }
    }
    return Status;
}

SHELL_STATUS EFIAPI DumpAllHobs(BOOLEAN VerboseFlag, BOOLEAN RawFlag)
{
	EFI_STATUS Status;
    EFI_PEI_HOB_POINTERS Hob;

    Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID**)&Hob.Raw);
    if(EFI_ERROR(Status))
    {
        ShellPrintEx(-1, -1, L"Hob System Configuration Table was not found\n");
    }
    else
    {
        for( ; !END_OF_HOB_LIST(Hob); Hob.Raw = GET_NEXT_HOB(Hob) )
        {
            PrintSingleHob(VerboseFlag, RawFlag, Hob);
        }
        ShellPrintEx(-1, -1, L"\n");
    }
	return Status;
}

SHELL_STATUS EFIAPI ShellCommandDumpHobs(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_STATUS Status;

    LIST_ENTRY *Package;
    CHAR16 *ProblemParam;

    CONST CHAR16 *HobTypeStr;
    UINTN HobType = 0;

    CONST CHAR16 *GuidTypeStr;
    EFI_GUID HobGuid;
    
    BOOLEAN VerboseFlag = FALSE;
    BOOLEAN RawFlag = FALSE;

    Status = ShellCommandLineParse (ParamList, &Package, &ProblemParam, TRUE);
    if(!EFI_ERROR(Status))
    {
        VerboseFlag = (BOOLEAN)(ShellCommandLineGetFlag (Package, L"-v") || ShellCommandLineGetFlag (Package, L"-verbose"));

        RawFlag = (BOOLEAN)(ShellCommandLineGetFlag (Package, L"-r") || ShellCommandLineGetFlag (Package, L"-raw"));
        
        if( ShellCommandLineGetFlag (Package, L"-t") )
        {
            HobTypeStr = ShellCommandLineGetValue(Package, L"-t");
            if( HobTypeStr == NULL)
            {
                ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_NO_VALUE), gShellExtensionsLibHiiHandle, L"DumpHobs", L"-t");
            }
            else
            {   
                HobType = ShellStrToUintn(HobTypeStr);
                DumpSpecificHobTypes(VerboseFlag, RawFlag, HobType);
            }
        }
        else if( ShellCommandLineGetFlag (Package, L"-g") )
        {
            GuidTypeStr = ShellCommandLineGetValue(Package, L"-g");
            if( GuidTypeStr == NULL)
            {
                ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_NO_VALUE), gShellExtensionsLibHiiHandle, L"DumpHobs", L"-g");
            }
            else
            {   
                Status = ConvertStrToGuid(GuidTypeStr, &HobGuid);
                ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_DUMPHOBS_OUTPUT_GUID_HEADER), gShellExtensionsLibHiiHandle, HobGuid);
                DumpSpecificHobGuid(VerboseFlag, RawFlag, &HobGuid);
            }
        }
        else
        {
            DumpAllHobs(VerboseFlag, RawFlag);
        }
        ShellCommandLineFreeVarList (Package);
    }
    return Status;
}


