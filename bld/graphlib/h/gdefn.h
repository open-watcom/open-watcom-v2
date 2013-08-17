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
* Description:  Graph library internal structures and function prototypes.
*
****************************************************************************/


#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <i86.h>
#if defined( VERSION2 )
#include "graph2.gh"
#else
#include "graph.gh"
#endif


#if defined( VERSION2 )

// Mode attributes flags
#define VBEMODE_CAPS_AVAILABLE  (1<<0)  // Mode is available
#define VBEMODE_CAPS_EXTINFO    (1<<1)  // optional information available
#define VBEMODE_CAPS_TTYOUTPUT  (1<<2)  // TTY(Bios) output is available
#define VBEMODE_CAPS_COLORMODE  (1<<3)  // 1Color video mode/0MONO
#define VBEMODE_CAPS_GRAPHMODE  (1<<4)  // 1Graphics mode/0TextMode
//VBE2.0
#define VBEMODE_CAPS_NONVGA     (1<<5)  // Non-VGA mode
#define VBEMODE_CAPS_NONBANKED  (1<<6)  // Banked frame buffer not supported
#define VBEMODE_CAPS_LINEAR     (1<<7)  // Linear frame buffer is supported
#define VBEMODE_CAPS_DOUBLESCAN (1<<8)  // Double scan mode available
//VBE3.0
#define VBEMODE_CAPS_INTERLACED (1<<9)  // Interlaced Mode Available OR
                                        // (VBE/AF) must call enabledirectaccess before calling bank switching
#define VBEMODE_CAPS_TRIPLEBUF   (1<<10) // Triple Buffering supported
#define VBEMODE_CAPS_STEREO     (1<<11) // Stereoscopic display supported
#define VBEMODE_CAPS_DUALDISP   (1<<12) // dual display start address support

// Mode memory models

#define VBEMODE_MODEL_TEXT      (0)     // Text mode
#define VBEMODE_MODEL_CGA       (1)     // CGA mode
#define VBEMODE_MODEL_HERCULES  (2)     // Hercules graphics mode
#define VBEMODE_MODEL_PLANAR    (3)     // VGA planar style mode
#define VBEMODE_MODEL_PACKED    (4)     // Packed pixel mode
#define VBEMODE_MODEL_MODEX     (5)     // ModeX (unchained 4, 256 color)
#define VBEMODE_MODEL_RGB       (6)     // Direct color RGB
#define VBEMODE_MODEL_YUV       (7)     // Direct color YUV

#endif

// The page setting functions are defined as FARC pointers.
// This is _WCI86FAR for 16-bit, and nothing for 32-bit flat model.
// QNX 32-bit uses small model, and FARC is defined as _WCI86FAR.
// For QNX 32-bit, we still want only near pointers in the table though,
// to avoid segment relocations in the executable.
// The assignment to _SetVGAPage provides the CS value at runtime.

#if defined( __QNX__ ) && defined( __386__ )
    #pragma aux cs = "mov ax,cs" value [ax] modify []
    extern unsigned short cs(void);
    #define MAKE_VGA_PG_PTR( x )        MK_FP( cs(), x )
#else
    #define MAKE_VGA_PG_PTR( x )        x
#endif

#if defined( __QNX__ ) || !defined( __386__ )
    #define _FARC       __far
#else
    #define _FARC
#endif

#if defined ( __386__ )
    #pragma aux VGAPAGE_FUNC "*" parm caller [eax];
#else
    #pragma aux VGAPAGE_FUNC "*" parm caller [ax];
#endif

typedef void pascal vgapage_fn( short );

#pragma aux (VGAPAGE_FUNC) vgapage_fn;


#pragma pack(push, 1);
typedef struct {
    unsigned char       start_range;
    unsigned char       end_range;
} dbcs_pair;

#include "arcinfo.h"
#include "fontdef.h"
#include "scrdesc.h"
#include "globals.h"
#include "asmrefs.h"
#include "grdev.h"
#include "lineent.h"
#include "fpi.h"
#include "entry.h"

#if( defined( __WINDOWS__ ) || defined( __OS2__ ) )
    #define _DEFAULT_WINDOWS
    #if defined( __WINDOWS__ )
        #include <win16.h>
    #else
        #define __OS2_PM__
        #define INCL_WIN
        #define INCL_GPI
        #include <wos2.h>
    #endif
    #include "wpi.h"
    #include "wingph.h"
#endif

#define MASK_LEN        8               /* length of fill mask  */

struct clip_region {
    short       xmin;
    short       xmax;
    short       ymin;
    short       ymax;
};

#if defined( VERSION2 )

struct VbeModeInfo
{
  // Mandatory information for all VBE revisions:

  unsigned short    ModeAttributes;        // mode attributes
  unsigned char     WinAAttributes;        // window A attributes
  unsigned char     WinBAttributes;        // window B attributes
  unsigned short    WinGranularity;        // window granularity
  unsigned short    WinSize;               // window size
  unsigned short    WinASegment;           // window A start segment
  unsigned short    WinBSegment;           // window B start segment
  unsigned long     WinFuncPtr;            // pointer to window function
  unsigned short    BytesPerScanLine;      // bytes per scan line

