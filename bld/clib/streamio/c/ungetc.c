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
* Description:  Implementation of ungetc() - push character back on stream.
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
#include "orient.h"
#ifdef __WIDECHAR__
    #include <mbstring.h>
    #include <string.h>
    #include <wchar.h>
#endif
#include "streamio.h"


_WCRTLINK INTCHAR_TYPE __F_NAME(ungetc,ungetwc)( INTCHAR_TYPE c, FILE *fp )
{
    if( c == __F_NAME(EOF,WEOF) ) {   /* cannot push EOF */
        return( c );
    }
    _ValidFile( fp, __F_NAME(EOF,WEOF) );
    _AccessFile( fp );

    /*** Deal with stream orientation ***/
    ORIENT_STREAM(fp,__F_NAME(EOF,WEOF));

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
            fp->_cnt = sizeof( wchar_t );
            fp->_ptr = _FP_BASE(fp) + fp->_bufsize - sizeof( wchar_t );
            fp->_flag |= _UNGET;                    /* 10-mar-90 */
            memcpy( fp->_ptr, &c, sizeof( wchar_t ) );
        } else if( fp->_ptr != _FP_BASE(fp) ) {
            fp->_cnt += sizeof( wchar_t );
            fp->_ptr -= sizeof( wchar_t );
            fp->_flag |= _UNGET;
            memcpy( fp->_ptr, &c, sizeof( wchar_t ) );
        } else {                        /* read buffer is full */
            _ReleaseFile( fp );
            return( WEOF );
        }
    } else {
        unsigned char   mbc[MB_CUR_MAX];
        int             mbcLen;

        /*** Convert the character to multibyte form ***/
        if( wctomb( (char *)mbc, c ) == -1 ) {
            __set_errno( EILSEQ );
            return( WEOF );
        }
        mbcLen = _mbclen( mbc );

        /*** Store the converted character ***/
        if( fp->_cnt == 0 ) {           /* read buffer is empty */
            fp->_cnt = mbcLen;
            fp->_ptr = _FP_BASE(fp) + fp->_bufsize - mbcLen;
            fp->_flag |= _UNGET;                            /* 10-mar-90 */
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
        fp->_cnt = 1;
        fp->_ptr = _FP_BASE(fp) + fp->_bufsize - 1;
        fp->_flag |= _UNGET;                                /* 10-mar-90 */
        *fp->_ptr = c;
    } else if( fp->_ptr != _FP_BASE(fp) ) {
        fp->_cnt++;
        fp->_ptr--;
        if( *fp->_ptr != c ) {
            fp->_flag |= _UNGET;                            /* 10-mar-90 */
        }
        *fp->_ptr = c;
    } else {                            /* read buffer is full */
        _ReleaseFile( fp );
        return( EOF );
    }
#endif
    fp->_flag &= ~ _EOF;

    _ReleaseFile( fp );
    return( (UCHAR_TYPE)c );
}
