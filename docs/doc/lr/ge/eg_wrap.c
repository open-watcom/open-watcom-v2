#include <conio.h>
#include <graph.h>
#include <stdio.h>

main()
{
    int i;
    char buf[ 80 ];

    _setvideomode( _TEXTC80 );
    _settextwindow( 5, 20, 20, 30 );
    _wrapon( _GWRAPOFF );
    for( i = 1; i <= 3; ++i ) {
	_settextposition( 2 * i, 1 );
	sprintf( buf, "Very very long line %d", i );
	_outtext( buf );
    }
    _wrapon( _GWRAPON );
    for( i = 4; i <= 6; ++i ) {
	_settextposition( 2 * i, 1 );
	sprintf( buf, "Very very long line %d", i );
	_outtext( buf );
    }
    getch();
    _setvideomode( _DEFAULTMODE );
}
