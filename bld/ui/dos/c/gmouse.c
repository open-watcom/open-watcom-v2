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


#include <dos.h>
#include <stdlib.h>

#include "uidef.h"
#include "charmap.h"
#include "biosui.h"

#define  CURSOR_HEIGHT 14                       /* Mouse cursor height      */

#define  DEFCHAR    0xD5
#define  DEFCHAR2   0xD7

#define  VidCol     (UIData->width)
#define  VidRow     (UIData->height)

extern   MOUSEORD   MouseRow;
extern   MOUSEORD   MouseCol;

extern   void (intern *DrawCursor)(void);
extern   void (intern *EraseCursor)(void);
extern   int  MouseInstalled;

static char             SaveChars[2][2];        /* Overwritten characters  */
extern unsigned short   Points;                 /* Number of lines / char  */
static unsigned char    CharDefs[64];           /* Character definitons.    */
static unsigned char    SaveDefs[64];           /* Saved character defs     */

enum   Function    { ERASE, DRAW, SAVE };

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

static void intern PlotEgaVgaCursor( unsigned );
static char intern MouInit( void );
static void intern MouDeinit( void );
static char intern CheckEgaVga( void );

void intern DrawEgaVgaCursor(void);
void intern EraseEgaVgaCursor( void );

//  Plot the cursor on the screen, save background, draw grid, etc.

