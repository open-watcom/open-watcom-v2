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
* Description:  Routines to support creation of object on a specified grid.
*
****************************************************************************/


#include <windows.h>
#include "global.h"
#include "fmedit.def"
#include "state.def"
#include "grid.def"
#include "fmdlgs.h"

extern BOOL SnapRectToGrid( RECT *rec )
/*************************************/
{
    /*  Make sure the passed rectangle aligns with the user-specified
     *  resize grid.
     */
    int     inc;
    int     rnd;
    int     size;
    BOOL    changed;
    int     new;

    changed = FALSE;
    inc = GetResizeVInc();
    rnd = inc - 1;
    size = rec->bottom - rec->top - 1;    /* exclude borders */
    new = ((size + rnd) / inc) * inc;
    if( new != size ) {
        changed = TRUE;
        rec->bottom = rec->top + new;
    }
    inc = GetResizeHInc();
    rnd = inc - 1;
    size = rec->right - rec->left - 1;
    new = ((size + rnd) / inc) * inc;
    if( new != size ) {
        changed = TRUE;
        rec->right = rec->left + new;
    }
    return( changed );
}


static void DoPointSnap( POINT *pt, unsigned vinc, unsigned hinc )
/****************************************************************/
{
    pt->y = ((pt->y + (vinc >> 1)) / vinc) * vinc;
    pt->x = ((pt->x + (hinc >> 1)) / hinc) * hinc;
}


extern void SnapPointToGrid( POINT *pt )
/**************************************/
{
    /* snap the given point to the current grid */
    DoPointSnap( pt, GetVerticalInc(), GetHorizontalInc() );
}

extern void SnapPointToResizeGrid( POINT *pt )
/********************************************/
{
    /* snap the given point to the current grid */
    DoPointSnap( pt, GetResizeVInc(), GetResizeHInc() );
}


WINEXPORT BOOL CALLBACK FMGrid( HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam )
/*************************************************************************************/
{
    /* Processes messages for "Grid" dialog box */
    RECT        rect;
    HANDLE      appwnd;
    unsigned    inc;
    BOOL        ret;

    lParam = lParam;                 /* reference to avoid warning */
    switch( message ) {
    case WM_INITDIALOG:
        InheritState( hdlg );
        SetDlgItemInt( hdlg, ID_VPREC, GetVerticalInc(), FALSE );
        SetDlgItemInt( hdlg, ID_HPREC, GetHorizontalInc(), FALSE );
        return( TRUE );
    case WM_COMMAND:
        InitState( hdlg );
        switch( LOWORD( wParam ) ) {
        case IDOK:
            inc = GetDlgItemInt( hdlg, ID_VPREC, &ret, FALSE );
            if( ret && inc >= 1 && inc <= 100 ) {
                SetVerticalInc( inc );
            } else {
                MessageBox( hdlg,
                            "The vertical precision must be an integer "
                               "between 1 and 100",
                            NULL,
                            MB_ICONEXCLAMATION | MB_OK );
                SetFocus( GetDlgItem( hdlg, ID_VPREC ) );
                return( TRUE );
            }
            inc = GetDlgItemInt( hdlg, ID_HPREC, &ret, FALSE );
            if( ret && inc >= 1 && inc <= 100 ) {
                SetHorizontalInc( inc );
            } else {
                MessageBox( hdlg,
                            "The horizontal precision must be an integer "
                                "between 1 and 100",
                            NULL,
                            MB_ICONEXCLAMATION | MB_OK );
                SetFocus( GetDlgItem( hdlg, ID_HPREC ) );
                return( TRUE );
            }
            EndDialog( hdlg, TRUE );
            return( TRUE );
        case IDCANCEL:
            EndDialog( hdlg, FALSE );
            return( TRUE );
        }
        break;
    case WM_MOVE:
        InitState( hdlg );
        appwnd = GetAppWnd();
        GetWindowRect( hdlg, &rect );
        InvalidateRect( appwnd, &rect, TRUE );
        UpdateWindow( appwnd );
        break;
    }
    return( FALSE );
}
