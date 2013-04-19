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


#include "precomp.h"
#include <stdlib.h>
#include <string.h>
#include "watcom.h"
#include "wrdll.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

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

#if 0
void WREMemOpen( void )
{
    WRMemOpen();
}

void WREMemClose( void )
{
    WRMemClose();
}
#endif

void *WREMemAlloc( size_t size )
{
    return( WRMemAlloc( size ) );
}

void WREMemFree( void *ptr )
{
    WRMemFree( ptr );
}

void *WREMemRealloc( void *old_ptr, size_t newsize )
{
    return( WRMemRealloc( old_ptr, newsize ) );
}

int WREMemValidate( void *ptr )
{
    return( WRMemValidate( ptr ) );
}

int WREMemChkRange( void *start, size_t len )
{
    return( WRMemChkRange( start, len ) );
}

/* functions to replace those in mem.c in SDK/MISC */

void MemStart( void )
{
#ifdef _M_I86
    __win_alloc_flags = GMEM_MOVEABLE | GMEM_SHARE;
    __win_realloc_flags = GMEM_MOVEABLE | GMEM_SHARE;
#endif
}

void *MemAlloc( unsigned size )
{
    void *p;

    p = WREMemAlloc( size );

    if( p != NULL ) {
        memset( p, 0, size );
    }

    return( p );
}

void *MemReAlloc( void *ptr, unsigned size )
{
    void *p;

    p = WREMemRealloc( ptr, size );

    return( p );
}

void MemFree( void *ptr )
{
    WREMemFree( ptr );
}
