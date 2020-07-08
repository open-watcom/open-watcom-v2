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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <dos.h>
#include "uidef.h"
#include "int33.h"
#include "charmap.h"
#include "uimouse.h"
#include "uibmous.h"
#include "realmod.h"


#define CURSOR_HEIGHT   14                       /* Mouse cursor height      */

#define DEFCHAR         0xD5
#define DEFCHAR2        0xD7

typedef enum {
    ERASE,
    DRAW,
    SAVE
} plot_func;

static unsigned char    SaveChars[2][2];        /* Overwritten characters  */
static unsigned char    CharDefs[64];           /* Character definitons.    */
static unsigned char    SaveDefs[64];           /* Saved character defs     */

// Masks for the cursor

static unsigned short MouCursorMask[CURSOR_HEIGHT] =  {
    0x0000,  /*0000000000000000*/
    0x4000,  /*0100000000000000*/
    0x6000,  /*0110000000000000*/
    0x7000,  /*0111000000000000*/
    0x7800,  /*0111100000000000*/
    0x7c00,  /*0111110000000000*/
    0x7e00,  /*0111111000000000*/
    0x7f00,  /*0111111100000000*/
    0x7c00,  /*0111110000000000*/
    0x4600,  /*0100011000000000*/
    0x0600,  /*0000011000000000*/
    0x0300,  /*0000001100000000*/
    0x0300,  /*0000001100000000*/
    0x0000   /*0000000000000000*/
};

static unsigned short MouScreenMask[CURSOR_HEIGHT] =  {
    0x3fff,  /*0011111111111111*/
    0x1fff,  /*0001111111111111*/
    0x0fff,  /*0000111111111111*/
    0x07ff,  /*0000011111111111*/
    0x03ff,  /*0000001111111111*/
    0x01ff,  /*0000000111111111*/
    0x00ff,  /*0000000011111111*/
    0x007f,  /*0000000001111111*/
    0x01ff,  /*0000000111111111*/
    0x10ff,  /*0001000011111111*/
    0xb0ff,  /*1011000011111111*/
    0xf87f,  /*1111100001111111*/
    0xf87f,  /*1111100001111111*/
    0xfcff   /*1111110011111111*/
};

//  Plot the cursor on the screen, save background, draw grid, etc.

static void PlotEgaVgaCursor( plot_func action )
{
    static unsigned lsavex = 0;
    static unsigned lsavey = 0;
    unsigned        width;
    unsigned        height;
    unsigned        disp;
    unsigned        i;
    unsigned        j;
    unsigned        x;
    unsigned        y;
    LP_PIXEL        screen;

    switch( action ) {
    case ERASE :                        /* Erase grid, put save info    */
        x = lsavex;
        y = lsavey;
        break;
    case DRAW :                         /* Draw grid                    */
        x = MouseCol / 8;
        y = MouseRow / Points;
        break;
    case SAVE :                         /* Save grid                    */
        x = lsavex = MouseCol / 8;
        y = lsavey = MouseRow / Points;
        break;
    }

    width = UIData->width - x;
    if( width > 2 ) {
        width = 2;
    }

    height = UIData->height - y;
    if( height > 2 ) {
        height = 2;
    }

    screen = UIData->screen.origin + y * UIData->width + x;
    disp = UIData->width - width;

    switch( action ) {
    case ERASE:
        for( i = 0; i < height; i++, screen += disp ) {
            for( j = 0; j < width; j++, screen++ ) {
                screen->ch = SaveChars[i][j];
            }
        }
        break;
    case DRAW:
        for( i = 0; i < height; i++, screen += disp ) {
            if( width > 0 ) {
                screen->ch = DEFCHAR + i;
                screen++;
            }
            if( width > 1 ) {
                screen->ch = DEFCHAR2 + i;
                screen++;
            }
        }
        break;
    case SAVE:
        for( i = 0; i < height; i++, screen += disp ) {
            for( j = 0; j < width; j++, screen++ ) {
                SaveChars[i][j] = screen->ch;
            }
        }
        break;
    }
}

static void intern DrawEgaVgaCursor( void )
{
    unsigned short  off;
    unsigned short  shift;
    unsigned short  addmask;
    unsigned short  i;
    unsigned short  j;
    unsigned short  s1;
    unsigned short  s2;
    unsigned short  *defs;
    unsigned short  *masks;

    PlotEgaVgaCursor( SAVE );                   /* Save the current grid    */

    SetSequencer();                             /* Program the sequencer    */
    off = 0;
    for( i = 0; i < 2;  i++ ) {                 /* The grid is 2 chars high */
        s1 = SaveChars[i][0] * 32;
        s2 = SaveChars[i][1] * 32;
        for( j = 0; j < Points; j++ ) {
            CharDefs[off++] = VIDEOData( 0xa000, s2++ );
            CharDefs[off++] = VIDEOData( 0xa000, s1++ );
        }
    }

    shift   = MouseCol % 8;
    addmask = 0xFF00 << (8 - shift);

    masks = MouScreenMask;
    defs = (unsigned short *)CharDefs + ( MouseRow % Points );

    for( i = 0; i < CURSOR_HEIGHT; i++ ) {
        *defs++ &= (*masks++ >> shift) | addmask;
    }

    masks = MouCursorMask;
    defs = (unsigned short *)CharDefs + ( MouseRow % Points );

    for( i = 0; i < CURSOR_HEIGHT; i++ ) {
        *defs++ |= *masks++ >> shift;
    }

    SetWriteMap();                              /* Put characters back      */

    off = 0;
    for( i = 0; i < 2; i++ ) {                  /* The grid is 2 chars high */
        s1 = ( DEFCHAR  + i ) * 32;
        s2 = ( DEFCHAR2 + i ) * 32;
        for( j = 0; j < Points; j++ ) {
            VIDEOData( 0xA000, s2++ ) = CharDefs[off++];
            VIDEOData( 0xA000, s1++ ) = CharDefs[off++];
        }
    }

    ResetSequencer();

    PlotEgaVgaCursor( DRAW );                   /* Plot the new grid        */
}

