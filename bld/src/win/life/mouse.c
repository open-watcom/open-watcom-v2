#include "life.h"

static pixels           SelectStartX, SelectStartY;
static pixels           SelectEndX, SelectEndY;
static BOOL             RegionIsSelected = FALSE;


extern BOOL SelectOn()
/********************/
{
    return( RegionIsSelected );
}


extern void SelectOff()
/**********************

    Turn off the selected region, if there is one
*/
{
    if( MouseMode != MENU_SELECT ) return;
    if( RegionIsSelected ) XORSelectedRegion();
    RegionIsSelected = FALSE;
}


static void GetCoords( pixels *x1, pixels *x2,
                       pixels start, pixels end, pixels grid )
/*************************************************************

    Return the grid co-ordinates x1,x2 based on screen co-ordinates start,end
    and grid size (in pixels) grid.
*/
{
    pixels      temp;

    if( start > end ) {
        temp = start;
        start = end;
        end = temp;
    }
    *x1 = start / grid;
    *x2 = end / grid;
}


extern void GetSelectedCoords( pixels *x1, pixels *x2, pixels *y1, pixels *y2 )
/******************************************************************************

    Get the grid co-ordinates of the currently selected region
*/
{
    if( RegionIsSelected ) {
        GetCoords( x1, x2, SelectStartX, SelectEndX, BitInfo.bmWidth );
        GetCoords( y1, y2, SelectStartY, SelectEndY, BitInfo.bmHeight );
    } else {
        *x1 = 0;
        *x2 = ArraySizeX;
        *y1 = 0;
        *y2 = ArraySizeY;
    }
}


extern void FlipSelect()
/***********************

    Flip the selected region highlight. (Turn on if off and vice-versa)
*/
{
    if( RegionIsSelected ) {
        XORSelectedRegion();
    }
}


static void XORSelectedRegion()
/**************************

    XOR the select rectangle with a black pen
*/
{
    HDC         dc;
    int         old_rop;

    dc = GetDC( WinHandle );
    SelectObject( dc, Pen );
    old_rop = SetROP2( dc, R2_XORPEN );
    Rectangle( dc, SelectStartX, SelectStartY, SelectEndX, SelectEndY );
    SetROP2( dc, old_rop );
    ReleaseDC( WinHandle, dc );
}


static void Snap( pixels *x, pixels max, pixels bitmap_size )
/************************************************************
    utility - see SnapToGrid
*/
{
    if( *x > max ) *x = max;
    if( *x < 0 ) *x = 0;
    *x += bitmap_size / 2;
    *x -= *x % bitmap_size;
}


static void NotSame( pixels *x, pixels max, pixels step, pixels start )
/**********************************************************************
    utility -- see NotSameAsStart
*/
{
    if( *x == start ) {
        if( *x + step < max ) {
            *x += step;
        } else {
            *x -= step;
        }
    }
}


static void NotSameAsStart( pixels *x, pixels *y )
/*************************************************

    Modify x, y so that they are not the same as SelectStartX, SelectStartY
*/
{
    NotSame( x, WindowWidth, BitInfo.bmWidth, SelectStartX );
    NotSame( y, WindowHeight, BitInfo.bmHeight, SelectStartY );
}


static void SnapToGrid( pixels *x, pixels *y )
/*********************************************

    Modify x, y so that they are exactly on grid co-ordinates
*/
{
    Snap( x, WindowWidth, BitInfo.bmWidth );
    Snap( y, WindowHeight, BitInfo.bmHeight );
}


extern BOOL EndSelect( pixels x, pixels y )
/******************************************

    We are done a select (the user let the mouse button go).
*/
{
    if( MouseMode != MENU_SELECT ) return( FALSE );
    SnapToGrid( &x, &y );
    ReleaseCapture();
    RegionIsSelected = TRUE;
    return( TRUE );
}

extern BOOL MoveSelect( unsigned state, pixels x, pixels y )
/***********************************************************

    The mouse moved. Update the select rectangle.
*/
{
    SnapToGrid( &x, &y );
    NotSameAsStart( &x, &y );
    if( MouseMode != MENU_SELECT ) return( FALSE );
    if( state & MK_LBUTTON ) {
        if( SelectEndX != x || SelectEndY != y ) {
            XORSelectedRegion();
            SelectEndX = x;
            SelectEndY = y;
            XORSelectedRegion();
        }
    }
    return( TRUE );
}


extern BOOL StartSelect( pixels x, pixels y )
/********************************************

    Record the spot at which selection started
*/
{
    if( MouseMode != MENU_SELECT ) return( FALSE );
    if( RegionIsSelected ) XORSelectedRegion();
    SetCapture( WinHandle );
    x -= BitInfo.bmWidth / 2;
    y -= BitInfo.bmWidth / 2;
    SnapToGrid( &x, &y );
    SelectStartX = x;
    SelectStartY = y;
    NotSameAsStart( &x, &y );
    SelectEndX = x;
    SelectEndY = y;
    XORSelectedRegion();
    return( TRUE );
}


extern void FlipPattern( unsigned state, pixels x, pixels y )
/***********************************************************

    Process a mouse event at (pixel_x,pixel_y)
*/
{
    if( MouseMode != MENU_FLIP_PATTERNS ) return;
    if( !( state & (MK_LBUTTON+MK_RBUTTON) ) ) return;
    DrawPattern( x, y, ( state & MK_RBUTTON ) != 0 );
}
