/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Win32 local file access functions. Very incomplete!
*
****************************************************************************/


#include <stddef.h>
#include <limits.h>
#include <time.h>
#include <direct.h>
#include <windows.h>
#include "wio.h"
#include "dbgdefn.h"
#include "rfxdata.h"
#include "dbgio.h"
#include "trprfx.h"
#include "local.h"
#include "rfx.h"

#include "clibext.h"


#define SYSH2LH(sh)     (int)((sh).u._32[0])
#define LH2SYSH(sh,lh)  (sh).u._32[0]=lh;(sh).u._32[1]=0

void LocalTime( int *hour, int *min, int *sec, int *hundredths )
/**************************************************************/
{
    time_t          cur_time;
    struct tm       *lctime;

    cur_time = time( NULL );
    lctime   = localtime( &cur_time );
    *hour       = lctime->tm_hour;
    *min        = lctime->tm_min;
    *sec        = lctime->tm_sec;
    *hundredths = 0;
}

void LocalDate( int *year, int *month, int *day, int *weekday )
/*************************************************************/
{
    time_t          cur_time;
    struct tm       *lctime;

    cur_time = time( NULL );
    lctime   = localtime( &cur_time );
    *year    = lctime->tm_year;
    *month   = lctime->tm_mon;
    *day     = lctime->tm_mday;
    *weekday = lctime->tm_wday;
}

static unsigned __DOS2NTAtrr( char dos_attribs )
{
    unsigned    nt_attribs;

    nt_attribs = FILE_ATTRIBUTE_NORMAL;
    if( dos_attribs & _A_SYSTEM ) {
        nt_attribs |= FILE_ATTRIBUTE_SYSTEM;
    }
    if( dos_attribs & _A_HIDDEN ) {
        nt_attribs |= FILE_ATTRIBUTE_HIDDEN;
    }
    if( dos_attribs & _A_ARCH ) {
        nt_attribs |= FILE_ATTRIBUTE_ARCHIVE;
    }
    if( dos_attribs & _A_RDONLY ) {
        nt_attribs |= FILE_ATTRIBUTE_READONLY;
    }
    if( dos_attribs & _A_SUBDIR ) {
        nt_attribs |= FILE_ATTRIBUTE_DIRECTORY;
    }
    return( nt_attribs );
}

static unsigned __NT2DOSAttr( unsigned nt_attribs )
{
    unsigned    dos_attribs;

    dos_attribs = 0;
    if( nt_attribs & FILE_ATTRIBUTE_SYSTEM ) {
        dos_attribs |= _A_SYSTEM;
    }
    if( nt_attribs & FILE_ATTRIBUTE_HIDDEN ) {
        dos_attribs |= _A_HIDDEN;
    }
    if( nt_attribs & FILE_ATTRIBUTE_ARCHIVE ) {
        dos_attribs |= _A_ARCH;
    }
    if( nt_attribs & FILE_ATTRIBUTE_READONLY ) {
        dos_attribs |= _A_RDONLY;
    }
    if( nt_attribs & FILE_ATTRIBUTE_DIRECTORY ) {
        dos_attribs |= _A_SUBDIR;
    }
    return( dos_attribs );
}

bool LocalInteractive( sys_handle sh )
/************************************/
{
    return( isatty( SYSH2LH( sh ) ) != 0 );
}

void LocalGetBuff( char *buff, unsigned size )
/********************************************/
{
    DWORD   cRead;
    HANDLE  hStdin;

    hStdin = GetStdHandle( STD_INPUT_HANDLE );
    if( !ReadFile( hStdin, buff, size, &cRead, NULL ) ) {
        buff[0] = '\r';
        buff[1] = NULLCHAR;
        return;
    }
    /* Kill the trailing \r\n. */
    if( cRead > 2 ) {
        buff[cRead - 2] = NULLCHAR;
    } else {
        buff[0] = '\r';
        buff[1] = NULLCHAR;
    }
}

error_handle LocalRename( const char *from, const char *to )
/**********************************************************/
{
    return( StashErrCode( rename( from, to ), OP_LOCAL ) );
}

error_handle LocalMkDir( const char *name )
/*****************************************/
{
    return( StashErrCode( mkdir( name ), OP_LOCAL ) );
}

error_handle LocalRmDir( const char *name )
/*****************************************/
{
    return( StashErrCode( rmdir( name ), OP_LOCAL ) );
}

