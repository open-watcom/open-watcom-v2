/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Basic screen initialization for RDOS
*
****************************************************************************/


#include <rdos.h>
#include <conio.h>
#include <stdio.h>
#include "uidef.h"
#include "uicurshk.h"


static MONITOR ui_data = {
    25,
    80,
    M_VGA,
    NULL,
    NULL,
    NULL,
    NULL,
    4,
    1
};

bool UIAPI uiset80col( void )
{
    return( true );
}

bool intern initbios( void )
{
    LP_PIXEL    bufptr;
    size_t      size;
    size_t      i;
    int         height;
    int         width;

    if( UIData == NULL ) {
        UIData = &ui_data;
    }

    UIData->colour = M_VGA;
    RdosGetTextSize( &height, &width );
    UIData->height = height;
    UIData->width = width;
    size = UIData->width * UIData->height * sizeof( PIXEL );
    bufptr = uimalloc( size );
    size /= sizeof( PIXEL );
    for( i = 0; i < size; i++ ) {
        bufptr[i].ch = ' ';
        bufptr[i].attr = 0x07;
    }
    UIData->screen.origin = bufptr;
    UIData->screen.increment = UIData->width;

    uiinitcursor();
    initkeyboard();
    UIData->mouse_acc_delay = uiclockdelay( 250 /* ms */ );
    UIData->mouse_rpt_delay = uiclockdelay( 100 /* ms */ );
    UIData->mouse_clk_delay = uiclockdelay( 250 /* ms */ );
    UIData->tick_delay      = uiclockdelay( 500 /* ms */ );
    UIData->mouse_speed = 8;

    return( true );
}

void intern finibios( void )
{
    uifinicursor();
    finikeyboard();
}

void intern physupdate( SAREA *area )
{
    int i;
    int pos;
    short *bufptr = (short *)UIData->screen.origin;

    for( i = 0; i < area->height; i++ ) {
        pos = UIData->width * (i + area->row) + area->col;
        bufptr = (short *)UIData->screen.origin + pos;
        RdosWriteAttributeString( i + area->row, area->col, bufptr, area->width );
    }
}
