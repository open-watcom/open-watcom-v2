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


#ifndef _GUICONTR_H_
#define _GUICONTR_H_

#if defined( __GNUC__ )
struct control_item {
#else
typedef struct control_item {
#endif
    bool                checked;
    unsigned            id;
    gui_control_class   class;
    WPI_PROC            call_back;
    HWND                hwnd;
    char                *text;
    gui_control_styles  style;
    struct control_item *next;
};

#define BAD_CLASS ( (gui_control_class)-1 )

typedef struct {
    char                *classname;
    DWORD               style;
//    BOOL CALLBACK     (*call_back)( HWND, unsigned, UINT, LONG );
#if defined( UNIX )
    long (*call_back)( HWND, WPI_MSG, WPI_PARAM1, WPI_PARAM2 );
#elif defined( __WINDOWS_386__ )
    // Can't use definition below on this platform due to bug in 10.6 compiler
    // that causes relocation error on link or compiler warning, depending
    // on where CALLBACK word is. (SteveMcD)
    void *              call_back;
#else
    WPI_MRESULT CALLBACK (* call_back)( HWND, WPI_MSG, WPI_PARAM1, WPI_PARAM2 );
#endif
} controls_struct;

extern control_item *GUIGetControlByID( gui_window *parent, unsigned id );
extern control_item *GUIGetControlByHwnd( gui_window *parent, HWND control );
extern control_item *GUIControlInsert( gui_window *parent_wnd, gui_control_class class,
                              HWND control, gui_control_info *info,
                              WPI_PROC call_back );
extern control_item *GUIControlInsertByHWND( HWND hwnd, gui_window *parent );
extern void GUIControlDelete( gui_window *wnd, unsigned id );
extern void GUIControlDeleteAll( gui_window *wnd );
extern void GUIChangeHWnd( HWND, HWND );
extern gui_window *GUIGetParentWnd( HWND );
extern bool GUIAddParent( HWND, gui_window * );
WPI_PROC GUIDoSubClass( HWND, gui_control_class );
extern bool GUIInsertCtrlWnd( gui_window *wnd );
extern gui_window *GUIGetCtrlWnd( HWND hwnd );
extern LONG GUISetControlStyle( gui_control_info *info );
extern bool GUICheckRadioButton( gui_window *wnd, unsigned id );
#endif // _GUICONTR_H_
