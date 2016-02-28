/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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


typedef bool (*toolhook_fn)( HWND, WPI_MSG, WPI_PARAM1, WPI_PARAM2 );
typedef void (*helphook_fn)( HWND, ctl_id, bool );

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
    ctl_id      id;             /* should be unique for each item */
    WORD        flags;          /* see list of flags above */
    HBITMAP     depressed;      /* bitmap to show when button is depressed */
    char        tip[MAX_TIP];   /* tool tip string */
} TOOLITEMINFO;

typedef struct tool {
    struct tool *next;
    union {
        HBITMAP bitmap;
        WORD    blank_space;
    } u;
    HBITMAP     depressed;
    ctl_id      id;
    UINT        flags;
    WORD        state;
    WPI_RECT    area;
#ifdef __NT__
    char        tip[MAX_TIP];
#endif
} tool;

typedef struct toolbar {
    HWND        hwnd;
    HWND        owner;
    toolhook_fn hook;
    helphook_fn helphook;
    WPI_POINT   button_size;
    WPI_POINT   border;
    HBITMAP     background;
    HBRUSH      foreground;
    HBRUSH      bgbrush;
    int         border_width;
    tool        *tool_list;
    char        is_fixed    : 1;
    char        spare       : 7;
#ifdef __NT__
    HWND        container;
    WNDPROC     old_wndproc;
    HWND        tooltips;
#endif
} toolbar;

toolbar *ToolBarInit( HWND );
void    ToolBarDisplay( toolbar *, TOOLDISPLAYINFO * );
void    ToolBarAddItem( toolbar *, TOOLITEMINFO * );
bool    ToolBarDeleteItem( toolbar *, ctl_id id );
HWND    ToolBarWindow( toolbar * );
void    ToolBarSetState( toolbar *, ctl_id id, WORD state );
WORD    ToolBarGetState( toolbar *bar, ctl_id id );
void    ToolBarDestroy ( toolbar *bar );
void    ToolBarFini( toolbar * );
void    ToolBarDrawBitmap( WPI_PRES pres, WPI_POINT size, WPI_POINT org, HBITMAP bitmap );
void    UpdateToolBar( toolbar *bar );
void    ChangeToolButtonBitmap( toolbar *bar, ctl_id id, HBITMAP newbmp );
bool    HasToolAtPoint( toolbar *bar, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
bool    FindToolIDAtPoint( toolbar *bar, WPI_PARAM1 wparam, WPI_PARAM2 lparam, ctl_id *id );
#ifndef __OS2_PM__
void    ToolBarChangeSysColors( COLORREF, COLORREF, COLORREF );
#endif
void    ToolBarRedrawButtons( toolbar *bar );

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
