#include <windows.h>
#include <stdio.h>
#include "watzee.h"


/*
 * AboutDlgProc -- About Box dialog
 */

extern BOOL _EXPORT FAR PASCAL AboutDialogProc( HWND hdlg, WORD message,
                                                WORD wparam, LONG lparam )
/************************************************************************/
{
    lparam = lparam;            /* shut compiler up */
    switch( message ) {
    case WM_INITDIALOG :
        return( TRUE );
    case WM_CLOSE :
        EndDialog( hdlg, 0 );
        return( TRUE );
    case WM_COMMAND :
        switch( wparam ) {
        case IDD_OK :
            EndDialog( hdlg, 0 );
            return( TRUE );
        }
        break;
    }
    return( FALSE );
}



/*
 * GetNumPlayersDlgProc -- dialog box to get the number of players
 */

extern BOOL _EXPORT FAR PASCAL GetNumPlayersDialogProc( HWND hdlg,
                                                        WORD message,
                                                        WORD wparam,
                                                        LONG lparam )
/********************************************************************/
{

    lparam = lparam;    /* shut compiler up*/

    switch( message ) {
    case WM_INITDIALOG :
        return( TRUE );
    case WM_COMMAND :
        if( wparam >= '1' && wparam <= '5' ) {
            NumberOfPlayers = (short) (wparam - '0');
            EndDialog( hdlg, 0 );
            return( TRUE );
        }
    }
    return( FALSE );
}



/*
 * GetInitialsDialogProc -- dialog box to get the players' initials
 */

extern BOOL _EXPORT FAR PASCAL GetInitialsDialogProc( HWND hdlg,
                                                       WORD message,
                                                       WORD wparam,
                                                       LONG lparam )
/******************************************************************/
{
    static short        player;
    static short        initial;
    static RECT         rect;
    char                numberstring[4];
    PAINTSTRUCT         ps;
    HDC                 hdc;

    lparam = lparam;    /* shut compiler up*/
    switch( message ) {
    case WM_INITDIALOG :
        EnableWindow( GetDlgItem( hdlg, IDD_OK ), FALSE );
        player = 0;
        initial = 0;
        rect.top = 8 * SysHeight / 3 + 2;
        rect.bottom = rect.top + SysHeight + SysHeight / 2 - 4;
        rect.left = CharWidth * 10;
        rect.right = rect.left + CharWidth * 5;
        SetFocus( hdlg );
        return( TRUE );
    case WM_COMMAND :
        hdc = GetDC( hdlg );
        SelectObject( hdc, GetStockObject( SYSTEM_FONT ) );
        if( wparam >= 'A' && wparam <= 'Z' ) {
            EnableWindow( GetDlgItem( hdlg, IDD_OK ), TRUE );
            EnableWindow( GetDlgItem( hdlg, IDD_PC ), FALSE );
            PlayerName[player][initial] = (char) wparam;
            if( initial < 2 ) {
                initial++;
                PlayerName[player][initial] = NULL;
            }
            Rectangle( hdc, rect.left, rect.top-2, rect.right, rect.bottom );
            DrawText( hdc,PlayerName[player],-1,&rect,DT_CENTER|DT_VCENTER );
        } else {
            switch( wparam ) {
            case IDD_CLEAR :
                EnableWindow( GetDlgItem( hdlg, IDD_OK ), FALSE );
                EnableWindow( GetDlgItem( hdlg, IDD_PC), TRUE );
                sprintf( PlayerName[player], "   " );
                initial = 0;
                Rectangle( hdc,rect.left,rect.top-2,rect.right,rect.bottom );
                break;
            case IDD_PC :
                EnableWindow( GetDlgItem( hdlg, IDD_OK ), TRUE );
                sprintf( PlayerName[player], "PC" );
                Player[player][IS_COMPUTER] = TRUE;
                DrawText( hdc, "PC", 2, &rect, DT_CENTER|DT_VCENTER );
                SendMessage( hdlg, WM_COMMAND, IDD_OK, 0 );
                break;
            case IDD_OK :
                if( player+1 == NumberOfPlayers ) {
                    EndDialog( hdlg, 0 );
                } else {
                    EnableWindow( GetDlgItem( hdlg, IDD_OK ), FALSE );
                    EnableWindow( GetDlgItem( hdlg, IDD_PC ), TRUE );
                    if( LOWORD( GetTextExtent( hdc, PlayerName[player],
                                               initial ) ) > 24 ) {
                        PlayerName[player][1] = PlayerName[player][2];
                        PlayerName[player][2] = NULL;
                    }
                    player++;
                    initial = 0;
                    sprintf( numberstring, "%d :", player+1 );
                    TextOut(hdc, 17*CharWidth/2, 11*SysHeight/4,
                            numberstring, 3);
                    Rectangle(hdc,rect.left,rect.top-2,rect.right,rect.bottom);
                }
                SetFocus( hdlg );
                break;
            default :
                return( FALSE );
            }
        }
        ReleaseDC( hdlg, hdc );
        return( TRUE );
    case WM_PAINT :
        hdc = BeginPaint( hdlg, &ps );
        SelectObject( hdc, GetStockObject( SYSTEM_FONT ) );
        sprintf( numberstring, "%d :", player+1 );
        TextOut( hdc, 17*CharWidth/2, 11*SysHeight/4, numberstring, 3 );
        Rectangle( hdc, rect.left, rect.top-2, rect.right, rect.bottom );
        DrawText( hdc, PlayerName[player], -1, &rect, DT_CENTER|DT_VCENTER );
        EndPaint( hdlg, &ps );
        return( TRUE );
    }
    return( FALSE );
}



