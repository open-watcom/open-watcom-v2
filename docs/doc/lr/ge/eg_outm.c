#include <conio.h>
#include <graph.h>

main()
{
    int i;
    char buf[ 1 ];

    _clearscreen( _GCLEARSCREEN );
    for( i = 0; i <= 255; ++i ) {
        _settextposition( 1 + i % 16,
                          1 + 5 * ( i / 16 ) );
        buf[ 0 ] = i;
        _outmem( buf, 1 );
    }
    getch();
}
