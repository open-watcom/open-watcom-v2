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


#include <i86.h>
#include "uidef.h"
#include "uimouse.h"

#define MOUSE_INT           0x33

extern short FMRMouseInit( void );
#pragma aux FMRMouseInit =      \
    "mov    ax,0000h"           \
    "int    33h"                \
    value   [ax];

extern void FMRMouseCursor( char invisible );
#pragma aux FMRMouseCursor =    \
    "inc    ax"                 \
    "int    33h"                \
    parm    [ax];

struct mouse_stat {
    short horz;
    short vert;
    short pres_rel;
    short buttons;
};

extern void FMRMouseStat( struct mouse_stat * );
#pragma aux FMRMouseStat =     \
    "push   si"                 \
    "push   dx"                 \
    "push   cx"                 \
    "push   bx"                 \
    "mov    si,ax"              \
    "mov    ax,0003h"           \
    "int    33h"                \
    "mov    [si],cx"            \
    "mov    2[si],dx"           \
    "mov    6[si],bx"           \
    "pop    bx"                 \
    "pop    cx"                 \
    "pop    dx"                 \
    "pop    si"                 \
    parm    [ax];

/*
    column - horizontal position
    row    - vertical   position
*/

extern void FMRMouseSetPosn( short col, short row );
#pragma aux FMRMouseSetPosn =  \
    "mov    ax,0004h"           \
    "int    33h"                \
    parm    [cx][dx];

extern void FMRMousePress( short button, struct mouse_stat * );
#pragma aux FMRMousePress =     \
    "push   si"                 \
    "push   cx"                 \
    "push   bx"                 \
    "mov    si,dx"              \
    "mov    bx,ax"              \
    "mov    ax,0005h"           \
    "int    33h"                \
    "mov    [si],cx"            \
    "mov    2[si],dx"           \
    "mov    4[si],bx"           \
    "mov    6[si],ax"           \
    "pop    bx"                 \
    "pop    cx"                 \
    "pop    si"                 \
    parm    [ax][dx];

extern void FMRMouseRelease( short button, struct mouse_stat * );
#pragma aux FMRMouseRelease =   \
    "push   si"                 \
    "push   cx"                 \
    "push   bx"                 \
    "mov    si,dx"              \
    "mov    bx,ax"              \
    "mov    ax,0006h"           \
    "int    33h"                \
    "mov    [si],cx"            \
    "mov    2[si],dx"           \
    "mov    4[si],bx"           \
    "mov    6[si],ax"           \
    "pop    bx"                 \
    "pop    cx"                 \
    "pop    si"                 \
    parm    [ax][dx];

extern void FMRMouseTextCursor( void );
#pragma aux FMRMouseTextCursor = \
    "push   dx"                 \
    "push   cx"                 \
    "push   bx"                 \
    "mov    bx,0"               \
    "mov    cx,0xffff"          \
    "mov    dx,0x7700"          \
    "mov    ax,000ah"           \
    "int    33h"                \
    "pop    bx"                 \
    "pop    cx"                 \
    "pop    dx";

extern void FMRMouseSpeed( int speed );
#pragma aux FMRMouseSpeed =     \
    "push   dx"                 \
    "push   cx"                 \
    "mov    cx,ax"              \
    "mov    dx,ax"              \
    "mov    ax,000fh"           \
    "int    33h"                \
    "pop    cx"                 \
    "pop    dx"                 \
    parm    [ax];

extern void FMRMouseRegionUpdate( unsigned short r1, unsigned short c1,
                                  unsigned short r2, unsigned short c2 );
#pragma aux FMRMouseRegionUpdate = \
    "mov    ax,0010h"           \
    "int    33h"                \
    parm    [cx][dx][si][di];

extern void FMRSystemConfig( unsigned short offset, unsigned short segment );
#pragma aux FMRSystemConfig =   \
    "push   ds"                 \
    "push   di"                 \
    "mov    di,ax"              \
    "mov    ds,dx"              \
    "mov    ah,05h"             \
    "int    0afh"               \
    "pop    di"                 \
    "pop    ds"                 \
    parm    [ax][dx];



extern unsigned long uiclock( void );

extern          MOUSEORD                MouseRow;       /* UIMOUSEV.C */
extern          MOUSEORD                MouseCol;       /* UIMOUSEV.C */
extern          unsigned short          MouseStatus;    /* UIMOUSEV.C */
extern          bool                    MouseInstalled; /* UIMOUSEV.C */
extern          bool                    MouseOn;        /* UIMOUSEV.C */

static          unsigned long           MouseClock          = 0L;
static          unsigned short          MouseSlow           = 0;
static          struct mouse_stat       MousePressStatus    = { 0,0,0,0 };
static          struct mouse_stat       MouseReleaseStatus  = { 0,0,0,0 };

