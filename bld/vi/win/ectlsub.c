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


#include "winvi.h"
#include <string.h>
#include <ctype.h>
#include "keys.h"

static FARPROC      oldEditProc;
static FARPROC      editProc;
static history_data *hData;
static int      currHist;

/*
 * setEditText - set the specified edit window's text
 */
static void setEditText( HWND hwnd, char *tmp )
{
    int     len;

    if( tmp == NULL ) {
    return;
    }
    len = strlen( tmp );
    SetWindowText( hwnd, tmp );
    SendMessage( hwnd, EM_SETSEL, GET_EM_SETSEL_MPS( len, len ) );

} /* setEditText */

/*
 * insertEditText - insert some text into the current edit window
 */
static void insertEditText( HWND hwnd, char *tmp )
{
    SendMessage( hwnd, EM_REPLACESEL, 0, (LONG) tmp );
} /* insertEditText */

/*
 * handleKey - handle a key press
 */
static bool handleKey( HWND hwnd, int ch, bool process )
{
    char    tmp[MAX_INPUT_LINE];

    switch( ch ) {
    case VI_KEY( ALT_O ):
    case VI_KEY( CTRL_O ):
        if( process ) {
        GetWindowText( hwnd, tmp, sizeof( tmp ) );
        InsertTextForSpecialKey( ch, tmp );
    }
    break;
    case VI_KEY( CTRL_R ):
    if( !SelRgn.selected || ( SelRgn.lines && ( SelRgn.start_line != SelRgn.end_line ) ) ) {
        return( FALSE );
    }
    case VI_KEY( CTRL_W ):
    case VI_KEY( CTRL_E ):
    case VI_KEY( ALT_L ):
    case VI_KEY( CTRL_L ):
    case VI_KEY( CTRL_D ):
        if( process ) {
        if( GetTextForSpecialKey( sizeof( tmp ), ch, tmp ) ) {
        insertEditText( hwnd, tmp );
        }
    }
    break;
    case VI_KEY( CTRL_INS ):
        if( process ) {
        SendMessage( hwnd, EM_UNDO, 0, 0L );
    }
    break;
    case VI_KEY( UP ):
        if( process ) {
        currHist--;
        if( currHist < 0 || currHist < (hData->curr - hData->max) ) {
        currHist = hData->curr-1;
        }
        setEditText( hwnd, hData->data[ currHist % hData->max ] );
    }
    break;
    case VI_KEY( DOWN ):
        if( process ) {
        currHist++;
        if( currHist >= hData->curr ) {
        currHist = hData->curr - hData->max;
        if( currHist < 0 ) {
            currHist = 0;
        }
        }
        setEditText( hwnd, hData->data[ currHist % hData->max ] );
    }
    break;
    case VI_KEY( CTRL_F ):
    case VI_KEY( CTRL_B ):
        if( process ) {
    }
        break;
    default:
    return( FALSE );
    break;
    }
    return( TRUE );

} /* handleKey */

/*
 * EditSubClassProc - handle keystrokes for an edit control
 */
long WINEXP EditSubClassProc( HWND hwnd, UINT msg, UINT wparam, LONG lparam )
{
    int     ch;

    switch( msg ) {
    case WM_KEYDOWN:
    ch = MapVirtualKeyToVIKey( wparam, HIWORD( lparam ) );
    if( handleKey( hwnd, ch, TRUE ) ) {
        return( 0L );
    }
    break;
    case WM_CHAR:
    if( handleKey( hwnd, wparam, FALSE ) ) {
        return( 0L );
    }
    break;
    }
    return( CallWindowProc( (LPVOID) oldEditProc, hwnd, msg, wparam, lparam ) );

} /* EditSubClassProc */

/*
 * EditSubClass - sub-class an edit control in a dialog
 */
void EditSubClass( HWND hwnd, int id, history_data *h )
{
    HWND    edit;

    hData = h;
    currHist = h->curr;
    edit = GetDlgItem( hwnd, id );
    oldEditProc = (FARPROC) GetWindowLong( edit, GWL_WNDPROC );
    editProc = MakeProcInstance( (FARPROC) EditSubClassProc, InstanceHandle );
    SetWindowLong( edit, GWL_WNDPROC, (LONG) editProc );
    SendMessage( edit, EM_LIMITTEXT, MAX_INPUT_LINE, 0L );

} /* EditSubClass */

/*
 * RemoveEditSubClass - remove the sub-class of an edit control in a dialog
 */
void RemoveEditSubClass( HWND hwnd, int id )
{
    HWND    edit;

    edit = GetDlgItem( hwnd, id );
    SetWindowLong( edit, GWL_WNDPROC, (LONG) oldEditProc );
    (void)FreeProcInstance( editProc );
    FinishFileComplete();

} /* RemoveEditSubClass */
