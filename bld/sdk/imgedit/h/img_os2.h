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


#define INCL_PM
#define INCL_WINFRAMEMGR
#define INCL_NLS
#define INCL_GPI
#define INCL_GPIBITMAPS
#define INCL_ERRORS
#define INCL_DOSMODULEMGR
#define INCL_WIN
#include <os2.h>

#include "wbitmap.h"

#define RETURN_SUCCESS            0      /* successful return in DosExit */
#define RETURN_ERROR              1           /* error return in DosExit */
#define BEEP_WARN_FREQ           60         /* frequency of warning beep */
#define BEEP_WARN_DUR           100          /* duration of warning beep */
#define IDMSG_MAINWINCREATEFAILED            302

#define IMGED_MAIN              1
#define IMGED_CLR_PAL           2
#define IMGED_CLR_CUR           3

#define IMGED_FILE              1000
#define IMGED_EDIT              1001
#define IMGED_SHIFT             1002
#define IMGED_FLIP              1003
#define IMGED_ROTATE            1004
#define IMGED_OPTIONS           1005
#define IMGED_BS                1006
#define IMGED_PAL               1007
#define IMGED_TOOLS             1008
#define IMGED_ELLIPSE           1009
#define IMGED_RECTANGLE         1010
#define IMGED_WINDOWS           1011
#define IMGED_HELP              2065

#define SELECTIMAGE             1
#define ICONTYPE                2
#define BITMAPTYPE              3
#define CURSORTYPE              4
#define IMAGESIZE               5
#define CURRENT_SETTINGS        6
#define SELBKCOLOUR             9

#define ICON_ID                 1
#define MENU_ID                 1
#define POINT_CUR               10
#define PENCILCURSOR            11
#define CROSSHAIRSCUR           12
#define FILLCURSOR              13
#define PAINTBRUSHCUR           14
#define HOTSPOTCUR              15

#define PENCIL                  100
#define PENCILD                 101
#define LINE                    102
#define LINED                   103
#define CIRCLEO                 104
#define CIRCLEOD                105
#define CIRCLEF                 106
#define CIRCLEFD                107
#define RECTO                   108
#define RECTOD                  109
#define RECTF                   110
#define RECTFD                  111
#define FILL                    112
#define FILLD                   113
#define BRUSH                   114
#define BRUSHD                  115
#define CLPRECT                 116
#define CLPRECTD                117
#define HOTSPOT                 118
#define HOTSPOTD                119

#define OPENBMP                 200
#define NEWBMP                  201
#define SAVEBMP                 202
#define GRIDBMP                 203
#define GRIDDBMP                204
#define MAXIMIZEBMP             205
#define MAXIMIZEDBMP            206
#define CUTBMP                  207
#define COPYBMP                 208
#define PASTEBMP                209
#define UNDOBMP                 210
#define REDOBMP                 211
#define CLEARBMP                212
#define SNAPBMP                 213
#define RIGHTBMP                214
#define LEFTBMP                 215
#define UPBMP                   216
#define DOWNBMP                 217
#define HFLIPBMP                218
#define HFLIPDBMP               219
#define VFLIPBMP                220
#define VFLIPDBMP               221
#define CLROTBMP                222
#define CLROTDBMP               223
#define CCROTBMP                224
#define CCROTDBMP               225

#define DKGRAY          0x808080
#define LTGRAY          0xC0C0C0
#define WHITE           0xFFFFFF
#define BLACK           0x000000
#define BK_WHITE        0xFFFFFF
