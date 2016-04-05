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


#include "heapwalk.h"


/* Local Window callback functions prototypes */
BOOL __export FAR PASCAL PaintAll( HWND hwnd, LPARAM lparam );

static FARPROC  PaintFP;

/* position of the list window within the main window */

#define SPY_X   0
#define SPY_Y   15      /* must be greater than or equal to 15 */

/*
 * PositionListBox - position the list box so it doesn't interfere
 *                      with the push window title bar
 */

void PositionListBox( ListBoxInfo *info, HWND parent ) {

    RECT        rect;

    if( info->title == NULL ) {
        info->ypos = 0;
    } else {
        GetWindowRect( info->title[1], &rect );
        info->ypos = rect.bottom - rect.top + 1;
    }
    if( info->box != NULL ) {
        GetClientRect( parent, &rect );
        ResizeListBox( rect.right - rect.left, rect.bottom - rect.top, info );
    }
}

/*
 * CreateListBox - create the list box for displaying results
 */
void CreateListBox( HWND parent, ListBoxInfo *info, WORD type  )
{

    info->box = NULL;
    PositionListBox( info, parent );
    if( type == GLOBAL_LB ) {
        info->box = CreateTextBox( Instance, parent, GetMonoFont(),
                                    (HMENU)HEAPEX_LIST, GetGlobalTextItem, 0 );
    } else {
        info->box = CreateTextBox( Instance, parent, GetMonoFont(),
                                (HMENU)HEAPEX_LIST, GetLocalTextItem, LIST_NOSELECT );
    }
} /* CreateListBox */

/*
 * ResizeListBox - make list box new size, based on height/width of parent
 *                client area.
 */
void ResizeListBox( WORD width, WORD height, ListBoxInfo *info )
{
    int     nheight;
    int     nwidth;

    nheight = height - info->ypos;
    if( nheight < 0 )
        nheight = 0;

    nwidth = width - SPY_X;
    if( nwidth < SPY_X )
        nwidth = SPY_X;

    MoveWindow( info->box, SPY_X, info->ypos, nwidth, nheight, TRUE );
} /* ResizeListBox */

BOOL FAR PASCAL PaintAll( HWND hwnd, LPARAM lparam )
{
    lparam = lparam;
    RedrawWindow( hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE );
    return( TRUE );
}

/*
 * InitPaintProc - we need to always keep an instance of this procedure
 *                 because it is usually needed when memory is too scarce
 *                 to allow it to be created on the fly
 */

void InitPaintProc( void ) {

    PaintFP = MakeProcInstance( (FARPROC)PaintAll, Instance );
}

void FiniPaintProc( void ) {
    FreeProcInstance( PaintFP );
}

/*
 * PaintListBox - force all windows to be painted in case our
 *                font has been dumped
 */

void PaintAllWindows( void ) {

    HTASK       task;

    task = GetCurrentTask();
    EnumTaskWindows( task, (WNDENUMPROC)PaintFP, 0 );
}
