/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include <wwindows.h>
#include "global.h"
#include "fmedit.def"
#include "state.def"
#include "grid.def"
#include "fmdlgs.rh"


bool SnapRectToGrid( RECT *rec )
/******************************/
{
    /*  Make sure the passed rectangle aligns with the user-specified
     *  resize grid.
     */
    int     inc;
    int     rnd;
    int     size;
    bool    changed;
    int     new;

    changed = false;
    inc = GetResizeVInc();
    rnd = inc - 1;
    size = rec->bottom - rec->top - 1;    /* exclude borders */
    new = ((size + rnd) / inc) * inc;
    if( new != size ) {
        changed = true;
        rec->bottom = rec->top + new;
    }
    inc = GetResizeHInc();
    rnd = inc - 1;
    size = rec->right - rec->left - 1;
    new = ((size + rnd) / inc) * inc;
    if( new != size ) {
        changed = true;
        rec->right = rec->left + new;
    }
    return( changed );
}


static void DoPointSnap( POINT *pt, int vinc, int hinc )
/******************************************************/
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


INT_PTR CALLBACK FMGridDlgProc( HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam )
/*************************************************************************************/
{
    /* Processes messages for "Grid" dialog box */
    RECT        rect;
    HANDLE      appwnd;
    unsigned    inc;
    bool        ret;
    BOOL        translated;

    lParam = lParam;                 /* reference to avoid warning */

    ret = false;
    switch( message ) {
    case WM_INITDIALOG:
        InheritState( hdlg );
        SetDlgItemInt( hdlg, ID_VPREC, GetVerticalInc(), FALSE );
        SetDlgItemInt( hdlg, ID_HPREC, GetHorizontalInc(), FALSE );
        ret = true;
        break;
    case WM_COMMAND:
        InitState( hdlg );
        switch( LOWORD( wParam ) ) {
        case IDOK:
            inc = GetDlgItemInt( hdlg, ID_VPREC, &translated, FALSE );
            if( translated && inc >= 1 && inc <= 100 ) {
                SetVerticalInc( inc );
                inc = GetDlgItemInt( hdlg, ID_HPREC, &translated, FALSE );
                if( translated && inc >= 1 && inc <= 100 ) {
                    SetHorizontalInc( inc );
                    EndDialog( hdlg, TRUE );
                } else {
                    MessageBox( hdlg,
                                "The horizontal precision must be an integer "
                                    "between 1 and 100",
                                NULL,
                                MB_ICONEXCLAMATION | MB_OK );
                    SetFocus( GetDlgItem( hdlg, ID_HPREC ) );
                }
            } else {
                MessageBox( hdlg,
                            "The vertical precision must be an integer "
                               "between 1 and 100",
                            NULL,
                            MB_ICONEXCLAMATION | MB_OK );
                SetFocus( GetDlgItem( hdlg, ID_VPREC ) );
            }
            ret = true;
            break;
        case IDCANCEL:
            EndDialog( hdlg, FALSE );
            ret = true;
            break;
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
    return( ret );
}
