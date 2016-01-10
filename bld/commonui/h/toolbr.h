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
* Description:  Toolbar interface.
*
****************************************************************************/


#ifndef _TOOLBR_H_INCLUDED
#define _TOOLBR_H_INCLUDED

#include "wpi.h"

typedef int     CMDID;

typedef bool (*toolhook_fn)( HWND, WPI_MSG, WPI_PARAM1, WPI_PARAM2 );
typedef void (*helphook_fn)( HWND, int, bool );

typedef struct TOOLDISPLAYINFO {
    WPI_POINT   button_size;        /* size, in pixels, of one tool item */
    WPI_POINT   border_size;        /* width/height, in pixels, of border around tools */
    WPI_RECT    area;               /* area of window in units appropriate to style */
    DWORD       style;              /* style of toolbar window */
    toolhook_fn hook;               /* function called before toolbar window proc */
    helphook_fn helphook;           /* function called when help text is needed */
    HBITMAP     background;         /* background of depressed button (0 == default) */
    HBRUSH      foreground;         /* color of mono bitmap when depressed (0 == default) */
    bool        is_fixed    : 1;    /* is toolbar fixed or floating? */
    bool        use_tips    : 1;    /* use tool tips? */
} TOOLDISPLAYINFO;

/* Button states for use with ITEM_STICKY flag below */
#define BUTTON_UP       0x00
#define BUTTON_DOWN     0x01

/* Item flags */
#define ITEM_STICKY     0x01    /* item is sticky, i.e. stays down when clicked */
#define ITEM_DOWNBMP    0x02
#define ITEM_BLANK      0x04

/* Maximum tool tip length */
#define MAX_TIP         128

typedef struct TOOLITEMINFO {
    union {
        HBITMAP bmp;            /* handle to bitmap to display */
        WORD    blank_space;    /* space if item is blank */
    } u;
    CMDID       id;             /* should be unique for each item */
    WORD        flags;          /* see list of flags above */
    HBITMAP     depressed;      /* bitmap to show when button is depressed */
    char        tip[MAX_TIP];   /* tool tip string */
} TOOLITEMINFO;

struct toolbar  *ToolBarInit( HWND );
void    ToolBarDisplay( struct toolbar *, TOOLDISPLAYINFO * );
void    ToolBarAddItem( struct toolbar *, TOOLITEMINFO * );
bool    ToolBarDeleteItem( struct toolbar *, CMDID id );
HWND    ToolBarWindow( struct toolbar * );
void    ToolBarSetState( struct toolbar *, CMDID id, WORD state );
WORD    ToolBarGetState( struct toolbar *bar, CMDID id );
void    ToolBarDestroy ( struct toolbar *bar );
void    ToolBarFini( struct toolbar * );
void    ToolBarDrawBitmap( WPI_PRES pres, WPI_POINT size, WPI_POINT org, HBITMAP bitmap );
void    UpdateToolBar( struct toolbar *bar );
void    ChangeToolButtonBitmap( struct toolbar *bar, CMDID id, HBITMAP newbmp );
bool    HasToolAtPoint( struct toolbar *bar, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
bool    FindToolIDAtPoint( struct toolbar *bar, WPI_PARAM1 wparam, WPI_PARAM2 lparam, CMDID *id );
#ifndef __OS2_PM__
void    ToolBarChangeSysColors( COLORREF, COLORREF, COLORREF );
#endif
void    ToolBarRedrawButtons( struct toolbar *bar );

#if defined( __NT__ ) || defined( __WINDOWS__ )
void    TB_TransparentBlt( HDC hDC, UINT x, UINT y, UINT width, UINT height, HDC hDCIn, COLORREF cr );
#endif

#ifndef __OS2_PM__
    #define TOOLBAR_FIXED_STYLE         (WS_BORDER | WS_CHILDWINDOW)
    #define TOOLBAR_FLOAT_STYLE         (WS_CAPTION | WS_POPUP | WS_THICKFRAME | WS_SYSMENU)
    #define TOOLBAR_FLOATNOSIZE_STYLE   (WS_CAPTION | WS_POPUP | WS_SYSMENU)
#else
    #define TOOLBAR_FIXED_STYLE         (FCF_BORDER)
    #define TOOLBAR_FLOAT_STYLE         (FCF_TITLEBAR | FCF_SIZEBORDER | FCF_SYSMENU)
    #define TOOLBAR_FLOATNOSIZE_STYLE   (FCF_TITLEBAR | FCF_BORDER | FCF_SYSMENU)
#endif

#endif /* _TOOLBR_H_INCLUDED */
