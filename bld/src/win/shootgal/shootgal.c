#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shootgal.h"
#include "win1632.h"

static char ShootGalClass[32]="ShootGalClass";
HWND        MessageWnd = NULL;
HWND        ScoreWnd = NULL;
BOOL        MessagesOn = FALSE;

int PASCAL WinMain( HANDLE, HANDLE, LPSTR, int );
static BOOL FirstInstance( HANDLE );
static BOOL AnyInstance( HANDLE, int );

BOOL _EXPORT FAR PASCAL About( HWND, unsigned, UINT, LONG );
BOOL _EXPORT FAR PASCAL SpeedDlgProc( HWND, unsigned, UINT, LONG );
long _EXPORT FAR PASCAL WindowProc( HWND, unsigned, UINT, LONG );
BOOL TurnMessageWindowOn( HWND );
BOOL TurnScoreWindowOn( HWND );
BOOL _EXPORT FAR PASCAL MessageWindowProc( HWND, unsigned, UINT, LONG );
BOOL _EXPORT FAR PASCAL ScoreProc( HWND, unsigned, UINT, LONG );
static void CheckHit( HDC, POINT );
static void DrawBitmap( HDC, HBITMAP, short, short );
POINT RandPoint( RECT, POINT );
void _EXPORT FAR PASCAL DrawBolt( short, short, LPSTR );
static void ShootBolt( HWND );
static void BoomSound();
static void BoltSound();

/*
 * WinMain - initialization, message loop
 */
int PASCAL WinMain( HANDLE this_inst, HANDLE prev_inst, LPSTR cmdline,
                    int cmdshow )
/*******************************/
{
    MSG         msg;

    cmdline = cmdline;          /* shut up compiler warning */
    prev_inst = prev_inst;

#ifdef __WINDOWS_386__
    sprintf( ShootGalClass,"ShootGalClass%d", this_inst );
#else
    if( !prev_inst )
#endif
        if( !FirstInstance( this_inst ) ) return( FALSE );

    if( !AnyInstance( this_inst, cmdshow ) ) return( FALSE );

    while( GetMessage( &msg, NULL, NULL, NULL ) ) {
        /*
         * check to see if any of the messages are for a modeless dialog box,
         */
        if( ( MessageWnd == NULL || !IsDialogMessage( MessageWnd, &msg ) ) &&
            ( ScoreWnd == NULL || !IsDialogMessage( ScoreWnd, &msg ) ) ) {

            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }

    }

    return( msg.wParam );

} /* WinMain */

/*
 * FirstInstance - register window class for the application,
 *                 and do any other application initialization
 */
static BOOL FirstInstance( HANDLE this_inst )
/*******************************************/
{
    WNDCLASS    wc;
    BOOL        rc;

    /*
     * set up and register window classes
     */
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (LPVOID) WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( DWORD );
    wc.hInstance = this_inst;
    wc.hIcon = LoadIcon( this_inst, "ShootGalIcon" );
    wc.hCursor = LoadCursor( this_inst, "guncursor" );
    wc.hbrBackground = GetStockObject( WHITE_BRUSH );
    wc.lpszMenuName = "ShootGalMenu";
    wc.lpszClassName = ShootGalClass;
    rc = RegisterClass( &wc );

    return( rc );

} /* FirstInstance */

/*
 * AnyInstance - do work required for every instance of the application:
 *                create the window, initialize data
 */
static BOOL AnyInstance( HANDLE this_inst, int cmdshow )
/******************************************************/
{
    HWND        window_handle;
    BITMAP      bitmapbuff;
    extra_data  *edata_ptr;

    /*
     * create main window
     */
    window_handle = CreateWindow(
        ShootGalClass,           /* class */
        "WATCOM Shooting Gallery - Sample Application",   /* caption */
        WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,    /* style */
        CW_USEDEFAULT,          /* init. x pos */
        CW_USEDEFAULT,          /* init. y pos */
        CW_USEDEFAULT,          /* init. x size */
        CW_USEDEFAULT,          /* init. y size */
        NULL,                   /* parent window */
        NULL,                   /* menu handle */
        this_inst,              /* program handle */
        NULL                    /* create parms */
        );

    if( !window_handle ) return( FALSE );

    /*
     * get a timer
     */
    while( !SetTimer( window_handle, TARGET_TIMER, STD_TARGET_SPEED,
                                                 ( FARPROC ) NULL ) ) {
        if( MessageBox( window_handle, "Too many timers in use!", NULL,
                        MB_ICONEXCLAMATION | MB_RETRYCANCEL ) == IDCANCEL ) {
            return( FALSE );
        }
    }

    /*
     * initialize data
     */
    edata_ptr = calloc( 1, sizeof( extra_data ) );
    if( edata_ptr == NULL ) return( FALSE );

    edata_ptr->target_bmp = LoadBitmap( this_inst, "target" );
    GetObject( edata_ptr->target_bmp, sizeof( BITMAP ), (LPSTR) &bitmapbuff);

    edata_ptr->sound_on = FALSE;
    edata_ptr->score_on = FALSE;
    edata_ptr->target.x = 0;
    edata_ptr->target.y = 0;
    edata_ptr->size.x = bitmapbuff.bmWidth;
    edata_ptr->size.y = bitmapbuff.bmHeight;
    edata_ptr->aim = edata_ptr->target;
    edata_ptr->bolt = edata_ptr->aim;
    edata_ptr->target_speed = STD_TARGET_SPEED;
    edata_ptr->bolt_speed = STD_BOLT_SPEED;
    GetClientRect( window_handle, &edata_ptr->client_rect );
    edata_ptr->message_window_proc =
             MakeProcInstance( MessageWindowProc, this_inst );
    edata_ptr->score_window_proc =
             MakeProcInstance( ScoreProc, this_inst );
    edata_ptr->bolt_icon = LoadIcon( this_inst, "Bolt" );

    /*
     * put a pointer to the above structure in the main window structure
     */
    SetWindowLong( window_handle, EXTRA_DATA_OFFSET, (DWORD) edata_ptr );

    /*
     * display window
     */
    ShowWindow( window_handle, cmdshow );
    UpdateWindow( window_handle );

    return( TRUE );

} /* AnyInstance */

