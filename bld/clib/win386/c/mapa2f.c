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


#include <string.h>
#include <dos.h>
#include "alloc.h"

void *MapAliasToFlat( DWORD alias )
{
    union REGPACK       r;
    DWORD               base_32;
    DWORD               base_16;
    DWORD               res;

    memset( &r, 0, sizeof( r ) );
    r.x.ax = 0x06;
    r.x.bx = FP_SEG( (void far *) &r );
    intr( 0x31, &r );
    base_32 = (r.x.cx << 16L) + (DWORD) r.x.dx;
    r.x.ax = 0x06;
    r.x.bx = alias >> 16;
    intr( 0x31, &r );
    base_16 = (r.x.cx << 16L) + (DWORD) r.x.dx;
    res = base_16 - base_32 + (DWORD)((WORD)alias);
    return( (void *) res );

} /* MapAliasToFlat */
