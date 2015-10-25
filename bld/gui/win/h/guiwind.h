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


#ifndef _GUIWIND_H_
#define _GUIWIND_H_


#ifdef __OS2_PM__
    #define INCL_GPI
    #define INCL_WIN
    #define INCL_DOSNLS
    #define INCL_DOSMISC
    #include <wos2.h>
#else
    #define INCLUDE_COMMDLG_H
    #define NOCOMM
    #define WIN32_LEAN_AND_MEAN
    #include <wwindows.h>
    #include <commdlg.h>
#endif

#include "wpitypes.h"
#include "wpi.h"
#include "guiwpi.h"
#include "guitypes.h"

#include "gui.h"
#include "toolbr.h"
#include "guihint.h"

#define GUI_IS_GUI

/* use 0-9 for os-specific menu constants */

#define GUI_MENU_FONT   0
#define GUI_CHANGE_FONT ( GUI_LAST_MENU_ID + GUI_MENU_FONT + 1 )

typedef struct control_item     control_item;
typedef struct popup_info       popup_info;

typedef enum flags {
    NONE_SET            = 0x0000,
    HRANGE_SET          = 0x0001,
    VRANGE_SET          = 0x0002,
    NEEDS_RESIZE_REDRAW = 0x0004,
    NEEDS_RESIZE_NOTIFY = 0x0008,
    VRANGE_ROW          = 0x0010,
    HRANGE_COL          = 0x0020,
    DOING_CLOSE         = 0x0040,
    SENT_INIT           = 0x0080,
    IS_DIALOG           = 0x0100,
    HAS_CAPTION         = 0x0200,
    IS_MINIMIZED        = 0x0400,
    IS_ROOT             = 0x0800,
    DOING_DESTROY       = 0x1000,
    PARTIAL_ROWS        = 0x2000,
    IS_RES_DIALOG       = 0x4000,
    UTILITY_BIT         = 0x8000,
} gui_flags;

#define GUI_HRANGE_SET( wnd ) ( ( wnd->flags & HRANGE_SET ) != 0 )
#define GUI_VRANGE_SET( wnd ) ( ( wnd->flags & VRANGE_SET ) != 0 )

typedef struct wnd_colour {
    WPI_COLOUR fore;
    WPI_COLOUR back;
} wnd_colour;

typedef struct toolbarinfo {
    struct toolbar      *hdl;
    TOOLDISPLAYINFO     info;
    WPI_RECT            floatrect;
    WPI_RECT            fixedrect;
    int                 num;
    HBITMAP             *bitmaps;
} toolbarinfo;

#define NUMBER_OF_FORCED_REPAINTS 3
typedef struct gui_paint_info {
    WPI_HANDLE          old_bmp;
    WPI_HANDLE          draw_bmp;
    HDC                 compatible_hdc;
    WPI_PRES            compatible_pres;
    WPI_PRES            normal_pres;
    int                 in_use;
    int                 delete_when_done;
    int                 force_count;
} gui_paint_info;

typedef struct gui_window {
    HWND                hwnd;
    HWND                hwnd_frame;
    HWND                root;
    HWND                root_frame;
    gui_window          *parent;
    WPI_FONT            font;
    WPI_FONT            prev_font;
    HBRUSH              bk_brush;
    WPI_PRES            hdc;
    PAINTSTRUCT         *ps;
    toolbarinfo         *toolbar;
    gui_create_styles   style;
    gui_scroll_styles   scroll;
    gui_flags           flags;
    int                 num_attrs;
    gui_colour_set      *colours;
    gui_ord             num_rows;
    gui_ord             old_rows;
    gui_ord             num_cols;
    unsigned int        hscroll_range;
    unsigned int        vscroll_range;
    GUICALLBACK         *call_back;
    void                *extra;
    HWND                status;
    int                 num_menus;
    gui_menu_struct     *mainmenu;
    hintinfo            hint;
    int                 hrange;
    int                 vrange;
    int                 hpos;
    int                 vpos;
    gui_rgb             bk_rgb;
    gui_window          *next;
    control_item        *controls;
    WPI_HICON           icon;
    popup_info          *popup;
    WPI_RECT            root_client;
    WPI_RECT            hwnd_client;
    gui_paint_info      root_pinfo;
    gui_paint_info      hwnd_pinfo;
} gui_window;

#define GUI_DO_HSCROLL( wnd )   ( ( ( wnd->scroll & GUI_HSCROLL ) != 0 ) && \
                                  ( ( wnd->style & GUI_HSCROLL_EVENTS ) == 0 ) )
#define GUI_DO_VSCROLL( wnd )   ( ( ( wnd->scroll & GUI_VSCROLL ) != 0 ) && \
                                  ( ( wnd->style & GUI_VSCROLL_EVENTS ) == 0 ) )
#define GUI_VSCROLL_ON( wnd )   ( ( wnd->scroll & GUI_VSCROLL ) != 0 )
#define GUI_HSCROLL_ON( wnd )   ( ( wnd->scroll & GUI_HSCROLL ) != 0 )
#define GUI_HSCROLL_COLS( wnd ) ( ( wnd->scroll & GUI_HCOLS ) != 0 )
#define GUI_VSCROLL_ROWS( wnd ) ( ( wnd->scroll & GUI_VROWS ) != 0 )

#define AVGXCHAR( tm ) ( _wpi_metricavecharwidth(tm) + _wpi_metricoverhang( tm ) )
#define AVGYCHAR( tm ) ( _wpi_metricheight(tm) + _wpi_metricexleading(tm) )
#define MAXXCHAR( tm ) ( _wpi_metricmaxcharwidth(tm) + _wpi_metricoverhang(tm) )
#define MAXYCHAR( tm ) ( _wpi_metricheight(tm) + _wpi_metricexleading(tm) )
#define EXTRA_SIZE              sizeof( LONG_PTR )
#define GUI_CONTAINER_WORD1     0
#if defined(__NT__)
#define GUI_CONTAINER_WORD2     1       // NT needs 2 longs for MDI
#else
#define GUI_CONTAINER_WORD2     0
#endif
#define GUI_EXTRA_WORD          ( GUI_CONTAINER_WORD2 + 1 )
#define GUI_MDI_WORD            ( GUI_EXTRA_WORD + 1 )
#define NUM_EXTRA_WORDS         ( GUI_MDI_WORD + 1 )
#define GUI_ISROOTWIN(wnd)      ( wnd->root != NULL )

typedef BOOL CALLBACK GUIENUMPROC( HWND hwnd, WPI_PARAM2 lparam );

#include "guix.h"

extern gui_window       *GUICurrWnd;

extern WPI_INST         GUIResHInst;
extern WPI_INST         GUIMainHInst;
extern char             GUIDefaultClassName[];

#endif // _GUIWIND_H_
