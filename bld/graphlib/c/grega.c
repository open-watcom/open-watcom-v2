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
* Description:  Setup for standard EGA modes.
*
****************************************************************************/


#include <conio.h>
#include "gdefn.h"
#include "gbios.h"


#define SEQADDR         0x03C4
#define GRADDR          0x03CE

/*      EGA sequence registers  */

#define MAPMASK         0x02

/*      EGA control registers   */

#define SRREG           0x00
#define ESRREG          0x01
#define CMPREG          0x02
#define ACTN            0x03
#define RMSEL           0x04
#define MODEREG         0x05
#define DONTCARE        0x07
#define MASK            0x08

/*      EGA mode values     */

#define WRITE_MODE_0    0x00
#define WRITE_MODE_1    0x01
#define WRITE_MODE_2    0x02
#define READ_MODE_0     0x00
#define READ_MODE_1     0x08
#define EVEN_ODD_MODE   0x10


static char             ActnTab[4] = {
    0x00,                   /* replace  */
    0x18,                   /* xor      */
    0x08,                   /* and      */
    0x10                    /* of       */
};


static char             MonoTab[4] = {
    0x00,                   /* black    */
    0x03,                   /* video    */
    0x0C,                   /* blink    */
    0x0F                    /* bright   */
};


gr_device _FARD         _GrEGA_EO;      // forward reference


static short _EGAInit( short mode )
/*=================================

    Initialize the video mode using BIOS interrupt 0x10. If the adapter
    has only 64K memory, then we are in an even odd mode.   */

{
    if( _SetMode( mode ) == mode ) {
        //             x,   y, str, col, bpp, pag, seg,     off,     siz,   mis
        if( mode == 13 ) {
            _GrInit( 320, 200,  40,  16,   4,   8, _EgaSeg, _EgaOff, 0x200, PLANAR );
        } else if( mode == 14 ) {
            _GrInit( 640, 200,  80,  16,   4,   4, _EgaSeg, _EgaOff, 0x400, PLANAR );
        } else if( mode == 15 ) {
            _GrInit( 640, 350,  40,   4,   2,   2, _EgaSeg, _EgaOff, 0x800, PLANAR );
        } else {    // mode is 16
            _GrInit( 640, 350,  80,  16,   4,   2, _EgaSeg, _EgaOff, 0x800, PLANAR );
        }
        if( ( EGA_Memory() & 0x00ff ) == 0 ) {      // only 64K memory
            _CurrState->vc.numvideopages = max( 1, _CurrState->vc.numvideopages / 4 );
            if( mode == 15 || mode == 16 ) {
                _CurrState->deviceptr = &_GrEGA_EO;
                _CurrState->vc.numcolors = 4;
                _CurrState->vc.bitsperpixel = 2;
            }
        }
        outpw( SEQADDR, 0x0F00 + MAPMASK );         // set map mask register
        return( TRUE );                             // to all planes
    }
    return( FALSE );
}


static void _NoOp( void )
/*=======================

    Dummy function that does nothing.   */

{
}


void _EGASet( void )
/*==================

    Initialize graphics registers. */

{
    outpw( GRADDR, ( ( READ_MODE_0 + WRITE_MODE_2 ) << 8 ) + MODEREG );
    outpw( GRADDR, ( ActnTab[ _PlotAct ] << 8 ) + ACTN );
//  outpw( GRADDR, 0x0F00 + DONTCARE );
}


void _EGASetEO( void )
/*====================

    Initialize graphics registers. */

{
    outpw( GRADDR, ( ( EVEN_ODD_MODE + READ_MODE_0 + WRITE_MODE_2 ) << 8 ) + MODEREG );
    outpw( GRADDR, ( ActnTab[ _PlotAct ] << 8 ) + ACTN );
//  outpw( GRADDR, 0x0F00 + DONTCARE );
}


void _EGAReset( void )
/*====================

    Terminate the current EGA video mode.   */

{
    outpw( GRADDR, ( (WRITE_MODE_0+READ_MODE_0) << 8 ) + MODEREG );
    outpw( GRADDR, 0xFF00 + MASK );                     /* unmask all bits  */
    outpw( GRADDR, 0x0F00 + DONTCARE );                 /* open all planes  */
    outpw( GRADDR, ACTN );                              /* replace action   */
}


static void _EGAResetEO( void )
/*=============================

    Terminate the current EGA even/odd video mode.   */

