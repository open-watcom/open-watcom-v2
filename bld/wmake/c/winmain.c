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


#include <windows.h>
#include <malloc.h>
#include "wmakerc.h"


#define TMPSLEN 256

static char _class[] = "WMAKEWClass";
static HWND MainWindowHandle,OutputWindow;
static HANDLE InstanceHandle;
static char _tmpstr[TMPSLEN+1];
static int _tmpoff=0;

static int _SplitParms( char *, char ** );
extern void wmake_main( int, const char ** );
long PASCAL FAR MainDriver( HWND, unsigned, WORD, LONG );
BOOL FAR PASCAL About( HWND, unsigned, WORD, LONG );

int PASCAL WinMain( HANDLE inst, HANDLE previnst, LPSTR cmd, int show)
{
        char    **argv;
        int     argc;

        argc = _SplitParms( cmd, NULL ) + 1;
        argv = malloc( (argc+1) * sizeof( char * ) );
        if( argv == NULL ) return( FALSE );
        _SplitParms( cmd, &argv[1] );
        argv[argc] = NULL;

        if( !WindowsInit( inst, previnst, show ) ) return( FALSE );

        wmake_main( argc, argv );

        while( !MessageLoop() );

        return( FALSE );

} /* WinMain */

BOOL StartOutput( short x, short y )
{
char foo[64];

        OutputWindow = CreateWindow(
            "LISTBOX",              /* class */
            "Messages",             /* caption */
            WS_CHILD | WS_CAPTION | WS_HSCROLL | WS_VSCROLL | WS_BORDER, /* style */
            15,                     /* init. x pos */
            20,                     /* init. y pos */
            3*(x/4),                    /* init. x size */
            y/2,                    /* init. y size */
            MainWindowHandle,       /* parent window */
            NULL,                   /* menu handle */
            InstanceHandle,         /* program handle */
            NULL                    /* create parms */
            );

        if( !OutputWindow ) return( FALSE );
        ShowWindow( OutputWindow, SW_SHOWNORMAL );
        UpdateWindow( OutputWindow );
        return( TRUE );

} /* StartOutput */


/*
 * WindowsInit - windows-specific initialization
*/
int WindowsInit( HANDLE inst, HANDLE previnst, int showcmd )
{
        WNDCLASS        wc;
        LOGFONT         logfont;
        BOOL            rc;
        WORD            x,y;
        char            class[128];

        if( !previnst ) {
            wc.style = NULL;
            wc.lpfnWndProc = MainDriver;
            wc.cbClsExtra = 0;
            wc.cbWndExtra = 0;
            wc.hInstance = inst;
            wc.hIcon = LoadIcon(inst,MAKEINTRESOURCE( WMAKEICON ) );
            wc.hCursor = LoadCursor(NULL, IDC_ARROW);
            wc.hbrBackground = GetStockObject(GRAY_BRUSH);
            wc.lpszMenuName =  "WCCMenu";
            wc.lpszClassName = _class;

            rc = RegisterClass( &wc );
            if( !rc ) return( FALSE );
        } /* if */

        x = GetSystemMetrics( SM_CXSCREEN );
        y = GetSystemMetrics( SM_CYSCREEN );

        InstanceHandle = inst;

        MainWindowHandle = CreateWindow(
            _class,
            "Watcom Make for Windows",
            WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, /* Window style.                      */
            0,                              /* Default horizontal position.       */
            0,                              /* Default vertical position.         */
            x,                              /* width.                     */
            y,                              /* height.                    */
            NULL,                           /* Overlapped windows have no parent. */
            NULL,                           /* Use the window class menu.         */
            inst,                           /* This instance owns this window.    */
            NULL                            /* Pointer not needed.                */
        );

        if( !MainWindowHandle ) return( FALSE );

        ShowWindow( MainWindowHandle, showcmd );
        UpdateWindow( MainWindowHandle );

        return( StartOutput( x,y ) );

} /* WindowsInit */