/*
 * HelpDialogProc -- dialog to display the scoring rules
 */

extern BOOL _EXPORT FAR PASCAL HelpDialogProc( HWND hdlg, WORD message,
                                               WORD wparam, LONG lparam )
/***********************************************************************/
{
    lparam = lparam;    /* shut compiler up*/
    switch( message ) {
    case WM_INITDIALOG :
        return( TRUE );
    case WM_CLOSE :
        EndDialog( hdlg, 0 );
        return( TRUE );
    case WM_COMMAND :
        switch( wparam ) {
        case IDD_OK :
            EndDialog( hdlg, 0 );
            return( TRUE );
        }
        break;
    }
    return( FALSE );
}



/*
 * OptionsDialogProc -- options dialog box
 */

extern BOOL _EXPORT FAR PASCAL OptionsDialogProc( HWND hdlg, WORD message,
                                                 WORD wparam, LONG lparam )
/*************************************************************************/
{
    static BOOL old_diecheck;
    static BOOL old_resetdiechecks;

    lparam = lparam;    /* shut compiler up*/
    switch( message ) {
    case WM_INITDIALOG :
        old_diecheck = DieCheckMeansRoll;
        old_resetdiechecks = ResetDieChecks;
        if( DieCheckMeansRoll ) {
            CheckRadioButton( hdlg, IDD_ROLL, IDD_KEEP, IDD_ROLL );
        } else {
            CheckRadioButton( hdlg, IDD_ROLL, IDD_KEEP, IDD_KEEP );
        }
        CheckDlgButton( hdlg, IDD_RESET, (WORD) ResetDieChecks );
        return( TRUE );
    case WM_CLOSE :
        SendMessage( hdlg, WM_COMMAND, IDD_CANCEL, 0 );
        return( 0 );
    case WM_COMMAND :
        switch( wparam ) {
        case IDD_CANCEL :
            DieCheckMeansRoll = old_diecheck;
            ResetDieChecks = old_resetdiechecks;        // fall through
        case IDD_OK :
            EndDialog( hdlg, 0 );
            return( TRUE );
        case IDD_ROLL :
            DieCheckMeansRoll = TRUE;
            CheckRadioButton( hdlg, IDD_ROLL, IDD_KEEP, wparam );
            return( TRUE );
        case IDD_KEEP :
            DieCheckMeansRoll = FALSE;
            CheckRadioButton( hdlg, IDD_ROLL, IDD_KEEP, wparam );
            return( TRUE );
        case IDD_RESET :
            ResetDieChecks = !ResetDieChecks;
            CheckDlgButton( hdlg, IDD_RESET, (WORD) ResetDieChecks );
            return( TRUE );
        }
        return( FALSE );
    }
    return( FALSE );
}
