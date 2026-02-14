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
* Description:  Memory tracker cover routines.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include "wio.h"
#include "watcom.h"
#include "trmem.h"
#include "trmemcvr.h"

#include "clibext.h"


/*
 * There is no equivalent expand function in NetWare or non-Watcom
 */
#if defined( __NETWARE__ ) || !defined( __WATCOMC__ )
    #define _expand NULL
#else
	#include <malloc.h>
#endif

#if defined( TRMEM ) && defined( _M_IX86 ) && ( __WATCOMC__ > 1290 )
#define _XSTR(s)    # s
#define TRMEMAPI(x) _Pragma(_XSTR(aux x __frame))
#else
#define TRMEMAPI(x)
#endif


#if defined( TRMEM ) && defined( _M_IX86 ) && ( __WATCOMC__ > 1290 )
#define _XSTR(s)    # s
#define TRMEMAPI(x) _Pragma(_XSTR(aux x __frame))
#else
#define TRMEMAPI(x)
#endif

#ifdef TRMEM

static _trmem_hdl   TRMemHandle;
static FILE         *TRFileHandle = NULL;   /* stream to put output on */

/* extern to avoid problems with taking address and overlays */
static void TRPrintLine( void *parm, const char *buff, size_t len )
/*****************************************************************/
{
    /* unused parameters */ (void)parm; (void)len;

    if( TRFileHandle != NULL ) {
        fprintf( TRFileHandle, "%s\n", buff );
    }
}

#endif  /* TRMEM */

void TRMemRedirect( FILE *fp )
/****************************/
{
#ifndef TRMEM
    /* unused parameters */ (void)fp;
#else
    TRFileHandle = fp;
#endif
}

void TRMemOpen( void )
/********************/
{
#ifdef TRMEM
    TRFileHandle = stderr;
    TRMemHandle = _trmem_open( malloc, free, realloc, _expand,
            TRFileHandle, TRPrintLine,
            _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 |
            _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
#endif
}

void TRMemClose( void )
/*********************/
{
#ifdef TRMEM
    _trmem_close( TRMemHandle );
#endif
}

TRMEMAPI( TRMemAlloc )
void *TRMemAlloc( size_t size )
/*****************************/
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), TRMemHandle ) );
#else
    return( malloc( size ) );
#endif
}

TRMEMAPI( TRMemFree )
void TRMemFree( void *ptr )
/*************************/
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), TRMemHandle );
#else
    free( ptr );
#endif
}

TRMEMAPI( TRMemRealloc )
void *TRMemRealloc( void *ptr, size_t size )
/******************************************/
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _trmem_guess_who(), TRMemHandle ) );
#else
    return( realloc( ptr, size ) );
#endif
}

TRMEMAPI( TRMemStrdup )
char *TRMemStrdup( const char *str )
/**********************************/
{
#ifdef TRMEM
    return( _trmem_strdup( str, _trmem_guess_who(), TRMemHandle ) );
#else
    return( strdup( str ) );
#endif
}

#ifdef TRMEM

void TRMemPrtUsage( void )
/************************/
{
    _trmem_prt_usage( TRMemHandle );
}

unsigned TRMemPrtList( void )
/***************************/
{
    return( _trmem_prt_list( TRMemHandle ) );
}

TRMEMAPI( TRMemValidate )
int TRMemValidate( void *ptr )
/****************************/
{
    return( _trmem_validate( ptr, _trmem_guess_who(), TRMemHandle ) );
}

int TRMemValidateAll( void )
/**************************/
{
    return( _trmem_validate_all( TRMemHandle ) );
}

TRMEMAPI( TRMemChkRange )
int TRMemChkRange( void *start, size_t len )
/******************************************/
{
    return( _trmem_chk_range( start, len, _trmem_guess_who(), TRMemHandle ) );
}

#endif /* TRMEM */
