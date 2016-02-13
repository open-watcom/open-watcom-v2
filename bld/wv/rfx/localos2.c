/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
#ifdef _M_I86
#include "dta.h"
#else
#include "trprfx.h"
#endif
#include "local.h"
#include "rfx.h"


void LocalTime( int *hour, int *min, int *sec, int *hundredths )
{
    struct _DATETIME datetime;

    DosGetDateTime( &datetime );
    *hour = datetime.hours;
    *min = datetime.minutes;
    *sec = datetime.seconds;
    *hundredths = datetime.hundredths;
}

void LocalDate( int *year, int *month, int *day, int *weekday )
{
    struct _DATETIME datetime;

    DosGetDateTime( &datetime );
    *year = datetime.year;
    *month = datetime.month;
    *day = datetime.day;
    *weekday = datetime.weekday;
}

int LocalInteractive( sys_handle fh )
/*******************************/
{
    APIRET type;
    APIRET flags;

    //NYI: really should convert fh to sys_handle, but I know that it's
    // a one-to-one mapping
#ifdef _M_I86
    if( DosQHandType( fh, &type, &flags ) ) {
#else
    if( DosQueryHType( fh, &type, &flags ) ) {
#endif
        return( 0 );
    }
    if( type == 1 ) {   /* device type */
        return( 1 );
    }
    return( 0 );
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
        buff[1] = '\0';
        return;
    }
    buff[ length.cchIn ] = '\0';
}

error_handle LocalRename( const char *from, const char *to )
/**********************************************************/
{
#ifdef _M_I86
    return( StashErrCode( DosMove( from, to, 0 ), OP_LOCAL ) );
#else
    return( StashErrCode( DosMove( from, to ), OP_LOCAL ) );
#endif
}

error_handle LocalMkDir( const char *name )
/*****************************************/
{
#ifdef _M_I86
    return( StashErrCode( DosMkDir( name, 0 ), OP_LOCAL ) );
#else
    return( StashErrCode( DosCreateDir( name, NULL ), OP_LOCAL ) );
#endif
}

error_handle LocalRmDir( const char *name )
/*****************************************/
{
#ifdef _M_I86
    return( StashErrCode( DosRmDir( name, 0 ), OP_LOCAL ) );
#else
    return( StashErrCode( DosDeleteDir( name ), OP_LOCAL ) );
#endif
}

error_handle LocalSetDrv( int drv )
/*********************************/
{
#ifdef _M_I86
    return( StashErrCode( DosSelectDisk( drv + 1 ), OP_LOCAL ) );
#else
    return( StashErrCode( DosSetDefaultDisk( drv + 1 ), OP_LOCAL ) );
#endif
}

int LocalGetDrv( void )
/*********************/
{
    APIRET    drive;
    ULONG     map;

#ifdef _M_I86
    if( DosQCurDisk( &drive, &map ) ) {
#else
    if( DosQueryCurrentDisk( &drive, &map ) ) {
#endif
        return( -1 );
    }
    return( drive - 1 );
}

error_handle LocalSetCWD( const char *name )
/******************************************/
{
#ifdef _M_I86
    return( StashErrCode( DosChDir( name, 0 ), OP_LOCAL ) );
#else
    return( StashErrCode( DosSetCurrentDir( name ), OP_LOCAL ) );
#endif
}

long LocalGetFileAttr( const char *name )
/***************************************/
{
#ifdef _M_I86
    USHORT attr;

    if( DosQFileMode( name, &attr, 0 ) ) {
        return( -1L );
    }
    return( attr );
#else
    FILESTATUS3 fileinfo;

    if( DosQueryPathInfo( name, FIL_STANDARD, &fileinfo, sizeof( fileinfo ) ) ) {
        return( -1L );
    }
    return( fileinfo.attrFile );
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

error_handle LocalDateTime( sys_handle fh, int *time, int *date, int set )
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
        rc = DosQFileInfo( fh, 1, (PBYTE)&fstatus, sizeof( fstatus ) );
#else
        rc = DosQueryFileInfo( fh, FIL_STANDARD, (PBYTE)&fstatus, sizeof( fstatus ) );
#endif
        if( rc != 0 )
            return( StashErrCode( rc, OP_LOCAL ) );
        fstatus.ftimeLastWrite = *ptime;
        fstatus.fdateLastWrite = *pdate;
        rc = DosSetFileInfo( fh, 1, (PBYTE)&fstatus, sizeof( fstatus ) );
        if( rc != 0 ) {
            return( StashErrCode( rc, OP_LOCAL ) );
        }
    } else {
#ifdef _M_I86
        rc = DosQFileInfo( fh, 1, (PBYTE)&fstatus, sizeof( fstatus ) );
#else
        rc = DosQueryFileInfo( fh, FIL_STANDARD, (PBYTE)&fstatus, sizeof( fstatus ) );
#endif
        if( rc != 0 )
            return( StashErrCode( rc, OP_LOCAL ) );
        *ptime = fstatus.ftimeLastWrite;
        *pdate = fstatus.fdateLastWrite;
    }
    return( 0 );
}

