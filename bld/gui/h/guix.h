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


#define GUI_MENU_FIX_TOOLBAR            1

#define GUI_MENU_MDI_FIRST_SEPARATOR    10
#define GUI_MENU_MDI_CASCADE            11
#define GUI_MENU_MDI_TILE_HORZ          12
#define GUI_MENU_MDI_TILE_VERT          13
#define GUI_MENU_MDI_ARRANGE_ICONS      14
#define GUI_MENU_MDI_SECOND_SEPARATOR   15
#define GUI_MENU_MDI_MORE_WINDOWS       16
#define GUI_MENU_MDI_FIRST_WINDOW       17
#define MAX_NUM_MDI_WINDOWS             9

#define GUI_MENU_ID( c )                ( GUI_LAST_MENU_ID + c + 1 )

#define GUI_FIX_TOOLBAR         GUI_MENU_ID( GUI_MENU_FIX_TOOLBAR )

#define GUI_MDI_FIRST_SEPARATOR GUI_MENU_ID( GUI_MENU_MDI_FIRST_SEPARATOR )
#define GUI_MDI_CASCADE         GUI_MENU_ID( GUI_MENU_MDI_CASCADE )
#define GUI_MDI_TILE_HORZ       GUI_MENU_ID( GUI_MENU_MDI_TILE_HORZ )
#define GUI_MDI_TILE_VERT       GUI_MENU_ID( GUI_MENU_MDI_TILE_VERT )
#define GUI_MDI_ARRANGE_ICONS   GUI_MENU_ID( GUI_MENU_MDI_ARRANGE_ICONS )
#define GUI_MDI_SECOND_SEPARATOR GUI_MENU_ID( GUI_MENU_MDI_SECOND_SEPARATOR )
#define GUI_MDI_MORE_WINDOWS    GUI_MENU_ID( GUI_MENU_MDI_MORE_WINDOWS )
#define GUI_MDI_FIRST_WINDOW    GUI_MENU_ID( GUI_MENU_MDI_FIRST_WINDOW )

#define GUI_MDI_MENU_FIRST      GUI_MDI_FIRST_SEPARATOR
#define GUI_MDI_MENU_LAST       GUI_MDI_FIRST_WINDOW + MAX_NUM_MDI_WINDOWS - 1

#define IS_MDIWIN(x)            (x >= GUI_MDI_FIRST_WINDOW && x <= GUI_MDI_FIRST_WINDOW + MAX_NUM_MDI_WINDOWS - 1)
#define IS_MDIMENU(x)           (x >= GUI_MDI_MENU_FIRST && x <= GUI_MDI_MENU_LAST)

#define MDIWIN2ID(x)            (x + GUI_MDI_FIRST_WINDOW)
#define ID2MDIWIN(x)            (x - GUI_MDI_FIRST_WINDOW)

/* Initialization Functions */

extern bool GUIXWndInit( unsigned );


extern bool GUIXInitHotSpots( int num, gui_resource *hot );
extern void GUIXCleanupHotSpots( void );

/* Window Functions */
extern bool GUIXCreateWindow( gui_window *, gui_create_info *, gui_window * );
extern void GUIXSetupWnd( gui_window * );

/* Control Functions */
extern bool GUIXCreateDialog( gui_create_info *dlg_info, gui_window *wnd, int,
                              gui_control_info *controls_info, bool sys, res_name_or_id dlg_id );

/* Administration functions */

extern void GUIFreeWindowMemory( gui_window *, bool from_parent, bool dialog );

extern bool GUIXCreateFloatingPopup( gui_window *wnd, gui_point *location,
                                     int num_menu_items, gui_menu_struct *menu,
                                     gui_mouse_track track, gui_ctl_id *curr_item );
extern bool GUIXCreateToolBar( gui_window *wnd, bool fixed, gui_ord height,
                       int num_toolbar_items, gui_toolbar_struct *toolinfo,
                       bool excl, gui_colour_set *plain, gui_colour_set *standout,
                       gui_rect *rect );
extern bool GUIXCreateToolBarWithTips( gui_window *wnd, bool fixed, gui_ord height,
                       int num_toolbar_items, gui_toolbar_struct *toolinfo,
                       bool excl, gui_colour_set *plain, gui_colour_set *standout,
                       gui_rect *rect, bool use_tips );

extern bool GUIXCloseToolBar( gui_window *wnd );

extern void GUIXDrawText( gui_window *wnd, const char *text, size_t length, gui_coord *pos,
                          gui_attr attr, gui_ord extentx, bool draw_extent );
extern void GUIXDrawTextRGB( gui_window *wnd, const char *text, size_t length, gui_coord *pos,
                             gui_rgb fore, gui_rgb back, gui_ord extentx, bool draw_extent );
extern void GUISetKeyState( void );
extern gui_window *GUIXGetRootWindow( void );
