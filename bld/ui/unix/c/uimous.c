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
#include <sys/types.h>
#include "uidef.h"
#include "uimouse.h"
#include "uivirt.h"

extern ORD MouseRow, MouseCol;

void global uisetmouseposn(             /* SET MOUSE POSITION */
    ORD row,                            /* - mouse row        */
    ORD col )                           /* - mouse column     */
{
    MouseRow = row;
    MouseCol = col;
}

void intern checkmouse( unsigned short *status, MOUSEORD *row,
                        MOUSEORD *col, unsigned long *time )
/**********************************************/
{
    _checkmouse(status, row, col, time);
}

void intern stopmouse( void )
/***************************/
{
    _stopmouse();
}

bool global initmouse( int install )
/**********************************/
{
    return _initmouse( install );
}

void global finimouse( void )
/***************************/
{
    _finimouse();
}

void global uimousespeed( unsigned speed )
/****************************************/

/* Set speed of mouse. 1 is fastest; the higher the number the slower
 * it goes.
 * Since the mouse_param "gain" parameter is a "multiplier", a higher
 * number means faster. So, we've got to map from one scheme to another.
 * For now, 10 will be fastest and 1 will be slowest.
 */
{
    _uimousespeed(speed);
}
