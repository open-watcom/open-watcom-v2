#include <windows.h>
#include <stdio.h>
#include <malloc.h>
#include "generic.h"

HINSTANCE       MyInstance;
static char     GenericClass[32]="GenericClass";

static BOOL FirstInstance( HINSTANCE );
static BOOL AnyInstance( HINSTANCE, int, LPSTR );

long _EXPORT FAR PASCAL WindowProc( HWND, unsigned, UINT, LONG );

/*
 * WinMain - initialization, message loop
 */
int PASCAL WinMain( HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline,
                    int cmdshow )
{
#ifdef __NT__
    typedef VOID (WINAPI *PFNICC)( VOID );
    HINSTANCE   commctrl_inst;
    PFNICC      icc;
#endif
    MSG         msg;

#ifdef __NT__
    commctrl_inst = LoadLibrary( "comctl32.dll" );
    if( commctrl_inst != NULL ) {
        icc = (PFNICC)GetProcAddress( commctrl_inst, "InitCommonControls" );
        if( icc != NULL ) {
            icc();
        }
    }
#endif

    MyInstance = this_inst;
#ifdef __WINDOWS_386__
    sprintf( GenericClass, "GenericClass%d", this_inst );
    prev_inst = 0;
#endif
    if( !prev_inst ) {
        if( !FirstInstance( this_inst ) ) return( FALSE );
    }
    if( !AnyInstance( this_inst, cmdshow, cmdline ) ) return( FALSE );

    while( GetMessage( &msg, NULL, 0, 0 ) ) {

        TranslateMessage( &msg );
        DispatchMessage( &msg );

    }

#ifdef __NT__
    if( commctrl_inst != NULL ) {
        FreeLibrary( commctrl_inst );
    }
#endif

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
    wc.cbWndExtra = sizeof( DWORD );
    wc.hInstance = this_inst;
    wc.hIcon = LoadIcon( this_inst, "GenericIcon" );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( WHITE_BRUSH );
    wc.lpszMenuName = "GenericMenu";
    wc.lpszClassName = GenericClass;
    rc = RegisterClass( &wc );
    return( rc );

} /* FirstInstance */

/*
 * AnyInstance - do work required for every instance of the application:
 *                create the window, initialize data
 */
static BOOL AnyInstance( HINSTANCE this_inst, int cmdshow, LPSTR cmdline )
{
    HWND        hwnd;
    extra_data  *edata_ptr;

    /*
     * create main window
     */
    hwnd = CreateWindow(
        GenericClass,           /* class */
        "Open Watcom Generic",  /* caption */
        WS_OVERLAPPEDWINDOW,    /* style */
        CW_USEDEFAULT,          /* init. x pos */
        CW_USEDEFAULT,          /* init. y pos */
        320,                    /* init. x size */
        240,                    /* init. y size */
        NULL,                   /* parent window */
        NULL,                   /* menu handle */
        this_inst,              /* program handle */
        NULL                    /* create parms */
        );

    if( !hwnd ) return( FALSE );

    /*
     * set up data associated with this window
     */
    edata_ptr = malloc( sizeof( extra_data ) );
    if( edata_ptr == NULL ) return( FALSE );
    edata_ptr->cmdline = cmdline;
    SetWindowLong( hwnd, EXTRA_DATA_OFFSET, (DWORD) edata_ptr );

    /*
     * display window
     */
    ShowWindow( hwnd, cmdshow );
    UpdateWindow( hwnd );

    return( TRUE );

} /* AnyInstance */

/*
 * AboutDlgProc - processes messages for the about dialog.
 */
BOOL _EXPORT FAR PASCAL AboutDlgProc( HWND hwnd, unsigned msg,
                                UINT wparam, LONG lparam )
{
    lparam = lparam;                    /* turn off warning */

    switch( msg ) {
    case WM_INITDIALOG:
        return( TRUE );

    case WM_COMMAND:
        if( LOWORD( wparam ) == IDOK ) {
            EndDialog( hwnd, TRUE );
            return( TRUE );
        }
        break;
    }
    return( FALSE );

} /* AboutDlgProc */

/*
 * WindowProc - handle messages for the main application window
 */
LONG _EXPORT FAR PASCAL WindowProc( HWND hwnd, unsigned msg,
                                     UINT wparam, LONG lparam )
{
    FARPROC     proc;
    extra_data  *edata_ptr;
    char        buff[128];

    switch( msg ) {
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case MENU_ABOUT:
            proc = MakeProcInstance( (FARPROC)AboutDlgProc, MyInstance );
            DialogBox( MyInstance,"AboutBox", hwnd, (DLGPROC)proc );
            FreeProcInstance( proc );
            break;

        case MENU_CMDSTR:
            edata_ptr = (extra_data *) GetWindowLong( hwnd,
                                                EXTRA_DATA_OFFSET );
#ifdef __NT__
            sprintf( buff, "Command string was \"%s\"", edata_ptr->cmdline );
#else
            sprintf( buff, "Command string was \"%Fs\"", edata_ptr->cmdline );
#endif
            MessageBox( NULL, buff, "Program Information", MB_OK );
            break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( 0L );

} /* WindowProc */

