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
* Description:  Cover routines to access the trmem memory tracker
*               Trmem access techniques copied from GUI. 2004-11-25
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uidef.h"
#include "memfuncs.h"
#ifdef TRMEM
    #include "trmem.h"
#endif

#include "clibext.h"


#if defined( TRMEM ) && defined( _M_IX86 ) && !defined( __WINDOWS__ ) && !defined( _M_I86 ) && ( __WATCOMC__ > 1290 )
#define _XSTR(s)    # s
#define TRMEMAPI(x) _Pragma(_XSTR(aux x __frame))
#else
#define TRMEMAPI(x)
#endif

#ifdef TRMEM

static _trmem_hdl  TrHdl = _TRMEM_HDL_NONE;

static FILE *TrFile = NULL;    /* stream to put output on */
static int  UIMemOpened = 0;

static void UIMemPrintLine( void *parm, const char *buff, size_t len )
{
    /* unused parameters */ (void)parm; (void)len;

    if( TrFile != NULL ) {
        fprintf( TrFile, "%s\n", buff );
    }
}

#endif  /* TRMEM */

#if 0
void UIMemRedirect( FILE *fp )
{
#ifdef TRMEM
    TrFile = fp;
#else
    /* unused parameters */ (void)fp;
#endif
}
#endif

void UIAPI UIMemOpen( void )
{
#ifdef TRMEM
    const char      *tmpdir;

    if( !UIMemOpened ) {
        TrFile = stderr;
        TrHdl = _trmem_open( malloc, free, realloc, _TRMEM_NO_STRDUP,
            TrFile, UIMemPrintLine, _TRMEM_DEF );

        tmpdir = getenv( "TRMEMFILE" );
        if( tmpdir != NULL ) {
            TrFile = fopen( tmpdir, "w" );
        }
        UIMemOpened = 1;
    }
#endif
}

void UIAPI UIMemClose( void )
{
#ifdef TRMEM
    _trmem_prt_list( TrHdl );
    _trmem_close( TrHdl );
    if( TrFile != stderr ) {
        fclose( TrFile );
        TrFile = NULL;
    }
#endif
}

#if 0
void UIMemPrtUsage( void )
{
#ifdef TRMEM
    _trmem_prt_usage( TrHdl );
#endif
}
#endif


TRMEMAPI( MemAlloc )
void * MemAlloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _TRMEM_WHO( 1 ), TrHdl ) );
#else
    return( malloc( size ) );
#endif
}

TRMEMAPI( MemFree )
void MemFree( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _TRMEM_WHO( 2 ), TrHdl );
#else
    free( ptr );
#endif
}

TRMEMAPI( MemRealloc )
void * MemRealloc( void *old, size_t size )
{
#ifdef TRMEM
    return( _trmem_realloc( old, size, _TRMEM_WHO( 3 ), TrHdl ) );
#else
    return( realloc( old, size ) );
#endif
}
