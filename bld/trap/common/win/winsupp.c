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


#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <windows.h>
#include "winserv.h"
#include "packet.h"
#include "trperr.h"


extern int RemoteLinkObtained;

extern HANDLE CProcHandle;
extern HWND MainWindowHandle;
extern HWND DebugWindow;

HWND EditChild,SelectChild;
FARPROC OldEditProc;

extern char DefLinkName[];

/*
 * SetAppl - show initial selection state
 */
void SetAppl()
{
    SetWindowText( EditChild, DefLinkName );
} /* SetAppl */


/*
 * CreateSelections
 */
BOOL CreateSelections( short x, short y, short avgx, short avgy )
{
HWND win;
int xp,yp,xs,ys;

    xp = 15 + (x/4);
    yp = 40 + y/2;
    ys = avgy+avgy/2;
    xs = (30+1) * avgx+avgx/2;

    win = CreateWindow(
        "STATIC",                  /* class */
        TRP_WIN_server_name,      /* caption */
        WS_CHILD | ES_LEFT,
        xp-(14*avgx),                     /* init. x pos */
        yp+3,                     /* init. y pos */
        13*avgx,                     /* init. x size */
        ys,                     /* init. y size */
        MainWindowHandle,       /* parent window */
        NULL,                   /* child id */
        CProcHandle,            /* program handle */
        NULL                    /* create parms */
        );
    ShowWindow( win, SW_SHOWNORMAL );
    UpdateWindow( win );

    win = CreateWindow(
        "EDIT",                  /* class */
        NULL,                   /* caption */
        WS_CHILD | ES_LEFT | WS_BORDER | ES_AUTOHSCROLL,
        xp,                     /* init. x pos */
        yp,                     /* init. y pos */
        xs,                     /* init. x size */
        ys,                     /* init. y size */
        MainWindowHandle,       /* parent window */
        EDIT_1,                  /* child id */
        CProcHandle,            /* program handle */
        NULL                    /* create parms */
        );

    if( win == NULL ) return( FALSE );
    EditChild = win;

    SendMessage( win, EM_LIMITTEXT, MAX_SERVER_NAME, 0L );
    ShowWindow( win, SW_SHOWNORMAL );
    UpdateWindow( win );

    ys = avgy+avgy/2;
    xs = (30+1) * avgx+avgx/2;
    win = CreateWindow(
        "BUTTON",                  /* class */
        TRP_WIN_register_name,                /* caption */
        WS_CHILD | BS_PUSHBUTTON,
        xp,                     /* init. x pos */
        yp+avgy*2,                     /* init. y pos */
        avgx*35,                     /* init. x size */
        avgy*1+2*avgy/3,                     /* init. y size */
        MainWindowHandle,            /* parent window */
        SELECT_1,              /* child id */
        CProcHandle,            /* program handle */
        NULL                    /* create parms */
        );

    if( win == NULL ) return( FALSE );
    SelectChild = win;
    ShowWindow( win, SW_SHOWNORMAL );
    UpdateWindow( win );

    return( TRUE );

} /* CreateSelections */


/*
 * ProcAppl - receives server specific messages for the main window
 */
void ProcAppl( HWND windhandle, unsigned message, WORD worddata, LONG longdata )
{
    char        name[80];
    const char  *err;
    int         i;
    char        buff[80];

    windhandle = windhandle; message = message; longdata = longdata;
    switch( worddata ) {
    case SELECT_1:
        i = GetWindowTextLength( EditChild );
        if( i == 0 ) {
            Output( TRP_ERR_null_name );
            Output( "\r\n" );
        } else {
            *name = '\0';
            GetWindowText( EditChild, name, sizeof( name ) );
            if( RemoteLinkObtained )
                RemoteUnLink();
            err = RemoteLink( name, TRUE );
            if( err != NULL ) {
                StartupErr( err );
                RemoteLinkObtained = FALSE;
            } else {
                sprintf( buff, TRP_WIN_setting_name, name );
                Output( buff );
                Output( "\r\n" );
                RemoteLinkObtained = TRUE;
            } /* if */
        } /* if */
        break;
    } /* switch */

} /* ProcAppl */