long PASCAL FAR MainDriver( HWND hWnd, unsigned message, WORD wParam, LONG lParam )
{
FARPROC proc;

        /*
         * change a close message to something we like
         */
        switch (message) {
        case WM_COMMAND:
            switch( wParam ) {
            case WMAKE_ABOUT:
                proc = MakeProcInstance( About, InstanceHandle );
                DialogBox(InstanceHandle, "AboutBox", hWnd, proc);
                FreeProcInstance(proc);
                break;
            default:
                return( DefWindowProc( hWnd, message, wParam, lParam ) );
            } /* switch */
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return( DefWindowProc( hWnd, message, wParam, lParam ) );
        } /* switch */
        return (NULL);

} /* MainDriver */

BOOL FAR PASCAL About( HWND hDlg, unsigned message, WORD wParam, LONG lParam )
{
        lParam = lParam;
        switch (message) {
        case WM_INITDIALOG:
            return (TRUE);

        case WM_COMMAND:
            if (wParam == IDOK || wParam == IDCANCEL) {
                EndDialog( hDlg, TRUE );
                return( TRUE );
            } /* if */
            break;
        } /* switch */
        return( FALSE );
} /* About */

void Output( char *str )
{
        int     i;
        char    c;

        while( (c = *str) != 0 ) {
            str++;
            if( c == '\r' ) continue;
            if( c == '\n' ) {
                _tmpstr[_tmpoff] = 0;
                if( OutputWindow != NULL ) {
                    SendMessage( OutputWindow, LB_ADDSTRING, 0,
                        (LONG) (LPSTR) _tmpstr );
                } /* if */
                _tmpoff = 0;
            } else {
                _tmpstr[_tmpoff++] = c;
                if( _tmpoff >= TMPSLEN-1 ) _tmpoff--;
            } /* if */
        } /* while */
} /* Output */

int MessageLoop( void )
{
MSG msg;
WORD rc;

        while( PeekMessage( &msg, NULL, NULL, NULL, PM_NOREMOVE | PM_NOYIELD ) ) {
            rc = GetMessage( &msg, NULL, NULL, NULL );
            if( !rc ) {
                return( TRUE );
            } /* if */
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        } /* if */
        Yield();
        return( FALSE );

} /* MessageLoop */

static int _SplitParms( char *p, char **argv )
{
    int argc;
    char *start;
    char *new;

    argc = 0;
    for(;;) {
        while( *p == ' ' || *p == '\t' ) ++p; /* skip over blanks or tabs */
        if( *p == '\0' ) break;
        /* we are at the start of a parm */
        if( *p == '\"' ) {
            p++;
            new = start = p;
            for(;;) {
                /* end of parm: NULLCHAR or quote */
                if( *p == '\"' ) break;
                if( *p == '\0' ) break;
                if( *p == '\\' ) {
                    if( p[1] == '\"'  ||  p[1] == '\\' )  ++p;
                }
                if( argv ) {
                    *(new++) = *(p++);
                } else {
                    ++p;
                }
            }
        } else {
            new = start = p;
            for(;;) {
                /* end of parm: NULLCHAR or blank */
                if( *p == '\0' ) break;
                if( *p == ' '  ) break;
                if( *p == '\t' ) break;
                if(( *p == '\\' )&&( p[1] == '\"' )) {
                    ++p;
                }
                if( argv ) {
                    *(new++) = *(p++);
                } else {
                    ++p;
                }
            }
        }
        if( argv ) {
            argv[ argc ] = start;
            ++argc;

            /*
              The *new = '\0' is req'd in case there was a \" to "
              translation. It must be after the *p check against
              '\0' because new and p could point to the same char
              in which case the scan would be terminated too soon.
            */

            if( *p == '\0' ) {
                *new = '\0';
                break;
            }
            *new = '\0';
            ++p;
        } else {
            ++argc;
            if( *p == '\0' ) {
                break;
            }
            ++p;
        }
    }
    return( argc );
}

#pragma off (unreferenced);
int system( const char *cmd )
{
        return( 0 );
}

int spawnvp( int mode, const char *pgm, const char *argv[] )
{
        return( 0 );
}
