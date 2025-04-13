/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
#include <stdlib.h>
#include <limits.h>
#include <dos.h>
#include "dbgdefn.h"
#include "rfxdata.h"
#include "dbgio.h"
#include "tinyio.h"
#include "trprfx.h"
#include "local.h"
#include "rfx.h"


#define SYSH2LH(sh)     (tiny_handle_t)((sh).u._32[0])
#define LH2SYSH(sh,lh)  (sh).u._32[0]=lh;(sh).u._32[1]=0

extern void __buffered_keyboard_input( char * );
#pragma aux __buffered_keyboard_input = \
        _MOV_AH DOS_BUFF_INPUT \
        _INT_21 \
    __parm __caller [__ds __dx] \
    __value         \
    __modify        [__ax]

static error_handle DOSErrCode( tiny_ret_t rc )
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

bool LocalInteractive( sys_handle sh )
/************************************/
{
    tiny_ret_t rc;

    rc = TinyGetDeviceInfo( SYSH2LH( sh ) );
    if( TINY_ERROR( rc ) ) {
        return( false );
    }
    if( TINY_INFO( rc ) & TIO_CTL_DEVICE ) {
        return( true );
    }
    return( false );
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
    *buff = NULLCHAR;
}

error_handle LocalRename( const char *from, const char *to )
/**********************************************************/
{
    return( DOSErrCode( TinyRename( from, to ) ) );
}

error_handle LocalMkDir( const char *name )
/*****************************************/
{
    return( DOSErrCode( TinyMakeDir( name ) ) );
}

error_handle LocalRmDir( const char *name )
/*****************************************/
{
    return( DOSErrCode( TinyRemoveDir( name ) ) );
}

error_handle LocalSetDrv( int drv )
/*********************************/
{
    TinySetCurrDrive( drv );
    return( 0 );
}

int LocalGetDrv( void )
/*********************/
{
    return( TinyGetCurrDrive() );
}

error_handle LocalSetCWD( const char *name )
/******************************************/
{
    return( DOSErrCode( TinyChangeDir( name ) ) );
}

long LocalGetFileAttr( const char *name )
/***************************************/
{
    tiny_ret_t rc;

    rc = TinyGetFileAttr( name );
    if( TINY_ERROR( rc ) ) {
        return( RFX_INVALID_FILE_ATTRIBUTES );
    }
    return( TINY_INFO( rc ) );
}

long LocalGetFreeSpace( int drv )
/*******************************/
{
    return( TinyFreeSpace( drv ) );
}

error_handle LocalDateTime( sys_handle sh, int *time, int *date, int set )
/************************************************************************/
{
    tiny_ftime_t *ptime;
    tiny_fdate_t *pdate;
    tiny_ret_t rc;
    tiny_file_stamp_t *file_stamp;

    ptime = (tiny_ftime_t *)time;
    pdate = (tiny_fdate_t *)date;
    if( set ) {
        rc = TinySetFileStamp( SYSH2LH( sh ), *ptime, *pdate );
    } else {
        rc = TinyGetFileStamp( SYSH2LH( sh ) );
        file_stamp = (void *)&rc;
        *ptime = file_stamp->time;
        *pdate = file_stamp->date;
    }
    return( DOSErrCode( rc ) );
}

error_handle LocalGetCwd( int drv, char *where, unsigned len )
/************************************************************/
{
    /* unused parameters */ (void)len;

    return( DOSErrCode( TinyGetCWDir( where, drv ) ) );
}

error_handle LocalFindFirst( const char *pattern, rfx_find *info, unsigned info_len, int attrib )
/***********************************************************************************************/
{
    /* unused parameters */ (void)info_len;

    TinySetDTA( info );
    return( DOSErrCode( TinyFindFirst( pattern, attrib ) ) );
}

int LocalFindNext( rfx_find *info, unsigned info_len )
/****************************************************/
{
    /* unused parameters */ (void)info_len;

    TinySetDTA( info );
    return( TinyFindNext() );
}

error_handle LocalFindClose( rfx_find *info, unsigned info_len )
/**************************************************************/
{
    /* unused parameters */ (void)info; (void)info_len;

    return( DOSErrCode( 0 ) );
}

error_handle LocalSetFileAttr( const char *name, long attr )
/**********************************************************/
{
    return( DOSErrCode( TinySetFileAttr( name, attr ) ) );
}
