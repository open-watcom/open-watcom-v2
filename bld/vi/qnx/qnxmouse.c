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


#include "vi.h"
#include <unistd.h>
#include <sys/seginfo.h>
#include <sys/console.h>
#include <sys/con_msg.h>
#include <sys/dev.h>
#include <sys/osinfo.h>
#include <sys/sidinfo.h>
#include <sys/psinfo.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/proxy.h>
#include <sys/vc.h>
#include <sys/mouse.h>
#include <sys/timers.h>
#include "mouse.h"

static int                      lastStatus;
static int                      lastRow;
static int                      lastCol;
static timer_t                  mouseTimer;
static struct _mouse_ctrl       *mouseCtrl;
static struct _timesel          *sysTime;
static struct _mouse_param      SaveParms;


extern pid_t                    Proxy;
extern int                      QNXConHandle;

#define MOUSE_SCALE     8

/*
 * SetMouseSpeed - set mouse movement speed
 */
void SetMouseSpeed( int speed )
{
    struct _mouse_param     mparam;

    if( EditFlags.UseMouse && !EditFlags.IsWindowedConsole ) {
        if( mouseCtrl == NULL ) return;
        if( mouse_param( mouseCtrl, 0, &mparam ) == 0 ) {
            SaveParms = mparam;
            speed /= 2;
            if( speed <= 0 ) {
                mparam.gain = 10;
            } else if( speed > 5 ) {
                mparam.gain = 1;
            } else {
                mparam.gain = 6 - speed;
            }
            mparam.gain += 2;
            mouse_param( mouseCtrl, 1, &mparam );
        }
    }

} /* SetMouseSpeed */

/*
 * SetMousePosition - set the mouse position
 */
void SetMousePosition( int row, int col )
{
    lastRow = MouseRow = row;
    lastCol = MouseCol = col;

} /* SetMousePosition */

/*
 * PollMouse - poll the mouse for it's state
 */
void PollMouse( int *status, int *row, int *col )
{
    struct mouse_event  event;
    struct itimerspec   timer;
    bool                rc;

    if( !EditFlags.UseMouse ) {
        return;
    }
    rc = FALSE;
    if( EditFlags.IsWindowedConsole ) {
    } else {
        if( mouseCtrl == NULL ) return;
        if( mouse_read( mouseCtrl, &event, 1, Proxy, 0 ) > 0 ) {
            rc = TRUE;
            lastRow -= event.dy;
            if( lastRow < 0 ) {
                lastRow = 0;
            }
            if( lastRow > (WindMaxHeight-1) * MOUSE_SCALE ) {
                lastRow = (WindMaxHeight-1) * MOUSE_SCALE;
            }
            lastCol += event.dx;
            if( lastCol < 0 ) {
                lastCol = 0;
            }
            if( lastCol > (WindMaxWidth-1) * MOUSE_SCALE ) {
                lastCol = (WindMaxWidth-1) * MOUSE_SCALE;
            }
            lastStatus = 0;
            if( event.buttons & _MOUSE_LEFT ) {
                lastStatus |= MOUSE_LEFT_BUTTON_DOWN;
            }
            if( event.buttons & _MOUSE_MIDDLE ) {
                lastStatus |= MOUSE_MIDDLE_BUTTON_DOWN;
            }
            if( event.buttons & _MOUSE_RIGHT ) {
                lastStatus |= MOUSE_RIGHT_BUTTON_DOWN;
            }
            timer.it_value.tv_sec = 0;
            timer.it_value.tv_nsec = 0;
        } else if( !(lastStatus & MOUSE_ANY_BUTTON_DOWN ) ) {
            timer.it_value.tv_sec = 0;
            timer.it_value.tv_nsec = 0;
        } else {
            timer.it_value.tv_sec = 0;
            timer.it_value.tv_nsec = MouseRepeatDelay * 1000000UL;
            if( timer.it_value.tv_nsec == 0 ) {
                timer.it_value.tv_nsec = 1;
            }
        }
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_nsec = 0;
        reltimer( mouseTimer, &timer, NULL );
        *status = lastStatus;
        *row = lastRow / MOUSE_SCALE;
        *col = lastCol / MOUSE_SCALE;
    }

} /* PollMouse */

/*
 * InitMouse - initialize the mouse
 */
void InitMouse( void )
{
    struct itimercb     timercb;
    struct _osinfo      osinfo;

    if( !EditFlags.UseMouse ) {
        return;
    }

    EditFlags.UseMouse = FALSE;

    if( EditFlags.IsWindowedConsole ) {
//      write( QNXConHandle, QW_INIT, sizeof( QW_INIT ) - 1 );
    } else {
        mouseCtrl = mouse_open( 0, 0, QNXConHandle );
        if( mouseCtrl == NULL ) {
            return;
        }
        timercb.itcb_event.evt_value = Proxy;
        mouseTimer = mktimer( TIMEOFDAY, _TNOTIFY_PROXY, &timercb );
        if( mouseTimer == -1 ) {
            mouse_close( mouseCtrl );
            return;
        }
    }
    EditFlags.UseMouse = TRUE;

    qnx_osinfo( 0, &osinfo );
    sysTime = MK_FP( osinfo.timesel, 0 );

    SetMousePosition( WindMaxWidth/2-1, WindMaxHeight/2-1 );
    SetMouseSpeed( MouseSpeed );
    PollMouse( &MouseStatus, &MouseRow, &MouseCol );
    StopMouse();

} /* InitMouse */

/*
 * FiniMouse - done with the mouse
 */
void FiniMouse( void )
{
    if( EditFlags.UseMouse ) {
        if( EditFlags.IsWindowedConsole ) {
//          write( UIConHandle, QW_FINI, sizeof( QW_FINI ) - 1 );
        } else {
            if( mouseCtrl == NULL ) return;
            mouse_param( mouseCtrl, 1, &SaveParms );
            mouse_close( mouseCtrl );
            rmtimer( mouseTimer );
        }
    }

} /* FiniMouse */

void StopMouse( void )
{
    struct itimerspec   timer;


    if( EditFlags.UseMouse && !EditFlags.IsWindowedConsole ) {

        timer.it_value.tv_sec = 0;
        timer.it_value.tv_nsec = 0;

        if( mouseCtrl == NULL ) return;
        reltimer( mouseTimer, &timer, NULL );
        mouse_read( mouseCtrl, NULL, 0, -1, 0 );
        while( Creceive( Proxy, 0, 0 ) > 0 );
    }
} /* StopMouse */
