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


#include <stdio.h>
#include <stdlib.h>

#include "tinyio.h"

void main( void )
{
    tiny_file_stamp_t   stamp;
    tiny_date_t         date;
    tiny_time_t         time;
    tiny_handle_t       fh;
    tiny_ret_t          rc;

    date = TinyGetDate();
    time = TinyGetTime();
    rc = TinyOpen( __FILE__, TIO_READ );
    if( TINY_ERROR( rc ) ) {
        puts( "open error" );
        exit( 1 );
    }
    fh = TINY_INFO( rc );
    rc = TinyGetFileStamp( fh, &stamp );
    printf( "current time is %02u:%02u:%02u\n", time.hour, time.minutes, time.seconds );
    printf( "current date is %02u-%02u-%02u\n", date.year, date.month, date.day_of_month );
    printf( "file stamp is: %02u-%02u-%02u\n", stamp.ud.date.year + 80, stamp.ud.date.month, stamp.ud.date.day );
    printf( "file stamp is: %02u:%02u:%02u\n", stamp.ut.time.hours, stamp.ut.time.minutes, stamp.ut.time.twosecs * 2 );
}