  // Mandatory information for VBE 1.2 and above:

  unsigned short  XResolution;           // horizontal resolution in pixels or chars
  unsigned short  YResolution;           // vertical resolution in pixels or chars
  unsigned char   XCharSize;             // character cell width in pixels
  unsigned char   YCharSize;             // character cell height in pixels
  unsigned char   NumberOfPlanes;        // number of memory planes
  unsigned char   BitsPerPixel;          // bits per pixel
  unsigned char   NumberOfBanks;         // number of banks
  unsigned char   MemoryModel;           // memory model type
  unsigned char   BankSize;              // bank size in KB
  unsigned char   NumberOfImagePages;    // number of image pages (less one) to fit in video ram
  unsigned char   _Reserved;             // reserved for page function

  // Direct Color fields (required for direct/6 and YUV/7 memory models)

  unsigned char   RedMaskSize;           // size of direct color red mask in bits
  unsigned char   RedFieldPosition;      // bit position of lsb of red mask
  unsigned char   GreenMaskSize;         // size of direct color green mask in bits
  unsigned char   GreenFieldPosition;    // bit position of lsb of green mask
  unsigned char   BlueMaskSize;          // size of direct color blue mask in bits
  unsigned char   BlueFieldPosition;     // bit position of lsb of blue mask
  unsigned char   RsvdMaskSize;          // size of direct color reserved mask in bits
  unsigned char   RsvdFieldPosition;     // bit position of lsb of reserved mask
  unsigned char   DirectColorModeInfo;   // direct color mode attributes BIT0: Color Ramp Programmable

  // Mandatory information for VBE 2.0 and above:

  unsigned long   PhysBasePtr;           // physical address for flat frame buffer
  unsigned long   OffScreenMemOffset;    // pointer to start of off screen memory
  unsigned short  OffScreenMemSize;      // amount of off screen memory in 1k units
  // VBE3.0
  unsigned short  BytesPerScanLineL;     // bytes per scanline in linear modes
  unsigned char   NumberImagesB;         // number of images for banked modes
  unsigned char   NumberImagesL;         // number of images for linear modes
  unsigned char   RedMaskSizeL;          // size of direct color red mask in bits(linear)
  unsigned char   RedFieldPositionL;     // bit position of lsb of red mask(linear)
  unsigned char   GreenMaskSizeL;        // size of direct color green mask in bits(linear)
  unsigned char   GreenFieldPositionL;   // bit position of lsb of green mask(linear)
  unsigned char   BlueMaskSizeL;         // size of direct color blue mask in bits(linear)
  unsigned char   BlueFieldPositionL;    // bit position of lsb of blue mask(linear)
  unsigned char   RsvdMaskSizeL;         // size of direct color reserved mask in bits(linear)
  unsigned char   RsvdFieldPositionL;    // bit position of lsb of reserved mask(linear)
  unsigned long   MaxPixelClock;         // Maximum pixel clock for graphics video mode (Hertz)
  char                  __Reserved[190];

};

#endif

struct videoinfo {
    struct videoconfig      vc;
    gr_device _FARD         *deviceptr;
    struct clip_region      clip;
    struct clip_region      clip_def;
    unsigned short          screen_seg;
    unsigned int            screen_off;
    unsigned short          screen_seg_base;
    unsigned int            screen_off_base;
    unsigned short          page_size;
    unsigned short          misc_info;
    unsigned short          stride;
#if defined( VERSION2 )
    grcolor                 pixel_mask;
    short                   bytes_per_pixel;
    struct VbeModeInfo      mi;
#endif
};

#define PLANAR          0x0001      // videoinfo.misc_info bits
#define NO_BIOS         0x0002

#define               _DEFAULT_ATTR   0x07    // default text attribute
#define               IsTextMode      ( !_GrMode )

struct window_def {
    short           invert;
    float           xleft;
    float           ybottom;
    float           xright;
    float           ytop;
};
#pragma pack (pop);

#if defined( _DEFAULT_WINDOWS )
    // This macro causes repaints to be done
    #define _RefreshWindow()    _wpi_updatewindow( _CurrWin ); _MessageLoop( TRUE );
#else
    #define _RefreshWindow()
    enum {
            FALSE, TRUE
    };
#endif

#define SOLID_LINE      0xffff  /* linestyle for solid lines */

enum {                          /* clipping indicator */
        _GCLIPOFF, _GCLIPON
};

enum {                          /* font type indicator  */
        _BITMAPPED, _STROKE
};

enum {                          /* scrolling directions */
        _SCROLL_UP = 1, _SCROLL_DOWN
};

#define roundoff( x )   ( ( x >= 0 ) ? 0.5F : -0.5F )

