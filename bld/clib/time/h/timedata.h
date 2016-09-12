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
* Description:  Specification of <time.h> internal interfaces.
*
****************************************************************************/

#ifndef TIMEDATA_H_INCLUDED
#define TIMEDATA_H_INCLUDED

#include <time.h>

extern int              __dst_adjust;
extern struct tm        __start_dst; /* start of daylight savings */
extern struct tm        __end_dst;   /* end of daylight savings */

#define SECONDS_FROM_1900_TO_1970       2208988800UL
#define SECONDS_PER_DAY                 ( 24 * 60 * 60UL )
#define DAYS_FROM_1900_TO_1970          ( ( long ) ( SECONDS_FROM_1900_TO_1970 / SECONDS_PER_DAY ) )

extern struct tm        *__brktime( unsigned long, time_t, long, struct tm * );
extern time_t           __local_mktime( const struct tm *, long *, long * );
extern int              __leapyear( unsigned );
extern int              __isindst( struct tm * );
extern int              __getctime( struct tm * );
extern int              __read_tzfile( const char *tz );
extern void             __check_tzfile( time_t t, struct tm *timep );

extern short const      __diyr[];  /* days in normal year array */
extern short const      __dilyr[]; /* days in leap year array */

#endif
