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
#include <i86.h>
#include "gdefn.h"
#include "gbios.h"
#include "rotate.h"
#if defined( __386__ )
#include "extender.h"
#endif


extern void             _NECSet();
extern void             _NECReset();
extern void pascal      _NECRep();
extern void pascal      _NEC1120MoveUp();
extern void pascal      _NEC1120MoveDown();
extern void pascal      _NEC1120MoveLeft();
extern void pascal      _NEC1120MoveRight();
extern void pascal      _NEC1120Xor();
extern void pascal      _NEC1120Or();
extern void pascal      _NEC1120And();
extern void pascal      _NEC1120Zap();
extern void pascal      _NEC1120Fill();
extern void pascal      _NEC1120GetDot();
extern void pascal      _NEC1120Copy();
extern void pascal      _NEC1120Read();
extern void pascal      _NECMoveUp();
extern void pascal      _NECMoveLeft();
extern void pascal      _NECMoveDown();
extern void pascal      _NECMoveRight();
extern void pascal      _NECRep();
extern void pascal      _NECXor();
extern void pascal      _NECAnd();
extern void pascal      _NECOr();
extern void pascal      _NECGetDot();
extern void pascal      _NECPixZap();
extern void pascal      _NECPixFill();
extern void pascal      _NECPixCopy();
extern void pascal      _NECPixRead();
extern short pascal     _NECScanLeft();
extern short pascal     _NECScanRight();
extern void pascal      _NEC16Xor();
extern void pascal      _NEC16And();
extern void pascal      _NEC16Or();
extern void pascal      _NEC16GetDot();
extern void pascal      _NEC16PixZap();
extern void pascal      _NEC16PixFill();
extern void pascal      _NEC16PixCopy();
extern void pascal      _NEC16PixRead();


#define ALL             0xC0
#define COLOUR          0x00


static short            swap_bits[ 16 ] = {      // swap bits 2 and 3
     0,  1,  4,  5,  2,  3,  6,  7,
     8,  9, 12, 13, 10, 11, 14, 15
};


#if defined( __386__ )

extern int              NECInt18( int, int, int, int );
#pragma aux             NECInt18 = \
                        "push    bp    ", \
                        "int     018h  ", \
                        "pop     bp    ", \
                        parm caller [eax] [ebx] [ecx] [edx] \
                        value [eax];

extern int              NECIntDC( int, int, int, int );
#pragma aux             NECIntDC = \
                        "push    bp    ", \
                        "int     0DCh  ", \
                        "pop     bp    ", \
                        parm caller [eax] [ebx] [ecx] [edx] \
                        value [eax];

extern int              NECHiresInt( int, phar_regs far * );
#pragma aux             NECHiresInt = \
                        "push    bp        ", \
                        "mov     ax, 02511h", \
                        "mov     ds, cx    ", \
                        "int     021h      ", \
                        "pop     bp        ", \
                        parm caller [ebx] [cx edx] \
                        value [eax];


static short DoInt( short ax, short bx, short cx, short dx,
                                        short ds, short intr )
//============================================================

{
    RMI                 rmi;
    phar_regs           regs;

    if( _IsRational() ) {
        /* Set up real mode call structure */
        memset( &rmi, 0, sizeof( RMI ) );
        rmi.EAX = ax;
        rmi.EBX = bx;
        rmi.ECX = cx;
        rmi.EDX = dx;
        rmi.DS = ds;

        /* Use DPMI call 300h to issue the DOS interrupt */
        DPMICall( 0x0300, intr, 0, 0, &rmi );
        return( rmi.EAX );
    } else {    // Pharlap
        if( ds != 0 ) {
            /* Issue a Pharlap real-mode interupt call */
            memset( &regs, 0, sizeof( phar_regs ) );
            regs.int_num = intr;
            regs.EAX = ax;
            regs.EDX = dx;
            regs.DS = ds;
            return( NECHiresInt( bx, &regs ) );
        } else {
            if( intr == 0x0018 ) {
                return( NECInt18( ax, bx, cx, dx ) );
            } else {
                return( NECIntDC( ax, bx, cx, dx ) );
            }
        }
    }
}


short NECVideoInt( short ax, short bx, short cx, short dx )
//=========================================================

{
    return( DoInt( ax, bx, cx, dx, 0x0000, 0x0018 ) );
}


short NECVideoIntDC( short ax, short bx, short cx, short dx )
//===========================================================

{
    return( DoInt( ax, bx, cx, dx, 0x0000, 0x00DC ) );
}


short NECHiresVideoInt( short ax, short ds, short bx, short dx )
//==============================================================

{
    return( DoInt( ax, bx, 0, dx, ds, 0x0018 ) );
}

#endif


short _SwapBits( short val )
//==========================

{
    return( swap_bits[ val ] );
}


static short _NECSetMode( short mode )
//====================================

