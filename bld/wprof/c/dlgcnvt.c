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


#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <process.h>

#include "common.h"
#include "aui.h"
#include "wpaui.h"
#include "dip.h"
#include "guidlg.h"
#include "dlgbutn.h"
#include "dlgcnvt.h"
#include "sampinfo.h"
#include "msg.h"
#include "pathlist.h"

//#include "dlgcnvt.def"
//#include "wpdriver.def"
//#include "utils.def"
//#include "msg.def"
extern void ReplaceExt(char *path,char *addext);
extern void ErrorMsg(char *msg,... );


static char * cnvtFilterList = {
    "DIF Files (*.dif)\0*.dif\0"
    "Comma Delimited Files (*.txt)\0*.txt\0"
    ALLFILES
};

STATIC char     convertPath[_MAX_PATH];

bint            OptDIFFormat = B_TRUE;
bint            OptCommaFormat = B_FALSE;
FILE *          ConvertFile;

STATIC void     getDlgValues( gui_window * );
STATIC void     setDlgValues( gui_window * );
STATIC void     setDlgDefaults( gui_window * );
STATIC bint     dlgOpenFmtFile();
STATIC void     dlgBrowseFmtFile( gui_window * );
STATIC bool     progEvent( gui_window *, gui_event, void * );

extern sio_data *   CurrSIOData;



extern void DlgGetConvert( a_window * wnd )
/*****************************************/
{
    ConvertFile = NULL;
    CurrSIOData = WndExtra( wnd );
    if( CurrSIOData == NULL ) return;
    strcpy( convertPath, CurrSIOData->samp_file_name );
    DlgOpen( LIT( Convert_Data ), DLG_CNVT_ROWS, DLG_CNVT_COLS,
             &convertControls[0], ArraySize( convertControls ), &progEvent, NULL );
    if( CurrSIOData != NULL ) {
        WndDirty( CurrSIOData->sample_window );
    }
}



STATIC void getDlgValues( gui_window * gui )
/******************************************/
{
    OptDIFFormat = GUIIsChecked( gui, CTL_DIF_FMT );
    OptCommaFormat = GUIIsChecked( gui, CTL_COMMA_FMT );
    GUIDlgBuffGetText( gui, CTL_NAME, convertPath, _MAX_PATH );
}



STATIC void setDlgValues( gui_window * gui )
/******************************************/
{
    char *      add_ext;

    GUISetChecked( gui, CTL_DIF_FMT, OptDIFFormat );
    GUISetChecked( gui, CTL_COMMA_FMT, OptCommaFormat );
    if( OptDIFFormat ) {
        add_ext = ".dif";
    } else {
        add_ext = ".txt";
    }
    ReplaceExt( convertPath, add_ext );
    GUISetText( gui, CTL_NAME, convertPath );
}



STATIC void setDlgDefaults( gui_window * gui )
/********************************************/
{
    OptDIFFormat = B_TRUE;
    OptCommaFormat = B_FALSE;
    strcpy( convertPath, CurrSIOData->samp_file_name );
    setDlgValues( gui );
}



STATIC bint dlgOpenFmtFile()
/**************************/
{
    ConvertFile = fopen( convertPath, "W" );
    if( ConvertFile == NULL ) {
        ErrorMsg( "Cannot open the convert file %s", convertPath );
        return( B_FALSE );
    }
    return( B_TRUE );
}



STATIC void dlgBrowseFmtFile( gui_window * gui )
/**********************************************/
{
    for( ;; ) {
        if( !DlgFileBrowse( LIT( Convert_File_Name ), cnvtFilterList,
                            convertPath, sizeof( convertPath ),
                            OFN_HIDEREADONLY ) ) break;
        if( dlgOpenFmtFile() ) {
            fclose( ConvertFile );
            remove( convertPath );
            break;
        }
    }
    GUISetFocus( gui, CTL_NAME );
}



STATIC bool progEvent( gui_window * gui, gui_event gui_ev, void * param )
/***********************************************************************/
{
    unsigned            id;

    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        setDlgValues( gui );
        GUISetFocus( gui, CTL_NAME );
        return( B_TRUE );
    case GUI_CONTROL_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_BROWSE:
            dlgBrowseFmtFile( gui );
            return( B_TRUE );
        case CTL_OK:
            getDlgValues( gui );
            if( dlgOpenFmtFile() ) {
                GUICloseDialog( gui );
            }
            return( B_TRUE );
        case CTL_DEFAULTS:
            setDlgDefaults( gui );
            return( B_TRUE );
        case CTL_CANCEL:
            GUICloseDialog( gui );
            return( B_TRUE );
        }
        return( B_FALSE );
    case GUI_DESTROY:
        return( B_TRUE );
    }
    return( B_FALSE );
}
