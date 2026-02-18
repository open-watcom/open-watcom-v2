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


#include "wlib.h"
#include "wresmem.h"
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

static _trmem_hdl   TRMemHandle;

extern void TRPrintLine( void *parm, const char *buff, size_t len )
/*****************************************************************/
{
    /* unused parameters */ (void)parm; (void)len;

    fprintf( stderr, "%s\n", buff );
}

#endif  /* TRMEM */

void InitMem( void )
/******************/
{
#ifdef TRMEM
    TRMemHandle = _trmem_open( malloc, free, realloc, strdup,
        NULL, TRPrintLine, _TRMEM_DEF );
#endif
}

void FiniMem( void )
/******************/
{
#ifdef TRMEM
    _trmem_prt_list_ex( TRMemHandle, 100 );
    _trmem_close( TRMemHandle );
#endif
}

TRMEMAPI( MemAlloc )
void *MemAlloc( size_t size )
/***************************/
{
    void *ptr;

    if( size == 0 ) {
        return( NULL );
    }
#ifdef TRMEM
    ptr = _trmem_alloc( size, _TRMEM_WHO( 1 ), TRMemHandle );
#else
    ptr = malloc( size );
#endif
    if( ptr == NULL )
        FatalError( ERR_NO_MEMORY );
    return( ptr );
}

char *MemStrdup( const char *str )
/********************************/
{
    char *ptr;

    if( str == NULL )
        return( NULL );
#ifdef TRMEM
    ptr = _trmem_strdup( str, _TRMEM_WHO( 2 ), TRMemHandle );
#else
    ptr = strdup( str );
#endif
    if( ptr == NULL )
        FatalError( ERR_NO_MEMORY );
    return( ptr );
}

TRMEMAPI( MemRealloc )
void *MemRealloc( void *ptr, size_t size )
/****************************************/
{
    void  *mptr;

#ifdef TRMEM
    mptr = _trmem_realloc( ptr, size, _TRMEM_WHO( 3 ), TRMemHandle );
#else
    mptr = realloc( ptr, size );
#endif
    if( mptr == NULL && size != 0 )
        FatalError( ERR_NO_MEMORY );
    return( mptr );
}

TRMEMAPI( MemFree )
void MemFree( void *ptr )
/***********************/
{
    if( ptr == NULL )
        return;
#ifdef TRMEM
    _trmem_free( ptr, _TRMEM_WHO( 4 ), TRMemHandle );
#else
    free( ptr );
#endif
}

TRMEMAPI( wres_alloc )
void *wres_alloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _TRMEM_WHO( 5 ), TRMemHandle ) );
#else
    return( malloc( size ) );
#endif
}

TRMEMAPI( wres_free )
void wres_free( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _TRMEM_WHO( 6 ), TRMemHandle );
#else
    free( ptr );
#endif
}
