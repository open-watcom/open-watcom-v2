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
* Description:  Memory management routines for fcenable
*
****************************************************************************/


#include <stdlib.h>
#include "fcenable.h"
#ifdef TRMEM
    #include "trmem.h"
#endif


#if defined( TRMEM ) && defined( _M_IX86 ) && ( __WATCOMC__ > 1290 )
#define _XSTR(s)    # s
#define TRMEMAPI(x) _Pragma(_XSTR(aux x __frame))
#else
#define TRMEMAPI(x)
#endif

#ifdef TRMEM

static _trmem_hdl TrHdl;

static void PrintLine( void *parm, const char *buff, size_t len )
/***************************************************************/
{
    /* unused parameters */ (void)parm; (void)len;

    printf( "%s\n", buff );
}

#endif  /* TRMEM */


void MemInit( void )
/******************/
{
#ifdef TRMEM
    TrHdl = _trmem_open( malloc, free, _TRMEM_NO_REALLOC, _TRMEM_NO_STRDUP,
                            NULL, PrintLine, _TRMEM_DEF );
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

TRMEMAPI( MemAlloc )
void *MemAlloc( size_t size )
/***************************/
{
    void        *ptr;

#ifdef TRMEM
    ptr = _trmem_alloc( size, _TRMEM_WHO( 1 ), TrHdl );
#else
    ptr = malloc( size );
#endif
    if( ptr == NULL ) {
        Error( "Dynamic Memory Exhausted!!!" );
    }
    return( ptr );
}

TRMEMAPI( MemFree )
void MemFree( void *p )
/*********************/
{
    if( p == NULL )
        return;
#ifdef TRMEM
    _trmem_free( p, _TRMEM_WHO( 2 ), TrHdl );
#else
    free( p );
#endif
}
