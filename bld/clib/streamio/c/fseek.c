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
* Description:  Implementation of fseek - set stream position.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <unistd.h>
#include "rtdata.h"
#include "fileacc.h"
#include "seterrno.h"
#include "flush.h"
#include "errorno.h"


#ifdef __INT64__
static int __update_buffer( long long diff, FILE *fp )
#else
static int __update_buffer( long diff, FILE *fp )
#endif
{
    /*
      diff is relative to fp->_ptr
      if diff is within the buffer update the pointers and return 0
      otherwise update the pointers and return 1
    */
    if( diff <= fp->_cnt  &&  diff >= (_FP_BASE(fp) - fp->_ptr) ) {
        fp->_flag &= ~(_EOF);
        fp->_ptr += diff;
        fp->_cnt -= diff;
        return( 0 );
    }
    return( 1 );
}


/*
 * This used to be in __update_buffer(), but we don't want to do this until
 * AFTER we've made certain that lseek() will be a successful one.
 */
static void __reset_buffer( FILE *fp )
{
    fp->_flag &= ~(_EOF);
    fp->_ptr = _FP_BASE(fp);
    fp->_cnt = 0;
}


#ifdef __INT64__
_WCRTLINK int _fseeki64( FILE *fp, long long offset, int origin )
#else
_WCRTLINK int fseek( FILE *fp, long offset, int origin )
#endif
{
    _ValidFile( fp, -1 );
    _AccessFile( fp );
    /*
      if the file is open for any sort of writing we must ensure that
      the buffer is flushed when dirty so that the integrity of the
      data is preserved.
      if there is an ungotten character in the buffer then the data must
      be discarded to ensure the integrity of the data
    */
    if( fp->_flag & (_WRITE | _UNGET) ) {
        if( fp->_flag & _DIRTY ) {
            /*
              the __flush routine aligns the DOS file pointer with the
              start of the resulting cleared buffer, as such, the SEEK_CUR
              code used for the non-_DIRTY buffer case is not required
            */
            if( __flush( fp ) ) {
                // assume __flush set the errno value
                // if erroneous input, override errno value
                if( origin == SEEK_SET && offset < 0 ) {
                    __set_errno( EINVAL );
                }
                _ReleaseFile( fp );
                return( -1 );
            }
        } else {
            if( origin == SEEK_CUR ) {
                offset -= fp->_cnt;
            }
            fp->_ptr = _FP_BASE(fp);
            fp->_cnt = 0;
        }
        fp->_flag &= ~(_EOF|_UNGET);
#ifdef __INT64__
        if( _lseeki64( fileno( fp ), offset, origin ) == -1 ) {
#else
        if( lseek( fileno( fp ), offset, origin ) == -1 ) {
#endif
            _ReleaseFile( fp );
            return( -1 );
        }
    } else {
        // file is open for read only,
        // no characters have been ungotten
        // the OS file pointer is at fp->_ptr + fp->_cnt relative to the
        // FILE* buffer
        switch( origin ) {
        case SEEK_CUR:
        {
            long    ptr_delta = fp->_cnt;

            if( __update_buffer( offset, fp ) ) {
                offset -= ptr_delta;
#ifdef __INT64__
                if( _lseeki64( fileno( fp ), offset, origin ) == -1 ) {
#else
                if( lseek( fileno( fp ), offset, origin ) == -1 ) {
#endif
                    _ReleaseFile( fp );
                    return( -1 );
                }
                __reset_buffer(fp);
            }
        }   break;
        case SEEK_SET:
        {
#ifdef __INT64__
            long long   file_ptr = _telli64( fileno( fp ) );
#else
            long        file_ptr = tell( fileno( fp ) );
#endif

            file_ptr -= fp->_cnt;
            if( __update_buffer( offset - file_ptr, fp ) ) {
#ifdef __INT64__
                if( _lseeki64( fileno( fp ), offset, origin ) == -1 ) {
#else
                if( lseek( fileno( fp ), offset, origin ) == -1 ) {
#endif
                    _ReleaseFile( fp );
                    return( -1 );
                }
                __reset_buffer(fp);
            }
        }   break;
        case SEEK_END:
            __reset_buffer(fp);
#ifdef __INT64__
            if( _lseeki64( fileno( fp ), offset, origin ) == -1 ) {
#else
            if( lseek( fileno( fp ), offset, origin ) == -1 ) {
#endif
                _ReleaseFile( fp );
                return( -1 );
            }
            break;
        default:
            __set_errno( EINVAL );
            _ReleaseFile( fp );
            return( -1 );
        }
    }
    _ReleaseFile( fp );
    return( 0 );            /* indicate success */
}
