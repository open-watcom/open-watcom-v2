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


#define PUSH_STYLE          BS_PUSHBUTTON
#define DEFPUSH_STYLE       BS_DEFPUSHBUTTON
#define RADIO_STYLE         BS_RADIOBUTTON
#define CHECK_STYLE         BS_CHECKBOX
#define COMBOBOX_STYLE      CBS_AUTOHSCROLL | CBS_DROPDOWNLIST

#ifdef __OS2_PM__
#define EDIT_STYLE          ES_AUTOHSCROLL | ES_MARGIN
#define LISTBOX_STYLE       LBS_NOTIFY
#define STATIC_STYLE        SS_LEFTNOWORDWRAP | DT_WORDBREAK
#else
#define EDIT_STYLE          ES_AUTOHSCROLL | WS_BORDER
#define LISTBOX_STYLE       WS_BORDER | WS_VSCROLL | LBS_NOTIFY
#define STATIC_STYLE        SS_LEFTNOWORDWRAP
#endif

#define EDIT_MLE_STYLE      EDIT_STYLE | ES_MULTILINE
#define SCROLLBAR_STYLE     0
#define GROUPBOX_STYLE      BS_GROUPBOX
#define EDIT_COMBOBOX_STYLE CBS_AUTOHSCROLL | CBS_DROPDOWN
#define MODAL_STYLE         WS_VISIBLE | WS_CAPTION | WS_SYSMENU | DS_MODALFRAME
#define SYSTEM_MODAL_STYLE  MODAL_STYLE | DS_SYSMODAL
#define COMMON_STYLES       WS_OVERLAPPED
#define CHILD_STYLE         WS_CHILD | WS_CLIPSIBLINGS

#define X_FRACT 4
#define Y_FRACT 8
