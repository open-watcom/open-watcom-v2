/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include <string.h>
#include "guiwind.h"
#include "guidlg.h"

#if !defined( __NT__ ) || !defined( GUI_IS_GUI )
static gui_colour_set DlgWndColours[GUI_NUM_ATTRS + 1] = {
  /* Fore              Back        */
  { GUI_BR_BLUE,  GUI_BR_WHITE },   /* GUI_MENU_PLAIN     */
  { GUI_BR_WHITE, GUI_BLUE },       /* GUI_MENU_STANDOUT  */
  { GUI_WHITE,    GUI_BR_WHITE },   /* GUI_BACKGROUND     */
  { GUI_RED,      GUI_WHITE },      /* GUI_TITLE_ACTIVE   */
  { GUI_GREY,     GUI_WHITE },      /* GUI_TITLE_INACTIVE */
  { GUI_BR_BLUE,  GUI_WHITE },      /* GUI_FRAME_ACTIVE   */
  { GUI_GREY,     GUI_WHITE },      /* GUI_FRAME_INACTIVE */
  { GUI_BR_CYAN,  GUI_CYAN }        /* GUI_FIRST_UNUSED   */
};
#endif

static gui_create_info DlgControl = {
    NULL,
    { 0, 0, 0, 0 },
    GUI_SCROLL_BOTH,
    GUI_SCROLL_EVENTS | GUI_ALL,
    NULL,
    GUI_NO_MENU,                            // Menu array
#if !defined( __NT__ ) || !defined( GUI_IS_GUI )
    GUI_COLOUR_ARRAY( DlgWndColours ),      // Colour attribute array
#else
    GUI_NO_COLOUR,                          // Colour attribute array
#endif
    NULL,                                   // GUI Event Callback Function
    NULL,
    NULL,
    NULL                                    // Menu Resource
};

static bool DlgModal = false;

static void GetHalfAndAdjust( gui_coord *charuse, gui_coord *half, gui_ord *char_ui_adjust )
{
    half->x = charuse->x / 2;
    half->y = charuse->y / 2;
    GUITruncToPixel( half );
    if( half->y == 0 ) {
        *char_ui_adjust = charuse->y;
    } else {
        *char_ui_adjust = 0;
    }
}

static void DlgSetCtlSizes( gui_control_info *controls_info,
                            int num, gui_coord *charuse, gui_coord *charspace )
{
    gui_coord   half;
    gui_ord     char_ui_adjust;

    /* unused parameters */ (void)charspace;

    GetHalfAndAdjust( charuse, &half, &char_ui_adjust );
    while( --num >= 0 ) {
        if( controls_info->style & GUI_STYLE_CONTROL_CHARCOORD ) {
            controls_info->rect.x = ( controls_info->rect.x + 1 ) * charuse->x + half.x;
            controls_info->rect.y = controls_info->rect.y * charuse->y + half.y + char_ui_adjust;
            switch( controls_info->control_class ) {
            case GUI_GROUPBOX:
                controls_info->rect.width = controls_info->rect.width * charuse->x - half.x;
                break;
            default:
                controls_info->rect.width = controls_info->rect.width * charuse->x;
                break;
            }
            switch( controls_info->control_class ) {
#ifdef __OS2_PM__
            // brutal hack to get OS/2 and Windows dialogs to look the same.
            case GUI_EDIT:
                controls_info->rect.height = charspace->y;
                break;
#endif
            case GUI_GROUPBOX:
                controls_info->rect.height = controls_info->rect.height * charuse->y - half.y;
                break;
            default:
                controls_info->rect.height = controls_info->rect.height * charuse->y;
                break;
            }
            controls_info->style &= ~GUI_STYLE_CONTROL_CHARCOORD;
        }
        ++controls_info;
    }
}

