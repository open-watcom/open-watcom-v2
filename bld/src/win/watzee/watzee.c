#include <windows.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "watzee.h"

static char     *AppName = "Watzee";
static char     ClassName[32]="Watzee";

static void                 DoScore( HWND, WORD );
static void                 DoScoreTotals( HWND );
static WORD                 GetDieCheck( HWND, POINT );
static WORD                 GetScoreCheck( POINT );
static void                 InitializeGameData( void );
static void                 NextPlayer( HWND, HDC );

long _EXPORT FAR PASCAL    WndProc( HWND, WORD, UINT, LONG );


/*
 * WinMain -- register the window class and call some initialization
 *            routines; then send a message to WndProc to start a new
 *            game
 */

int PASCAL WinMain( HANDLE instance, HANDLE previnstance,
                    LPSTR cmdline, short cmdshow )
/*******************************************************/
{
    HWND        hwnd;
    MSG         msg;
    WNDCLASS    wndclass;

    cmdline = cmdline;
    previnstance = previnstance;
#ifdef __WINDOWS_386__
    sprintf( ClassName,"Watzee%d",instance );
#else
    if( !previnstance ) {
#endif
        wndclass.style          = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc    = (LPVOID) WndProc;
        wndclass.cbClsExtra     = 0;
        wndclass.cbWndExtra     = 6 * sizeof( DWORD );
        wndclass.hInstance      = instance;
        wndclass.hIcon          = LoadIcon( instance, AppName );
        wndclass.hCursor        = LoadCursor( NULL, IDC_ARROW );
        wndclass.hbrBackground  = GetStockObject( WHITE_BRUSH );
        wndclass.lpszMenuName   = AppName;
        wndclass.lpszClassName  = ClassName;
        RegisterClass( &wndclass );
#ifndef __WINDOWS_386__
    }
#endif
    InitializeGameData();
    GetFontInfo();
    CreateFonts();
    hwnd = CreateWindow( ClassName, "WATZEE",
                         WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                         CharWidth * 28 + CharWidth / 2, CharHeight * 28,
                         NULL, NULL, instance, NULL );
    GetDiceBitmaps( hwnd );
    ShowWindow( hwnd, cmdshow );
    UpdateWindow( hwnd );
    SendMessage( hwnd, WMW_START_NEW_GAME, 0, 0 );
    while( GetMessage( &msg, NULL, 0, 0 ) ) {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
    return( msg.wParam );
}



/*
 * WndProc -- process messages from Windows
 */

long _EXPORT FAR PASCAL WndProc( HWND hwnd, UINT message, UINT wparam,
                                  LONG lparam )
/*********************************************************************/
{
    static HANDLE   hdlginstance;
    static BOOL     got_watzee_bonus;
    PAINTSTRUCT     ps;
    FARPROC         dlg_proc;
    HDC             hdc;
    POINT           point;
    short           x;
    short           y;
    short           dy;
    short           i;

    switch( message ) {
    case WM_CREATE :
        hdlginstance = ((CREATESTRUCT far *) MK_FP32((void*)lparam))->hInstance;
        x = CharWidth * 22 + CharWidth / 2;
        y = CharHeight * 3;
        dy = CharHeight * 3;
        dy += dy / 7;
       /*  create the check marks for the dice bitmaps, and the ROLL and
           OK buttons  */
        for( i = 0; i < 5; i++, y += dy ) {
            CreateWindow( "BUTTON", "", WS_CHILD|WS_VISIBLE|BS_CHECKBOX,
                          x, y, CharWidth, CharHeight, hwnd,
                          (HMENU)(IDW_DICE1+i),
                          hdlginstance,
                          NULL );
        }
        CreateWindow( "BUTTON", "ROLL", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
                      x, CharHeight * 19, 9 * CharWidth / 2 ,
                      CharHeight * 2, hwnd, (HMENU)IDW_ROLL,
                      hdlginstance, NULL );
        CreateWindow( "BUTTON", "OK", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
                      x, 43 * CharHeight / 2, 9 * CharWidth / 2,
                      CharHeight * 2, hwnd, (HMENU)IDW_OK,
                      hdlginstance, NULL );
        return( 0 );
    case WMW_START_NEW_GAME :
        dlg_proc = MakeProcInstance( GetNumPlayersDialogProc, hdlginstance );
        DialogBox( hdlginstance, "GetNumPlayers", hwnd, dlg_proc );
        FreeProcInstance( dlg_proc );
        dlg_proc = MakeProcInstance( GetInitialsDialogProc, hdlginstance );
        DialogBox( hdlginstance, "GetPlayersInitials", hwnd, dlg_proc );
        FreeProcInstance( dlg_proc );
        EnableWindow( GetDlgItem( hwnd, IDW_OK ), FALSE );
        EnableWindow( GetDlgItem( hwnd, IDW_ROLL ), TRUE );
        PlayingGameYet = TRUE;
        InvalidateRect( hwnd, NULL, FALSE );
        for( i = 0; i < NumberOfPlayers; i++ ) {
            if( Player[i][IS_COMPUTER] ) break;
        }
        if( i < NumberOfPlayers && !GotTimer ) {
            while( !SetTimer( hwnd, ID_TIMER, TIMER_INTERVAL, NULL ) ) {
                if( MessageBox( hwnd, "Too many clocks or timers are active",
                 AppName, MB_ICONEXCLAMATION|MB_RETRYCANCEL ) == IDCANCEL ) {
                    DestroyWindow( hwnd );
                    return( 0 );
                }
            }
            GotTimer = TRUE;
        }
        if( i == 0 ) {
            PCTurn = TRUE;
            EnableWindow( GetDlgItem( hwnd, IDW_ROLL ), FALSE );
        }
        hdc = GetDC( hwnd );
        RollDice( hwnd, hdc );
        GetDiceInfo();
        ReleaseDC( hwnd, hdc );
//      SetFocus( hwnd );
        return( 0 );
    case WM_COMMAND :
        switch( LOWORD( wparam ) ) {
        case IDM_NEWGAME :
            if( GotTimer ) {
                KillTimer( hwnd, ID_TIMER );
            }
            if( MessageBox( hwnd, "Start New Game:  Are you sure?", "WATZEE",
                            MB_YESNO | MB_ICONEXCLAMATION ) == IDYES ) {
                InitializeGameData();
                InvalidateRect( hwnd, NULL, TRUE );
                SendMessage( hwnd, WMW_START_NEW_GAME, 0, 0 );
            } else if( GotTimer ) {
                SetTimer( hwnd, ID_TIMER, TIMER_INTERVAL, NULL );
            }
            break;
        case IDM_OPTIONS :
            if( GotTimer ) {
                KillTimer( hwnd, ID_TIMER );
            }
            dlg_proc = MakeProcInstance( OptionsDialogProc, hdlginstance );
            DialogBox( hdlginstance, "Options", hwnd, dlg_proc );
            FreeProcInstance( dlg_proc );
            if( GotTimer ) {
                SetTimer( hwnd, ID_TIMER, TIMER_INTERVAL, NULL );
            }
            break;
        case IDM_EXIT :
            DestroyWindow( hwnd );
            break;
        case IDM_SCORING :
            if( GotTimer ) {
                KillTimer( hwnd, ID_TIMER );
            }
            dlg_proc = MakeProcInstance( HelpDialogProc, hdlginstance );
            DialogBox( hdlginstance, "WatzeeHelp", hwnd, dlg_proc );
            FreeProcInstance( dlg_proc );
            if( GotTimer ) {
                SetTimer( hwnd, ID_TIMER, TIMER_INTERVAL, NULL );
            }
            break;
        case IDM_ABOUT :
            if( GotTimer ) {
                KillTimer( hwnd, ID_TIMER );
            }
            dlg_proc = MakeProcInstance( AboutDialogProc, hdlginstance );
            DialogBox( hdlginstance, "AboutWatzee", hwnd, dlg_proc );
            FreeProcInstance( dlg_proc );
            if( GotTimer ) {
                SetTimer( hwnd, ID_TIMER, TIMER_INTERVAL, NULL );
            }
            break;
        case IDW_DICE1 :
        case IDW_DICE2 :
        case IDW_DICE3 :
        case IDW_DICE4 :
        case IDW_DICE5 :
            if( !PCTurn && CurrentRoll < 2 ) {
                SendMessage( hwnd, WMW_DIE_CHECK, wparam, 0 );
            }
            break;
        case IDW_ROLL :
            SendMessage( hwnd, WMW_ROLL, 0, 0 );
            break;
        case IDW_OK :
            SendMessage( hwnd, WMW_OK, 0, 0 );
            break;
        }
        return( 0 );
    case WM_LBUTTONUP :
        if( !PCTurn ) {
            MAKE_POINT( point, lparam );
            if( point.x > CharWidth * 24
             && point.y < CharHeight * 20 ) {
                wparam = GetDieCheck( hwnd, point );
                if( wparam && CurrentRoll < 2 ) {
                    SendMessage( hwnd, WMW_DIE_CHECK, wparam, 0 );
                }
            } else {
                wparam = GetScoreCheck( point );
                if( wparam && Player[CurrentPlayer][wparam] == UNDEFINED ) {
                     SendMessage( hwnd, WMW_SCORE_CHECK, wparam, 0 );
                }
            }
        }
        return( 0 );
    case WMW_DIE_CHECK :
        i = LOWORD( wparam ) - IDW_DICE1;
        Dice[i].is_checked = !Dice[i].is_checked;
        CheckDlgButton( hwnd, wparam, (BOOL)Dice[i].is_checked );
//      SetFocus( hwnd );
        return( 0 );
    case WMW_SCORE_CHECK :
        hdc = GetDC( hwnd );
        if( wparam != LastScoreSelection ) {
            DoScore( hdc, wparam );
            EnableWindow( GetDlgItem( hwnd, IDW_OK ), TRUE );
        }
        ReleaseDC( hwnd, hdc );
//      SetFocus( hwnd );
        return( 0 );
    case WMW_ROLL :
        hdc = GetDC( hwnd );
        if( DieCheckMeansRoll ) {
            for( i = 0; i < 5; i++ ) {
                if( Dice[i].is_checked ) break;
            }
        } else {
            for( i = 0; i < 5; i++ ) {
                if( !Dice[i].is_checked ) break;
            }
        }
        if( i < 5 ) {
            if( LastScoreSelection ) {
                Player[CurrentPlayer][LastScoreSelection] = UNDEFINED;
                WriteScore( hdc, CurrentPlayer, LastScoreSelection );
                DoScoreTotals( hdc );
                LastScoreSelection = 0;
            }
            got_watzee_bonus = FALSE;
            CurrentRoll++;
            RollDice( hwnd, hdc );
            GetDiceInfo();
            if( DiceInfo.got_watzee && Player[CurrentPlayer][WATZEE] == 50 ) {
                got_watzee_bonus = TRUE;
            }
            if( CurrentRoll == 2 ) {
                EnableWindow( GetDlgItem( hwnd, IDW_ROLL ), FALSE );
            }
        }
        ReleaseDC( hwnd, hdc );
//      SetFocus( hwnd );
        return( 0 );
    case WMW_OK :
        hdc = GetDC( hwnd );
        LastScoreSelection = 0;
        if( got_watzee_bonus ) {
            DoScore( hdc, WATZEE_BONUS );
            LastScoreSelection = 0;
            got_watzee_bonus = FALSE;
        }
        NextPlayer( hwnd, hdc );
        ReleaseDC( hwnd, hdc );
//      SetFocus( hwnd );
        return( 0 );
    case WMW_GAME_OVER :
        hdc = GetDC( hwnd );
        WriteScoreOptions( hdc );
        ReleaseDC( hwnd, hdc );
        if( GotTimer ) {
            KillTimer( hwnd, ID_TIMER );
            GotTimer = FALSE;
        }
        if( MessageBox( hwnd, "Another Game ?", "WATZEE", MB_YESNO ) == IDYES ) {
            InitializeGameData();
            InvalidateRect( hwnd, NULL, TRUE );
            SendMessage( hwnd, WMW_START_NEW_GAME, 0, 0 );
        } else {
            DestroyWindow( hwnd );
        }
        return( 0 );
    case WM_TIMER :
        if( PCTurn ) {
            PCPlay( hwnd );
        }
        return( 0 );
    case WM_PAINT :
        hdc = BeginPaint( hwnd, &ps );
        DrawDice( hwnd, hdc );
        DrawScoreCard( hdc );
        WriteScoreOptions( hdc );
        if( PlayingGameYet ) {
            WriteInitials( hdc );
            HighliteName( hdc, CurrentPlayer );
            WriteScores( hdc );
        }
        EndPaint( hwnd, &ps );
        return( 0 );
    case WM_DESTROY :
        {
            HBITMAP     hbm;

            if( GotTimer ) {
                KillTimer( hwnd, ID_TIMER );
            }
            for( i = 0; i < 6; i++ ) {
                hbm = (HBITMAP)GetWindowLong( hwnd, i * sizeof( DWORD ) );
                DeleteObject( hbm );
            }
            PostQuitMessage( 0 );
            return( 0 );
        }
    }
    return( DefWindowProc( hwnd, message, wparam, lparam ) );
}



/*
 * DoScore -- update the score card after a score selection
 */

static void DoScore( HDC hdc, WORD selection )
/********************************************/
{
    HFONT   hfont;
    short   score;

    score = 0;
    if( LastScoreSelection ) {
        Player[CurrentPlayer][LastScoreSelection] = UNDEFINED;
        WriteScore( hdc, CurrentPlayer, LastScoreSelection );
        DoScoreTotals( hdc );
    }
    LastScoreSelection = selection;
    switch( selection ) {
    case ACES :
    case TWOS :
    case THREES :
    case FOURS :
    case FIVES :
    case SIXES :
        score = DiceInfo.count[selection] * selection;
        break;
    case THREE_KIND :
        if( DiceInfo.got_three_kind ) {
            score = DiceInfo.sum;
        }
        break;
    case FOUR_KIND :
        if( DiceInfo.got_four_kind ) {
            score = DiceInfo.sum;
        }
        break;
    case FULL_HOUSE :
        if( DiceInfo.got_full_house ) {
            score = 25;
        }
        break;
    case SMALL_STRAIGHT :
        if( DiceInfo.got_small_straight ) {
            score = 30;
        }
        break;
    case LARGE_STRAIGHT :
        if( DiceInfo.got_large_straight ) {
            score = 40;
        }
        break;
    case WATZEE :
        if( DiceInfo.got_watzee ) {
            score = 50;
        }
        break;
    case WATZEE_BONUS :
        score = Player[CurrentPlayer][WATZEE_BONUS];
        if( score == UNDEFINED ) {
            score = 100;
        } else {
            score += 100;
        }
        break;
    case CHANCE :
        score = DiceInfo.sum;
        break;
    }
    Player[CurrentPlayer][selection] = score;
    hfont = SelectObject( hdc, CreateFontIndirect( &SmallBoldFont ) );
    WriteScore( hdc, CurrentPlayer, selection );
    DeleteObject( SelectObject( hdc, hfont ) );
    DoScoreTotals( hdc );
}



/*
 * DoScoreTotals -- update the current player's Upper Total, Bonus, Lower Total
 *                  and Grand Total
 */

static void DoScoreTotals( HDC hdc )
/**********************************/
{
    short       upper_total;
    short       lower_total;
    short       i;

    upper_total = 0;
    lower_total = 0;
    for( i = ACES; i <= SIXES; i++ ) {
        if( Player[CurrentPlayer][i] != UNDEFINED ) {
            upper_total += Player[CurrentPlayer][i];
        }
    }
    if( upper_total >= 63 ) {
        upper_total += 35;
        Player[CurrentPlayer][BONUS] = 35;
    }
    Player[CurrentPlayer][UPPER_TOTAL] = upper_total;
    for( i = THREE_KIND; i <= WATZEE_BONUS; i++ ) {
        if( Player[CurrentPlayer][i] != UNDEFINED ) {
            lower_total += Player[CurrentPlayer][i];
        }
    }
    Player[CurrentPlayer][LOWER_TOTAL] = lower_total;
    Player[CurrentPlayer][GRAND_TOTAL] = upper_total + lower_total;
    SelectObject( hdc, GetStockObject( SYSTEM_FONT ) );
    for( i = BONUS; i <= GRAND_TOTAL; i++ ) {
        WriteScore( hdc, CurrentPlayer, i );
    }
}



/*
 * GetDieCheck -- determine where on the score card the user just clicked,
 *                and decide whether the user just checked one of the die;
 *                return a value between 1 and 5 indicating which die, or
 *                zero if a die was not clicked
 */

static WORD GetDieCheck( HWND hwnd, POINT point )
/***********************************************/
{
    BITMAP      bm;
    RECT        rect;
    WORD        die_checked;
    short       dy;
    short       i;
    HBITMAP     hbm;

    die_checked = 0;
    hbm = (HBITMAP)GetWindowLong( hwnd, 0 );
    GetObject( hbm, sizeof( BITMAP ), (LPSTR) &bm );
    rect.top = CharHeight * 2;
    rect.bottom = rect.top + bm.bmHeight;
    rect.left = CharWidth * 24;
    rect.right = rect.left + bm.bmWidth;
    dy = bm.bmHeight + bm.bmHeight / 7;
    for( i = 0; i < 5; i++ ) {
        if( PtInRect( &rect, point ) ) break;
        rect.top += dy;
        rect.bottom += dy;
    }
    if( i < 5 ) {
        die_checked = IDW_DICE1 + i;
    }
    return( die_checked );
}



/*
 * GetScoreCheck -- determine where on the score card the user just clicked,
 *                  and decide whether the user just made a valid score
 *                  selection; return the score option that was chosen, or
 *                  zero if a score option was not clicked
 */

static WORD GetScoreCheck( POINT point )
/**************************************/
{
    short       x;
    short       y;
    short       char_height;
    short       char_width;
    short       score_option_vline;
    short       left_column;
    short       right_column;
    short       score_selection;

    x = point.x;
    y = point.y;
    score_selection = 0;
    char_height = CharHeight;
    char_width = CharWidth;
    score_option_vline = char_width * SCORE_OPTIONS_WIDTH + char_width;
    left_column = score_option_vline + CurrentPlayer * char_width * 3;
    right_column = left_column + char_width * 3;
    if( ( x < score_option_vline && x > char_width )
     || ( x > left_column && x < right_column ) ) {
        if( y < char_height * 9 && y > char_height * 3 ) {
            if( y < char_height * 6 ) {
                if( y < char_height * 4 ) {
                    score_selection = ACES;
                } else if( y < char_height * 5 ) {
                    score_selection = TWOS;
                } else {
                    score_selection = THREES;
                }
            } else {
                if( y < char_height * 7 ) {
                    score_selection = FOURS;
                } else if( y < char_height * 8 ) {
                    score_selection = FIVES;
                } else {
                    score_selection = SIXES;
                }
            }
        } else if( y > char_height * 12 && y < char_height * 19 ) {
            if( y < char_height * 16 ) {
                if( y < char_height * 14 ) {
                    if( y < char_height * 13 ) {
                        score_selection = THREE_KIND;
                    } else {
                        score_selection = FOUR_KIND;
                    }
                } else if( y < char_height * 15 ) {
                    score_selection = FULL_HOUSE;
                } else {
                    score_selection = SMALL_STRAIGHT;
                }
            } else if( y < char_height * 17 ) {
                score_selection = LARGE_STRAIGHT;
            } else if( y < char_height * 18 ) {
                score_selection = WATZEE;
            } else {
                score_selection = CHANCE;
            }
        }
    }
    return( score_selection );
}



/*
 * InitializeGameData -- initialize the scores, the player's initials, and
 *                       other global variables
 */

static void InitializeGameData( void )
/************************************/
{
    short       i;
    short       j;

    NumberOfPlayers = 0;
    PlayingGameYet = FALSE;
    CurrentTurn = 0;
    CurrentPlayer = 0;
    CurrentRoll = 0;
    DieCheckMeansRoll = TRUE;
    ResetDieChecks = TRUE;
    GotTimer = FALSE;
    LastScoreSelection = 0;
    PCTurn = FALSE;
    for( i = 0; i < MAX_PLAYERS; i++ ) {
        Player[i][IS_COMPUTER] = FALSE;
        for( j = ACES; j < LOWER_TOTAL; j++ ) {
            Player[i][j] = UNDEFINED;
        }
        Player[i][UPPER_TOTAL] = 0;
        Player[i][LOWER_TOTAL] = 0;
        Player[i][GRAND_TOTAL] = 0;
        sprintf( PlayerName[i], "   " );
    }
    for( i = 0; i < 5; i++ ) {
        Dice[i].value = i+1;
        Dice[i].is_checked = TRUE;
    }
}



/*
 * NextPlayer -- determine if the game is over; if not, reset the
 *               CurrentTurn counter for the next player and initialize
 *               the dice
 */

static void NextPlayer( HWND hwnd, HDC hdc )
/******************************************/
{
    BOOL    reset_value;
    short   i;

    PCTurn = FALSE;
    HighliteName( hdc, CurrentPlayer );
    CurrentPlayer++;
    if( CurrentPlayer == NumberOfPlayers ) {
        CurrentPlayer = 0;
        CurrentTurn++;
        if( CurrentTurn == 13 ) {
            SendMessage( hwnd, WMW_GAME_OVER, 0, 0 );
            return;
        }
    }
    HighliteName( hdc, CurrentPlayer );
    WriteScoreOptions( hdc );
    CurrentRoll = 0;
    reset_value = FALSE;
    if( DieCheckMeansRoll ) {
        reset_value = TRUE;
    }
    for( i = 0; i < 5; i++ ) {
        Dice[i].is_checked = reset_value;
        CheckDlgButton( hwnd, IDW_DICE1+i, reset_value );
    }
    RollDice( hwnd, hdc );
    GetDiceInfo();
    EnableWindow( GetDlgItem( hwnd, IDW_OK ), FALSE );
    if( Player[CurrentPlayer][IS_COMPUTER] ) {
        PCTurn = TRUE;
        EnableWindow( GetDlgItem( hwnd, IDW_ROLL ), FALSE );
    } else {
        EnableWindow( GetDlgItem( hwnd, IDW_ROLL ), TRUE );
    }
}
