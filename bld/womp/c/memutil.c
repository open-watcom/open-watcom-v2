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


//#include <malloc.h>
#include <stdlib.h>
#include "watcom.h"
#include "womp.h"
#include "genutil.h"
#include "memutil.h"
#ifdef  TRMEM
//    #include <malloc.h>
    #include <io.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include "trmem.h"
#endif


#if defined( TRMEM ) && defined( _M_IX86 )
#define _XSTR(s)    # s
#define TRMEMAPI(x) _Pragma(_XSTR(aux x __frame))
#else
#define TRMEMAPI(x)
#endif

#ifdef  TRMEM

STATIC _trmem_hdl   TrHdl = _TRMEM_HDL_NONE;
STATIC FILE         *TrFile = NULL;

STATIC void memPrintLine( void *fh, const char *buf, unsigned size )
{
    /* unused parameters */ (void)fh; (void)size;

    fprintf( stderr, "***%s\n", buf );
    if( TrFile != NULL ) {
        fprintf( TrFile, "%s\n", buf );
    }
}

#endif  /* TRMEM */

void MemInit( void ) {

#ifdef TRMEM
    TrFile = fopen( "mem.trk", "wt" );
    TrHdl = _trmem_open( malloc, free, realloc, _TRMEM_NO_STRDUP,
        TrFile, memPrintLine, _TRMEM_DEF );
    if( TrHdl == _TRMEM_HDL_NONE ) {
        exit( EXIT_FAILURE );
    }
#endif
}

void MemFini( void ) {

#ifdef TRMEM
    if( TrHdl != _TRMEM_HDL_NONE ) {
        _trmem_prt_list( TrHdl );
        _trmem_close( TrHdl );
        if( TrFile != NULL ) {
            fclose( TrFile );
            TrFile = NULL;
        }
        TrHdl = _TRMEM_HDL_NONE;
    }
#endif
}

static void *check_nomem( void *ptr )
{
    if( ptr == NULL ) {
        Fatal( MSG_OUT_OF_MEMORY );
    }
    return( ptr );
}

TRMEMAPI( MemAlloc )
void *MemAlloc( size_t size )
/***************************/
{
#ifdef TRMEM
    return( check_nomem( _trmem_alloc( size, _TRMEM_WHO( 1 ), TrHdl ) ) );
#else
    return( check_nomem( malloc( size ) ) );
#endif
}

TRMEMAPI( MemRealloc )
void *MemRealloc( void *ptr, size_t size )
/****************************************/
{
    void *new;

#ifdef TRMEM
    return( check_nomem( _trmem_realloc( ptr, size, _TRMEM_WHO( 2 ), TrHdl ) ) );
#else
    return( check_nomem( realloc( ptr, size ) ) );
#endif
}

TRMEMAPI( MemFree )
void MemFree( void *ptr ) {
/***********************/
#ifdef TRMEM
    _trmem_free( ptr, _TRMEM_WHO( 3 ), TrHdl );
#else
    free( ptr );
#endif
}
