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
* Description:  Setup for SuperVGA and VESA modes.
*
****************************************************************************/


#include <conio.h>
#include "gdefn.h"
#include "gbios.h"
#include "svgadef.h"
#if defined( __386__ )
  #include "rmalloc.h"
#endif


extern void             _EGASet( void );
extern void             _EGAReset( void );


/*  Use PASCAL pragma to define our convention for
    calling the SVGA page number routine.    */

#define PAGE_FUNC pascal        // make sure page # gets passed in ax

#if defined ( __386__ )
    #pragma aux pascal "*" parm caller [eax];
#else
    #pragma aux pascal "*" far parm caller [ax];
#endif


static short            _SuperVGAModes[ _SV_MAX-2 ][ 6 ] = {
//                      100h    101h    102h    103h    104h    105h
//                      ====    ====    ====    ====    ====    ====
/* _SV_VIDEO7 */        0x66,   0x67,   0x62,   0x69,   0x65,   0x6a,
/* _SV_PARADISE */      0x5e,   0x5f,   0x58,   0x5c,   0x5d,   0,
/* _SV_ATI */           0x61,   0x62,   0x54,   0x63,   0x55,   0x64,
/* _SV_TSENG3000 */        0,   0x2e,   0x29,   0x30,   0x37,   0,
/* _SV_TSENG4000 */     0x2f,   0x2e,   0x29,   0x30,   0x37,   0x38,
/* _SV_OAK */              0,   0x53,   0x52,   0x54,   0x56,   0x59,
/* _SV_TRIDENT */       0x5c,   0x5d,   0x5b,   0x5e,   0x5f,   0x62,
/* _SV_CHIPS */         0x78,   0x79,   0x6a,   0x7c,   0x72,   0x7e,
/* _SV_GENOA */         0x7e,   0x5c,   0x79,   0x5e,   0x5f,   0,
/* _SV_S3 */               0,   0x101,  0x102,  0x103,  0x104,  0x105,
/* _SV_CIRRUS */           0,   0x5f,   0x58,   0x5c,   0x5d,   0x60,
/* _SV_VIPER */            0,  0x101,      0,  0x103,      0,  0x105,
};


// The page setting functions are defined as FARC pointers.
// This is _WCI86FAR for 16-bit, and nothing for 32-bit flat model.
// QNX 32-bit uses small model, and FARC is defined as _WCI86FAR.
// For QNX 32-bit, we still want only near pointers in the table though,
// to avoid segment relocations in the executable.
// The assignment to _SetVGAPage provides the CS value at runtime.
#if defined(__QNX__) && defined(__386__)
    #pragma aux cs = "mov ax,cs" value [ax] modify []
    extern unsigned short cs(void);
    #define INIT_FARC
    #define MAKE_VGA_PG_PTR( x )        MK_FP( cs(), x )
#else
    #define INIT_FARC   _FARC
    #define MAKE_VGA_PG_PTR( x )        x
#endif
static void INIT_FARC * _VGAPageFunc[ _SV_MAX-1 ] = {
    _PageVESA,
    _PageVideo7,
    _PageParadise,
    _PageATI,
    _PageTseng3,
    _PageTseng4,
    _PageOak,
    _PageTrident,
    _PageChips,
    _PageGenoa,
    _PageS3,
    _PageCirrus,
    _PageViper,
};


static short SuperVGASetMode( short adapter, short mode, short *stride )
//======================================================================

