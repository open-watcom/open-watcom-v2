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
#include <time.h>
#include "dbgdefn.h"
#include "dbgio.h"
#include "trpcore.h"
#include "trprfx.h"
#include "remote.h"
#include "trapglbl.h"
#include "trpld.h"
#include "rfxacc.h"

extern char *TxtBuff;

trap_shandle    SuppRFXId;

#define SUPP_RFX_SERVICE( in, request )         \
        in.supp.core_req        = REQ_PERFORM_SUPPLEMENTARY_SERVICE;    \
        in.supp.id              = SuppRFXId;    \
        in.req                  = request;

bool InitRFXSupp( void )
{
    SuppRFXId = GetSuppId( RFX_SUPP_NAME );
    if( SuppRFXId == 0 ) return( false );
    return( true );
}


error_idx RemoteRename( const char * from, const char *to )
{
    in_mx_entry         in[3];
    mx_entry            out[1];
    rfx_rename_req      acc;
    rfx_rename_ret      ret;

    SUPP_RFX_SERVICE( acc, REQ_RFX_RENAME );
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = from;
    in[1].len = (trap_elen)( strlen( from ) + 1 );
    in[2].ptr = to;
    in[2].len = (trap_elen)( strlen( to ) + 1 );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    TrapAccess( 3, in, 1, out );
    return( StashErrCode( ret.err, OP_REMOTE ) );
}

error_idx RemoteMkDir( const char *name )
{
    in_mx_entry         in[2];
    mx_entry            out[1];
    rfx_mkdir_req       acc;
    rfx_mkdir_ret       ret;

    SUPP_RFX_SERVICE( acc, REQ_RFX_MKDIR );
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = name;
    in[1].len = (trap_elen)( strlen( name ) + 1 );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    TrapAccess( 2, in, 1, out );
    return( StashErrCode( ret.err, OP_REMOTE ) );
}

error_idx RemoteRmDir( const char *name )
{
    in_mx_entry         in[2];
    mx_entry            out[1];
    rfx_rmdir_req       acc;
    rfx_rmdir_ret       ret;

    SUPP_RFX_SERVICE( acc, REQ_RFX_RMDIR );
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = name;
    in[1].len = (trap_elen)( strlen( name ) + 1 );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    TrapAccess( 2, in, 1, out );
    return( StashErrCode( ret.err, OP_REMOTE ) );
}

error_idx RemoteSetDrv( int drv )
{
    rfx_setdrive_req    acc;
    rfx_setdrive_ret    ret;

    SUPP_RFX_SERVICE( acc, REQ_RFX_SETDRIVE );
    acc.drive = drv;
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    return( StashErrCode( ret.err, OP_REMOTE ) );
}

int RemoteGetDrv( void )
{
    rfx_getdrive_req    acc;
    rfx_getdrive_ret    ret;

    SUPP_RFX_SERVICE( acc, REQ_RFX_GETDRIVE );
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    return( ret.drive );
}

error_idx RemoteSetCWD( const char *name )
{
    in_mx_entry         in[2];
    mx_entry            out[1];
    rfx_setcwd_req      acc;
    rfx_setcwd_ret      ret;

    SUPP_RFX_SERVICE( acc, REQ_RFX_SETCWD );
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = name;
    in[1].len = (trap_elen)( strlen( name ) + 1 );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    TrapAccess( 2, in, 1, out );
    return( StashErrCode( ret.err, OP_REMOTE ) );
}

long RemoteGetFileAttr( const char * name )
{
    in_mx_entry         in[2];
    mx_entry            out[1];
    rfx_getfileattr_req acc;
    rfx_getfileattr_ret ret;

    SUPP_RFX_SERVICE( acc, REQ_RFX_GETFILEATTR );
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = name;
    in[1].len = (trap_elen)( strlen( name ) + 1 );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    TrapAccess( 2, in, 1, out );
    if( (ret.attribute & 0xffff0000) == 0xffff0000 ) {
        StashErrCode( ret.attribute, OP_REMOTE );
        return( -1L );
    }
    return( ret.attribute );
}

error_idx RemoteSetFileAttr( const char * name, long attrib )
{
    in_mx_entry         in[2];
    mx_entry            out[1];
    rfx_setfileattr_req acc;
    rfx_setfileattr_ret ret;

    SUPP_RFX_SERVICE( acc, REQ_RFX_SETFILEATTR );
    acc.attribute = attrib;
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = name;
    in[1].len = (trap_elen)( strlen( name ) + 1 );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    TrapAccess( 2, in, 1, out );
    return( StashErrCode( ret.err, OP_REMOTE ) );
}

