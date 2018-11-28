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
#include <Protocol/GraphicsOutput.h>
#include <Protocol/EdidActive.h>
#include <Library/UefiLib.h>
#include <Library/ShellLib.h>
#include <Library/HandleParsingLib.h>
#include <Library/UefiBootServicesTableLib.h>   
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>


#define COLOR_GRAY_15       {0x26, 0x26, 0x26, 0x00}
#define COLOR_GRAY_40       {0x66, 0x66, 0x66, 0x00}
#define COLOR_WHITE_75      {0xC0, 0xC0, 0xC0, 0x00}
#define COLOR_WHITE_100     {0xFF, 0xFF, 0xFF, 0x00}
#define COLOR_YELLOW_75     {0x00, 0xC0, 0xC0, 0x00}
#define COLOR_YELLOW_100    {0x00, 0xFF, 0xFF, 0x00}
#define COLOR_CYAN_75       {0xC0, 0xC0, 0x00, 0x00}
#define COLOR_CYAN_100      {0xFF, 0xFF, 0x00, 0x00}
#define COLOR_GREEN_75      {0x00, 0xC0, 0x00, 0x00}
#define COLOR_MAGENTA_75    {0xC0, 0x00, 0xC0, 0x00}
#define COLOR_RED_75        {0x00, 0x00, 0xC0, 0x00}
#define COLOR_RED_100       {0x00, 0x00, 0xFF, 0x00}
#define COLOR_BLUE_75       {0xC0, 0x00, 0x00, 0x00}
#define COLOR_BLUE_100      {0xFF, 0x00, 0x00, 0x00}
#define COLOR_BLACK         {0x04, 0x04, 0x04, 0x00}

VOID DisplayPattern(EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop)
{
    UINTN BarWidth;
    UINTN BarHeight;

    UINTN StartHeight;
    UINTN PixelsPerYRamp;

    UINT32 Remainder = 0;
    UINTN i = 0;

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Pattern1Bars[] = { COLOR_GRAY_40, COLOR_WHITE_75, COLOR_YELLOW_75, COLOR_CYAN_75, COLOR_GREEN_75, COLOR_MAGENTA_75, COLOR_RED_75, COLOR_BLUE_75, COLOR_GRAY_40 };

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Pattern2Bars[] = { COLOR_CYAN_100, COLOR_WHITE_100, COLOR_WHITE_75, COLOR_WHITE_75, COLOR_WHITE_75, COLOR_WHITE_75, COLOR_WHITE_75, COLOR_WHITE_75, COLOR_BLUE_100 };

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Pattern3Yellow = COLOR_YELLOW_100;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Pattern3Red = COLOR_RED_100;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL PatternRamp = {0x00, 0x00, 0x00, 0x00};

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Pattern4Gray15 = COLOR_GRAY_15;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Pattern4Black = COLOR_BLACK;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Pattern4White = COLOR_WHITE_100;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Pattern4BlackP2 =  {0x02, 0x02, 0x02, 0x00};
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Pattern4BlackP4 =  {0x04, 0x04, 0x04, 0x00};

    // Pattern 1
    BarHeight = (UINTN)DivU64x32(MultU64x32(Gop->Mode->Info->VerticalResolution, 7), (UINT32)12);
    BarWidth = (UINTN)DivU64x32(Gop->Mode->Info->HorizontalResolution, 9);

    for(i = 0; i < (sizeof(Pattern1Bars)/sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)); i++)
    {
        Gop->Blt(   Gop, 
                    &(Pattern1Bars[i]), 
                    EfiBltVideoFill, 
                    0, 
                    0, 
                    (UINTN)MultU64x32(BarWidth, (UINT32)i),
                    0,
                    BarWidth,
                    BarHeight,
                    0);
    }


    // Pattern 2
    StartHeight = BarHeight;

    BarHeight = (UINTN)DivU64x32(Gop->Mode->Info->VerticalResolution, (UINT32)12);
    BarWidth = (UINTN)DivU64x32(Gop->Mode->Info->HorizontalResolution, 9);
    for(i = 0; i < (sizeof(Pattern2Bars)/sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)); i++)
    {
        Gop->Blt(   Gop, 
                    &(Pattern2Bars[i]), 
                    EfiBltVideoFill, 
                    0, 
                    0, 
                    (UINTN)MultU64x32(BarWidth, (UINT32)i),
                    StartHeight,
                    BarWidth,
                    BarHeight,
                    0);
    }

    // Pattern 3
    StartHeight = StartHeight + BarHeight;

    BarHeight = (UINTN)DivU64x32(Gop->Mode->Info->VerticalResolution, (UINT32)12);
    BarWidth = (UINTN)DivU64x32(Gop->Mode->Info->HorizontalResolution, 9);
    Gop->Blt(   Gop, 
                &(Pattern3Yellow), 
                EfiBltVideoFill, 
                0, 
                0, 
                0,
                StartHeight,
                BarWidth,
                BarHeight,
                0);

