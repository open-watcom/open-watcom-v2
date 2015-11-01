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


#include <string.h>
#include <ctype.h>
#include "tinyio.h"
#include "dosver.h"
#include "trpimp.h"
#include "trpcomm.h"

trap_retval ReqRfx_rename( void )
{
    tiny_ret_t      rc;
    char            *old_name;
    char            *new_name;
    rfx_rename_ret  *ret;

    old_name = GetInPtr( sizeof( rfx_rename_req ) );
    new_name = GetInPtr( sizeof( rfx_rename_req ) + strlen( old_name ) + 1 );
    ret = GetOutPtr( 0 );
    rc = TinyRename( old_name, new_name );
    ret->err = TINY_ERROR( rc ) ? TINY_INFO( rc ) : 0;
    return( sizeof( *ret ) );
}

trap_retval ReqRfx_mkdir( void )
{
    tiny_ret_t      rc;
    rfx_mkdir_ret   *ret;

    ret = GetOutPtr( 0 );
    rc = TinyMakeDir( (char *)GetInPtr( sizeof( rfx_mkdir_req ) ) );
    ret->err = TINY_ERROR( rc ) ? TINY_INFO( rc ) : 0;
    return( sizeof( *ret ) );
}

trap_retval ReqRfx_rmdir( void )
{
    tiny_ret_t      rc;
    rfx_mkdir_ret   *ret;

    ret = GetOutPtr( 0 );
    rc = TinyRemoveDir( (char *)GetInPtr( sizeof( rfx_rmdir_req ) ) );
    ret->err = TINY_ERROR( rc ) ? TINY_INFO( rc ) : 0;
    return( sizeof( *ret ) );
}

