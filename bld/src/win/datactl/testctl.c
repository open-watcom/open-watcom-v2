#include <stdio.h>
#include <windows.h>
#include "testctl.h"
#include "ctltype.h"            /* data control library stuff */
#include "dialog.h"             /* control ids for the dialog */
#include "stddef.h"             /* defines offsetof macro */

/*
 * GetDcomboData: Data fetch routine for the dcombo control (dynamic combo box).
 *
*/
char *GetDcomboData( int elt )
{
    switch( elt ) {             // index is always passed in 0 origin
    case 0:
        return( "This" );

    case 1:
        return( "is" );

    case 2:
        return( "a" );

    case 3:
        return( "Dynamic" );

    case 4:
        return( "Combo" );

    case 5:
        return( "Box" );
    }

    return( NULL );
}

typedef struct {
    int         list_1;
    int         list_2;
    BOOL        check;
    int         radio;
    char        text[100];
    int         number;
    float       value;
    int         list_3;
    int         range_num;
    float       range_value;
} dlg_data;

static dlg_data Data={
    1, 0, TRUE, 1, "Bananas", 123, 123.456, 2, 10, -3
};

#include "testdata.h"           // This file defines the Data control


static char TestCtlClass[32]="TestCtlClass";

static BOOL FirstInstance( HINSTANCE );
static BOOL AnyInstance( HINSTANCE, int, LPSTR );

long _EXPORT FAR PASCAL WindowProc( HWND, unsigned, UINT, LONG );

/*
 * WinMain - initialization, message loop
 */
int PASCAL WinMain( HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline,
                    int cmdshow )
{
    MSG         msg;

    prev_inst = prev_inst;
#ifdef __WINDOWS_386__
    sprintf( TestCtlClass,"TestCtlClass%d",this_inst );
#else
    if( !prev_inst )
#endif
        if( !FirstInstance( this_inst ) ) return( FALSE );

    if( !AnyInstance( this_inst, cmdshow, cmdline ) ) return( FALSE );

    while( GetMessage( &msg, (HWND)0, 0, 0 ) ) {

        TranslateMessage( &msg );
        DispatchMessage( &msg );

    }

    return( msg.wParam );

} /* WinMain */

/*
 * FirstInstance - register window class for the application,
 *                 and do any other application initialization
 */
static BOOL FirstInstance( HINSTANCE this_inst )
{
    WNDCLASS    wc;
    BOOL        rc;

    /*
     * set up and register window class
     */
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (LPVOID) WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = this_inst;
    wc.hIcon = LoadIcon( this_inst, IDI_APPLICATION );
    wc.hCursor = LoadCursor( (HINSTANCE)0, IDC_ARROW );
    wc.hbrBackground = GetStockObject( WHITE_BRUSH );
    wc.lpszMenuName = "TestCtlMenu";
    wc.lpszClassName = TestCtlClass;
    rc = RegisterClass( &wc );
    return( rc );

} /* FirstInstance */

/*
 * AnyInstance - do work required for every instance of the application:
 *                create the window, initialize data
 */
static BOOL AnyInstance( HINSTANCE this_inst, int cmdshow, LPSTR cmdline )
{
    HWND        wind_handle;

    cmdline = cmdline;

    /*
     * create main window
     */
    wind_handle = CreateWindow(
        TestCtlClass,           /* class */
        "Open Watcom Data Control Test Program",     /* caption */
        WS_OVERLAPPEDWINDOW,    /* style */
        CW_USEDEFAULT,          /* init. x pos */
        CW_USEDEFAULT,          /* init. y pos */
        CW_USEDEFAULT,          /* init. x size */
        CW_USEDEFAULT,          /* init. y size */
        (HWND)0,                /* parent window */
        (HMENU)0,               /* menu handle */
        this_inst,              /* program handle */
        NULL                    /* create parms */
        );

    if( !wind_handle ) return( FALSE );

    /*
     * display window
     */
    ShowWindow( wind_handle, cmdshow );
    UpdateWindow( wind_handle );

    return( TRUE );

} /* AnyInstance */

/*
 * AboutDlgProc - processes messages for the about dialogue.
 */
BOOL _EXPORT FAR PASCAL About( HWND window_handle, unsigned msg,
                                UINT wparam, LONG lparam )
{
    lparam = lparam;                    /* turn off warning */
    window_handle = window_handle;

    switch( msg ) {
    case WM_INITDIALOG:
        return( TRUE );

    case WM_COMMAND:
        if( LOWORD( wparam ) == IDOK ) {
            EndDialog( window_handle, TRUE );
            return( TRUE );
        }
        break;
    }
    return( FALSE );

} /* AboutDlgProc */

/*
 * DataCtlProc - processes messages for the Data Control Dialog
 */
BOOL _EXPORT FAR PASCAL DataCtlProc( HWND window_handle, unsigned msg,
                                UINT wparam, LONG lparam )
{
    HINSTANCE           hinst;

    switch( msg ) {
    case WM_INITDIALOG:
        /* get the controls in the dialog initialized with the correct
           settings based on the control data */
        hinst = GET_HINST( window_handle );
        ctl_dlg_init( hinst, window_handle, &Data, &CtlData );
        return( TRUE );         // let WINDOWS set the input focus

    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case IDOK:
            hinst = GET_HINST( window_handle );
            if( !ctl_dlg_done( hinst, window_handle, &Data, &CtlData ) ) {
                /* some control on the dialog is set with invalid data...
                   must continue processing until they enter valid data */
                return( TRUE );
            }
            /* DROP DOWN INTO THE IDCANCEL CASE when the dialog verifies ok */

        case IDCANCEL:
            EndDialog( window_handle, TRUE );         /* Exits the dialog box        */
            return( TRUE );
        }
        /* let the data control library look at the message to see if any
           controls have been modified */
        ctl_dlg_process( &CtlData, wparam, lparam );

    }

    return( FALSE );
} /* DataCtlProc */

/*
 * WindowProc - handle messages for the main application window
 */
LONG _EXPORT FAR PASCAL WindowProc( HWND window_handle, unsigned msg,
                                     UINT wparam, LONG lparam )
{
    FARPROC     proc;
    HANDLE      inst_handle;

    switch( msg ) {
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case MENU_ABOUT:
            inst_handle = GET_HINST( window_handle );
            proc = MakeProcInstance( (FARPROC)About, inst_handle );
            DialogBox( inst_handle,"AboutBox", window_handle, (DLGPROC)proc );
            FreeProcInstance( proc );
            break;

        case MENU_DIALOG:
            inst_handle = GET_HINST( window_handle );
            proc = MakeProcInstance( (FARPROC)DataCtlProc, inst_handle );
            DialogBox( inst_handle,"DATACTL", window_handle, (DLGPROC)proc );
            FreeProcInstance( proc );
            break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    default:
        return( DefWindowProc( window_handle, msg, wparam, lparam ) );
    }
    return( 0L );

} /* WindowProc */
