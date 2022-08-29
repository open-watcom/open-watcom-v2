/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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

void __check_tzfile( unsigned char *tzdata, time_t t, struct tm *timep )
{
//    long                tzh_ttisutccnt;
//    long                tzh_ttisstdcnt;
//    long                tzh_leapcnt;
    long                tzh_timecnt;
    long                tzh_typecnt;
//    long                tzh_charcnt;
    long                timidx;
    long                stdzon;
    long                dstzon;
    long                i;
    const unsigned char *tzp;
    int                 isdst;
    char                version;

    tzp = tzdata;
    if( tzp == NULL ) {
        tzp = tzfile;
        if( tzp == NULL ) {
            return;
        }
    }
    /*
     * process header
     */
//    signature      = pntohl( tzp );
    version        = *( tzp + 4 );
    if( version == 0 ) {
        version = '1';
    }
//    tzh_ttisutccnt = pntohl( tzp + 20);
//    tzh_ttisstdcnt = pntohl( tzp + 24 );
//    tzh_leapcnt    = pntohl( tzp + 28 );
    tzh_timecnt    = pntohl( tzp + 32 );
    tzh_typecnt    = pntohl( tzp + 36 );
//    tzh_charcnt    = pntohl( tzp + 40 );
    tzp += 44;
    /*
     * Version 1 data
     *
     * // transition times
     * tzp += tzh_timecnt * 4;
     * // transition types
     * tzp += tzh_timecnt;
     * // local time types
     * tzp += tzh_typecnt * 6;
     * // time zone designations
     * tzp += tzh_charcnt;
     * // ignore leap seconds for now
     * tzp += tzh_leapcnt * 8;
     * // ignore standard/wall indicators for now
     * tzp += tzh_ttisstdcnt;
     * // ignore UTC/local indicators for now
     * tzp += tzh_ttisutcnt;
     */

    /*
     * find time in transition times table
     */
    timidx = 0;
    for( i = 0; i < tzh_timecnt; i++ ) {
        if( t >= (time_t)pntohl( tzp ) )
            timidx = i;
        tzp += 4;
    }
    /*
     * get timezone info
     */
    stdzon = tzh_timecnt + tzp[timidx] * 6;
    isdst = tzp[stdzon + 4];
    if( timep != NULL )
        timep->tm_isdst = isdst;
    /*
     * update current timezone data
     */
    if( tzdata != NULL ) {
        /*
         * save new timezone data
         */
        if( tzfile != NULL )
            free( tzfile );
        tzfile = tzdata;
        /*
         * update current timezone data by new one
         */
        dstzon = stdzon;
        if( timidx > 0 ) {
            if( isdst ) {
                stdzon = tzh_timecnt + tzp[timidx - 1] * 6;
            } else {
                dstzon = tzh_timecnt + tzp[timidx - 1] * 6;
            }
        }
        if( dstzon != stdzon ) {
            _RWD_daylight = 1;
            _RWD_dst_adjust = pntohl( &tzp[dstzon] ) - pntohl( &tzp[stdzon] );
        } else {
            _RWD_daylight = 0;
            _RWD_dst_adjust = 0;
        }
        _RWD_timezone = -pntohl( &tzp[stdzon] );
        strcpy( _RWD_tzname[0], (char *)&tzp[tzp[stdzon + 5] + tzh_timecnt + tzh_typecnt * 6] );
        strcpy( _RWD_tzname[1], (char *)&tzp[tzp[dstzon + 5] + tzh_timecnt + tzh_typecnt * 6] );
    }
}

#define DEFAULT_ZONEFILE    "/etc/localtime"
#define DEFAULT_ZONEDIR     "/usr/share/zoneinfo/"

int __read_tzfile( const char *tz )
/**********************************
 * - if no file name specified in TZ then
 * use system default file "/etc/localtime"
 * - if file name is specified with absolute path
 * then use it
 * - otherwise location "/usr/share/zoneinfo/"
 * is used for specified file
 */
{
    long            fsize;
    int             fd;
    char            *filename = DEFAULT_ZONEFILE;
    size_t          filenamelen;
    int             rc;
    unsigned char   *tzdata;

    rc = 0;
    if( tz != NULL ) {
        if( *tz == ':' )
            tz++;
        if( *tz != '\0' ) {
            filenamelen = strlen( tz ) + 1;
            if( *tz != '/' ) {
                /*
                 * relative path, add DEFAULT_ZONEDIR
                 */
                filenamelen += sizeof( DEFAULT_ZONEDIR ) - 1;
            }
            filename = alloca( filenamelen );
            if( filename != NULL ) {
                *filename = '\0';
                if( *tz != '/' ) {
                    /*
                     * relative path, add DEFAULT_ZONEDIR
                     */
                    strcpy( filename, DEFAULT_ZONEDIR );
                }
                strcat( filename, tz );
            }
        }
    }

    if( filename != NULL ) {
        fd = open( filename, O_RDONLY );
        if( fd != -1 ) {
            fsize = lseek( fd, 0, SEEK_END );
            if( fsize != -1 ) {
                tzdata = malloc( (size_t)fsize );
                if( tzdata != NULL ) {
                    lseek( fd, 0, SEEK_SET );
                    *tzdata = '\0';
                    read( fd, tzdata, (size_t)fsize );
                    if( pntohl( tzdata ) == TZif ) {
                        __check_tzfile( tzdata, time( NULL ), NULL );
                        rc = 1;
                    } else {
                        free( tzdata );
                    }
                }
            }
            close( fd );
        }
    }
    return( rc );
}