{
    short               display;
    short               colour_mode;
    char far            *graph_on;

    display = NECVideoInt( _BIOS_SENSE_MODE, 0, 0, 0 );
    if( ( display & 0x80 ) == 0 ) {     // dedicated high-resolution display
        return( 0 );                    // required for 640x400
    }
    switch( mode ) {
    case _98HIRES16COLOR:
    case _98HIRESS16COLOR:
        if( _GRCGPort == 0x7C ) {       // machine is low res
            return( 0 );
        }

        // these inp's of 0xa0 may not be neccessary
        while( !(inp( 0xa0 ) & 0x04) ); // wait until fifo is empty
        while( inp( 0xa0 ) & 0x08 );    // wait until not busy drawing
        while( inp( 0xa0 ) & 0x20 );    // wait for vsync to go inactive
        while( !(inp( 0xa0 ) & 0x20) ); // wait for vsync to go active

        outp( 0xa2, 0x0d );
        graph_on = (char far *) _BIOS_data( GRAPH_ON_FLAG );
        *graph_on |= 0x80;
        outp( 0x6a, 1 );        // enable 16 colour mode (4096 colour)
        break;

    case _98RESS16COLOR:
    case _98RES16COLOR:
        // check for 16 colour option
        if( ( inp( 0x42 ) & 0x08 ) != 0 ) {
            return( 0 );
        }
    case _98RESS8COLOR:
    case _98RES8COLOR:
        if( _GRCGPort == 0xA4 ) {       // machine is in hires mode
            return( 0 );
        }
        NECVideoInt( _BIOS_GRAPH_START, 0, 0, 0 );   // display graphics screen
        outp( 0x6a, 1 );        // enable 16 colour mode (4096 colour)
        break;
    case _98RESSCOLOR:
    case _98RESCOLOR:
        if( _GRCGPort == 0xA4 ) {       // machine is in hires mode
            return( 0 );
        }
        NECVideoInt( _BIOS_GRAPH_START, 0, 0, 0 );   // display graphics screen
        outp( 0x6a, 0 );        // disable 16 colour mode
    }

    if( mode & SUPERIMPOSED ) {
        NECVideoInt( _BIOS_TEXT_START, 0, 0, 0 );// display text screen
    } else {
        NECVideoInt( _BIOS_TEXT_STOP, 0, 0, 0 ); // stop display of text
    }
    colour_mode = ALL + COLOUR;
    NECVideoInt( _BIOS_GRAPH_SET, 0, colour_mode << 8, 0 ); // set screen

    return( 1 );
}


static short _NECInit( short mode )
//=================================

{
    if( _NECSetMode( mode ) ) {
        switch( mode ) {
        case _98RESCOLOR:
        case _98RESSCOLOR:
        case _98RES8COLOR:
        case _98RESS8COLOR:
//                      x,   y, col, bpp, pag,     seg,     off, siz,    mis
            _GrInit(  640, 400,   8,   3,   1, _NecSeg, _NecOff,   0, PLANAR );
            break;
        case _98RES16COLOR:
        case _98RESS16COLOR:
            _GrInit(  640, 400,  16,   4,   1, _NecSeg, _NecOff,   0, PLANAR );
            break;
        case _98HIRES16COLOR:
        case _98HIRESS16COLOR:
            _GrInit( 1120, 750,  16,   4,   1, _NecSeg, _NecOff,   0, PLANAR );
            break;
        }
    } else {
        return( FALSE );
    }

    _CurrState->vc.mode = mode;     // _GrInit fills in bios_mode
    _GrClear( 0, 0, _CurrState->vc.numxpixels - 1, _CurrState->vc.numypixels - 1 );
    if( mode & SUPERIMPOSED ) {
        _CharAttr = _DEFAULT_ATTR;
        _TxtClear( 0, 0, _CurrState->vc.numtextrows - 1, _CurrState->vc.numtextcols - 1 );
        _setvisualpage( 0 );
        _setactivepage( 0 );
        NECVideoIntDC( 0x0300, 0, 0x0010, 0 );      // tell DOS where cursor is
        NECVideoInt( _BIOS_CURSOR_SET, 0, 0, 0 );
        NECVideoInt( _BIOS_CURSOR_START, 0, 0, 0 );      // display cursor
    }
    _remapallpalette( _NECDefPalette );
    return( TRUE );
}


static void _NECFini()
//====================

{
    char far            *graph_on;

    _GrClear( 0, 0, _CurrState->vc.numxpixels - 1, _CurrState->vc.numypixels - 1 );
    _remapallpalette( _NECDefPalette );
    if( _GRCGPort == 0xa4 ) {
        // these inp's of 0xa0 may not be neccessary
        while( !(inp( 0xa0 ) & 0x04) ); // wait until fifo is empty
        while( inp( 0xa0 ) & 0x08 );    // wait until not busy drawing
        while( inp( 0xa0 ) & 0x20 );    // wait for vsync to go inactive
        while( !(inp( 0xa0 ) & 0x20) ); // wait for vsync to go active

        outp( 0xa2, 0x05 );             // stop graphics
        graph_on = (char far *) _BIOS_data( GRAPH_ON_FLAG );
        *graph_on &= 0x7F;
    } else {
        NECVideoInt( _BIOS_GRAPH_STOP, 0, 0, 0 );            // stop graphics
    }

    if( _CurrState->vc.mode & SUPERIMPOSED ) {
        _CharAttr = _DEFAULT_ATTR;
        _TxtClear( 0, 0, _CurrState->vc.numtextrows - 1, _CurrState->vc.numtextcols - 1 );
    }
    if( _GRCGPort != 0xa4 ) {
        outp( 0x6a, 0 );    // disable 16 colour mode
    }
}


