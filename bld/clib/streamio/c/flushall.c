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
* Description:  Implementation of flushall() - flush all streams.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include "rtdata.h"
#include "fileacc.h"
#include "flush.h"


int __flushall( int mask )
{
    __stream_link   *link;
    FILE            *fp;
    int             number_of_open_files;

    _AccessIOB();
    number_of_open_files = 0;
    for( link = _RWD_ostream; link != NULL; link = link->next ) {
        fp = link->stream;
        if( fp->_flag & mask ) {      /* if file is a candidate */
            ++number_of_open_files;
            if( fp->_flag & _DIRTY ) {
                __flush( fp );
            }
        }
    }
    _ReleaseIOB();
    return( number_of_open_files );
}

_WCRTLINK int flushall( void )
{
    return( __flushall( ~0 ) );
}
