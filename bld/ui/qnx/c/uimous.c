/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  QNX mouse input handling.
*
****************************************************************************/


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


void UIAPI uisetmouseposn( ORD row, ORD col )
{
    MouseRow = row;
    MouseCol = col;
}

void intern checkmouse( MOUSESTAT *status, MOUSEORD *row, MOUSEORD *col, MOUSETIME *time )
/****************************************************************************************/
{
    _checkmouse( status, row, col, time );
}

bool UIAPI initmouse( init_mode install )
/***************************************/
{
    return( _initmouse( install ) );
}

void UIAPI finimouse( void )
/**************************/
{
    _finimouse();
}

void UIAPI uimousespeed( unsigned speed )
/***************************************/

/* Set speed of mouse. 1 is fastest; the higher the number the slower
 * it goes.
 * Since the mouse_param "gain" parameter is a "multiplier", a higher
 * number means faster. So, we've got to map from one scheme to another.
 * For now, 10 will be fastest and 1 will be slowest.
 */
{
    _uimousespeed( speed );
}
