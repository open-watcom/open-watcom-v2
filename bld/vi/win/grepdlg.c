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
* Description:  Grep dialog.
*
****************************************************************************/


#include "vi.h"
#include "grep.h"

static DLGPROC  grepProc;
static HWND     grepHwnd;
static bool     cancelPressed;

/*
 * GrepDlgProc - callback routine for grep dialog
 */
BOOL WINEXP GrepDlgProc( HWND hwnd, UINT msg, UINT wparam, LONG lparam )
{
    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        return( TRUE );
    case WM_CLOSE:
        PostMessage( hwnd, WM_COMMAND, GET_WM_COMMAND_MPS( IDCANCEL, 0, 0 ) );
        return( TRUE );
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case IDCANCEL:
            cancelPressed = TRUE;
            break;
        default:
            return( FALSE );
        }
        return( TRUE );
    }
    return( FALSE );

} /* GrepDlgProc */

/*
 * InitGrepDialog - start the grep dialog
 */
void InitGrepDialog( void )
{
    grepProc = (DLGPROC) MakeProcInstance( (FARPROC) GrepDlgProc, InstanceHandle );
    cancelPressed = FALSE;
    grepHwnd = CreateDialog( InstanceHandle, "GREPDLG", Root, grepProc );

} /* InitGrepDialog */

/*
 * FiniGrepDialog - done with the grep dialog
 */
void FiniGrepDialog( void )
{
    BringWindowToTop( Root );
    SetFocus( Root );
    if( grepHwnd != NULL ) {
        DestroyWindow( grepHwnd );
    }
#ifndef __NT__
    (void)FreeProcInstance( (FARPROC) grepProc );
#endif
    grepProc = NULL;
    grepHwnd = (HWND)NULLHANDLE;

} /* FiniGrepDialog */

/*
 * SetGrepDialogFile - set the current file being used by the dialog
 */
bool SetGrepDialogFile( char *str )
{
    if( grepHwnd != NULL ) {
        SetDlgItemText( grepHwnd, GREP_CURRENT_FILE, str );
        MessageLoop( FALSE );
    }
    return( cancelPressed );

} /* SetGrepDialogFile */
