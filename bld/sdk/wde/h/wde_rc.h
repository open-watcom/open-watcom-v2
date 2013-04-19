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


#ifndef WDE_RC_INCLUDED
#define WDE_RC_INCLUDED

#include "symbols.h"
#include "loadcust.h"
#include "selcust.h"
#include "seldlg.h"
#include "info.h"
#include "opts.h"
#include "define.h"

#include "constant.def"

/****************************************************************************/
/* Misc. Constants                                                          */
/****************************************************************************/
#define IDB_NOINPUT             -1

/****************************************************************************/
/* Misc. Menu Identifiers                                                   */
/****************************************************************************/
#define IDM_TAB                 200
#define IDM_STAB                201
#define IDM_GOTO_INFOBAR        202

/****************************************************************************/
/* File Menu                                                                */
/****************************************************************************/
#define FILE_MENU               0
#define CUSTOM_SUBMENU          5
#define IDM_NEW_RES             100
#define IDM_OPEN_RES            101
#define IDM_SAVE_RES            102
#define IDM_SAVEAS_RES          103
#define IDM_MS_CUSTOM           104
#define IDM_BOR_CUSTOM          105
#define IDM_LOADLIB             106
#define IDM_SELCUST1            107
#define IDM_SELCUST2            108
#define IDM_LOAD_SYMBOLS        109
#define IDM_VIEW_SYMBOLS        110
#define IDM_WRITE_SYMBOLS       111
#define IDM_EXIT                112
#define IDM_DDE_CLEAR           113
#define IDM_DDE_UPDATE_PRJ      114
#define WDE_FATAL_EXIT          115

/****************************************************************************/
/* Edit Menu                                                                */
/****************************************************************************/
#define EDIT_MENU               1
#define ALIGN_SUBMENU           5
#define SAMESIZE_SUBMENU        6
#define SPACE_SUBMENU           7
#define SYMBOLS_SUBMENU         16
#define IDM_SAME_WIDTH          120
#define IDM_SAME_HEIGHT         121
#define IDM_SAME_SIZE           122
#define IDM_SIZETOTEXT          123
#define IDM_DEFINEOBJECT        124
#define IDM_GEN_DEFINEOBJECT    125
#define IDM_OPTIONS             126
#define IDM_SELECT_DIALOG       127
#define IDM_REMOVE_DIALOG       128
#define IDM_HIDE_DIALOG         129
#define IDM_GOTO_OBJECT         130

/****************************************************************************/
/* Dialog Menu                                                              */
/****************************************************************************/
#define DIALOG_MENU             2
#define IDM_TEST_MODE           140
#define IDM_DIALOG_RESTORE      141
#define IDM_DIALOG_NEW          142
#define IDM_DIALOG_SAVE         143
#define IDM_DIALOG_SAVEAS       144
#define IDM_DIALOG_SAVEINTO     145
#define IDM_SET_ORDER           146
#define IDM_SET_TABS            147
#define IDM_SET_GROUPS          148

/****************************************************************************/
/* Tools Menu                                                               */
/****************************************************************************/
#define TOOLS_MENU              3
#define COMM_CTRL_MENU          19
#define IDM_SELECT_MODE         150
#define IDM_DIALOG_TOOL         151
#define IDM_PBUTTON_TOOL        152
#define IDM_CBUTTON_TOOL        153
#define IDM_RBUTTON_TOOL        154
#define IDM_GBUTTON_TOOL        155
#define IDM_FRAME_TOOL          156
#define IDM_TEXT_TOOL           157
#define IDM_ICON_TOOL           158
#define IDM_EDIT_TOOL           159
#define IDM_LISTBOX_TOOL        160
#define IDM_COMBOBOX_TOOL       161
#define IDM_HSCROLL_TOOL        162
#define IDM_VSCROLL_TOOL        163
#define IDM_SIZEBOX_TOOL        164
#define IDM_CUSTOM1_TOOL        165
#define IDM_CUSTOM2_TOOL        166
#define IDM_STICKY_TOOLS        167
#define IDM_STATUSBAR_TOOL      168
#define IDM_LISTVIEW_TOOL       169
#define IDM_TREEVIEW_TOOL       170
#define IDM_TABCNTL_TOOL        171
#define IDM_ANIMATE_TOOL        172
#define IDM_UPDOWN_TOOL         173
#define IDM_TRACKBAR_TOOL       174
#define IDM_PROGRESS_TOOL       175
#define IDM_HOTKEY_TOOL         176
#define IDM_HEADER_TOOL         177

/****************************************************************************/
/* Window Menu                                                              */
/****************************************************************************/
#define WINDOW_MENU             4
#define IDM_SHOW_TOOLS          180
#define IDM_SHOW_RIBBON         181
#define IDM_MDI_CASCADE         182
#define IDM_MDI_TILEH           183
#define IDM_MDI_TILEV           184
#define IDM_MDI_ARRANGE         185

/****************************************************************************/
/* Help Menu                                                                */
/****************************************************************************/
#define HELP_MENU               5
#define IDM_ABOUT               190
#define IDM_HELP                191
#define IDM_HELP_SEARCH         192
#define IDM_HELP_ON_HELP        193

#endif
