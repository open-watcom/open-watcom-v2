/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  conversion function for DOS timestamp -> time_t (local time)
*
****************************************************************************/


#include "variety.h"
#include "d2timet.h"


enum {
    TIME_SEC_B  = 0,
    TIME_SEC_F  = 0x001f,
    TIME_MIN_B  = 5,
    TIME_MIN_F  = 0x07e0,
    TIME_HOUR_B = 11,
    TIME_HOUR_F = 0xf800
};

enum {
    DATE_DAY_B  = 0,
    DATE_DAY_F  = 0x001f,
    DATE_MON_B  = 5,
    DATE_MON_F  = 0x01e0,
    DATE_YEAR_B = 9,
    DATE_YEAR_F = 0xfe00
};

time_t _INTERNAL __dos2timet( unsigned short dos_date, unsigned short dos_time )
/******************************************************************************/
{
    struct tm t;

    t.tm_year  = ((dos_date & DATE_YEAR_F) >> DATE_YEAR_B) + 80;
    t.tm_mon   = ((dos_date & DATE_MON_F) >> DATE_MON_B) - 1;
    t.tm_mday  = (dos_date & DATE_DAY_F) >> DATE_DAY_B;

    t.tm_hour  = (dos_time & TIME_HOUR_F) >> TIME_HOUR_B;
    t.tm_min   = (dos_time & TIME_MIN_F) >> TIME_MIN_B;
    t.tm_sec   = ((dos_time & TIME_SEC_F) >> TIME_SEC_B) * 2;

    t.tm_wday  = -1;
    t.tm_yday  = -1;
    t.tm_isdst = -1;

    return( mktime( &t ) );
}
