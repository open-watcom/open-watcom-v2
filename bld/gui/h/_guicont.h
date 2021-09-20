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

/*    gui_control_classs    uitype          classn          classn_os2          style                   xstyle_nt */
pick( GUI_PUSH_BUTTON,      FLD_HOT,        WC_BUTTON,      WC_SYS_BUTTON,      PUSH_STYLE,             0 )
pick( GUI_DEFPUSH_BUTTON,   FLD_HOT,        WC_BUTTON,      WC_SYS_BUTTON,      DEFPUSH_STYLE,          0 )
pick( GUI_RADIO_BUTTON,     FLD_RADIO,      WC_BUTTON,      WC_SYS_BUTTON,      RADIO_STYLE,            0 )
pick( GUI_CHECK_BOX,        FLD_CHECK,      WC_BUTTON,      WC_SYS_BUTTON,      CHECK_STYLE,            0 )
pick( GUI_COMBOBOX,         FLD_PULLDOWN,   WC_COMBOBOX,    WC_SYS_COMBOBOX,    COMBOBOX_STYLE,         WS_EX_CLIENTEDGE )
pick( GUI_EDIT,             FLD_EDIT,       WC_ENTRYFIELD,  WC_SYS_ENTRYFIELD,  EDIT_STYLE,             WS_EX_CLIENTEDGE )
pick( GUI_LISTBOX,          FLD_LISTBOX,    WC_LISTBOX,     WC_SYS_LISTBOX,     LISTBOX_STYLE,          WS_EX_CLIENTEDGE )
pick( GUI_SCROLLBAR,        FLD_VOID,       WC_SCROLLBAR,   WC_SYS_SCROLLBAR,   SCROLLBAR_STYLE,        0 )
pick( GUI_STATIC,           FLD_TEXT,       WC_STATIC,      WC_SYS_STATIC,      STATIC_STYLE,           0 )
pick( GUI_GROUPBOX,         FLD_FRAME,      WC_GROUPBOX,    WC_SYS_STATIC,      GROUPBOX_STYLE,         0 )
pick( GUI_EDIT_COMBOBOX,    FLD_COMBOBOX,   WC_COMBOBOX,    WC_SYS_COMBOBOX,    EDIT_COMBOBOX_STYLE,    WS_EX_CLIENTEDGE )
pick( GUI_EDIT_MLE,         FLD_EDIT_MLE,   WC_MLE,         WC_SYS_MLE,         EDIT_MLE_STYLE,         WS_EX_CLIENTEDGE )