/*
 * About -  processes messages for the about dialogue.
 */
BOOL _EXPORT FAR PASCAL About( HWND window_handle, unsigned msg,
                                UINT wparam, LONG lparam )
/********************************************************/
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

} /* About */

/*
 * SpeedDlgProc - processes messages for the speed dialogue box.
 *
 * Allows the user to select speeds for the target or lightning bolt,
 * depending on which selection was chosen.
 */
BOOL _EXPORT FAR PASCAL SpeedDlgProc( HWND dialog_wnd, unsigned msg,
                                UINT wparam, LONG lparam )
/********************************************************/
{
    extra_data              *edata_ptr;
    short                   speed;
    static short            old_speed = 0; /* in case the user hits CANCEL */
    static short            what_are_we_setting = 0;
    static short            fastest_speed = 0;
    static short            slowest_speed = 0;
    HWND                    scrollbar;
    WORD                    scroll_code;

    edata_ptr = (extra_data *) GetWindowLong(
                 GetWindow( dialog_wnd, GW_OWNER ), EXTRA_DATA_OFFSET );

    switch( msg ) {
    case WM_INITDIALOG:
        what_are_we_setting = (short)lparam;
        if( what_are_we_setting == SET_TARGET_SPEED ) {
            SetDlgItemText( dialog_wnd, SET_WHAT, (LPSTR)"Set Target Speed" );
            old_speed = edata_ptr->target_speed;
            fastest_speed = FASTEST_TARGET_SPEED;
            slowest_speed = SLOWEST_TARGET_SPEED;
        } else {
            SetDlgItemText( dialog_wnd, SET_WHAT, (LPSTR)"Set Lightning Bolt Speed" );
            SetDlgItemText( dialog_wnd, TEST, (LPSTR)"Click RIGHT mouse button to test." );
            /*
             * we want fastest on left, slowest on right,
             * so use negative numbers for bolt speed
             */
            old_speed = -edata_ptr->bolt_speed;
            fastest_speed = -FASTEST_BOLT_SPEED;
            slowest_speed = -SLOWEST_BOLT_SPEED;
        }
        scrollbar = GetDlgItem( dialog_wnd, SPEED_SCROLL );
        SetScrollRange( scrollbar, SB_CTL, fastest_speed, slowest_speed, FALSE );
        SetScrollPos( scrollbar, SB_CTL, old_speed, FALSE );
        return( TRUE );

    case WM_RBUTTONDOWN:
        if( what_are_we_setting == SET_BOLT_SPEED ) {
            /*
             * shoot bolt diagonally accross the screen as a test
             */
            edata_ptr->aim.x = edata_ptr->client_rect.left;
            edata_ptr->aim.y = edata_ptr->client_rect.bottom;
            ShootBolt( GetWindow( dialog_wnd, GW_OWNER ) );
        }
        break;
    case WM_HSCROLL:
        speed = ( what_are_we_setting == SET_TARGET_SPEED )
                ? edata_ptr->target_speed : -edata_ptr->bolt_speed;
        scrollbar = GetDlgItem( dialog_wnd, SPEED_SCROLL );
        scroll_code = GET_WM_HSCROLL_CODE( wparam, lparam );
        switch( scroll_code ) {
        case SB_PAGEDOWN:
            speed += 15;    /* note - no break - flow through to next case */
        case SB_LINEDOWN:
            speed = min( slowest_speed, ++speed );
            break;
        case SB_PAGEUP:
            speed -= 15;    /* note - no break - flow through to next case */
        case SB_LINEUP:
            speed = max( fastest_speed, --speed );
            break;
        case SB_TOP:
            speed = fastest_speed;
            break;
        case SB_BOTTOM:
            speed = slowest_speed;
            break;
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            speed = GET_WM_HSCROLL_POS( wparam, lparam );
            break;
        default:
            return( FALSE );
        }
        SetScrollPos( scrollbar, SB_CTL, speed , TRUE );
        if( what_are_we_setting == SET_TARGET_SPEED ) {
            edata_ptr->target_speed = speed;

            /* restart timer at new speed */
            KillTimer( GetWindow( dialog_wnd, GW_OWNER), TARGET_TIMER );
            SetTimer( GetWindow( dialog_wnd, GW_OWNER), TARGET_TIMER,
                      speed, ( FARPROC ) NULL );
        } else {
            /* change speed back to positive value */
            edata_ptr-> bolt_speed = -speed;
        }
        break;
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case IDOK:
            EndDialog( dialog_wnd, TRUE );
            return( TRUE );
        case IDCANCEL:
            /* change speed back to old value */
            if( what_are_we_setting == SET_TARGET_SPEED ) {
                edata_ptr->target_speed = old_speed;
            } else {
                edata_ptr->bolt_speed = -old_speed;
            }
            EndDialog( dialog_wnd, TRUE );
            return( TRUE );
        break;
        }
    }
    return( FALSE );

} /* SpeedDlgProc */

