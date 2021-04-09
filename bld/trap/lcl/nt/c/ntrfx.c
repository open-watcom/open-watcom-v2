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
* Description:  NT local implementation of remote file access.
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#include <dos.h>
#include <time.h>
#include <windows.h>
#include "trpimp.h"
#include "trpcomm.h"
#include "libwin32.h"
#include "ntext.h"


#define TRPH2LH(th)     (HANDLE)((th)->handle.u._32[0])
#define LH2TRPH(th,lh)  (th)->handle.u._32[0]=(unsigned_32)lh;(th)->handle.u._32[1]=0

#define GetError(x)     ((x) ? 0 : GetLastError())

#define CHARLOW(c)      ((c) | 0x20)
#define CHARUP(c)       ((c) & ~0x20)

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
    ret->err = GetError( MoveFile( old_name, new_name ) );
    return( sizeof( *ret ) );
}

trap_retval TRAP_RFX( mkdir )( void )
{
    char                *name;
    rfx_mkdir_ret       *ret;

    name = GetInPtr( sizeof( rfx_mkdir_req ) );
    ret = GetOutPtr( 0 );
    ret->err = GetError( CreateDirectory( name, NULL ) );
    return( sizeof( *ret ) );
}


trap_retval TRAP_RFX( rmdir )( void )
{
    char                *name;
    rfx_rmdir_ret       *ret;

    name = GetInPtr( sizeof( rfx_rmdir_req ) );
    ret = GetOutPtr( 0 );
    ret->err = GetError( RemoveDirectory( name ) );
    return( sizeof( *ret ) );
}

static int nt_set_drive( int drive )
{
    char        path[4];

    path[0] = 'A' + drive;
    path[1] = ':';
    path[2] = '.';
    path[3] = '\0';
    return( GetError( SetCurrentDirectory( path ) ) );
}

static int nt_get_drive( void )
{
    char        dir[MAX_PATH];

    if( GetCurrentDirectory( sizeof( dir ), dir ) ) {
        if( dir[0] != '\0' && dir[1] == ':' ) {
            return( CHARLOW( dir[0] ) - 'a' );
        }
    }
    return( -1 );
}

trap_retval TRAP_RFX( setdrive )( void )
/* entry 0=A,1=B,... */
{
    rfx_setdrive_req    *acc;
    rfx_setdrive_ret    *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = nt_set_drive( acc->drive );
    return( sizeof( *ret ) );
}


trap_retval TRAP_RFX( getdrive )( void )
/* return 0=A,1=B,... */
{
    rfx_getdrive_ret    *ret;

    ret = GetOutPtr( 0 );
    ret->drive = nt_get_drive();
    return( sizeof( *ret ) );
}


trap_retval TRAP_RFX( setcwd )( void )
{
    char                *name;
    rfx_setcwd_ret      *ret;

    name = GetInPtr( sizeof( rfx_setcwd_req ) );
    ret = GetOutPtr( 0 );
    ret->err = GetError( SetCurrentDirectory( name ) );
    return( sizeof( *ret ) );
}


trap_retval TRAP_RFX( getfileattr )( void )
{
    HANDLE              h;
    WIN32_FIND_DATA     ffd;
    rfx_getfileattr_ret *ret;

    ret = GetOutPtr( 0 );
    h = __fixed_FindFirstFile( GetInPtr( sizeof( rfx_getfileattr_req ) ), &ffd );
    if( h == INVALID_HANDLE_VALUE ) {
        ret->attribute = (0xffff0000 | GetLastError());
    } else {
        ret->attribute = NT2DOSATTR( ffd.dwFileAttributes );
        FindClose( h );
    }
    return( sizeof( *ret ) );
}


trap_retval TRAP_RFX( setfileattr )( void )
{
    LPTSTR              name;
    rfx_setfileattr_req *acc;
    rfx_setfileattr_ret *ret;

    // Not tested, and not used right now
    acc = GetInPtr( 0 );
    name = GetInPtr( sizeof( *acc ) );
    ret = GetOutPtr( 0 );
    ret->err = GetError( SetFileAttributes( name, DOS2NTATTR( acc->attribute ) ) );
    return( sizeof( *ret ) );
}

