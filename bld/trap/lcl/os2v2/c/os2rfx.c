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
* Description:  OS/2 32-bit local implementation of remote file access.
*
****************************************************************************/


#include <stddef.h>
#include <string.h>
#include <ctype.h>
#define INCL_BASE
#define INCL_DOSDEVICES
#define INCL_DOSMEMMGR
#define INCL_DOSSIGNALS
#include <os2.h>
#include <os2dbg.h>
#include "trpimp.h"

#if 0
typedef struct {
    struct {
        char                i_dunno[13];
        unsigned int        dir_entry_num;
        unsigned int        cluster;
        char                i_still_dunno[4];
    } dos;
    char                attr;
    unsigned int        time;
    unsigned int        date;
    long                size;
    char                name[14];
} dos_dta;
#endif

#define NIL_DOS_HANDLE  ((HFILE)0xFFFF)
#define BUFF_SIZE       256


trap_retval ReqRfx_rename( void )
{
    char                *old_name;
    char                *new_name;
    rfx_rename_ret      *ret;

    old_name = GetInPtr( sizeof( rfx_rename_req ) );
    new_name = GetInPtr( sizeof( rfx_rename_req ) + strlen( old_name ) + 1 );
    ret = GetOutPtr( 0 );
    ret->err = DosMove( old_name, new_name );
    return( sizeof( *ret ) );
}


trap_retval ReqRfx_mkdir( void )
{
    char                *name;
    rfx_mkdir_ret       *ret;

    name = GetInPtr( sizeof( rfx_mkdir_req ) );
    ret = GetOutPtr( 0 );
    ret->err = DosCreateDir( name, 0 );
    return( sizeof( *ret ) );
}


trap_retval ReqRfx_rmdir( void )
{
    char                *name;
    rfx_rmdir_ret       *ret;

    name = GetInPtr( sizeof( rfx_rmdir_req ) );
    ret = GetOutPtr( 0 );
    ret->err = DosDeleteDir( name );
    return( sizeof( *ret ) );
}


trap_retval ReqRfx_setdrive( void )
{
    rfx_setdrive_req    *acc;
    rfx_setdrive_ret    *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = DosSetDefaultDisk( acc->drive + 1 );
    return( sizeof( *ret ) );
}


trap_retval ReqRfx_getdrive( void )
{
    ULONG               drive;
    ULONG               map;
    rfx_getdrive_ret    *ret;

    ret = GetOutPtr( 0 );
    if( DosQueryCurrentDisk( &drive, &map ) == 0 ) {
        ret->drive = drive - 1;
    } else {
        ret->drive = 0;
    }
    return( sizeof( *ret ) );
}


trap_retval ReqRfx_setcwd( void )
{
    char                *name;
    rfx_setcwd_ret      *ret;

    name = GetInPtr( sizeof( rfx_setcwd_req ) );
    ret = GetOutPtr( 0 );
    ret->err = DosSetCurrentDir( name );
    return( sizeof( *ret ) );
}


trap_retval ReqRfx_getfileattr( void )
{
    FILESTATUS3         info;
    USHORT              ret_code;
    char                *name;
    rfx_getfileattr_ret *ret;

    name = GetInPtr( sizeof( rfx_getfileattr_req ) );
    ret = GetOutPtr( 0 );
    ret_code = DosQueryPathInfo( name, FIL_STANDARD, &info, sizeof( info ) );
    ret->attribute = (ret_code == 0) ? info.attrFile : (0xffff0000 | ret_code);
    return( sizeof( *ret ) );
}


trap_retval ReqRfx_setfileattr( void )
{
    FILESTATUS3         info;
    char                *name;
    rfx_setfileattr_req *acc;
    rfx_setfileattr_ret *ret;

    // Not tested, and not used right now
    acc = GetInPtr( 0 );
    name = GetInPtr( sizeof( *acc ) );
    ret = GetOutPtr( 0 );
    ret->err = DosQueryPathInfo( name, FIL_STANDARD, &info, sizeof( info ) );
    if (ret->err == 0) {
        info.attrFile = acc->attribute;
        ret->err = DosSetPathInfo( name, FIL_STANDARD, &info, sizeof( info ), 0 );
        }
    return( sizeof( *ret ) );
}

