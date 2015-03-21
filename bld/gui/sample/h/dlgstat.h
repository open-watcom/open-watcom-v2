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


#define NUM_CONTROLS            13

enum {
    STATIC_CONTROL,
    EDIT_CONTROL,
    GROUP_CONTROL1,
    RADIOBUTTON_CONTROL1,
    RADIOBUTTON_CONTROL2,
    GROUP_CONTROL2,
    CHECKBOX_CONTROL1,
    CHECKBOX_CONTROL2,
    OKBUTTON_CONTROL,
    CANCELBUTTON_CONTROL,
    LISTBOX_CONTROL,
    ADDBUTTON_CONTROL,
    CLEARBUTTON_CONTROL,
};

extern  gui_control_info Controls[];

extern  bool            DialogScaled;
extern  bool            ButtonsScaled;
extern  bool            ControlsScaled;

extern int  NUM_LIST_BOX_DATA;
extern void StaticDialogInit( void );
extern void StaticDialogCreate( gui_window *parent );
extern GUICALLBACK StaticDialogEventWnd;
