/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  memory tracking cover functions
*
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "wio.h"
#include "trmem.h"
#include "memfuncs.h"
#include "wresmem.h"

#include "clibext.h"


#ifdef TRMEM
static _trmem_hdl   TRMemHandle = NULL;
static FILE         *TRFileFP = NULL;       /* stream to put output on */

static void MemPrintLine( void *parm, const char *buff, size_t len )
/******************************************************************/
{
    /* unused parameters */ (void)parm; (void)len;

    if( TRFileFP != NULL ) {
        fprintf( TRFileFP, "%s\n", buff );
    }
}
#endif

void MemOpen( void )
{
#ifdef TRMEM
    TRFileFP = stderr;
    TRMemHandle = _trmem_open( malloc, free, realloc, NULL,
            NULL, MemPrintLine,
            _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 |
            _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
#endif
}

void *MemAlloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), TRMemHandle ) );
#else
    return( malloc( size ) );
#endif
}

void *wres_alloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), TRMemHandle ) );
#else
    return( malloc( size ) );
#endif
}

void *MemRealloc( void *ptr, size_t size )
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _trmem_guess_who(), TRMemHandle ) );
#else
    return( realloc( ptr, size ) );
#endif
}

void MemFree( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), TRMemHandle );
#else
    free( ptr );
#endif
}

void wres_free( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), TRMemHandle );
#else
    free( ptr );
#endif
}

void MemPrtList( void )
{
#ifdef TRMEM
    if( TRMemHandle != NULL ) {
        _trmem_prt_list( TRMemHandle );
    }
#endif
}

void MemClose( void )
{
#ifdef TRMEM
    if( TRMemHandle != NULL ) {
        _trmem_close( TRMemHandle );
    }
#endif
}
