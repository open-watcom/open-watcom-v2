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
* Description:  OS/2 local file access functions.
*
****************************************************************************/


#include <stddef.h>
#include <limits.h>
#define INCL_DOS
#define INCL_SUB
#include "wos2.h"
#include "dbgdefn.h"
#include "rfxdata.h"
#include "dbgio.h"
#include "trprfx.h"
#include "local.h"
#include "rfx.h"


#ifdef _M_I86
#define FINDBUF     FILEFINDBUF
#else
#define FINDBUF     FILEFINDBUF3
#endif

#define SYSH2LH(sh)     (HFILE)((sh).u._32[0])
#define LH2SYSH(sh,lh)  (sh).u._32[0]=lh;(sh).u._32[1]=0

void LocalTime( int *hour, int *min, int *sec, int *hundredths )
/**************************************************************/
{
    struct _DATETIME datetime;

    DosGetDateTime( &datetime );
    *hour = datetime.hours;
    *min = datetime.minutes;
    *sec = datetime.seconds;
    *hundredths = datetime.hundredths;
}

void LocalDate( int *year, int *month, int *day, int *weekday )
/*************************************************************/
{
    struct _DATETIME datetime;

    DosGetDateTime( &datetime );
    *year = datetime.year;
    *month = datetime.month;
    *day = datetime.day;
    *weekday = datetime.weekday;
}

