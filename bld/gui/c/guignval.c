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


#include "guiwind.h"
#include "guidlg.h"
#include "guistr.h"
#include <string.h>

#define MIN_LENGTH      10
#define MAX_LENGTH      25
#define BUTTON_WIDTH    7
#define TEXT_ROW        0
#define BUTTON_ROW      2
#define NUM_ROWS        4
#define START_STATIC    5
#define START_EQUAL     7
#define START_EDIT      9

enum {
    STATIC,
    EQUAL,
    EDIT,
    CANCEL,
    OK
};

/* all 0 values are set in the code */

static gui_control_info GetNew[] = {
    DLG_STRING( NULL,  START_STATIC, TEXT_ROW,   0 ), /* STATIC */
    DLG_STRING( "=",              0, TEXT_ROW,   1 ),            /* EQUAL  */
    DLG_EDIT( NULL,       EDIT,   0, TEXT_ROW,   0 ),            /* EDIT   */
    DLG_BUTTON( NULL, CANCEL, 0, BUTTON_ROW, BUTTON_WIDTH ), /* CANCEL */
    DLG_DEFBUTTON( NULL,  OK,     0, BUTTON_ROW, BUTTON_WIDTH )  /* OK     */
};

#define NUM_CONTROLS  ( sizeof( GetNew ) / sizeof( gui_control_info ) )

typedef struct ret_info {
    char                *text;
    gui_message_return  ret_val;
} ret_info;

/*
 * GetNewFunction - call back routine for the GetNewVal dialog
 */

static bool GetNewFunction( gui_window *gui, gui_event gui_ev, void *param )
{
    unsigned    id;
    ret_info    *info;

    info = GUIGetExtra( gui );
    switch( gui_ev ) {
    case GUI_INIT_DIALOG :
        info->ret_val = GUI_RET_CANCEL;
        break;
    case GUI_CONTROL_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
            case CANCEL :
                GUICloseDialog( gui );
                info->ret_val = GUI_RET_CANCEL;
                break;
            case OK :
                info->text = GUIGetText( gui, EDIT );
                GUICloseDialog( gui );
                info->ret_val = GUI_RET_OK;
                break;
            default :
                break;
        }
        break;
    default :
        break;
    }
    return( true );
}

/*
 * GUIGetNewVal --
 */

gui_message_return GUIGetNewVal( const char * title, const char * old, char ** new_val )
{
    int         length;
    int         disp_length;
    int         cols;
    ret_info    info;

    info.ret_val = GUI_RET_ABORT;
    info.text = NULL;
    length = 0;
    if( old != NULL ) {
        length = strlen( old );
    }
    disp_length = length;
    if( disp_length < MIN_LENGTH ) {
        disp_length = MIN_LENGTH;
    }
    if( disp_length > MAX_LENGTH ) {
        disp_length = MAX_LENGTH;
    }

    GetNew[CANCEL].text = LIT( Cancel );
    GetNew[OK].text = LIT( OK );

    GetNew[EDIT].style |= GUI_FOCUS;

    GetNew[STATIC].rect.width = DLG_COL( disp_length );
    GetNew[STATIC].text = old;

    GetNew[EQUAL].rect.x = DLG_COL( START_EQUAL + disp_length );

    GetNew[EDIT].rect.x = DLG_COL( START_EDIT + disp_length );
    GetNew[EDIT].rect.width = DLG_COL( disp_length );
    GetNew[EDIT].text = old;

    cols = START_EDIT + disp_length * 2 + START_STATIC;

    GetNew[OK].rect.x = DLG_COL( ( cols / 2 ) -
                         ( ( cols / 2 - BUTTON_WIDTH ) / 2 ) - BUTTON_WIDTH );
    GetNew[CANCEL].rect.x = DLG_COL( cols -
                         ( ( cols / 2 - BUTTON_WIDTH ) / 2 ) - BUTTON_WIDTH );


    GUIDlgOpen( title, NUM_ROWS, cols, GetNew, NUM_CONTROLS,
                &GetNewFunction, &info );
    *new_val = info.text;
    return( info.ret_val );
}