trap_retval ReqRfx_setdrive( void )
{
    rfx_setdrive_req    *acc;
    rfx_setdrive_ret    *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    TinySetCurrDrive( acc->drive );
    ret->err = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqRfx_getdrive( void )
{
    rfx_getdrive_ret    *ret;

    ret = GetOutPtr( 0 );
    ret->drive = TinyGetCurrDrive();
    return( sizeof( *ret ) );
}

trap_retval ReqRfx_setcwd( void )
{
    tiny_ret_t          rc;
    rfx_setcwd_ret      *ret;

    ret = GetOutPtr( 0 );
    rc = TinyChangeDir( GetInPtr( sizeof( rfx_setcwd_req ) ) );
    ret->err = TINY_ERROR( rc ) ? TINY_INFO( rc ) : 0;
    return( sizeof( *ret ) );
}

trap_retval ReqRfx_getcwd( void )
{
    tiny_ret_t          rc;
    rfx_getcwd_req      *acc;
    rfx_getcwd_ret      *ret;
    char                *cwd;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    cwd = GetOutPtr( sizeof( *ret ) );
    rc = TinyGetCWDir( cwd, acc->drive );
    if( TINY_ERROR( rc ) ) {
        ret->err = TINY_INFO( rc );
        *cwd = '\0';
    }
    return( sizeof( *ret ) + 1 + strlen( cwd ) );
}

trap_retval ReqRfx_setfileattr( void )
{
    tiny_ret_t          rc;
    rfx_setfileattr_req *acc;
    rfx_setfileattr_ret *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    rc = TinySetFileAttr( GetInPtr( sizeof( *acc ) ), acc->attribute );
    ret->err = TINY_ERROR( rc ) ? TINY_INFO( rc ) : 0;
    return( sizeof( *ret ) );
}

trap_retval ReqRfx_getfileattr( void )
{
    tiny_ret_t          rc;
    rfx_getfileattr_ret *ret;

    ret = GetOutPtr( 0 );
    rc = TinyGetFileAttr( GetInPtr( sizeof( rfx_getfileattr_req ) ) );
    ret->attribute = rc;
    return( sizeof( *ret ) );
}

trap_retval ReqRfx_getfreespace( void )
{
    rfx_getfreespace_req    *acc;
    rfx_getfreespace_ret    *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->size = TinyFreeSpace( acc->drive );
    return( sizeof( *ret ) );
}

static void mylocaltime( unsigned long date_time, tiny_ftime_t *time, tiny_fdate_t *date )
{
    unsigned      num_yr_since_1970;
    unsigned      num_leap_since_1970;
    unsigned      day, month;
    unsigned      day_since_jan[] = { 0,31,59,90,120,151,181,212,243,273,304,334,365 };

    num_yr_since_1970 = date_time / 31622400UL;
    num_leap_since_1970 = ( num_yr_since_1970 - 2 ) / 4;
    date_time -= ( ( num_leap_since_1970 * 366 +
                   ( num_yr_since_1970 - num_leap_since_1970 ) * 365 )
                   * 86400 );
    day = ( date_time / 86400 ) + 1;   // Start from Jan 1, not Jan 0
    if( ( ( num_yr_since_1970 - 2 ) % 4 ) == 0 ) {
        //leap
        if( day >= 366 ) {
            day -= 366;
            num_yr_since_1970++;
        }
    } else {
        if( day >= 365 ) {
            day -= 365;
            num_yr_since_1970++;
        }
    }
    if( ( ( num_yr_since_1970 - 2 ) % 4 ) == 0 ) {
        for( month=2; month<=12; ++day_since_jan[month], ++month ) {}
    }
    date->year = num_yr_since_1970 - 10;
    for( month=1;( day > day_since_jan[month] && month <= 12 ); month++ ) {}
    date->month = month;
    date->day = day - day_since_jan[month - 1];
    date_time %= 86400;
    time->hours = date_time / 3600;
    date_time %= 3600;
    time->minutes = date_time / 60;
    time->twosecs = ( date_time % 60 ) / 2;
}

trap_retval ReqRfx_setdatetime( void )
{
    tiny_ftime_t        time;
    tiny_fdate_t        date;
    rfx_setdatetime_req *acc;

    acc = GetInPtr( 0 );
    mylocaltime( acc->time, &time, &date );
    TinySetFileStamp( acc->handle, time, date );
    return( 0 );
}

#define NM_SEC_1970_1980 315532800UL

static unsigned long mymktime( unsigned time, unsigned date )
{
    unsigned      day_since_jan[] = { 0,31,59,90,120,151,181,212,243,273,304,334 };
    unsigned      num_yr_since_1980;
    unsigned      num_leap_since_1980;
    unsigned      sec, min, hour, day, month, year;

    //NYI: Output in local time, but not in GMT time
    sec   = ( time & 0x1f ) * 2;
    min   = ( time >> 5 ) & 0x3f;
    hour  = ( time >> 11 ) & 0x1f;
    day   = date & 0x1f;
    month = ( ( date >> 5 ) & 0xf );
    year  = ( ( date >> 9 ) & 0x7f );

    //note that year 2000 is a leap year and I don't think this prog. will still
    //be around in year 2099....
    num_yr_since_1980 = year;
    num_leap_since_1980 = ( num_yr_since_1980 + 3 ) / 4;
    if( ( ( num_yr_since_1980 % 4 ) == 0 ) && ( month > 2 ) ) {  // is leap year
        day++;
    }
    day += ( num_leap_since_1980 * 366
             + ( num_yr_since_1980 - num_leap_since_1980 ) * 365
             + day_since_jan[month-1] - 1 );
    return( NM_SEC_1970_1980 + day*86400 + hour*3600 + min*60 + sec );
}

trap_retval ReqRfx_getdatetime( void )
{
    tiny_ret_t          rc;
    rfx_getdatetime_req *acc;
    rfx_getdatetime_ret *ret;
    unsigned            time;
    unsigned            date;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    rc = TinyGetFileStamp( acc->handle );
    time = rc & 0xffff;
    date = rc >> 16;
    ret->time = mymktime( time, date );
    return( sizeof( *ret ) );
}

trap_retval ReqRfx_findfirst( void )
{
    tiny_ret_t          rc;
    rfx_findfirst_req   *acc;
    rfx_findfirst_ret   *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    TinySetDTA( GetOutPtr( sizeof( *ret ) ) );
    rc = TinyFindFirst( (char *)GetInPtr( sizeof( *acc ) ), acc->attrib );
    ret->err = TINY_ERROR( rc ) ? TINY_INFO( rc ) : 0;
    return( sizeof( *ret ) + sizeof( tiny_find_t ) );
}

trap_retval ReqRfx_findnext( void )
{
    tiny_ret_t          rc;
    rfx_findnext_ret    *ret;
    void                *info;

    ret = GetOutPtr( 0 );
    info = GetInPtr( sizeof( rfx_findnext_req ) );
    TinyFarSetDTA( info );
    rc = TinyFindNext();
    if( TINY_ERROR( rc ) ) {
        ret->err = TINY_INFO( rc );
    } else {
        memcpy( GetOutPtr( sizeof( *ret ) ), info, sizeof( tiny_find_t ) );
        ret->err = 0;
    }
    return( sizeof( *ret ) + sizeof( tiny_find_t ) );
}

trap_retval ReqRfx_findclose( void )
{
    rfx_findclose_ret    *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqRfx_nametocannonical( void )
{
    rfx_nametocannonical_ret    *ret;
    char                        *name;
    char                        *fullname;
    char                        *p;
    int                         drive;
    int                         level = 0;
    char                        ch;

    name = GetInPtr( sizeof( rfx_nametocannonical_req ) );
    ret = GetOutPtr( 0 );
    fullname = GetOutPtr( sizeof( *ret ) );
    ret->err = 1;
    while( *name == ' ' ) {
        name++;
    }
    if( *( name + 1 ) == ':' ) {
        drive = toupper( *name ) - 'A';
        name += 2;
    } else {
        drive = TinyGetCurrDrive();
    }
    *fullname++ = 'A' + drive;
    *fullname++ = ':';
    if( *name != '\\' ) {
        *fullname++ = '\\';
        TinyGetCWDir( fullname, drive + 1 );
        if( *fullname != '\0' ) {
            level++;
            while( *fullname != '\0' ) {
                if( *fullname == '\\' ) {
                    level++;
                }
                fullname++;
            }
        }
    } else {
        name++;
        if( *name == '\0' ) {
            *fullname++ = '\\';
        }
        *fullname = '\0';
    }
    p = name;
    for( ;; ) {
        for( ;; ) {
            ch = *p++;
            if( ch == '\\' ) break;
            if( ch == '/' ) break;
            if( ch == '\0' ) {
                return( sizeof( *ret ) + strlen( GetOutPtr( 0 ) ) + 1 );
            }
        }
        if( p[0] == '.' ) {
            if( p[1] == '.' ) {
                if( level > 0 ) {
                    while( *fullname != '\\' ) {
                        fullname--;
                    }
                    level--;
                    *fullname = '\0';
                } else {
                    ret->err = 1;
                    break;
                }
            }
        } else {
            *fullname++ = '\\';
            level++;
            do {
                *fullname++ = *p++;
            } while( *p != '\0' );
            *fullname = '\0';
        }
    }
    return( sizeof( *ret ) );
}