/* map logical coordinates to physical coordinates  */

#define _GetPhysX( x )  ( x + _LogOrg.xcoord )
#define _GetPhysY( y )  ( y + _LogOrg.ycoord )

/* map viewport coordinates to physical coordinates */

#define _VtoPhysX( x )  ( x + _LogOrg.xcoord )
#define _VtoPhysY( y )  ( y + _LogOrg.ycoord )

/* map physical coordinates to logical coordinates  */

#define _GetLogX( x )  ( x - _LogOrg.xcoord )
#define _GetLogY( y )  ( y - _LogOrg.ycoord )

/* map window coordinates to physical coordinates   */

extern short            _WtoPhysX( double );
extern short            _WtoPhysY( double );
extern short            _WtoScaleX( double );
extern short            _WtoScaleY( double );
extern void             _resetscalefactor( void );

extern void             _CalcNumPages( void );
extern short            _CharLen( char );
extern short            _CnvColour( long );
extern void             _CursorOff( void );
extern void             _CursorOn( void );
extern short            _FastMap( long _WCI86FAR *, short );
extern void             _RemapNum( long _WCI86FAR *, short );
extern void             _InitSegments( void );
extern void             _InitState( void );
extern void             _FiniDevice( void );
extern unsigned         _GetStackLow( void );
extern short            _GraphMode( void );
extern void             _GrClear( short, short, short, short );
extern void             _TxtClear( short, short, short, short );
extern void             _GrInit( short, short, short, grcolor, short,
                                     short, short, int, short, short );
extern short            _GrProlog( void );
extern void             _GrEpilog( void );
extern void             _GetState( void );
extern void             _HershDraw( char, short, short, short, short, short, short );
extern short            _InitDevice( short );
extern short            _Line( short, short, short, short );
extern void             _LineInit( short, short, short, short, struct line_entry * );
extern void             _LineMove( struct line_entry * );
extern float            _NewtonSqrt( float );
extern void             _PaletteInit( void );
extern void             _PutChar( short, short, short );
extern void             _StartDevice( void );
extern void             _ResetDevice( void );
extern short            _RowLen( short );
extern void             _ScrollWindow( short, short );
extern short            _SetMode( short );
extern short            _SetRows( short );
extern short            _SwapBits( short );
extern short            _SysMonType( void );
extern short            _SuperVGAType( void );

extern short            _L0BlockClip( short *, short *, short *, short * );
extern void             _L0DrawLine( char far *, grcolor, unsigned short, short,
                                         short, short, move_fn *, move_fn *, putdot_fn * );
extern void             _L0Ellipse( short, short, short, short, void (*)( short, short, short ) );
extern short            _L0LineClip( short *, short *, short *, short * );

extern void             _L1Arc( short, short, short, short, short,
                                    short, short, short, short );
extern void             _L1Block( short, short, short, short );
extern void             _L1Ellipse( short, short, short, short, short );
extern void             _L1Fill( short, short, short );
extern void             _L1ClipFill( short, short, short );
extern short            _L1FillArea( short, struct xycoord _WCI86FAR * );
extern grcolor          _L1GetDot( short, short );
extern void             _L1GetPic( short, short, short, short, struct picture _WCI86HUGE * );
extern short            _L1Line( short, short, short, short );
extern char             _L1OutCode( short, short );
extern short            _L1Paint( grcolor, short, short );
extern grcolor          _L1PutDot( short, short );
extern void             _L1PutPic( short, short, short, struct picture _WCI86HUGE * );
extern void             _L1SLine( short, short, short, short );
extern void             _L1Text( short, short, char _WCI86FAR * );
extern void             _L1TXX( short, short, char _WCI86FAR *,
                                    struct xycoord _WCI86FAR *, struct xycoord _WCI86FAR * );

extern short            _WCI86FAR _L2arc( short, short, short, short, short, short, short, short );
extern void             _WCI86FAR _L2clearscreen( short );
extern short            _WCI86FAR _L2ellipse( short, short, short, short, short );
extern short            _WCI86FAR _L2floodfill( short, short, grcolor );
extern short            _WCI86FAR _L2line( short, short, short, short );
extern short            _WCI86FAR _L2pie( short, short, short, short, short, short, short, short, short );
extern short            _WCI86FAR _L2polygon( short, short, struct xycoord _WCI86FAR * );
extern short            _WCI86FAR _L2rectangle( short, short, short, short, short );
extern void             _WCI86FAR _L2getimage( short, short, short, short, char _WCI86HUGE * );
extern void             _WCI86FAR _L2putimage( short, short, char _WCI86HUGE *, short );
extern long             _WCI86FAR _L2imagesize( short, short, short, short );
extern short            _WCI86FAR _L2grtext( short, short, char _WCI86FAR * );
extern void             _WCI86FAR _L2setcliprgn( short, short, short, short );
extern short            _WCI86FAR _L2setclip( short );
extern grcolor          _WCI86FAR _L2setcolor( grcolor );