static bool MouInit( void )
{
    static bool     first_time = true;
    unsigned char   savedmode;
    unsigned short  off;
    unsigned short  i;
    unsigned short  j;
    unsigned short  s1;
    unsigned short  s2;
    unsigned short  ret;

    Points = BIOSData( BDATA_POINT_HEIGHT, unsigned char );

    /*
     * MASSIVE KLUDGE: It turns out that the DOS debugger ends up
     * calling MouInit & MouDeInit every time a screen swap occurs
     * (no matter what the flipping mechanism is). Doing the mouse
     * driver initialization every time is extremely slow. Things seem
     * to work if we only do the driver initialization the first time
     * through. Talk to Brian Stecher/John Dahms if you run into problems
     * with not doing the initialization all the time.
     */
    if( first_time ) {
        first_time = false;
        savedmode = BIOSData( BDATA_CURR_VIDEO_MODE, unsigned char );    /* Save video mode         */
        BIOSData( BDATA_CURR_VIDEO_MODE, unsigned char ) = 6;            /* Set magic mode          */
        ret = _BIOSMouseDriverReset();                                   /* Reset driver for change */
        BIOSData( BDATA_CURR_VIDEO_MODE, unsigned char ) = savedmode;    /* Put the old mode back   */
        if( ret != MOUSE_DRIVER_OK ) {
            return( false );
        }
    }

    SetSequencer();                     /* Program the sequencer    */
    off = 0;
    for( i = 0; i < 2;  i++ ) {
        s1 = ( DEFCHAR + i ) * 32;
        s2 = ( DEFCHAR2 + i ) * 32;
        for( j = 0; j < Points; j++ ) {
            SaveDefs[off++] = VIDEOData( 0xa000, s2++ );
            SaveDefs[off++] = VIDEOData( 0xa000, s1++ );
        }
    }
    ResetSequencer();
    return( true );
}

//  Deinitialize the mouse routines.

static void MouDeinit( void )
{
    unsigned short  i;
    unsigned short  j;
    unsigned short  s1;
    unsigned short  s2;
    unsigned short  off;

    SetSequencer();
    SetWriteMap();                          /* Put characters back      */

    off = 0;
    for( i = 0; i < 2; i++ ) {              /* The grid is 2 chars high */
        s1 = ( DEFCHAR  + i ) * 32;
        s2 = ( DEFCHAR2 + i ) * 32;
        for( j = 0; j < Points; j++ ) {
            VIDEOData( 0xA000, s2++ ) = SaveDefs[off++];
            VIDEOData( 0xA000, s1++ ) = SaveDefs[off++];
        }
    }
    ResetSequencer();
    /* MASSIVE KLUDGE: See comment in MouInit routine
//    _BIOSMouseDriverReset();
    */
}

static void intern EraseEgaVgaCursor( void )
{
    PlotEgaVgaCursor( ERASE );
}

static bool CheckEgaVga( void )
{
    if( ( UIData->colour == M_EGA || UIData->colour == M_VGA )
      && !UIData->desqview
      && !UIData->no_graphics
/*     && UIData->height == 25     */
        ) {
        DrawCursor  = DrawEgaVgaCursor;
        EraseCursor = EraseEgaVgaCursor;
        return( true );
    }
    return( false );
}

bool UIAPI uiinitgmouse( init_mode install )
{
    MouseInstalled = false;
    if( install != INIT_MOUSELESS && mouse_installed() ) {
        if( install == INIT_MOUSE_INITIALIZED ) {
            if( CheckEgaVga() ) {
                if( MouInit() ) {
                    UIData->mouse_yscale = BIOSData( BDATA_POINT_HEIGHT, unsigned char );
                    UIData->mouse_xscale = 8;
                } else {
                    install = INIT_MOUSELESS;
                }
            } else if( _BIOSMouseDriverReset() != MOUSE_DRIVER_OK ) {
                install = INIT_MOUSELESS;
            }
        }
        if( install != INIT_MOUSELESS ) {
            setupmouse();
        }
    }
    return( MouseInstalled );
}

void UIAPI uifinigmouse( void )
{
    if( MouseInstalled && DrawCursor != NULL ) {
        uioffmouse();
        MouDeinit();
    }
}
