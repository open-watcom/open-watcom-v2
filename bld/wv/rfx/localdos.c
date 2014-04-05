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
#include <malloc.h>
#include <limits.h>
#include <dos.h>

#include "dbgdefn.h"
#include "dbgreg.h"
#include "dbgio.h"

#include "tinyio.h"
#include "local.h"

extern void __buffered_keyboard_input( char * );
#pragma aux __buffered_keyboard_input = \
        _MOV_AH 0x0a \
        _INT_21 \
        parm caller [ds dx] \
        modify [ax];

static unsigned DOSErrCode( tiny_ret_t rc )
{
    if( TINY_OK( rc ) )
        return( 0 );
    return( StashErrCode( rc, OP_LOCAL ) );
}

void LocalTime( int *hour, int *min, int *sec, int *hundredths )
{
    tiny_time_t time;

    time = TinyGetTime();
    *hour = time.hour;
    *min = time.minutes;
    *sec = time.seconds;
    *hundredths = time.hundredths;
}

void LocalDate( int *year, int *month, int *day, int *weekday )
{
    tiny_date_t date;

    date = TinyGetDate();
    *year = date.year + 1900;
    *month = date.month;
    *day = date.day_of_month;
    *weekday = date.day_of_week;
}

int LocalInteractive( sys_handle fh )
/*******************************/
{
    tiny_ret_t rc;

    rc = TinyGetDeviceInfo( fh );
    if( TINY_ERROR( rc ) ) {
        return( 0 );
    }
    if( TINY_INFO( rc ) & TIO_CTL_DEVICE ) {
        return( 1 );
    }
    return( 0 );
}

void LocalGetBuff( char *buff, unsigned size )
/********************************************/
{
    char *new_buff;
    char *p;
    size_t len;

    if( size > UCHAR_MAX ) {
        size = UCHAR_MAX;
    }
    new_buff = alloca( size + 1 );
    if( new_buff == NULL ) {
        if( size > ( UCHAR_MAX - 2 ) ) {
            size -= 2;
        }
        new_buff = buff;
    }
    new_buff[0] = size;
    new_buff[1] = 0;
    __buffered_keyboard_input( new_buff );
    p = &new_buff[2];
    for( len = new_buff[1]; len != 0; --len ) {
        *(buff++) = *(p++);
    }
    *buff = '\0';
}

unsigned LocalRename( char *from, char *to )
/**************************************/
{
    return( DOSErrCode( TinyRename( from, to )) );
}

unsigned LocalMkDir( char *name )
/***************************/
{
    return( DOSErrCode( TinyMakeDir( name )) );
}

unsigned LocalRmDir( char *name )
/***************************/
{
    return( DOSErrCode( TinyRemoveDir( name )) );
}

unsigned LocalSetDrv( int drv )
/*************************/
{
    TinySetCurrDrive( drv );
    return( 0 );
}

int LocalGetDrv( void )
/*********************/
{
    return( TinyGetCurrDrive() );
}

unsigned LocalSetCWD( char *name )
/****************************/
{
    return( DOSErrCode( TinyChangeDir( name )) );
}

long LocalGetFileAttr( char *name )
/*********************************/
{
    tiny_ret_t rc;

    rc = TinyGetFileAttr( name );
    if( TINY_ERROR( rc ) ) {
        return( -1L );
    }
    return( TINY_INFO( rc ) );
}

long LocalGetFreeSpace( int drv )
/*******************************/
{
    return( TinyFreeSpace( drv ) );
}

unsigned LocalDateTime( sys_handle fh, int *time, int *date, int set )
/**************************************************************/
{
    tiny_ftime_t *ptime;
    tiny_fdate_t *pdate;
    tiny_ret_t rc;
    tiny_file_stamp_t *file_stamp;

    ptime = (tiny_ftime_t *)time;
    pdate = (tiny_fdate_t *)date;
    if( set ) {
        rc = TinySetFileStamp( fh, *ptime, *pdate );
    } else {
        rc = TinyGetFileStamp( fh );
        file_stamp = (void *) &rc;
        *ptime = file_stamp->time;
        *pdate = file_stamp->date;
    }
    return( DOSErrCode( rc) );
}

unsigned LocalGetCwd( int drv, char *where )
/**************************************/
{
    return( DOSErrCode( TinyGetCWDir( where, drv )) );
}

unsigned LocalFindFirst( char *pattern, void *info, unsigned info_len, int attrib )
/*****************************************************************************/
{
    info_len = info_len;
    TinySetDTA( info );
    return( DOSErrCode( TinyFindFirst( pattern, attrib )) );
}

unsigned LocalFindNext( void *info, unsigned info_len )
/*************************************************/
{
    info_len = info_len;
    TinySetDTA( info );
    return( DOSErrCode( TinyFindNext()) );
}

unsigned LocalSetFileAttr( char *name, long attr )
/********************************************/
{
    return( DOSErrCode( TinySetFileAttr( name, attr )) );
}