//    PixelsPerYRamp = (UINTN)DivU64x32((UINTN)MultU64x32(BarWidth, 7), 255);
    PixelsPerYRamp = (UINTN)DivU64x32Remainder( (UINTN)MultU64x32(BarWidth, 7), 256, &Remainder);

    for(i = 0; i <= 256; i++)
    {
        Gop->Blt(   Gop, 
                    &(PatternRamp), 
                    EfiBltVideoFill, 
                    0, 
                    0, 
                    Remainder + BarWidth + (UINTN)MultU64x32(PixelsPerYRamp, (UINT32)i),
                    StartHeight,
                    PixelsPerYRamp,
                    BarHeight,
                    0);


        PatternRamp.Red += 1;
        PatternRamp.Green += 1;
        PatternRamp.Blue += 1;
    }

    Gop->Blt(   Gop, 
                &(Pattern3Red), 
                EfiBltVideoFill, 
                0, 
                0, 
                (UINTN)MultU64x32(BarWidth, 8),
                StartHeight,
                BarWidth,
                BarHeight,
                0);

    // Pattern 4
    StartHeight = StartHeight + BarHeight;

    BarHeight = (UINTN)DivU64x32(MultU64x32(Gop->Mode->Info->VerticalResolution, 3), (UINT32)12);
    BarWidth = (UINTN)DivU64x32(Gop->Mode->Info->HorizontalResolution, 9);

    // 15% gray
    Gop->Blt(   Gop, 
                &(Pattern4Gray15), 
                EfiBltVideoFill, 
                0, 
                0, 
                0,
                StartHeight,
                BarWidth,
                BarHeight,
                0);

    // Black
    Gop->Blt(   Gop, 
                &(Pattern4Black), 
                EfiBltVideoFill, 
                0, 
                0, 
                BarWidth,
                StartHeight,
                (UINTN)DivU64x32(MultU64x32(BarWidth, 6), 2),
                BarHeight,
                0);

    // White
    Gop->Blt(   Gop, 
                &(Pattern4White), 
                EfiBltVideoFill, 
                0, 
                0, 
                BarWidth + (UINTN)DivU64x32(MultU64x32(BarWidth, 3), 2),
                StartHeight,
                (UINTN)MultU64x32(BarWidth, 2),
                BarHeight,
                0);

    // 0% black
    Gop->Blt(   Gop, 
                &(Pattern4Black), 
                EfiBltVideoFill, 
                0, 
                0, 
                BarWidth + (UINTN)DivU64x32(MultU64x32(BarWidth, 3), 2) + (UINTN)MultU64x32(BarWidth, 2),
                StartHeight,
                (UINTN)DivU64x32(MultU64x32(BarWidth, 5), 6),
                BarHeight,
                0);

    // -2% Black
    Gop->Blt(   Gop, 
                &(Pattern4Black), 
                EfiBltVideoFill, 
                0, 
                0, 
                BarWidth + (UINTN)DivU64x32(MultU64x32(BarWidth, 3), 2) + (UINTN)MultU64x32(BarWidth, 2) + (UINTN)DivU64x32(MultU64x32(BarWidth, 5), 6),
                StartHeight,
                (UINTN)DivU64x32(BarWidth, 3),
                BarHeight,
                0);

    // 0% Black
    Gop->Blt(   Gop, 
                &(Pattern4Black), 
                EfiBltVideoFill, 
                0, 
                0, 
                BarWidth + (UINTN)DivU64x32(MultU64x32(BarWidth, 3), 2) + (UINTN)MultU64x32(BarWidth, 2) + (UINTN)DivU64x32(MultU64x32(BarWidth, 5), 6) + (UINTN)DivU64x32(BarWidth, 3),
                StartHeight,
                (UINTN)DivU64x32(BarWidth, 3),
                BarHeight,
                0);

    // 2% Black
    Gop->Blt(   Gop, 
                &(Pattern4BlackP2), 
                EfiBltVideoFill, 
                0, 
                0, 
                BarWidth + (UINTN)DivU64x32(MultU64x32(BarWidth, 3), 2) + (UINTN)MultU64x32(BarWidth, 2) + (UINTN)DivU64x32(MultU64x32(BarWidth, 5), 6) + (UINTN)MultU64x32(DivU64x32(BarWidth, 3), 2),
                StartHeight,
                (UINTN)DivU64x32(BarWidth, 3),
                BarHeight,
                0);

    // 0% Black
    Gop->Blt(   Gop, 
                &(Pattern4Black), 
                EfiBltVideoFill, 
                0, 
                0, 
                BarWidth + (UINTN)DivU64x32(MultU64x32(BarWidth, 3), 2) + (UINTN)MultU64x32(BarWidth, 2) + (UINTN)DivU64x32(MultU64x32(BarWidth, 5), 6) + (UINTN)MultU64x32(DivU64x32(BarWidth, 3), 3),
                StartHeight,
                (UINTN)DivU64x32(BarWidth, 3),
                BarHeight,
                0);

    // 4% Black
    Gop->Blt(   Gop, 
                &(Pattern4BlackP4), 
                EfiBltVideoFill, 
                0, 
                0, 
                BarWidth + (UINTN)DivU64x32(MultU64x32(BarWidth, 3), 2) + (UINTN)MultU64x32(BarWidth, 2) + (UINTN)DivU64x32(MultU64x32(BarWidth, 5), 6) + (UINTN)MultU64x32(DivU64x32(BarWidth, 3), 4),
                StartHeight,
                (UINTN)DivU64x32(BarWidth, 3),
                BarHeight,
                0);

    // 0% Black
    Gop->Blt(   Gop, 
                &(Pattern4Black), 
                EfiBltVideoFill, 
                0, 
                0, 
                BarWidth + (UINTN)DivU64x32(MultU64x32(BarWidth, 3), 2) + (UINTN)MultU64x32(BarWidth, 2) + (UINTN)DivU64x32(MultU64x32(BarWidth, 5), 6) + (UINTN)MultU64x32(DivU64x32(BarWidth, 3), 5),
                StartHeight,
                BarWidth,
                BarHeight,
                0);

    // 15% Gray
    Gop->Blt(   Gop, 
                &(Pattern4Gray15), 
                EfiBltVideoFill, 
                0, 
                0, 
                (UINTN)MultU64x32(BarWidth, 8),
                StartHeight,
                BarWidth,
                BarHeight,
                0);
}

