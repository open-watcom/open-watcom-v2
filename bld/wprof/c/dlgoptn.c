/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Profiler options dialog.
*
****************************************************************************/


#include <stdio.h>
#include "wio.h"
#include "common.h"
#include "aui.h"
#include "wpaui.h"
#include "dip.h"
#include "guidlg.h"
#include "dlgbutn.h"
#include "dlgoptnd.h"
#include "sampinfo.h"
#include "msg.h"
#include "setsamps.h"
#include "wpdata.h"
#include "dlgoptn.h"


#define SMALL_CUT_WIDTH 10
#define DLG_OPTS_ROWS   17
#define DLG_OPTS_COLS   50

#define C0 0
#define C1 1

#define R0 0
#define R1 5
#define R2 9
#define R3 15

#define BW 10
#define W 50
#define B1 BUTTON_POS( 1, 3, W, BW )
#define B2 BUTTON_POS( 2, 3, W, BW )
#define B3 BUTTON_POS( 3, 3, W, BW )

#define DLGOPTS_CTLS() \
    pick_p5(   BARGRAPHBOX, GUI_CTL_BOX,            "Bar Graph",                    C1+1, R0,   W-C1-2, 5 ) \
    pick_p4id( STRETCH,     GUI_CTL_CHECK,          "&Stretch the graph",           C1+2, R0+1, W-C1-4 ) \
    pick_p4id( ABS_BAR,     GUI_CTL_CHECK,          "Show &Absolute Bar Graphs",    C1+2, R0+2, W-C1-4 ) \
    pick_p4id( REL_BAR,     GUI_CTL_CHECK,          "Show &Relative Bar Graphs",    C1+2, R0+3, W-C1-4 ) \
    pick_p5(   SORTBYBOX,   GUI_CTL_BOX,            "Sort by",                      C1+1, R1,   W-C1-2, 4 ) \
    pick_p4id( SORT_COUNT,  GUI_CTL_RADIO_START,    "Sample &Count",                C1+2, R1+1, W-C1-4 ) \
    pick_p4id( SORT_NAME,   GUI_CTL_RADIO_END,      "&Name",                        C1+2, R1+2, W-C1-4 ) \
    pick_p5(   GATHERBOX,   GUI_CTL_BOX,            "Gather",                       C1+1, R2,   W-C1-2, 5 ) \
    pick_p4id( GATHER_TEXT, GUI_CTL_DYNSTRING,      "Cutoff Percentage",            C1+2, R2+1, W-C1-4 ) \
    pick_p4id( GATHER_CUT,  GUI_CTL_EDIT,           "",                             W-13, R2+1, 11 ) \
    pick_p4id( GATHER,      GUI_CTL_CHECK,          "Gather Small &Values",         C1+2, R2+3, W-C1-4 ) \
    pick_p4id( OK,          GUI_CTL_DEFBUTTON,      "OK",                           B1,   R3,   BW+1 ) \
    pick_p4id( DEFAULTS,    GUI_CTL_BUTTON,         "&Defaults",                    B2,   R3,   BW+1 ) \
    pick_p4id( CANCEL,      GUI_CTL_BUTTON,         "Cancel",                       B3,   R3,   BW+1 )

enum {
    DUMMY_ID = 100,
    #define pick_p4id(id,m,p1,p2,p3,p4)     CTL_ ## id,
    #define pick_p5(id,m,p1,p2,p3,p4,p5)    CTL_ ## id,
    DLGOPTS_CTLS()
    #undef pick_p5
    #undef pick_p4id
};

enum {
    #define pick_p4id(id,m,p1,p2,p3,p4)     id ## _IDX,
    #define pick_p5(id,m,p1,p2,p3,p4,p5)    id ## _IDX,
    DLGOPTS_CTLS()
    #undef pick_p5
    #undef pick_p4id
};

bool                OptStretchGlobal   = false;
bool                OptAbsBarGlobal    = true;
bool                OptRelBarGlobal    = true;
bool                OptSortCountGlobal = true;
bool                OptSortNameGlobal  = false;
bool                OptGatherGlobal    = false;
int                 OptGatherCut       = 10;

static gui_control_info optionControls[] = {
    #define pick_p4id(id,m,p1,p2,p3,p4)     m(p1,CTL_ ## id,p2,p3,p4),
    #define pick_p5(id,m,p1,p2,p3,p4,p5)    m(p1,p2,p3,p4,p5),
    DLGOPTS_CTLS()
    #undef pick_p5
    #undef pick_p4id
};

STATIC bool         getDlgValues( gui_window * );
STATIC void         setDlgValues( gui_window * );
STATIC void         setDlgDefaults( gui_window * );