error_handle LocalGetCwd( int drive, char *where )
/************************************************/
{
    APIRET len;

    len = 256;
#ifdef _M_I86
    return( StashErrCode( DosQCurDir( drive, (PBYTE)where, &len ), OP_LOCAL ) );
#else
    return( StashErrCode( DosQueryCurrentDir( drive, (PBYTE)where, &len ), OP_LOCAL ) );
#endif
}

#ifdef _M_I86
static void makeDOSDTA( struct _FILEFINDBUF *os2, dta *dos )
#else
static void makeDOSDTA( struct _FILEFINDBUF3 *os2, trap_dta *dos )
#endif
{
    dos->dos.dir_entry_num = *(USHORT *)&os2->fdateLastWrite;
    dos->dos.cluster = *(USHORT *)&os2->ftimeLastWrite;
    dos->attr = os2->attrFile;
    dos->time = *(USHORT *)&os2->ftimeLastWrite;
    dos->date = *(USHORT *)&os2->fdateLastWrite;
    dos->size = os2->cbFile;
    strcpy( dos->name, os2->achName );
}

error_handle LocalFindFirst( const char *pattern, void *info, unsigned info_len, int attrib )
/*******************************************************************************************/
{
#ifdef _M_I86
    FILEFINDBUF dta;
#else
    FILEFINDBUF3  dta;
#endif
    HDIR handle = 1;
    APIRET count = 1;
    APIRET err;

    info_len = info_len;
#ifdef _M_I86
    err = DosFindFirst( pattern, &handle, attrib, &dta, sizeof( dta ), &count,0);
#else
    err = DosFindFirst( pattern, &handle, attrib, &dta, sizeof( dta ), &count, FIL_STANDARD );
#endif
    if( err != 0 )
        return( StashErrCode( err, OP_LOCAL ) );
    makeDOSDTA( &dta, info );
    return( 0 );
}

int LocalFindNext( void *info, unsigned info_len )
/************************************************/
{
#ifdef _M_I86
    FILEFINDBUF   dta;
#else
    FILEFINDBUF3  dta;
#endif
    APIRET        count = 1;
    APIRET        rc;

    info_len = info_len;
    rc = DosFindNext( 1, &dta, sizeof( dta ), &count );
    if( rc != 0 )
        return( -1 );
    if( count == 0 ) {
        DosFindClose( 1 );
        return( -1 );
    }
    makeDOSDTA( &dta, info );
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
{
#ifdef _M_I86
    PFNSIGHANDLER handler;
    USHORT action;
#endif

    interruptOccurred = false;
#ifdef _M_I86
    DosSetSigHandler( doInterrupt, &handler, &action,SIGA_ACCEPT,SIG_CTRLC);
    DosSetSigHandler( doInterrupt, &handler, &action,SIGA_ACCEPT,SIG_CTRLBREAK);
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


error_handle LocalSetFileAttr( const char *name, long attr )
/**********************************************************/
{
#ifdef _M_I86
    return( StashErrCode( DosSetFileMode( name, attr, 0 ), OP_LOCAL ) );
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
