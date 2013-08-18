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

extern unsigned short   cs( void );
#pragma aux cs = "mov ax,cs" value [ax] modify []

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


static vgapage_fn _WCI86FAR *_VGAPageFunc[ _SV_MAX-1 ] = {
    (vgapage_fn _WCI86FAR *)_PageVESA,
    (vgapage_fn _WCI86FAR *)_PageVideo7,
    (vgapage_fn _WCI86FAR *)_PageParadise,
    (vgapage_fn _WCI86FAR *)_PageATI,
    (vgapage_fn _WCI86FAR *)_PageTseng3,
    (vgapage_fn _WCI86FAR *)_PageTseng4,
    (vgapage_fn _WCI86FAR *)_PageOak,
    (vgapage_fn _WCI86FAR *)_PageTrident,
    (vgapage_fn _WCI86FAR *)_PageChips,
    (vgapage_fn _WCI86FAR *)_PageGenoa,
    (vgapage_fn _WCI86FAR *)_PageS3,
    (vgapage_fn _WCI86FAR *)_PageCirrus,
    (vgapage_fn _WCI86FAR *)_PageViper,
};

#if defined( __386__ ) && !defined(__QNX__)
static void mymemcpy( char *d, char far *s, int len )
{
    while( len-- > 0 ) {
        *d++ = *s++;
    }
}
#endif

#define U16(p,o)    *((unsigned short *)p + o)

static short SuperVGASetMode( short adapter, short mode, short *stride )
//======================================================================

