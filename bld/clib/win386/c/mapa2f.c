/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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


#include "variety.h"
#include <string.h>
#include <dos.h>
#include "cover.h"


void *MapAliasToFlat( DWORD alias )
{
    union REGPACK       regs;
    DWORD               base_32;
    DWORD               base_16;
    DWORD               res;

    memset( &regs, 0, sizeof( regs ) );
    regs.x.ax = 0x06;
    regs.x.bx = _FP_SEG( (void __far *)&regs );
    intr( 0x31, &regs );
    base_32 = ( regs.x.cx << 16L ) + (DWORD)regs.x.dx;
    regs.x.ax = 0x06;
    regs.x.bx = alias >> 16;
    intr( 0x31, &regs );
    base_16 = ( regs.x.cx << 16L ) + (DWORD)regs.x.dx;
    res = base_16 - base_32 + (DWORD)((WORD)alias);
    return( (void *)res );

} /* MapAliasToFlat */
