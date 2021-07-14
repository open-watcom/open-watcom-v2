/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2021 The Open Watcom Contributors. All Rights Reserved.
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

extern void         GUISetUseWnd( gui_window *wnd );
extern bool         GUIPtInRect( const SAREA *, ORD, ORD );
extern bool         GUIOverlap( const SAREA *one, const SAREA *two );
extern bool         GUISetArea( SAREA *, const gui_rect *, gui_window *parent_wnd, bool check_min, bool adjust );
extern bool         GUISetDialogArea( gui_window *wnd, SAREA *area, const gui_rect *rect, const SAREA *parent_area );
extern bool         GUIIsOpen( gui_window *wnd );
extern char         *GUIMakeEditCopy( char *buffer, int length );
extern void         GUIRedrawTitle( gui_window *wnd );
extern void         GUIMakeRelative( gui_window *wnd, const guix_point *scr_point, gui_point *point );
extern bool         GUIJustSetWindowText( gui_window *wnd, const char *title );
extern bool         GUIInArea( ORD row, ORD col, SAREA *area );
extern void         GUISetUseArea( gui_window *wnd, SAREA *area, SAREA *use );
extern void         GUIDirtyArea( gui_window *wnd, SAREA *area );
extern gui_window   *GUIGetTopWnd( gui_window *wnd );
extern bool         GUISetCursor( gui_window *wnd );
extern void         GUIAdjustDialogArea( SAREA *area, int mult );
extern void         GUIGetSAREA( gui_window *wnd, SAREA *area );
extern void         GUIGetClientSAREA( gui_window *wnd, SAREA *sarea );
extern bool         GUISetupStruct( gui_window *wnd, gui_create_info *, bool dialog );

extern void         GUIDestroyDialog( gui_window *wnd );
extern bool         GUICloseWnd( gui_window *wnd );

#endif // _GUIXUTIL_H_
