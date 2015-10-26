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


#include "commonui.h"
#include <stdio.h>
#include "wddespy.h"

/* position of the list window within the main window */

#define SPY_X   0

/*
 * CreateListBox - create the list box for displaying results
 */
void CreateListBox( HWND parent, ListBoxInfo *info )
{
#if defined( __NT__ )
    if( LOBYTE( LOWORD( GetVersion() ) ) >= 4 ) {
        info->box = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            "LISTBOX",                      /* Window class name */
            "Messages",                     /* Window caption */
            WS_CHILD | LBS_NOTIFY |
            WS_VSCROLL | WS_HSCROLL |
            LBS_NOINTEGRALHEIGHT,
            SPY_X,                          /* Initial X position */
            info->ypos,                     /* Initial Y position */
            0,                              /* Initial X size */
            0,                              /* Initial Y size */
            parent,                         /* Parent window handle */
            NULL,                           /* Window menu handle */
            Instance,                       /* Program instance handle */
            NULL );                         /* Create parameters */
    } else {
#endif
        info->box = CreateWindow(
            "LISTBOX",                      /* Window class name */
            "Messages",                     /* Window caption */
            WS_CHILD | LBS_NOTIFY |
            WS_VSCROLL | WS_HSCROLL |
            LBS_NOINTEGRALHEIGHT|
            WS_BORDER,                      /* Window style */
            SPY_X,                          /* Initial X position */
            info->ypos,                     /* Initial Y position */
            0,                              /* Initial X size */
            0,                              /* Initial Y size */
            parent,                         /* Parent window handle */
            NULL,                           /* Window menu handle */
            Instance,                       /* Program instance handle */
            NULL );                         /* Create parameters */
#if defined( __NT__ )
    }
#endif
    ShowWindow( info->box, SW_NORMAL );
    UpdateWindow( info->box );
    SendMessage( info->box, WM_SETFONT, (WPARAM)GetMonoFont(), 0L );

} /* CreateListBox */

/*
 * ResizeListBox - make list box new size, based on height/width of parent
 *                 client area
 */
void ResizeListBox( WORD width, WORD height, ListBoxInfo *info )
{
    int     nheight;
    int     nwidth;

    nheight = height - info->ypos - info->hinthite;
    if( nheight < 0 ) {
        nheight = 0;
    }

    nwidth = width - SPY_X;
    if( nwidth < SPY_X ) {
        nwidth = SPY_X;
    }

    MoveWindow( info->box, SPY_X, info->ypos, nwidth, nheight, TRUE );
    /*
     * This is a kludge to force the window to refresh properly
     * when it is scrolled horizontally.
     */
    if( info->old_area.right - info->old_area.left < width ) {
        InvalidateRect( info->box, &info->old_area, TRUE );
        UpdateWindow( info->box );
    }
    info->old_area.top = 0;
    info->old_area.left = 0;
    info->old_area.right = nwidth;
    info->old_area.bottom = nheight;

} /* ResizeListBox */
