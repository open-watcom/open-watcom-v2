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
    These functions allow global memory to be used like 'malloc' amd 'free'.

*/

#ifdef PLAT_OS2
#define INCL_PM
#include <os2.h>
#else
#include <windows.h>
#endif
#include "wpi.h"
#include "wpitypes.h"

void *gmem_alloc(
/***************/

    unsigned            size
) {
#ifdef PLAT_OS2
    return( malloc( (size_t) size ) );
#else
    HANDLE              mem_hld;
    void                *mem;

    mem_hld = GlobalAlloc( GMEM_MOVEABLE | GMEM_SHARE, size );

    if( mem_hld ) {
        mem = GlobalLock( mem_hld );
        if( mem != NULL ) {
            return( mem );
        }
    }

    return( NULL );
#endif
}

void *gmem_realloc(
/*****************/

    void                *ptr,
    unsigned            size
) {
#ifdef PLAT_OS2
    return( realloc( ptr, (size_t) size ) );
#else
    HGLOBAL             hld;

    if( ptr != NULL ) {
        hld = _wpi_getglobalhdl( ptr );

        if( hld ) {
            GlobalUnlock( hld );
            hld = GlobalReAlloc( hld, size, GMEM_MOVEABLE );
            if( hld ) {
                ptr = GlobalLock( hld );
                return( ptr );
            }
        }
    } else {
        return( gmem_alloc( size ) );
    }

    return( NULL );
#endif
}


void gmem_free(
/*************/

    void                *mem
) {
#ifdef PLAT_OS2
    free( mem );
#else
    HGLOBAL             hld;

    if( mem != NULL ) {
        hld = _wpi_getglobalhdl( mem );

        if( hld ) {
            GlobalUnlock( hld );
            GlobalFree( hld );
        }
    }
#endif
}
