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


extern char             _StartUp;                   // graphics startup flag
extern char             _GrMode;                    // graphics mode flag
extern short            _ErrorStatus;               // error status

extern struct xycoord   _CurrPos;                   // current logical position
extern struct xycoord   _LogOrg;                    // logical origin location

extern char             _Clipping;                  // clipping indicator

extern short            _Tx_Row_Min;                // upper left and lower right
extern short            _Tx_Col_Min;                // corners of text window
extern short            _Tx_Row_Max;
extern short            _Tx_Col_Max;

extern struct rccoord   _TextPos;                   // dummy text postion
extern char             _CharAttr;                  // current text attribute
extern short            _Wrap;                      // text wrap flag
extern short            _CursState;                 // cursor state flag
extern short            _CursorShape;               // cursor shape
extern short            _GrCursor;                  // graphics cursor

extern short            _CurrActivePage;            // current active page
extern short            _CurrVisualPage;            // current display page
extern short            _DefMode;                   // default video mode
extern short            _DefTextRows;               // default # of text rows

extern long             _CurrBkColor;               // current background colour
extern short            _CurrColor;                 // current colour number
extern short            _Palette;                   // current palette number

extern unsigned char    _FillMask[];                // current fill mask
extern unsigned char    _DefMask[];                 // default fill mask
extern char             _HaveMask;                  // is there a fill mask?
extern short            _Transparent;               // fill transparency
extern short            _PaRf_x;                    // pattern reference x-coord
extern short            _PaRf_y;                    // pattern reference y-coord
extern unsigned short   _LineStyle;                 // current line style
extern short            _StyleWrap;                 // flag for continuous style
extern short            _PlotAct;                   // plotting action

extern struct textsettings _TextSettings;           // default text settings
extern font_def _WCI86FAR    _StdFont;                   // standard font
extern font_def _WCI86FAR    *_CurrFont;                 // current font

extern struct arcinfo   _ArcInfo;                   // arc information

extern struct _wxycoord _CurrPos_w;                 // current window position
extern struct window_def _Window;                   // default window def'n
extern SCREEN_DESC      _Screen;                    // screen description
extern long             _VGA_Colours[ 16 ];         // VGA to EGA mapping

extern unsigned char    _IsDBCS;                    // is this a DBCS system
extern dbcs_pair        _DBCSPairs[];               // range of DBCS chars

extern char             _VGAPage;                   // SuperVGA page number
extern char             _VGAGran;                   // SuperVGA page granularity
extern short            _SVGAType;                  // type of SuperVGA
extern void             ( _FARC *_SetVGAPage )( short );  // function to set SVGA page

#if defined ( _NEC_PC )
extern unsigned short   _BiosSeg;                   // seg of BIOS data area
extern unsigned short   _NecSeg;                    // seg of graphic VRAM
extern unsigned short   _TextSeg;                   // seg of TEXT VRAM
extern unsigned short   _AttrSeg;                   // seg of ATTRIBUTE VRAM
extern unsigned int     _BiosOff;                   // off of BIOS data area
extern unsigned int     _NecOff;                    // offset of graphic VRAM
extern unsigned int     _TextOff;                   // offset of TEXT VRAM
extern unsigned int     _AttrOff;                   // offset of ATTRIBUTE VRAM
extern unsigned short   _StackSeg;                  // seg of stack
extern unsigned short   _GRCGPort;                  // port of graphics charger
  #if defined( __386__ )
    extern struct kanji_buf     _KanjiBuf;
  #endif
#else
extern unsigned short   _BiosSeg;                   // seg of BIOS data area
extern unsigned short   _MonoSeg;                   // seg of MONO screen
extern unsigned short   _CgaSeg;                    // seg of CGA screen
extern unsigned short   _EgaSeg;                    // seg of EGA/VGA screen
extern unsigned short   _RomSeg;                    // seg of ROM BIOS area
extern unsigned short   _StackSeg;                  // seg of stack
extern unsigned int     _BiosOff;                   // off of BIOS data area
extern unsigned int     _MonoOff;                   // off of MONO screen
extern unsigned int     _CgaOff;                    // off of CGA screen
extern unsigned int     _EgaOff;                    // off of EGA/VGA screen
extern unsigned int     _RomOff;                    // off of ROM BIOS area
#endif

#if defined( __QNX__ )
extern unsigned short   _CompileBuf;                // scratch compile area
extern unsigned short   _CompileSeg;                // seg of _CompileBuffer with CODE attributes
#endif

extern struct videoinfo _ConfigBuffer;              // video state

#if defined( _NEC_PC )
extern char             _NECPalette[ 4 ];
extern long             _NECDefPalette[ 16 ];
#endif

#include "curstate.h"
