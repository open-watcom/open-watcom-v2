#include <stdlib.h>
#include "life.h"
#include "bitmap.h"

static HBITMAP          CellBitMap = NULL;
static HDC              MemoryDC;

extern void InitBitMap()
/**********************

    Initialize the bitmap used to draw cells
*/
{
    CellBitMap = LoadBitmap( ThisInst, "CellBitMap" );
    NewBitMap();
}


extern void FiniBitMap()
/**********************/
{
    if( CellBitMap != NULL ) DeleteObject( CellBitMap );
}


extern void LoadNewBitmap()
/**************************

    Load a new bitmap from disk, and upate accordingly
*/
{
    HBITMAP     bit_map;

    bit_map = RequestBitmapFile();
    FlushMouse();
    if( bit_map == NULL ) return;
    if( CellBitMap != NULL ) DeleteObject( CellBitMap );
    CellBitMap = bit_map;
    NewBitMap();
}



extern void NewBitMap()
/**********************

    Process a new bit map for cells, pointed to by CellBitMap
*/
{
    HDC         dc;

    dc = GetDC( WinHandle );
    MemoryDC = CreateCompatibleDC( dc );
    ReleaseDC( WinHandle, dc );
    SelectObject( MemoryDC, CellBitMap );
    GetObject( CellBitMap, sizeof( BITMAP ), (LPSTR)&BitInfo );
}


extern void BlitBitMap( HDC dc, pixels x, pixels y )
/***************************************************

    Draw a cell at location (x,y)
*/
{
    pixels      start_x, start_y;
    HANDLE      old;

    if( IsAnIcon ) {
        PatBlt( dc, x, y, 1, 1, BLACKNESS );
    } else {
        start_x = BitInfo.bmWidth*x;
        start_y = BitInfo.bmHeight*y;
        BitBlt( dc, start_x, start_y, BitInfo.bmWidth, BitInfo.bmHeight,
                MemoryDC, 0, 0, SRCCOPY );
        if( DrawGrid ) {
            old = SelectObject( dc, Brush );
            Rectangle( dc, start_x, start_y,
                       start_x+BitInfo.bmWidth+1, start_y+BitInfo.bmHeight+1 );
            SelectObject( dc, old );
        }
    }
}


extern void UnBlitBitMap( HDC dc, pixels x, pixels y )
/*****************************************************

    Erase a cell at location (x,y)
*/
{
    pixels      start_x, start_y;

    if( IsAnIcon ) {
        PatBlt( dc, x, y, 1, 1, WHITENESS );
    } else {
        start_x = BitInfo.bmWidth*x;
        start_y = BitInfo.bmHeight*y;
        if( DrawGrid ) {
            Rectangle( dc, start_x, start_y,
                       start_x+BitInfo.bmWidth+1, start_y+BitInfo.bmHeight+1 );
        } else {
            PatBlt( dc, start_x, start_y,
                    BitInfo.bmWidth, BitInfo.bmHeight, WHITENESS );
        }
    }
}
