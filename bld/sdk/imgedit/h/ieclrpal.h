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


#define PALETTE_SIZE    28

/* Dimensions of the Colour Palette Window */
#define CP_WIDTH        418     /* Initial window horizontal size */
#define CP_HEIGHT       60      /* Initial window vertical size */

typedef struct {
    wie_clrtype type;           /* NORMAL_CLR, INVERSE_CLR, or SCREEN_CLR */
    COLORREF    colour;
    COLORREF    solid;          /* for the solid colour */
    HBITMAP     bitmap;
} selected_colour;

/* Definitions for Current Selection */
#define CUR_WND_X       44
#define CUR_WND_Y       10
#define CUR_WND_WIDTH   42
#define CUR_WND_HEIGHT  37
#define CUR_SQR_SIZE    18      /* Size of the sample squares */
#define CUR_LCOL_X      0       /* Position of left button display colour */
#define CUR_COL_Y       0
#define CUR_RCOL_X      23

/* Definitions for Available Colours */
#define SQR_SIZE        17      /* Size of a palette square */
#define COL_WND_X       170
#define COL_WND_Y       10
#define COL_WND_WIDTH   SQR_SIZE*14 + 1
#define COL_WND_HEIGHT  37

#define FIRST_SQR_X     0       /* Position of first square (position of */
#define FIRST_SQR_Y     0       /* other square are dependent on the first) */

/* Definitions for the Screen and Inverse Colours */
#define SCRN_WND_X      145
#define SCRN_WND_Y      10
#define SCRN_WND_WIDTH  SQR_SIZE+1
#define SCRN_WND_HEIGHT 37
#define SCRN_COL_X      0
#define SCRN_COL_Y      0

/* Definitions for the static text windows */
#define LINE1Y          12      /* Position of the first line of text */
#define LINE2Y          30      /* Position of the second line of text */
#define TEXTHEIGHT      13      /* The height of a line of text */
#define LINE1X          93
#define LINE_WIDTH      47
#define TEXT1X          5
#define TEXT1Y          12
#define TEXT2Y          33
#define TEXT_WIDTH      35

#define CUR_BMP_WIDTH   239
#define CUR_BMP_HEIGHT  35

/* definitions for the window classes */
#define CURRENT_CLASS   "CurrentClass"
#define AVAIL_CLASS     "ColoursClass"
#define SCREEN_CLASS    "ScreenClass"
