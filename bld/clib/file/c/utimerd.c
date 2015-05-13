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
* Description:  RDOS implementation of utime().
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stddef.h>
#include <sys/types.h>
#include <time.h>
#include <dos.h>
#include <sys/stat.h>
#include <direct.h>
#include <sys/utime.h>
#include <rdos.h>
#include "rtdata.h"

_WCRTLINK int __F_NAME(utime,_wutime)( CHAR_TYPE const *fn, struct utimbuf const *times )
/**********************************************************************************/
{
    int                 handle;
    unsigned long       msb, lsb;
    struct tm          *tmptime;
    time_t              curr_time;
    struct utimbuf      time_buf;

    handle = RdosOpenFile( fn, 0 );

    if( handle == 0 )
        return( -1 );


    if( times == NULL ) {
        curr_time = time( NULL );
        time_buf.modtime = curr_time;
        time_buf.actime = curr_time;
        times = &time_buf;
    }

    tmptime = localtime( &time_buf.actime );

    tmptime->tm_year += 1900;
    tmptime->tm_mon++;

    msb = RdosCodeMsbTics(
                       tmptime->tm_year + 1900, 
                       tmptime->tm_mon + 1,
                       tmptime->tm_mday,
                       tmptime->tm_hour );

    lsb = RdosCodeLsbTics(
                       tmptime->tm_min,
                       tmptime->tm_sec,
                       0,
                       0 );
                           
    RdosSetFileTime( handle, msb, lsb );
    RdosCloseFile( handle );
    return( 0 );
}
