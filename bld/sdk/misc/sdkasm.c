/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <windows.h>
#include "sdkasm.h"

static DisAsmRtns       *CurRtns;

/*
 * RegisterRtns - register a set of formatting routines.  This function
 *              must be called for each DisAsmRtns structure before it is
 *              used
 */
void RegisterRtns( DisAsmRtns *rtns ) {
    rtns = rtns;
}

/*
 * MiscDoCode - cover function for the DoCode function of the disassembler
 */

void MiscDoCode( instruction *ins ,char is32, DisAsmRtns *rtns ) {

    if( CurRtns != NULL ) {
        MessageBox( NULL, "Disassembly Error!!", "", MB_OK );
        return;
    }
    CurRtns = rtns;
    DoCode( ins, is32 );
    CurRtns = NULL;
}

/*
 * MiscFormatIns - cover function for the FormatIns function of the
 *                 disassembler
 */

void MiscFormatIns(char *buf, instruction *ins, form_option optn,
                        DisAsmRtns *rtns ) {
    if( CurRtns != NULL ) {
        MessageBox( NULL, "Disassembly Error!!", "", MB_OK );
        return;
    }
    CurRtns = rtns;
    FormatIns( buf, ins, optn );
    CurRtns = NULL;
}

/*
 * Cover Functions for disassebler interface routines
 */

int_16 GetDataByte() {
    return( CurRtns->GetDataByte() );
} /* GetDataByte */

int_16 GetDataWord() {
    return( CurRtns->GetDataWord() );
} /* GetDataWord */

int_16 GetNextByte() {
    return( CurRtns->GetNextByte() );
} /* GetNextByte */

long GetDataLong() {
    return( CurRtns->GetDataLong() );
}

char EndOfSegment() {
    return( CurRtns->EndOfSegment() );
}

DWORD GetOffset() {
    return( CurRtns->GetOffset() );
}

char *ToStr( unsigned long value, uint_16 len, DWORD addr ) {
    return( CurRtns->ToStr( value, len, addr ) );
}

char *JmpLabel( unsigned long addr, DWORD off ) {
    return( CurRtns->JmpLabel( addr, off ) );
}

char *ToBrStr( unsigned long value, DWORD addr ) {
    return( CurRtns->ToBrStr( value, addr ) );
}

char *ToIndex( long value, unsigned long addr ) {
    return( CurRtns->ToIndex( value, addr ) );
}

char *ToSegStr( DWORD value, WORD seg, DWORD addr ) {
    return( CurRtns->ToSegStr( value, seg, addr ) );
}

char *GetWtkInsName( unsigned ins ) {
    return( CurRtns->GetWtkInsName( ins ) );
}

void DoWtk(void)
{
    CurRtns->DoWtk();
}

int IsWtk() {
    return( CurRtns->IsWtk() );
}