trap_retval ReqRfx_getfreespace( void )
{
    FSALLOCATE              info;
    rfx_getfreespace_req    *acc;
    rfx_getfreespace_ret    *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    DosQueryFSInfo( acc->drive, 1, (PBYTE)&info, sizeof( info ) );
    ret->size = (long)info.cbSector
                 * (long)info.cSectorUnit
                 * (long)info.cUnitAvail;
    return( sizeof( *ret ) );
}

static void mylocaltime( ULONG date_time, USHORT *time, USHORT *date )
{
    unsigned      num_yr_since_1970;
    unsigned      num_leap_since_1970;
    unsigned      sec, min, hour, day, month, year;
    unsigned      day_since_jan[] = { 0,31,59,90,120,151,181,212,243,273,304,334,365 };

    num_yr_since_1970 = date_time / 31622400UL;
    num_leap_since_1970 = (num_yr_since_1970 - 2) / 4;
    date_time -= ((num_leap_since_1970 * 366 +
                  (num_yr_since_1970 - num_leap_since_1970) * 365)
                   * 86400 );
    day = (date_time / 86400) + 1;   // Start from Jan 1, not Jan 0
    if( ((num_yr_since_1970 - 2) % 4) == 0 ) {
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
    if( (( num_yr_since_1970 - 2) % 4) == 0 ) {
        for( month = 2; month <= 12; ++day_since_jan[month], ++month )
            ;
    }
    year = num_yr_since_1970 - 10;
    for( month = 1; (day > day_since_jan[month] && month <= 12); month++ )
        ;
    day -= day_since_jan[month - 1];
    date_time %= 86400;
    hour = date_time / 3600;
    date_time %= 3600;
    min = date_time / 60;
    sec = date_time % 60;

    *time = (hour << 11) | (min << 5) | (sec / 2);
    *date = (year << 9) | (month << 5) | day;
}

trap_retval ReqRfx_setdatetime( void )
{
    FILESTATUS3         info;
    FTIME               time;
    FDATE               date;
    rfx_setdatetime_req *acc;

    acc = GetInPtr( 0 );
    mylocaltime( acc->time, (USHORT*)&time, (USHORT*)&date );
    info.fdateCreation = date;
    info.ftimeCreation = time;
    info.fdateLastAccess = date;
    info.ftimeLastAccess = time;
    info.fdateLastWrite = date;
    info.ftimeLastWrite = time;
    DosSetFileInfo( acc->handle, FIL_STANDARD, &info, sizeof( info ) );
    return( 0 );
}

#define NM_SEC_1970_1980 315532800UL

static unsigned long mymktime( unsigned time, unsigned date )
{
    unsigned      day_since_jan[] = { 0,31,59,90,120,151,181,212,243,273,304,334 };
    unsigned      num_yr_since_1980;
    unsigned      num_leap_since_1980;
    unsigned      sec, min, hour, day, month, year;

    sec   = (time & 0x1f) * 2;
    min   = (time >> 5) & 0x3f;
    hour  = (time >> 11) & 0x1f;
    day   = date & 0x1f;
    month = ((date >> 5) & 0xf);
    year  = ((date >> 9) & 0x7f);

    //note that year 2000 is a leap year and I don't think this prog. will still
    //be around in year 2099....
    num_yr_since_1980 = year;
    num_leap_since_1980 = (num_yr_since_1980 + 3) / 4;
    if( ( (num_yr_since_1980 % 4) == 0 ) && (month > 2) ) {  // is leap year
        day++;
    }
    day += (num_leap_since_1980 * 366
             + (num_yr_since_1980 - num_leap_since_1980) * 365
             + day_since_jan[month - 1] - 1);
    return( NM_SEC_1970_1980 + day*86400 + hour*3600 + min*60 + sec );
}

trap_retval ReqRfx_getdatetime( void )
{
    rfx_getdatetime_req *acc;
    rfx_getdatetime_ret *ret;
    FILESTATUS3         info;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    DosQueryFileInfo( acc->handle, FIL_STANDARD, (char *)&info, sizeof( info ) );
    ret->time = mymktime( *(USHORT *)&info.ftimeLastWrite,
                          *(USHORT *)&info.fdateLastWrite );
    return( sizeof( *ret ) );
}

trap_retval ReqRfx_getcwd( void )
{
    ULONG               len = BUFF_SIZE;
    rfx_getcwd_req      *acc;
    rfx_getcwd_ret      *ret;
    char                *buff;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    buff = GetOutPtr( sizeof( *ret ) );
    ret->err = DosQueryCurrentDir( acc->drive, (PBYTE)buff, &len );
    return( sizeof( *ret ) + len );
}

static void MoveDirInfo( FILEFINDBUF3 *os2, trap_dta *dos )
{
    dos->dos.dir_entry_num = *(USHORT *)&os2->fdateLastWrite;
    dos->dos.cluster = *(USHORT *)&os2->ftimeLastWrite;
    dos->attr = os2->attrFile;
    dos->time = *(USHORT *)&os2->ftimeLastWrite;
    dos->date = *(USHORT *)&os2->fdateLastWrite;
    dos->size = os2->cbFile;
    strcpy( dos->name, os2->achName );
}

trap_retval ReqRfx_findfirst( void )
{
    FILEFINDBUF3         info;
    APIRET               rc;
    HDIR                 hdl = 1;
    ULONG                count = 1;
    rfx_findfirst_req    *acc;
    rfx_findfirst_ret    *ret;
    char                 *filename;

    acc = GetInPtr( 0 );
    filename = GetInPtr( sizeof( *acc ) );
    ret = GetOutPtr( 0 );
    rc = DosFindFirst( filename, &hdl, acc->attrib, &info,
                      sizeof( info ), &count, FIL_STANDARD );
    if( rc == 0 ) {
        MoveDirInfo( &info, (trap_dta *)GetOutPtr( sizeof( *ret ) ) );
        ret->err = 0;
        return( sizeof( *ret ) + sizeof( trap_dta ) );
    } else {
        ret->err = rc;
        return( sizeof( *ret ) );
    }
}

trap_retval ReqRfx_findnext( void )
{
    FILEFINDBUF3        info;
    APIRET              rc;
    ULONG               count = 1;
    rfx_findnext_ret    *ret;

    ret = GetOutPtr( 0 );
    rc = DosFindNext( 1, &info, sizeof( info ), &count );
    if( rc == 0 ) {
        MoveDirInfo( &info, (trap_dta *)GetOutPtr( sizeof( *ret ) ) );
        ret->err = 0;
        return( sizeof( *ret ) + sizeof( trap_dta ) );
    } else {
        ret->err = rc;
        return( sizeof( *ret ) );
    }
}

trap_retval ReqRfx_findclose( void )
{
    return( 0 );
}

trap_retval ReqRfx_nametocannonical( void )
{
    rfx_nametocannonical_ret    *ret;
    char                        *name;
    char                        *fullname;
    char                        *p;
    int                         level = 0;
    ULONG                       drive;
    ULONG                       map;
    ULONG                       len = BUFF_SIZE;

    // Not tested, and not used right now
    name = GetInPtr( sizeof( rfx_nametocannonical_req ) );
    ret = GetOutPtr( 0 );
    fullname = GetOutPtr( sizeof( *ret ) );
    ret->err = 1;
    while( *name == ' ' ) {
        name++;
    }
    if( *(name + 1) == ':' ) {
        drive = toupper( *name ) - 'A';
        name += 2;
    } else {
        DosQueryCurrentDisk( &drive, &map );
    }
    if( *name != '\\' ) {
        *fullname++ = '\\';
        // DOS : TinyGetCWDir( fullname, TinyGetCurrDrive() + 1 );
        DosQueryCurrentDir( drive + 1, (PBYTE)fullname, &len );
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
            if( *p == '\0' )
                goto done;
            if( *p == '\\' )
                break;
            if( *p == '/' )
                break;
            ++p;
        }
        if( strcmp( p, "." ) == 0 ) {
        } else if( strcmp( p, ".." ) == 0 ) {
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
        } else {
            *fullname++ = '\\';
            level++;
            do {
                *fullname++ = *p++;
            } while( *p != '\0' )
                ;
            *fullname = '\0';
        }
    }
done:
    return( sizeof( *ret ) + strlen( GetOutPtr( sizeof( *ret ) ) ) + 1 );
}
