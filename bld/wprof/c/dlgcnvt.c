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
* Description:  File open dialog for profiling information conversion.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include "wio.h"
#include "common.h"
#include "aui.h"
#include "wpaui.h"
#include "dip.h"
#include "guidlg.h"
#include "dlgbutn.h"
#include "dlgcnvtd.h"
#include "sampinfo.h"
#include "msg.h"
#include "utils.h"
#include "dlgcnvt.h"
#include "wpdata.h"

#include "clibext.h"


#define DLG_CNVT_ROWS   10
#define DLG_CNVT_COLS   50

#define C0 0
#define C1 38

#define R0 0
#define R1 3
#define R2 8

#define BW 10
#define W 50
#define B1 BUTTON_POS( 1, 3, W, BW )
#define B2 BUTTON_POS( 2, 3, W, BW )
#define B3 BUTTON_POS( 3, 3, W, BW )

#define DLGCONVERT_CTLS() \
    pick_p5(   OUTFILEBOX,  GUI_CTL_BOX,            "Output File Name",     C0,   R0,   C1-C0-1, 3 ) \
    pick_p4id( NAME,        GUI_CTL_EDIT,           "",                     C0+1, R0+1, C1-C0-3 ) \
    pick_p4id( BROWSE,      GUI_CTL_BUTTON,         "&Browse ...",          C1,   R0+1, W-C1 ) \
    pick_p5(   FORMATBOX,   GUI_CTL_BOX,            "Format Type",          C0,   R1,   C1-C0-1, 4 ) \
    pick_p4id( DIF_FMT,     GUI_CTL_RADIO_START,    "&DIF Format",          C0+1, R1+1, C1-C0-3 ) \
    pick_p4id( COMMA_FMT,   GUI_CTL_RADIO_END,      "&Comma Format",        C0+1, R1+2, C1-C0-3 ) \
    pick_p4id( OK,          GUI_CTL_DEFBUTTON,      "OK",                   B1,   R2,   BW+1 ) \
    pick_p4id( DEFAULTS,    GUI_CTL_BUTTON,         "&Defaults",            B2,   R2,   BW+1 ) \
    pick_p4id( CANCEL,      GUI_CTL_BUTTON,         "Cancel",               B3,   R2,   BW+1 )

enum {
    DUMMY_ID = 100,
    #define pick_p4id(id,m,p1,p2,p3,p4)     CTL_ ## id,
    #define pick_p5(id,m,p1,p2,p3,p4,p5)    CTL_ ## id,
    DLGCONVERT_CTLS()
    #undef pick_p5
    #undef pick_p4id
};

enum {
    #define pick_p4id(id,m,p1,p2,p3,p4)     id ## _IDX,
    #define pick_p5(id,m,p1,p2,p3,p4,p5)    id ## _IDX,
    DLGCONVERT_CTLS()
    #undef pick_p5
    #undef pick_p4id
};

static gui_control_info convertControls[] =
{
    #define pick_p4id(id,m,p1,p2,p3,p4)     m(p1,CTL_ ## id,p2,p3,p4),
    #define pick_p5(id,m,p1,p2,p3,p4,p5)    m(p1,p2,p3,p4,p5),
    DLGCONVERT_CTLS()
    #undef pick_p5
    #undef pick_p4id
};

static char * cnvtFilterList = {
    "DIF Files (*.dif)\0*.dif\0"
    "Comma Delimited Files (*.txt)\0*.txt\0"
    ALLFILES
};

STATIC char     convertPath[_MAX_PATH];

bool            OptDIFFormat = true;
bool            OptCommaFormat = false;
FILE            *ConvertFile;


STATIC void getDlgValues( gui_window *gui )
/*****************************************/
{
    OptDIFFormat = ( GUIIsChecked( gui, CTL_DIF_FMT ) == GUI_CHECKED );
    OptCommaFormat = ( GUIIsChecked( gui, CTL_COMMA_FMT ) == GUI_CHECKED );
    GUIDlgBuffGetText( gui, CTL_NAME, convertPath, sizeof( convertPath ) );
}


STATIC void setDlgValues( gui_window *gui )
/*****************************************/
{
    char        *add_ext;

    GUISetChecked( gui, CTL_DIF_FMT, ( OptDIFFormat ) ? GUI_CHECKED : GUI_NOT_CHECKED );
    GUISetChecked( gui, CTL_COMMA_FMT, ( OptCommaFormat ) ? GUI_CHECKED : GUI_NOT_CHECKED );
    if( OptDIFFormat ) {
        add_ext = ".dif";
    } else {
        add_ext = ".txt";
    }
    ReplaceExt( convertPath, add_ext );
    GUISetText( gui, CTL_NAME, convertPath );
}



STATIC void setDlgDefaults( gui_window *gui )
/*******************************************/
{
    OptDIFFormat = true;
    OptCommaFormat = false;
    strcpy( convertPath, CurrSIOData->samp_file_name );
    setDlgValues( gui );
}



STATIC bool dlgOpenFmtFile( void )
/********************************/
{
    ConvertFile = fopen( convertPath, "w" );
    if( ConvertFile == NULL ) {
        ErrorMsg( "Cannot open the convert file %s", convertPath );
        return( false );
    }
    return( true );
}



STATIC void dlgBrowseFmtFile( gui_window *gui )
/*********************************************/
{
    for( ;; ) {
        if( !DlgFileBrowse( LIT( Convert_File_Name ), cnvtFilterList,
                            convertPath, sizeof( convertPath ),
                            FN_HIDEREADONLY ) )
            break;
        if( dlgOpenFmtFile() ) {
            fclose( ConvertFile );
            remove( convertPath );
            break;
        }
    }
    GUISetFocus( gui, CTL_NAME );
}



STATIC bool progGUIEventProc( gui_window *gui, gui_event gui_ev, void *param )
/****************************************************************************/
{
    gui_ctl_id      id;

    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        setDlgValues( gui );
        GUISetFocus( gui, CTL_NAME );
        return( true );
    case GUI_CONTROL_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_BROWSE:
            dlgBrowseFmtFile( gui );
            return( true );
        case CTL_OK:
            getDlgValues( gui );
            if( dlgOpenFmtFile() ) {
                GUICloseDialog( gui );
            }
            return( true );
        case CTL_DEFAULTS:
            setDlgDefaults( gui );
            return( true );
        case CTL_CANCEL:
            GUICloseDialog( gui );
            return( true );
        }
        break;
    case GUI_DESTROY:
        return( true );
    }
    return( false );
}


void DlgGetConvert( a_window wnd )
/********************************/
{
    ConvertFile = NULL;
    CurrSIOData = WndExtra( wnd );
    if( CurrSIOData == NULL )
        return;
    strcpy( convertPath, CurrSIOData->samp_file_name );
    DlgOpen( LIT( Convert_Data ), DLG_CNVT_ROWS, DLG_CNVT_COLS,
             convertControls, ArraySize( convertControls ), &progGUIEventProc, NULL );
    if( CurrSIOData != NULL ) {
        WndDirty( CurrSIOData->sample_window );
    }
}
