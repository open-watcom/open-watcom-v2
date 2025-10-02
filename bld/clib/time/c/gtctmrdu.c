/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of __getctime() for RDOS
*
****************************************************************************/


#include "variety.h"
#include <rdos.h>
#include <time.h>
#include "getctime.h"


int _WCNEAR __getctime( struct tm *t )
{
    unsigned long msb;
    unsigned long lsb;
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int min = 0;
    int sec = 0;

    RdosGetTime( &msb, &lsb );
    RdosDecodeTicsBase( msb, lsb );

    __asm {
        movzx edx,dx
        mov year,edx
        movzx edx,ch
        mov month,edx
        movzx edx,cl
        mov day,edx
        movzx edx,bh
        mov hour,edx
        movzx edx,bl
        mov min,edx
        movzx edx,ah
        mov sec,edx
    }

    t->tm_year = year;
    t->tm_mon = month;
    t->tm_mday = day;
    t->tm_hour = hour;
    t->tm_min = min;
    t->tm_sec = sec;
    t->tm_year -= 1900;
    t->tm_mon--;
    t->tm_isdst = -1;

    return( 0 );
}
