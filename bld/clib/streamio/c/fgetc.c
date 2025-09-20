/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Platform independent fgetc() implementation.
*
****************************************************************************/


#define __FUNCTION_DATA_ACCESS
#include "variety.h"
#include "widechar.h"
#include "seterrno.h"
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#ifndef __UNIX__
    #include <conio.h>
#endif
#ifdef __WIDECHAR__
    #include <mbstring.h>
#endif
#include "libwchar.h"
#if defined( __NT__ )
    #include <windows.h>
#elif defined( __OS2__ )
    #include <wos2.h>
#elif defined( __NETWARE__ )
    #include "nw_lib.h"
#endif
#include "rtdata.h"
#include "fileacc.h"
#include "qread.h"
#include "orient.h"
#include "_flush.h"
#include "streamio.h"
#include "thread.h"
#include "fillbuf.h"


#define DOS_EOF_CHAR        0x1a

#ifndef __WIDECHAR__

int _WCNEAR __fill_buffer( FILE *fp )
{
    if( _FP_BASE( fp ) == NULL ) {
        __ioalloc( fp );
    }
    if( fp->_flag & _ISTTY ) {
        if( fp->_flag & (_IONBF | _IOLBF) ) {
            __flushall( _ISTTY );           /* flush all TTY output */
        }
    }
    fp->_flag &= ~_UNGET;
    fp->_ptr = _FP_BASE( fp );
#ifdef __UNIX__
    fp->_cnt = __qread( fileno( fp ), fp->_ptr, (fp->_flag & _IONBF) ? 1 : fp->_bufsize );
#else
    if( (fp->_flag & (_IONBF | _ISTTY)) == (_IONBF | _ISTTY)
      && ( fileno( fp ) == STDIN_FILENO ) ) {
        int c;

        fp->_cnt = 0;
        c = getche();
        if( c != EOF ) {
            *fp->_ptr = c;
            fp->_cnt = 1;
        }
    } else {
        fp->_cnt = __qread( fileno( fp ), fp->_ptr,
            (fp->_flag & _IONBF) ? 1 : fp->_bufsize );
    }
#endif
    if( fp->_cnt <= 0 ) {
        if( fp->_cnt == 0 ) {
            fp->_flag |= _EOF;
        } else {
            fp->_flag |= _SFERR;
            fp->_cnt = 0;
        }
    }
    return( fp->_cnt );
}

static int _WCNEAR __filbuf( FILE *fp )
{
    if( __fill_buffer( fp ) == 0 ) {
        return( EOF );
    } else {
        fp->_cnt--;
        fp->_ptr++;
        return( *(fp->_ptr - 1) );
    }
}

_WCRTLINK int fgetc( FILE *fp )
{
    int c;

    _ValidFile( fp, EOF );
    _AccessFile( fp );

    /*** Deal with stream orientation ***/
    ORIENT_STREAM( fp, EOF );

    if( (fp->_flag & _READ) == 0 ) {
        lib_set_errno( EBADF );
        fp->_flag |= _SFERR;
        c = EOF;
    } else {
        fp->_cnt--;
        // it is important that this remain a relative comparison
        // to ensure that the getc() macro works properly
        if( fp->_cnt < 0 ) {
            c = __filbuf( fp );
        } else {
            c = *fp->_ptr;
            fp->_ptr++;
        }
    }
#ifndef __UNIX__
    if( (fp->_flag & _BINARY) == 0 ) {
        if( c == '\r' ) {
            fp->_cnt--;
            // it is important that this remain a relative comparison
            // to ensure that the getc() macro works properly
            if( fp->_cnt < 0 ) {
                c = __filbuf( fp );
            } else {
                c = *fp->_ptr;
                fp->_ptr++;
            }
        }
        if( c == DOS_EOF_CHAR ) {
            fp->_flag |= _EOF;
            c = EOF;
        }
    }
#endif
    _ReleaseFile( fp );
    return( c );
}

#else

static int _WCNEAR __read_wide_char( FILE *fp, wchar_t *wc )
/**********************************************************/
{
    if( fp->_flag & _BINARY ) {
        /*** Read a wide character ***/
        return( fread( wc, sizeof( wchar_t ), 1, fp ) );
    } else {
        char            mbc[MB_CUR_MAX];
        wchar_t         wcTemp;
        int             rc;

        /*** Read the multibyte character ***/
        if( fread( &mbc[0], 1, 1, fp ) == 0 )
            return( 0 );

        if( _ismbblead( (unsigned char)mbc[0] ) ) {
            if( fread( &mbc[1], 1, 1, fp ) == 0 ) {
                return( 0 );
            }
        }

        /*** Convert it to wide form ***/
        rc = mbtowc( &wcTemp, mbc, MB_CUR_MAX );
        if( rc >= 0 ) {
            *wc = wcTemp;
            return( 1 );
        } else {
            lib_set_errno( EILSEQ );
            return( 0 );
        }
    }
}

_WCRTLINK wint_t fgetwc( FILE *fp )
{
    wchar_t             c;

    _ValidFile( fp, WEOF );
    _AccessFile( fp );

    /*** Deal with stream orientation ***/
    ORIENT_STREAM( fp, WEOF );

    /*** Read the character ***/
    if( __read_wide_char( fp, &c ) == 0 ) {
        _ReleaseFile( fp );
        return( WEOF );
    }
    if( (fp->_flag & _BINARY) == 0
      && (c == L'\r') ) {
        if( __read_wide_char( fp, &c ) == 0 ) {
            _ReleaseFile( fp );
            return( WEOF );
        }
    }

    _ReleaseFile( fp );
    return( (wint_t)c );
}

#endif