/*
 * WindowProc - handle messages for the main application window
 */
LONG _EXPORT FAR PASCAL WindowProc( HWND window_handle, unsigned msg,
                                     UINT wparam, LONG lparam )
/*************************************************************/
{
    FARPROC             proc;
    HANDLE              inst_handle;
    extra_data          *edata_ptr;
    HDC                 hdc;
    PAINTSTRUCT         ps;
    RECT                rect;
    HBRUSH              brush;
    WORD                cmd;

    /*
     * if the message window is ON, send all messages we want to display to the
     * message window, so that we can see what is happening
     * ( before we actually process the message )
     */
    if( MessagesOn ) {
        switch( msg ) {
        case WM_COMMAND:
        case WM_MOUSEMOVE:
        case WM_LBUTTONUP:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONDBLCLK:
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_CHAR:
        case WM_TIMER:
        case WM_HSCROLL:
        case WM_VSCROLL:
            SendMessage( MessageWnd, msg, wparam, lparam );
            break;
        }
    }

    /*
     * now process the message
     */
    switch( msg ) {
    case WM_CREATE:
        inst_handle = GET_HINST( window_handle );
        /*
         * make sure message window is turned OFF to start
         */
        MessagesOn = FALSE;
        CheckMenuItem ( GetMenu( window_handle ), MENU_MESSAGE_WINDOW_ON,
                        MF_BYCOMMAND | MF_UNCHECKED );
        break;
    case WM_LBUTTONDOWN:
        /*
         * zap the target with a lightning bolt!
         */
        edata_ptr = (extra_data *) GetWindowLong( window_handle,
                                            EXTRA_DATA_OFFSET );
        /*
         * set the aim point to where the mouse was just clicked
         * set the bolt start point to the top left corner of the window
         */

        MAKE_POINT( edata_ptr->aim, lparam );
        edata_ptr->bolt.x = edata_ptr->client_rect.right - BOLTWIDTH;
        edata_ptr->bolt.y = edata_ptr->client_rect.top;
        /*
         * shoot the bolt from the current bolt position to the aim point
         */
        ShootBolt( window_handle );
        break;
    case WM_SIZE:
        edata_ptr = (extra_data *) GetWindowLong( window_handle,
                                            EXTRA_DATA_OFFSET );
        /*
         * store the new size of the window
         */
        GetClientRect( window_handle, &edata_ptr->client_rect );
        SetScrollRange( window_handle, SB_HORZ, edata_ptr->client_rect.left,
                        edata_ptr->client_rect.right, TRUE );
        SetScrollRange( window_handle, SB_VERT, edata_ptr->client_rect.top,
                        edata_ptr->client_rect.bottom, TRUE );
        break;
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( cmd ) {
        case MENU_ABOUT:
            inst_handle = GET_HINST( window_handle );
            proc = MakeProcInstance( About, inst_handle );
            DialogBox( inst_handle,"AboutBox", window_handle, proc );
            FreeProcInstance( proc );
            break;
        case MENU_EXIT:
            SendMessage( window_handle, WM_CLOSE, 0, 0L );
            break;
        case MENU_SET_TARGET_SPEED:
            inst_handle = GET_HINST( window_handle );
            proc = MakeProcInstance( SpeedDlgProc, inst_handle );
            DialogBoxParam( inst_handle,"SpeedDlg", window_handle, proc,
                            (DWORD)SET_TARGET_SPEED );
            FreeProcInstance( proc );
            break;
        case MENU_SET_BOLT_SPEED:
            inst_handle = GET_HINST( window_handle );
            proc = MakeProcInstance( SpeedDlgProc, inst_handle );
            DialogBoxParam( inst_handle,"SpeedDlg", window_handle, proc,
                            (DWORD)SET_BOLT_SPEED );
            FreeProcInstance( proc );
            break;
        case MENU_SCORE_WINDOW_ON:
            /*
             * toggle the score window on or off
             */
            edata_ptr = (extra_data *) GetWindowLong( window_handle,
                                                EXTRA_DATA_OFFSET );
            if( !edata_ptr->score_on ) {
                TurnScoreWindowOn( window_handle );
                CheckMenuItem( GetMenu( window_handle ),
                                cmd, MF_BYCOMMAND | MF_CHECKED );
                edata_ptr->score_on = TRUE;
            } else {
                SendMessage( ScoreWnd, WM_CLOSE, 0, 0L );
                CheckMenuItem ( GetMenu( window_handle ), cmd,
                    MF_BYCOMMAND | MF_UNCHECKED );
                edata_ptr->score_on = FALSE;
            }
            break;
        case MENU_SOUND_ON:
            /*
             * toggle the sound on or off
             */
            edata_ptr = (extra_data *) GetWindowLong( window_handle,
                                                EXTRA_DATA_OFFSET );
            if( !edata_ptr->sound_on ) {
                CheckMenuItem ( GetMenu( window_handle ),
                                cmd, MF_BYCOMMAND | MF_CHECKED );
                edata_ptr->sound_on = TRUE;
            } else {
                CheckMenuItem ( GetMenu( window_handle ), cmd,
                    MF_BYCOMMAND | MF_UNCHECKED );
                edata_ptr->sound_on = FALSE;
            }
            break;
        case MENU_MESSAGE_WINDOW_ON:
            /*
             * toggle the message window on or off
             */
            if( !MessagesOn ) {
                TurnMessageWindowOn( window_handle );
                CheckMenuItem( GetMenu( window_handle ),
                                cmd, MF_BYCOMMAND | MF_CHECKED );
                MessagesOn = TRUE;
            } else {
                SendMessage( MessageWnd, WM_CLOSE, 0, 0L );
                CheckMenuItem( GetMenu( window_handle ), cmd,
                    MF_BYCOMMAND | MF_UNCHECKED );
                MessagesOn = FALSE;
            }
            break;
        }
        break;
    case WM_PAINT:
        edata_ptr = (extra_data *) GetWindowLong( window_handle,
                                            EXTRA_DATA_OFFSET );
        hdc = BeginPaint (window_handle, &ps);
        /*
         * paint the invalid area with the background colour
         */
        brush = CreateSolidBrush( BACKGROUND );
        FillRect( hdc, &ps.rcPaint, brush );
        DeleteObject( brush );

        rect.left   = edata_ptr->target.x;
        rect.top    = edata_ptr->target.y;
        rect.right  = rect.left + edata_ptr->size.x;
        rect.bottom = rect.top + edata_ptr->size.y;

        /*
         * if part of the target bitmap is invalid, redraw it
         */
        if( IntersectRect( &rect, &rect, &ps.rcPaint ) ) {
            DrawBitmap( hdc, edata_ptr->target_bmp,
                        edata_ptr->target.x, edata_ptr->target.y );
        }
        EndPaint(window_handle, &ps);
        break;
    case WM_HSCROLL:
    case WM_VSCROLL:
        /*
         * use the scrollbars to move the target around
         */
        {
            short       position; /* the x or y position of the scrollbar */
            short       max;
            short       min;
            WORD        code;
            WORD        pos;

            edata_ptr = (extra_data *) GetWindowLong( window_handle,
                                                EXTRA_DATA_OFFSET );
            if( msg == WM_HSCROLL ) {
                position = edata_ptr->target.x;
                code = GET_WM_HSCROLL_CODE( wparam, lparam );
                pos = GET_WM_HSCROLL_POS( wparam, lparam );
            } else {
                position = edata_ptr->target.y;
                code = GET_WM_VSCROLL_CODE( wparam, lparam );
                pos = GET_WM_VSCROLL_POS( wparam, lparam );
            }

            switch( code ) {
            case SB_PAGEDOWN:
                position += 15;
                break;
            case SB_LINEDOWN:
                position++;
                break;
            case SB_PAGEUP:
                position -= 15;
                break;
            case SB_LINEUP:
                position--;
                break;
            case SB_THUMBPOSITION:
            case SB_THUMBTRACK:
                position = pos;
                break;
            default:
                return( 0L );
            }
            if( msg == WM_HSCROLL ) {
                GetScrollRange( window_handle, SB_HORZ, (LPINT)&min, (LPINT)&max );
                edata_ptr->target.x = max( min( position, max ), min );
                SetScrollPos( window_handle, SB_HORZ, edata_ptr->target.x , TRUE );
            } else {
                GetScrollRange( window_handle, SB_VERT, (LPINT)&min, (LPINT)&max );
                edata_ptr->target.y = max( min( position, max ), min );
                SetScrollPos( window_handle, SB_VERT, edata_ptr->target.y, TRUE );
            }
            InvalidateRect( window_handle, &edata_ptr->client_rect, FALSE );
        }
        break;
    case WM_MOVE_TARGET:

        /*
         * move the target to a random location on the screen
         */

        edata_ptr = (extra_data *) GetWindowLong( window_handle,
                                            EXTRA_DATA_OFFSET );
        rect.left   = edata_ptr->target.x;
        rect.top    = edata_ptr->target.y;
        rect.right  = rect.left + edata_ptr->size.x;
        rect.bottom = rect.top + edata_ptr->size.y;
        InvalidateRect( window_handle, &rect, TRUE );

        edata_ptr->target = RandPoint( edata_ptr->client_rect, edata_ptr->size );

        rect.left   = edata_ptr->target.x;
        rect.top    = edata_ptr->target.y;
        rect.right  = rect.left + edata_ptr->size.x;
        rect.bottom = rect.top + edata_ptr->size.y;
        InvalidateRect( window_handle, &rect, TRUE );

        /* set the scrollbars to indicate the new position */
        SetScrollPos( window_handle, SB_HORZ, edata_ptr->target.x , TRUE );
        SetScrollPos( window_handle, SB_VERT, edata_ptr->target.y, TRUE );

        break;
    case WM_TIMER:
        SendMessage( window_handle, WM_MOVE_TARGET, 0, 0L );
        break;
        break;
    case WM_DESTROY:
        edata_ptr = (extra_data *) GetWindowLong( window_handle,
                                                  EXTRA_DATA_OFFSET );
        KillTimer( window_handle, TARGET_TIMER );       /* Stops the timer */
        #ifndef __NT__
        FreeProcInstance( edata_ptr->message_window_proc );
        FreeProcInstance( edata_ptr->score_window_proc );
        #endif
        PostQuitMessage( 0 );
        break;

    default:
        return( DefWindowProc( window_handle, msg, wparam, lparam ) );
    }
    return( 0L );

} /* WindowProc */

