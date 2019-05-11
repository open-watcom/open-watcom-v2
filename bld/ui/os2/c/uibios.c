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


#include <conio.h>
#include <stdio.h>
#include "uidef.h"
#include "doscall.h"
#include "uicurshk.h"


#define HANDLE 0

static MONITOR ui_data = {
    25,
    80,
    M_CGA,
    NULL,
    NULL,
    NULL,
    NULL,
    4,
    1
};


bool UIAPI uiset80col( void )
/****************************/
{
    return( true );
}


static bool initmonitor( void )
/*****************************/
{
    struct      _VIOMODEINFO            vioMode;
    struct      _VIOCONFIGINFO          config;

    if( UIData == NULL ) {
        UIData = &ui_data;
    }
    vioMode.cb = sizeof( vioMode );
    if( VioGetMode( &vioMode, HANDLE ) != 0 )
        return( false );

    UIData->width  = vioMode.col;
    UIData->height = vioMode.row;

    config.cb = sizeof( config );
    if( VioGetConfig( 0, &config, 0 ) != 0 )
        return( false );
    if( config.display == 3 ) {
        UIData->colour = M_BW;
    } else {
        switch( config.adapter ) {
        case 0:         UIData->colour = M_MONO; break;
        case 1:         UIData->colour = M_CGA; break;
        case 2:         UIData->colour = M_EGA; break;
        case 3:         UIData->colour = M_VGA; break;
        default:        UIData->colour = M_VGA; break;
        }
    }
    return( true );
}


bool intern initbios( void )
/**************************/
{
    bool            initialized;
    // unsigned        offset;
#ifdef _M_I86
    unsigned long   ptrLVB;
#else
    void            __far16 *ptrLVB;
    void            *ptr;
#endif
    unsigned short  SizeOfLVB;

    initialized = false;
    if( initmonitor() ) {
        VioGetBuf( (PULONG)&ptrLVB, (PUSHORT)&SizeOfLVB, 0 );
        // offset = BIOS_SCREEN_OFFSET;
#ifdef _M_I86
        UIData->screen.origin = (LP_PIXEL)ptrLVB;
#else
        ptr = ptrLVB;
        UIData->screen.origin = ptr;
#endif
        UIData->screen.increment = UIData->width;
        uiinitcursor();
        initkeyboard();
        UIData->mouse_acc_delay = uiclockdelay( 277 /* ms */ );  /* 5 ticks */
        UIData->mouse_rpt_delay = uiclockdelay( 55  /* ms */ );  /* 1 ticks */
        UIData->mouse_clk_delay = uiclockdelay( 277 /* ms */ );  /* 5 ticks */
        UIData->tick_delay      = uiclockdelay( 500 /* ms */ );  /* 9 ticks */
        UIData->mouse_speed = 8;        /* mickeys to ticks ratio */
        initialized = true;
    }
    return( initialized );
}


void intern finibios( void )
/**************************/
{
    uifinicursor();
    finikeyboard();
}


/* update the physical screen with contents of virtual copy */

void intern physupdate( SAREA *area )
/***********************************/
{
    int i;

    for( i = area->row; i < (area->row + area->height); i++ ) {
        VioShowBuf( ( i * UIData->width + area->col ) * sizeof( PIXEL ), area->width * sizeof( PIXEL ), 0 );
    }
}