long RemoteGetFreeSpace( int drv )
{
    rfx_getfreespace_req    acc;
    rfx_getfreespace_ret    ret;

    SUPP_RFX_SERVICE( acc, REQ_RFX_GETFREESPACE );
    acc.drive = drv;
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    if( (ret.size & 0xffff0000) == 0xffff0000 ) {
        StashErrCode( ret.size, OP_REMOTE );
        return( -1L );
    }
    return( ret.size );
}

static void mylocaltime( unsigned long date_time, int *time, int *date )
{
    unsigned      num_yr_since_1970;
    unsigned      num_leap_since_1970;
    unsigned      sec, min, hour, day, month, year;
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
    year = num_yr_since_1970 - 10;
    for( month=1;( day > day_since_jan[month] && month <= 12 ); month++ ) {}
    day -= day_since_jan[month - 1];
    date_time %= 86400;
    hour = date_time / 3600;
    date_time %= 3600;
    min = date_time / 60;
    sec = date_time % 60;

    *time = (hour << 11) | (min << 5) | (sec / 2);
    *date = (year << 9) | (month << 5) | day;
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

error_idx RemoteDateTime( sys_handle hdl, int *time, int *date, int set )
{
    if( set ) {
        rfx_setdatetime_req     acc;

        SUPP_RFX_SERVICE( acc, REQ_RFX_SETDATETIME );
        acc.handle = hdl;
        acc.time = mymktime( *time, *date );

        TrapSimpAccess( sizeof( acc ), &acc, 0, NULL );
    } else {
        rfx_getdatetime_req     acc;
        rfx_getdatetime_ret     ret;

        SUPP_RFX_SERVICE( acc, REQ_RFX_GETDATETIME );
        acc.handle = hdl;

        TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );

        mylocaltime( ret.time, time, date );
    }
    return( 0 );
}

//NYI: Assume max cwd lenght is 80
#define MAX_STRING_LEN  80

error_idx RemoteGetCwd( int drv, char *where )
{
    in_mx_entry         in[1];
    mx_entry            out[2];
    rfx_getcwd_req      acc;
    rfx_getcwd_ret      ret;

    SUPP_RFX_SERVICE( acc, REQ_RFX_GETCWD );
    acc.drive = drv;
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    out[1].ptr = where;
    out[1].len = MAX_STRING_LEN;
    TrapAccess( 1, in, 2, out );
    return( StashErrCode( ret.err, OP_REMOTE ) );
}

error_idx RemoteFindFirst( const char *pattern, void *info, trap_elen info_len, int attrib )
{
    in_mx_entry          in[2];
    mx_entry             out[2];
    rfx_findfirst_req   acc;
    rfx_findfirst_ret   ret;

    SUPP_RFX_SERVICE( acc, REQ_RFX_FINDFIRST );
    acc.attrib = attrib;
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = pattern;
    in[1].len = (trap_elen)( strlen( pattern ) + 1 );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    out[1].ptr = info;
    out[1].len = info_len;
    TrapAccess( 2, in, 2, out );
    return( StashErrCode( ret.err, OP_REMOTE ) );
}


int RemoteFindNext( void *info, trap_elen info_len )
{
    in_mx_entry          in[2];
    mx_entry             out[2];
    rfx_findnext_req    acc;
    rfx_findnext_ret    ret;

    SUPP_RFX_SERVICE( acc, REQ_RFX_FINDNEXT );
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = info;
    in[1].len = info_len;
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    out[1].ptr = info;
    out[1].len = info_len;
    TrapAccess( 2, in, 2, out );
    return( ret.err );
}

error_idx RemoteFindClose( void )
{
    rfx_findclose_req   acc;
    rfx_findclose_ret   ret;

    SUPP_RFX_SERVICE( acc, REQ_RFX_FINDCLOSE );
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    return( StashErrCode( ret.err, OP_REMOTE ) );
}

size_t RenameNameToCannonical( char *name, char *fullname, trap_elen fullname_len )
{
    in_mx_entry           in[2];
    mx_entry              out[2];
    rfx_nametocannonical_req    acc;
    rfx_nametocannonical_ret    ret;

    SUPP_RFX_SERVICE( acc, REQ_RFX_NAMETOCANNONICAL );
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = name;
    in[1].len = (trap_elen)( strlen( name ) + 1 );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    out[1].ptr = fullname;
    out[1].len = fullname_len;
    TrapAccess( 2, in, 2, out );
    if( ret.err != 0 ) {
        StashErrCode( ret.err, OP_REMOTE );
        *fullname = NULLCHAR;
    }
    return( strlen( fullname ) );
}
