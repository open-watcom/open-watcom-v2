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


#define NUM_CONTROLS            13

#define CTRLS \
    pick( STATIC_CONTROL ) \
    pick( EDIT_CONTROL ) \
    pick( GROUP_CONTROL1 ) \
    pick( RADIOBUTTON_CONTROL1 ) \
    pick( RADIOBUTTON_CONTROL2 ) \
    pick( GROUP_CONTROL2 ) \
    pick( CHECKBOX_CONTROL1 ) \
    pick( CHECKBOX_CONTROL2 ) \
    pick( OKBUTTON_CONTROL ) \
    pick( CANCELBUTTON_CONTROL ) \
    pick( LISTBOX_CONTROL ) \
    pick( ADDBUTTON_CONTROL ) \
    pick( CLEARBUTTON_CONTROL )

enum {
    #define pick(x) x ## _IDX,
    CTRLS
    #undef pick
};

enum {
    DUMMY_START = 0,
    #define pick(x) x,
    CTRLS
    #undef pick
};


extern gui_control_info Controls[];

extern bool         DialogScaled;
extern bool         ButtonsScaled;
extern bool         ControlsScaled;

extern int          NUM_LIST_BOX_DATA;

extern void         StaticDialogInit( void );
extern void         StaticDialogCreate( gui_window *parent_wnd );
extern GUICALLBACK  StaticDialogWndGUIEventProc;