trap_retval TRAP_RFX( getfreespace )( void )
{
    rfx_getfreespace_req    *acc;
    rfx_getfreespace_ret    *ret;
    char                    path[4];
    char                    *pname;
    DWORD                   sectors_per_cluster;
    DWORD                   bytes_per_sector;
    DWORD                   avail_clusters;
    DWORD                   total_clusters;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    pname = NULL;
    if( acc->drive != 0 ) {
        path[0] = acc->drive - 1 + 'A';
        path[1] = ':';
        path[2] = '\\';
        path[3] = 0;
        pname = path;
    }
    ret->size = 0;
    if( GetDiskFreeSpace( pname, &sectors_per_cluster, &bytes_per_sector, &avail_clusters, &total_clusters ) )
        ret->size = avail_clusters * sectors_per_cluster * bytes_per_sector;
    return( sizeof( *ret ) );
}

trap_retval TRAP_RFX( setdatetime )( void )
{
    rfx_setdatetime_req *acc;
    FILETIME            fctime;
    FILETIME            fatime;
    FILETIME            fwtime;
    HANDLE              h;

    acc = GetInPtr( 0 );
    h = TRPH2LH( acc );
    __NT_timet_to_filetime( acc->time, &fwtime );
    fatime = fctime = fwtime;
    SetFileTime( h, &fctime, &fatime, &fwtime );
    return( 0 );
}

trap_retval TRAP_RFX( getdatetime )( void )
{
    rfx_getdatetime_req *acc;
    rfx_getdatetime_ret *ret;
    FILETIME            fctime;
    FILETIME            fatime;
    FILETIME            fwtime;
    HANDLE              h;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    h = TRPH2LH( acc );
    GetFileTime( h, &fctime, &fatime, &fwtime );
    ret->time = __NT_filetime_to_timet( &fwtime );
    return( sizeof( *ret ) );
}

static void nt_getdcwd( int drive, char *buff, size_t max_len )
/* entry 0=current drive,1=A,2=B,... */
{
    int                 old_drive;
    char                tmp[MAX_PATH];

    *buff = '\0';
    if( GetError( GetCurrentDirectory( sizeof( tmp ), tmp ) ) == 0 ) {
        if( drive == 0 ) {
            strncpy( buff, tmp, max_len );
            buff[max_len] = '\0';
        } else {
            old_drive = CHARLOW( tmp[0] ) - 'a';
            if( nt_set_drive( drive ) == 0 ) {
                if( GetError( GetCurrentDirectory( sizeof( tmp ), tmp ) ) == 0 ) {
                    strncpy( buff, tmp, max_len );
                    buff[max_len] = '\0';
                }
                nt_set_drive( old_drive );
            }
        }
    }
}

trap_retval TRAP_RFX( getcwd )( void )
/* entry 0=current drive,1=A,2=B,... */
{
    rfx_getcwd_req      *acc;
    rfx_getcwd_ret      *ret;
    char                *buff;
    size_t              max_len;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    buff = GetOutPtr( sizeof( *ret ) );
    max_len = GetTotalSizeOut() - 1 - sizeof( *ret );
    nt_getdcwd( acc->drive, buff, max_len );
    return( sizeof( *ret ) + strlen( buff ) + 1 );
}

static void makeDTARFX( LPWIN32_FIND_DATA ffd, rfx_find *info, HANDLE h, unsigned nt_attribs )
/********************************************************************************************/
{
    DTARFX_HANDLE_OF( info ) = (pointer_uint)h;
    DTARFX_ATTRIB_OF( info ) = nt_attribs;
    info->attr = NT2DOSATTR( ffd->dwFileAttributes );
    __MakeDOSDT( &ffd->ftLastWriteTime, &info->date, &info->time );
    DTARFX_TIME_OF( info ) = info->time;
    DTARFX_DATE_OF( info ) = info->date;
    info->size = ffd->nFileSizeLow;
#if RFX_NAME_MAX < MAX_PATH
    strncpy( info->name, ffd->cFileName, RFX_NAME_MAX );
    info->name[RFX_NAME_MAX] = '\0';
#else
    strncpy( info->name, ffd->cFileName, MAX_PATH - 1 );
    info->name[MAX_PATH - 1] = '\0';
#endif
}

