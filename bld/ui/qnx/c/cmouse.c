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


#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/proxy.h>
#include <sys/mouse.h>
#include <sys/types.h>
#include <sys/timers.h>
#include <sys/kernel.h>
#include <sys/osinfo.h>
#include <sys/dev.h>
#include "uidef.h"
#include "uimouse.h"


#include "uivirt.h"
#include "qnxuiext.h"


#define MOUSE_SCALE             8


extern ORD                  MouseRow;
extern ORD                  MouseCol;

extern struct _mouse_ctrl   *MouseCtrl;

extern unsigned short       MouseStatus;
extern bool                 MouseInstalled;

static __segment            SysTimeSel;
static int                  ScaledRow;
static int                  ScaledCol;
static int                  MyStatus;
static timer_t              MouseTimer;

#if defined( _M_I86 )
    #define _SysTime    ((struct _timesel __far *)MK_FP( SysTimeSel, 0 ))
    #define GET_MSECS   (_SysTime->nsec / 1000000 + (_SysTime->seconds) * 1000)
#else
    extern unsigned long GetLong( unsigned short time_sel,
                                  unsigned long  time_off );

    #pragma aux GetLong = "mov ES,AX"           \
                          "mov EAX,ES:[EDX]"    \
                                                \
                          parm   [EAX] [EDX]    \
                          value  [EAX]          \
                          modify [ES];

    #define GET_MSECS   (GetLong( SysTimeSel, offsetof( struct _timesel, nsec ) ) / 1000000 \
                       + GetLong( SysTimeSel, offsetof( struct _timesel, seconds ) ) * 1000)
#endif


static int cm_check( unsigned short *status, unsigned short *row, unsigned short *col, unsigned long *time )
/**********************************************************************************************************/
{
    struct  mouse_event    event;
    struct  itimerspec     timer;

    if( MouseInstalled ) {
        if( mouse_read( MouseCtrl, &event, 1, UIRemProxy, 0 ) > 0 ) {
            ScaledRow -= event.dy;
            if( ScaledRow < 0 )
                ScaledRow = 0;
            if( ScaledRow > ( UIData->height - 1 ) * MOUSE_SCALE ) {
                ScaledRow = ( UIData->height - 1 ) * MOUSE_SCALE;
            }
            ScaledCol += event.dx;
            if( ScaledCol < 0 )
                ScaledCol = 0;
            if( ScaledCol > ( UIData->width - 1 ) * MOUSE_SCALE ) {
                ScaledCol = ( UIData->width - 1 ) * MOUSE_SCALE;
            }
            MyStatus = 0;
            if( event.buttons & _MOUSE_LEFT )
                MyStatus |= MOUSE_PRESS;
            if( event.buttons & _MOUSE_MIDDLE )
                MyStatus |= MOUSE_PRESS_MIDDLE;
            if( event.buttons & _MOUSE_RIGHT )
                MyStatus |= MOUSE_PRESS_RIGHT;
            timer.it_value.tv_sec = 0;
            timer.it_value.tv_nsec = 0;
        } else if( (MyStatus & MOUSE_PRESS_ANY) == 0 ) {
            timer.it_value.tv_sec = 0;
            timer.it_value.tv_nsec = 0;
        } else {
            timer.it_value.tv_sec = 0;
            timer.it_value.tv_nsec = UIData->mouse_rpt_delay * 1000000UL;
            if( timer.it_value.tv_nsec == 0 ) {
                timer.it_value.tv_nsec = 1;
            }
        }
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_nsec = 0;
        reltimer( MouseTimer, &timer, NULL );
        *status = MyStatus;
        *time = GET_MSECS;
        *row = ScaledRow / MOUSE_SCALE;
        *col = ScaledCol / MOUSE_SCALE;
    }
    uisetmouse( *row, *col );
    return( 0 );
}

static int cm_stop()
/*********************/
{
   struct itimerspec    timer;


    if( MouseInstalled ) {

        timer.it_value.tv_sec = 0;
        timer.it_value.tv_nsec = 0;

        reltimer( MouseTimer, &timer, NULL );
        mouse_read( MouseCtrl, NULL, 0, -1, 0 );
        while( Creceive( UILocalProxy, 0, 0 ) > 0 )
            {}
    }
    return 0;
}

static int cm_init( int install )
/*******************************/
{
    struct itimercb     timercb;
    struct _osinfo      osinfo;
    MOUSEORD            row;
    MOUSEORD            col;

    MouseInstalled = false;
    if( install == 0 )
        return( false );

    MouseCtrl = mouse_open( 0, 0, UIConHandle );
    if( MouseCtrl == 0 )
        return( false );
    timercb.itcb_event.evt_value = UIProxy;
    MouseTimer = mktimer( TIMEOFDAY, _TNOTIFY_PROXY, &timercb );
    if( MouseTimer == -1 ) {
        mouse_close( MouseCtrl );
        return( false );
    }
    MouseInstalled = true;

    UIData->mouse_xscale = 1;
    UIData->mouse_yscale = 1;

    qnx_osinfo( 0, &osinfo );
    SysTimeSel = osinfo.timesel;

    checkmouse( &MouseStatus, &row, &col, &MouseTime );
    MouseRow = row;
    MouseCol = col;
    stopmouse();
    return( true );
}


static int cm_fini( void )
/************************/
{
    if( MouseInstalled ) {
        uioffmouse();
        mouse_close( MouseCtrl );
        rmtimer( MouseTimer );
    }
    return 0;
}

static int cm_set_speed( int speed )
/**********************************/

/* Set speed of mouse. 1 is fastest; the higher the number the slower
 * it goes.
 * Since the mouse_param "gain" parameter is a "multiplier", a higher
 * number means faster. So, we've got to map from one scheme to another.
 * For now, 10 will be fastest and 1 will be slowest.
 */
{
    struct _mouse_param     mparam;

    if( MouseInstalled ) {
        if( mouse_param( MouseCtrl, 0, &mparam ) == 0 ) {
            if( speed <= 0 ) {
                mparam.gain = 10;
            } else if( speed > 10 ) {
                mparam.gain = 1;
            } else {
                mparam.gain = 11 - speed;
            }
            mouse_param( MouseCtrl, 1, &mparam );
        }
    }
    return 0;
}

Mouse ConsMouse = {
    cm_init,
    cm_fini,
    cm_set_speed,
    cm_stop,
    cm_check,
};
