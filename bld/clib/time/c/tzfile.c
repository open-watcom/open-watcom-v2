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
* Description:  Process /etc/localtime and friends; see tzfile(5)
*
****************************************************************************/

#include "variety.h"
#include <time.h>
#include "rtdata.h"
#include "timedata.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define TZif 0x545A6966

static long pntohl( const unsigned char *p )
{
    return( p[0] << 24 ) | ( p[1] << 16 ) | ( p[2] << 8 ) | p[3];
}

static unsigned char *tzfile = NULL;

void __check_tzfile( time_t t, struct tm *timep )
{
    long                tzh_timecnt;
    long                tzh_typecnt;
#if 0
    long                tzh_ttisgmtcnt;
    long                tzh_ttisstdcnt;
    long                tzh_leapcnt;
    long                tzh_charcnt;
#endif
    const char          *dstname;
    long                timidx;
    long                stdzon;
    long                dstzon;
    long                i;
    const unsigned char *tzp;
    int                 isdst;

    if( tzfile == NULL )
        return;
    tzp = tzfile + 16 + 4;
#if 0
    tzh_ttisgmtcnt = pntohl( tzp );
    tzh_ttisstdcnt = pntohl( tzp + 4 );
    tzh_leapcnt    = pntohl( tzp + 8 );
    tzh_charcnt    = pntohl( tzp + 20 );
#endif
    tzh_timecnt    = pntohl( tzp + 12 );
    tzh_typecnt    = pntohl( tzp + 16 );
    tzp += 24;
    timidx = 0;
    for( i = 0; i < tzh_timecnt; i++ ) {
        if( t >= (time_t)pntohl( tzp ) )
            timidx = i;
        tzp += 4;
    }
    stdzon = tzh_timecnt + tzp[timidx] * 6;
    isdst = tzp[stdzon + 4];
    if( timep != NULL )
        timep->tm_isdst = isdst;
    dstname = "\0";
    dstzon = stdzon;
    if( timidx > 0 ) {
        if( isdst )
            stdzon = tzh_timecnt + tzp[timidx - 1] * 6;
        else
            dstzon = tzh_timecnt + tzp[timidx - 1] * 6;
        dstname = (char *)&tzp[tzp[dstzon + 5] + tzh_timecnt + tzh_typecnt * 6];
        _RWD_dst_adjust = pntohl( &tzp[dstzon] ) - pntohl( &tzp[stdzon] );
    } else {
        _RWD_daylight = 0;  // daylight savings not supported
        _RWD_dst_adjust = 0;
    }
    _RWD_timezone = -pntohl( &tzp[stdzon] );
    strcpy( _RWD_tzname[0], (char *)&tzp[tzp[stdzon + 5] + tzh_timecnt + tzh_typecnt * 6] );
    strcpy( _RWD_tzname[1], dstname );
#if 0
    tzp += tzh_timecnt;
    tzp += tzh_typecnt * 6;
    tzp += tzh_charcnt;
    /* ignore leap seconds for now */
    tzp += tzh_leapcnt * 8;
    /* ignore standard/wall indicators for now */
    tzp += tzh_ttisstdcnt;
    /* ignore UTC/local indicators for now */
    tzp += tzh_ttisgmtcnt;
#endif
}

int __read_tzfile( const char *tz )
{
    long        fsize;
    int         fd;
    char        *filename = ( char * ) "/etc/localtime";

    if( tz != NULL ) {
        size_t const filenamelen = 21 + strlen( tz ) + 1;
        filename = alloca( filenamelen );
        if( filename == NULL )
            return( 0 );
        strcpy( filename, "/usr/share/zoneinfo/" );
        strcat( filename, tz );
    }

    fd = open( filename, O_RDONLY );
    fsize = lseek( fd, 0, SEEK_END );
    if( fsize == -1 )
        return( 0 );
    if( tzfile != NULL )
        free( tzfile );
    tzfile = malloc( (size_t)fsize );
    lseek( fd, 0, SEEK_SET );
    read( fd, tzfile, (unsigned int)fsize );
    close( fd );
    if( pntohl( tzfile ) != TZif ) {
        free( tzfile );
        tzfile = NULL;
        return( 0 );
    }
    __check_tzfile( time( NULL ), NULL );
    return( 1 );
}