{
    outpw( GRADDR, ( (WRITE_MODE_0+READ_MODE_0+EVEN_ODD_MODE) << 8 ) + MODEREG );
    outpw( GRADDR, 0xFF00 + MASK );                     /* unmask all bits  */
    outpw( GRADDR, 0x0F00 + DONTCARE );                 /* open all planes  */
    outpw( GRADDR, ACTN );                              /* replace action   */
}


static void _EGASetup40( short x, short y, short colour )
/*=======================================================

    Calculate screen memory address and associated masks for the
    position (x,y).    */

{
    unsigned short      pixel_offset;

    pixel_offset = y * 40 + ( x >> 3 );
    _Screen.mem = MK_FP( _CurrState->screen_seg,
                         _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = x & 7;            // position of pixel in byte
    _Screen.mask = ( 0x80 >> _Screen.bit_pos ) << 8;
    _Screen.colour = colour;
}


void _EGASetup80( short x, short y, short colour )
/*================================================

    Calculate screen memory address and associated masks for the
    position (x,y).    */

{
    unsigned short      pixel_offset;

    pixel_offset = y * 80 + ( x >> 3 );
    _Screen.mem = MK_FP( _CurrState->screen_seg,
                         _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = x & 7;            // position of pixel in byte
    _Screen.mask = ( 0x80 >> _Screen.bit_pos ) << 8;
    _Screen.colour = colour;
}


static void _EGASetupMono( short x, short y, short colour )
/*=========================================================

    Calculate screen memory address and associated masks for the
    position (x,y).    */

{
    unsigned short      pixel_offset;

    pixel_offset = y * 80 + ( x >> 3 );
    _Screen.mem = MK_FP( _CurrState->screen_seg,
                         _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = x & 7;            // position of pixel in byte
    _Screen.mask = ( 0x80 >> _Screen.bit_pos ) << 8;
    _Screen.colour = MonoTab[ colour ];
}


gr_device _FARD         _GrEGA_13 = {
    _EGAInit, _NoOp,
    _EGASet, _EGAReset,
    _EGASetup40,
    _EGAMoveUpLo,_EGAMoveLeft,_EGAMoveDownLo,_EGAMoveRight,
    _EGARep,_EGARep,_EGARep,_EGARep,
    _EGAGetDot,_EGAZap,_EGAFill,_EGAPixCopy,_EGAReadRow,
    _EGAScanLeft,_EGAScanRight
};

gr_device _FARD         _GrEGA_14 = {
    _EGAInit, _NoOp,
    _EGASet, _EGAReset,
    _EGASetup80,
    _EGAMoveUpHi,_EGAMoveLeft,_EGAMoveDownHi,_EGAMoveRight,
    _EGARep,_EGARep,_EGARep,_EGARep,
    _EGAGetDot,_EGAZap,_EGAFill,_EGAPixCopy,_EGAReadRow,
    _EGAScanLeft,_EGAScanRight
};

gr_device _FARD         _GrEGA_15 = {
    _EGAInit, _NoOp,
    _EGASet, _EGAReset,
    _EGASetupMono,
    _EGAMoveUpHi,_EGAMoveLeft,_EGAMoveDownHi,_EGAMoveRight,
    _EGARep,_EGARep,_EGARep,_EGARep,
    _EGAGetDotMono,_EGAZapMono,_EGAFillMono,_EGAPixCopyMono,_EGAReadRowMono,
    _EGAScanLeftMono,_EGAScanRightMono
};

gr_device _FARD         _GrEGA_16 = {
    _EGAInit, _NoOp,
    _EGASet, _EGAReset,
    _EGASetup80,
    _EGAMoveUpHi,_EGAMoveLeft,_EGAMoveDownHi,_EGAMoveRight,
    _EGARep,_EGARep,_EGARep,_EGARep,
    _EGAGetDot,_EGAZap,_EGAFill,_EGAPixCopy,_EGAReadRow,
    _EGAScanLeft,_EGAScanRight
};

gr_device _FARD         _GrEGA_EO = {
    (short (*)(short))_NoOp, _NoOp,     // init routine is never called
    _EGASetEO, _EGAResetEO,
    _EGASetupMono,
    _EGAMoveUpHi,_EGAMoveLeft,_EGAMoveDownHi,_EGAMoveRight,
    _EGARep,_EGARep,_EGARep,_EGARep,
    _EGAGetDotEO,_EGAZapEO,_EGAFillEO,_EGAPixCopyEO,_EGAReadRowEO,
    _EGAScanLeftEO,_EGAScanRightEO
};