STATIC bool getDlgValues( gui_window * gui )
/******************************************/
{
    char *      endptr;
    double      fcut;
    int         sort_type;
    char        cut_buff[SMALL_CUT_WIDTH+1];

    GUIDlgBuffGetText( gui, CTL_GATHER_CUT, cut_buff, sizeof( cut_buff ) );
    fcut = strtod( cut_buff, &endptr );
    if( *endptr == NULLCHAR ) {
        if( fcut > 10.0 ) {
            fcut = 10.0;
        } else if( fcut < 0.0 ) {
            fcut = 0.0;
        }
        OptGatherCut = (int)( fcut * 10.0 );
    }
    OptStretchGlobal = ( GUIIsChecked( gui, CTL_STRETCH ) == GUI_CHECKED );
    OptAbsBarGlobal = ( GUIIsChecked( gui, CTL_ABS_BAR ) == GUI_CHECKED );
    OptRelBarGlobal = ( GUIIsChecked( gui, CTL_REL_BAR ) == GUI_CHECKED );
    OptSortCountGlobal = ( GUIIsChecked( gui, CTL_SORT_COUNT ) == GUI_CHECKED );
    OptSortNameGlobal = ( GUIIsChecked( gui, CTL_SORT_NAME ) == GUI_CHECKED );
    OptGatherGlobal = ( GUIIsChecked( gui, CTL_GATHER ) == GUI_CHECKED );
    if( CurrSIOData != NULL ) {
        StretchSetAll( CurrSIOData, OptStretchGlobal );
        AbsSetAll( CurrSIOData, OptAbsBarGlobal );
        RelSetAll( CurrSIOData, OptRelBarGlobal );
        if( OptSortCountGlobal ) {
            sort_type = SORT_COUNT;
        } else {
            sort_type = SORT_NAME;
        }
        SortSetAll( CurrSIOData, sort_type );
        GatherSetAll( CurrSIOData, OptGatherGlobal );
    }
    return( true );
}



STATIC void setDlgValues( gui_window * gui )
/******************************************/
{
    char        cut_buff[SMALL_CUT_WIDTH+1];

    GUISetChecked( gui, CTL_STRETCH, ( OptStretchGlobal ) ? GUI_CHECKED : GUI_NOT_CHECKED );
    GUISetChecked( gui, CTL_ABS_BAR, ( OptAbsBarGlobal ) ? GUI_CHECKED : GUI_NOT_CHECKED );
    GUISetChecked( gui, CTL_REL_BAR, ( OptRelBarGlobal ) ? GUI_CHECKED : GUI_NOT_CHECKED );
    GUISetChecked( gui, CTL_SORT_COUNT, ( OptSortCountGlobal ) ? GUI_CHECKED : GUI_NOT_CHECKED );
    GUISetChecked( gui, CTL_SORT_NAME, ( OptSortNameGlobal ) ? GUI_CHECKED : GUI_NOT_CHECKED );
    GUISetChecked( gui, CTL_GATHER, ( OptGatherGlobal ) ? GUI_CHECKED : GUI_NOT_CHECKED );
    sprintf( cut_buff, "%d.%d", OptGatherCut / 10, OptGatherCut - ( OptGatherCut / 10 ) * 10 );
    GUISetText( gui, CTL_GATHER_CUT, cut_buff );
}



STATIC void setDlgDefaults( gui_window * gui )
/********************************************/
{
    OptStretchGlobal   = false;
    OptAbsBarGlobal    = true;
    OptRelBarGlobal    = true;
    OptSortCountGlobal = true;
    OptSortNameGlobal  = false;
    OptGatherGlobal    = false;
    OptGatherCut       = 1;
    setDlgValues( gui );
}



STATIC bool optsGUIEventProc( gui_window *gui, gui_event gui_ev, void *param )
/****************************************************************************/
{
    gui_ctl_id      id;

    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        setDlgValues( gui );
        GUISetFocus( gui, CTL_GATHER );
        return( true );
    case GUI_CONTROL_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_OK:
            getDlgValues( gui );
            /* fall through */
        case CTL_CANCEL:
            GUICloseDialog( gui );
            return( true );
        case CTL_DEFAULTS:
            setDlgDefaults( gui );
            return( true );
        }
        break;
    case GUI_DESTROY:
        return( true );
    }
    return( false );
}


void DlgGetOptions( a_window wnd )
/********************************/
{
    CurrSIOData = WndExtra( wnd );
    DlgOpen( LIT( Options ), DLG_OPTS_ROWS, DLG_OPTS_COLS,
            optionControls, ArraySize( optionControls ), &optsGUIEventProc, NULL );
    if( CurrSIOData != NULL ) {
        WndDirty( CurrSIOData->sample_window );
    }
}