bool LocalInteractive( sys_handle sh )
/************************************/
{
    APIRET type;
    APIRET flags;

    //NYI: really should convert fh to sys_handle, but I know that it's
    // a one-to-one mapping
#ifdef _M_I86
    if( DosQHandType( SYSH2LH( sh ), &type, &flags ) ) {
#else
    if( DosQueryHType( SYSH2LH( sh ), &type, &flags ) ) {
#endif
        return( false );
    }
    if( type == 1 ) {   /* device type */
        return( true );
    }
    return( false );
}

void LocalGetBuff( char *buff, unsigned size )
/********************************************/
{
    struct _STRINGINBUF length;

    if( size > UCHAR_MAX ) {
        size = UCHAR_MAX;
    }
    length.cb = size;
    length.cchIn = 0;
    if( KbdStringIn( buff, &length, 0, 0 ) ) {
        buff[0] = '\r';
        buff[1] = NULLCHAR;
        return;
    }
    buff[length.cchIn] = NULLCHAR;
}

error_handle LocalRename( const char *from, const char *to )
/**********************************************************/
{
#ifdef _M_I86
    return( StashErrCode( DosMove( (char *)from, (char *)to, 0 ), OP_LOCAL ) );
#else
    return( StashErrCode( DosMove( from, to ), OP_LOCAL ) );
#endif
}

error_handle LocalMkDir( const char *name )
/*****************************************/
{
#ifdef _M_I86
    return( StashErrCode( DosMkDir( (char *)name, 0 ), OP_LOCAL ) );
#else
    return( StashErrCode( DosCreateDir( name, NULL ), OP_LOCAL ) );
#endif
}

error_handle LocalRmDir( const char *name )
/*****************************************/
{
#ifdef _M_I86
    return( StashErrCode( DosRmDir( (char *)name, 0 ), OP_LOCAL ) );
#else
    return( StashErrCode( DosDeleteDir( name ), OP_LOCAL ) );
#endif
}

error_handle LocalSetDrv( int drv )
/*********************************/
{
    return( StashErrCode( DosSelectDisk( drv + 1 ), OP_LOCAL ) );
}

int LocalGetDrv( void )
/*********************/
{
    APIRET    drive;
    ULONG     map;

    if( DosQCurDisk( &drive, &map ) ) {
        return( -1 );
    }
    return( drive - 1 );
}

error_handle LocalSetCWD( const char *name )
/******************************************/
{
#ifdef _M_I86
    return( StashErrCode( DosChDir( (char *)name, 0 ), OP_LOCAL ) );
#else
    return( StashErrCode( DosSetCurrentDir( name ), OP_LOCAL ) );
#endif
}

long LocalGetFileAttr( const char *name )
/***************************************/
{
#ifdef _M_I86
    USHORT attr;

    if( DosQFileMode( (char *)name, &attr, 0 ) ) {
        return( RFX_INVALID_FILE_ATTRIBUTES );
    }
    return( attr );
#else
    FILESTATUS3 fileinfo;

    if( DosQueryPathInfo( name, FIL_STANDARD, &fileinfo, sizeof( fileinfo ) ) ) {
        return( RFX_INVALID_FILE_ATTRIBUTES );
    }
    return( fileinfo.attrFile );
#endif
}

error_handle LocalSetFileAttr( const char *name, long attr )
/**********************************************************/
{
#ifdef _M_I86
    return( StashErrCode( DosSetFileMode( (char *)name, attr, 0 ), OP_LOCAL ) );
#else
    FILESTATUS3 fileinfo;
    APIRET      rc;

    rc = DosQueryPathInfo( name, FIL_STANDARD, &fileinfo, sizeof( fileinfo ) );
    if( rc == 0 ) {
        fileinfo.attrFile = attr;
        rc = DosSetPathInfo( name, FIL_STANDARD, &fileinfo, sizeof( fileinfo ), 0 );
    }
    return( StashErrCode( rc, OP_LOCAL ) );
#endif
}

long LocalGetFreeSpace( int drv )
/*******************************/
{
    struct _FSALLOCATE usage;

#ifdef _M_I86
    if( DosQFSInfo( drv, 1, (PBYTE)&usage, sizeof( usage ) ) ) {
#else
    if( DosQueryFSInfo( drv, 1, (PBYTE)&usage, sizeof( usage ) ) ) {
#endif
        return( -1L );
    }
    return( usage.cbSector * usage.cSectorUnit * usage.cUnitAvail );
}

error_handle LocalDateTime( sys_handle sh, int *time, int *date, int set )
/************************************************************************/
{
    struct _FILESTATUS fstatus;
    struct _FDATE *pdate;
    struct _FTIME *ptime;
    unsigned    rc;

    pdate = (struct _FDATE *)date;
    ptime = (struct _FTIME *)time;
    if( set ) {
#ifdef _M_I86
        rc = DosQFileInfo( SYSH2LH( sh ), 1, (PBYTE)&fstatus, sizeof( fstatus ) );
#else
        rc = DosQueryFileInfo( SYSH2LH( sh ), FIL_STANDARD, (PBYTE)&fstatus, sizeof( fstatus ) );
#endif
        if( rc != 0 )
            return( StashErrCode( rc, OP_LOCAL ) );
        fstatus.ftimeLastWrite = *ptime;
        fstatus.fdateLastWrite = *pdate;
        rc = DosSetFileInfo( SYSH2LH( sh ), 1, (PBYTE)&fstatus, sizeof( fstatus ) );
        if( rc != 0 ) {
            return( StashErrCode( rc, OP_LOCAL ) );
        }
    } else {
#ifdef _M_I86
        rc = DosQFileInfo( SYSH2LH( sh ), 1, (PBYTE)&fstatus, sizeof( fstatus ) );
#else
        rc = DosQueryFileInfo( SYSH2LH( sh ), FIL_STANDARD, (PBYTE)&fstatus, sizeof( fstatus ) );
#endif
        if( rc != 0 )
            return( StashErrCode( rc, OP_LOCAL ) );
        *ptime = fstatus.ftimeLastWrite;
        *pdate = fstatus.fdateLastWrite;
    }
    return( 0 );
}

error_handle LocalGetCwd( int drive, char *where, unsigned len )
/**************************************************************/
{
    APIRET xlen;

    xlen = len;
#ifdef _M_I86
    return( StashErrCode( DosQCurDir( drive, (PBYTE)where, &xlen ), OP_LOCAL ) );
#else
    return( StashErrCode( DosQueryCurrentDir( drive, (PBYTE)where, &xlen ), OP_LOCAL ) );
#endif
}

static void makeDTARFX( rfx_find *info, FINDBUF *findbuf )
/********************************************************/
{
    info->time = DTARFX_TIME_OF( info ) = *(USHORT FAR *)&findbuf->ftimeLastWrite;
    info->date = DTARFX_DATE_OF( info ) = *(USHORT FAR *)&findbuf->fdateLastWrite;
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

error_handle LocalFindFirst( const char *pattern, rfx_find *info, unsigned info_len, int attrib )
/***********************************************************************************************/
{
    FINDBUF     findbuf;
    HDIR        h;
    APIRET      count = 1;
    APIRET      err;

    /* unused parameters */ (void)info_len;

    h = HDIR_CREATE;
#ifdef _M_I86
    err = DosFindFirst( (char *)pattern, &h, attrib, &findbuf, sizeof( findbuf ), &count, 0 );
#else
    err = DosFindFirst( pattern, &h, attrib, &findbuf, sizeof( findbuf ), &count, FIL_STANDARD );
#endif
    if( err ) {
        DTARFX_HANDLE_OF( info ) = DTARFX_INVALID_HANDLE;
        return( StashErrCode( err, OP_LOCAL ) );
    }
    DTARFX_HANDLE_OF( info ) = h;
    makeDTARFX( info, &findbuf );
    return( 0 );
}

int LocalFindNext( rfx_find *info, unsigned info_len )
/****************************************************/
{
    FINDBUF     findbuf;
    APIRET      count = 1;
    APIRET      rc;
    HDIR        h;

    /* unused parameters */ (void)info_len;

    if( DTARFX_HANDLE_OF( info ) == DTARFX_INVALID_HANDLE ) {
        return( -1 );
    }
    h = DTARFX_HANDLE_OF( info );
    rc = DosFindNext( h, &findbuf, sizeof( findbuf ), &count );
    if( rc || count == 0 ) {
        DosFindClose( h );
        DTARFX_HANDLE_OF( info ) = DTARFX_INVALID_HANDLE;
        return( -1 );
    }
    makeDTARFX( info, &findbuf );
    return( 0 );
}

error_handle LocalFindClose( rfx_find *info, unsigned info_len )
/**************************************************************/
{
    /* unused parameters */ (void)info_len;

    if( DTARFX_HANDLE_OF( info ) != DTARFX_INVALID_HANDLE ) {
        DosFindClose( DTARFX_HANDLE_OF( info ) );
        DTARFX_HANDLE_OF( info ) = DTARFX_INVALID_HANDLE;
    }
    return( 0 );
}

/*
  SIGNAL HANDLING
*/
static volatile bool    interruptOccurred;

#ifdef _M_I86
static void __pascal __far doInterrupt( USHORT signal_argument, USHORT signal_num )
{
    PFNSIGHANDLER handler;
    USHORT action;

    signal_argument = signal_argument;
    interruptOccurred = true;
    switch( signal_num ) {
    case SIG_CTRLBREAK:
        DosSetSigHandler( doInterrupt, &handler, &action, SIGA_ACKNOWLEDGE, SIG_CTRLBREAK );
        break;
    case SIG_CTRLC:
        DosSetSigHandler( doInterrupt, &handler, &action, SIGA_ACKNOWLEDGE, SIG_CTRLC );
        break;
    }
}
#endif

void InitInt( void )
{
#ifdef _M_I86
    PFNSIGHANDLER handler;
    USHORT action;
#endif

    interruptOccurred = false;
#ifdef _M_I86
    DosSetSigHandler( doInterrupt, &handler, &action, SIGA_ACCEPT, SIG_CTRLC );
    DosSetSigHandler( doInterrupt, &handler, &action, SIGA_ACCEPT, SIG_CTRLBREAK );
#endif
    DosError( 0x0002 ); /* disable hard-error processing */
}

void FiniInt( void )
{
}

bool CtrlCHit( void )
{
    bool    hit;

#ifdef _M_I86
    DosHoldSignal( HLDSIG_DISABLE );
#endif
    hit = interruptOccurred;
    interruptOccurred = false;
#ifdef _M_I86
    DosHoldSignal( HLDSIG_ENABLE );
#endif

    return( hit );
}
