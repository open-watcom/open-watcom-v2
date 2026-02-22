/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


#include "wreglbl.h"
#include "wrdll.h"
#include "wresmem.h"
#include "memfuncs.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

#if defined( _M_IX86 ) && defined( __NT__ )
#define _XSTR(s)    # s
#define TRMEMAPI(x)     _Pragma(_XSTR(aux x __frame))
#else
#define TRMEMAPI(x)
#endif

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

/* function to replace this in mem.c in commonui */

TRMEMAPI( MemAlloc )
void *MemAlloc( size_t size )
{
    void *p;

    p = WRMemAlloc( size, _TRMEM_WHO( 1 ) );

    if( p != NULL ) {
        memset( p, 0, size );
    }

    return( p );
}

TRMEMAPI( MemRealloc )
void *MemRealloc( void *old, size_t size )
{
    return( WRMemRealloc( old, size, _TRMEM_WHO( 2 ) ) );
}

/* function for wres.lib */

TRMEMAPI( wres_alloc )
void *wres_alloc( size_t size )
{
    return( WRMemAlloc( size, _TRMEM_WHO( 3 ) ) );
}

/* function to replace this in mem.c in commonui */

TRMEMAPI( MemFree )
void MemFree( void *ptr )
{
    WRMemFree( ptr, _TRMEM_WHO( 4 ) );
}

/* function for wres.lib */

TRMEMAPI( wres_free )
void wres_free( void *ptr )
{
    WRMemFree( ptr, _TRMEM_WHO( 5 ) );
}
