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


#include "win1632.h"

#define SELECTIMAGE             "SELECTIMAGE"
#define ICONTYPE                "ICONTYPE"
#define BITMAPTYPE              "BITMAPTYPE"
#define CURSORTYPE              "CURSORTYPE"
#define IMAGESIZE               "IMAGESIZE"
#define SELBKCOLOR              "SELBKCOLOR"
#define CURRENT_SETTINGS        "CURRENT_SETTINGS"

#define POINT_CUR               "PointCursor"
#define PENCILCURSOR            "PencilCursor"
#define CROSSHAIRSCUR           "CrossHairs"
#define FILLCURSOR              "FillCursor"
#define PAINTBRUSHCUR           "PaintBrush"
#define HOTSPOTCUR              "HotSpotCur"

#define PENCIL                  "PENCIL"
#define PENCILD                 "PENCILD"
#define LINE                    "LINE"
#define LINED                   "LINED"
#define CIRCLEO                 "CIRCLEO"
#define CIRCLEOD                "CIRCLEOD"
#define CIRCLEF                 "CIRCLEF"
#define CIRCLEFD                "CIRCLEFD"
#define RECTO                   "RECTO"
#define RECTOD                  "RECTOD"
#define RECTF                   "RECTF"
#define RECTFD                  "RECTFD"
#define FILL                    "FILL"
#define FILLD                   "FILLD"
#define BRUSH                   "BRUSH"
#define BRUSHD                  "BRUSHD"
#define CLPRECT                 "CLPRECT"
#define CLPRECTD                "CLPRECTD"
#define HOTSPOT                 "HOTSPOT"
#define HOTSPOTD                "HOTSPOTD"

#define OPENBMP                 "OPENBMP"
#define NEWBMP                  "NEWBMP"
#define SAVEBMP                 "SAVEBMP"
#define GRIDBMP                 "GRIDBMP"
#define GRIDDBMP                "GRIDDBMP"
#define MAXIMIZEBMP             "MAXIMIZEBMP"
#define MAXIMIZEDBMP            "MAXIMIZEDBMP"
#define CUTBMP                  "CUTBMP"
#define COPYBMP                 "COPYBMP"
#define PASTEBMP                "PASTEBMP"
#define UNDOBMP                 "UNDOBMP"
#define REDOBMP                 "REDOBMP"
#define CLEARBMP                "CLEARBMP"
#define SNAPBMP                 "SNAPBMP"
#define RIGHTBMP                "RIGHTBMP"
#define LEFTBMP                 "LEFTBMP"
#define UPBMP                   "UPBMP"
#define DOWNBMP                 "DOWNBMP"
#define HFLIPBMP                "HFLIPBMP"
#define HFLIPDBMP               "HFLIPDBMP"
#define VFLIPBMP                "VFLIPBMP"
#define VFLIPDBMP               "VFLIPDBMP"
#define CLROTBMP                "CLROTBMP"
#define CLROTDBMP               "CLROTDBMP"
#define CCROTBMP                "CCROTBMP"
#define CCROTDBMP               "CCROTDBMP"

#if defined ( __NT__ )
    #define DKGRAY          GetSysColor( COLOR_BTNSHADOW )
    #define LTGRAY          GetSysColor( COLOR_BTNFACE )
#else
    #define DKGRAY          RGB( 0x80, 0x80, 0x80 )
    #define LTGRAY          RGB( 0xC0, 0xC0, 0xC0 )
#endif
#define BLACK           RGB( 0x00, 0x00, 0x00 )
#define BK_WHITE        RGB( 0xFF, 0xFF, 0xFF )
#define WHITE           RGB( 0xFF, 0xFF, 0xFF )
#define CLR_BLACK       BLACK
#define CLR_PALEGRAY    LTGRAY
#define CLR_DARKGRAY    DKGRAY
#define CLR_WHITE       WHITE
