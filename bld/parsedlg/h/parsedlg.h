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
* Description:  windows -> os2 dialog conversion data structures definition
*
****************************************************************************/


#define __str__(a) #a

/***************************************************************************
 * definitions for control type conversion
 ***************************************************************************/
#define TABLE 1

#define pick(a,b) T_##a,
typedef enum {
#include "restab.h"
    CTRL_TYPE_CNT
} control_type;
#undef pick

#define pick(a,b) #a,
char *control_type_win[] = {
#include "restab.h"
};
#undef pick

#define pick(a,b) #b,
char *control_type_os2[] = {
#include "restab.h"
};
#undef pick
#undef TABLE

/***************************************************************************
 * definitions for control class conversion
 ***************************************************************************/
#define TABLE 2

#define pick(a,b) T_TEXT_##a,
enum {
#include "restab.h"
    CTRL_NAME_CNT
};
#undef pick

#define pick(a,b) __str__(#a),
char *control_class_win[] = {
#include "restab.h"
};
#undef pick

#define pick(a,b) #b,
char *control_class_os2[] = {
#include "restab.h"
};
#undef pick
#undef TABLE

/***************************************************************************
 * definitions for font name conversion
 ***************************************************************************/
#define TABLE 3

#define pick(a,b,c) T_##a,
enum {
#include "restab.h"
    FONT_CNT
};
#undef pick

#define pick(a,b,c) #b,
char *font_win[] = {
#include "restab.h"
};
#undef pick

#define pick(a,b,c) #c,
char *font_os2[] = {
#include "restab.h"
};
#undef pick
#undef TABLE

/***************************************************************************
 * definitions for window style conversion
 ***************************************************************************/
#define TABLE 4

#define pick(a,b) T_##a,
#define pick1(a) T_##a,
enum {
#include "restab.h"
    WND_STYLE_CNT
};
#undef pick
#undef pick1

#define pick(a,b) #a,
#define pick1(a) #a,
char *window_style_win[] = {
#include "restab.h"
};
#undef pick
#undef pick1

#define pick(a,b) #b,
#define pick1(a) "",
char *window_style_os2[] = {
#include "restab.h"
};
#undef pick
#undef pick1
#undef TABLE

/***************************************************************************
 * definitions for style items conversion
 ***************************************************************************/
#define TABLE 5

#define pick(a,b,c) T_##a##_##b,
#define pick1(a,b) T_##a##_##b,
typedef enum {
#include "restab.h"
    STYLE_CNT
} style;
#undef pick
#undef pick1

#define pick(a,b,c) #b,
#define pick1(a,b) #b,
char *style_win_items[] = {
#include "restab.h"
};
#undef pick
#undef pick1

#define pick(a,b,c) #c,
#define pick1(a,b) "",
char *style_os2_items[] = {
#include "restab.h"
};
#undef pick
#undef pick1
#undef TABLE

int style_cnt[] = {
    T_RTEXT_WS_TABSTOP - T_LTEXT_WS_TABSTOP,
    T_CTEXT_WS_TABSTOP - T_RTEXT_WS_TABSTOP,
    T_CHECKBOX_WS_TABSTOP - T_CTEXT_WS_TABSTOP,
    T_PUSHBUTTON_WS_TABSTOP - T_CHECKBOX_WS_TABSTOP,
    T_LISTBOX_WS_BORDER - T_PUSHBUTTON_WS_TABSTOP,
    T_GROUPBOX_WS_TABSTOP - T_LISTBOX_WS_BORDER,
    T_DEFPUSHBUTTON_WS_TABSTOP - T_GROUPBOX_WS_TABSTOP,
    T_RADIOBUTTON_WS_TABSTOP - T_DEFPUSHBUTTON_WS_TABSTOP,
    T_EDITTEXT_WS_TABSTOP - T_RADIOBUTTON_WS_TABSTOP,
    T_COMBOBOX_WS_TABSTOP - T_EDITTEXT_WS_TABSTOP,
    T_ICON_SS_ICON - T_COMBOBOX_WS_TABSTOP,
    T_SCROLLBAR_WS_TABSTOP - T_ICON_SS_ICON,
    T_CONTROL_BS_PUSHBUTTON - T_SCROLLBAR_WS_TABSTOP,
    STYLE_CNT - T_CONTROL_BS_PUSHBUTTON
};

char **style_win[] = {
    style_win_items + T_LTEXT_WS_TABSTOP,
    style_win_items + T_RTEXT_WS_TABSTOP,
    style_win_items + T_CTEXT_WS_TABSTOP,
    style_win_items + T_CHECKBOX_WS_TABSTOP,
    style_win_items + T_PUSHBUTTON_WS_TABSTOP,
    style_win_items + T_LISTBOX_WS_BORDER,
    style_win_items + T_GROUPBOX_WS_TABSTOP,
    style_win_items + T_DEFPUSHBUTTON_WS_TABSTOP,
    style_win_items + T_RADIOBUTTON_WS_TABSTOP,
    style_win_items + T_EDITTEXT_WS_TABSTOP,
    style_win_items + T_COMBOBOX_WS_TABSTOP,
    style_win_items + T_ICON_SS_ICON,
    style_win_items + T_SCROLLBAR_WS_TABSTOP,
    style_win_items + T_CONTROL_BS_PUSHBUTTON
};

char **style_os2[] = {
    style_os2_items + T_LTEXT_WS_TABSTOP,
    style_os2_items + T_RTEXT_WS_TABSTOP,
    style_os2_items + T_CTEXT_WS_TABSTOP,
    style_os2_items + T_CHECKBOX_WS_TABSTOP,
    style_os2_items + T_PUSHBUTTON_WS_TABSTOP,
    style_os2_items + T_LISTBOX_WS_BORDER,
    style_os2_items + T_GROUPBOX_WS_TABSTOP,
    style_os2_items + T_DEFPUSHBUTTON_WS_TABSTOP,
    style_os2_items + T_RADIOBUTTON_WS_TABSTOP,
    style_os2_items + T_EDITTEXT_WS_TABSTOP,
    style_os2_items + T_COMBOBOX_WS_TABSTOP,
    style_os2_items + T_ICON_SS_ICON,
    style_os2_items + T_SCROLLBAR_WS_TABSTOP,
    style_os2_items + T_CONTROL_BS_PUSHBUTTON
};

/***************************************************************************
 * definitions only for CONTROL type style items conversion
 ***************************************************************************/
#define CTRL_STYLE_CNT (STYLE_CNT-T_CONTROL_BS_PUSHBUTTON)

char **control_style_win = style_win_items + T_CONTROL_BS_PUSHBUTTON;
char **control_style_os2 = style_os2_items + T_CONTROL_BS_PUSHBUTTON;