/*
 * TurnMessageWindowOn - create the Message window, which will display
 * messages received by WindowProc
 * this window is a modeless dialog box
 */
BOOL TurnMessageWindowOn( HWND window_handle )
/********************************************/
{

    HANDLE      inst_handle;
    extra_data  *edata_ptr;

    edata_ptr = (extra_data *) GetWindowLong( window_handle, EXTRA_DATA_OFFSET );

    inst_handle = GET_HINST( window_handle );

    MessageWnd = CreateDialog( inst_handle,"MessageWindow", window_handle,
                     edata_ptr->message_window_proc );
    return( MessageWnd != NULL );

} /* TurnMessageWindowOn */

/*
 * TurnMessageWindowOn - create the score window, which will display
 * the number of shots the user has taken and what locations they have hit
 * this window is a modeless dialog box
 */
BOOL TurnScoreWindowOn( HWND window_handle )
/******************************************/
{

    HANDLE      inst_handle;
    extra_data  *edata_ptr;

    edata_ptr = (extra_data *) GetWindowLong( window_handle, EXTRA_DATA_OFFSET );

    inst_handle = GET_HINST( window_handle );

    ScoreWnd = CreateDialog( inst_handle,"ScoreWindow", window_handle,
                     edata_ptr->score_window_proc );
    return( ScoreWnd != NULL );

} /* TurnScoreWindowOn */

