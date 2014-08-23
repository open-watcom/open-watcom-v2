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
* Description:  Disassembler interface.
*
****************************************************************************/


#ifndef _SDKASM_H_INCLUDED
#define _SDKASM_H_INCLUDED

#include "standard.h"
#include "deasm.h"

typedef struct disasmrtns {
    int_16              (*GetDataByte)( void );
    int_16              (*GetDataWord)( void );
    int_16              (*GetNextByte)( void );
    long                (*GetDataLong)( void );
    char                (*EndOfSegment)( void );
    DWORD               (*GetOffset)( void );
    void                (*DoWtk)( void );
    int                 (*IsWtk)( void );
    char                *(*ToStr)( unsigned long value, uint_16 len, DWORD addr );
    char                *(*JmpLabel)( unsigned long addr, DWORD off );
    char                *(*ToBrStr)( unsigned long value, DWORD addr );
    char                *(*ToIndex)( unsigned long value, unsigned long addr );
    char                *(*ToSegStr)( DWORD value, WORD seg, DWORD addr );
    char                *(*GetWtkInsName)( unsigned ins );
} DisAsmRtns;

void    RegisterRtns( DisAsmRtns *rtns );
void    MiscDoCode( instruction *, bool, DisAsmRtns * );
void    MiscFormatIns( char *, instruction *, form_option, DisAsmRtns * );

#endif /* _SDKASM_H_INCLUDED */
