#include <conio.h>
#include <graph.h>
#include <stdio.h>

main()
{
    int rows;
    char buf[ 80 ];

    rows = _setvideomoderows( _TEXTC80, _MAXTEXTROWS );
    if( rows != 0 ) {
	sprintf( buf, "Number of rows is %d\n", rows );
	_outtext( buf );
	getch();
	_setvideomode( _DEFAULTMODE );
    }
}
