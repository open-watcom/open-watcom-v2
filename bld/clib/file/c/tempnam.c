/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "variety.h"
#include "widechar.h"
#include <stdlib.h>
#include <unistd.h>
#if defined( CLIB_USE_MBCS_TRANSLATION )
    #include <mbstring.h>
#endif
#include <process.h>
#include <stdio.h>
#include <string.h>
#include "pathmac.h"
#include "_tcsstr.h"


#define UNIQUE                  5

static CHAR_TYPE *try_dir( const CHAR_TYPE *dir, const CHAR_TYPE *prefix )
{
    CHAR_TYPE *         buf;
    size_t              numchars;
    int                 addslash = 0;
    unsigned            number;
    int                 alive;

    /*** Ensure the directory exists ***/
    if( dir[0] != NULLCHAR ) {
        if( __F_NAME(access,_waccess)( dir, F_OK ) == -1 ) {
            return( NULL );
        }
    }

    /*** Initialize addslash ***/
    numchars = _TCSLEN( dir );
    if( numchars > 0 && prefix[0] != DIR_SEP ) {
        if( _TCSCMP( _TCSNINC( dir, numchars - 1 ), DIR_SEP_STR ) ) {
            addslash = 1;
        }
    }

    /*** Ensure the filename wouldn't be too long ***/
    /* dir + slash if needed + prefix + room for unique characters */
    /* strlen is ok for DBCS, since we want the byte count */
    numchars = __F_NAME(strlen,wcslen)( dir )  +  addslash  +  __F_NAME(strlen,wcslen)( prefix )  +  UNIQUE;
    if( numchars > _MAX_PATH )  return( NULL );

    /*** Try to build a unique filename ***/
    buf = malloc( CHARSIZE * (numchars+1) );
    for( number=1,alive=1; alive; number++ ) {
        __F_NAME(strcpy,wcscpy)( buf, dir );
        if( addslash ) {
            __F_NAME(strcat,wcscat)( buf, DIR_SEP_STR );
        }
        __F_NAME(strcat,wcscat)( buf, prefix );
        __F_NAME(utoa,_utow)( number, buf + __F_NAME(strlen,wcslen)(buf), 10 );

        if( __F_NAME(access,_waccess)( buf, F_OK )  !=  0 ) {
            return( buf );
        }
        if( number == 65535u )  alive = 0;
    }

    return( NULL );
}


_WCRTLINK CHAR_TYPE *__F_NAME(tempnam,_wtempnam)( const CHAR_TYPE *dir, const CHAR_TYPE *prefix )
{
    CHAR_TYPE *         p;
    const CHAR_TYPE *   envp;

    /*** Try the directory given by the TMP environment variable ***/
    envp = __F_NAME(getenv,_wgetenv)( STRING( "TMP" ) );
    if( envp != NULL ) {
        p = try_dir( envp, prefix );
        if( p != NULL )  return( p );
    }

    /*** Try the directory passed to this function (dir) ***/
    if( dir != NULL ) {
        p = try_dir( dir, prefix );
        if( p != NULL )  return( p );
    }

    /*** Try the P_tmpdir directory (#defined in stdio.h) ***/
    p = try_dir( __F_NAME(P_tmpdir,wP_tmpdir), prefix );
    if( p != NULL )  return( p );

    /*** Try the current directory ***/
    p = try_dir( STRING( "" ), prefix );
    if( p != NULL )  return( p );

    return( NULL );
}