error_handle LocalSetDrv( int drv )
/*********************************/
{
    char        dir[MAX_PATH];
    DWORD       rc;

    dir[0] = drv + 'A';
    dir[1] = '\\';
    dir[2] = '.';
    dir[3] = '\0';

    if( SetCurrentDirectory( dir ) ) {
        rc = GetCurrentDirectory( sizeof( dir ), dir );
        if( rc && rc < sizeof( dir ) ) {
            if( DRIVECHAR( (unsigned char)dir[0] ) - 'A' == drv ) {
                return( 0 );
            }
        }
    }
    return( StashErrCode( -1, OP_LOCAL ) );
}

int LocalGetDrv( void )
/*********************/
{
    char        dir[MAX_PATH];
    DWORD       rc;

    rc = GetCurrentDirectory( sizeof( dir ), dir );
    if( rc && rc < sizeof( dir ) )
        return( DRIVECHAR( (unsigned char)dir[0] ) - 'A' );
    return( -1 );
}

error_handle LocalGetCwd( int drive, char *where, unsigned len )
/**************************************************************/
{
    return( StashErrCode( _getdcwd( drive, where, len ) == NULL, OP_LOCAL ) );
}

error_handle LocalSetCWD( const char *name )
/******************************************/
{
    return( StashErrCode( chdir( name ), OP_LOCAL ) );
}

long LocalGetFileAttr( const char *name )
/***************************************/
{
    HANDLE              h;
    WIN32_FIND_DATA     ffb;

    h = FindFirstFile( (LPTSTR)name, &ffb );
    if( h == INVALID_HANDLE_VALUE ) {
        return( RFX_INVALID_FILE_ATTRIBUTES );
    }
    FindClose( h );
    return( ffb.dwFileAttributes );
}

error_handle LocalSetFileAttr( const char *name, long attr )
/**********************************************************/
{
    if( attr == 0 )
        attr = FILE_ATTRIBUTE_NORMAL;

    if( !SetFileAttributes( (LPTSTR)name, attr ) ) {
        return( StashErrCode( -1, OP_LOCAL ) );
    }
    return( 0 );
}

long LocalGetFreeSpace( int drv )
/*******************************/
{
    struct diskfree_t   dfre;

    _getdiskfree( drv, &dfre );
    return( dfre.avail_clusters * dfre.sectors_per_cluster * dfre.bytes_per_sector );
}

error_handle LocalDateTime( sys_handle sh, int *time, int *date, int set )
/************************************************************************/
{
#if 0
    struct _FILESTATUS fstatus;
    struct _FDATE *pdate;
    struct _FTIME *ptime;
    unsigned    rc;

    pdate = (struct _FDATE *)date;
    ptime = (struct _FTIME *)time;
    if( set ) {
        rc = DosQueryFileInfo( SYSH2LH( sh ), FIL_STANDARD, (PBYTE)&fstatus, sizeof( fstatus ) );
        if( rc != 0 )
            return( StashErrCode( rc, OP_LOCAL ) );
        fstatus.ftimeLastWrite = *ptime;
        fstatus.fdateLastWrite = *pdate;
        rc = DosSetFileInfo( SYSH2LH( sh ), 1, (PBYTE)&fstatus, sizeof( fstatus ) );
        if( rc != 0 ) {
            return( StashErrCode( rc, OP_LOCAL ) );
        }
    } else {
        rc = DosQueryFileInfo( SYSH2LH( sh ), FIL_STANDARD, (PBYTE)&fstatus, sizeof( fstatus ) );
        if( rc != 0 )
            return( StashErrCode( rc, OP_LOCAL ) );
        *ptime = fstatus.ftimeLastWrite;
        *pdate = fstatus.fdateLastWrite;
    }
    return( 0 );
#else
    (void)sh; (void)time; (void)date; (void)set;
    return 0;
#endif
}

static void __MakeDOSDT( FILETIME *NT_stamp, unsigned short *d, unsigned short *t )
/*********************************************************************************/
{
    FILETIME local_ft;

    FileTimeToLocalFileTime( NT_stamp, &local_ft );
    FileTimeToDosDateTime( &local_ft, d, t );
}

#define NT_ATTRIBUTES_MASK (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DIRECTORY)

static bool __NTFindNextFileWithAttr( HANDLE h, unsigned nt_attribs, LPWIN32_FIND_DATA ffb )
/******************************************************************************************/
{
    for( ;; ) {
        if( ffb->dwFileAttributes == 0 ) {
            // Win95 seems to return 0 for the attributes sometimes?
            // In that case, treat as a normal file
            ffb->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
        }
        if( (nt_attribs | !ffb->dwFileAttributes) & NT_ATTRIBUTES_MASK ) {
            return( true );
        }
        if( !FindNextFile( h, ffb ) ) {
            return( false );
        }
    }
}

