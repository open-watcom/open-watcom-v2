#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "watzee.h"


/*
 * DrawScoreCard -- draw the lines that make up the score card
 */

extern void DrawScoreCard( HDC hdc )
/**********************************/
{
    short   score_card_right;
    short   score_card_bottom;
    short   x;
    short   y;
    short   i;

    /*  draw the horizontal lines of the score card  */
    score_card_right = CharWidth + CharWidth * SCORE_CARD_WIDTH;
    score_card_bottom = CharHeight * 23;
    y = CharHeight + CharHeight / 2 - 1;
    for( i = 0; i < 2; i++, y++ ) {
        MoveTo( hdc, CharWidth + CharWidth * SCORE_OPTIONS_WIDTH - 1, y );
        LineTo( hdc, score_card_right + 2, y );
    }
    y = CharHeight * 3 - 1;
    MoveTo( hdc, CharWidth, y );
    LineTo( hdc, score_card_right, y );
    for( ++y; y <= CharHeight * 9; y += CharHeight ) {
        MoveTo( hdc, CharWidth, y );
        LineTo( hdc, score_card_right, y );
    }
    y += CharHeight / 2;
    MoveTo( hdc, CharWidth, y );
    LineTo( hdc, score_card_right, y );
    y += CharHeight + CharHeight / 2 - 1;
    MoveTo( hdc, CharWidth, y );
    LineTo( hdc, score_card_right, y );
    for( ++y; y <= CharHeight * 20; y += CharHeight ) {
        MoveTo( hdc, CharWidth, y );
        LineTo( hdc, score_card_right, y );
    }
    y += CharHeight / 2 - 1;
    MoveTo( hdc, CharWidth, y );
    LineTo( hdc, score_card_right, y );
    y++;
    MoveTo( hdc, CharWidth, y );
    LineTo( hdc, score_card_right, y );
    y += CharHeight + CharHeight / 2;
    MoveTo( hdc, CharWidth, y );
    LineTo( hdc, score_card_right, y );
    y++;
    MoveTo( hdc, CharWidth, y );
    LineTo( hdc, score_card_right, y );
    /*  draw the vertical lines of the score card  */
    x = CharWidth;
    for( i = 0; i < 2; i++, x++ ) {
        MoveTo( hdc, x, CharHeight*3 );
        LineTo( hdc, x, score_card_bottom );
    }
    x = CharWidth + CharWidth * SCORE_OPTIONS_WIDTH - 1;
    y = CharHeight + CharHeight / 2;
    MoveTo( hdc, x, y );
    LineTo( hdc, x, score_card_bottom );
    for( ++x; x <= score_card_right; x += CharWidth * 3 ) {
        MoveTo( hdc, x, y );
        LineTo( hdc, x, score_card_bottom );
    }
    x = score_card_right + 1;
    MoveTo( hdc, x, y );
    LineTo( hdc, x, score_card_bottom );
    /*  draw the score card's shadow */
    y = CharHeight * 3 - CharHeight / 3;
    x = score_card_right + 1;
    score_card_right += 3 * CharWidth / 4;
    for(; x < score_card_right; x++ ) {
        MoveTo( hdc, x, y );
        LineTo( hdc, x, score_card_bottom );
    }
    x = CharWidth * 2;
    y = score_card_bottom;
    score_card_bottom += 3 * CharHeight / 4;
    for(; y < score_card_bottom; y++ ) {
        MoveTo( hdc, x, y );
        LineTo( hdc, score_card_right, y );
    }
}



/*
 * WriteScoreOptions -- fill in the score options down the leftmost column
 *                      of the score card
 */