static short _NECTextInit( short mode )
//=====================================

{
    mode = mode; // rid ourselves of the warning
    if( _GRCGPort != 0xa4 ) {
        outp( 0x6a, 0 );    // disable 16 colour mode
    }
    NECVideoInt( _BIOS_TEXT_START, 0, 0, 0 );            // display text scrn
    _GetState();
    _CharAttr = _DEFAULT_ATTR;
    _TxtClear( 0, 0, _CurrState->vc.numtextrows - 1, _CurrState->vc.numtextcols - 1 );
    _setvisualpage( 0 );
    _setactivepage( 0 );
    NECVideoInt( _BIOS_CURSOR_START, 0, 0, 0 );          // display cursor
    return( TRUE );
}

static void _NECTextFini()
//========================

{
    _CharAttr = _DEFAULT_ATTR;
    _TxtClear( 0, 0, _CurrState->vc.numtextrows - 1, _CurrState->vc.numtextcols - 1 );
    _setvisualpage( 0 );
    _setactivepage( 0 );
    NECVideoInt( _BIOS_CURSOR_START, 0, 0, 0 );          // display cursor
}

static void _NoOp( void )
/*=======================

    Dummy function that does nothing.   */

{
}

static void _NECSetup( short x, short y, short colour )
/*=====================================================

    Calculate screen memory address and associated masks for the
    position (x,y).    */

{
    unsigned short      pixel_offset;
    char                rotate;
    char                mask;

    pixel_offset =  y * 80 + ( x >> 3 );
    _Screen.mem = MK_FP( _CurrState->screen_seg,
                         _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = x & 7;            // position of pixel in byte
    rotate = _Screen.bit_pos + 1;
    mask = _rotr_b( ~1, rotate );
    _Screen.mask = ( mask << 8 ) | ( (char) ~mask ); // place complement of mask in CL
    _Screen.colour = colour;
}

static void _NEC1120Setup( short x, short y, short colour )
/*=========================================================

    Calculate screen memory address and associated masks for the
    position (x,y), in hires 1120 x 750 x 16 color mode. */

{
    unsigned long       pixel_offset;
    unsigned long       seg_index;
    char                rotate;
    char                mask;

    pixel_offset = (long) y * 140 + ( x >> 3 );

#ifdef __386__
    seg_index = seg_index;      // unused

    _Screen.mem = MK_FP( _CurrState->screen_seg,
                         _CurrState->screen_off + pixel_offset );
#else
    seg_index = (pixel_offset & 0x00018000L) >> 4;
    pixel_offset &= 0x7fff;
    _Screen.mem = MK_FP( _CurrState->screen_seg + seg_index, pixel_offset );
#endif

    _Screen.bit_pos = x & 7;            // position of pixel in byte
    rotate = _Screen.bit_pos + 1;
    mask = _rotr_b( ~1, rotate );
    _Screen.mask = ( mask << 8 ) | ( (char) ~mask ); // place complement of mask in CL
    _Screen.colour = colour;
}

gr_device _FARD         _GrNEC_TEXT = {     // for text modes
    _NECTextInit, _NECTextFini,
    _NoOp, _NoOp                // rest of routines will never be called
};

gr_device _FARD         _GrNEC_GRAPH8 = {
    _NECInit, _NECFini,
    _NECSet, _NECReset,
    _NECSetup,
    _NECMoveUp, _NECMoveLeft, _NECMoveDown, _NECMoveRight,
    _NECRep,_NECXor,_NECAnd,_NECOr,
    _NECGetDot,_NECPixZap,_NECPixFill,_NECPixCopy,_NECPixRead,
    _NECScanLeft,_NECScanRight
};

gr_device _FARD         _GrNEC_GRAPH16 = {
    _NECInit, _NECFini,
    _NECSet, _NECReset,
    _NECSetup,
    _NECMoveUp, _NECMoveLeft, _NECMoveDown, _NECMoveRight,
    _NECRep,_NEC16Xor,_NEC16And,_NEC16Or,
    _NEC16GetDot,_NEC16PixZap,_NEC16PixFill,_NEC16PixCopy,_NEC16PixRead,
    _NECScanLeft,_NECScanRight
};

gr_device _FARD         _GrNEC_GRAPH1120 = {
    _NECInit, _NECFini,
    _NECSet, _NECReset,
    _NEC1120Setup,
    _NEC1120MoveUp, _NEC1120MoveLeft, _NEC1120MoveDown, _NEC1120MoveRight,
    _NECRep,_NEC1120Xor,_NEC1120And,_NEC1120Or,
    _NEC1120GetDot,_NEC1120Zap,_NEC1120Fill,_NEC1120Copy,_NEC1120Read,
    _NECScanLeft,_NECScanRight
};