/*
 * processes messages for the score dialog box
 */
BOOL _EXPORT FAR PASCAL ScoreProc( HWND window_handle, unsigned msg,
                                            UINT wparam, LONG lparam )
/********************************************************************/
{

    extra_data *edata_ptr;

    wparam = wparam;
    lparam = lparam;

    edata_ptr = (extra_data *) GetWindowLong(
                GetWindow( window_handle, GW_OWNER ), EXTRA_DATA_OFFSET );

    switch (msg) {
    case WM_INITDIALOG:
        SetDlgItemInt( window_handle, SHOTS, 0, FALSE );
        SetDlgItemInt( window_handle, YELLOW, 0, FALSE );
        SetDlgItemInt( window_handle, RED, 0, FALSE );
        SetDlgItemInt( window_handle, BLUE, 0, FALSE );
        SetDlgItemInt( window_handle, BLACK, 0, FALSE );
        SetDlgItemInt( window_handle, WHITE, 0, FALSE );
        SetDlgItemInt( window_handle, MISSED, 0, FALSE );
        return( TRUE );
    case WM_CLOSE:
        CheckMenuItem ( GetMenu( GetWindow( window_handle, GW_OWNER ) ),
            MENU_SCORE_WINDOW_ON, MF_BYCOMMAND | MF_UNCHECKED );
        DestroyWindow( window_handle );
        /* not EndDialog, since this is a MODELESS dialog box */
        edata_ptr->score_on = FALSE;
        ScoreWnd = NULL;
        break;
    }
    return( FALSE );
} /* ScoreProc */

BOOL _EXPORT FAR PASCAL MessageWindowProc( HWND window_handle, unsigned msg,
                                            UINT wparam, LONG lparam )
