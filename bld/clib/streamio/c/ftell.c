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
* Description:  Platform independent ftell() implementaiton.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "fileacc.h"


#ifdef __INT64__
_WCRTLINK long long _ftelli64( FILE *fp )
{
    long long   pos;
#else
_WCRTLINK long ftell( FILE *fp )
{
    long        pos;
#endif

    _ValidFile( fp, -1 );
    if( fp->_flag & _APPEND  &&  fp->_flag & _DIRTY ) {
        fflush( fp );   /* if data written in append mode, OS must know */
    }
#ifdef __INT64__
    pos = _telli64( fileno( fp ) );
    if( pos != -1LL ) {
#else
    pos = tell( fileno( fp ) );
    if( pos != -1L ) {
#endif
        _AccessFile( fp );
        if( fp->_cnt != 0 ) {                   /* if something in buffer */
            if( fp->_flag & _DIRTY ) {          /* last operation was a put */
                pos += fp->_cnt;
            } else {                            /* last operation was a get */
                pos -= fp->_cnt;
            }
        }
        _ReleaseFile( fp );
    }
    return( pos );
}