{
    short               val;
//#if !defined( __QNX__ )
    short               granule;
    char                buf[ 256 ];
#if defined( __386__ ) && !defined(__QNX__)
//    short               i;
    char far            *rbuf;
    RM_ALLOC            mem;
#endif
//#endif

    *stride = 0;
    switch( adapter ) {
//#if !defined( __QNX__ )
    case _SV_VESA:
#if defined( __386__ ) && !defined(__QNX__)
//            assert(256>=sizeof(struct VbeModeInfo));//large enough?
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
    #if defined( VERSION2 )
            mymemcpy( (char *)&(_CurrState->mi), rbuf, sizeof( struct VbeModeInfo ) );
    #endif
            mymemcpy( buf, rbuf, 16 );
        }
        _RMFree( &mem );
        if( val != 0x004f ) {
            return( FALSE );
        }
#else
        if( GetVESAInfo( 0x4f01, mode, &buf ) != 0x004f ) {
            return( FALSE );
        }
    #if defined( VERSION2 )
        memcpy( &(_CurrState->mi), buf, sizeof( struct VbeModeInfo ) );
    #endif
#endif
#if !defined( __QNX__ )
        if( U16( buf, 3 ) != 64 || U16( buf, 4 ) != 0xa000 ) {    // need 64k pages
#else
        if( U16( buf, 3 ) != 64 ) {    // need 64k pages
#endif
            return( FALSE );                            // starting at A000
        }
        *stride = U16( buf, 8 );
        /*
            AH=0x4F is a VESA BIOS call AL=0x02 VESA set SVGA Display Mode
            BX=desired mode
            return value AL=0x4F AH=0x00
        */
        if( VideoInt( 0x4f02, mode, 0, 0 ) != 0x004f ) {   // set mode
            return( FALSE );
        }
        granule = 64 / U16( buf, 2 );
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

#if defined( VERSION2 )

struct gfx_parm
{
    int xres,yres;
    grcolor ncolors;
};

#define CTXT 0
#define C256 256
#define C16 16
#define C32K 32768
#define C64K 65536
#define CTRUE 16777216
static struct gfx_parm parmtbl[32]=
{
    { 640,  400,  256},
    { 640,  480,  256},
    { 800,  600,  16},
    { 800,  600,  256},
    { 1024, 768,  16},
    { 1024, 768,  256},
    { 1280, 1024, 16},
    { 1280, 1024, 256},
    { 80,   60,   0},
    { 132,  25,   0},
    { 132,  43,   0},
    { 132,  50,   0},
    { 132,  60,   0},
    { 320,  200,  C32K},
    { 320,  200,  C64K},
    { 320,  200,  CTRUE},
    { 640,  480,  C32K},
    { 640,  480,  C64K},
    { 640,  480,  CTRUE},
    { 800,  600,  C32K},
    { 800,  600,  C64K},
    { 800,  600,  CTRUE},
    { 1024, 768,  C32K},
    { 1024, 768,  C64K},
    { 1024, 768,  CTRUE},
    { 1280, 1024, C32K},
    { 1280, 1024, C64K},
    { 1280, 1024, CTRUE},
    { 1600, 1200, 256},
    { 1600, 1200, C32K},
    { 1600, 1200, C64K},
    { 1600, 1200, CTRUE}
};

#if 0
static void printModeCaps(struct VbeModeInfo * ModeInfo)
{
   char * model_strings[]={
   "TEXTMODE",
   "CGA",
   "HERCULES",
   "PLANAR",
   "PACKED",
   "MODEX",
   "RGB",
   "YUV"
   };
   int i;
   char * p;
   if(ModeInfo->ModeAttributes&VBEMODE_CAPS_AVAILABLE)    printf("Mode is available\n");
   if(ModeInfo->ModeAttributes&VBEMODE_CAPS_EXTINFO)    printf("Extended info\n");
   if(ModeInfo->ModeAttributes&VBEMODE_CAPS_TTYOUTPUT)    printf("TTY(Bios) output is available\n");
   if(ModeInfo->ModeAttributes&VBEMODE_CAPS_COLORMODE)    printf("Color video mode\n");
   if(ModeInfo->ModeAttributes&VBEMODE_CAPS_GRAPHMODE)    printf("Graphics mode\n");
   if(ModeInfo->ModeAttributes&VBEMODE_CAPS_NONVGA)    printf("Non-VGA mode\n");
   if(ModeInfo->ModeAttributes&VBEMODE_CAPS_NONBANKED)    printf("Banked frame buffer not supported\n");
   if(ModeInfo->ModeAttributes&VBEMODE_CAPS_LINEAR)    printf("Linear frame buffer is supported\n");
   if(ModeInfo->ModeAttributes&VBEMODE_CAPS_DOUBLESCAN)    printf("Double scan mode available\n");
   if(ModeInfo->ModeAttributes&VBEMODE_CAPS_INTERLACED)    printf("Interlaced Mode Available\n");
   if(ModeInfo->ModeAttributes&VBEMODE_CAPS_TRIPLEBUF)    printf("Triple Buffering supported\n");
   if(ModeInfo->ModeAttributes&VBEMODE_CAPS_STEREO)    printf("Stereoscopic display supported\n");
   if(ModeInfo->ModeAttributes&VBEMODE_CAPS_DUALDISP)    printf("dual display start address support\n");
   
   i=ModeInfo->MemoryModel;
   if(i>(sizeof(model_strings)/sizeof(*model_strings)))
     p="invalid";
   else
     p=model_strings[i];
   printf("Memory mode: %s\n",p);
   return;
}
#endif

static void _SetupVESA( short x, short y, long colour );
static void _Setup128( short x, short y, long colour );
static void _NoOp( void );
static void _SVGAReset( void );
static void _SVGAReset16( void );
gr_device _FARD         _GrVESA;

static void _setup_grvesa(void)
//======================================

// Sets the VESA specific device function pointers and some data
{
    _CurrState->bytes_per_pixel = _CurrState->mi.BytesPerScanLine / _CurrState->mi.XResolution;
    _VGABytesPerPixel = _CurrState->mi.BytesPerScanLine / _CurrState->mi.XResolution;
    _GrVESA.setup = _SetupVESA;
    _GrVESA.set = _NoOp;
    _GrVESA.reset = _SVGAReset;
    _GrVESA.left = _MoveLeftVESA;
    _GrVESA.right = _MoveRightVESA;
//    printf("_VGABytesPerPixel: %d\n",_VGABytesPerPixel);
//    printModeCaps(&(_CurrState->mi));
    /*
    _SuperVGAInit, _NoOp,
    _EGASet, _SVGAReset16,
    _Setup128,
    _MoveUp128,_EGAMoveLeft,_MoveDown128,_EGAMoveRight,
    _EGARep,_EGARep,_EGARep,_EGARep,
    _EGAGetDot,_EGAZap,_EGAFill,_EGAPixCopy,_EGAReadRow,
    _EGAScanLeft,_EGAScanRight
    */
    switch( _VGABytesPerPixel ) {
    case 0:
        _GrVESA.set=_EGASet;
        _GrVESA.reset=_SVGAReset16;
        _GrVESA.setup=_Setup128;
        _GrVESA.left=_MoveLeftEGAX;
        _GrVESA.right=_MoveRightEGAX;
        _GrVESA.plot[0]=_EGARep;
        _GrVESA.plot[1]=_EGARep;
        _GrVESA.plot[2]=_EGARep;
        _GrVESA.plot[3]=_EGARep;
        _GrVESA.getdot=_EGAGetDot;
        _GrVESA.zap=_ZapEGAX;
        _GrVESA.fill=_FillEGAX;
        _GrVESA.pixcopy=_PixCopyEGAX;
        _GrVESA.readrow=_PixReadRowEGAX;
        _GrVESA.scanleft=_ScanLeftEGAX;
        _GrVESA.scanright=_ScanRightEGAX;
        return;//we are palettized
    case 1:
        _GrVESA.plot[0]=_Rep19;
        _GrVESA.plot[1]=_CoXor;
        _GrVESA.plot[2]=_And19;
        _GrVESA.plot[3]=_CoOr;
        _GrVESA.getdot=_GetDot19;
        _GrVESA.zap=_Zap256;
        _GrVESA.fill=_Fill256;
        _GrVESA.pixcopy=_PixCopy256;
        _GrVESA.readrow=_PixRead256;
        _GrVESA.scanleft=_ScanLeft256;
        _GrVESA.scanright=_ScanRight256;
        return;//we are palettized
    case 2:
        _GrVESA.plot[0]=_RepWord;
        _GrVESA.plot[1]=_XorWord;
        _GrVESA.plot[2]=_AndWord;
        _GrVESA.plot[3]=_OrWord;
        _GrVESA.getdot=_GetDotWord;
        _GrVESA.zap=_ZapWord;
        _GrVESA.fill=_FillWord;
        _GrVESA.pixcopy=_PixCopyWord;
        _GrVESA.readrow=_PixReadWord;
        _GrVESA.scanleft=_ScanLeftWord;
        _GrVESA.scanright=_ScanRightWord;
        break;
    case 3:
        _GrVESA.plot[0]=_RepTByte;
        _GrVESA.plot[1]=_XorTByte;
        _GrVESA.plot[2]=_AndTByte;
        _GrVESA.plot[3]=_OrTByte;
        _GrVESA.getdot=_GetDotTByte;
        _GrVESA.zap=_ZapTByte;
        _GrVESA.fill=_FillTByte;
        _GrVESA.pixcopy=_PixCopyTByte;
        _GrVESA.readrow=_PixReadTByte;
        _GrVESA.scanleft=_ScanLeftTByte;
        _GrVESA.scanright=_ScanRightTByte;
        break;
    case 4:
        _GrVESA.plot[0]=_RepDWord;
        _GrVESA.plot[1]=_XorDWord;
        _GrVESA.plot[2]=_AndDWord;
        _GrVESA.plot[3]=_OrDWord;
        _GrVESA.getdot=_GetDotDWord;
        _GrVESA.zap=_ZapDWord;
        _GrVESA.fill=_FillDWord;
        _GrVESA.pixcopy=_PixCopyDWord;
        _GrVESA.readrow=_PixReadDWord;
        _GrVESA.scanleft=_ScanLeftDWord;
        _GrVESA.scanright=_ScanRightDWord;
        break;
    }
    _CurrState->pixel_mask = _rgb2pixel( 255, 255, 255 );

    //                        r    g    b
    _coltbl[0]  = _rgb2pixel( 0,   0,   0   );
    _coltbl[1]  = _rgb2pixel( 0,   0,   170 );
    _coltbl[2]  = _rgb2pixel( 0,   170, 0   );
    _coltbl[3]  = _rgb2pixel( 0,   170, 170 );
    _coltbl[4]  = _rgb2pixel( 170, 0,   0   );
    _coltbl[5]  = _rgb2pixel( 170, 0,   170 );
    _coltbl[6]  = _rgb2pixel( 170, 85,  0   );
    _coltbl[7]  = _rgb2pixel( 170, 170, 170 );
    _coltbl[8]  = _rgb2pixel( 85,  85,  85  );
    _coltbl[9]  = _rgb2pixel( 85,  85,  255 );
    _coltbl[10] = _rgb2pixel( 85,  255, 85  );
    _coltbl[11] = _rgb2pixel( 85,  255, 255 );
    _coltbl[12] = _rgb2pixel( 255, 85,  85  );
    _coltbl[13] = _rgb2pixel( 255, 85,  255 );
    _coltbl[14] = _rgb2pixel( 255, 255, 85  );
    _coltbl[15] = _rgb2pixel( 255, 255, 255 );
}

#endif

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

// The page setting functions are defined as FARC pointers.
// This is _WCI86FAR for 16-bit, and nothing for 32-bit flat model.
// QNX 32-bit uses small model, and FARC is defined as _WCI86FAR.
// For QNX 32-bit, we still want only near pointers in the table though,
// to avoid segment relocations in the executable.
// The assignment to _SetVGAPage provides the CS value at runtime.
#if defined( __QNX__ ) && defined( __386__ )
    _SetVGAPage = MK_FP( cs(), _VGAPageFunc[ adapter - 1 ] );
#else
    _SetVGAPage = _VGAPageFunc[ adapter - 1 ];
#endif

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
#if defined( VERSION2 )
        //actually 64(12 pixel) and 96(8 pixel) would work well, too.. but 48 seems to be what the BIOS uses (reading the appropriate byte gives 47)
        _CurrState->vc.numtextrows = 48;
#else
        _CurrState->vc.numtextrows = 50;
#endif
        break;
    case 0x105:
        _GrInit( 1024, 768, 1024, 256, 8, 1, _EgaSeg, _EgaOff, 0, NO_BIOS );
        _CurrState->vc.numtextcols = 128;
#if defined( VERSION2 )
        _CurrState->vc.numtextrows = 48;
#else
        _CurrState->vc.numtextrows = 50;
#endif
        break;
#if 0
// This mode is untested. 
    case 0x110:
        _GrInit( 640, 480, 1280, 32768, 16, 1, _EgaSeg, _EgaOff, 0, NO_BIOS );
        _CurrState->vc.numtextcols = 80;
        _CurrState->vc.numtextrows = 30;
        break;
#endif
    default:
#if defined( VERSION2 )
//    printf("default: adapter: %d, mode: %d, bpp: %d\n",adapter,mode,_CurrState->mi.BitsPerPixel);
        if( ( adapter == _SV_VESA ) && ( mode >= 0x100 ) && ( mode <= 0x11F ) ) {       //mode info is valid
            struct gfx_parm p = parmtbl[mode - 0x100];
//            printf("p.ncolors: %d\n",p.ncolors);
            _GrInit( p.xres, p.yres, _CurrState->mi.BytesPerScanLine, p.ncolors, _CurrState->mi.BitsPerPixel,
                     1, _EgaSeg, _EgaOff, 0,
                     (( p.ncolors != 0 ) ? NO_BIOS : 0) + (( _CurrState->mi.MemoryModel == VBEMODE_MODEL_PLANAR ) ? PLANAR : 0) );
            if( p.ncolors == 0 ) {      //textmode
                _CurrState->vc.numtextcols = p.xres;
                _CurrState->vc.numtextrows = p.yres;
            } else {
                _CurrState->vc.numtextcols = p.xres / 8;
                if( p.yres < 480 ) {
                    _CurrState->vc.numtextrows = p.yres / 8;
                } else if( p.yres == 600 ) {
                    _CurrState->vc.numtextrows = 40;
                } else {
                    _CurrState->vc.numtextrows = p.yres / 16;
                }
            }
            _setup_grvesa();
        } else {
            return( FALSE );
        }
#endif
        break;
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

    ( *_SetVGAPage )( 0 );    // reset to page 0
*/
}


static void _SVGAReset16( void )
//==============================

{
    _EGAReset();
    _SVGAReset();
}


static void _Setup100( short x, short y, grcolor colour )
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


static void _Setup128( short x, short y, grcolor colour )
/*=====================================================

    Calculate screen memory address and associated masks for the
    position (x,y) in 1024 x 768, 16 colour mode. */

{
    short               page_num;
    unsigned long       pixel_offset;

    pixel_offset = (long) y * _CurrState->stride + ( x >> 3 );
    page_num = pixel_offset / 0x10000;
    pixel_offset &= 0xffff;
    ( *_SetVGAPage )( page_num );
    _Screen.mem = MK_FP( _CurrState->screen_seg, _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = x & 7;            // position of pixel in byte
    _Screen.mask = ( 0x80 >> _Screen.bit_pos ) << 8;
    _Screen.colour = colour;
}

#if !defined( VERSION2 )

static void _Setup640( short x, short y, grcolor colour )
/*=====================================================

    Calculate screen memory address and associated masks for the
    position (x,y) in 640 x ???, 256 colour mode. */

{
    short               page_num;
    unsigned long       pixel_offset;

    pixel_offset = (long) y * _CurrState->stride + x;
    page_num = pixel_offset / 0x10000;
    pixel_offset &= 0xffff;
    ( *_SetVGAPage )( page_num );
    _Screen.mem = MK_FP( _CurrState->screen_seg, _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = 0;        // position of pixel in byte
    _Screen.colour = colour;
}


static void _Setup800( short x, short y, grcolor colour )
/*=====================================================

    Calculate screen memory address and associated masks for the
    position (x,y) in 800 x 600, 256 colour mode. */

{
    short               page_num;
    unsigned long       pixel_offset;

    pixel_offset = (long) y * _CurrState->stride + x;
    page_num = pixel_offset / 0x10000;
    pixel_offset &= 0xffff;
    ( *_SetVGAPage )( page_num );
    _Screen.mem = MK_FP( _CurrState->screen_seg, _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = 0;        // position of pixel in byte
    _Screen.colour = colour;
}

#endif

static void _Setup1024( short x, short y, grcolor colour )
/*======================================================

    Calculate screen memory address and associated masks for the
    position (x,y) in 1byte modes. */

{
    short               page_num;
    unsigned long       pixel_offset;

    pixel_offset = (long) y * _CurrState->stride + x;
    page_num = pixel_offset / 0x10000;
    pixel_offset &= 0xffff;
    ( *_SetVGAPage )( page_num );
    _Screen.mem = MK_FP( _CurrState->screen_seg, _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = 0;        // position of pixel in word
    _Screen.colour = colour;
}

#if defined( VERSION2 )

static void _SetupVESA( short x, short y, grcolor colour )
/*======================================================

    Calculate screen memory address and associated masks for the
    position (x,y) in arbitrary byte modes modes. */

{
    short               page_num;
    unsigned long       pixel_offset;

    pixel_offset = (long) y * _CurrState->stride + x * _CurrState->bytes_per_pixel;
    page_num = pixel_offset / 0x10000;
    pixel_offset &= 0xffff;
    ( *_SetVGAPage )( page_num );
    _Screen.mem = MK_FP( _CurrState->screen_seg, _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = 0;        // position of pixel in word
    _Screen.colour = colour;
}

#endif

#if 0
// This mode is untested
static void _Setup1280( short x, short y, grcolor colour )
/*======================================================

    Calculate screen memory address and associated masks for the
    position (x,y) in 2byte modes. */

{
    short               page_num;
    unsigned long       pixel_offset;

    pixel_offset = (long) y * _CurrState->stride + (x << 1);
    page_num = pixel_offset / 0x10000;
    pixel_offset &= 0xffff;
    ( *_SetVGAPage )( page_num );
    _Screen.mem = MK_FP( _CurrState->screen_seg, _CurrState->screen_off + pixel_offset );
    _Screen.bit_pos = 0;        // position of pixel in word
    _Screen.colour = colour;
}
#endif

gr_device _FARD         _GrSVGA_100 = {     // 640 x 400, 256 colours
    _SuperVGAInit, _NoOp,                   // 640 x 480, 256 colours
    _NoOp, _SVGAReset,
#if defined( VERSION2 )
    _Setup1024,
#else
    _Setup640,
#endif
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
#if defined( VERSION2 )
    _Setup1024,
#else
    _Setup800,
#endif
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

#if defined( VERSION2 )

gr_device _FARD         _GrVESA={     // various
    _SuperVGAInit, _NoOp,
    _NoOp, _SVGAReset,
    _SetupVESA,
    _MoveUpVESA,_MoveLeftVESA,_MoveDownVESA,_MoveRightVESA,
    _Rep19,_CoXor,_And19,_CoOr,
    _GetDot19,_Zap19,_Fill19,_PixCopy19,_PixRead19,
    _ScanLeft19,_ScanRight19
};

#endif
