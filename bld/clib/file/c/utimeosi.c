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
* Description:  OSI implementation of utime().
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#include <sys/types.h>
#include <time.h>
#include "tinyio.h"
#include <sys/utime.h>


int utime( char const *fn, struct utimbuf const *times )
/******************************************************/
{
    int         handle;
    int         rc;
    struct tm   *split;
    time_t      curr_time;
    tiny_ftime_t ftime;
    tiny_fdate_t fdate;

    handle = TinyOpen( fn, TIO_READ_WRITE );
    if( handle < 0 ) {
        return( -1 );
    }
    if( times == NULL ) {
        curr_time = time( NULL );
        split = localtime( &curr_time );
    } else {
        split = localtime( &(times->modtime) );
    }
    if( split->tm_year < 80 ) {
        /* DOS file-system cannot handle dates before 1980 */
        TinyClose( handle );
        return( -1 );
    }
    ftime.hours   = split->tm_hour;
    ftime.minutes = split->tm_min;
    ftime.twosecs = split->tm_sec >> 1;
    fdate.year    = split->tm_year - 80;
    fdate.month   = split->tm_mon + 1;
    fdate.day     = split->tm_mday;
    rc = TinySetFileStamp( handle, ftime, fdate );
    TinyClose( handle );
    if( rc < 0 ) return( -1 );
    return( 0 );
}
