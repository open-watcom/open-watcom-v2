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
* Description:  Execution sampler option dialogs for Windows.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commonui.h"
#include "sample.h"
#include "smpstuff.h"
#include "sampwin.h"
#include "wsamprc.h"


static FARPROC  oldClassProc;
static HWND     mainWindow;
static HWND     editChild;
static bool     canContinue = false;
static char     *dataPtr;
static char     startClass[] = "WSampleStart";
static char     fileFilter[] = \
                        "Executable Files (*.EXE)" \
                        "\0" \
                        "*.EXE" \
                        "\0\0";

#define AvgXChar( tm ) (tm.tmAveCharWidth  + (7+tm.tmAveCharWidth)/8)
#define AvgYChar( tm ) (tm.tmHeight + tm.tmExternalLeading)

/*
 * getFile - use common dialog to get a file name
 */
static BOOL getFile( char *fname )
{
    OPENFILENAME        of;

    memset( &of, 0, sizeof( OPENFILENAME ) );
    of.lStructSize = sizeof( OPENFILENAME );
    of.hwndOwner = mainWindow;
    of.lpstrFilter = fileFilter;
    of.lpstrCustomFilter = NULL;
    of.nMaxCustFilter = 0L;
    of.nFilterIndex = 1L;
    of.lpstrFile = fname;
    of.nMaxFile = _MAX_PATH;
    of.lpstrFileTitle = NULL;
    of.nMaxFileTitle = 0;
    of.lpstrInitialDir = ".";
    of.Flags = OFN_NOVALIDATE;
    of.nFileOffset = 0;
    of.nFileExtension = 0;
    of.lpstrTitle = "Open Watcom Sampler - File Open";
    of.lpstrDefExt = NULL;

    return( GetOpenFileName( &of ) );

} /* getFile */

/*
 * About2 - processes messages for the about dialog.
 */
static BOOL FAR PASCAL About2( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    lparam = lparam;

    switch( msg ) {
    case WM_INITDIALOG:
        return( TRUE );

    case WM_COMMAND:
        if( wparam == IDOK ) {
            EndDialog( hwnd, TRUE );
            return( TRUE );
        }
        break;
    }
    return( FALSE );

} /* About2 */

/*
 * StartUpDriver - the main message handling loop
 */
static long FAR PASCAL StartUpDriver( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam )
{
    FARPROC     farproc;
    HWND        tmpw;
    int         len;
    char        data[_MAX_PATH];
    char        tmp[80 + _MAX_PATH];
    HINSTANCE   inst;

    switch( message ) {
    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    case WM_COMMAND:
        switch( wparam ) {
        case MSG_ABOUT:
            inst = (HINSTANCE)GetWindowWord( hwnd, GWW_HINSTANCE );
            farproc = MakeProcInstance( (FARPROC)About2, inst );
            DialogBox( inst, ResName( "AboutBox" ), hwnd, (DLGPROC)farproc );
            FreeProcInstance( farproc );
            SetFocus( editChild );
            break;

        case PUSH_GETFILES_ID:
        case MSG_GETFILES:
            data[0] = 0;
            if( getFile( data ) ) {
                GetWindowText( editChild, tmp, sizeof( tmp ) );
                strcat( tmp, data );
                SetWindowText( editChild, tmp );
            }
            SetFocus( editChild );
            break;

        case PUSH_OK_ID:
        case SELECT_ID:
            len = GetWindowTextLength( editChild ) + 1;
            if( len > _MAX_PATH )
                len = _MAX_PATH;
            GetWindowText( editChild, dataPtr, len );
            canContinue = true;
            tmpw = GetParent( editChild );
            SetWindowLong( editChild, GWL_WNDPROC, (LONG) oldClassProc );
            /* fall through, like exit was picked */

        case MSG_EXIT:
            DestroyWindow( mainWindow );
            PostQuitMessage( 0 );
            break;

        } /* switch */
        break;

    default:
        return( DefWindowProc( hwnd, message, wparam, lparam ) );

    } /* switch */
    return( 0L );

} /* StartUpDriver */

/*
 * SubClassProc - handles sub-classing of edit control
 */
static long FAR PASCAL SubClassProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam )
{

    switch( message ) {
    case WM_KEYDOWN:
        if( wparam == VK_RETURN ) {
            PostMessage( GetParent( hwnd ), WM_COMMAND, SELECT_ID, 0 );
            return( 0L );
        }
        break;
    }
    return( CallWindowProc( (WNDPROC)oldClassProc, hwnd, message, wparam, lparam ) );

} /* SubClassProc */

/*
 * GetFileName - create a window, and get file info
 */
