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
* Description:  Graph library globals.
*
****************************************************************************/


#include "gdefn.h"

/* External Variable Definitions */

char                    _StartUp = 1;               // graphics startup flag
char                    _GrMode = 0;                // graphics mode flag
short                   _ErrorStatus = _GROK;       // error status

struct xycoord          _CurrPos = { 0, 0 };        // current logical position
struct xycoord          _LogOrg = { 0, 0 };         // logical origin location

char                    _Clipping = _GCLIPON;       // clipping indicator

short                   _Tx_Row_Min;                // upper left and lower right
short                   _Tx_Col_Min;                // corners of text window
short                   _Tx_Row_Max;
short                   _Tx_Col_Max;

struct rccoord          _TextPos;                   // dummy text postion
#if defined( VERSION2 )
grcolor                 _CharAttr = _DEFAULT_ATTR;  // current text attribute
#else
char                    _CharAttr = _DEFAULT_ATTR;  // current text attribute
#endif
short                   _Wrap = _GWRAPON;           // text wrap flag
short                   _CursState = _GCURSORON;    // cursor state flag
short                   _CursorShape;               // cursor shape
short                   _GrCursor = 0;              // graphics cursor

short                   _CurrActivePage = 0;        // current active page
short                   _CurrVisualPage = 0;        // current display page
short                   _DefMode;                   // default video mode
short                   _DefTextRows;               // default # of text rows

long                    _CurrBkColor = 0;           // current background colour
grcolor                 _CurrColor = 0;             // current colour number
short                   _Palette = 3;               // current palette number

unsigned char           _FillMask[ MASK_LEN ] =     // current fill mask
                            { 0xff, 0xff, 0xff, 0xff,
                              0xff, 0xff, 0xff, 0xff };
unsigned char           _DefMask[ MASK_LEN ] =      // default fill mask
                            { 0xff, 0xff, 0xff, 0xff,
                              0xff, 0xff, 0xff, 0xff };
char                    _HaveMask = 0;              // is there a fill mask?
short                   _Transparent = 1;           // fill transparency
short                   _PaRf_x = 0;                // pattern reference x-coord
short                   _PaRf_y = 0;                // pattern reference y-coord
unsigned short          _LineStyle = SOLID_LINE;    // current line style
short                   _StyleWrap = 0;             // flag for continuous style
short                   _PlotAct = 0;               // plotting action

struct textsettings     _TextSettings =             // default text settings
                            { 1, 0, _PATH_RIGHT, 0,
                              0, 0, _NORMAL, _NORMAL };
extern font_def _WCI86FAR    _StdFont;                   // standard font
font_def _WCI86FAR           *_CurrFont = &_StdFont;     // current font

struct arcinfo          _ArcInfo =                  // arc information
                            { { 0, 0 }, { 0, 0 }, { 0, 0 } };

struct _wxycoord        _CurrPos_w =                // current window position
                            { 0.0, 0.0 };
struct window_def       _Window =                   // default window def'n
                            { TRUE, 0.0, 0.0, 1.0, 1.0 };
SCREEN_DESC             _Screen;                    // screen description

unsigned char           _IsDBCS;                    // is this a DBCS system
dbcs_pair               _DBCSPairs[ 5 ];            // range of DBCS chars

#if defined( VERSION2 )
long                    _coltbl[16];                // EGA colors for TC modes
#endif

#if defined( _SUPERVGA )
unsigned char           _VGAPage;                   // SuperVGA page number
#if defined( VERSION2 )
unsigned short          _VGABytesPerPixel;          // SuperVGA num bytes per pixel
#endif
unsigned char           _VGAGran;                   // SuperVGA page granularity
short                   _SVGAType;                  // type of SuperVGA
void                    ( _FARC *_SetVGAPage )( short );  // function to set SVGA page
short                   _VGAStride;                 // SuperVGA mode stride
#endif

// segments of screen memory and BIOS and ROM BIOS data area

#if defined ( __386__ )
    unsigned short      _BiosSeg = 0x0034;          // seg of BIOS data area
    unsigned short      _MonoSeg = 0x001C;          // seg of MONO screen
    unsigned short      _CgaSeg  = 0x001C;          // seg of CGA screen
    unsigned short      _EgaSeg  = 0x001C;          // seg of EGA/VGA screen
    unsigned short      _RomSeg  = 0x0034;          // seg of ROM BIOS area
    unsigned int        _BiosOff = 0x00000400;      // off of BIOS data area
    unsigned int        _MonoOff = 0x00000000;      // off of MONO screen
    unsigned int        _CgaOff  = 0x00000000;      // off of CGA screen
    unsigned int        _EgaOff  = 0x00000000;      // off of EGA/VGA screen
    unsigned int        _RomOff  = 0x000C0000;      // off of ROM BIOS area
    unsigned short      _StackSeg;                  // seg of stack
#else
    unsigned short      _BiosSeg = 0x0040;          // seg of BIOS data area
    unsigned short      _MonoSeg = 0xB000;          // seg of MONO screen
    unsigned short      _CgaSeg  = 0xB800;          // seg of CGA screen
    unsigned short      _EgaSeg  = 0xA000;          // seg of EGA/VGA screen
    unsigned short      _RomSeg  = 0xC000;          // seg of ROM BIOS area
    unsigned int        _BiosOff = 0x0000;          // off of BIOS data area
    unsigned int        _MonoOff = 0x0000;          // off of MONO screen
    unsigned int        _CgaOff  = 0x0000;          // off of CGA screen
    unsigned int        _EgaOff  = 0x0000;          // off of EGA/VGA screen
    unsigned int        _RomOff  = 0x0000;          // off of ROM BIOS area
    unsigned short      _StackSeg;                  // seg of stack
#endif

#if defined( __QNX__ )
    unsigned short      _CompileBuf;                // seg for compiled line drawing function
    unsigned short      _CompileSeg;                // executable shadow for CompileBuf
#endif

#if defined( _CurrState )
    #undef _CurrState
#endif

struct videoinfo        _ConfigBuffer;              // video state
struct videoinfo _FARD  *_CurrState = &_ConfigBuffer;