SHELL_STATUS EFIAPI ShellCommandColorScreen(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;

	EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop = NULL;
    UINTN i = 0;

    EFI_HANDLE *HandleBuffer = NULL;
    UINTN HandleCount = 0;

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Buffer = {0,0,0,0};

    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiGraphicsOutputProtocolGuid, NULL, &HandleCount, &HandleBuffer);
    if(!EFI_ERROR(Status))
    {
        for(i = 0; i < HandleCount; i++)
        {
            Status = gBS->HandleProtocol(HandleBuffer[i], &gEfiGraphicsOutputProtocolGuid, &Gop);
            if(!EFI_ERROR(Status))
            {
                DisplayPattern(Gop);
            }
        }

        //wait for a keypress
        Status = ShellPromptForResponse(ShellPromptResponseTypeEnterContinue, L"Press any key to continue", NULL);

        for(i = 0; i < HandleCount; i++)
        {
            Status = gBS->HandleProtocol(HandleBuffer[i], &gEfiGraphicsOutputProtocolGuid, &Gop);
            if(!EFI_ERROR(Status))
            {
                Gop->Blt(   Gop,
                            &Buffer,
                            EfiBltVideoFill,
                            0,
                            0,
                            0,
                            0,
                            Gop->Mode->Info->HorizontalResolution,
                            Gop->Mode->Info->VerticalResolution,
                            0);
            }
        }

        gBS->FreePool(HandleBuffer);
    }
    return EFI_SUCCESS;
}