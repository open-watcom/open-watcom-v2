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
#include "winvi.h"
#include "source.h"
#include "setscr.h"
#include "stddef.h"
#include "ctltype.h"
#include "util.h"

#define FILEENDSTRINGWIDTH      200

typedef struct {
    BOOL        JumpyScroll;
    BOOL        LineBased;
    int         PageLinesExposed;
    char        FileEndString[ FILEENDSTRINGWIDTH ];
    BOOL        SavePosition;
    BOOL        AutoMessageClear;
} dlg_data;

static  dlg_data    dlgData;

#include "setscr.ch"

static void globalTodlgData( void )
{
    dlgData.JumpyScroll = EditFlags.JumpyScroll;
    dlgData.LineBased = EditFlags.LineBased;
    dlgData.PageLinesExposed = PageLinesExposed;
    strncpy( dlgData.FileEndString, FileEndString, FILEENDSTRINGWIDTH - 1 );
    dlgData.SavePosition = EditFlags.SavePosition;
    dlgData.AutoMessageClear = EditFlags.AutoMessageClear;
}

static void dlgDataToGlobal( void )
{
    UtilUpdateBoolean( EditFlags.JumpyScroll, dlgData.JumpyScroll, "jumpyscroll" );
    UtilUpdateBoolean( EditFlags.LineBased, dlgData.LineBased, "linebased" );
    UtilUpdateBoolean( EditFlags.SavePosition, dlgData.SavePosition, "saveposition" );
    UtilUpdateBoolean( EditFlags.AutoMessageClear, dlgData.AutoMessageClear, "automessageclear" );
    UtilUpdateInt( PageLinesExposed, dlgData.PageLinesExposed, "pagelinesexposed" );
    UtilUpdateStr( FileEndString, dlgData.FileEndString, "fileendstring" );
}

static void setdlgDataDefaults( void )
{
    // this sort of suck ssince the default values aren't isolated in 1 place
    dlgData.JumpyScroll = TRUE;
    dlgData.LineBased = FALSE;
    dlgData.PageLinesExposed = 1;
    dlgData.FileEndString[ 0 ] = '\0';
    dlgData.SavePosition = TRUE;
    dlgData.AutoMessageClear = TRUE;
}

/*
 * SetScrProc - processes messages for the Data Control Dialog
 */
BOOL WINEXP SetScrProc( HWND hwndDlg, unsigned msg,
                        WORD wParam, LONG lParam )
{
    switch( msg ) {
    case WM_INITDIALOG:
        CenterWindowInRoot( hwndDlg );
        globalTodlgData();
        ctl_dlg_init( GET_HINSTANCE( hwndDlg ), hwndDlg,
                      &dlgData, &Ctl_setscr );
        return( TRUE );

    case WM_COMMAND:
        switch( LOWORD( wParam ) ) {
        case SETSCR_DEFAULTS:
            setdlgDataDefaults();
            ctl_dlg_reset( GET_HINSTANCE( hwndDlg ),
                           hwndDlg, &dlgData, &Ctl_setscr, TRUE );
            return( TRUE );
        case IDOK:
            if( !ctl_dlg_done( GET_HINSTANCE( hwndDlg ),
                               hwndDlg, &dlgData, &Ctl_setscr ) ) {
                return( TRUE );
            }
            dlgDataToGlobal();

            // fall through
        case IDCANCEL:
            EndDialog( hwndDlg, TRUE );
            return( TRUE );
        }

        ctl_dlg_process( &Ctl_setscr, wParam, lParam );
    }

    return( FALSE );
}

/*
 * GetSetScrDialog - create dialog box & get result
 */
bool GetSetScrDialog( void )
{
    DLGPROC     proc;
    bool        rc;

    proc = (DLGPROC) MakeProcInstance( (FARPROC) SetScrProc, InstanceHandle );
    rc = DialogBox( InstanceHandle, "SETSCR", Root, proc );
    FreeProcInstance( (FARPROC) proc );

    // redisplay all files to ensure screen completely correct
    ReDisplayBuffers( FALSE );
    return( rc );
} /* GetSetScrDialog */
