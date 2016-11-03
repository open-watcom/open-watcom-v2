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

    /* uitype        classn         classn_os2  style               xstyle_nt                 gui_control_classs  */
pick( FLD_HOT,      WC_BUTTON,        "#3",     PUSH_STYLE,           0 )                   /* GUI_PUSH_BUTTON    */
pick( FLD_HOT,      WC_BUTTON,        "#3",     DEFPUSH_STYLE,        0 )                   /* GUI_DEFPUSH_BUTTON */
pick( FLD_RADIO,    WC_BUTTON,        "#3",     RADIO_STYLE,          0 )                   /* GUI_RADIO_BUTTON   */
pick( FLD_CHECK,    WC_BUTTON,        "#3",     CHECK_STYLE,          0 )                   /* GUI_CHECK_BOX      */
pick( FLD_PULLDOWN, WC_COMBOBOX,      "#2",     COMBOBOX_STYLE,       WS_EX_CLIENTEDGE )    /* GUI_COMBOBOX       */
pick( FLD_EDIT,     WC_ENTRYFIELD,    "#6",     EDIT_STYLE,           WS_EX_CLIENTEDGE )    /* GUI_EDIT           */
pick( FLD_LISTBOX,  WC_LISTBOX,       "#7",     LISTBOX_STYLE,        WS_EX_CLIENTEDGE )    /* GUI_LISTBOX        */
pick( FLD_VOID,     WC_SCROLLBAR,     "#8",     SCROLLBAR_STYLE,      0 )                   /* GUI_SCROLLBAR      */
pick( FLD_TEXT,     WC_STATIC,        "#5",     STATIC_STYLE,         0 )                   /* GUI_STATIC         */
pick( FLD_FRAME,    WC_GROUPBOX,      "#5",     GROUPBOX_STYLE,       0 )                   /* GUI_GROUPBOX       */
pick( FLD_COMBOBOX, WC_COMBOBOX,      "#2",     EDIT_COMBOBOX_STYLE,  WS_EX_CLIENTEDGE )    /* GUI_EDIT_COMBOBOX  */
pick( FLD_EDIT_MLE, WC_MLE,           "#10",    EDIT_MLE_STYLE,       WS_EX_CLIENTEDGE )    /* GUI_EDIT_MLE       */
