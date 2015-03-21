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


#include <string.h>
#include "gui.h"

#define HEIGHT 5
#define WIDTH 8

static gui_rect Scale = { 0, 0, 80, 100 };
static gui_ord Width = 0;

#define NUM_TEXT 5

static char Text[][NUM_TEXT] = { {"0%"}, {"25%"}, {"50%"}, {"75%"}, {"100%"} };
static int Strlen[NUM_TEXT] = { 2, 3, 3, 3, 4 };

static GUICALLBACK GetNewFunction;
static GUICALLBACK StatusFunction;

static gui_create_info DialogWnd = {
    "Install Program: ",
    { 20, 20, 40, 40 },
    GUI_NOSCROLL,
    GUI_VISIBLE,
    NULL,
    0,
    NULL,
    0,
    NULL,
    &GetNewFunction,
    NULL,
    0
};

static gui_rect Rect;
static int Row;
static int NumEnters = 0;

static gui_colour_set StatusColours[GUI_NUM_ATTRS+1] = {
    /* Fore              Back        */
    GUI_BLUE,          GUI_WHITE,   /* GUI_MENU_PLAIN     */
    GUI_BLUE,          GUI_WHITE,   /* GUI_MENU_STANDOUT  */
    GUI_BLUE,          GUI_WHITE,   /* GUI_BACKGROUND     */
    GUI_BLUE,          GUI_WHITE,   /* GUI_TITLE_ACTIVE   */
    GUI_GREY,          GUI_WHITE,   /* GUI_TITLE_INACTIVE */
    GUI_BLUE,          GUI_WHITE,   /* GUI_FRAME_ACTIVE   */
    GUI_GREY,          GUI_WHITE,   /* GUI_FRAME_INACTIVE */
    GUI_BRIGHT_WHITE,  GUI_MAGENTA  /* GUI_FIRST_UNUSED   */
};

static gui_create_info StatusWnd = {
    "Percent of Installation Complete",
    { 19, 70, 42, 20 },
    GUI_NOSCROLL,
    GUI_VISIBLE | GUI_DIALOG_LOOK,
    NULL,
    0,
    NULL,
    GUI_NUM_ATTRS + 1,
    &StatusColours,
    &StatusFunction,
    NULL,
    0
};

enum {
   ctr_static,
   ctr_edit,
   ctr_cancelbutton,
   ctr_okbutton
};

static gui_control_info GetNew[] = {
    { GUI_STATIC,         "Please enter install path:",     { 4, 4, 30, HEIGHT }, NULL, GUI_NOSCROLL, GUI_NONE, ctr_static },
    { GUI_EDIT,           NULL,     { 4, 15, 30, HEIGHT }, NULL, GUI_NOSCROLL, GUI_NONE, ctr_edit },
    { GUI_PUSH_BUTTON,   "CANCEL",  { 6, 30, WIDTH, HEIGHT }, NULL, GUI_NOSCROLL, GUI_NONE, ctr_cancelbutton },
    { GUI_DEFPUSH_BUTTON, "OK",     { 26, 30, WIDTH, HEIGHT }, NULL, GUI_NOSCROLL, GUI_NONE, ctr_okbutton }
};

static gui_colour_set Colours[GUI_NUM_INIT_COLOURS] =
{
    { GUI_BRIGHT_WHITE, GUI_BLUE },
    { GUI_BRIGHT_WHITE, GUI_BLUE }
};

static char * text = NULL;
static gui_message_return ret_val = GUI_RET_CANCEL;

static gui_window * Status = NULL;

/*
 * GetNewFunction - call back routine for the GetNewVal dialog
 */

static bool GetNewFunction( gui_window *gui, gui_event gui_ev, void *param )
{
    unsigned id;

    switch( gui_ev ) {
    case GUI_INIT_DIALOG :
        ret_val = GUI_RET_CANCEL;
        break;
    case GUI_DESTROY :
        if( Status != NULL ) {
            GUIDestroyWnd( Status );
        }
        break;
    case GUI_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case ctr_cancelbutton :
            GUICloseDialog( gui );
            ret_val = GUI_RET_CANCEL;
            break;
        case ctr_okbutton :
            text = GUIGetText( gui, ctr_edit );
            if( Status == NULL ) {
                Status = GUICreateWindow( &StatusWnd );
            } else {
                NumEnters ++;
                Rect.width = ( NumEnters * Width ) / 4;
                if( NumEnters > 4 ) {
                    GUICloseDialog( gui );
                } else {
                    GUIWndDirty( Status );
                }
            }
            ret_val = GUI_RET_OK;
            break;
        }
        break;
    }
    return( true );
}

/*
 * StatusFunction - call back routine for the status window
 */

static bool StatusFunction( gui_window * gui, gui_event gui_ev, void * param )
{
    int              i;
    int              pos;
    gui_text_metrics metrics;

    param = param;

    switch( gui_ev ) {
    case GUI_INIT_WINDOW :
        Row = GUIGetNumRows( gui ) / 2;
        GUIGetTextMetrics( gui, &metrics );
        GUIGetClientRect( gui, &Rect );
#if 1
        Rect.x = 1;
        Rect.y = 1;
        Width = Rect.width - 2 * Rect.x;
#endif
#if 0
        Rect.height -= Rect.y;
        Rect.x = 0;
        Rect.y = 0;
#endif
        Rect.width = 0;
        for( i = 0; i < NUM_TEXT; i++ ) {
            Strlen[i] *= metrics.max.x;
        }
        break;
    case GUI_DESTROY :
        break;
    case GUI_PAINT :
        GUIDrawRect( gui, &Rect, GUI_FIRST_UNUSED );
        for( i = 0; i < NUM_TEXT; i++ ) {
            pos = ( i * Width / 4 ) - Strlen[i] + Rect.x;
            if( pos < (int)Rect.x ) {
                pos = Rect.x;
            }
            if( ( i > NumEnters ) ||
                ( i == 0 ) && ( NumEnters == 0 ) ) {
                GUIDrawText( gui, &Text[i], Strlen[i], Row, pos,
                             GUI_TITLE_ACTIVE );
            } else {
                GUIDrawText( gui, &Text[i], Strlen[i], Row, pos,
                             GUI_FIRST_UNUSED );
            }
        }
        break;
    }
    return( true );
}

void GUImain( void )
{

    GUIMemOpen();
    GUIWndInit( 250 );
    GUISetScale( &Scale );
    GUISetColour( &Colours );
    GUICreateDialog( &DialogWnd, NUM_CONTROLS, &GetNew );
}
