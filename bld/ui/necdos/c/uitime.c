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


#ifdef __386__
#include "stdui.h"
#include "biosui.h"

typedef unsigned short int timer_t;

unsigned long int clock_ticks;

#pragma aux uiBangUponTheHardware = \
        "mov al, 30h "\
        "out 77h, al "\
        "mov al, bl "\
        "out 71h, al "\
        "mov al, bh "\
        "out 71h, al "\
        "in al, 02h "\
        "and al, 0feh "\
        "out 02h, al "\
        modify [al] parm [bx];
extern void uiBangUponTheHardware( timer_t bx );

#pragma aux uiAcquireBogusTimerInfo = \
        "mov al, 00h "\
        "out 77h, al "\
        "in al, 71h "\
        "mov bl, al "\
        "in al, 71h "\
        "mov bh, al "\
        "mov ax, bx "\
        modify [ax bx] value [ax];
extern timer_t uiAcquireBogusTimerInfo();

#define TICKS_PER_CENTISECOND (20000)
#define ELAPSED( n ) \
    ( uiAcquireBogusTimerInfo() < ( 0xFFFF - ( TICKS_PER_CENTISECOND * (n) ) ) )

void uirefreshclock()
{
    if( ELAPSED( 1 ) ) {
        ++clock_ticks;
        uiBangUponTheHardware( 0xFFFF );
    }
}

unsigned long uiclock()
{
    uirefreshclock();
    return( clock_ticks );
}
#endif
