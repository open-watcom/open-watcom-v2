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
#include <errno.h>
#include <stdio.h>
#include "rtdata.h"
#include "fileacc.h"
#include "exitwmsg.h"
#include "liballoc.h"
#include "seterrno.h"
#ifdef __WIDECHAR__
    #include <mbstring.h>
    #include <string.h>
    #include <wchar.h>
#endif

extern void __ioalloc( FILE * );


#ifndef __WIDECHAR__
    _WCRTLINK int    (ungetc)( int c, FILE *fp ) /* push one character back onto file fp */
#else
    _WCRTLINK wint_t (ungetwc)( wint_t c, FILE *fp )
#endif
    {
        if( c == __F_NAME(EOF,WEOF) ) {   /* cannot push EOF */
            return( c );
        }
        _ValidFile( fp, __F_NAME(EOF,WEOF) );
        _AccessFile( fp );

        /*** Deal with stream orientation ***/
        #ifndef __NETWARE__
            #ifdef __WIDECHAR__
                if( _FP_ORIENTATION(fp) != _WIDE_ORIENTED ) {
                    if( _FP_ORIENTATION(fp) == _NOT_ORIENTED ) {
                        _FP_ORIENTATION(fp) = _WIDE_ORIENTED;
                    } else {
                        _ReleaseFile( fp );
                        return( WEOF );
                    }
                }
            #else
                if( _FP_ORIENTATION(fp) != _BYTE_ORIENTED ) {
                    if( _FP_ORIENTATION(fp) == _NOT_ORIENTED ) {
                        _FP_ORIENTATION(fp) = _BYTE_ORIENTED;
                    } else {
                        _ReleaseFile( fp );
                        return( EOF );
                    }
                }
            #endif
        #endif

        if( fp->_flag & _DIRTY ) {        /* cannot unget after a put */
            _ReleaseFile( fp );
            return( __F_NAME(EOF,WEOF) );
        }
        if(( fp->_flag & _READ ) == 0 ) { /* not open for input */
            _ReleaseFile( fp );
            return( __F_NAME(EOF,WEOF) );
        }
        if( _FP_BASE(fp) == NULL ) {      /* no buffer allocated */
            __ioalloc( fp );
        }
        #ifdef __WIDECHAR__
            if( fp->_flag & _BINARY ) {
                /*** Leave the character in wide form ***/
                if( fp->_cnt == 0 ) {           /* read buffer is empty */
                    fp->_cnt = sizeof(wchar_t);
                    fp->_ptr = _FP_BASE(fp) + fp->_bufsize - sizeof(wchar_t);
                    fp->_flag |= _UNGET;                    /* 10-mar-90 */
                    memcpy( fp->_ptr, &c, sizeof(wchar_t) );
                } else if( fp->_ptr != _FP_BASE(fp) ) {
                    fp->_cnt += sizeof(wchar_t);
                    fp->_ptr -= sizeof(wchar_t);
                    fp->_flag |= _UNGET;
                    memcpy( fp->_ptr, &c, sizeof(wchar_t) );
                } else {                        /* read buffer is full */
                    _ReleaseFile( fp );
                    return( WEOF );
                }
            } else {
                char    mbc[MB_CUR_MAX];
                int     mbcLen;

                /*** Convert the character to multibyte form ***/
                if( wctomb( mbc, c ) == -1 ) {
                    __set_errno( EILSEQ );
                    return( WEOF );
                }
                mbcLen = _mbclen( mbc );

                /*** Store the converted character ***/
                if( fp->_cnt == 0 ) {           /* read buffer is empty */
                    fp->_cnt = mbcLen;
                    fp->_ptr = _FP_BASE(fp) + fp->_bufsize - mbcLen;
                    fp->_flag |= _UNGET;                    /* 10-mar-90 */
                    _mbccpy( fp->_ptr, mbc );
                } else if( fp->_ptr != _FP_BASE(fp) ) {
                    fp->_cnt += mbcLen;
                    fp->_ptr -= mbcLen;
                    fp->_flag |= _UNGET;
                    _mbccpy( fp->_ptr, mbc );
                } else {                        /* read buffer is full */
                    _ReleaseFile( fp );
                    return( WEOF );
                }
            }
        #else
            if( fp->_cnt == 0 ) {               /* read buffer is empty */
                fp->_cnt = CHARSIZE;
                fp->_ptr = _FP_BASE(fp) + fp->_bufsize - CHARSIZE;
                fp->_flag |= _UNGET;                                 /* 10-mar-90 */
                *(CHAR_TYPE*)(fp->_ptr) = c;
            } else if( fp->_ptr != _FP_BASE(fp) ) {
                fp->_cnt += CHARSIZE;
                fp->_ptr -= CHARSIZE;
                if( *(CHAR_TYPE*)(fp->_ptr) != c )  fp->_flag |= _UNGET; /* 10-mar-90 */
                *(CHAR_TYPE*)(fp->_ptr) = c;
            } else {                            /* read buffer is full */
                _ReleaseFile( fp );
                return( EOF );
            }
        #endif
        fp->_flag &= ~ _EOF;

        _ReleaseFile( fp );
        return( (UCHAR_TYPE) c );
    }