/********************************************************************/
{
    char textbuffer[48];
    /* buffer to hold strings before they are sent to dialog box controls */

    static unsigned long timercount = 0;
    /* counter - incremented each time a timer message is recieved */


    /*
     * other than WM_INITDIALOG & WM_CLOSE, all messages should be displayed
     * in the dialog box, in the appropriate location
     * so use wsprintf to format the message & then send the string to a
     * static text control
     */
    switch (msg) {
    case WM_INITDIALOG:
        return( TRUE );
    case WM_CLOSE:
        CheckMenuItem ( GetMenu( GetWindow( window_handle, GW_OWNER ) ),
            MENU_MESSAGE_WINDOW_ON, MF_BYCOMMAND | MF_UNCHECKED );
        DestroyWindow( window_handle );
        /* not EndDialog, since this is a MODELESS dialog box */
        MessagesOn = FALSE;
        MessageWnd = NULL;
        break;
    case WM_MOUSEMOVE:
        wsprintf( textbuffer, "WM_MOUSEMOVE: %x, %x, %x", wparam,
                (int)(short)LOWORD(lparam ), (int)(short)HIWORD(lparam ) );
        SetDlgItemText( window_handle, MOUSE_MOVE_BOX, (LPSTR)textbuffer );
        return( TRUE );

    case WM_LBUTTONDOWN:
        wsprintf( textbuffer, "WM_LBUTTONDOWN: %x, %x, %x", wparam,
                  (int)(short)LOWORD(lparam ), (int)(short)HIWORD(lparam ) );
        SetDlgItemText( window_handle, L_BUTTON_BOX, (LPSTR)textbuffer );
        return( TRUE );

    case WM_LBUTTONUP:
        wsprintf( textbuffer, "WM_LBUTTONUP: %x, %x, %x", wparam,
                (int)(short)LOWORD(lparam ), (int)(short)HIWORD(lparam ) );
        SetDlgItemText( window_handle, L_BUTTON_BOX, (LPSTR)textbuffer );
        return( TRUE );

    case WM_LBUTTONDBLCLK:
        wsprintf(textbuffer, "WM_LBUTTONDBLCLK: %x, %x, %x", wparam,
                (int)(short)LOWORD(lparam ), (int)(short)HIWORD(lparam ) );
        SetDlgItemText( window_handle, L_BUTTON_BOX, (LPSTR)textbuffer );
        return( TRUE );
    case WM_RBUTTONDOWN:
        wsprintf(textbuffer, "WM_RBUTTONDOWN: %x, %x, %x", wparam,
                (int)(short)LOWORD(lparam ), (int)(short)HIWORD(lparam ) );
        SetDlgItemText( window_handle, R_BUTTON_BOX, (LPSTR)textbuffer );
        return( TRUE );

    case WM_RBUTTONUP:
        wsprintf(textbuffer, "WM_RBUTTONUP: %x, %x, %x", wparam,
                (int)(short)LOWORD(lparam ), (int)(short)HIWORD(lparam ) );
        SetDlgItemText( window_handle, R_BUTTON_BOX, (LPSTR)textbuffer );
        return( TRUE );

    case WM_RBUTTONDBLCLK:
        wsprintf(textbuffer, "WM_RBUTTONDBLCLK: %x, %x, %x", wparam,
                (int)(short)LOWORD(lparam ), (int)(short)HIWORD(lparam ) );
        SetDlgItemText( window_handle, R_BUTTON_BOX, (LPSTR)textbuffer );
        return( TRUE );

    case WM_KEYDOWN:
        wsprintf(textbuffer, "WM_KEYDOWN: %x, %x, %x", wparam,
                (int)(short)LOWORD(lparam ), (int)(short)HIWORD(lparam ) );
        SetDlgItemText( window_handle, KEY_UP_OR_DOWN_BOX, (LPSTR)textbuffer );
        return( TRUE );

    case WM_KEYUP:
        wsprintf(textbuffer, "WM_KEYUP: %x, %x, %x", wparam,
                (int)(short)LOWORD(lparam ), (int)(short)HIWORD(lparam ) );
        SetDlgItemText( window_handle, KEY_UP_OR_DOWN_BOX, (LPSTR)textbuffer );
        return( TRUE );

    case WM_CHAR:
        wsprintf(textbuffer, "WM_CHAR: %c, %x, %x", wparam , LOWORD(lparam ), HIWORD(lparam ));
        SetDlgItemText( window_handle, CHARACTER_BOX, (LPSTR)textbuffer );
        return( TRUE );

    case WM_TIMER:
        {
            extra_data *edata_ptr;

            edata_ptr = (extra_data *) GetWindowLong(
                GetWindow( window_handle, GW_OWNER ), EXTRA_DATA_OFFSET );

            timercount += edata_ptr->target_speed;
            wsprintf(textbuffer, "WM_TIMER: %d seconds", timercount / 1000 );
            SetDlgItemText( window_handle, TIMER_BOX, (LPSTR)textbuffer );
            return( TRUE );
        }
    case WM_HSCROLL:
    case WM_VSCROLL:
        {
            char        *buff1;
            char        *buff2;
            WORD        code;

            if( msg == WM_HSCROLL ) {
                buff1 = "WM_HSCROLL";
                code = GET_WM_HSCROLL_CODE( wparam, lparam );
            } else {
                buff1 = "WM_VSCROLL";
                code = GET_WM_VSCROLL_CODE( wparam, lparam );
            }
            switch( code ){
            case SB_LINEUP:
                buff2 = "SB_LINEUP";
                break;
            case SB_LINEDOWN:
                buff2 = "SB_LINEDOWN";
                break;
            case SB_PAGEUP:
                buff2 = "SB_PAGEUP";
                break;
            case SB_PAGEDOWN:
                buff2 = "SB_PAGEDOWN";
                break;
            case SB_THUMBPOSITION:
                buff2 = "SB_THUMBPOSITION";
                break;
            case SB_THUMBTRACK:
                buff2 = "SB_THUMBTRACK";
                break;
            case SB_ENDSCROLL:
                buff2 = "SB_ENDSCROLL";
                break;
            default :
                buff2 = "unknown";
                break;
            }
            wsprintf(textbuffer, "%s: %s, %x, %x",
                (LPSTR)buff1, (LPSTR)buff2, (int)(short)LOWORD(lparam ),
                (int)(short)HIWORD(lparam ));
            SetDlgItemText( window_handle, SCROLL_BOX, (LPSTR)textbuffer );
            return( TRUE );
        }
    }
    return ( FALSE );
} /* MessageWindowProc */

