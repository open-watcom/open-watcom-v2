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
#include "dlgoptn.h"
#include "sampinfo.h"
#include "msg.h"
#include "setsamps.h"

//#include "dlgoptn.def"
//#include "wpdriver.def"
//#include "setsamps.def"


bool    OptStretchGlobal   = false;
bool    OptAbsBarGlobal    = true;
bool    OptRelBarGlobal    = true;
bool    OptSortCountGlobal = true;
bool    OptSortNameGlobal  = false;
bool    OptGatherGlobal    = false;
int     OptGatherCut       = 10;

STATIC bool     getDlgValues( gui_window * );
STATIC void     setDlgValues( gui_window * );
STATIC void     setDlgDefaults( gui_window * );
STATIC GUICALLBACK progEvent;

extern sio_data *   CurrSIOData;



extern void DlgGetOptions( a_window * wnd )
/*****************************************/
{
    CurrSIOData = WndExtra( wnd );
    DlgOpen( LIT( Options ), DLG_OPTS_ROWS, DLG_OPTS_COLS,
             &optionControls[0], ArraySize( optionControls ), &progEvent, NULL );
    if( CurrSIOData != NULL ) {
        WndDirty( CurrSIOData->sample_window );
    }
}



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
        } else if( fcut < 0 ) {
            fcut = 0.0;
        }
        OptGatherCut = fcut * 10;
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



STATIC bool progEvent( gui_window *gui, gui_event gui_ev, void *param )
/*********************************************************************/
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
            GUICloseDialog( gui );
            return( true );
        case CTL_DEFAULTS:
            setDlgDefaults( gui );
            return( true );
        case CTL_CANCEL:
            GUICloseDialog( gui );
            return( true );
        }
        return( false );
    case GUI_DESTROY:
        return( true );
    }
    return( false );
}
