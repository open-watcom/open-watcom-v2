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
#include "int64.h"
/* gross hack for building 11.0 libraries with 10.6 compiler */
#ifndef __WATCOM_INT64__
    #include <limits.h>         /* a gross hack to make a gross hack work */
    #define __WATCOM_INT64__
    #define __int64             double
#endif
/* most includes should go after this line */
#include <stdio.h>
#ifdef __QNX__
    #include <unistd.h>
#else
    #include <io.h>
#endif
#include "fileacc.h"
#include "rtcheck.h"


#if defined(__INT64__) && !defined(__NT__)


_WCRTLINK __int64 _filelengthi64( int handle )
{
    INT_TYPE            retval;
    long                size;

    __handle_check( handle, -1 );
    size = filelength( handle );
    if( size != -1 ) {
        _clib_U32ToU64( size, retval ); /* retval = (__int64)size */
    } else {
        _clib_I32ToI64( -1L, retval );  /* retval = (__int64)-1 */
    }
    RETURN_INT64(retval);
}


#else   /* defined(__INT64__) && !defined(__NT__) */


#ifdef __NETWARE__
 _WCRTLINK unsigned long filelength( int handle )
#else
 #ifdef __INT64__
  _WCRTLINK __int64 _filelengthi64( int handle )
 #else
  _WCRTLINK long filelength( int handle )
 #endif
#endif
    {
        LONG_TYPE               current_posn, file_len;
        #ifdef __INT64__
            INT_TYPE            zero, minusone;
            REAL_INT_TYPE       retval;
        #endif

        __handle_check( handle, -1 );
        _AccessFileH( handle );

        #ifdef __INT64__
            _clib_I32ToI64( 0L, zero );
            retval = _lseeki64( handle, GET_REALINT64(zero), SEEK_CUR );
            current_posn = GET_INT64(retval);
            _clib_I32ToI64( -1L, minusone );
            if( !_clib_U64Cmp(current_posn,minusone) ) {
                _ReleaseFileH( handle );
                RETURN_INT64(minusone);
            }

            retval = _lseeki64( handle, GET_REALINT64(zero), SEEK_END );
            file_len = GET_INT64(retval);

            _lseeki64( handle, GET_REALINT64(current_posn), SEEK_SET );

            _ReleaseFileH( handle );
            RETURN_INT64(file_len);
        #else
            current_posn = lseek( handle, 0L, SEEK_CUR );
            if( current_posn == -1L ) {
                _ReleaseFileH( handle );
                return( -1L );
            }
            file_len = lseek( handle, 0L, SEEK_END );
            lseek( handle, current_posn, SEEK_SET );

            _ReleaseFileH( handle );
            return( file_len );
        #endif
    }


#endif  /* defined(__INT64__) && !defined(__NT__) */