extern void WriteScoreOptions( HDC hdc )
/**************************************/
{
    static char *score_options[] = { NULL, "Aces", "Twos", "Threes",
                                     "Fours", "Fives", "Sixes",
                                     "3 of a Kind", "4 of a Kind",
                                     "Full House", "Small Straight",
                                     "Large Straight", "WATZEE",
                                     "Chance", "WATZEE Bonus" };
    HFONT       hfont;
    RECT        rect;
    short       pass;
    short       score;
    short       y;
    short       i;

    SelectObject( hdc, GetStockObject( SYSTEM_FONT ) );
    rect.left = CharWidth + 2;
    rect.right = CharWidth + CharWidth * SCORE_OPTIONS_WIDTH - 1;
    rect.top = CharHeight * 9 + 3;
    rect.bottom = rect.top + 3 * CharHeight / 2 - 2;
    DrawText( hdc, "BONUS", 5, &rect, DT_CENTER | DT_VCENTER );
    rect.top = rect.bottom + 3;
    rect.bottom += 3 * CharHeight / 2;
    DrawText( hdc, "TOTAL", 5, &rect, DT_CENTER | DT_VCENTER );
    rect.top = CharHeight * 20 + 4;
    rect.bottom = rect.top + CharHeight + CharHeight / 2;
    DrawText( hdc, "TOTAL", 5, &rect, DT_CENTER | DT_VCENTER );
    rect.top = rect.bottom + 1;
    rect.bottom = rect.top + CharHeight + CharHeight / 2 - 1;
    DrawText( hdc, "GRAND TOTAL", 11, &rect, DT_CENTER | DT_VCENTER );
    hfont = SelectObject( hdc, CreateFontIndirect( &SmallBoldFont ) );
    for( pass = 0; pass < 2; pass++ ) {
        y = CharHeight * 3;
        if( pass == 1 ) {
            hfont = SelectObject( hdc, CreateFontIndirect( &SmallNormalFont ) );
        }
        for( i = ACES; i <= SIXES; i++ ) {
            score = Player[CurrentPlayer][i];
            if( ( pass == 0 && score == UNDEFINED )
             || ( pass && score != UNDEFINED ) )   {
                rect.top = y + 1;
                rect.bottom = y + CharHeight - 1;
                FillRect( hdc, &rect, GetStockObject( WHITE_BRUSH ) );
                DrawText( hdc,score_options[i],-1,&rect,DT_CENTER|DT_VCENTER );
            }
            y += CharHeight;
        }
        y = CharHeight * 12;
        for( i = THREE_KIND; i <= CHANCE; i++ ) {
            score = Player[CurrentPlayer][i];
            if( ( pass == 0 && score == UNDEFINED )
             || ( pass && score != UNDEFINED ) )   {
                rect.top = y + 1;
                rect.bottom = y + CharHeight - 1;
                FillRect( hdc, &rect, GetStockObject( WHITE_BRUSH ) );
                DrawText( hdc,score_options[i],-1,&rect,DT_CENTER|DT_VCENTER );
            }
            y += CharHeight;
        }
        if( ( pass == 0 && Player[CurrentPlayer][WATZEE] == 50 )
         || ( pass == 1 && Player[CurrentPlayer][WATZEE] != 50 ) ) {
             rect.top = CharHeight * 19 + 1;
             rect.bottom = rect.top + CharHeight - 1;
             FillRect( hdc, &rect, GetStockObject( WHITE_BRUSH ) );
             DrawText( hdc, score_options[WATZEE_BONUS], -1, &rect,
                       DT_CENTER|DT_VCENTER );
        }
        DeleteObject( SelectObject( hdc, hfont ) );
    }
}




/*
 * WriteInitials -- write the players' initials across the top of the
 *                  score card
 */

extern void WriteInitials( HDC hdc )
/**********************************/
{
    RECT    rect;
    short   i;

    SelectObject( hdc, GetStockObject( SYSTEM_FONT ) );
    rect.left = CharWidth + CharWidth * SCORE_OPTIONS_WIDTH + 1;
    rect.right = rect.left + 3 * CharWidth - 2;
    rect.top = CharHeight + CharHeight / 2 + 2;
    rect.bottom = CharHeight * 3 - 2;
    rect.top++;
    rect.bottom++;
    for( i = 0; i < NumberOfPlayers; i++ ) {
        FillRect( hdc, &rect, GetStockObject( WHITE_BRUSH ) );
        DrawText( hdc, PlayerName[i], -1, &rect, DT_CENTER | DT_VCENTER );
        rect.left += 3 * CharWidth;
        rect.right += 3 * CharWidth;
    }
}



/*
 * DrawDice -- "blit" out the dice bitmaps
 */

