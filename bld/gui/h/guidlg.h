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


#define GUI_NO_ID   ((gui_ctl_id)-1)

#define DLG_SET_RECT_CHARCOORD( r, x1, y1, width1, height1 ) \
      { (r).rect.x = x1; \
        (r).rect.y = y1; \
        (r).rect.width = width1; \
        (r).rect.height = height1; \
        (r).style |= GUI_STYLE_CONTROL_CHARCOORD; }
#define DLG_SET_RECT_CHARCOORD_1( r, x1, y1, width1 ) \
      { (r).rect.x = x1; \
        (r).rect.y = y1; \
        (r).rect.width = width1; \
        (r).rect.height = 1; \
        (r).style |= GUI_STYLE_CONTROL_CHARCOORD; }

#define GUI_CTL_BOX( s, x1, y1, width1, height1 ) \
    { GUI_GROUPBOX, s, \
      { x1, y1, width1, height1 }, \
      NULL, \
      GUI_NOSCROLL, \
      GUI_STYLE_CONTROL_CHARCOORD | GUI_STYLE_CONTROL_AUTOMATIC, \
      GUI_NO_ID }

#define GUI_CTL_DYNSTRING( s, id, x1, y1, width1 ) \
    { GUI_STATIC, s, \
      { x1, y1, width1, 1 }, \
      NULL, \
      GUI_NOSCROLL, \
      GUI_STYLE_CONTROL_CHARCOORD | GUI_STYLE_CONTROL_NOPREFIX | GUI_STYLE_CONTROL_AUTOMATIC, \
      id }

#define GUI_CTL_STRING( s, x1, y1, width1 ) \
    GUI_CTL_DYNSTRING( s, GUI_NO_ID, x1, y1, width1 )

#define GUI_CTL_DOBUTTON( g, s, id, x1, y1, width1 ) \
    { g, s, \
      { x1, y1, width1, 1 }, \
      NULL, \
      GUI_NOSCROLL, \
      GUI_STYLE_CONTROL_CHARCOORD | GUI_STYLE_CONTROL_TAB_GROUP | GUI_STYLE_CONTROL_AUTOMATIC, \
      id }

#define GUI_CTL_BUTTON( s, id, x1, y1, width1 ) \
    GUI_CTL_DOBUTTON( GUI_PUSH_BUTTON, s, id, x1, y1, width1 )

#define GUI_CTL_DEFBUTTON( s, id, x1, y1, width1 ) \
    GUI_CTL_DOBUTTON( GUI_DEFPUSH_BUTTON, s, id, x1, y1, width1 )

#define GUI_CTL_CHECK( s, id, x1, y1, width1 ) \
  { GUI_CHECK_BOX, s, \
    { x1, y1, width1, 1 }, \
    NULL, \
    GUI_NOSCROLL, \
    GUI_STYLE_CONTROL_CHARCOORD | GUI_STYLE_CONTROL_TAB_GROUP | GUI_STYLE_CONTROL_AUTOMATIC, \
    id }

#define GUI_CTL_DORADIO( g, s, id, x1, y1, width1 ) \
  { GUI_RADIO_BUTTON, s, \
    { x1, y1, width1, 1 }, \
    NULL, \
    GUI_NOSCROLL, \
    GUI_STYLE_CONTROL_CHARCOORD | GUI_STYLE_CONTROL_TAB_GROUP | GUI_STYLE_CONTROL_AUTOMATIC | (g), \
    id }

#define GUI_CTL_RADIO_START( s, id, x1, y1, width1 ) \
    GUI_CTL_DORADIO( GUI_STYLE_CONTROL_GROUP, s, id, x1, y1, width1 )

#define GUI_CTL_RADIO( s, id, x1, y1, width1 ) \
    GUI_CTL_DORADIO( GUI_STYLE_CONTROL_NOSTYLE, s, id, x1, y1, width1 )

#define GUI_CTL_RADIO_END( s, id, x1, y1, width1 ) \
    GUI_CTL_DORADIO( GUI_STYLE_CONTROL_GROUP, s, id, x1, y1, width1 )

#define GUI_CTL_DOEDIT( s, id, x1, y1, width1, v ) \
  { GUI_EDIT, s, \
    { x1, y1, width1, 1 }, \
    NULL, \
    GUI_NOSCROLL, \
    GUI_STYLE_CONTROL_CHARCOORD | GUI_STYLE_CONTROL_TAB_GROUP + (v), \
    id }

#define GUI_CTL_EDIT( s, id, x1, y1, width1 ) \
    GUI_CTL_DOEDIT( s, id, x1, y1, width1, 0 )

#define GUI_CTL_INVISIBLE_EDIT( s, id, x1, y1, width1 ) \
    GUI_CTL_DOEDIT( s, id, x1, y1, width1, GUI_STYLE_CONTROL_EDIT_INVISIBLE )

#define GUI_CTL_LIST_BOX( s, id, x1, y1, width1, height1 ) \
    { GUI_LISTBOX, s, \
      { x1, y1, width1, height1 }, \
      NULL, \
      GUI_NOSCROLL, \
      GUI_STYLE_CONTROL_CHARCOORD | GUI_STYLE_CONTROL_TAB_GROUP | GUI_STYLE_CONTROL_AUTOMATIC, \
      id }

#define GUI_CTL_COMBO_BOX( s, id, x1, y1, width1, height1 ) \
    { GUI_COMBOBOX, s, \
      { x1, y1, width1, height1 }, \
      NULL, \
      GUI_NOSCROLL, \
      GUI_STYLE_CONTROL_CHARCOORD | GUI_STYLE_CONTROL_TAB_GROUP | GUI_STYLE_CONTROL_AUTOMATIC, \
      id }

extern void GUIAPI  GUIDlgOpen( const char *title, gui_text_ord rows, gui_text_ord cols, gui_control_info *controls_info,
                        int num_controls, GUICALLBACK *gui_call_back, void *extra );

extern void GUIAPI  GUIModalDlgOpen( gui_window *parent, const char *title, gui_text_ord rows, gui_text_ord cols,
                        gui_control_info *controls_info, int num_controls, GUICALLBACK *gui_call_back, void *extra );

extern void GUIAPI  GUISysModalDlgOpen( const char *title, gui_text_ord rows, gui_text_ord cols,
                        gui_control_info *controls_info, int num_controls, GUICALLBACK *gui_call_back, void *extra );
