#include <windows.h>
#include <stdio.h>
#include "edit.h"
#include "win1632.h"

char            EditTitle[] = "Open Watcom Edit Example";
psupp           PrinterSupport;
static char     EditClass[32]="EditClass";

static BOOL FirstInstance( HINSTANCE );
static BOOL AnyInstance( HINSTANCE, int );
long _EXPORT FAR PASCAL WindowProc( HWND, unsigned, UINT, LONG );

/*
 * WinMain - initialization, message loop
 */
int PASCAL WinMain( HINSTANCE inst, HINSTANCE previnst, LPSTR cmdline,
                    int cmdshow )
{
    MSG         msg;

    previnst = previnst;        /* shut up warning */
    cmdline = cmdline;
#ifdef __WINDOWS_386__
    sprintf( EditClass,"EditClass%d", inst );
#else
    if( !previnst )
#endif
        if( !FirstInstance( inst ) ) return( FALSE );

    if( !AnyInstance( inst, cmdshow ) ) return( FALSE );

    while( GetMessage( &msg, 0, 0, 0 ) ) {

        TranslateMessage( &msg );
        DispatchMessage( &msg );

    }

    return( msg.wParam );

} /* WinMain */

/*
 * FirstInstance - register window class for the application,
 *                 and do any other application initialization
 */
