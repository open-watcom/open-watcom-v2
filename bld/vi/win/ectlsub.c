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


#include "vi.h"
#include "wclbproc.h"


/* Local Windows CALLBACK function prototypes */
WINEXPORT LRESULT CALLBACK EditSubClassProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

static WNDPROC      oldEditProc;
static WNDPROC      editProc;
static history_data *histData;
static int          currHist;

/*
 * setEditText - set the specified edit window's text
 */
static void setEditText( HWND hwnd, const char *tmp )
{
    size_t  len;

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
    SendMessage( hwnd, EM_REPLACESEL, 0, (LPARAM)(LPSTR)tmp );

} /* insertEditText */

/*
 * handleKey - handle a key press
 */
static bool handleKey( HWND hwnd, vi_key key, bool process )
{
    char    tmp[MAX_INPUT_LINE];

    switch( key ) {
    case VI_KEY( ALT_O ):
    case VI_KEY( CTRL_O ):
        if( process ) {
            GetWindowText( hwnd, tmp, sizeof( tmp ) );
            InsertTextForSpecialKey( key, tmp );
        }
        break;
    case VI_KEY( CTRL_R ):
        if( !SelRgn.selected ||
            (SelRgn.lines && (SelRgn.start.line != SelRgn.end.line)) ) {
            return( false );
        }
    case VI_KEY( CTRL_W ):
    case VI_KEY( CTRL_E ):
    case VI_KEY( ALT_L ):
    case VI_KEY( CTRL_L ):
        if( process ) {
            if( GetTextForSpecialKey( key, tmp, sizeof( tmp ) ) ) {
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
        if( process && histData->curr > 0 ) {
            currHist--;
            if( currHist < 0 || currHist < (histData->curr - histData->max) ) {
                currHist = histData->curr - 1;
            }
            setEditText( hwnd, histData->data[currHist % histData->max] );
        }
        break;
    case VI_KEY( DOWN ):
        if( process && histData->curr > 0 ) {
            currHist++;
            if( currHist >= histData->curr ) {
                currHist = histData->curr - histData->max;
                if( currHist < 0 ) {
                    currHist = 0;
                }
            }
            setEditText( hwnd, histData->data[currHist % histData->max] );
        }
        break;
    case VI_KEY( CTRL_F ):
    case VI_KEY( CTRL_B ):
        if( process ) {
        }
        break;
    default:
        return( false );
    }
    return( true );

} /* handleKey */

/*
 * EditSubClassProc - handle keystrokes for an edit control
 */
WINEXPORT LRESULT CALLBACK EditSubClassProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    vi_key      key;

    switch( msg ) {
    case WM_KEYDOWN:
        key = MapVirtualKeyToVIKey( LOWORD( wparam ), HIWORD( lparam ) );
        if( handleKey( hwnd, key, true ) ) {
            return( 0L );
        }
        break;
    case WM_CHAR:
        if( handleKey( hwnd, wparam, false ) ) {
            return( 0L );
        }
        break;
    }
    return( CallWindowProc( oldEditProc, hwnd, msg, wparam, lparam ) );

} /* EditSubClassProc */

/*
 * EditSubClass - sub-class an edit control in a dialog
 */
void EditSubClass( HWND hwnd, int id, history_data *h )
{
    HWND    edit;

    histData = h;
    currHist = h->curr;
    edit = GetDlgItem( hwnd, id );
    oldEditProc = (WNDPROC)GET_WNDPROC( edit );
    editProc = MakeProcInstance_WND( EditSubClassProc, InstanceHandle );
    SET_WNDPROC( edit, (LONG_PTR)editProc );
    SendMessage( edit, EM_LIMITTEXT, MAX_INPUT_LINE, 0L );

} /* EditSubClass */

/*
 * RemoveEditSubClass - remove the sub-class of an edit control in a dialog
 */
void RemoveEditSubClass( HWND hwnd, int id )
{
    HWND    edit;

    edit = GetDlgItem( hwnd, id );
    SET_WNDPROC( edit, (LONG_PTR)oldEditProc );
    FreeProcInstance_WND( editProc );
    FinishFileComplete();

} /* RemoveEditSubClass */