static void makeDTARFX( rfx_find *info, LPWIN32_FIND_DATA ffb, HANDLE h, unsigned nt_attribs )
/********************************************************************************************/
{
    DTARFX_HANDLE_OF( info ) = (pointer_int)h;
    DTARFX_ATTRIB_OF( info ) = nt_attribs;
    info->attr = __NT2DOSAttr( ffb->dwFileAttributes );
    __MakeDOSDT( &ffb->ftLastWriteTime, &info->date, &info->time );
    DTARFX_TIME_OF( info ) = info->time;
    DTARFX_DATE_OF( info ) = info->date;
    info->size = ffb->nFileSizeLow;
#if RFX_NAME_MAX < MAX_PATH
    strncpy( info->name, ffb->cFileName, RFX_NAME_MAX );
    info->name[RFX_NAME_MAX] = '\0';
#else
    strncpy( info->name, ffb->cFileName, MAX_PATH - 1 );
    info->name[MAX_PATH - 1] = '\0';
#endif
}

error_handle LocalFindFirst( const char *pattern, rfx_find *info, unsigned info_len, int dos_attribs )
/****************************************************************************************************/
{
    HANDLE              h;
    int                 error;
    WIN32_FIND_DATA     ffb;
    unsigned            nt_attribs;

    /* unused parameters */ (void)info_len;

    nt_attribs = __DOS2NTAtrr( dos_attribs );
    h = FindFirstFile( (LPTSTR)pattern, &ffb );
    if( h == INVALID_HANDLE_VALUE || !__NTFindNextFileWithAttr( h, nt_attribs, &ffb ) ) {
        error = GetLastError();
        if( h != INVALID_HANDLE_VALUE ) {
            FindClose( h );
        }
        DTARFX_HANDLE_OF( info ) = DTARFX_INVALID_HANDLE;
        return( StashErrCode( -1, OP_LOCAL ) );
    }
    makeDTARFX( info, &ffb, h, nt_attribs );
    return( 0 );
}

int LocalFindNext( rfx_find *info, unsigned info_len )
/****************************************************/
{
    WIN32_FIND_DATA     ffb;
    HANDLE              h;
    unsigned            nt_attribs;

    /* unused parameters */ (void)info_len;

    if( DTARFX_HANDLE_OF( info ) == DTARFX_INVALID_HANDLE ) {
        return( -1 );
    }
    h = (HANDLE)DTARFX_HANDLE_OF( info );
    nt_attribs = DTARFX_ATTRIB_OF( info );
    if( !FindNextFile( h, &ffb ) || !__NTFindNextFileWithAttr( h, nt_attribs, &ffb ) ) {
        FindClose( h );
        DTARFX_HANDLE_OF( info ) = DTARFX_INVALID_HANDLE;
        return( -1 );
    }
    makeDTARFX( info, &ffb, h, nt_attribs );
    return( 0 );
}

error_handle LocalFindClose( rfx_find *info, unsigned info_len )
/**************************************************************/
{
    /* unused parameters */ (void)info_len;

    if( DTARFX_HANDLE_OF( info ) != DTARFX_INVALID_HANDLE ) {
        FindClose( (HANDLE)DTARFX_HANDLE_OF( info ) );
        DTARFX_HANDLE_OF( info ) = DTARFX_INVALID_HANDLE;
    }
    return( 0 );
}

/*
  SIGNAL HANDLING
*/
static volatile bool    interruptOccurred;

#if 0
static void __pascal __far doInterrupt( USHORT signal_argument, USHORT signal_num )
{
    PFNSIGHANDLER handler;
    USHORT action;

    signal_argument = signal_argument;
    interruptOccurred = true;
    switch( signal_num ) {
    case SIG_CTRLBREAK:
        DosSetSigHandler( doInterrupt, &handler, &action,
                          SIGA_ACKNOWLEDGE, SIG_CTRLBREAK );
        break;
    case SIG_CTRLC:
        DosSetSigHandler( doInterrupt, &handler, &action,
                          SIGA_ACKNOWLEDGE, SIG_CTRLC );
        break;
    }
}
#endif

void InitInt( void )
/******************/
{
//    PFNSIGHANDLER handler;
//    USHORT action;

    interruptOccurred = false;
//    DosSetSigHandler( doInterrupt, &handler, &action,SIGA_ACCEPT,SIG_CTRLC);
//    DosSetSigHandler( doInterrupt, &handler, &action,SIGA_ACCEPT,SIG_CTRLBREAK);
//    DosError( 0x0002 ); /* disable hard-error processing */
}

void FiniInt( void )
/******************/
{
}

bool CtrlCHit( void )
/******************/
{
    bool    hit;

//    DosHoldSignal( HLDSIG_DISABLE );
    hit = interruptOccurred;
    interruptOccurred = false;
//    DosHoldSignal( HLDSIG_ENABLE );

    return( hit );
}
