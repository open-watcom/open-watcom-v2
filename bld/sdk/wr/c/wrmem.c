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
* Description:  WR memory manipulation routines with optional tracking.
*
****************************************************************************/


#include "wrglbl.h"
#include "wrmemi.h"
#include "wresmem.h"
#include "trmem.h"


#if defined( _M_IX86 ) && defined( __NT__ )
#define _XSTR(s)    # s
#define TRMEMAPI(x)     _Pragma(_XSTR(aux x __frame))
#define TRMEMAPIDLL(x)  _Pragma(_XSTR(aux (__stdcall) x __frame))
#else
#define TRMEMAPI(x)
#define TRMEMAPIDLL(x)
#endif

static _trmem_hdl   TRMemHandle;
static FILE         *TRMemFile = NULL;

static void TRPrintLine( void *parm, const char *buff, size_t len )
/*****************************************************************/
{
    /* unused parameters */ (void)parm; (void)len;

    if( TRMemFile != NULL ) {
        fprintf( TRMemFile, "%s\n", buff );
    }
}

void WRMemOpen( bool trace )
{
    char    *tmpdir;

    if( trace ) {
        TRMemHandle = _trmem_open( malloc, free, realloc, NULL,
                                   NULL, TRPrintLine,
                                   _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 |
                                   _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
        tmpdir = getenv( "TRMEMFILE" );
        if( tmpdir != NULL ) {
            TRMemFile = fopen( tmpdir, "w" );
        }
    } else {
        TRMemHandle = NULL;
    }
}

void WRMemClose( void )
{
    _trmem_prt_list( TRMemHandle );
    _trmem_close( TRMemHandle );
    if( TRMemFile != NULL ) {
        fclose( TRMemFile );
        TRMemFile = NULL;
    }
}

TRMEMAPIDLL( WRMemAlloc )
void *WRAPI WRMemAlloc( size_t size )
{
    if( TRMemHandle != NULL ) {
        return( _trmem_alloc( size, _trmem_guess_who(), TRMemHandle ) );
    } else {
        return( malloc( size ) );
    }
}

TRMEMAPIDLL( WRMemFree )
void WRAPI WRMemFree( void *ptr )
{
    if( TRMemHandle != NULL ) {
        _trmem_free( ptr, _trmem_guess_who(), TRMemHandle );
    } else {
        free( ptr );
    }
}

TRMEMAPIDLL( WRMemRealloc )
void *WRAPI WRMemRealloc( void *ptr, size_t size )
{
    if( TRMemHandle != NULL ) {
        return( _trmem_realloc( ptr, size, _trmem_guess_who(), TRMemHandle ) );
    } else {
        return( realloc( ptr, size ) );
    }
}

TRMEMAPIDLL( WRMemValidate )
int WRAPI WRMemValidate( void *ptr )
{
    if( TRMemHandle != NULL ) {
        return( _trmem_validate( ptr, _trmem_guess_who(), TRMemHandle ) );
    } else {
        return( TRUE );
    }
}

TRMEMAPIDLL( WRMemChkRange )
int WRAPI WRMemChkRange( void *start, size_t len )
{
    if( TRMemHandle != NULL ) {
        return( _trmem_chk_range( start, len, _trmem_guess_who(), TRMemHandle ) );
    } else {
        return( TRUE );
    }
}

void WRAPI WRMemPrtUsage( void )
{
    if( TRMemHandle != NULL ) {
        _trmem_prt_usage( TRMemHandle );
    }
}

/* function to replace this in mem.c in commonui */

TRMEMAPI( MemAlloc )
void *MemAlloc( size_t size )
{
    void *p;

    if( TRMemHandle != NULL ) {
        p = _trmem_alloc( size, _trmem_guess_who(), TRMemHandle );
    } else {
        p = malloc( size );
    }

    if( p != NULL ) {
        memset( p, 0, size );
    }

    return( p );
}

/* function for wres.lib */

TRMEMAPI( wres_alloc )
void *wres_alloc( size_t size )
{
    if( TRMemHandle != NULL ) {
        return( _trmem_alloc( size, _trmem_guess_who(), TRMemHandle ) );
    } else {
        return( malloc( size ) );
    }
}

/* function to replace this in mem.c in commonui */

TRMEMAPI( MemRealloc )
void *MemRealloc( void *ptr, size_t size )
{
    void *p;

    if( TRMemHandle != NULL ) {
        p = _trmem_realloc( ptr, size, _trmem_guess_who(), TRMemHandle );
    } else {
        p = realloc( ptr, size );
    }
    return( p );
}

/* function to replace this in mem.c in commonui */

TRMEMAPI( MemFree )
void MemFree( void *ptr )
{
    if( TRMemHandle != NULL ) {
        _trmem_free( ptr, _trmem_guess_who(), TRMemHandle );
    } else {
        free( ptr );
    }
}

/* function for wres.lib */

TRMEMAPI( wres_free )
void wres_free( void *ptr )
{
    if( TRMemHandle != NULL ) {
        _trmem_free( ptr, _trmem_guess_who(), TRMemHandle );
    } else {
        free( ptr );
    }
}
