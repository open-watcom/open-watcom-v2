/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Standard stream/file accessor routines.
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#if defined( __NETWARE__ )
    #include <io.h>
#endif
#include "clibsupp.h"
#include "rtdata.h"


#if defined( __NETWARE__ ) && defined( _THIN_LIB ) && defined( _NETWARE_LIBC )
/*
 * from Netware LIBC library, not implemented by Open Watcom
 */
_WCRTLINK extern FILE   **___stdin ( void );
_WCRTLINK extern FILE   **___stdout( void );
_WCRTLINK extern FILE   **___stderr( void );
_WCRTLINK extern FILE   **___cin   ( void );
_WCRTLINK extern FILE   **___cout  ( void );
#endif

#if defined( __NETWARE__ ) && !defined( _THIN_LIB )
/*
 * This code is for "fat" Netware libraries.
 * We are using the Open Watcom FILE *'s so this will be
 * calling Open Watcom __get_std_stream.
 */
_WCRTLINK FILE **__get_stdin( void )
{
    static FILE         *stdin_ptr;

    stdin_ptr = __get_std_stream( STDIN_FILENO );
    return( &stdin_ptr );
}

_WCRTLINK FILE **__get_stdout( void )
{
    static FILE         *stdout_ptr;

    stdout_ptr = __get_std_stream( STDOUT_FILENO );
    return( &stdout_ptr );
}

_WCRTLINK FILE **__get_stderr( void )
{
    static FILE         *stderr_ptr;

    stderr_ptr = __get_std_stream( STDERR_FILENO );
    return( &stderr_ptr );
}
#endif

/*
 * Implementation notes for __get_std_stream:
 *
 * __get_std_stream name must not be changed, it must be the same as the function
 * name in the Netware C run-time library.  This ensures that Netware libraries
 * will call the Open Watcom library version of __get_std_stream instead of
 * the Netware C run-time library version.
 */

_WCRTLINK FILE *__get_std_stream( unsigned handle )
{
    FILE    *pFile = NULL;

#if defined( __NETWARE__ ) && defined( _THIN_LIB )
  #if defined( _NETWARE_LIBC )
    switch( handle ) {
    case STDIN_FILENO:
        pFile = *___stdin();
        break;
    case STDOUT_FILENO:
        pFile = *___stdout();
        break;
    case STDERR_FILENO:
        pFile = *___stderr();
        break;
    default:
        break;
    }
  #elif defined( _NETWARE_CLIB )
    switch( handle ) {
    case STDIN_FILENO:
        pFile = *__get_stdin();
        break;
    case STDOUT_FILENO:
        pFile = *__get_stdout();
        break;
    case STDERR_FILENO:
        pFile = *__get_stderr();
        break;
    default:
        break;
    }
  #endif
#else
    if( handle < NUM_STD_STREAMS ) {
        pFile = &_RWD_iob[handle];
    }
#endif
    return( pFile );
}

#if !defined( __NETWARE__ ) || !defined( _THIN_LIB )
_WCRTLINK FILE *__get_std_file( unsigned handle )
{
    return( __get_std_stream( handle ) );
}
#endif
