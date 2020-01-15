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
* Description:  Memory allocator with optional tracking.
*
****************************************************************************/


#include "spy.h"
#include "cguimem.h"
#include "spymem.h"
#ifdef TRMEM
    #include "trmem.h"
#endif


#ifdef TRMEM
static _trmem_hdl  MemHandle;

static FILE *MemFP = NULL;   /* stream to put output on */

static void MemPrintLine( void *parm, const char *buff, size_t len )
/******************************************************************/
{
    /* unused parameters */ (void)parm; (void)len;

    if( MemFP != NULL ) {
        fprintf( MemFP, "%s\n", buff );
    }
}
#endif

void MemOpen( void )
/******************/
{
#ifdef TRMEM
    char    *tmpdir;
#endif

#ifdef _M_I86
    __win_alloc_flags = GMEM_MOVEABLE | GMEM_SHARE;
    __win_realloc_flags = GMEM_MOVEABLE | GMEM_SHARE;
#endif
#ifdef TRMEM
    MemHandle = _trmem_open( malloc, free, realloc, NULL,
        NULL, MemPrintLine,
        _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 |
        _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );

    tmpdir = getenv( "TRMEMFILE" );
    if( tmpdir != NULL ) {
        MemFP = fopen( tmpdir, "w" );
    }
#endif
}

void MemClose( void )
/*******************/
{
#ifdef TRMEM
    _trmem_prt_list( MemHandle );
    _trmem_close( MemHandle );
    if( MemFP != NULL ) {
        fclose( MemFP );
        MemFP = NULL;
    }
#endif
}

void *MemAlloc( size_t size )
/***************************/
{
    void        *ptr;

#ifdef TRMEM
    ptr = _trmem_alloc( size, _trmem_guess_who(), MemHandle );
#else
    ptr = malloc( size );
    memset( ptr, 0, size );
#endif
    return( ptr );
}

void *MemRealloc( void *ptr, size_t size )
/****************************************/
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _trmem_guess_who(), MemHandle ) );
#else
    return( realloc( ptr, size ) );
#endif
}

void MemFree( void *ptr )
/***********************/
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), MemHandle );
#else
    free( ptr );
#endif
}
