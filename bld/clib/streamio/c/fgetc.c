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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#if defined(__PENPOINT__)  ||  defined(__QNX__)
#include <unistd.h>
#else
#include <conio.h>
#include <io.h>
#endif
#include "fileacc.h"
#include <errno.h>
#include "rtdata.h"
#include "seterrno.h"
#ifdef __WIDECHAR__
    #include <mbstring.h>
    #include <wchar.h>
#endif

extern  void    __ioalloc( FILE * );
extern  int     __flushall( int );

#if defined(__PENPOINT__)
 #define __qread( h, b, l ) (read( h, b, l ) )
#elif defined(__QNX__)
 extern int  __qread( int handle, char *buffer, unsigned len );
#elif defined(NETWARE)
 #define DOS_EOF_CHAR   0x1a
 #define __qread( h, b, l ) read( h, b, l )
#else
 #define DOS_EOF_CHAR   0x1a
 extern int  __qread( int handle, char *buffer, unsigned len );
#endif

#if defined( __WIDECHAR__ )
 #define CHARMASK       0xffff
#else
 #define CHARMASK       0xff
#endif


#ifndef __WIDECHAR__

_WCRTLINK int fgetc( FILE *fp )
{
    int c;

    _ValidFile( fp, EOF );
    _AccessFile( fp );

    /*** Deal with stream orientation ***/
    #ifndef __NETWARE__
        if( _FP_ORIENTATION(fp) != _BYTE_ORIENTED ) {
            if( _FP_ORIENTATION(fp) == _NOT_ORIENTED ) {
                _FP_ORIENTATION(fp) = _BYTE_ORIENTED;
            } else {
                _ReleaseFile( fp );
                return( EOF );              /* error return */
            }
        }
    #endif

    if(( fp->_flag & _READ ) == 0 ) {
        __set_errno( EBADF );
        fp->_flag |= _SFERR;
        c = EOF;
    } else {
#if 0
        /*** If the buffer is _DIRTY, resync it before reading ***/
        if( fp->_flag & (_WRITE|_UNGET) ) {
            if( fp->_flag & _DIRTY ) {
                fseek( fp, 0L, SEEK_CUR );
            }
        }
#endif

        fp->_cnt--;
        // it is important that this remain a relative comparison
        // to ensure that the getc() macro works properly
        if( fp->_cnt < 0 ) {
            c = __F_NAME(__filbuf,__wfilbuf)(fp);
        } else {
            c = *(char*)fp->_ptr;
            fp->_ptr++;
        }
    }
    #if !defined(__PENPOINT__) && !defined(__QNX__)
        if( ! (fp->_flag & _BINARY) ) {
            if( c == '\r' ) {
                fp->_cnt--;
                // it is important that this remain a relative comparison
                // to ensure that the getc() macro works properly
                if( fp->_cnt < 0 ) {
                    c = __F_NAME(__filbuf,__wfilbuf)(fp);
                } else {
                    c = *(CHAR_TYPE*)fp->_ptr & CHARMASK;
                    fp->_ptr += CHARSIZE;
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


static int __read_wide_char( FILE *fp, wchar_t *wc )
/**************************************************/
{
    if( fp->_flag & _BINARY ) {
        /*** Read a wide character ***/
        return( fread( wc, sizeof(wchar_t), 1, fp ) );
    } else {
        char            mbc[MB_CUR_MAX];
        wchar_t         wcTemp;
        int             rc;

        /*** Read the multibyte character ***/
        if( !fread( &mbc[0], 1, 1, fp ) )  return( 0 );
        if( _ismbblead( mbc[0] ) ) {
            if( !fread( &mbc[1], 1, 1, fp ) )  return( 0 );
        }

        /*** Convert it to wide form ***/
        rc = mbtowc( &wcTemp, mbc, MB_CUR_MAX );
        if( rc >= 0 ) {
            *wc = wcTemp;
            return( 1 );
        } else {
            __set_errno( EILSEQ );
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
    #ifndef __NETWARE__
        if( _FP_ORIENTATION(fp) != _WIDE_ORIENTED ) {
            if( _FP_ORIENTATION(fp) == _NOT_ORIENTED ) {
                _FP_ORIENTATION(fp) = _WIDE_ORIENTED;
            } else {
                _ReleaseFile( fp );
                return( WEOF );             /* error return */
            }
        }
    #endif

    /*** Read the character ***/
    if( !__read_wide_char( fp, &c ) ) {
        _ReleaseFile( fp );
        return( WEOF );
    }
    if( !(fp->_flag & _BINARY)  &&  c == L'\r' ) {
        if( !__read_wide_char( fp, &c ) ) {
            _ReleaseFile( fp );
            return( WEOF );
        }
    }

    _ReleaseFile( fp );
    return( (wint_t)c );
}

#endif


int __F_NAME(__filbuf,__wfilbuf)( FILE *fp )
{
    if( __F_NAME(__fill_buffer,__wfill_buffer)( fp ) == 0 ) {
        return( EOF );
    } else {
        fp->_cnt -= CHARSIZE;
        fp->_ptr += CHARSIZE;
        return( *(CHAR_TYPE*)(fp->_ptr - CHARSIZE) & CHARMASK );
    }
}

int __F_NAME(__fill_buffer,__wfill_buffer)( FILE *fp )
{
    if( _FP_BASE(fp) == NULL ) {
        __ioalloc( fp );
    }
    if( fp->_flag & _ISTTY ) {                      /* 20-aug-90 */
        if( fp->_flag & (_IONBF | _IOLBF) ) {
            __flushall( _ISTTY );           /* flush all TTY output */
        }
    }
    fp->_flag &= ~_UNGET;                           /* 10-mar-90 */
    fp->_ptr = _FP_BASE(fp);
#if defined(__PENPOINT__)
    fp->_cnt = __qread( fileno( fp ), fp->_ptr,
                        (fp->_flag & _IONBF) ? CHARSIZE : fp->_bufsize );
#elif defined(__QNX__)
    fp->_cnt = __qread( fileno( fp ), fp->_ptr,
                        (fp->_flag & _IONBF) ? CHARSIZE : fp->_bufsize );
#else
    if(( fp->_flag & (_IONBF|_ISTTY)) == (_IONBF|_ISTTY) &&
#ifndef __NETWARE__
       ( fileno( fp ) == STDIN_FILENO )) {
#else
       ( fileno( fp ) == STDIN_HANDLE )) {
#endif
        int c;                      /* JBS 31-may-91 */

        fp->_cnt = 0;
        c = getche();
        if( c != EOF ) {
            *(CHAR_TYPE*)fp->_ptr = c;
            fp->_cnt = CHARSIZE;
        }
    } else {
#ifndef __NETWARE__
        fp->_cnt = __qread( fileno( fp ), fp->_ptr,
                            (fp->_flag & _IONBF) ? CHARSIZE : fp->_bufsize );
#else
        fp->_cnt = read( fileno( fp ), fp->_ptr,
                             (fp->_flag & _IONBF) ? CHARSIZE : fp->_bufsize );
#endif
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
