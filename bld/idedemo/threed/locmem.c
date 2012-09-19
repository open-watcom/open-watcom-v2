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


/*
 Description:
 ============
    These functions replace the C malloc/free functions in windows.
    This is done since we definitely want local
    memory usage to come from the DLL local heap (the C library
    will use Global memory for Large model windows allocs).

*/

#ifdef PLAT_OS2
#define INCL_PM
#include <os2.h>
#else
#include <windows.h>
#endif
#include "wpi.h"
#include "wpitypes.h"

void near *lmem_alloc(
/********************/

    unsigned            size
) {
#ifdef PLAT_OS2
    return( malloc( (size_t) size ) );
#else
    HANDLE              mem_hld;
    void near           *mem;

    mem_hld = LocalAlloc( LMEM_MOVEABLE, size );

    if( mem_hld ) {
        mem = LocalLock( mem_hld );
        if( mem != NULL ) {
            return( mem );
        }
    }

    return( NULL );
#endif
}

void near *lmem_realloc(
/**********************/

    void                *mem,
    unsigned            size
) {
#ifdef PLAT_OS2
    return( realloc( mem, (size_t) size ) );
#else
    HANDLE              hld;
    void near           *ptr;

    if( mem != NULL ) {
        hld = _wpi_getlocalhdl( mem );

        if( hld ) {
            LocalUnlock( hld );
            hld = LocalReAlloc( hld, size, LMEM_MOVEABLE );
            if( hld ) {
                ptr = LocalLock( hld );
                return( ptr );
            }
        }
    } else {
        return( lmem_alloc( size ) );
    }

    return( NULL );
#endif
}


void lmem_free(
/*************/

    void                *mem
) {
#ifdef PLAT_OS2
    free( mem );
#else
    HLOCAL      hld;

    if( LOWORD(mem) != 0 ) {
        hld = _wpi_getlocalhdl( mem );

        if( hld ) {
            LocalUnlock( hld );
            LocalFree( hld );
        }
    }
#endif
}
