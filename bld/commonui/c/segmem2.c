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
* Description:  Segmented memory access via WDebug386 or DPMI.
*
****************************************************************************/


#include "precomp.h"
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include "descript.h"
#include "wdebug.h"

/*
 * WDebug386 must be defined in a program using these procedures
 */
extern BOOL             near WDebug386;

extern WORD CopySize;
extern short far _CopyMemory( WORD, DWORD, WORD, DWORD );
#pragma aux _CopyMemory parm [cx] [dx di] [ax] [si bx] value[ax];

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
        r.w.es = FP_SEG( desc );
        r.w.di = FP_OFF( desc );
        intr( 0x31, &r );
    }

} /* GetADescriptor */

/*
 * IsSeg32 - given a segment, check if it is a BIG one.
 */
int IsSeg32( WORD seg )
{
    descriptor  desc;

    GetADescriptor( seg, &desc );
    if( desc.big_or_default ) {
        return( TRUE );
    }
    return( FALSE );

} /* IsSeg32 */

void PushAll( void );
void PopAll( void );
#pragma aux PushAll = 0x60;
#pragma aux PopAll = 0x61 modify[dx ax];

/*
 * ReadMem - read some memory, using toolhelp or wdebug.386
 */
DWORD ReadMem( WORD sel, DWORD off, LPVOID buff, DWORD size )
{
    DWORD       rc;

    if( !WDebug386 ) {
        PushAll();
        rc = MemoryRead( sel, off, buff, size );
        PopAll();
        return( rc );
    } else {
        CopySize = size;
        return( _CopyMemory( FP_SEG( buff ), FP_OFF( buff ), sel, off  ) );
    }

} /* ReadMem */

/*
 * WriteMem - write some memory, using toolhelp or wdebug.386
 */
DWORD WriteMem( WORD sel, DWORD off, LPVOID buff, DWORD size )
{
    DWORD       rc;
    if( !WDebug386 ) {
        PushAll();
        rc = MemoryWrite( sel, off, buff, size );
        PopAll();
        return( rc );
    } else {
        CopySize = size;
        return( _CopyMemory( sel, off, FP_SEG( buff ), FP_OFF( buff ) ) );
    }

} /* WriteMem */