static void DlgSetSize( gui_window *parent_wnd, gui_create_info *dlg_info, gui_text_ord rows,
                        gui_text_ord cols, gui_coord *charuse )
{
    gui_rect            max_size;
    gui_rect            rect;
    gui_system_metrics  metrics;
    gui_coord           half;
    gui_ord             char_ui_adjust;

    GetHalfAndAdjust( charuse, &half, &char_ui_adjust );
    GUIGetSystemMetrics( &metrics );
    GUIGetScale( &max_size );
    dlg_info->rect.width = ( cols + 3 ) * charuse->x + metrics.dialog_top_left_size.x +
                      metrics.dialog_bottom_right_size.x;
    dlg_info->rect.height= rows * charuse->y + metrics.dialog_top_left_size.y +
                      metrics.dialog_bottom_right_size.y + char_ui_adjust;
    if( dlg_info->rect.width  > max_size.width ) {
        dlg_info->rect.width  = max_size.width;
    }
    if( dlg_info->rect.height > max_size.height ) {
        dlg_info->rect.height = max_size.height;
    }
    dlg_info->rect.x = max_size.x + ( max_size.width - dlg_info->rect.width ) / 2;
    dlg_info->rect.y = max_size.y + ( max_size.height - dlg_info->rect.height ) / 2;
    if( parent_wnd != NULL ) {
        GUIGetClientRect( parent_wnd, &rect );
        dlg_info->rect.x -= rect.x;
        dlg_info->rect.y -= rect.y;
    }
}

static void doDlgOpen( gui_window *parent_wnd, const char *title, gui_text_ord rows, gui_text_ord cols,
                     gui_control_info *controls_info, int num_controls,
                     GUICALLBACK *gui_call_back, void *extra, bool sys )
{
    gui_coord           charuse;
    gui_coord           charspace;
    gui_text_metrics    metrics;

    DlgControl.title = title;
    DlgControl.gui_call_back = gui_call_back;
    DlgControl.extra = extra;
    DlgControl.parent = parent_wnd;
    GUIGetDlgTextMetrics( &metrics );
    charspace.x = metrics.max.x;
    charspace.y = metrics.max.y;
#ifdef __OS2_PM__
    // brutal hack to get OS/2 and Windows dialogs to look the same.
    charuse.x = ( 5 * metrics.avg.x ) / 4;
    charuse.y = ( 3 * metrics.avg.y ) / 2;
#else
    charuse.x = metrics.avg.x;
    charuse.y = ( 7 * metrics.avg.y ) / 4;
#endif
    GUITruncToPixel( &charuse );
    DlgSetSize( parent_wnd, &DlgControl, rows, cols, &charuse );
    DlgSetCtlSizes( controls_info, num_controls, &charuse, &charspace );
    if( sys || DlgModal ) {
        GUICreateSysModalDialog( &DlgControl, num_controls, controls_info );
    } else {
        GUICreateDialog( &DlgControl, num_controls, controls_info );
    }
}

void GUIAPI GUISetModalDlgs( bool modal )
{
    DlgModal = modal;
}

void GUIAPI GUIDlgOpen( const char *title, gui_text_ord rows, gui_text_ord cols, gui_control_info *controls_info,
                 int num_controls, GUICALLBACK *gui_call_back, void *extra )
{
    doDlgOpen( NULL, title, rows, cols, controls_info, num_controls, gui_call_back, extra, false );
}

void GUIAPI GUIModalDlgOpen( gui_window *parent_wnd, const char *title, gui_text_ord rows, gui_text_ord cols,
                      gui_control_info *controls_info, int num_controls,
                      GUICALLBACK *gui_call_back, void *extra )
{
    doDlgOpen( parent_wnd, title, rows, cols, controls_info, num_controls, gui_call_back, extra, false );
}

void GUIAPI GUISysModalDlgOpen( const char *title, gui_text_ord rows, gui_text_ord cols,
                         gui_control_info *controls_info, int num_controls,
                         GUICALLBACK *gui_call_back, void *extra )
{
    doDlgOpen( NULL, title, rows, cols, controls_info, num_controls, gui_call_back, extra, true );
}
