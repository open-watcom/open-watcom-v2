#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include "life.h"


extern void Clear( void )
/************************

    Kill all cells in the Selected Region, and update the screen
*/
{
    pixels      x,y;
    pixels      start_x, end_x;
    pixels      start_y, end_y;
    HDC         dc;

    GetSelectedCoords( &start_x, &end_x, &start_y, &end_y );
    if( !SelectOn() ) Generation = 0;
    dc = GetDC( WinHandle );
    FlipSelect();
    for( y = start_y; y < end_y; ++y ) {
        for( x = start_x; x < end_x; ++x ) {
            TurnOffCell( dc, CellPointer( x, y ), x, y );
        }
    }
    FlipSelect();
    ReleaseDC( WinHandle, dc );
    SetCaption();
}

extern void Randomize( void )
/****************************

    Randomize cells in the Selected Region, and update the screen
*/
{
    pixels      x,y;
    cell_ptr    cell;
    pixels      start_x, end_x;
    pixels      start_y, end_y;
    HDC         dc;

    GetSelectedCoords( &start_x, &end_x, &start_y, &end_y );
    if( !SelectOn() ) Generation = 0;
    FlipSelect();
    dc = GetDC( WinHandle );
    srand( clock() );
    for( y = start_y; y < end_y; ++y ) {
        for( x = start_x; x < end_x; ++x ) {
            cell = CellPointer( x, y );
            if( rand() < RAND_MAX/4 ) {
                TurnOnCell( dc, cell, x, y );
            } else {
                TurnOffCell( dc, cell, x, y );
            }
        }
    }
    FlipSelect();
    ReleaseDC( WinHandle, dc );
    SetCaption();
}


static int Adjust[8][2] =
{
    { -1, -1 },
    { -1,  0 },
    { -1,  1 },
    {  0, -1 },
    {  0,  1 },
    {  1, -1 },
    {  1,  0 },
    {  1,  1 }
};


static int SumNeighbours( pixels x, pixels y )
/*********************************************

    Calculate the number of neighbours of cell at (x,y)
*/
{
    int         i;
    int         neighbours;
    pixels      newx,newy;

    neighbours = 0;
    for( i = 0; i < 8; ++i ) {
        newx = x + Adjust[i][0];
        newy = y + Adjust[i][1];
        if( CurvedSpace ) {
            WrapAround( &newx, &newy );
        }
        if( CellPointer( newx, newy )->alive ) {
            ++neighbours;
        }
    }
    return( neighbours );
}


extern void NextGeneration( void )
/*********************************

    Calculate the next generation of cells and update the screen.
*/
{
    pixels      x,y;
    cell_ptr    cell;
    int         neighbours;
    BOOL        change;
    HDC         dc;

    Generation++;

    change = FALSE;
    for( x = 0; x < ArraySizeX; ++x ) {
        for( y = 0; y < ArraySizeY; ++y ) {
            cell = CellPointer( x, y );
            if( y == 0 ) RelinquishControl();
            neighbours = SumNeighbours( x, y );
            cell->next_alive = cell->alive;
            if( cell->alive ) {
                if( Deaths[ neighbours ] ) {
                    cell->next_alive = 0;
                    change = TRUE;
                }
            } else {
                if( Births[ neighbours ] ) {
                    cell->next_alive = 1;
                    change = TRUE;
                }
            }
        }
    }

    dc = GetDC( WinHandle );
    for( x = 0; x < ArraySizeX; ++x ) {
        for( y = 0; y < ArraySizeY; ++y ) {
            cell = CellPointer( x, y );
            if( cell->next_alive ) {
                TurnOnCell( dc, cell, x, y );
            } else {
                TurnOffCell( dc, cell, x, y );
            }
            cell->next_alive = 0;
        }
    }
    ReleaseDC( WinHandle, dc );

    SetCaption();
}


extern void FreeArray( cell_array junk )
/***************************************

    Free up the cell array pointed to by "junk"
*/
{
    int         i;

    for( i = 0; junk[ i ] != NULL; ++i ) {
        _free( junk[ i ] );
    }
    free( junk );
}


extern BOOL ReSizeArray( pixels width, pixels height, WORD type )
/****************************************************************

    Record the new size of the window and redraw the screen accordingly.
    Allocate a new cell array, and copy as much of the old array into
    it as is sensible.
*/
{
    cell_array          old_array;
    pixels              old_grid_x,old_grid_y;
    pixels              x, y, max_x, max_y;
    cell_ydim           ydim;
    cell_ptr            cell;

    SelectOff();
    WindowWidth = width;
    WindowHeight = height;
    old_grid_x = ArraySizeX;
    old_grid_y = ArraySizeY;
    old_array = CellArray;
    if( type == SIZEICONIC ) {
        IsAnIcon = TRUE;
        ArraySizeX = WindowWidth;
        ArraySizeY = WindowHeight;
    } else {
        IsAnIcon = FALSE;
        ArraySizeX = WindowWidth / BitInfo.bmWidth;
        ArraySizeY = WindowHeight / BitInfo.bmHeight;
        if( ArraySizeX == 0 ) {
            ArraySizeX = 1;
        }
        if( ArraySizeY == 0 ) {
            ArraySizeY = 1;
        }
    }
    CellArray = calloc( ArraySizeX + 3, sizeof( cell_ydim ) );
    if( CellArray == NULL ) return( FALSE );
    for( x = 0; x < ArraySizeX + 2; ++x ) {
        ydim = _malloc( ArraySizeY + 2 );
        if( ydim == NULL ) return( FALSE );
        _memset( ydim, 0, ( ArraySizeY + 2 ) * sizeof( cell_type ) );
        CellArray[ x ] = ydim;
    }
    if( old_array == NULL ) {
        Randomize();
        Mode = MENU_RESUME;
    } else {
        max_x =  ArraySizeX < old_grid_x ? ArraySizeX : old_grid_x;
        max_y =  ArraySizeY < old_grid_y ? ArraySizeY : old_grid_y;
        Population = 0;
        for( x = 0; x < max_x; ++x ) {
            for( y = 0; y < max_y; ++y ) {
                cell = CellPointer( x, y );
                *cell = *ArrayPointer( old_array, x, y );
                if( cell->alive ) ++Population;
            }
        }
        SetCaption();
        FreeArray( old_array );
    }
    return( TRUE );
}


static pixels PutIntoRange( pixels x, pixels max )
/*************************************************

    Wrap a point around the edge of space if it's out of range.
*/
{
    while( x < 0 ) x += max;
    while( x >= max ) x -= max;
    return( x );
}

extern void WrapAround( pixels *x, pixels *y )
/*********************************************

    Wrap an (x,y) around the edge of space if it's out of range.
*/
{
    *x = PutIntoRange( *x, ArraySizeX );
    *y = PutIntoRange( *y, ArraySizeY );
}
