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

static BOOL FirstInstance( HANDLE );
static BOOL AnyInstance( HANDLE, int, LPSTR );
long _EXPORT FAR PASCAL WindowProc( HWND, unsigned, WORD, LONG );

/*
 * WinMain - initialization, message loop
 */
int PASCAL WinMain( HANDLE this_inst, HANDLE prev_inst, LPSTR cmdline,
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

    while( GetMessage( &msg, NULL, NULL, NULL ) ) {

        TranslateMessage( &msg );
        DispatchMessage( &msg );

    }

    return( msg.wParam );

} /* WinMain */

/*
 * FirstInstance - register window class for the application,
 *                 and do any other application initialization
 */
static BOOL FirstInstance( HANDLE this_inst )
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
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
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
static BOOL AnyInstance( HANDLE this_inst, int cmdshow, LPSTR cmdline )
{
    HWND        wind_handle;

    cmdline = cmdline;

    /*
     * create main window
     */
    wind_handle = CreateWindow(
        TestCtlClass,           /* class */
        "WATCOM Data Control Test Program",     /* caption */
        WS_OVERLAPPEDWINDOW,    /* style */
        CW_USEDEFAULT,          /* init. x pos */
        CW_USEDEFAULT,          /* init. y pos */
        CW_USEDEFAULT,          /* init. x size */
        CW_USEDEFAULT,          /* init. y size */
        NULL,                   /* parent window */
        NULL,                   /* menu handle */
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
                                WORD wparam, LONG lparam )
{
    lparam = lparam;                    /* turn off warning */
    window_handle = window_handle;

    switch( msg ) {
    case WM_INITDIALOG:
        return( TRUE );

    case WM_COMMAND:
        if( wparam == IDOK ) {
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
                                WORD wparam, LONG lparam )
{
    switch( msg ) {
    case WM_INITDIALOG:
        /* get the controls in the dialog initialized with the correct
           settings based on the control data */
        ctl_dlg_init( GetWindowWord( window_handle, GWW_HINSTANCE ),
                                            window_handle, &Data, &CtlData );
        return( TRUE );         // let WINDOWS set the input focus

    case WM_COMMAND:
        switch( wparam ) {
        case IDOK:
            if( !ctl_dlg_done( GetWindowWord( window_handle, GWW_HINSTANCE ),
                                        window_handle, &Data, &CtlData ) ) {
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
                                     WORD wparam, LONG lparam )
{
    FARPROC     proc;
    HANDLE      inst_handle;

    switch( msg ) {
    case WM_COMMAND:
        switch( wparam ) {
        case MENU_ABOUT:
            inst_handle = GetWindowWord( window_handle, GWW_HINSTANCE );
            proc = MakeProcInstance( About, inst_handle );
            DialogBox( inst_handle,"AboutBox", window_handle, proc );
            FreeProcInstance( proc );
            break;

        case MENU_DIALOG:
            inst_handle = GetWindowWord( window_handle, GWW_HINSTANCE );
            proc = MakeProcInstance( DataCtlProc, inst_handle );
            DialogBox( inst_handle,"DATACTL", window_handle, proc );
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