void global uisetmouseposn(             /* SET MOUSE POSITION */
    ORD row,                            /* - mouse row        */
    ORD col )                           /* - mouse column     */
{
    MouseRow = row * UIData->mouse_yscale;
    MouseCol = col * UIData->mouse_xscale;
    FMRMouseSetPosn( MouseCol, MouseRow );
}

void global uimousesave( SAREA *area )
/****************************************
    area->row    \___ starting row,column
    area->col    /
    area->height \___ dimensions of area
    area->width  /
 ****************************************/
{
    unsigned short row1, col1, row2, col2;

    if( MouseInstalled == TRUE ) {
        /*
         * tell the mouse driver the area of screen we are updating
         * in case we overwrite the mouse cursor
         */
        if( MouseOn ) {
            row1 = area->row * UIData->mouse_yscale;
            col1 = area->col * UIData->mouse_xscale;
            row2 = (area->row + area->height) * UIData->mouse_yscale;
            col2 = (area->col + area->width) * UIData->mouse_xscale;
            FMRMouseRegionUpdate( col1, row1, col2, row2 );
        }
    }
}

void global uimouserestore()
{
    if( MouseInstalled == TRUE ) {
        /* turn on mouse cursor in case we turned it off with uiregionupdate */
        if( MouseOn ) {
            uimouse( MOUSE_ON );
        }
    }
}

void uimousespeed( unsigned speed )
/*********************************/
/*
    Set speed of mouse (0 is fastest, 255 is slowest).
*/
{
    if( speed > 255 ) speed = 255;
    FMRMouseSpeed( (char) speed );
    UIData->mouse_speed = speed;
}

bool global initmouse( install )
/******************************/

register        int                     install;
{
    char system_config[16];

    MouseInstalled = FALSE;
    if( install > 0 && installed( MOUSE_INT ) ) {
        if( FMRMouseInit() != 0x0000 ) {
            /*
                The cursor is off but positioned in the middle of
                the screen.  Read position to determine resolution.
                The FM-R PC supports either 1120x750 or 640x400 resolution.
            */
            FMRSystemConfig( FP_OFF( system_config), FP_SEG( system_config ) );
            if( (system_config[3] & 0x03) == 2 ) {
                UIData->mouse_xscale = 1120 / 80;
                UIData->mouse_yscale = 750 / 25;
            } else {
                UIData->mouse_xscale = 640 / 80;
                UIData->mouse_yscale = 400 / 25;
            }
            FMRMouseTextCursor();
            uisetmouseposn( UIData->height/2 - 1, UIData->width/2 - 1 );
            UIData->mouse_swapped = FALSE;
            checkmouse( &MouseStatus, &MouseRow, &MouseCol, &MouseClock );
            uimousespeed( UIData->mouse_speed );
            MouseOn = FALSE;
            MouseInstalled = TRUE;
        }
    }
    return( MouseInstalled );
}

void global uimouse( func )
/*************************/
{
    if( func == MOUSE_ON ) {
        /* make cursor visible */
        FMRMouseCursor( 0 );
    } else {
        /* make cursor invisible */
        FMRMouseCursor( 1 );
    }
}

void intern checkmouse( stat, row, col, time )
/**********************************************/

register        unsigned short          *stat;
register        MOUSEORD                *row;
register        MOUSEORD                *col;
register        unsigned long           *time;
{
    struct mouse_stat status;
    unsigned long start;

    if( (MousePressStatus.pres_rel + MouseReleaseStatus.pres_rel) == 0 ) {
        /* get mouse status and number of left button presses */
        FMRMousePress( 0, &MousePressStatus );
        /* get mouse status and number of left button releases */
        FMRMouseRelease( 0, &MouseReleaseStatus );
    }
    if( (MousePressStatus.pres_rel + MouseReleaseStatus.pres_rel) != 0 ) {
        if( MousePressStatus.pres_rel >= MouseReleaseStatus.pres_rel ) {
            status.buttons = 0x01;      /* simulate left button press */
            status.vert = MousePressStatus.vert;
            status.horz = MousePressStatus.horz;
            MousePressStatus.pres_rel--;
        } else {
            status.buttons = 0x00;      /* simulate left button release */
            status.vert = MouseReleaseStatus.vert;
            status.horz = MouseReleaseStatus.horz;
            MouseReleaseStatus.pres_rel--;
        }
        start = MouseClock;
    } else {
        FMRMouseStat( &status );
        start = uiclock();
    }
    *stat = status.buttons;
    *row = status.vert;
    *col = status.horz;
    *time = start;
    if( (start - MouseClock) > 250 ) MouseSlow++;
    MouseClock = start;
}

void global finimouse()
/*********************/
{
    if( MouseInstalled ) {
        uioffmouse();
        MouseInstalled = FALSE;
    }
}
