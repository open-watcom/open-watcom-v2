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


#include "intrptr.h"
#include "os.h"
#include "miniproc.h"


typedef struct {
    WORD    limit;
    LONG    base;
} baseoffset;

typedef struct {
    WORD    loffs;
    WORD    select;
    BYTE    wcount;
    BYTE    arights;
    WORD    hoffs;
} idtentry;

extern void GetIDTBaseOff( baseoffset * );
#pragma aux GetIDTBaseOff = \
        0x0f 0x01 0x08 /* sidt  [eax] */ \
    parm caller [eax];

intrptr HookVect( intrptr new_intrptr, int vect )
{
    LONG        temp;
    intrptr     old_intrptr;
    baseoffset  idt_baseoff;
    idtentry    *idt_table;

    GetIDTBaseOff( &idt_baseoff );
    temp = MapAbsoluteAddressToDataOffset( idt_baseoff.base );
    idt_table = (idtentry *)temp;
    idt_table = &idt_table[vect];
    temp = idt_table->hoffs;
    temp <<= 16;
    temp += idt_table->loffs;
    old_intrptr = MK_FP( idt_table->select, temp );
    temp = (unsigned)new_intrptr;
    Disable();
    idt_table->hoffs = temp >> 16;
    idt_table->loffs = temp;
    Enable();
    return( old_intrptr );
}


intrptr HookBreak( intrptr new_int03 )
{
    return( HookVect( new_int03, 3 ) );
}


intrptr HookTimer( intrptr new_int08 )
{
    return( HookVect( new_int08, FileServerMajorVersionNumber == 3 ? 8 : 40 ) );
}
