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


extern void pascal      _MoveUp19();
extern void pascal      _Move1Left();
extern void pascal      _EGAMoveLeft();
extern void pascal      _MoveLeft19();
extern void pascal      _MoveDown19();
extern void pascal      _Move1Right();
extern void pascal      _EGAMoveRight();
extern void pascal      _MoveRight19();
extern void pascal      _EGAMoveUpHi();
extern void pascal      _EGAMoveDownHi();
extern void pascal      _CoRep();
extern void pascal      _CoXor();
extern void pascal      _CoAnd();
extern void pascal      _CoOr();
extern void pascal      _EGARep();
extern void pascal      _Get1Dot();
extern void pascal      _GetDot19();
extern void             _EGASet();
extern void             _EGAReset();
extern void             _EGASetup80();
extern void pascal      _EGAGetDot();
extern void pascal      _Rep19();
extern void pascal      _And19();
extern void pascal      _Pix1Zap();
extern void pascal      _EGAZap();
extern void pascal      _Zap19();
extern void pascal      _Pix1Fill();
extern void pascal      _EGAFill();
extern void pascal      _Fill19();
extern void pascal      _Pix1Copy();
extern void pascal      _EGAPixCopy();
extern void pascal      _PixCopy19();
extern void pascal      _Pix1Read();
extern void pascal      _EGAReadRow();
extern void pascal      _PixRead19();
extern void pascal      _CGAScanLeft();
extern void pascal      _EGAScanLeft();
extern void pascal      _ScanLeft19();
extern void pascal      _CGAScan1Right();
extern void pascal      _EGAScanRight();
extern void pascal      _ScanRight19();


static short _VGAInit( short mode )
//=================================

{
    if( _SetMode( mode ) == mode ) {
        //             x,   y, col, bpp, pag, seg,     off,     siz,   mis
        if( mode == 17 ) {
            _GrInit( 640, 480,   2,   1,   1, _EgaSeg, _EgaOff,   0,   0 );
        } else if( mode == 18 ) {
            _GrInit( 640, 480,  16,   4,   1, _EgaSeg, _EgaOff,   0,   PLANAR );
        } else {    // mode is 19
            _GrInit( 320, 200, 256,   8,   1, _EgaSeg, _EgaOff,   0,   0 );
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


static void _Setup17( short x, short y, short colour )
/*====================================================

    Calculate screen memory address and associated masks for the
    position (x,y).    */

{
    unsigned int        pixel_offset;
    char                rotate;

    pixel_offset = y * 80 + ( x >> 3 );
    _Screen.mem = MK_FP( _CurrState->screen_seg,
                         _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = x & 7;        // position of pixel in byte
    rotate = _Screen.bit_pos + 1;
    _Screen.mask = ( _rotr_b( ~1, rotate ) << 8 ) + 1;
    _Screen.colour = _rotr_b( colour, rotate );
}


static void _Setup19( short x, short y, short colour )
/*====================================================

    Calculate screen memory address and associated masks for the
    position (x,y).    */

{
    unsigned int        pixel_offset;

    pixel_offset = y * 320 + x;
    _Screen.mem = MK_FP( _CurrState->screen_seg,
                         _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = 0;        // position of pixel in byte
    _Screen.colour = colour;
}


gr_device _FARD         _GrVGA_17 = {
    _VGAInit, _NoOp,
    _NoOp, _NoOp,
    _Setup17,
    _EGAMoveUpHi,_Move1Left,_EGAMoveDownHi,_Move1Right,
    _CoRep,_CoXor,_CoAnd,_CoOr,
    _Get1Dot,_Pix1Zap,_Pix1Fill,_Pix1Copy,_Pix1Read,
    _CGAScanLeft,_CGAScan1Right
};


gr_device _FARD         _GrVGA_18 = {
    _VGAInit, _NoOp,
    _EGASet, _EGAReset,
    _EGASetup80,
    _EGAMoveUpHi,_EGAMoveLeft,_EGAMoveDownHi,_EGAMoveRight,
    _EGARep,_EGARep,_EGARep,_EGARep,
    _EGAGetDot,_EGAZap,_EGAFill,_EGAPixCopy,_EGAReadRow,
    _EGAScanLeft,_EGAScanRight
};


gr_device _FARD         _GrVGA_19 = {
    _VGAInit, _NoOp,
    _NoOp, _NoOp,
    _Setup19,
    _MoveUp19,_MoveLeft19,_MoveDown19,_MoveRight19,
    _Rep19,_CoXor,_And19,_CoOr,
    _GetDot19,_Zap19,_Fill19,_PixCopy19,_PixRead19,
    _ScanLeft19,_ScanRight19
};
