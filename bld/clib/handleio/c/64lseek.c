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


/*
 * This file is not indirected with an #include so we can compile this code
 * for multiple platforms without duplicating it in multiple files.
 */

#include "variety.h"
#define __INT64__
#include "int64.h"
#include <limits.h>
#ifdef __QNX__
    #include <unistd.h>
#else
    #include <io.h>
#endif


_WCRTLINK __int64 _lseeki64( int hid, __int64 _offset, int origin )
{
    long                rc;
    INT_TYPE            offset = GET_INT64(_offset);
    INT_TYPE            minoff, maxoff;
    INT_TYPE            minusone;
    INT_TYPE            retval;
    INT_TYPE            negminoff, negoffset;
    long                offset32;

    /*** Fail if offset isn't in the interval [LONG_MIN,LONG_MAX] ***/
    _clib_I32ToI64( -1L, minusone );
    _clib_I32ToI64( LONG_MAX, maxoff );
    if( _clib_U64Cmp( offset, maxoff )  >  0 ) {
        RETURN_INT64(minusone);         /* offset > LONG_MAX */
    }
    _clib_I32ToI64( LONG_MIN, minoff );
    _clib_I64Neg( minoff, negminoff );
    _clib_I64Neg( offset, negoffset );
    if( _clib_U64Cmp( offset, maxoff )  >  0 ) {
        RETURN_INT64(minusone);         /* offset < LONG_MIN */
    }

    /*** Ok, now call lseek ***/
    _clib_I64ToLong( offset, offset32 );
    rc = lseek( hid, offset32, origin );
    _clib_I32ToI64( rc, retval );
    RETURN_INT64(retval);
}
