/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Cover routines to access the trmem memory tracker (UI mode)
*
****************************************************************************/


#include "guiwind.h"
#include <stdlib.h>
#include "guimem.h"
#include "stdui.h"
#include "helpmem.h"
#ifdef TRMEM
    #include "trmem.h"
#endif


#ifdef TRMEM

static _trmem_hdl  GUIMemHandle;

static FILE *GUIMemFP = NULL;   /* stream to put output on */
static int  GUIMemOpened = 0;

static void GUIMemPrintLine( void *parm, const char *buff, size_t len )
/*********************************************************************/
{
    /* unused parameters */ (void)parm; (void)len;

    if( GUIMemFP != NULL ) {
        fprintf( GUIMemFP, "%s\n", buff );
    }
}

#endif  /* TRMEM */

void GUIMemPrtUsage( void )
/*************************/
{
#ifdef TRMEM
    _trmem_prt_usage( GUIMemHandle );
#endif
}

void GUIMemRedirect( FILE *fp )
/*****************************/
{
#ifdef TRMEM
    GUIMemFP = fp;
#else
    /* unused parameters */ (void)fp;
#endif
}

void GUIMemOpen( void )
/*********************/
{
#ifdef TRMEM
    char * tmpdir;

    if( !GUIMemOpened ) {
        GUIMemFP = stderr;
        GUIMemHandle = _trmem_open( malloc, free, realloc, NULL,
            NULL, GUIMemPrintLine,
            _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 |
            _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );

        tmpdir = getenv( "TRMEMFILE" );
        if( tmpdir != NULL ) {
            GUIMemFP = fopen( tmpdir, "w" );
        }
        GUIMemOpened = 1;
    }
#endif
}
void UIAPI UIMemOpen( void ) {}

void GUIMemClose( void )
/**********************/
{
#ifdef TRMEM
    _trmem_prt_list( GUIMemHandle );
    _trmem_close( GUIMemHandle );
    if( GUIMemFP != stderr ) {
        fclose( GUIMemFP );
        GUIMemFP = NULL;
    }
#endif
}
void UIAPI UIMemClose( void ) {}

/*
 * Alloc functions
 */

#if defined( TRMEM ) && defined( _M_IX86 )
#pragma aux (WFRM) GUIMemAlloc
#endif
void *GUIMemAlloc( size_t size )
/******************************/
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), GUIMemHandle ) );
#else
    return( malloc( size ) );
#endif
}
#if defined( TRMEM ) && defined( _M_IX86 )
#pragma aux (WFRM) uimalloc
#endif
void * UIAPI uimalloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), GUIMemHandle ) );
#else
    return( malloc( size ) );
#endif
}
#if defined( TRMEM ) && defined( _M_IX86 )
#pragma aux (WFRM) HelpMemAlloc
#endif
void *HelpMemAlloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), GUIMemHandle ) );
#else
    return( malloc( size ) );
#endif
}

/*
 * Free functions
 */

#if defined( TRMEM ) && defined( _M_IX86 )
#pragma aux (WFRM) GUIMemFree
#endif
void GUIMemFree( void *ptr )
/**************************/
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), GUIMemHandle );
#else
    free( ptr );
#endif
}

#if defined( TRMEM ) && defined( _M_IX86 )
#pragma aux (WFRM) uifree
#endif
void UIAPI uifree( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), GUIMemHandle );
#else
    free( ptr );
#endif
}
#if defined( TRMEM ) && defined( _M_IX86 )
#pragma aux (WFRM) HelpMemFree
#endif
void HelpMemFree( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), GUIMemHandle );
#else
    free( ptr );
#endif
}

/*
 * Realloc functions
 */

#if defined( TRMEM ) && defined( _M_IX86 )
#pragma aux (WFRM) GUIMemRealloc
#endif
void *GUIMemRealloc( void *ptr, size_t size )
/*******************************************/
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _trmem_guess_who(), GUIMemHandle ) );
#else
    return( realloc( ptr, size ) );
#endif
}
#if defined( TRMEM ) && defined( _M_IX86 )
#pragma aux (WFRM) uirealloc
#endif
void * UIAPI uirealloc( void *ptr, size_t size )
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _trmem_guess_who(), GUIMemHandle ) );
#else
    return( realloc( ptr, size ) );
#endif
}
#if defined( TRMEM ) && defined( _M_IX86 )
#pragma aux (WFRM) HelpMemRealloc
#endif
void *HelpMemRealloc( void *ptr, size_t size )
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _trmem_guess_who(), GUIMemHandle ) );
#else
    return( realloc( ptr, size ) );
#endif
}
