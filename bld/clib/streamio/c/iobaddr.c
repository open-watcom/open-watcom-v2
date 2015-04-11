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
* Description:  Standard stream/file accessor routines.
*
****************************************************************************/


#include "variety.h"
#include "clibsupp.h"
#include "rtdata.h"
#if defined( __NETWARE__ )
  #if !defined( _THIN_LIB ) || defined( _NETWARE_CLIB )
    extern FILE   **__get_stdin ( void );
    extern FILE   **__get_stdout( void );
    extern FILE   **__get_stderr( void );
  #endif
  #if defined( _THIN_LIB ) && defined( _NETWARE_LIBC )
    extern FILE   **___stdin ( void );
    extern FILE   **___stdout( void );
    extern FILE   **___stderr( void );
    extern FILE   **___cin   ( void );
    extern FILE   **___cout  ( void );
  #endif
#endif

#if !defined( __NETWARE__ ) || !defined( _THIN_LIB )

/*
 *  This code should be included on non-netware platforms and in cases where thin lib is not defined
 *  This will ensure that fat netware libraries will be calling the Watcom version of __get_std_stream
 */

_WCRTLINK FILE *__get_std_stream( unsigned handle )
{
    if( handle > NUM_STD_STREAMS ) {
        return( NULL );
    } else {
        return( &_RWD_iob[handle] );
    }
}

_WCRTLINK FILE *__get_std_file( unsigned handle )
{
    return( __get_std_stream( handle ) );
}

#else

#include <io.h>

#if defined( _NETWARE_LIBC )
    _WCRTLINK FILE *__get_std_stream( unsigned handle )
    {
        FILE    *pFile = NULL;

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
        return( pFile );
    }
#elif defined( _NETWARE_CLIB )
    _WCRTLINK FILE *__get_std_stream( unsigned handle )
    {
        FILE    *pFile = NULL;

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
        return( pFile );
    }
#endif

#endif


#if defined( __NETWARE__ ) && !defined( _THIN_LIB )

/*
 *  This code is for fat netware libraries. We are using the Watcom FILE *'s
 *  so this will be calling __get_std_stream from the top of this file.
 */

#include <io.h>

FILE **__get_stdin( void )
{
    static FILE         *stdin_ptr;

    stdin_ptr = __get_std_stream( STDIN_FILENO );
    return( &stdin_ptr );
}

FILE **__get_stdout( void )
{
    static FILE         *stdout_ptr;

    stdout_ptr = __get_std_stream( STDOUT_FILENO );
    return( &stdout_ptr );
}

FILE **__get_stderr( void )
{
    static FILE         *stderr_ptr;

    stderr_ptr = __get_std_stream( STDERR_FILENO );
    return( &stderr_ptr );
}

#endif