/*
 * calculate where on the target the bolt hit
 * this is based on the colours of the target rings
 */
static void CheckHit( HDC hdc, POINT hit_point )
/**********************************************/
{
    DWORD               colour;
    unsigned short      points;
    unsigned short      dialog_item;
    BOOL                translated;

    colour = GetPixel( hdc, hit_point.x, hit_point.y );

    switch( colour ) {
    case RING1:
        dialog_item = YELLOW;
        break;
    case RING2:
        dialog_item = RED;
        break;
    case RING3:
        dialog_item = BLUE;
        break;
    case RING4:
        dialog_item = BLACK;
        break;
    case RING5:
        dialog_item = WHITE;
        break;
    case BACKGROUND:
        dialog_item = MISSED;
        break;
    }

    /*
     * increment # of hits on location
     */
    points = GetDlgItemInt( ScoreWnd, dialog_item, &translated, FALSE );
    points++;
    SetDlgItemInt( ScoreWnd, dialog_item, points, FALSE );

    /*
     * increment # of shots
     */
    points = GetDlgItemInt( ScoreWnd, SHOTS, &translated, FALSE );
    points++;
    SetDlgItemInt( ScoreWnd, SHOTS, points, FALSE );

    return;

} /* CheckHit */

/*
 * display a bitmap on the given DC, at device coordinates x,y
 * make the bitmap the same size as the source bitmap
 */
static void DrawBitmap( HDC hdc, HBITMAP bitmap, short x, short y )
/*****************************************************************/
{
    BITMAP      bitmapbuff;
    HDC         memorydc;
    POINT       origin;
    POINT       size;

    memorydc = CreateCompatibleDC( hdc );
    SelectObject( memorydc, bitmap );
    SetMapMode( memorydc, GetMapMode( hdc ) );
    GetObject( bitmap, sizeof( BITMAP ), (LPSTR) &bitmapbuff );

    origin.x = x;
    origin.y = y;
    size.x = bitmapbuff.bmWidth;
    size.y = bitmapbuff.bmHeight;

    DPtoLP( hdc, &origin, 1 );
    DPtoLP( memorydc, &size, 1 );

    BitBlt( hdc, origin.x, origin.y, size.x, size.y, memorydc, 0, 0, SRCCOPY);
    DeleteDC( memorydc );
} /* DrawBitmap */

/*
 * pick a random point so that a bitmap with width & height specified by
 * the second parameter will fit inside the given rectangle
 */
POINT RandPoint( RECT rect, POINT size )
/**************************************/
{
    POINT random_point;
    short width;    /* width of the area to pick from */
    short height;   /* height of the area to pick from */

    width = rect.right - rect.left - size.x;
    height = rect.bottom - rect.top - size.y;

    random_point.x = rand() % width + rect.left;
    random_point.y = rand() % height + rect.top;

    return( random_point );
} /* RandPoint */

/*
 * Draws an icon at the coordinates x,y, after copying a saved bitmap back
 * onto the screen at the coordinates where the last icon was drawn and
 * saving the area under the icon to a DC in memory
 */
