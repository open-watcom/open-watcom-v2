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


#include "variety.h"
#include <time.h>
#ifdef __NT__
    #include <windows.h>
#else
#endif
#include "find.h"


#ifdef __NT__


time_t __nt_filetime_cvt( FILETIME *ft )
{
    FILETIME            localft;
    SYSTEMTIME          systime;
    BOOL                rc;
    struct tm           tm;
    time_t              time;

    /*** If the time is 0,0 it's unsupported, so return -1 ***/
    if( ft->dwLowDateTime == 0  &&  ft->dwHighDateTime == 0 ) {
        return( (time_t)-1 );
    }

    /*** Convert from FILETIME to SYSTEMTIME format ***/
    rc = FileTimeToLocalFileTime( ft, &localft );
    if( rc == FALSE )  return( (time_t)-1 );
    rc = FileTimeToSystemTime( &localft, &systime );
    if( rc == FALSE )  return( (time_t)-1 );

    /*** Initialize the struct tm ***/
    tm.tm_year = systime.wYear - 1900;
    tm.tm_mon = systime.wMonth - 1;
    tm.tm_mday = systime.wDay;
    tm.tm_hour = systime.wHour;
    tm.tm_min = systime.wMinute;
    tm.tm_sec = systime.wSecond;
    tm.tm_wday = systime.wDayOfWeek;
    tm.tm_yday = -1;
    tm.tm_isdst = -1;

    /*** Convert to time_t form ***/
    time = mktime( &tm );               /* make a time_t */
    return( time );
}


#else   /* __NT__ */


#define DATE_DAY(__d)       ( (__d) & 0x001F )              /* [1..31] */
#define DATE_MONTH(__d)     ( ( (__d) & 0x01E0 ) >> 5 )     /* [1..12] */
#define DATE_YEAR(__d)      ( ( (__d) & 0xFE00 ) >> 9 )     /* since 1980 */

#define TIME_SECONDS(__t)   ( ( (__t) & 0x001F ) * 2 )      /* [0..58] */
#define TIME_MINUTES(__t)   ( ( (__t) & 0x07E0 ) >> 5 )     /* [0..59] */
#define TIME_HOURS(__t)     ( ( (__t) & 0xF800 ) >> 11 )    /* [0..23] */


time_t __dos_filetime_cvt( unsigned short ftime, unsigned short fdate )
{
    struct tm           tm;
    time_t              time;

    /*** Initialize the struct tm ***/
    tm.tm_year = DATE_YEAR(fdate) + 80;
    tm.tm_mon = DATE_MONTH(fdate) - 1;
    tm.tm_mday = DATE_DAY(fdate);
    tm.tm_hour = TIME_HOURS(ftime);
    tm.tm_min = TIME_MINUTES(ftime);
    tm.tm_sec = TIME_SECONDS(ftime);
    tm.tm_wday = -1;
    tm.tm_yday = -1;
    tm.tm_isdst = -1;

    /*** Convert to time_t form ***/
    time = mktime( &tm );               /* make a time_t */
    return( time );
}


#endif  /* __NT__ */
