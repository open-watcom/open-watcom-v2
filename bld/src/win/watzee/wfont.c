#include <stdlib.h>
#include <windows.h>
#include "watzee.h"



/*
 * CreateFonts -- set up two logical font structures
 */

extern void CreateFonts( void )
/*****************************/
{
    SmallNormalFont.lfHeight = 9 * CharHeight / 10;
    SmallNormalFont.lfWidth = 2 * CharWidth / 5;
    SmallNormalFont.lfWeight = FW_NORMAL;
    SmallBoldFont.lfHeight = 9 * CharHeight / 10;
    SmallBoldFont.lfWidth = 2 * CharWidth / 5;
    SmallBoldFont.lfWeight = FW_BOLD;
}



/*
 * GetFontInfo -- determine the height and width of the system font
 */

extern void GetFontInfo( void )
/*****************************/
{
    HDC             hdc;
    TEXTMETRIC      tm;
    int             sc_height;

    hdc = CreateIC( "DISPLAY", NULL, NULL, NULL );
    SelectObject( hdc, GetStockObject( SYSTEM_FONT ) );
    GetTextMetrics( hdc, &tm );
    sc_height = GetSystemMetrics( SM_CYSCREEN );
    CharHeight = min( tm.tmHeight, sc_height/28 );
    SysHeight = tm.tmHeight;
    CharWidth = tm.tmMaxCharWidth;
    DeleteDC( hdc );
}
