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
#include <errno.h>
#include "fileacc.h"
#include "rtdata.h"
#include "seterrno.h"
#ifdef __WIDECHAR__
    #include <mbstring.h>
    #include <wchar.h>
#endif


extern void __ioalloc( FILE * );
extern int  __flush( FILE * );


#ifndef __WIDECHAR__

_WCRTLINK int fputc( int c, FILE *fp )
{
    int flags;

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

    if( !(fp->_flag & _WRITE) ) {
        __set_errno( EBADF );
        fp->_flag |= _SFERR;
        _ReleaseFile( fp );
        return( EOF );
    }
    if( _FP_BASE(fp) == NULL ) {
        __ioalloc( fp );
    }
    flags = _IONBF;
    if( c == '\n' ) {
        flags = _IONBF | _IOLBF;
        #if !defined(__PENPOINT__)  &&  !defined(__QNX__)
            if( !(fp->_flag & _BINARY) ) {
                fp->_flag |= _DIRTY;
                *(char*)fp->_ptr = '\r';   /* '\n' -> '\r''\n' */
                fp->_ptr++;
                fp->_cnt++;
                if( fp->_cnt == fp->_bufsize ) {
                    if( __flush( fp ) ) {
                        _ReleaseFile( fp );
                        return( EOF );
                    }
                }
            }
        #endif
    }
    fp->_flag |= _DIRTY;
    *(char*)fp->_ptr = c;
    fp->_ptr++;
    fp->_cnt++;
    if( (fp->_flag & flags)  ||  (fp->_cnt == fp->_bufsize) ) {
        if( __flush( fp ) ) {
            _ReleaseFile( fp );
            return( EOF );
        }
    }
    _ReleaseFile( fp );
    return( (UCHAR_TYPE) c );
}


#else


static int __write_wide_char( FILE *fp, wchar_t wc )
/**************************************************/
{
    if( fp->_flag & _BINARY ) {
        /*** Dump the wide character ***/
        return( fwrite( &wc, sizeof(wchar_t), 1, fp ) );
    } else {
        char            mbc[MB_CUR_MAX];
        int             rc;

        /*** Convert the wide character to multibyte form and write it ***/
        rc = wctomb( mbc, wc );
        if( rc > 0 ) {
            return( fwrite( mbc, rc, 1, fp ) );
        } else {
            __set_errno( EILSEQ );
            return( 0 );
        }
    }
}


_WCRTLINK wint_t fputwc( wint_t c, FILE *fp )
{
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

    /*** Write the character ***/
    if( !__write_wide_char( fp, c ) ) {
        _ReleaseFile( fp );
        return( WEOF );
    } else {
        _ReleaseFile( fp );
        return( c );
    }
}

#endif
