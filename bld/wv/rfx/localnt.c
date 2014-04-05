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
* Description:  Win32 local file access functions. Very incomplete!
*
****************************************************************************/


#include <stddef.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <direct.h>
#include <dos.h>
#include "wio.h"

#include "dbgdefn.h"
#include "dbgreg.h"
#include "dbgio.h"

#include "trprfx.h"

#include "local.h"

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

int LocalInteractive( sys_handle fh )
/***********************************/
{
    return( isatty( fh ) );
}

void LocalGetBuff( char *buff, unsigned size )
/********************************************/
{
    read( STDIN_FILENO, buff, size );
}

unsigned LocalRename( char *from, char *to )
/******************************************/
{
    return( StashErrCode( rename( from, to ), OP_LOCAL ) );
}

unsigned LocalMkDir( char *name )
/*******************************/
{
    return( StashErrCode( mkdir( name ), OP_LOCAL ) );
}

unsigned LocalRmDir( char *name )
/*******************************/
{
    return( StashErrCode( rmdir( name ), OP_LOCAL ) );
}

unsigned LocalSetDrv( int drv )
/*****************************/
{
    unsigned    total;

    _dos_setdrive( drv - 1, &total );
    return( 0 );
}

int LocalGetDrv( void )
/*********************/
{
    unsigned    drive;

    _dos_getdrive( &drive );
    return( drive - 1 );
}

unsigned LocalGetCwd( int drive, char *where )
/********************************************/
{
    return( StashErrCode( getcwd( where, 256 ) == NULL, OP_LOCAL ) );
}

unsigned LocalSetCWD( char *name )
/********************************/
{
    return( StashErrCode( chdir( name ), OP_LOCAL ) );
}

long LocalGetFileAttr( char *name )
/*********************************/
{
#if 0
    FILESTATUS3 fileinfo;

    if( DosQueryPathInfo( name, FIL_STANDARD, &fileinfo, sizeof( fileinfo ) ) ) {
        return( -1 );
    }
    return( fileinfo.attrFile );
#else
    return 0;
#endif
}

unsigned LocalSetFileAttr( char *name, long attr )
/************************************************/
{
#if 0
    FILESTATUS3 fileinfo;

    if ( DosQueryPathInfo( name, FIL_STANDARD, &fileinfo, sizeof( fileinfo ) ) )
        return -1;

    fileinfo.attrFile = attr;
    return( StashErrCode( DosSetPathInfo( name, FIL_STANDARD,
        &fileinfo, sizeof( fileinfo ) , 0), OP_LOCAL ) );
#else
    return 0;
#endif
}

long LocalGetFreeSpace( int drv )
/*******************************/
{
    struct diskfree_t   dfre;

    _dos_getdiskfree( drv, &dfre );
    return( dfre.avail_clusters * dfre.sectors_per_cluster * dfre.bytes_per_sector );
}

unsigned LocalDateTime( sys_handle fh, int *time, int *date, int set )
/********************************************************************/
{
#if 0
    struct _FILESTATUS fstatus;
    struct _FDATE *pdate;
    struct _FTIME *ptime;
    unsigned    rc;

    pdate = (struct _FDATE *)date;
    ptime = (struct _FTIME *)time;
    if( set ) {
        rc = DosQueryFileInfo( fh, FIL_STANDARD, (PBYTE)&fstatus, sizeof( fstatus ) );
        if( rc != 0 ) return( StashErrCode( rc, OP_LOCAL ) );
        fstatus.ftimeLastWrite = *ptime;
        fstatus.fdateLastWrite = *pdate;
        rc = DosSetFileInfo( fh, 1, (PBYTE)&fstatus, sizeof( fstatus ) );
        if( rc != 0 ) return( StashErrCode( rc, OP_LOCAL ) );
    } else {
        rc = DosQueryFileInfo( fh, FIL_STANDARD, (PBYTE)&fstatus, sizeof( fstatus ) );
        if( rc != 0 ) return( StashErrCode( rc, OP_LOCAL ) );
        *ptime = fstatus.ftimeLastWrite;
        *pdate = fstatus.fdateLastWrite;
    }
    return( 0 );
#else
    return 0;
#endif
}

static void makeDOSDTA( struct find_t *dos, trap_dta *trp )
{
    trp->dos.dir_entry_num = 0;
    trp->dos.cluster = 0;
    trp->attr = dos->attrib;
    trp->time = dos->wr_time;
    trp->date = dos->wr_date;
    trp->size = dos->size;
    strcpy( trp->name, dos->name );
}

unsigned LocalFindFirst( char *pattern, void *info, unsigned info_len, int attrib )
/*********************************************************************************/
{
    unsigned        rc;
    struct find_t   findbuf;

    rc = _dos_findfirst( pattern, attrib, &findbuf );
    if( rc )
        return( StashErrCode( rc , OP_LOCAL ) );
    makeDOSDTA( &findbuf, info );
    return( 0 );
#if 0
    FILEFINDBUF3  dta;
    HDIR   handle = 1;
    ULONG  count = 1;
    APIRET err;

    info_len = info_len;
    err = DosFindFirst( pattern, &handle, attrib, &dta, sizeof( dta ),
        &count, FIL_STANDARD );
    if( err != 0 ) return( StashErrCode( err, OP_LOCAL ) );
    makeDOSDTA( &dta, info );
    return( 0 );
#endif
}

unsigned LocalFindNext( void *info, unsigned info_len )
/*****************************************************/
{
    return( -1 );
#if 0
    unsigned    rc;
    find_t      findbuf;
    FILEFINDBUF3  dta;
    ULONG         count = 1;
    APIRET        rc;

    info_len = info_len;
    rc = DosFindNext( 1, &dta, sizeof( dta ), &count );
    if( rc != 0 ) return( StashErrCode( rc, OP_LOCAL ) );
    if( count == 0 ) {
        DosFindClose( 1 );
        return( -1 );
    }
    makeDOSDTA( &dta, info );
    return( 0 );
#endif
}

/*
  SIGNAL HANDLING
*/
static volatile int interruptOccurred;

#if 0
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
#endif

void InitInt( void )
/******************/
{
//    PFNSIGHANDLER handler;
//    USHORT action;

    interruptOccurred = 0;
//    DosSetSigHandler( doInterrupt, &handler, &action,SIGA_ACCEPT,SIG_CTRLC);
//    DosSetSigHandler( doInterrupt, &handler, &action,SIGA_ACCEPT,SIG_CTRLBREAK);
//    DosError( 0x0002 ); /* disable hard-error processing */
}

void FiniInt( void )
/******************/
{
}

int CtrlCHit( void )
/******************/
{
    int hit;

//    DosHoldSignal( HLDSIG_DISABLE );
    hit = interruptOccurred;
    interruptOccurred = 0;
//    DosHoldSignal( HLDSIG_ENABLE );

    return( hit );
}
