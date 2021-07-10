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

enum {
    DLG_ROW_0 = 30000,
    DLG_ROW_n = DLG_ROW_0 + 100,
    DLG_COL_0,
    DLG_COL_n = DLG_COL_0 + 100
};

#define DLG_ROW( n ) ( (n) + DLG_ROW_0 )
#define DLG_COL( n ) ( (n) + DLG_COL_0 )

#define DLG_SET_RECT( r, x1, y1, x2, y2 ) \
      { (r).rect.x = DLG_COL( x1 ); \
        (r).rect.y = DLG_ROW( y1 ); \
        (r).rect.width = DLG_COL( (x2)-(x1)+1 ); \
        (r).rect.height = DLG_ROW( (y2)-(y1)+1 ); }
#define DLG_RECT( x1, y1, x2, y2 ) \
      { DLG_COL( x1 ), DLG_ROW( y1 ), \
        DLG_COL( (x2)-(x1)+1 ), DLG_ROW( (y2)-(y1)+1 ) }

#define DLG_BOX( s, x1, y1, x2, y2 ) \
    { GUI_GROUPBOX, s, \
      DLG_RECT( x1, y1, x2, y2 ), \
      NULL, \
      GUI_NOSCROLL, \
      GUI_STYLE_CONTROL_AUTOMATIC, \
      GUI_NO_ID }

#define DLG_DYNSTRING( s, id, x1, y1, x2 ) \
    { GUI_STATIC, s, \
      DLG_RECT( x1, y1, x2, y1 ), \
      NULL, \
      GUI_NOSCROLL, \
      GUI_STYLE_CONTROL_NOPREFIX | GUI_STYLE_CONTROL_AUTOMATIC, \
      id }

#define DLG_STRING( s, x1, y1, x2 ) \
    DLG_DYNSTRING( s, GUI_NO_ID, x1, y1, x2 )

#define DLG_DOBUTTON( g, s, id, x1, y1, x2 ) \
    { g, s, \
      DLG_RECT( x1, y1, x2, y1 ), \
      NULL, \
      GUI_NOSCROLL, \
      GUI_STYLE_CONTROL_TAB_GROUP | GUI_STYLE_CONTROL_AUTOMATIC, \
      id }

#define DLG_BUTTON( s, id, x1, y1, x2 ) \
    DLG_DOBUTTON( GUI_PUSH_BUTTON, s, id, x1, y1, x2 )

#define DLG_DEFBUTTON( s, id, x1, y1, x2 ) \
    DLG_DOBUTTON( GUI_DEFPUSH_BUTTON, s, id, x1, y1, x2 )

#define DLG_CHECK( s, id, x1, y1, x2 ) \
  { GUI_CHECK_BOX, s, \
    DLG_RECT( x1, y1, x2, y1 ), \
    NULL, \
    GUI_NOSCROLL, \
    GUI_STYLE_CONTROL_TAB_GROUP | GUI_STYLE_CONTROL_AUTOMATIC, \
    id }

#define DLG_DORADIO( g, s, id, x1, y1, x2 ) \
  { GUI_RADIO_BUTTON, s, \
    DLG_RECT( x1, y1, x2, y1 ), \
    NULL, \
    GUI_NOSCROLL, \
    g | GUI_STYLE_CONTROL_TAB_GROUP | GUI_STYLE_CONTROL_AUTOMATIC, \
    id }

#define DLG_RADIO_START( s, id, x1, y1, x2 ) \
    DLG_DORADIO( GUI_STYLE_CONTROL_GROUP, s, id, x1, y1, x2 )

#define DLG_RADIO( s, id, x1, y1, x2 ) \
    DLG_DORADIO( GUI_STYLE_CONTROL_NOSTYLE, s, id, x1, y1, x2 )

#define DLG_RADIO_END( s, id, x1, y1, x2 ) \
    DLG_DORADIO( GUI_STYLE_CONTROL_GROUP, s, id, x1, y1, x2 )

#define DLG_DOEDIT( s, id, x1, y1, x2, v ) \
  { GUI_EDIT, s, \
    DLG_RECT( x1, y1, x2, y1 ), \
    NULL, \
    GUI_NOSCROLL, \
    GUI_STYLE_CONTROL_TAB_GROUP + (v), \
    id }

#define DLG_EDIT( s, id, x1, y1, x2 ) \
    DLG_DOEDIT( s, id, x1, y1, x2, 0 )

#define DLG_INVISIBLE_EDIT( s, id, x1, y1, x2 ) \
    DLG_DOEDIT( s, id, x1, y1, x2, GUI_STYLE_CONTROL_EDIT_INVISIBLE )

#define DLG_LIST_BOX( s, id, x1, y1, x2, y2 ) \
    { GUI_LISTBOX, s, \
      DLG_RECT( x1, y1, x2, y2 ), \
      NULL, \
      GUI_NOSCROLL, \
      GUI_STYLE_CONTROL_TAB_GROUP | GUI_STYLE_CONTROL_AUTOMATIC, \
      id }

#define DLG_COMBO_BOX( s, id, x1, y1, x2, y2 ) \
    { GUI_COMBOBOX, s, \
      DLG_RECT( x1, y1, x2, y2 ), \
      NULL, \
      GUI_NOSCROLL, \
      GUI_STYLE_CONTROL_TAB_GROUP | GUI_STYLE_CONTROL_AUTOMATIC, \
      id }

extern void GUIDlgOpen( const char *title, gui_text_ord rows, gui_text_ord cols, gui_control_info *controls_info,
                        int num_controls, GUICALLBACK *gui_call_back, void *extra );

extern void GUIModalDlgOpen( gui_window *parent, const char *title, gui_text_ord rows, gui_text_ord cols,
                gui_control_info *controls_info, int num_controls, GUICALLBACK *gui_call_back, void *extra );

extern void GUISysModalDlgOpen( const char *title, gui_text_ord rows, gui_text_ord cols,
                gui_control_info *controls_info, int num_controls, GUICALLBACK *gui_call_back, void *extra );