extern void DrawDice( HWND hwnd, HDC hdc )
/****************************************/
{
    HDC         hdcMem;
    HBITMAP     hdicebitmap[7];
    BITMAP      bm;
    short       x;
    short       y;
    short       i;

    for( i = 0; i < 6; i++ ) {
        hdicebitmap[i+1] = GetWindowWord( hwnd, i*2 );
    }
    hdcMem = CreateCompatibleDC( hdc );
    GetObject( hdicebitmap[1], sizeof( BITMAP ), (LPSTR) &bm );
    x = CharWidth * 24;
    y = CharHeight * 2;
    for( i = 0; i < 5; i++ ) {
        SelectObject( hdcMem, hdicebitmap[Dice[i].value] );
        BitBlt( hdc, x, y, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY );
        y += bm.bmHeight + bm.bmHeight / 7;
    }
    DeleteDC( hdcMem );
}




/*
 * HighliteName -- invert the rectangle at the top of the score card that
 *                 contains the initials of the player whose turn it is
 */

extern void HighliteName( HDC hdc, short player )
/***********************************************/
{
    RECT    rect;

    rect.left = CharWidth + CharWidth * SCORE_OPTIONS_WIDTH
                + player * 3 * CharWidth + 1;
    rect.right = rect.left + 3 * CharWidth - 1;
    rect.top = CharHeight + CharHeight / 2 + 1;
    rect.bottom = CharHeight * 3 - 1;
    InvertRect( hdc, &rect );
}



/*
 * WriteScore -- write out a particular score
 */

extern void WriteScore( HDC hdc, short player, short selection )
/**************************************************************/
{
    RECT    rect;
    char    score_str[4];
    short   score;

    score = Player[player][selection];
    rect.left = CharWidth + CharWidth * SCORE_OPTIONS_WIDTH + player * 3
                * CharWidth + 1;
    rect.right = rect.left + CharWidth * 3 - 2;
    switch( selection ) {
    case ACES :
    case TWOS :
    case THREES :
    case FOURS :
    case FIVES :
    case SIXES :
        rect.top = CharHeight * ( 2 + selection );
        rect.bottom = rect.top + CharHeight;
        break;
    case BONUS :
        rect.top = CharHeight * 9 + 3;
        rect.bottom = rect.top + CharHeight + CharHeight / 2 - 3;
        break;
    case UPPER_TOTAL :
        rect.top = CharHeight * 10 + CharHeight / 2 + 3;
        rect.bottom = rect.top + CharHeight + CharHeight / 2 - 3;
        break;
    case THREE_KIND :
    case FOUR_KIND :
    case FULL_HOUSE :
    case SMALL_STRAIGHT :
    case LARGE_STRAIGHT :
    case WATZEE :
    case CHANCE :
    case WATZEE_BONUS :
        rect.top = CharHeight * ( 12 + selection - THREE_KIND );
        rect.bottom = rect.top + CharHeight;
        break;
    case LOWER_TOTAL :
        rect.top = CharHeight * 20 + 3;
        rect.bottom = rect.top + CharHeight + CharHeight / 2 - 3;
        break;
    case GRAND_TOTAL :
        rect.top = CharHeight * 21 + CharHeight / 2 + 3;
        rect.bottom = rect.top + CharHeight + CharHeight / 2 - 3;
        break;
    }
    rect.top++;
    rect.bottom--;
    FillRect( hdc, &rect, GetStockObject( WHITE_BRUSH ) );
    if( score != UNDEFINED ) {
        sprintf( score_str, "%d", score );
        DrawText( hdc, score_str, -1, &rect, DT_CENTER | DT_VCENTER );
    }
}



/*
 * WriteScores -- write out all of the scores
 */

extern void WriteScores( HDC hdc )
/********************************/
{
    HFONT       hfont;
    short       i;
    short       j;

    hfont = SelectObject( hdc, CreateFontIndirect( &SmallBoldFont ) );
    for( i = 0; i < NumberOfPlayers; i++ ) {
        for( j = ACES; j < BONUS; j++ ) {
            WriteScore( hdc, i, j );
        }
    }
    DeleteObject( SelectObject( hdc, hfont ) );
    SelectObject( hdc, GetStockObject( SYSTEM_FONT ) );
    for( i = 0; i < NumberOfPlayers; i++ ) {
        for( j = BONUS; j <= GRAND_TOTAL; j++ ) {
            WriteScore( hdc, i, j );
        }
    }
}
