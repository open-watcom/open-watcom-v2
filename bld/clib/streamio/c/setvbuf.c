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
* Description:  Platform independent setvbuf() implementation.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <stddef.h>
#include <limits.h>
#include "fileacc.h"
#include "rtdata.h"
#include "streamio.h"


_WCRTLINK int setvbuf( FILE *fp, char *buf, int mode, size_t size )
{
    __stream_check( fp, 1 );
    __null_check( buf, 2 );
    if( size > INT_MAX ) {
        /* 16-bit buffer filling code can't handle >32k (-ve size) */
        return( -1 );
    }
    /* check for allowable values for mode */
    switch( mode ) {
    case _IOFBF:
    case _IOLBF:
    case _IONBF:
        break;
    default:
        return( -1 );
    }
    if( buf != NULL && size == 0 ) {                /* JBS 27-aug-90 */
        return( -1 );
    }
    _ValidFile( fp, -1 );
    _AccessFile( fp );
    __chktty( fp );                                 /* JBS 28-aug-90 */
    if( size != 0 ) {
        fp->_bufsize = size;                        /* JBS 27-aug-90 */
    }
    _FP_BASE( fp ) = (unsigned char *)buf;
    fp->_ptr = (unsigned char *)buf;
    fp->_flag &= ~(_IONBF | _IOLBF | _IOFBF);       /* FWC 14-jul-87 */
    fp->_flag |= mode;
    if( buf == NULL ) {                             /* FWC 16-mar-93 */
        __ioalloc( fp );
    }
    _ReleaseFile( fp );
    return( 0 );
}
