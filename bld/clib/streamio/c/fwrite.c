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
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "fileacc.h"
#include "rtdata.h"
#include "seterrno.h"
#include "qwrite.h"

extern void __ioalloc( FILE *fp );
extern int __flush( FILE * );


_WCRTLINK size_t fwrite( const void *buf, size_t size, size_t n, FILE *fp )
{
    size_t count;
    unsigned    oflag;

    _ValidFile( fp, 0 );
    _AccessFile( fp );
    if(( fp->_flag & _WRITE) == 0 ) {
        __set_errno( EBADF );
        fp->_flag |= _SFERR;
        _ReleaseFile( fp );
        return((size_t) 0 ); /* POSIX says return 0 */
    }
    n *= size;
    if( n == 0 ) {
        _ReleaseFile( fp );
        return( n );
    }
    if( _FP_BASE(fp) == NULL ) {
        __ioalloc( fp );                    /* allocate buffer */
    }
    oflag = fp->_flag & (_SFERR|_EOF);      /* JBS 27-jan-92 */
    fp->_flag &= ~(_SFERR|_EOF);                    /* JBS 27-jan-92 */
    count = 0;
#if !defined(__PENPOINT__)  &&  !defined(__QNX__)
    if( fp->_flag & _BINARY ) {             /* binary I/O */
#else
    {
#endif
        size_t bytes_left = n, bytes;

        do
        {
            /* if our buffer is empty, and user's buffer is larger,
               then write directly from user's buffer.  28-apr-90 */

            if( fp->_cnt == 0  &&  bytes_left >= fp->_bufsize ) {
                bytes = bytes_left & -512;          /* multiple of 512 */
                if( bytes == 0 ) bytes = bytes_left;/* bufsize < 512   */
                n = __qwrite( fileno(fp), buf, bytes );
                if( n == -1 ) {
                    fp->_flag |= _SFERR;
                }
                #if !defined(__QNX__)
                    else if( n == 0 ) {
                        _RWD_errno = ENOSPC;
                        fp->_flag |= _SFERR;
                    }
                #endif
                bytes = n;
            } else {
                bytes = fp->_bufsize - fp->_cnt;
                if( bytes > bytes_left )  bytes = bytes_left;
                memcpy( fp->_ptr, buf, bytes );
                fp->_ptr += bytes;
                fp->_cnt += bytes;
                fp->_flag |= _DIRTY;
                if(( fp->_cnt == fp->_bufsize ) || ( fp->_flag & _IONBF )) {
                    __flush(fp);
                }
            }
            buf = ((const char *)buf) + bytes;
            count += bytes;
            bytes_left -= bytes;
        } while( bytes_left && !ferror( fp ) );
#if !defined(__PENPOINT__)  &&  !defined(__QNX__)
    } else {        /* text I/O */
        const char *    bufptr;
        int             not_buffered;
        #ifndef __NETWARE__
            int         old_orientation;
        #endif
        /* temporarily enable buffering saving the previous setting */
        not_buffered = 0;
        if( fp->_flag & _IONBF ) {
            not_buffered = 1;
            fp->_flag &= ~_IONBF;
            fp->_flag |= _IOFBF;
        }

        /*** Use fputc, and make it think the stream is byte-oriented ***/
        #ifndef __NETWARE__
            old_orientation = _FP_ORIENTATION(fp);
            _FP_ORIENTATION(fp) = _BYTE_ORIENTED;
        #endif
        bufptr = (const char *)buf;
        do {
            fputc( *(bufptr++), fp );
            if( fp->_flag & (_EOF|_SFERR) ) break;
            ++count;
        } while( count != n );
        #ifndef __NETWARE__
            _FP_ORIENTATION(fp) = old_orientation;
        #endif

        if( not_buffered ) {        /* if wasn't buffered, then reset */
            fp->_flag &= ~_IOFBF;
            fp->_flag |= _IONBF;
            __flush( fp );
        }
#endif
    }
    if( fp->_flag & _SFERR ) {
        /*
         * Quantum 11-17-92 Temporary buffering confuses the return
         *                  value if the call is interrupted.
         *                  kludge: return 0 on error
         */
        count = 0;
    }
    fp->_flag |= oflag;                     /* JBS 27-jan-92 */
    _ReleaseFile( fp );
    return( count / size );
}
