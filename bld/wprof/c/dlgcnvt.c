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
#include "dlgcnvt.h"
#include "sampinfo.h"
#include "msg.h"
#include "clibext.h"


extern void ReplaceExt(char *path,char *addext);
extern void ErrorMsg(char *msg,... );


static char * cnvtFilterList = {
    "DIF Files (*.dif)\0*.dif\0"
    "Comma Delimited Files (*.txt)\0*.txt\0"
    ALLFILES
};

STATIC char     convertPath[_MAX_PATH];

bint            OptDIFFormat = P_TRUE;
bint            OptCommaFormat = P_FALSE;
FILE            *ConvertFile;

STATIC bool     progEvent( gui_window *, gui_event, void * );

extern sio_data *CurrSIOData;



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



STATIC void getDlgValues( gui_window *gui )
/*****************************************/
{
    OptDIFFormat = GUIIsChecked( gui, CTL_DIF_FMT );
    OptCommaFormat = GUIIsChecked( gui, CTL_COMMA_FMT );
    GUIDlgBuffGetText( gui, CTL_NAME, convertPath, _MAX_PATH );
}



STATIC void setDlgValues( gui_window *gui )
/*****************************************/
{
    char        *add_ext;

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



STATIC void setDlgDefaults( gui_window *gui )
/*******************************************/
{
    OptDIFFormat = P_TRUE;
    OptCommaFormat = P_FALSE;
    strcpy( convertPath, CurrSIOData->samp_file_name );
    setDlgValues( gui );
}



STATIC bint dlgOpenFmtFile( void )
/********************************/
{
    ConvertFile = fopen( convertPath, "w" );
    if( ConvertFile == NULL ) {
        ErrorMsg( "Cannot open the convert file %s", convertPath );
        return( P_FALSE );
    }
    return( P_TRUE );
}



STATIC void dlgBrowseFmtFile( gui_window *gui )
/*********************************************/
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
        return( P_TRUE );
    case GUI_CONTROL_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_BROWSE:
            dlgBrowseFmtFile( gui );
            return( P_TRUE );
        case CTL_OK:
            getDlgValues( gui );
            if( dlgOpenFmtFile() ) {
                GUICloseDialog( gui );
            }
            return( P_TRUE );
        case CTL_DEFAULTS:
            setDlgDefaults( gui );
            return( P_TRUE );
        case CTL_CANCEL:
            GUICloseDialog( gui );
            return( P_TRUE );
        }
        return( P_FALSE );
    case GUI_DESTROY:
        return( P_TRUE );
    }
    return( P_FALSE );
}