bool GetFileName( HINSTANCE inst, int shcmd, char *fname )
{
    BOOL        rc;
    HWND        mh,win;
    HDC         dchandle;
    TEXTMETRIC  tm;
    int         x,y,avgx,avgy;
    int         xp,yp,xs,ys;
    WNDCLASS    wc;
    MSG         msg;
    HANDLE      accel;

    dataPtr = fname;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = StartUpDriver;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = inst;
    wc.hIcon = LoadIcon( inst, "ApplIcon" );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( WHITE_BRUSH );
    wc.lpszMenuName = ResName( "WSampStartMenu" );
    wc.lpszClassName = startClass;
    rc = RegisterClass( &wc );
    if( !rc ) {
        return( false );
    }

    accel = LoadAccelerators( inst, "ApplAccl" );
    if( accel == NULL ) {
        return( false );
    }

    x = GetSystemMetrics( SM_CXSCREEN );
    y = GetSystemMetrics( SM_CYSCREEN );

    mh = CreateWindow(
        startClass,             /* class */
        "Open Watcom Sampler",  /* caption */
        WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW, /* style */
        x/8,                    /* init. x pos */
        y/6,                    /* init. y pos */
        6*x/8,                  /* init. x size */
        y/2,                    /* init. y size */
        NULL,                   /* parent window */
        NULL,                   /* menu handle */
        inst,                   /* program handle */
        NULL                    /* create parms */
        );

    if( !mh ) {
        return( false );
    }
    ShowWindow( mh, shcmd );
    UpdateWindow( mh );

    dchandle = GetDC( mh );
    GetTextMetrics( dchandle, &tm );
    avgx = AvgXChar( tm );
    avgy = AvgYChar( tm );
    ReleaseDC( mh, dchandle );
    mainWindow = mh;

    xp = 32;
    yp = y/8-10;
    ys = avgy+avgy/2;
    xs = (47+1) * avgx+avgx/2;

    win = CreateWindow(
        "STATIC",               /* class */
        "Sampler Options:",     /* caption */
        WS_CHILD | ES_LEFT,
        xp+(14*avgx)-6,         /* init. x pos */
        yp-3-avgy*3/2,          /* init. y pos */
        16*avgx,                /* init. x size */
        ys,                     /* init. y size */
        mh,                     /* parent window */
        NULL,                   /* child id */
        inst,                   /* program handle */
        NULL                    /* create parms */
        );
    ShowWindow( win, SW_SHOWNORMAL );
    UpdateWindow( win );

    win = CreateWindow(
        "EDIT",                 /* class */
        NULL,                   /* caption */
        WS_CHILD | ES_LEFT | WS_BORDER | ES_AUTOHSCROLL,
        xp+7,                   /* init. x pos */
        yp,                     /* init. y pos */
        xs,                     /* init. x size */
        ys,                     /* init. y size */
        mh,                     /* parent window */
        (HMENU)EDIT_ID,         /* child id */
        inst,                   /* program handle */
        NULL                    /* create parms */
        );


    if( win == NULL ) {
        return( false );
    }
    editChild = win;

    ShowWindow( win, SW_SHOWNORMAL );
    UpdateWindow( win );

    oldClassProc = (FARPROC) GetWindowLong( editChild, GWL_WNDPROC );
    SetWindowLong( editChild, GWL_WNDPROC, (LONG) SubClassProc );

    win = CreateWindow(
        "BUTTON",               /* class */
        "&Select File...",      /* caption */
        WS_CHILD | BS_PUSHBUTTON,
        xp+(6*avgx)-6,          /* init. x pos */
        yp+avgy*3,              /* init. y pos */
        16*avgx,                /* init. x size */
        avgy+avgy/2,            /* init. y size */
        mh,                     /* parent window */
        (HMENU)PUSH_GETFILES_ID,/* child id */
        inst,                   /* program handle */
        NULL                    /* create parms */
        );
    ShowWindow( win, SW_SHOWNORMAL );
    UpdateWindow( win );

    win = CreateWindow(
        "BUTTON",               /* class */
        "S&tart Sampling",      /* caption */
        WS_CHILD | BS_PUSHBUTTON,
        xp+(29*avgx)-6,         /* init. x pos */
        yp+avgy*3,              /* init. y pos */
        16*avgx,                /* init. x size */
        avgy+avgy/2,            /* init. y size */
        mh,                     /* parent window */
        (HMENU)PUSH_OK_ID,      /* child id */
        inst,                   /* program handle */
        NULL                    /* create parms */
        );
    ShowWindow( win, SW_SHOWNORMAL );
    UpdateWindow( win );

    SetFocus( editChild );

    while( GetMessage( &msg, NULL, 0, 0 ) ) {
        if( !TranslateAccelerator( mainWindow, accel, &msg ) ) {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }

    return( canContinue );

} /* GetFileName */