trap_retval TRAP_RFX( findfirst )( void )
{
    rfx_findfirst_req   *acc;
    rfx_findfirst_ret   *ret;
    HANDLE              h;
    WIN32_FIND_DATA     ffd;
    rfx_find            *info;
    unsigned            nt_attribs;

    acc = GetInPtr( 0 );
    nt_attribs = DOS2NTATTR( acc->attrib );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    info = GetOutPtr( sizeof( *ret ) );
    h = __fixed_FindFirstFile( GetInPtr( sizeof( *acc ) ), &ffd );
    if( h == INVALID_HANDLE_VALUE || !__NTFindNextFileWithAttr( h, nt_attribs, &ffd ) ) {
        ret->err = GetLastError();
        if( h != INVALID_HANDLE_VALUE ) {
            FindClose( h );
        }
        DTARFX_HANDLE_OF( info ) = DTARFX_INVALID_HANDLE;
        return( sizeof( *ret ) );
    }
    makeDTARFX( &ffd, info, h, nt_attribs );
    return( sizeof( *ret ) + offsetof( rfx_find, name ) + strlen( info->name ) + 1 );
}

trap_retval TRAP_RFX( findnext )( void )
{
    WIN32_FIND_DATA     ffd;
    rfx_findnext_ret    *ret;
    rfx_find            *info;
    HANDLE              h;
    unsigned            nt_attribs;

    info = GetInPtr( sizeof( rfx_findnext_req ) );
    ret = GetOutPtr( 0 );
    if( DTARFX_HANDLE_OF( info ) == DTARFX_INVALID_HANDLE ) {
        ret->err = -1;
        return( sizeof( *ret ) );
    }
    h = (HANDLE)DTARFX_HANDLE_OF( info );
    nt_attribs = DTARFX_ATTRIB_OF( info );
    info = GetOutPtr( sizeof( *ret ) );
    if( !__fixed_FindNextFile( h, &ffd ) || !__NTFindNextFileWithAttr( h, nt_attribs, &ffd ) ) {
        ret->err = GetLastError();
        FindClose( h );
        DTARFX_HANDLE_OF( info ) = DTARFX_INVALID_HANDLE;
        return( sizeof( *ret ) );
    }
    ret->err = 0;
    makeDTARFX( &ffd, info, h, nt_attribs );
    return( sizeof( *ret ) + offsetof( rfx_find, name ) + strlen( info->name ) + 1 );
}

trap_retval TRAP_RFX( findclose )( void )
{
    rfx_findclose_ret   *ret;
    rfx_find            *info;

    info = GetInPtr( sizeof( rfx_findclose_req ) );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    if( DTARFX_HANDLE_OF( info ) != DTARFX_INVALID_HANDLE ) {
        FindClose( (HANDLE)DTARFX_HANDLE_OF( info ) );
        DTARFX_HANDLE_OF( info ) = DTARFX_INVALID_HANDLE;
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_RFX( nametocanonical )( void )
{
    rfx_nametocanonical_ret     *ret;
    char                        *name;
    char                        *fullname;
    char                        *p;
    char                        tmp[MAX_PATH];
    int                         level = 0;
    int                         drive;
    size_t                      max_len;

    // Not tested, and not used right now
    name = GetInPtr( sizeof( rfx_nametocanonical_req ) );
    ret = GetOutPtr( 0 );
    fullname = GetOutPtr( sizeof( *ret ) );
    max_len = GetTotalSizeOut() - 1 -  sizeof( *ret );
    ret->err = 1;
    while( *name == ' ' ) {
        name++;
    }
    if( *(name + 1) == ':' ) {
        drive = CHARLOW( *name ) - 'a' + 1;
        name += 2;
    } else {
        drive = 0;
    }
    if( *name != '\\' ) {
        nt_getdcwd( drive, tmp, sizeof( tmp ) );
        p = tmp;
        if( p[0] != '\0' && p[1] == ':' )
            p += 2;
        strncpy( fullname, p, max_len );
        fullname[max_len] = '\0';
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
                break;
            if( IsPathSep( p ) )
                break;
            ++p;
        }
        if( *p == '\0' ) {
            break;
        } else if( IsDot( p ) ) {
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
            } while( *p != '\0' );
            *fullname = '\0';
        }
    }
    return( sizeof( *ret ) + strlen( GetOutPtr( sizeof( *ret ) ) ) + 1 );
}
