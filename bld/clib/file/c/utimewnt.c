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
* Description:  Win32 implementation of utime().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stddef.h>
#include <sys/types.h>
#include <time.h>
#include <dos.h>
#include <windows.h>
#include <sys/stat.h>
#include <errno.h>
#include <direct.h>
#include <sys/utime.h>
#include "libwin32.h"
#include "openmode.h"
#include "rtdata.h"
#include "seterrno.h"
#ifdef __WIDECHAR__
    #include <mbstring.h>
#endif

_WCRTLINK int __F_NAME(utime,_wutime)( CHAR_TYPE const *fn, struct utimbuf const *times )
/**********************************************************************************/
{
    HANDLE              h;
    struct tm           *split;
    time_t              curr_time;
    struct utimbuf      time_buf;
    FILETIME            fctime,fatime,fwtime;
    FILETIME            local_ft;
    SYSTEMTIME          atime,wtime;

    #ifdef __WIDECHAR__
        h = __lib_CreateFileW( fn, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                               OPEN_EXISTING, 0, NULL );
    #else
        h = CreateFileA( fn, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                         OPEN_EXISTING, 0, NULL );
    #endif
    if( h == (HANDLE)-1 ) {
        return( __set_errno_nt() );
    }
    if( !GetFileTime( h, &fctime, &fatime, &fwtime ) ) {
        CloseHandle( h );
        return( __set_errno_nt() );
    }
    if( times == NULL ) {
        curr_time = time( NULL );
        time_buf.modtime = curr_time;
        time_buf.actime = curr_time;
        times = &time_buf;
    }
    split = localtime( &(times->modtime) );
    wtime.wYear = atime.wYear = 1900 + split->tm_year;
    wtime.wMonth = atime.wMonth = split->tm_mon + 1;
    wtime.wDay = atime.wDay = split->tm_mday;
    wtime.wHour = atime.wHour = split->tm_hour;
    wtime.wMinute = atime.wMinute = split->tm_min;
    wtime.wSecond = atime.wSecond = split->tm_sec;
    wtime.wMilliseconds = atime.wMilliseconds = 0;
    SystemTimeToFileTime( &wtime, &local_ft );
    LocalFileTimeToFileTime( &local_ft, &fwtime );
    SystemTimeToFileTime( &atime, &local_ft );
    LocalFileTimeToFileTime( &local_ft, &fatime );

    if( !SetFileTime( h, &fctime, &fatime, &fwtime ) ) {
        CloseHandle( h );
        return( __set_errno_nt() );
    }

    CloseHandle( h );
    return( 0 );
}