void DoDrawBolt( int x, int y, icon_mover * mover)
/************************************************/
{

    /*
     * to make this look fast, even on slow machines, only redraw the icon
     * when the x coordinate is a multiple of the bolt speed
     * thus if speed = 5, the icon will be redrawn every 5th pixel it moves
     */
    if( x % mover->speed != 0 ) return;

    /* if it is not the first iteration */
    if( mover->last.x != -1 || mover->last.y != -1 ) {
        /* redraw the area that the last icon covered */
        BitBlt( mover->screen_dc, mover->last.x, mover->last.y, mover->size.x,
                mover->size.y, mover->storage_dc, 0, 0, SRCCOPY);
    }

    /* save the area that will be under the icon to a DC in memory */
    BitBlt( mover->storage_dc, 0, 0, mover->size.x, mover->size.y,
            mover->screen_dc, x, y, SRCCOPY);

    DrawIcon( mover->screen_dc, x, y, mover->icon );

    /* save the coordinates for next time */
    mover->last.x = x;
    mover->last.y = y;

} /* DoDrawBolt */

/*
 * called by windows from LineDDA funcion
 */
void _EXPORT FAR PASCAL DrawBolt( int x, int y, LPSTR dataptr )
/*************************************************************/
{

    DoDrawBolt( x, y, (icon_mover *) dataptr );

} /* DrawBolt */

/*
 * "shoot" a bolt from the current bolt location to the "aim" location
 */
static void ShootBolt( HWND window_handle )
/*****************************************/
{
    extra_data  *edata_ptr;
    HDC         hdc;
    HBITMAP     screensavebmp;
    MSG         msg;
    FARPROC     proc;
    HANDLE      inst_handle;
    icon_mover  mover;

    edata_ptr = (extra_data *) GetWindowLong( window_handle,
                                        EXTRA_DATA_OFFSET );

    hdc = GetDC( window_handle );
    inst_handle = GET_HINST( window_handle );

    /* set up an "icon_mover" struct to give all needed data to DrawBolt */
    mover.icon = edata_ptr->bolt_icon;
    mover.size.x = BOLTWIDTH;
    mover.size.y = BOLTHEIGHT;
    mover.last.x = -1;
    mover.last.y = -1;
    mover.screen_dc = hdc;
    mover.storage_dc = CreateCompatibleDC( hdc );
    mover.speed = edata_ptr->bolt_speed;

    screensavebmp = CreateCompatibleBitmap( hdc, mover.size.x, mover.size.y );
    SelectObject( mover.storage_dc, screensavebmp );

    if( edata_ptr->sound_on ) {
        BoltSound();
    }
    /*
     * for each point on the line between the points BOLT and AIM,
     * call DrawBolt
     * use aim - boltheight so that the bottom left corner of the bolt icon
     * stops at the point AIM
     */
    proc = MakeProcInstance( DrawBolt, inst_handle );
    LineDDA( edata_ptr->bolt.x, edata_ptr->bolt.y, edata_ptr->aim.x,
             edata_ptr->aim.y - BOLTHEIGHT, proc, (LPARAM)(LPVOID)&mover );
    FreeProcInstance( proc );

    /*
     * remove all WM_LBUTTONDOWN messages from the application queue which
     * occured while the bolt was "in the air"
     * this prevents the shots from building up - only 1 can happen at a time
     */
    while( PeekMessage( &msg, window_handle, WM_LBUTTONDOWN, WM_LBUTTONDOWN,
                        PM_REMOVE ) );

    /* make sure the bolt is drawn at the final location regardless of speed */
    mover.speed = 1;
    DoDrawBolt( edata_ptr->aim.x, edata_ptr->aim.y - BOLTHEIGHT, &mover );
    if( edata_ptr->sound_on ) {
        BoomSound();
    }
    /* redraw the background behind the icon */
    BitBlt( mover.screen_dc, mover.last.x, mover.last.y, mover.size.x,
            mover.size.y, mover.storage_dc, 0, 0, SRCCOPY);

    CheckHit( hdc, edata_ptr->aim );

    DeleteDC( mover.storage_dc );
    DeleteObject( screensavebmp );
    ReleaseDC(window_handle, hdc);

} /* ShootBolt */

/*
 * make a sound like thunder
 * this function is called after each time BoltSound is called
 * Sound is not implemented for the NT version
 */
static void BoomSound()
/*********************/
{
#ifndef __NT__
    short i;
    short low;
    short high;

    StopSound();
    SetVoiceQueueSize( 1, 600 );
    SetVoiceAccent( 1, 120, 50, S_STACCATO, 0 );

    for( i=0; i < 1000; i++ ) {
        /*
         * create a random low-pitched sound
         */
        high = rand() % 200 + 1;
        low = rand() % high + 1;
        SetVoiceSound( 1, MAKELONG( low, high ), 1 );
    }
    StartSound();
    WaitSoundState( S_QUEUEEMPTY ); /* wait for the sound to finish */
    StopSound();
    CloseSound();
#endif
} /* BoomSound */

/*
 * create a tone that goes from high-pitched to low-pitched
 * Sound is not implemented for the NT version
 */
static void BoltSound()
/*********************/
{
#ifndef __NT__
    short i;

    OpenSound();
    SetVoiceAccent( 1, 120, 50, S_LEGATO, 0 );
    for( i=84; i > 0; i-- ) {
        SetVoiceNote( 1, i, 128, 1 );
        /* place a 128th note of value i into voice queue 1 */
    }
    StartSound();
#endif
} /* BoltSound */
