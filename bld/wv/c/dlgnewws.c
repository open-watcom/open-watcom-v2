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


#include "dbgdefn.h"
#include "dbgwind.h"
#include "guidlg.h"
#include "dlgnew.h"

extern void             SymComplete( gui_window *gui, int id );
extern void             ModComplete( gui_window *gui, int id );

typedef enum {
    COMPLETE_SYMBOL,
    COMPLETE_MODULE
} comp_type;

static  void            (*CompRtn)( gui_window *gui, int id );

enum {
    CTL_NEW_SYMBOL = CTL_NEW__LAST
};

#define R0 0
#define R1 2
#define C0 1
#define W 48
#define BW 12
#define B1 BUTTON_POS( 1, 3, W, BW )
#define B2 BUTTON_POS( 2, 3, W, BW )
#define B3 BUTTON_POS( 3, 3, W, BW )

#define DLG_NEW_ROWS    4
#define DLG_NEW_COLS    W
#define DLG_MAX_COLS    70


static gui_control_info Controls[] = {

    DLG_EDIT( "", CTL_NEW_EDIT,                 C0, R0, W-1 ),

    DLG_DEFBUTTON( NULL, CTL_NEW_OK,                    B1, R1, B1+BW ),
    DLG_BUTTON( NULL, CTL_NEW_SYMBOL,   B2, R1, B2+BW ),
    DLG_BUTTON( NULL, CTL_NEW_CANCEL,           B3, R1, B3+BW ),
};

static GUICALLBACK NewSymEvent;
static bool NewSymEvent( gui_window * gui, gui_event event, void * param )
{
    unsigned    id;

    if( DlgNewEvent( gui, event, param ) ) return( TRUE );
    switch( event ) {
    case GUI_CONTROL_CLICKED:
        GUI_GETID( param, id );
        if( id == CTL_NEW_SYMBOL ) {
            CompRtn( gui, CTL_NEW_EDIT );
        }
        return( TRUE );
    default:
        return( FALSE );
    }
}

static void DoDlgNew( char *text, char *buff, unsigned buff_len, comp_type type )
{
    if( type == COMPLETE_SYMBOL ) {
        CompRtn = &SymComplete;
        Controls[2].text = LIT( XSymbol_ );
    } else {
        CompRtn = &ModComplete;
        Controls[2].text = LIT( XModule_ );
    }
    Controls[1].text = LIT( OK );
    Controls[3].text = LIT( Cancel );
    DlgNewWithCtl( text, buff, buff_len,
                   Controls, ArraySize( Controls ), NewSymEvent,
                   DLG_NEW_ROWS, DLG_NEW_COLS, DLG_MAX_COLS );
}

extern  void    DlgNewWithMod( char *text, char *buff, unsigned buff_len )
{
    DoDlgNew( text, buff, buff_len, COMPLETE_MODULE );
}

extern  void    DlgNewWithSym( char *text, char *buff, unsigned buff_len )
{
    DoDlgNew( text, buff, buff_len, COMPLETE_SYMBOL );
}