static void intern PlotEgaVgaCursor( unsigned action )
{
    unsigned width, height, disp, i, j, x, y;
    static int lsavex = 0, lsavey = 0;
    LPPIXEL screen;

    switch( action ) {
        case ERASE :                        /* Erase grid, put save info    */
            x = lsavex;
            y = lsavey;
            break;
        case DRAW :                         /* Draw grid                    */
            x = MouseCol/8;
            y = MouseRow/Points;
            break;
        case SAVE :                         /* Save grid                    */
            x = lsavex = MouseCol/8;
            y = lsavey = MouseRow/Points;
            break;
    }

    width = VidCol - x;
    if( width > 2 ) {
        width = 2;
    }

    height = VidRow - y;
    if( height > 2 ) {
        height = 2;
    }

    screen = UIData->screen.origin;
    screen += ( y * VidCol + x );
    disp = ( VidCol - width );

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

void intern DrawEgaVgaCursor(void)
{
    unsigned short off,  shift, addmask, i, j, s1, s2;
    unsigned short *defs, *masks;

    PlotEgaVgaCursor(SAVE);                     /* Save the current grid    */

    SetSequencer();                             /* Program the sequencer    */
    off = 0;
    for( i = 0; i < 4;  i += 2 ) {              /* The grid is 2 chars high */
        s1 = ( (char *) SaveChars )[i    ] * 32;
        s2 = ( (char *) SaveChars )[i + 1] * 32;
        for( j = 0; j < Points; j++ ) {
            CharDefs[off++] = _peekb( 0xa000, s2++ );
            CharDefs[off++] = _peekb( 0xa000, s1++ );
        }
    }

    shift   = MouseCol % 8;
    addmask = 0xFF00 << (8 - shift);

    masks   = MouScreenMask;
    defs    = ( (unsigned short *) CharDefs ) + MouseRow % Points;

    for( i = 0; i < CURSOR_HEIGHT; i++ ) {
        *defs++ &= (*masks++ >> shift) | addmask;
    }

    masks   = MouCursorMask;
    defs    = ( (unsigned short *) CharDefs ) + MouseRow % Points;

    for( i = 0; i < CURSOR_HEIGHT; i++ ) {
        *defs++ |= *masks++ >> shift;
    }

    SetWriteMap();                              /* Put characters back      */

    off = 0;
    for( i = 0; i < 2; i++ ) {                  /* The grid is 2 chars high */
        s1 = ( DEFCHAR  + i ) * 32;
        s2 = ( DEFCHAR2 + i ) * 32;
        for( j = 0; j < Points; j++ ) {
            _pokeb( 0xA000, s2++, CharDefs[off++] );
            _pokeb( 0xA000, s1++, CharDefs[off++] );
        }
    }

    ResetSequencer();

    PlotEgaVgaCursor(DRAW);                     /* Plot the new grid        */
}

static char intern MouInit( void )
{
    char  savedmode;
    unsigned short  off, i, j, s1, s2;
    int   ret;
    static int first_time = TRUE;

    Points = _POINTS;

    /*
        MASSIVE KLUDGE: It turns out that the DOS debugger ends up
        calling MouInit & MouDeInit every time a screen swap occurs
        (no matter what the flipping mechanism is). Doing the mouse
        driver initialization every time is extremely slow. Things seem
        to work if we only do the driver initialization the first time
        through. Talk to Brian Stecher/John Dahms if you run into problems
        with not doing the initialization all the time.
    */
    if( first_time ) {
        first_time = FALSE;
        savedmode = _peekb( BIOS_PAGE, 0x49 );    /* Save video mode      */
        _pokeb( BIOS_PAGE, 0x49, 6);           /* Set magic mode           */

        ret = MouseInt( 0, 0, 0, 0 );    /* Reset driver for change  */

        _pokeb( BIOS_PAGE, 0x49, savedmode);       /* Put the old mode back    */

        if( ret != -1 ) {
            return( FALSE );
        }
    }

    SetSequencer();                     /* Program the sequencer    */
    off = 0;
    for( i = 0; i < 2;  i++ ) {
        s1 = ( DEFCHAR + i ) * 32;
        s2 = ( DEFCHAR2 + i ) * 32;
        for( j = 0; j < Points; j++ ) {
            SaveDefs[off++] = _peekb( 0xa000, s2++ );
            SaveDefs[off++] = _peekb( 0xa000, s1++ );
        }
    }
    ResetSequencer();
    return( TRUE );
}

//  Deinitialize the mouse routines.

static void intern MouDeinit( void )
{
    unsigned short    i, j, s1, s2, off ;

    SetSequencer();
    SetWriteMap();                          /* Put characters back      */

    off = 0;
    for( i = 0; i < 2; i++ ) {              /* The grid is 2 chars high */
        s1 = (DEFCHAR  + i ) * 32;
        s2 = (DEFCHAR2 + i ) * 32;
        for( j = 0; j < Points; j++ ) {
            _pokeb( 0xA000, s2++, SaveDefs[off++] );
            _pokeb( 0xA000, s1++, SaveDefs[off++] );
        }
    }
    ResetSequencer();
    /* MASSIVE KLUDGE: See comment in MouInit routine
//    MouseInt( 0, 0, 0, 0 );
    */
}

void intern EraseEgaVgaCursor( void )
{
    PlotEgaVgaCursor( ERASE );
}

void global uifinigmouse( void )
{
    if( MouseInstalled && DrawCursor!=NULL ) {
        uioffmouse();
        MouDeinit();
    }
}

bool global uiinitgmouse( register int install )
{
    MouseInstalled = FALSE;
    if( install > 0 && installed( BIOS_MOUSE ) ) {
        if( install > 1 ) {
            if( CheckEgaVga() ) {
                if( MouInit() ) {
                    UIData->mouse_yscale = _POINTS;
                    UIData->mouse_xscale = 8;
                } else {
                    install = 0;
                }
            } else if( MouseInt( 0, 0, 0, 0 ) != -1 ) {
                install = 0;
            }
        }
        if( install > 0 ) {
            setupmouse();
        }
    }
    return( MouseInstalled );
}

static char intern CheckEgaVga()
{
    if( ( UIData->colour == M_EGA || UIData->colour == M_VGA )
        && !UIData->desqview
        && !UIData->no_graphics
/*      && UIData->height == 25     */
        ) {
            DrawCursor  = DrawEgaVgaCursor;
            EraseCursor = EraseEgaVgaCursor;
            return( TRUE );
    }
    return( FALSE );
}
