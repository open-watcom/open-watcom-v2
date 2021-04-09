/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include <string.h>
#include <ctype.h>
#define INCL_BASE
#define INCL_DOSDEVICES
#define INCL_DOSMEMMGR
#define INCL_DOSSIGNALS
#include <os2.h>
#include <os2dbg.h>
#include "trpimp.h"
#include "trpcomm.h"


#define NIL_DOS_HANDLE  ((HFILE)0xFFFF)

#define TRPH2LH(th)     (HFILE)((th)->handle.u._32[0])
#define LH2TRPH(th,lh)  (th)->handle.u._32[0]=(unsigned_32)lh;(th)->handle.u._32[1]=0

#define IsDot(p)        ((p)[0] == '.' && (p)[1] == '\0')
#define IsDotDot(p)     ((p)[0] == '.' && (p)[1] == '.' && (p)[2] == '\0')
#define IsPathSep(p)    ((p)[0] == '\\' || (p)[0] == '/')

trap_retval TRAP_RFX( rename )( void )
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

trap_retval TRAP_RFX( mkdir )( void )
{
    char                *name;
    rfx_mkdir_ret       *ret;

    name = GetInPtr( sizeof( rfx_mkdir_req ) );
    ret = GetOutPtr( 0 );
    ret->err = DosCreateDir( name, 0 );
    return( sizeof( *ret ) );
}


trap_retval TRAP_RFX( rmdir )( void )
{
    char                *name;
    rfx_rmdir_ret       *ret;

    name = GetInPtr( sizeof( rfx_rmdir_req ) );
    ret = GetOutPtr( 0 );
    ret->err = DosDeleteDir( name );
    return( sizeof( *ret ) );
}


trap_retval TRAP_RFX( setdrive )( void )
{
    rfx_setdrive_req    *acc;
    rfx_setdrive_ret    *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = DosSetDefaultDisk( acc->drive + 1 );
    return( sizeof( *ret ) );
}


trap_retval TRAP_RFX( getdrive )( void )
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


trap_retval TRAP_RFX( setcwd )( void )
{
    char                *name;
    rfx_setcwd_ret      *ret;

    name = GetInPtr( sizeof( rfx_setcwd_req ) );
    ret = GetOutPtr( 0 );
    ret->err = DosSetCurrentDir( name );
    return( sizeof( *ret ) );
}


trap_retval TRAP_RFX( getfileattr )( void )
{
    FILESTATUS3         info;
    USHORT              rc;
    char                *name;
    rfx_getfileattr_ret *ret;

    name = GetInPtr( sizeof( rfx_getfileattr_req ) );
    ret = GetOutPtr( 0 );
    rc = DosQueryPathInfo( name, FIL_STANDARD, &info, sizeof( info ) );
    ret->attribute = (rc == 0) ? info.attrFile : (0xffff0000 | rc);
    return( sizeof( *ret ) );
}


trap_retval TRAP_RFX( setfileattr )( void )
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
    if( ret->err == 0 ) {
        info.attrFile = acc->attribute;
        ret->err = DosSetPathInfo( name, FIL_STANDARD, &info, sizeof( info ), 0 );
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_RFX( getfreespace )( void )
{
    FSALLOCATE              info;
    rfx_getfreespace_req    *acc;
    rfx_getfreespace_ret    *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    DosQueryFSInfo( acc->drive, 1, (PBYTE)&info, sizeof( info ) );
    ret->size = (long)info.cbSector * (long)info.cSectorUnit * (long)info.cUnitAvail;
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
                  (num_yr_since_1970 - num_leap_since_1970) * 365) * 86400 );
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
        for( month = 2; month <= 12; ++day_since_jan[month], ++month ) {
            ;
        }
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

trap_retval TRAP_RFX( setdatetime )( void )
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
    DosSetFileInfo( TRPH2LH( acc ), FIL_STANDARD, &info, sizeof( info ) );
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
    return( NM_SEC_1970_1980 + day * 86400 + hour * 3600 + min * 60 + sec );
}

trap_retval TRAP_RFX( getdatetime )( void )
{
    rfx_getdatetime_req *acc;
    rfx_getdatetime_ret *ret;
    FILESTATUS3         info;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    DosQueryFileInfo( TRPH2LH( acc ), FIL_STANDARD, (char *)&info, sizeof( info ) );
    ret->time = mymktime( *(USHORT *)&info.ftimeLastWrite, *(USHORT *)&info.fdateLastWrite );
    return( sizeof( *ret ) );
}

