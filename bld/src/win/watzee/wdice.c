#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "watzee.h"


/*
 * GetDiceBitmaps -- load the dice bitmaps and scale them
 */

extern void GetDiceBitmaps( HWND hwnd )
/*************************************/
{
    BITMAP      bitmap1;
    BITMAP      bitmap2;
    HANDLE      hinstance;
    HDC         hdc;
    HDC         hdcmem1;
    HDC         hdcmem2;
    HBITMAP     hbitmap1;
    HBITMAP     hbitmap2;
    char        bitmapname[6];
    short       i;

    hinstance = GET_HINST( hwnd );
    hdc = CreateIC( "DISPLAY", NULL, NULL, NULL );
    hdcmem1 = CreateCompatibleDC( hdc );
    hdcmem2 = CreateCompatibleDC( hdc );
    for( i = 0; i < 6; i++ ) {
        sprintf( bitmapname, "DICE%d", i+1 );
        hbitmap1 = LoadBitmap( hinstance, bitmapname );
        GetObject( hbitmap1, sizeof( BITMAP ), (LPSTR) &bitmap1 );
        bitmap2 = bitmap1;
        bitmap2.bmWidth = CharWidth * 3;
        bitmap2.bmHeight = CharHeight * 3;
        bitmap2.bmWidthBytes = ( ( bitmap2.bmWidth + 15 ) / 16 ) * 2;
        hbitmap2 = CreateBitmapIndirect( &bitmap2 );
        SelectObject( hdcmem1, hbitmap1 );
        SelectObject( hdcmem2, hbitmap2 );
        StretchBlt( hdcmem2, 0, 0, bitmap2.bmWidth, bitmap2.bmHeight,
                    hdcmem1, 0, 0, bitmap1.bmWidth, bitmap1.bmHeight, SRCCOPY );
        SetWindowLong( hwnd, i * sizeof( DWORD ), (DWORD)hbitmap2 );
    }
    DeleteDC( hdcmem1 );
    DeleteDC( hdcmem2 );
    DeleteObject( hbitmap1 );
    DeleteDC( hdc );
}



/*
 * RollDice -- for the dice that have been selected, pick random values for
 *             them (between 1 and 6, ofcourse), and redraw the dice bitmaps
 */

extern void RollDice( HWND hwnd, HDC hdc )
/****************************************/
{
    short   i;
    short   j;

    srand( (unsigned short) time( NULL ) );
    for( i = 0; i < 150; i++ ) {
        for( j = 0; j < 5; j++ ) {
            if( ( DieCheckMeansRoll && Dice[j].is_checked )
             || ( !DieCheckMeansRoll && !Dice[j].is_checked ) ) {
                Dice[j].value = (short)(rand()%6+1);
            }
        }
        DrawDice( hwnd, hdc );
    }
    if( ResetDieChecks || CurrentRoll != 1 ) {
        for( i = 0; i < 5; i++ ) {
            CheckDlgButton( hwnd, IDW_DICE1+i, FALSE );
            Dice[i].is_checked = FALSE;
        }
    }
}



/*
 * GetDiceInfo -- after the dice have been rolled, collect some stats about
 *                them, such as how many 1's, 2's, 3's, etc. there are,
 *                and whether they form a straight, full house, Watzee, etc.
 */

extern void GetDiceInfo( void )
/*****************************/
{
    short   dice_sum;
    short   die_count[7];
    short   this_die;
    BOOL    got_three_kind;
    BOOL    got_four_kind;
    BOOL    got_full_house;
    BOOL    got_small_straight;
    BOOL    got_large_straight;
    BOOL    got_watzee;
    short   i;

    dice_sum = 0;
    got_three_kind = FALSE;
    got_four_kind = FALSE;
    got_watzee = FALSE;
    got_full_house = FALSE;
    got_small_straight = FALSE;
    got_large_straight = FALSE;
    for( i = 0; i < 7; i++ ) {
        die_count[i] = 0;
    }
    for( i = 0; i < 5; i++ ) {
        this_die = Dice[i].value;
        dice_sum += this_die;
        die_count[ this_die ]++;
    }
    for( i = ACES; i <= SIXES; i++ ) {
        DiceInfo.count[i] = die_count[i];
        if( die_count[i] >= 3 ) {
            got_three_kind = TRUE;
        }
        if( die_count[i] >= 4 ) {
            got_four_kind = TRUE;
        }
        if( die_count[i] == 5 ) {
            got_watzee = TRUE;
        }
    }
    if( got_three_kind ) {
        for( i = ACES; i <= SIXES; i++ ) {
            if( die_count[i] == 2 ) {
                got_full_house = TRUE;
                break;
            }
        }
    }
    if( die_count[3] && die_count[4] ) {
        if( ( die_count[1] && die_count[2] )
         || ( die_count[2] && die_count[5] )
         || ( die_count[5] && die_count[6] ) ) {
            got_small_straight = TRUE;
        }
    }
    if( (die_count[1] || die_count[6]) && die_count[2] && die_count[3] &&
                                          die_count[4] && die_count[5] ) {
        got_large_straight = TRUE;
    }
    DiceInfo.sum = dice_sum;
    DiceInfo.got_three_kind = got_three_kind;
    DiceInfo.got_four_kind = got_four_kind;
    DiceInfo.got_full_house = got_full_house;
    DiceInfo.got_small_straight = got_small_straight;
    DiceInfo.got_large_straight = got_large_straight;
    DiceInfo.got_watzee = got_watzee;
}
