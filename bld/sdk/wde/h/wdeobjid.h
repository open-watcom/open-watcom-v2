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


#ifndef WDEOBJID_INCLUDED
#define WDEOBJID_INCLUDED

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef enum {
    BASE_OBJ = USER_OBJ,
    DIALOG_OBJ,
    CONTROL_OBJ,
    PBUTTON_OBJ,
    CBUTTON_OBJ,
    RBUTTON_OBJ,
    GBUTTON_OBJ,
    FRAME_OBJ,
    TEXT_OBJ,
    ICON_OBJ,
    EDIT_OBJ,
    LISTBOX_OBJ,
    COMBOBOX_OBJ,
    HSCROLL_OBJ,
    VSCROLL_OBJ,
    SIZEBOX_OBJ,
    SBAR_OBJ,
    LVIEW_OBJ,
    TVIEW_OBJ,
    TABCNTL_OBJ,
    ANIMATE_OBJ,
    UPDOWN_OBJ,
    TRACKBAR_OBJ,
    PROGRESS_OBJ,
    HOTKEY_OBJ,
    HEADER_OBJ,
    CUSTCNTL1_OBJ,
    CUSTCNTL2_OBJ,
    LAST__OBJ
} Wde_Objects;

#define NUM_OBJECTS (LAST__OBJ - USER_OBJ)

#endif
