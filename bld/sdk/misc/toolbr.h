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


/*
 * Caveats: monochrome and colour bitmaps are drawn differently. When
 * a monochrome bitmap is pressed down, it is drawn with the BITMAP given
 * in the TOOLDISPLAYINFO structure as a background brush. If you are
 * using colour bitmaps or don't care, just fill in 0 for the background.
 *  However, I believe monochrome bitmaps look much better, so try and
 * use those. For am example, see the VI toolbar in the default configuration.
 * Half of the bitmaps are colour, and the other half are monochrome (accident?).
 */

typedef BOOL (*toolhook)( HWND, unsigned, UINT, LONG );
typedef void (*helphook)( HWND, UINT, BOOL );

typedef struct TOOLDISPLAYINFO {
    POINT       button_size;    // size, in pixels, of one tool item
    POINT       border_size;    // width/height, in pixels, of border around tools
    RECT        area;           // area of window in units appropriate to style
    DWORD       style;          // style of toolbar window
    toolhook    hook;           // function called before TOOLBAR window proc
    helphook    helphook;       // function called when help text is needed
    HBITMAP     background;     // background of depressed button (0 == default)
    // ---- not implemented yet
    HBRUSH      foreground;     // colour of mono-bitmap when depressed (0 == default)
    char        is_fixed:1;     // is toolbar fixed or floating?
    char        spare:7;        // spare bits
} TOOLDISPLAYINFO;

/* Button states for use with ITEM_STICKY flag below */
#define BUTTON_UP       0x00
#define BUTTON_DOWN     0x01

/* Item flags */
#define ITEM_STICKY     0x01    // item is sticky - ie stays down when clicked
#define ITEM_DOWNBMP    0x02
#define ITEM_BLANK      0x04

typedef struct TOOLITEMINFO {
    union {
        HBITMAP bmp;            // handle to bitmap to display
        WORD    blank_space;    // space if item is blank
    };
    WORD        id;             // should be unique for each item
    WORD        flags;          // see list of flags above
    HBITMAP     depressed;      // bitmap to show when button is depressed
} TOOLITEMINFO;

struct toolbar *ToolBarInit( HWND );
void ToolBarDisplay( struct toolbar *, TOOLDISPLAYINFO * );
void ToolBarAddItem( struct toolbar *, TOOLITEMINFO * );
BOOL ToolBarDeleteItem( struct toolbar *, WORD id );
HWND ToolBarWindow( struct toolbar * );
void ToolBarSetState( struct toolbar *, WORD id, WORD state );
WORD ToolBarGetState( struct toolbar *bar, WORD id );
void ToolBarDestroy ( struct toolbar *bar );
void ToolBarFini( struct toolbar * );
//void ToolBarDrawBitmap( HDC hdc, POINT size, POINT org, HBITMAP bitmap );
void UpdateToolBar( struct toolbar *bar );
void ChangeToolButtonBitmap( struct toolbar *bar, int id, HBITMAP new );
BOOL HasToolAtPoint( struct toolbar *bar, LONG lparam );
BOOL FindToolIDAtPoint( struct toolbar *bar, LPARAM lparam, UINT *id );
void ToolBarChangeSysColors( COLORREF, COLORREF, COLORREF );
void ToolBarRedrawButtons( struct toolbar *bar );

#define TOOLBAR_FIXED_STYLE     (WS_BORDER|WS_CHILDWINDOW)
#define TOOLBAR_FLOAT_STYLE     (WS_CAPTION|WS_POPUP|WS_THICKFRAME|WS_SYSMENU)