static BOOL FirstInstance( HINSTANCE inst )
{
    WNDCLASS    wc;
    BOOL        rc;

    /*
     * set up and register window class
     */
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (LPVOID) WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( LPEDATA );
    wc.hInstance = inst;
    wc.hIcon = LoadIcon( inst, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( WHITE_BRUSH );
    wc.lpszMenuName = "EditMenu";
    wc.lpszClassName = EditClass;
    rc = RegisterClass( &wc );
    return( rc );

} /* FirstInstance */

/*
 * AnyInstance - do work required for every instance of the application:
 *                create the window, initialize data
 */
static BOOL AnyInstance( HINSTANCE inst, int cmdshow )
{
    RECT        rect;
    HWND        hwnd;
    HWND        editwnd;
    LPEDATA     edata_ptr;

    /*
     * create main window
     */
    hwnd = CreateWindow(
        EditClass,              /* class */
        EditTitle,              /* caption */
        WS_OVERLAPPEDWINDOW,    /* style */
        CW_USEDEFAULT,          /* init. x pos */
        CW_USEDEFAULT,          /* init. y pos */
        CW_USEDEFAULT,          /* init. x size */
        CW_USEDEFAULT,          /* init. y size */
        NULL,                   /* parent window */
        NULL,                   /* menu handle */
        inst,                   /* program handle */
        NULL                    /* create parms */
        );

    if( !hwnd ) return( FALSE );
    GetClientRect( hwnd, &rect );

    editwnd = CreateWindow(
        "EDIT",                         /* class */
        NULL,                           /* no caption */
        WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
        ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL, /* style */
        0,                              /* init. x pos */
        0,                              /* init. y pos */
        rect.right-rect.left,           /* init. x size (entire parent) */
        rect.bottom-rect.top,           /* init. y size  (entire parent) */
        hwnd,                           /* parent window */
        (HMENU)EDIT_ID,                 /* i.d. */
        inst,                           /* program handle */
        NULL                            /* create parms */
        );

    /*
     * if this failed, then kill original window
     */
    if( !editwnd ) {
        DestroyWindow( hwnd );
        return( FALSE );
    }

    /*
     * set up data associated with window (IMAGINE! not using
     * a pile of global variables!)
     */
    edata_ptr = MemAlloc( sizeof( extra_data ) );
    if( edata_ptr == NULL ) return( FALSE );
    edata_ptr->hwnd = hwnd;
    edata_ptr->editwnd = editwnd;
    edata_ptr->inst = inst;
    edata_ptr->filename = NULL;
    edata_ptr->needs_saving = FALSE;
    edata_ptr->font = NULL;
    SetWindowLong( hwnd, EXTRA_DATA_OFFSET, (DWORD) edata_ptr );

    /*
     * display window
     */
    ShowWindow( hwnd, cmdshow );
    UpdateWindow( hwnd );
    GetAllFonts( edata_ptr );

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
    HANDLE      hinst;
    LPEDATA     ed;
    DWORD       sel;
    int         state;
    HDC         hdc;

    ed = (LPEDATA) GetWindowLong( hwnd, EXTRA_DATA_OFFSET );

    /*
     * all messages are in alphabetical order, except WM_COMMAND, at the end
     */
    switch( msg ) {
    case WM_CLOSE:
        /*
         * see if it is okay to close down
         */
        if( CheckFileSave( ed ) ) {
            DestroyWindow( hwnd );
        }
        break;

    case WM_CREATE:
        /*
         * try to get printer support.  If it works, delete the DC,
         * since these are a system resource
         */
        hdc = PrinterDC();
        if( hdc ) {
            DeleteDC( hdc );
        }
        break;

    case WM_DESTROY:
        if( ed->font != NULL ) DeleteObject( ed->font );
        PostQuitMessage( 0 );
        break;

    case WM_DEVMODECHANGE:
    case WM_WININICHANGE:
        /*
         * handle user changing printer info
         */
        hdc = PrinterDC();
        if( hdc ) {
            DeleteDC( hdc );
        }
        break;

    case WM_FONTCHANGE:
        GetAllFonts( ed );
        break;

    case WM_INITMENU:
        /*
         * initial menu state set here
         */
        if( (HMENU)wparam == GetMenu( hwnd ) ) {

            state = MF_GRAYED;
            if( OpenClipboard( ed->hwnd ) ) {
                if( IsClipboardFormatAvailable( CF_TEXT ) ||
                    IsClipboardFormatAvailable( CF_OEMTEXT )) {
                    state = MF_ENABLED;
                }
                CloseClipboard();
            }
            EnableMenuItem( (HMENU)wparam, MENU_PASTE, state );

            state = MF_GRAYED;
            if( SendMessage( ed->editwnd, EM_CANUNDO, 0, 0L ) ) {
                state = MF_ENABLED;
            }
            EnableMenuItem( (HMENU)wparam, MENU_UNDO, state );
            sel = SendMessage( ed->editwnd, EM_GETSEL, 0, 0L );

            state = MF_GRAYED;
            if( HIWORD( sel ) != LOWORD( sel ) ) state = MF_ENABLED;
            EnableMenuItem( (HMENU)wparam, MENU_CLEAR, state );
            EnableMenuItem( (HMENU)wparam, MENU_COPY, state );
            EnableMenuItem( (HMENU)wparam, MENU_CUT, state );

            state = MF_GRAYED;
            if( PrinterSupport != PSUPP_NONE ) state = MF_ENABLED;
            EnableMenuItem( (HMENU)wparam, MENU_PRINT, state );
            state = MF_GRAYED;
            if( PrinterSupport == PSUPP_CANPRINTANDSET ) state = MF_ENABLED;
            EnableMenuItem( (HMENU)wparam, MENU_PRINT_SETUP, state );

        }
        return( 0L );

    case WM_QUERYENDSESSION:
        /*
         * check if it is okay to end the session
         */
        return( CheckFileSave( ed ) );

    case WM_SETFOCUS:
        /*
         * move the focus to our editor, rather than to the frame
         */
        SetFocus( ed->editwnd );
        break;

    case WM_SIZE:
        /*
         * resize edit window to match size of our client area
         */
        MoveWindow( ed->editwnd, 0, 0, LOWORD( lparam ),
                        HIWORD( lparam ), TRUE );
        break;

    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case EDIT_ID:
            switch( GET_WM_COMMAND_CMD( wparam, lparam ) ){
            case EN_CHANGE:
                ed->needs_saving = TRUE;
                break;
            case EN_ERRSPACE:
                MessageBox( hwnd, "Out of Space", EditTitle, MB_OK );
                break;
            }
            break;
        case MENU_ABOUT:
            hinst = GET_HINST( hwnd );
            proc = MakeProcInstance( (FARPROC)AboutDlgProc, hinst );
            DialogBox( hinst,"AboutBox", hwnd, (DLGPROC)proc );
            FreeProcInstance( proc );
            break;
        case MENU_CLEAR:
            SendMessage( ed->editwnd, EM_REPLACESEL, 0, (LONG) (LPSTR)"" );
            break;
        case MENU_COPY:
            SendMessage( ed->editwnd, WM_COPY, 0, 0L );
            break;
        case MENU_CUT:
            SendMessage( ed->editwnd, WM_CUT, 0, 0L );
            break;
        case MENU_EXIT:
            if( CheckFileSave( ed ) ) {
                DestroyWindow( hwnd );
            }
            break;
        case MENU_FONT_SELECT:
            FontSelect( ed );
            break;
        case MENU_NEW:
            FileEdit( ed, FALSE );
            break;
        case MENU_OPEN:
            FileEdit( ed, TRUE );
            break;
        case MENU_PASTE:
            SendMessage( ed->editwnd, WM_PASTE, 0, 0L );
            break;
        case MENU_PRINT:
            Print( ed );
            break;
        case MENU_PRINT_SETUP:
            GetPrinterSetup( hwnd );
            break;
        case MENU_SAVE:
            FileSave( ed, FALSE );
            break;
        case MENU_SAVE_AS:
            FileSave( ed, TRUE );
            break;
        case MENU_UNDO:
            SendMessage( ed->editwnd, EM_UNDO, 0, 0L );
            break;
        }
        break;

    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( 0L );

} /* WindowProc */
