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
#include "_graph.gh"

#pragma pack(push, 1);

#if defined ( __386__ )
    #if defined( __QNX__ )
        // QNX uses flat, not small model
        // generated line code is in a different segment
        #define _FARC   __far
    #else
        #define _FARC
    #endif
#else
    #define _FARC       __far
#endif

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

extern short                _WtoPhysX( double );
extern short                _WtoPhysY( double );
extern short                _WtoScaleX( double );
extern short                _WtoScaleY( double );
extern void                 _resetscalefactor( void );

extern void                 _CalcNumPages( void );
extern short                _CharLen( char );
extern short                _CnvColour( long );
extern void                 _CursorOff( void );
extern void                 _CursorOn( void );
extern short                _FastMap( long _WCI86FAR *, short );
extern void                 _RemapNum( long _WCI86FAR *, short );
extern void                 _InitSegments( void );
extern void                 _InitState( void );
extern void                 _FiniDevice( void );
extern unsigned             _GetStackLow( void );
extern short                _GraphMode( void );
extern void                 _GrClear( short, short, short, short );
extern void                 _TxtClear( short, short, short, short );
extern void                 _GrInit( short, short, short, short, short,
                                     short, short, int, short, short );
extern short                _GrProlog( void );
extern void                 _GrEpilog( void );
extern void                 _GetState( void );
extern void                 _HershDraw( char, short, short, short, short, short, short );
extern short                _InitDevice( short );
extern short                _Line( short, short, short, short );
extern void                 _LineInit( short, short, short, short, struct line_entry * );
extern void                 _LineMove( struct line_entry * );
extern float                _NewtonSqrt( float );
extern void                 _PaletteInit( void );
extern void                 _PutChar( short, short, short );
extern void                 _StartDevice( void );
extern void                 _ResetDevice( void );
extern short                _RowLen( short );
extern void                 _ScrollWindow( short, short );
extern short                _SetMode( short );
extern short                _SetRows( short );
extern short                _SwapBits( short );
extern short                _SysMonType( void );
extern short                _SuperVGAType( void );

extern short                _L0BlockClip( short *, short *, short *, short * );
extern void                 _L0DrawLine( char far *, short, unsigned short, short,
                                         short, short, void (near *)(),
                                         void (near *)(), void (near *)() );
extern void                 _L0Ellipse( short, short, short, short, void (*)( short, short, short ) );
extern short                _L0LineClip( short *, short *, short *, short * );

extern void                 _L1Arc( short, short, short, short, short,
                                    short, short, short, short );
extern void                 _L1Block( short, short, short, short );
extern void                 _L1Ellipse( short, short, short, short, short );
extern void                 _L1Fill( short, short, short );
extern void                 _L1ClipFill( short, short, short );
extern short                _L1FillArea( short, struct xycoord _WCI86FAR * );
extern short                _L1GetDot( short, short );
extern void                 _L1GetPic( short, short, short, short, struct picture _WCI86HUGE * );
extern short                _L1Line( short, short, short, short );
extern char                 _L1OutCode( short, short );
extern short                _L1Paint( short, short, short );
extern short                _L1PutDot( short, short );
extern void                 _L1PutPic( short, short, short, struct picture _WCI86HUGE * );
extern void                 _L1SLine( short, short, short, short );
extern void                 _L1Text( short, short, char _WCI86FAR * );
extern void                 _L1TXX( short, short, char _WCI86FAR *,
                                    struct xycoord _WCI86FAR *, struct xycoord _WCI86FAR * );

extern short _WCI86FAR           _L2arc( short, short, short, short, short, short, short, short );
extern void _WCI86FAR            _L2clearscreen( short );
extern short _WCI86FAR           _L2ellipse( short, short, short, short, short );
extern short _WCI86FAR           _L2floodfill( short, short, short );
extern short _WCI86FAR           _L2line( short, short, short, short );
extern short _WCI86FAR           _L2pie( short, short, short, short, short, short, short, short, short );
extern short _WCI86FAR           _L2polygon( short, short, struct xycoord _WCI86FAR * );
extern short _WCI86FAR           _L2rectangle( short, short, short, short, short );
extern void _WCI86FAR            _L2getimage( short, short, short, short, char _WCI86HUGE * );
extern void _WCI86FAR            _L2putimage( short, short, char _WCI86HUGE *, short );
extern long _WCI86FAR            _L2imagesize( short, short, short, short );
extern short _WCI86FAR           _L2grtext( short, short, char _WCI86FAR * );
extern void _WCI86FAR            _L2setcliprgn( short, short, short, short );
extern short _WCI86FAR           _L2setclip( short );
extern short _WCI86FAR           _L2setcolor( short );

#pragma pack (pop);
