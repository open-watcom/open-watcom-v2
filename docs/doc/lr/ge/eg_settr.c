#include <conio.h>
#include <graph.h>
#include <stdio.h>

int valid_rows[] = {
    14, 25, 28, 30,
    34, 43, 50, 60
};

main()
{
    int i, j, rows;
    char buf[ 80 ];

    for( i = 0; i < 8; ++i ) {
	rows = valid_rows[ i ];
	if( _settextrows( rows ) == rows ) {
	    for( j = 1; j <= rows; ++j ) {
		sprintf( buf, "Line %d", j );
		_settextposition( j, 1 );
		_outtext( buf );
	    }
	    getch();
	}
    }
    _setvideomode( _DEFAULTMODE );
}
