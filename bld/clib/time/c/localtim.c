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
* Description:  _localtime() converts time_t to struct tm
*
****************************************************************************/

#include "variety.h"
#include <time.h>
#include "rtdata.h"
#include "thetime.h"
#include "timedata.h"

_WCRTLINK struct tm *_localtime( const time_t *timer, struct tm *t )
{
    time_t      tod;

    tzset();
    tod = *timer;
    t->tm_isdst = -1;
#ifdef __LINUX__
    __check_tzfile( tod, t );
#endif
    __brktime( DAYS_FROM_1900_TO_1970, tod, _RWD_timezone, t );

    if( __isindst( t ) )
        __brktime( DAYS_FROM_1900_TO_1970, tod,
                   _RWD_timezone - _RWD_dst_adjust, t );
    return( t );
}

_WCRTLINK struct tm *localtime( const time_t *timer )
{
    _INITTHETIME;
    return _localtime( timer, &_THE_TIME );
}
