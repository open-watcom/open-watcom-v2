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
#include <dos.h>
#include <conio.h>
#include <extender.h>
#include "uidos.h"
#include "uiattrs.h"
#include "biosui.h"
#include "uidef.h"

/* this is default setting for the some of UI_DATA attributes */
static          MONITOR                 ui_data         =       {
                25,
                80,
                M_NEC_NORM,
                NULL,
                NULL,
                NULL,
                NULL,
                4,
                1
};

extern void global      uiexpendclock();
// from uiscreen.c in necdos\c
extern bool             uiIsHighRes();
extern unsigned int     getcolumns();
extern unsigned int     getrows();
extern LP_ZEN_CODE      code_pages[ 2 ];
extern LP_ZEN_ATTR      attr_pages[ 2 ];
int                     BIOSCurPage = -1;

extern void far * GetFarPointer( unsigned segment, unsigned offset );

void uisetcurrpage( int p )
{
    BIOSCurPage = p;
}

int CurrentPageNum()
{
    return( BIOSCurPage );
}

static ZEN_CODE hires_last_line_code[ 80 ];
static ZEN_ATTR hires_last_line_attr[ 80 ];

void hiresGrabLastLine()
{
    LP_ZEN_CODE         codedst;
    LP_ZEN_ATTR         attrdst;
    int                 jdx;

    codedst = CODE_LOOKUP( 24, 0 );
    attrdst = ATTR_LOOKUP( 24, 0 );

    for( jdx = 0; jdx < 80; jdx++ ) {
        hires_last_line_code[ jdx ].left = codedst[ jdx ].left;
        hires_last_line_code[ jdx ].right = codedst[ jdx ].right;
        hires_last_line_attr[ jdx ].left = attrdst[ jdx ].left;
        hires_last_line_attr[ jdx ].right = attrdst[ jdx ].right;
    }
}

void hiresRestoreLastLine()
{
    LP_ZEN_CODE         codedst;
    LP_ZEN_ATTR         attrdst;
    int                 jdx;

    codedst = CODE_LOOKUP( 24, 0 );
    attrdst = ATTR_LOOKUP( 24, 0 );

    for( jdx = 0; jdx < 80; jdx++ ) {
        codedst[ jdx ].left = hires_last_line_code[ jdx ].left;
        codedst[ jdx ].right = hires_last_line_code[ jdx ].right;
        attrdst[ jdx ].left = hires_last_line_attr[ jdx ].left;
        attrdst[ jdx ].right = hires_last_line_attr[ jdx ].right;
    }
}

void blankScreen()
{
    LP_ZEN_CODE         codedst;
    LP_ZEN_ATTR         attrdst;
    int                 idx;

    codedst = CODE_LOOKUP( 0, 0 );
    attrdst = ATTR_LOOKUP( 0, 0 );

    for( idx = 0; idx < ( UIData->height * UIData->width ); idx++ ) {
        codedst[ idx ].left = 0x20;
        codedst[ idx ].right = 0;
        attrdst[ idx ].left = 0xe1;
    }
}

/*
 * Set the video mode according to one of the nec_modes listed above.
 */
void intern setvideomode( unsigned mode )
{
    BIOSSetMode( mode );
}

bool global uiset80col()
{
    register    bool            status;

    status = FALSE;
    if( UIData->width != 80 ) {
        setvideomode( NEC_25_LINES | NEC_80_COLS );
        status = TRUE;
    }
    return( status );
} /* uiset80col */

bool intern initmonitor()
/***********************************************************************
 initmonitor:
 -----------
    Input:      NONE
    Output:     NONE

    This function gets current display mode information byte and set
    UIData attributes according to the bit pattern of mode information
    byte
************************************************************************/
{
    UIData->width = getcolumns();
    UIData->height = getrows();

    UIData->colour = ( ( uiIsHighRes() ) ? M_NEC_HIRES : M_NEC_NORM );

    return( TRUE );
}

/*
 * this routine converts milli-seconds into platform
 * dependant units - used to set mouse & timer delays
 */
unsigned global uiclockdelay( unsigned milli )
{
    return( milli / 20 );
}

int intern initbios()
/***********************************************************************
 initbios:
 --------
    Input:      NONE
    Output:     NONE

    This function sets video memory origin and initialize allocated
    memory location ( clear screen ) and also do some initialization
    for cursor, keyboard, and mouse
************************************************************************/
{
    int                                 initialized;
    int                                 index;
    LPPIXEL                             tmpptr;

    int                                 vscreen_alloc_amount;

    initialized = FALSE;
    if( BIOSCurPage == -1 ) BIOSCurPage = 0;

    if( UIData == NULL ) UIData = &ui_data;

    UIData->screen.origin = NULL;

    if( initmonitor() ) {
        UIData->desqview = FALSE;
        UIData->f10menus = TRUE;
        UIData->dbcs = TRUE;

        if( !uiIsHighRes() ) {
            code_pages[ 0 ] = GetFarPointer( 0xa000, 0 );
            code_pages[ 1 ] = GetFarPointer( 0xa100, 0 );
            attr_pages[ 0 ] = GetFarPointer( 0xa200, 0 );
            attr_pages[ 1 ] = GetFarPointer( 0xa300, 0 );
        } else {
            code_pages[ 0 ] = GetFarPointer( 0xe000, 0 );
            code_pages[ 1 ] = GetFarPointer( 0xe100, 0 );
            attr_pages[ 0 ] = GetFarPointer( 0xe200, 0 );
            attr_pages[ 1 ] = GetFarPointer( 0xe300, 0 );
            hiresGrabLastLine();
        }
        vscreen_alloc_amount = sizeof( PIXEL ) * UIData->height * UIData->width;
        UIData->screen.origin = ( LPPIXEL )
            faralloc(
                    ( vscreen_alloc_amount )
                    );
        UIData->screen.increment = UIData->width;

        if ( UIData->screen.origin != NULL ) {
            /*  clear allocated screen ( both page 1 & page 2 ) area using
             *  0x20 (space) as fill in character and 0xe1 ( white foreground,
             *  black background ) as its attribute
             */
            tmpptr = UIData->screen.origin;
            for ( index = 0 ;
                  index < ( UIData->height * UIData->width ) ;
                  ++index ) {
                tmpptr->ch = 0x20;
                tmpptr->attr = 0xe1;
                ++tmpptr;
            } /* for loop */

            uiinitcursor();
            initkeyboard();
            UIData->mouse_acc_delay = uiclockdelay( 278 );
            UIData->mouse_rpt_delay = uiclockdelay( 56 );
            UIData->mouse_clk_delay = uiclockdelay( 278 );
            UIData->tick_delay = uiclockdelay( 500 );
            UIData->mouse_speed = 8;       /* mickeys to ticks ratio */
            initialized = TRUE;
        } else {
            initialized = FALSE;
        }
    }
    return( initialized );
}

void intern finibios()
/***********************************************************************
 finibios:
 --------
    Input:      NONE
    Output:     NONE

    clean up routine after bios operation is done
************************************************************************/
{
    uifinicursor();
    if( UIData->screen.origin != NULL )
        farfree( UIData->screen.origin );
    /* clear screen area, mainly for NEC system menu recovery */
#ifndef _NEC_DEBUG
//    if( uiIsHighRes() ) {
//      blankScreen();
//      hiresRestoreLastLine();
 //   } else {
        BIOSClrScreen();
//    }
#endif
    uiexpendclock();
    BIOSInitCrt();
}