{
    short               val;
//#if !defined( __QNX__ )
    short               granule;
    unsigned short      buf[ 128 ];     // 256 bytes
#if defined( __386__ ) && !defined(__QNX__)
    short               i;
    short far           *rbuf;
    RM_ALLOC            mem;
#endif
//#endif

    *stride = 0;
    switch( adapter ) {
//#if !defined( __QNX__ )
    case _SV_VESA:
        #if defined( __386__ ) && !defined(__QNX__)
            if( !_RMAlloc( 256, &mem ) ) {
                return( FALSE );
            }
            /*
                AH=0x4F is a VESA BIOS call AL=0x01 VESA return SVGA Mode info
                CX=desired mode
                ES:DI=address of information block
                return value AL=0x4F AH=0x00
            */
            val = _RMInterrupt( 0x10, 0x4f01, 0, mode, 0, mem.rm_seg, 0 );
            if( val == 0x004f ) {
                rbuf = mem.pm_ptr;
                for( i = 2; i <= 8; ++i ) {
                    buf[ i ] = rbuf[ i ];
                }
            }
            _RMFree( &mem );
            if( val != 0x004f ) {
                return( FALSE );
            }
        #else
            if( GetVESAInfo( 0x4f01, mode, &buf ) != 0x004f ) {
                return( FALSE );
            }
        #endif
        #if !defined( __QNX__ )
        if( buf[ 3 ] != 64 || buf[ 4 ] != 0xa000 ) {    // need 64k pages
        #else
        if( buf[ 3 ] != 64 ) {    // need 64k pages
        #endif
            return( FALSE );                            // starting at A000
        }
        *stride = buf[ 8 ];
        /*
            AH=0x4F is a VESA BIOS call AL=0x02 VESA set SVGA Display Mode
            BX=desired mode
            return value AL=0x4F AH=0x00
        */
        if( VideoInt( 0x4f02, mode, 0, 0 ) != 0x004f ) {   // set mode
            return( FALSE );
        }
        granule = 64 / buf[ 2 ];
        _VGAGran = 0;
        while( granule > 1 ) {
            granule >>= 1;
            ++_VGAGran;
        }
        break;
//#endif

    case _SV_VIDEO7:
        VideoInt( 0x6f05, mode, 0, 0 );         // set mode
        if( ( VideoInt( 0x6f04, 0, 0, 0 ) & 0x00ff ) != mode ) {  // get mode
            return( FALSE );
        }
        outpw( 0x3c4, 0xea06 );     // enable extended register access
        if( mode == 0x66 ) {        // enable access to 2nd bank
            outp( 0x3c4, 0xff );
            val = inp( 0x3c5 );
            outp( 0x3c5, val | 0x10 );
        }
        // different paging mechanism for 16/256 - use VGAGran to distinguish
        if( mode == 0x65 ) {    // 16 colours
            _VGAGran = 16;
        } else {
            _VGAGran = 256 - 1;
        }
        break;

    case _SV_PARADISE:
        VideoInt( _BIOS_SET_MODE + mode, 0, 0, 0 );
        if( GetVideoMode() != mode ) {
            return( FALSE );
        }
        outpw( 0x3ce, 0x050f );     // unlock extended registers
        break;

    case _SV_ATI:
        VideoInt( _BIOS_SET_MODE + mode, 0, 0, 0 );
        if( GetVideoMode() != mode ) {
            return( FALSE );
        }
        outp( 0x1ce, 0xbe );        // ensure we're in single bank mode
        val = inp( 0x1ce + 1 );
        val &= 0xf7;                // clear bit 3 at port 0xBE
        outpw( 0x1ce, ( val << 8 ) + 0xbe );
        break;

    case _SV_TSENG3000:
    case _SV_TSENG4000:
    case _SV_OAK:
    case _SV_GENOA:
    case _SV_VIPER:
        VideoInt( _BIOS_SET_MODE + mode, 0, 0, 0 );
        if( GetVideoMode() != mode ) {
            return( FALSE );
        }
        break;

    case _SV_S3:
        if( VideoInt( 0x4f02, mode, 0, 0 ) != 0x004f ) {   // set mode
            // if 100 range modes don't work, try 200 range
            if( VideoInt( 0x4f02, mode + 0x100, 0, 0 ) != 0x004f ) {
                return( FALSE );
            }
        }
        // different paging mechanism for 16/256 - use VGAGran to distinguish
        if( mode == 0x104 ) {    // 16 colours
            _VGAGran = 16;
        } else {
            _VGAGran = 256 - 1;
        }
        break;

    case _SV_TRIDENT:
        VideoInt( _BIOS_SET_MODE + mode, 0, 0, 0 );
        if( GetVideoMode() != mode ) {
            return( FALSE );
        }
        outp( 0x3c4, 0x0b );        // enable extended register
        inp( 0x3c4+1 );
        outp( 0x3ce, 0x06 );
        val = inp( 0x3ce+1 );
        outp( 0x3ce+1, ( val & 0xf3 ) | 0x04 );
        break;

    case _SV_CHIPS:
        VideoInt( _BIOS_SET_MODE + mode, 0, 0, 0 );
        if( GetVideoMode() != mode ) {
            return( FALSE );
        }
        outp( 0x46e8, 0x18 );       // enable setup
        outp( 0x103, 0x80 );        // enable extended registers
        outp( 0x46e8, 0x08 );       // disable setup
        outp( 0x3d6, 0x0b );        // enable memory beyond 256k
        val = inp( 0x3d6+1 );
        outp( 0x3d6+1, val | 1 );
        if( mode == 0x61 || mode == 0x72 ) {  // 16 colours
            _VGAGran = 4;
        } else {
            _VGAGran = 2;
        }

    case _SV_CIRRUS:
        VideoInt( _BIOS_SET_MODE + mode, 0, 0, 0 );
        if( GetVideoMode() != mode ) {
            return( FALSE );
        }
        outp( 0x3ce, 0x0b );        // enable single-page mapping
        val = inp( 0x3ce+1 );
        outp( 0x3ce+1, val & 0xfe );
        break;

    }
    return( TRUE );
}


static short _SuperVGAInit( short mode )
//======================================

// This routine checks for the presence of a SuperVGA adapter
// and performs the appropriate initialization.

{
    short               bios_mode;
    short               adapter;
    short               stride;

    adapter = _SuperVGAType();
    if( adapter == _SV_NONE ) {
        return( FALSE );
    }

    if( adapter == _SV_VESA ) {
        bios_mode = mode;
#if 0
// This mode is untested
    } else if( adapter == _SV_ATI && mode == 0x110 ) {
        // 640x480, 32k colour mode (VESA 0x110)
        bios_mode = 0x72;
#endif
    } else {
        bios_mode = _SuperVGAModes[ adapter - 2 ][ mode - 0x100 ];
    }

    if( bios_mode == 0 ) {
        return( FALSE );
    }

    if( !SuperVGASetMode( adapter, bios_mode, &stride ) ) {
        return( FALSE );
    }

    _VGAPage = 0xff;
    _SetVGAPage = MAKE_VGA_PG_PTR( _VGAPageFunc[ adapter - 1 ] );

    //              x,   y, strd, col, bpp, pag, seg,     off,    siz, mis
    switch( mode ) {
    case 0x100:
        _GrInit(  640, 400,  640, 256,   8,   1, _EgaSeg, _EgaOff,   0, NO_BIOS );
        _CurrState->vc.numtextcols = 80;
        _CurrState->vc.numtextrows = 25;
        break;
    case 0x101:
        _GrInit(  640, 480,  640, 256,   8,   1, _EgaSeg, _EgaOff,   0, NO_BIOS );
        _CurrState->vc.numtextcols = 80;
        _CurrState->vc.numtextrows = 30;
        break;
    case 0x102:
        _GrInit(  800, 600,  100,  16,   4,   1, _EgaSeg, _EgaOff,   0, PLANAR + NO_BIOS );
        _CurrState->vc.numtextcols = 100;
        _CurrState->vc.numtextrows = 40;
        break;
    case 0x103:
        _GrInit(  800, 600,  800, 256,   8,   1, _EgaSeg, _EgaOff,   0, NO_BIOS );
        _CurrState->vc.numtextcols = 100;
        _CurrState->vc.numtextrows = 40;
        break;
    case 0x104:
        _GrInit( 1024, 768,  128,  16,   4,   1, _EgaSeg, _EgaOff,   0, PLANAR + NO_BIOS );
        _CurrState->vc.numtextcols = 128;
        _CurrState->vc.numtextrows = 50;
        break;
    case 0x105:
        _GrInit( 1024, 768, 1024, 256,   8,   1, _EgaSeg, _EgaOff,   0, NO_BIOS );
        _CurrState->vc.numtextcols = 128;
        _CurrState->vc.numtextrows = 50;
        break;
#if 0
// This mode is untested
    case 0x110:
        _GrInit(  640, 480, 1280, 32768,16,   1, _EgaSeg, _EgaOff,   0, NO_BIOS );
        _CurrState->vc.numtextcols = 80;
        _CurrState->vc.numtextrows = 30;
        break;
#endif
    }

    _CurrState->vc.mode = mode;         // _GrInit fills in bios_mode
    _CurrState->vc.adapter = _SVGA;
    if( stride )
        _CurrState->stride = stride;    // Override default stride if necessary
    _VGAStride = _CurrState->stride;

    return( TRUE );
}


static void _NoOp( void )
/*=======================

    Dummy function that does nothing.   */

{
}


static void _SetPage( short page_num )
//====================================

//  Do an indirect call to the _SetVGAPage function.
//  Done here so that we can use the PAGE_FUNC pragma to cause
//  the argument to be passed in [E]AX.

{
    void PAGE_FUNC      ( _FARC *page_func )( short );

    page_func = (void PAGE_FUNC _FARC *) _SetVGAPage;
    (*page_func)( page_num );
}


static void _SVGAReset( void )
//============================

{
/*
    If you are using a SVGA mode that is banked (e.g. 1024x768x256), the
    graphics library unneccessarily resets the video bank to 0 after a
    call to any graphics library function. This means that a series of
    _lineto()'s will be half as slow at the bottom of the video screen
    compared to drawing near the top of the video screen.

    The library attempts to cache the video bank so that successive
    graphics operations in the same region do not unnecessarily spend
    time switching the bank. The caching works as long as no external
    user code causes the current page to change.  The caching is defeated
    by setting the page back to 0 after every graphics operation.

    J.B.Schueler

    _SetPage( 0 );    // reset to page 0
*/
}


static void _SVGAReset16( void )
//==============================

{
    _EGAReset();
    _SVGAReset();
}


static void _Setup100( short x, short y, short colour )
/*=====================================================

    Calculate screen memory address and associated masks for the
    position (x,y) in 800 x 600, 16 colour mode. */

{
    unsigned int        pixel_offset;

    pixel_offset = y * _CurrState->stride + ( x >> 3 );
    _Screen.mem = MK_FP( _CurrState->screen_seg,
                         _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = x & 7;            // position of pixel in byte
    _Screen.mask = ( 0x80 >> _Screen.bit_pos ) << 8;
    _Screen.colour = colour;
}


static void _Setup128( short x, short y, short colour )
/*=====================================================

    Calculate screen memory address and associated masks for the
    position (x,y) in 1024 x 768, 16 colour mode. */

{
    short               page_num;
    unsigned long       pixel_offset;

    pixel_offset = (long) y * _CurrState->stride + ( x >> 3 );
    page_num = pixel_offset / 0x10000;
    pixel_offset &= 0xffff;
    _SetPage( page_num );
    _Screen.mem = MK_FP( _CurrState->screen_seg,
                         _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = x & 7;            // position of pixel in byte
    _Screen.mask = ( 0x80 >> _Screen.bit_pos ) << 8;
    _Screen.colour = colour;
}


static void _Setup640( short x, short y, short colour )
/*=====================================================

    Calculate screen memory address and associated masks for the
    position (x,y) in 640 x ???, 256 colour mode. */

{
    short               page_num;
    unsigned long       pixel_offset;

    pixel_offset = (long) y * _CurrState->stride + x;
    page_num = pixel_offset / 0x10000;
    pixel_offset &= 0xffff;
    _SetPage( page_num );
    _Screen.mem = MK_FP( _CurrState->screen_seg,
                         _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = 0;        // position of pixel in byte
    _Screen.colour = colour;
}


static void _Setup800( short x, short y, short colour )
/*=====================================================

    Calculate screen memory address and associated masks for the
    position (x,y) in 800 x 600, 256 colour mode. */

{
    short               page_num;
    unsigned long       pixel_offset;

    pixel_offset = (long) y * _CurrState->stride + x;
    page_num = pixel_offset / 0x10000;
    pixel_offset &= 0xffff;
    _SetPage( page_num );
    _Screen.mem = MK_FP( _CurrState->screen_seg,
                         _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = 0;        // position of pixel in byte
    _Screen.colour = colour;
}


static void _Setup1024( short x, short y, short colour )
/*=====================================================

    Calculate screen memory address and associated masks for the
    position (x,y) in 1024 x 768, 256 colour mode. */

{
    short               page_num;
    unsigned long       pixel_offset;

    pixel_offset = (long) y * _CurrState->stride + x;
    page_num = pixel_offset / 0x10000;
    pixel_offset &= 0xffff;
    _SetPage( page_num );
    _Screen.mem = MK_FP( _CurrState->screen_seg,
                         _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = 0;        // position of pixel in byte
    _Screen.colour = colour;
}

#if 0
// This mode is untested
static void _Setup1280( short x, short y, short colour )
/*======================================================

    Calculate screen memory address and associated masks for the
    position (x,y) in 640 x 480, 32768 colour mode. */

{
    short               page_num;
    unsigned long       pixel_offset;

    pixel_offset = (long) y * _CurrState->stride + (x << 1);
    page_num = pixel_offset / 0x10000;
    pixel_offset &= 0xffff;
    _SetPage( page_num );
    _Screen.mem = MK_FP( _CurrState->screen_seg,
                         _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = 0;        // position of pixel in word
    _Screen.colour = colour;
}
#endif

gr_device _FARD         _GrSVGA_100 = {     // 640 x 400, 256 colours
    _SuperVGAInit, _NoOp,                   // 640 x 480, 256 colours
    _NoOp, _SVGAReset,
    _Setup640,
    _MoveUp640,_MoveLeft256,_MoveDown640,_MoveRight256,
    _Rep19,_CoXor,_And19,_CoOr,
    _GetDot19,_Zap256,_Fill256,_PixCopy256,_PixRead256,
    _ScanLeft256,_ScanRight256
};


gr_device _FARD         _GrSVGA_102 = {     // 800 x 600, 16 colours
    _SuperVGAInit, _NoOp,
    _EGASet, _EGAReset,
    _Setup100,
    _MoveUp100,_EGAMoveLeft,_MoveDown100,_EGAMoveRight,
    _EGARep,_EGARep,_EGARep,_EGARep,
    _EGAGetDot,_EGAZap,_EGAFill,_EGAPixCopy,_EGAReadRow,
    _EGAScanLeft,_EGAScanRight
};


gr_device _FARD         _GrSVGA_103 = {     // 800 x 600, 256 colours
    _SuperVGAInit, _NoOp,
    _NoOp, _SVGAReset,
    _Setup800,
    _MoveUp800,_MoveLeft256,_MoveDown800,_MoveRight256,
    _Rep19,_CoXor,_And19,_CoOr,
    _GetDot19,_Zap256,_Fill256,_PixCopy256,_PixRead256,
    _ScanLeft256,_ScanRight256
};


gr_device _FARD         _GrSVGA_104 = {     // 1024 x 768, 16 colours
    _SuperVGAInit, _NoOp,
    _EGASet, _SVGAReset16,
    _Setup128,
    _MoveUp128,_EGAMoveLeft,_MoveDown128,_EGAMoveRight,
    _EGARep,_EGARep,_EGARep,_EGARep,
    _EGAGetDot,_EGAZap,_EGAFill,_EGAPixCopy,_EGAReadRow,
    _EGAScanLeft,_EGAScanRight
};


gr_device _FARD         _GrSVGA_105 = {     // 1024 x 768, 256 colours
    _SuperVGAInit, _NoOp,
    _NoOp, _SVGAReset,
    _Setup1024,
    _MoveUp1024,_MoveLeft19,_MoveDown1024,_MoveRight19,
    _Rep19,_CoXor,_And19,_CoOr,
    _GetDot19,_Zap19,_Fill19,_PixCopy19,_PixRead19,
    _ScanLeft19,_ScanRight19
};

#if 0
// This mode is untested
gr_device _FARD         _GrSVGA_110 = {     // 640 x 480, 32768 colours
    _SuperVGAInit, _NoOp,
    _NoOp, _SVGAReset,
    _Setup1280,
    _MoveUp1280,_MoveLeftWord,_MoveDown1280,_MoveRightWord,
    _RepWord,_XorWord,_AndWord,_OrWord,
    _GetDotWord,_ZapWord,_FillWord,_PixCopyWord,_PixReadWord,
    _ScanLeftWord,_ScanRightWord
};
#endif
