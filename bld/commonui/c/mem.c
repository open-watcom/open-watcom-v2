/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Memory allocator with optional tracking.
*
****************************************************************************/


//#define DEBUGMEM

#include "commonui.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <malloc.h>
#include "mem.h"

/*
 * MemStart - set up required stuff for memory manager
 */
void MemStart( void )
{
#ifdef _M_I86
#ifndef __OS2_PM__
    __win_alloc_flags = GMEM_MOVEABLE | GMEM_SHARE;
    __win_realloc_flags = GMEM_MOVEABLE | GMEM_SHARE;
#endif
#endif

} /* MemStart */

/*
 * MemAlloc - allocate some memory
 */
void *MemAlloc( size_t size )
{
    void *x;
#if defined( DEBUGMEM )
    x = GlobalLock( GlobalAlloc( GMEM_ZEROINIT | GMEM_MOVEABLE, size ) );
#else
    x = calloc( 1, size );
#endif
#if defined( WANT_MSGS )
    if( x == NULL ) {
        MessageBox( HWND_DESKTOP, "AUUGH, Null Pointer", "Memory Allocation",
                    MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
    }
#endif
    return( x );

} /* MemAlloc */

/*
 * MemReAlloc - allocate some memory
 */
void *MemReAlloc( void *ptr, size_t size )
{
    void *x;
#ifndef __OS2_PM__
#if defined( DEBUGMEM )
    GLOBALHANDLE        h;

    h = GlobalHandle( FP_SEG( ptr ) );
    GlobalUnlock( h );
    x = GlobalLock( GlobalReAlloc( h, size, GMEM_ZEROINIT | GMEM_MOVEABLE ) );
#else
    x = realloc( ptr, size );
#endif
#else
    x = realloc( ptr, size );
#endif
#if defined( WANT_MSGS )
    if( x == NULL ) {
        MessageBox( HWND_DESKTOP, "AUUGH, Null Pointer", "Memory Allocation",
                    MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
    }
#endif
    return( x );

} /* MemReAlloc */

/*
 * MemFree - free some memory
 */
void MemFree( void *ptr )
{
#ifndef __OS2_PM__
#if defined( DEBUGMEM )
    GLOBALHANDLE        h;

    h = GlobalHandle( FP_SEG( ptr ) );
    GlobalUnlock( h );
    GlobalFree( h );
#else
    free( ptr );
#endif
#else
    free( ptr );
#endif

} /* MemFree */
