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


#include "gdefn.h"
#include "gbios.h"
#include "rotate.h"


extern void pascal            _MoveUp();
extern void pascal            _Move1Left();
extern void pascal            _Move2Left();
extern void pascal            _MoveDown();
extern void pascal            _Move1Right();
extern void pascal            _Move2Right();
extern void pascal            _CoRep();
extern void pascal            _CoXor();
extern void pascal            _CoAnd();
extern void pascal            _CoOr();
extern void pascal            _Get1Dot();
extern void pascal            _Get2Dot();
extern void pascal            _Pix1Zap();
extern void pascal            _Pix2Zap();
extern void pascal            _Pix1Fill();
extern void pascal            _Pix2Fill();
extern void pascal            _Pix1Copy();
extern void pascal            _Pix2Copy();
extern void pascal            _Pix1Read();
extern void pascal            _Pix2Read();
extern short pascal           _CGAScanLeft();
extern short pascal           _CGAScan1Right();
extern short pascal           _CGAScan2Right();


static short _CGAInit( short mode )
//=================================

{
    if( _SetMode( mode ) == mode ) {
        //             x,   y, col, bpp, pag, seg,     off,     siz, mis
        if( mode == 6 ) {
            _GrInit( 640, 200,   2,   1,   1, _CgaSeg, _CgaOff,   0,   0 );
        } else {    // modes 4 and 5
            _GrInit( 320, 200,   4,   2,   1, _CgaSeg, _CgaOff,   0,   0 );
        }
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


static void _Setup6( short x, short y, short colour )
/*===================================================

    Calculate screen memory address and associated masks for the
    position (x,y).    */

{
    unsigned short      pixel_offset;
    char                rotate;

    pixel_offset = ( y >> 1 ) * 80 + ( x >> 3 );
    if( y & 1 ) {
        pixel_offset += 0x2000;         // if odd row
    }
    _Screen.mem = MK_FP( _CurrState->screen_seg,
                         _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = x & 7;            // position of pixel in byte
    rotate = _Screen.bit_pos + 1;
    _Screen.mask = ( _rotr_b( ~1, rotate ) << 8 ) | 1;
    _Screen.colour = _rotr_b( colour, rotate );
}


static void _Setup4( short x, short y, short colour )
/*===================================================

    Calculate screen memory address and associated masks for the
    position (x,y).    */

{
    unsigned short      pixel_offset;
    char                rotate;

    pixel_offset = ( y >> 1 ) * 80 + ( x >> 2 );
    if( y & 1 ) {
        pixel_offset += 0x2000;         // if odd row
    }
    _Screen.mem = MK_FP( _CurrState->screen_seg,
                         _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = ( x & 3 ) << 1;   // position of pixel in byte
    rotate = _Screen.bit_pos + 2;
    _Screen.mask = ( _rotr_b( ~3, rotate ) << 8 ) + 2;
    _Screen.colour = _rotr_b( colour, rotate );
}


static short _TextInit( short mode )
//==================================

{
    if( _SetMode( mode ) == mode ) {
        _GetState();
        return( TRUE );
    } else {
        return( FALSE );
    }
}


gr_device _FARD         _GrCGA_4 = {
    _CGAInit, _NoOp,
    _NoOp, _NoOp,
    _Setup4,
    _MoveUp, _Move2Left, _MoveDown, _Move2Right,
    _CoRep,_CoXor,_CoAnd,_CoOr,
    _Get2Dot,_Pix2Zap,_Pix2Fill,_Pix2Copy,_Pix2Read,
    _CGAScanLeft,_CGAScan2Right
};


gr_device _FARD         _GrCGA_6 = {
    _CGAInit, _NoOp,
    _NoOp, _NoOp,
    _Setup6,
    _MoveUp,_Move1Left,_MoveDown,_Move1Right,
    _CoRep,_CoXor,_CoAnd,_CoOr,
    _Get1Dot,_Pix1Zap,_Pix1Fill,_Pix1Copy,_Pix1Read,
    _CGAScanLeft,_CGAScan1Right
};

gr_device _FARD         _TextDevice = {     // for text modes
    _TextInit, _NoOp,
    _NoOp, _NoOp                // rest of routines will never be called
};
