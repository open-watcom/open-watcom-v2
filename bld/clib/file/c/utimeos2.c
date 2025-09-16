/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  OS/2 implementation of utime().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <time.h>
#include <dos.h>
#include <sys/stat.h>
#include <direct.h>
#include <utime.h>
#include <wos2.h>
#include "seterrno.h"
#include "openmode.h"
#include "thread.h"


_WCRTLINK int __F_NAME(utime,_wutime)( CHAR_TYPE const *fn, struct utimbuf const *times )
/***************************************************************************************/
{
    APIRET      rc;
    OS_UINT     actiontaken;
    FILESTATUS  stat;
    HFILE       handle;
    struct tm   *split;
    time_t      curr_time;
    struct      utimbuf time_buf;
#ifdef __WIDECHAR__
    char        mbPath[MB_CUR_MAX * _MAX_PATH]; /* single-byte char */

    if( wcstombs( mbPath, fn, sizeof( mbPath ) ) == (size_t)-1 ) {
        mbPath[0] = '\0';
    }
#endif
    rc = DosOpen( (PSZ)__F_NAME(fn,mbPath), &handle, &actiontaken, 0,
                    FILE_NORMAL,
                    OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                    OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,
                    0 );
    if( rc != 0 ) {
        return( __set_errno_dos( rc ) );
    }
    if( DosQFileInfo( handle, 1, (PBYTE)&stat, sizeof( FILESTATUS ) ) != 0 ) {
        DosClose( handle );
        _RWD_errno = EACCES;
        return( -1 );
    }
    if( times == NULL ) {
        curr_time = time( NULL );
        time_buf.modtime = curr_time;
        time_buf.actime = curr_time;
        times = &time_buf;
    }
    split = localtime( &(times->modtime) );
    stat.fdateLastWrite.year     = split->tm_year - 80;
    stat.fdateLastWrite.month    = split->tm_mon + 1;
    stat.fdateLastWrite.day      = split->tm_mday;
    stat.ftimeLastWrite.hours    = split->tm_hour;
    stat.ftimeLastWrite.minutes  = split->tm_min;
    stat.ftimeLastWrite.twosecs  = split->tm_sec >> 1;

/*  5-Apr-90 DJG
 *  Early versions of OS/2 (1.10) do not support this fields properly.
 *  We'll assume that if OS/2 returned a value here that we can set it
 *  ourselves.  Day has to be non-zero if the date is valid.
 */
    if( stat.fdateLastAccess.day != 0 ) {
        split = localtime( &(times->actime) );
        stat.fdateLastAccess.year    = split->tm_year - 80;
        stat.fdateLastAccess.month   = split->tm_mon + 1;
        stat.fdateLastAccess.day     = split->tm_mday;
        stat.ftimeLastAccess.hours   = split->tm_hour;
        stat.ftimeLastAccess.minutes = split->tm_min;
        stat.ftimeLastAccess.twosecs = split->tm_sec >> 1;
    }

    if( DosSetFileInfo( handle, 1, (PBYTE)&stat, sizeof( FILESTATUS ) ) != 0 ) {
        DosClose( handle );
        _RWD_errno = EACCES;
        return( -1 );
    }
    if( DosClose( handle ) != 0 ) {
        _RWD_errno = EACCES;
        return( -1 );
    }
    return( 0 );
}
