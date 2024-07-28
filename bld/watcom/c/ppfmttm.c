/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Format information for __TIME__ and __DATE__.
*
****************************************************************************/


#include <stdlib.h>
#include "ppfmttm.h"


static const char * const Months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

void FormatTime_tm( char *buf, struct tm *t )
{
    div_t x;

    /*  01234567  */
    /* '00:00:00' */
    x = div( t->tm_sec, 10 );
    buf[7] = (char)( x.rem + '0' );
    x = div( x.quot, 10 );
    buf[6] = (char)( x.rem + '0' );
    x = div( t->tm_min, 10 );
    buf[4] = (char)( x.rem + '0' );
    x = div( x.quot, 10 );
    buf[3] = (char)( x.rem + '0' );
    x = div( t->tm_hour, 10 );
    buf[1] = (char)( x.rem + '0' );
    x = div( x.quot, 10 );
    buf[0] = (char)( x.rem + '0' );
}

void FormatDate_tm( char *buf, struct tm *t )
{
    div_t x;

    /*  01234567890  */
    /* 'Dec 00 0000' */
    x = div( t->tm_year + 1900, 10 );
    buf[10] = (char)( x.rem + '0' );
    x = div( x.quot, 10 );
    buf[9] = (char)( x.rem + '0' );
    x = div( x.quot, 10 );
    buf[8] = (char)( x.rem + '0' );
    x = div( x.quot, 10 );
    buf[7] = (char)( x.rem + '0' );
    x = div( t->tm_mday, 10 );
    buf[5] = (char)( x.rem + '0' );
    if( t->tm_mday < 10 ) {
        buf[4] = ' ';
    } else {
        x = div( x.quot, 10 );
        buf[4] = (char)( x.rem + '0' );
    }
    buf[2] = Months[t->tm_mon][2];
    buf[1] = Months[t->tm_mon][1];
    buf[0] = Months[t->tm_mon][0];
}

