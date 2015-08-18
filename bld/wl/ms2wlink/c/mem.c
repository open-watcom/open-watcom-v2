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
* Description:  memory management routines for ms2wlink
*
****************************************************************************/


#include <stdlib.h>
#include "ms2wlink.h"

#ifdef TRMEM

#include "wio.h"

#include "clibext.h"
#include "trmem.h"

static _trmem_hdl TrHdl;

static void PrintLine( void *handle, const char *buff, size_t len )
/*****************************************************************/
{
    handle = handle;
    QWrite( STDERR_HANDLE, buff, len, NULL );
    QWriteNL( STDERR_HANDLE, NULL );
}

#endif

void MemInit( void )
/******************/
{
#ifdef TRMEM
    TrHdl = _trmem_open( malloc, free, NULL, NULL, NULL, PrintLine,
            _TRMEM_ALLOC_SIZE_0 | _TRMEM_FREE_NULL | _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
#endif
}


void MemFini( void )
/******************/
{
#ifdef TRMEM
    _trmem_prt_list( TrHdl );
    _trmem_close( TrHdl );
#endif
}

void *MemAlloc( size_t size )
/***************************/
{
    void                *ptr;

#ifdef TRMEM
    ptr = _trmem_alloc( size, _trmem_guess_who(), TrHdl );
#else
    ptr = malloc( size );
#endif
    if( ptr == NULL ) {
        Error( "Dynamic Memory Exhausted!!!" );
    }
    return( ptr );
}

void MemFree( void *p )
/*********************/
{
    if( p == NULL ) return;
#ifdef TRMEM
    _trmem_free( p, _trmem_guess_who(), TrHdl );
#else
    free( p );
#endif
}
