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
#include "gdefn.h"
#include "rotate.h"
#include "montypes.h"
#include "gbios.h"


#define CRT_INDEX       0x03B4      /* 6845 Index register  */
#define CRT_CNTRL       0x03B8      /* Display mode control port    */
#define CRT_CONFIG      0x03BF      /* Hercules configuration port  */
#define HERC_HALF       0x0001      /* half configuration   */


extern void pascal      _HercMoveUp();
extern void pascal      _HercMoveDown();
extern void pascal      _Move1Left();
extern void pascal      _Move1Right();
extern void pascal      _CoRep();
extern void pascal      _CoXor();
extern void pascal      _CoAnd();
extern void pascal      _CoOr();
extern void pascal      _Get1Dot();
extern void pascal      _Pix1Zap();
extern void pascal      _Pix1Fill();
extern void pascal      _Pix1Copy();
extern void pascal      _Pix1Read();
extern void pascal      _CGAScanLeft();
extern void pascal      _CGAScan1Right();


/*  Table of values for CRT registers. Use 0x57 for the two 0x58 in the
    table to get 348 scan lines visible on the screen instead of 350.   */

static char             GTable[ 12 ] = {
    0x35, 0x2D, 0x2E, 0x07,
    0x5B, 0x02, 0x58, 0x58,
    0x02, 0x03, 0x00, 0x00
};


static void GraphicsMode()
/*========================

    Switch into Hercules graphics mode. */

{
    short           i;

    *(char far *)_BIOS_data( EQUIP_FLAGS ) |= 0x30; // turn on monochrome bits
    outp( CRT_CONFIG, HERC_HALF );                  // enable half support
    outp( CRT_CNTRL, 2 );                           // set graphics mode (with screen off)
    for( i = 0; i < 12; i++ ) {                     // load CRT registers
        outpw( CRT_INDEX, ( GTable[i] << 8 ) + i );
    }
    _fmemset( MK_FP( _MonoSeg, _MonoOff ), 0, 0x8000 );   // clear screen
    outp( CRT_CNTRL, 0x0A );                        // turn screen back on

    *(char far *)_BIOS_data( CRT_MODE ) = _HERCMONO;// set mode
    *(char far *)_BIOS_data( CRT_COLS ) = 90;       // set # of columns to 90
    *(char far *)_BIOS_data( ROWS ) = 25 - 1;       // set # of rows to 25
    *(short far *)_BIOS_data( CURSOR_POSN ) = 0;    // set cursor to (0,0)
}


static short _HercInit( short mode )
/*==================================

    Initialize the Hercules graphics video mode. */

{
    short               monitor;
    short               alternate;

    mode = mode;
    monitor = _SysMonType();
    alternate = monitor >> 8;       // separate active/alternate adapters
    monitor &= 0xff;
    if( ( monitor >= MT_HERC && monitor <= MT_HERCINCL ) ||
        ( alternate >= MT_HERC && alternate <= MT_HERCINCL ) ) {
        GraphicsMode();
        //         x,   y, col, bpp, pag, seg,      off,      siz, mis
        _GrInit( 720, 350,   2,   1,   1, _MonoSeg, _MonoOff,   0, NO_BIOS );
        return( TRUE );
    } else {
        return( FALSE );
    }
}


static void _NoOp( void )
/*=======================

    Dummy function that does nothing.   */

{
}


static void _HercSetup( short x, short y, short colour )
/*======================================================

    Calculate screen memory address and associated masks for the
    position (x,y).    */

{
    unsigned short      pixel_offset;
    char                rotate;

    pixel_offset = _wror( y & 3, 3 ) + 90 * ( y >> 2 ) + ( x >> 3 );
    _Screen.mem = MK_FP( _CurrState->screen_seg,
                         _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = x & 7;            // position of pixel in byte
    rotate = _Screen.bit_pos + 1;
    _Screen.mask = ( _rotr_b( ~1, rotate ) << 8 ) | 1;
    _Screen.colour = _rotr_b( colour, rotate );
}


gr_device _FARD         _GrHGC_11 = {
    _HercInit, _NoOp,
    _NoOp, _NoOp,
    _HercSetup,
    _HercMoveUp,_Move1Left,_HercMoveDown,_Move1Right,
    _CoRep,_CoXor,_CoAnd,_CoOr,
    _Get1Dot,_Pix1Zap,_Pix1Fill,_Pix1Copy,_Pix1Read,
    _CGAScanLeft,_CGAScan1Right
};
