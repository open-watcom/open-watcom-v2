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


#include "uidef.h"
#include "uivedit.h"
#include "uiledit.h"
#include "uidialog.h"

#define EV_SCREWUP      0x604

bool    Marking = false;


static a_hot_spot hot_msg = {
        "OK",      EV_ENTER, 1, -1, 11, HOT_DEFAULT
};

static a_hot_spot hot_num = {
        "Cancel",       EV_ESCAPE, 3, -1, 11, 0
};

static a_hot_spot hot_stuff = {
        "&Fill",            EV_SCREWUP, 5, -1, 11, 0
};

static char *strings_edit[] = {
        "yes",
        "no",
        "update",
        "insert",
        NULL
};

static char *list_data[] = {
    "HELLO", "THIS", "IS QUITE", "FUN", "ISN'T", "IT", "I DON'T", "KNOW",
    "WHY", "THIS", "HELLO", "THIS", "IS QUITE", "FUN", "ISN'T", "IT",
    "I DON'T", "KNOW", "WHY", "THIS", NULL
};

static char *list_data2[] = {
//    "HELLO", "THIS", "IS QUITE", "FUN", NULL
NULL
};

static a_combo_box toggle_edit = {
        0, strings_edit, NULL, NULL, NULL, 0, 0
};

static a_check check_home = {
        0,1,  "Home"
};

static a_check check_clear = {
        0, 0,  "Cl&ear"
};

static a_list list_box = {
        0, list_data2, NULL, NULL
};

static a_list list_box2 = {
        0, list_data, NULL, NULL
};

static a_radio_group neg_group = {
    0, "Neg", 0
};

static a_radio radio[] = {
      {  0,   "&Sign",      &neg_group },
      {  1,   "&Bill",      &neg_group }
};

an_edit_control     buf[5];

static VFIELD FieldDialog[] = {  // Define Field Dialog
        {1,     4,      1,      8,      FLD_LABEL,      "Formula"},
        {1,     13,     1,      27,     FLD_EDIT,       &buf[ 0 ]},
        {3,     3,      4,      13,     FLD_FRAME,      "Auto"},
        {4,     5,      1,      9,      FLD_CHECK,      &check_home},
        {5,     5,      1,      9,      FLD_CHECK,      &check_clear},
        {4,     28,     1,      8,      FLD_RADIO,      &radio[0]},
        {5,     28,     1,      8,      FLD_RADIO,      &radio[1]},
        {3,     26,     4,      14,     FLD_FRAME,      "Radio"},
        {2,     46,     8,      10,     FLD_LISTBOX,    &list_box},
        {8,     4,      1,      14,     FLD_TEXT,       "Allow edit"},
        {8,     18,     4,      9,      FLD_COMBOBOX,   &toggle_edit},
        {10,    4,      1,      9,      FLD_TEXT,       "Pulldown"},
        {10,    18,     6,      10,     FLD_PULLDOWN,   &list_box2},
        {1,     59,     1,      11,     FLD_HOT,        &hot_msg},
        {3,     59,     1,      11,     FLD_HOT,        &hot_num},
        {5,     59,     1,      11,     FLD_HOT,        &hot_stuff},
        {12,    0,      0,      0,      FLD_VOID,       NULL }
};

void sample_dialog()
{
    EVENT               event;
    a_dialog            *ui_dlg_info;

    static EVENT        events[] = {
        EV_NO_EVENT,
        EV_ENTER,
        EV_ESCAPE,
        EV_SCREWUP,
        EV_NO_EVENT
    };

    ui_dlg_info = uibegdialog( "TEST", FieldDialog, 12, 74, 0, 0 );
    uipushlist( events );
    for( ; ; ) {
        event = uidialog( ui_dlg_info );
        if( event == EV_ESCAPE || event == EV_ENTER ) {
            break;
        } else if( event == EV_SCREWUP ) {
            list_box.data_handle = list_data;
            uiupdatelistbox( &list_box );
        }
    }
    uipoplist();
    uienddialog( ui_dlg_info );
}
