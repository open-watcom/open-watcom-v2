/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Segmented memory access via WDebug386 or DPMI.
*
****************************************************************************/


#include "commonui.h"
#include <string.h>
#include <dos.h>
#include "bool.h"
#include "wdebug.h"
#include "descript.h"
#include "dpmi.h"
#include "segmem.h"


/*
 * following in-line code must be 32-bit code in 16-bit segment
 */
void PushAll( void );
#pragma aux PushAll = ".386" "pusha" \
    __parm __caller [] __value __modify __exact [__sp]

void PopAll( void );
#pragma aux PopAll = ".386" "popa" \
    __parm __caller [] __value __modify __exact [__ax __bx __cx __dx __sp __bp __di __si]

extern DWORD _GetASelectorSize( WORD );
#pragma aux _GetASelectorSize = \
        ".386"          \
        "movzx edx,ax"  \
        "xor   eax,eax" \
        "lsl   eax,edx" \
        "jnz short L1"  \
        "inc   eax"     \
    "L1: mov   edx,eax" \
        "shr   edx,16"  \
    __parm __caller [__ax] \
    __value         [__dx __ax] \
    __modify __exact [__ax __dx]

/*
 * WDebug386 must be defined in a program using these procedures
 */
extern bool             WDebug386;

DWORD GetASelectorSize( WORD sel )
{
    return( _GetASelectorSize( sel ) );
}

bool IsValidSelector( WORD sel )
{
    return( IsReadSelector( sel ) );
}

/*
 * GetADescriptor - read descriptor
 */
void GetADescriptor( WORD seg, descriptor *desc )
{
    union REGPACK       r;

    memset( desc, 0, sizeof( descriptor ) );
    if( WDebug386 ) {
        GetDescriptor( seg, desc );
    } else {
        memset( &r, 0, sizeof( r ) );
        r.w.ax = 0x0b;
        r.w.bx = seg;
        r.w.es = _FP_SEG( desc );
        r.w.di = _FP_OFF( desc );
        intr( 0x31, &r );
    }

} /* GetADescriptor */

/*
 * IsSeg32 - given a segment, check if it is a BIG one.
 */
bool IsSeg32( WORD seg )
{
    descriptor  desc;

    GetADescriptor( seg, &desc );
    return( desc.u2.flags.use32 );

} /* IsSeg32 */

/*
 * ReadMem - read some memory, using toolhelp or wdebug.386
 */
DWORD ReadMem( WORD sel, DWORD off, LPVOID buff, DWORD size )
{
    DWORD       rc;

    if( WDebug386 ) {
        return( CopyMemory386( _FP_SEG( buff ), _FP_OFF( buff ), sel, off, size  ) );
    } else {
        PushAll();
        rc = MemoryRead( sel, off, buff, size );
        PopAll();
        return( rc );
    }

} /* ReadMem */

/*
 * WriteMem - write some memory, using toolhelp or wdebug.386
 */
DWORD WriteMem( WORD sel, DWORD off, LPVOID buff, DWORD size )
{
    DWORD       rc;
    if( WDebug386 ) {
        return( CopyMemory386( sel, off, _FP_SEG( buff ), _FP_OFF( buff ), size ) );
    } else {
        PushAll();
        rc = MemoryWrite( sel, off, buff, size );
        PopAll();
        return( rc );
    }

} /* WriteMem */