trap_retval TRAP_RFX( getcwd )( void )
{
    ULONG               len;
    rfx_getcwd_req      *acc;
    rfx_getcwd_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    len = GetTotalSizeOut() - sizeof( *ret );
    ret->err = DosQueryCurrentDir( acc->drive, (PBYTE)GetOutPtr( sizeof( *ret ) ), &len );
    return( sizeof( *ret ) + len );
}

static void makeDTARFX( rfx_find *info, FILEFINDBUF3 *findbuf, HDIR h )
{
    DTARFX_HANDLE_OF( info ) = h;
    info->time = DTARFX_TIME_OF( info ) = *(USHORT *)&findbuf->ftimeLastWrite;
    info->date = DTARFX_DATE_OF( info ) = *(USHORT *)&findbuf->fdateLastWrite;
    info->attr = findbuf->attrFile;
    info->size = findbuf->cbFile;
#if RFX_NAME_MAX < CCHMAXPATHCOMP
    strncpy( info->name, findbuf->achName, RFX_NAME_MAX );
    info->name[RFX_NAME_MAX] = '\0';
#else
    strncpy( info->name, findbuf->achName, CCHMAXPATHCOMP );
    info->name[CCHMAXPATHCOMP] = '\0';
#endif
}

trap_retval TRAP_RFX( findfirst )( void )
{
    FILEFINDBUF3        findbuf;
    APIRET              rc;
    HDIR                h;
    ULONG               count = 1;
    rfx_findfirst_req   *acc;
    rfx_findfirst_ret   *ret;
    char                *filename;
    rfx_find            *info;

    acc = GetInPtr( 0 );
    filename = GetInPtr( sizeof( *acc ) );
    h = HDIR_CREATE;
    ret = GetOutPtr( 0 );
    ret->err = rc = DosFindFirst( filename, &h, acc->attrib, &findbuf, sizeof( findbuf ), &count, FIL_STANDARD );
    info = GetOutPtr( sizeof( *ret ) );
    if( rc ) {
        DTARFX_HANDLE_OF( info ) = DTARFX_INVALID_HANDLE;
        return( sizeof( *ret ) + offsetof( rfx_find, name ) );
    }
    makeDTARFX( info, &findbuf, h );
    return( sizeof( *ret ) + offsetof( rfx_find, name ) + strlen( info->name ) + 1 );
}

trap_retval TRAP_RFX( findnext )( void )
{
    FILEFINDBUF3        findbuf;
    APIRET              rc;
    ULONG               count = 1;
    rfx_findnext_ret    *ret;
    HDIR                h;
    rfx_find            *info;

    info = GetInPtr( sizeof( rfx_findnext_req ) );
    ret = GetOutPtr( 0 );
    if( DTARFX_HANDLE_OF( info ) == DTARFX_INVALID_HANDLE ) {
        ret->err = -1;
        return( sizeof( *ret ) );
    }
    h = DTARFX_HANDLE_OF( info );
    ret->err = rc = DosFindNext( h, &findbuf, sizeof( findbuf ), &count );
    info = GetOutPtr( sizeof( *ret ) );
    if( rc ) {
        DosFindClose( h );
        DTARFX_HANDLE_OF( info ) = DTARFX_INVALID_HANDLE;
        return( sizeof( *ret ) + offsetof( rfx_find, name ) );
    }
    makeDTARFX( info, &findbuf, h );
    return( sizeof( *ret ) + offsetof( rfx_find, name ) + strlen( info->name ) + 1 );
}

trap_retval TRAP_RFX( findclose )( void )
{
    rfx_findclose_ret   *ret;
    HDIR                h;
    rfx_find            *info;

    info = GetInPtr( sizeof( rfx_findclose_req ) );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    if( DTARFX_HANDLE_OF( info ) != DTARFX_INVALID_HANDLE ) {
        h = DTARFX_HANDLE_OF( info );
        DosFindClose( h );
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_RFX( nametocanonical )( void )
{
    rfx_nametocanonical_ret     *ret;
    char                        *name;
    char                        *fullname;
    char                        *p;
    int                         level = 0;
    ULONG                       drive;
    ULONG                       map;
    ULONG                       len;

    // Not tested, and not used right now
    name = GetInPtr( sizeof( rfx_nametocanonical_req ) );
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
    len = RFX_NAME_MAX + 1;
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
            if( IsPathSep( p ) )
                break;
            ++p;
        }
        if( IsDot( p ) ) {
        } else if( IsDotDot( p ) ) {
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
