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
* Description:  Cover routines to access the trmem memory tracker
*               Trmem access techniques copied from GUI. 2004-11-25
*
****************************************************************************/


#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef TRMEM
#include "trmem.h"
#include <malloc.h>

_trmem_hdl  UIMemHandle;
static int  UIMemFileHandle;   /* stream to put output on */
static void UIMemPrintLine( int *, const char * buff, size_t len );

static int  UIMemOpened = 0;

#endif

#ifdef NLM
/* There is no equivalent expand function in NetWare. */
#define _expand NULL
#endif

void UIMemRedirect( int handle )
{
    handle=handle;
#ifdef TRMEM
    UIMemFileHandle = handle;
#else
    handle = handle;
#endif
}

extern void UIMemOpen( void )
{
#ifdef TRMEM
    char        *tmpdir;

    if( !UIMemOpened ) {
#ifdef NLM
        UIMemFileHandle = STDERR_HANDLE;
#else
        UIMemFileHandle = STDERR_FILENO;
#endif
        UIMemHandle = _trmem_open( malloc, free, realloc, _expand,
            &UIMemFileHandle, UIMemPrintLine,
            _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 |
            _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );

        tmpdir = getenv( "TRMEMFILE" );
        if( tmpdir != NULL ) {
            UIMemFileHandle = open( tmpdir, O_RDWR+O_CREAT+O_TRUNC+O_BINARY, S_IWUSR+S_IRUSR );
        }
        UIMemOpened = 1;
    }
#endif
}

void UIMemClose( void )
{
#ifdef TRMEM
    _trmem_prt_list( UIMemHandle );
    _trmem_close( UIMemHandle );
#ifdef NLM
    if( UIMemFileHandle != STDERR_HANDLE ) {
#else
    if( UIMemFileHandle != STDERR_FILENO ) {
#endif
        close( UIMemFileHandle );
    }
#endif
}

void UIMemPrtUsage( void )
{
#ifdef TRMEM
    _trmem_prt_usage( UIMemHandle );
#endif
}


void *uicalloc( unsigned n, unsigned size )
{
#ifdef TRMEM
    void *result = _trmem_alloc( n * size, _trmem_guess_who(), UIMemHandle );

    if( result )
        memset( result, 0, n * size );
    return( result );
#else
    return( calloc( n, size ) );
#endif
}

void *uimalloc( unsigned size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), UIMemHandle ) );
#else
    return( malloc( size ) );
#endif
}

void uifree( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), UIMemHandle );
#else
    free( ptr );
#endif
}

void *uirealloc( void *old, unsigned size )
{
#ifdef TRMEM
    return( _trmem_realloc( old, size, _trmem_guess_who(), UIMemHandle ) );
#else
    return( realloc( old, size ) );
#endif
}

/* extern to avoid problems with taking address and overlays */
void UIMemPrintLine( int *handle, const char *buff, size_t len )
{
#ifdef TRMEM
    write( *handle, buff, len );
#else
    handle = handle, buff = buff, len = len;
#endif
}
