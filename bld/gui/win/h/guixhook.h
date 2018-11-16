/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2018 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef _GUIXHOOK_H_
#define _GUIXHOOK_H_

extern void GUISetMDIProcessMessage( bool (*func)(gui_window *, HWND, WPI_MSG, WPI_PARAM1, WPI_PARAM2, WPI_MRESULT *) );
extern bool GUIMDIProcessMessage( gui_window *wnd, HWND hwnd, WPI_MSG msg,
                                  WPI_PARAM1 wparam, WPI_PARAM2 lparam, WPI_MRESULT *ret );
extern void GUISetMDIMaximized( bool (*func)( gui_window *) );
extern bool GUIMDIMaximized( gui_window *wnd );
extern void GUISetMDIUpdatedMenu( bool (*func)( void ) );
extern bool GUIMDIUpdatedMenu( void );
extern void GUISetMDINewWindow( bool (*func)(HWND) );
extern bool GUIMDINewWindow( HWND );
extern void GUISetMDIMaximize( void (*func)(bool, gui_window *) );
extern void GUIMDIMaximize( bool max, gui_window *wnd );
extern void GUISetSetMDIRestoredSize( void (*func)(HWND, WPI_RECT *) );
extern void GUISetMDIRestoredSize( HWND hwnd, WPI_RECT *);
extern void GUISetIsMDIChildWindow( bool (*func)( gui_window *) );
extern bool GUIIsMDIChildWindow( gui_window *wnd );

#endif // _GUIXHOOK_H_
