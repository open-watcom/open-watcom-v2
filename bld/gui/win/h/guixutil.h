/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef _GUIXUTIL_H_
#define _GUIXUTIL_H_

extern bool         GUIIsOpen( gui_window *wnd );
extern bool         GUISetupStruct( gui_window *wnd, gui_create_info *, guix_coord *scr_pos, guix_coord *scr_size, HWND parent, HMENU * );
extern void         GUICalcLocation( const gui_rect *rect, guix_coord *scr_pos, guix_coord *scr_size, HWND parent );
extern void         GUIError( const char *err_str );
extern bool         GUIScrollOn( gui_window *wnd, int bar );
extern void         GUISetRowCol( gui_window *wnd, const guix_coord *size );
extern void         GUIInvalidateResize( gui_window *wnd );
extern WPI_MRESULT  GUISendMessage( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
extern WPI_MRESULT  GUISendDlgItemMessage( HWND hwnd, gui_ctl_id id, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
extern void         GUIMakeRelative( gui_window *wnd, WPI_POINT *wpi_point, gui_point *point );
extern HWND         GUIGetScrollHWND( gui_window *wnd );
extern guix_ord     GUIGetScrollPos( gui_window *wnd, int bar );
extern guix_ord     GUIGetScrollRange( gui_window *wnd, int bar );
extern void         GUISetScrollPos( gui_window *wnd, int bar, guix_ord new, bool redraw );
extern void         GUISetScrollRange( gui_window *wnd, int bar, int min, int max, bool redraw );
extern void         GUISetRangePos( gui_window *wnd, int bar );
extern void         GUIRedrawScroll( gui_window *wnd, int bar, bool redraw_now );
extern HWND         GUIGetParentFrameHWND( gui_window *wnd );
extern HWND         GUIGetParentHWND( gui_window *wnd );
extern gui_window   *GUIGetWindow( HWND hwnd );
extern HWND         GUIGetTopParentHWND( HWND hwnd );
extern gui_window   *GUIGetTopGUIWindow( HWND hwnd );
extern bool         GUIBringNewToFront( gui_window *prev );
extern WPI_FONT     GUIGetSystemFont( void );
extern gui_window   *GUIFindFirstChild( gui_window *parent_wnd );
extern void         GUIDestroyAllChildren( gui_window *parent_wnd );
extern void         GUIDestroyAllPopupsWithNoParent( void );
extern bool         GUIParentHasFlags( gui_window *wnd, gui_flags flags );
extern bool         GUIIsRectInUpdateRect( gui_window *wnd, WPI_RECT *wpi_rect );
extern bool         GUIIsParentADialog( gui_window *wnd );
extern void         GUIMaximizeZoomedChildren( gui_window *wnd );
extern gui_window   *GUIFindWindowFromHWND( HWND hwnd );
extern bool         GUIIsGUIChild( HWND hwnd );

#endif // _GUIXUTIL_H_
