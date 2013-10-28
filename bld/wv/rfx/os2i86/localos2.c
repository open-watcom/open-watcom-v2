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


#include <stddef.h>
#include <limits.h>
#include <string.h>

#include "dbgdefn.h"
#include "dbgreg.h"
#include "dbgio.h"

#include "dta.h"

#define INCL_DOS
#define INCL_SUB
#include "os2.h"

#include "local.h"

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
    USHORT type;
    USHORT flags;

    //NYI: really should convert fh to sys_handle, but I know that it's
    // a one-to-one mapping
    if( DosQHandType( fh, &type, &flags ) ) {
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

unsigned LocalRename( char *from, char *to )
/**************************************/
{
    return( StashErrCode( DosMove( from, to, 0 ), OP_LOCAL ) );
}

unsigned LocalMkDir( char *name )
/***************************/
{
    return( StashErrCode( DosMkDir( name, 0 ), OP_LOCAL ) );
}

unsigned LocalRmDir( char *name )
/***************************/
{
    return( StashErrCode( DosRmDir( name, 0 ), OP_LOCAL ) );
}

unsigned LocalSetDrv( int drv )
/*************************/
{
    return( StashErrCode( DosSelectDisk( drv + 1 ), OP_LOCAL ) );
}

int LocalGetDrv( void )
/*********************/
{
    USHORT drive;
    ULONG map;

    if( DosQCurDisk( &drive, &map ) ) {
        return( -1 );
    }
    return( drive - 1 );
}

unsigned LocalSetCWD( char *name )
/****************************/
{
    return( StashErrCode( DosChDir( name, 0 ), OP_LOCAL ) );
}

long LocalGetFileAttr( char *name )
/*********************************/
{
    USHORT attr;

    if( DosQFileMode( name, &attr, 0 ) ) {
        return( -1 );
    }
    return( attr );
}

long LocalGetFreeSpace( int drv )
/*******************************/
{
    struct _FSALLOCATE usage;

    if( DosQFSInfo( drv, 1, (PBYTE)&usage, sizeof( usage ) ) ) {
        return( -1 );
    }
    return( usage.cbSector * usage.cSectorUnit * usage.cUnitAvail );
}

unsigned LocalDateTime( sys_handle fh, int *time, int *date, int set )
/**************************************************************/
{
    struct _FILESTATUS fstatus;
    struct _FDATE *pdate;
    struct _FTIME *ptime;
    unsigned    rc;

    pdate = (struct _FDATE *)date;
    ptime = (struct _FTIME *)time;
    if( set ) {
        rc = DosQFileInfo( fh, 1, (PBYTE)&fstatus, sizeof( fstatus ) );
        if( rc != 0 ) return( StashErrCode( rc, OP_LOCAL ) );
        fstatus.ftimeLastWrite = *ptime;
        fstatus.fdateLastWrite = *pdate;
        rc = DosSetFileInfo( fh, 1, (PBYTE)&fstatus, sizeof( fstatus ) );
        if( rc != 0 ) return( StashErrCode( rc, OP_LOCAL ) );
    } else {
        rc = DosQFileInfo( fh, 1, (PBYTE)&fstatus, sizeof( fstatus ) );
        if( rc != 0 ) return( StashErrCode( rc, OP_LOCAL ) );
        *ptime = fstatus.ftimeLastWrite;
        *pdate = fstatus.fdateLastWrite;
    }
    return( 0 );
}

unsigned LocalGetCwd( int drive, char *where )
/****************************************/
{
    USHORT len;

    len = 256;
    return( StashErrCode( DosQCurDir( drive, where, &len ), OP_LOCAL ) );
}

static void makeDOSDTA( struct _FILEFINDBUF *os2, dta *dos )
{
    dos->dos.dir_entry_num = *(USHORT *)&os2->fdateLastWrite;
    dos->dos.cluster = *(USHORT *)&os2->ftimeLastWrite;
    dos->attr = os2->attrFile;
    dos->time = *(USHORT *)&os2->ftimeLastWrite;
    dos->date = *(USHORT *)&os2->fdateLastWrite;
    dos->size = os2->cbFile;
    strcpy( dos->name, os2->achName );
}

unsigned LocalFindFirst( char *pattern, void *info, unsigned info_len, int attrib )
/*****************************************************************************/
{
    FILEFINDBUF dta;
    HDIR handle = 1;
    USHORT count = 1;
    unsigned    err;

    info_len = info_len;
    err = DosFindFirst( pattern, &handle, attrib, &dta, sizeof( dta ), &count,0);
    if( err != 0 ) return( StashErrCode( err, OP_LOCAL ) );
    makeDOSDTA( &dta, info );
    return( 0 );
}

unsigned LocalFindNext( void *info, unsigned info_len )
/*************************************************/
{
    FILEFINDBUF dta;
    USHORT count = 1;
    unsigned    rc;

    info_len = info_len;
    rc = DosFindNext( 1, &dta, sizeof( dta ), &count );
    if( rc != 0 ) return( StashErrCode( rc, OP_LOCAL ) );
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
static volatile int interruptOccurred;

static void __pascal __far doInterrupt( USHORT signal_argument, USHORT signal_num )
{
    PFNSIGHANDLER handler;
    USHORT action;

    signal_argument = signal_argument;
    interruptOccurred = 1;
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

void InitInt( void )
{
    PFNSIGHANDLER handler;
    USHORT action;

    interruptOccurred = 0;
    DosSetSigHandler( doInterrupt, &handler, &action,SIGA_ACCEPT,SIG_CTRLC);
    DosSetSigHandler( doInterrupt, &handler, &action,SIGA_ACCEPT,SIG_CTRLBREAK);
    DosError( 0x0002 ); /* disable hard-error processing */
}

void FiniInt( void )
{
}

int CtrlCHit( void )
{
    int hit;

    DosHoldSignal( HLDSIG_DISABLE );
    hit = interruptOccurred;
    interruptOccurred = 0;
    DosHoldSignal( HLDSIG_ENABLE );

    return( hit );
}


unsigned LocalSetFileAttr( char *name, long attr )
/********************************************/
{

    return( StashErrCode( DosSetFileMode( name, attr, 0 ), OP_LOCAL ) );
}
