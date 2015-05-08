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
* Description:  Implementation of fread() - read data from stream.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "rtdata.h"
#include "fileacc.h"
#include "seterrno.h"
#include "qread.h"
#include "streamio.h"
#include "errorno.h"


#define DOS_EOF_CHAR    0x1a

extern int  __fill_buffer( FILE * );    /* located in fgetc */


_WCRTLINK size_t fread( void *_buf, size_t size, size_t n, FILE *fp )
{
    unsigned char   *buf = _buf;
    size_t          len_read;

    _ValidFile( fp, 0 );
    _AccessFile( fp );
    if( (fp->_flag & _READ) == 0 ) {
        __set_errno( EBADF );
        fp->_flag |= _SFERR;
        _ReleaseFile( fp );
        return( 0 );
    }

#if 0
    /*** If the buffer is _DIRTY, resync it before reading ***/
    if( fp->_flag & (_WRITE | _UNGET) ) {
        if( fp->_flag & _DIRTY ) {
            fseek( fp, 0, SEEK_CUR );
        }
    }
#endif

    n *= size;
    if( n == 0 ) {
        _ReleaseFile( fp );
        return( n );
    }
    if( _FP_BASE(fp) == NULL ) {
        __ioalloc( fp );                        /* allocate buffer */
    }
    len_read = 0;
#if !defined( __UNIX__ )
    if( fp->_flag & _BINARY )
#endif
    {
        size_t bytes_left = n, bytes;
        for( ;; ) {
            if( fp->_cnt != 0 ) {
                bytes = fp->_cnt;
                if( bytes > bytes_left ) {
                    bytes = bytes_left;
                }
                memcpy( buf, fp->_ptr, bytes );
                fp->_ptr += bytes;
                buf += bytes;
                fp->_cnt -= bytes;
                bytes_left -= bytes;
                len_read += bytes;
            }
            if( bytes_left == 0 ) break;

            /* if user's buffer is larger than our buffer, OR
               _IONBF is set, then read directly into user's buffer. */

            if( (bytes_left >= fp->_bufsize) || (fp->_flag & _IONBF) ) {
                bytes = bytes_left;
                fp->_ptr = _FP_BASE(fp);
                fp->_cnt = 0;
                if( !(fp->_flag & _IONBF) ) {
                    /* if more than a sector, set to multiple of sector size*/
                    if( bytes > 512 ) {
                        bytes &= -512;
                    }
                }
                n = __qread( fileno(fp), buf, bytes );
                if( n == -1 ) {
                    fp->_flag |= _SFERR;
                    break;
                } else if( n == 0 ) {
                    fp->_flag |= _EOF;
                    break;
                }
                buf += n;
                bytes_left -= n;
                len_read += n;
            } else {
                if( __fill_buffer( fp ) == 0 )  break;
            }
        } /* end for */
#if !defined(__UNIX__)
    } else {
        for( ;; ) {
            int c;

            // ensure non-empty buffer
            if( fp->_cnt == 0 ) {
                if( __fill_buffer( fp ) == 0 ) break;
            }
            // get character
            --fp->_cnt;
            c = *fp->_ptr;
            fp->_ptr++;
            // perform new-line translation
            if( c == '\r' ) {
                // ensure non-empty buffer
                if( fp->_cnt == 0 ) {
                    if( __fill_buffer( fp ) == 0 ) break;
                }
                // get character
                --fp->_cnt;
                c = *fp->_ptr;
                fp->_ptr++;
            }
            // check for DOS end of file marker
            if( c == DOS_EOF_CHAR ) {
                fp->_flag |= _EOF;
                break;
            }
            // store chracter
            buf[len_read] = (char)c;
            ++len_read;
            if( len_read == n ) break;
        }
#endif
    }
    _ReleaseFile( fp );
    return( len_read / size );
}
