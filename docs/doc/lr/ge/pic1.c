#include <conio.h>
#include <graph.h>

#define x1 120
#define y1 90
#define x2 520
#define y2 390
#define x3 500
#define y3 20
#define x4 450
#define y4 460

main()
{
    _setvideomode( _VRES16COLOR );
    _arc( x1, y1, x2, y2, x3, y3, x4, y4 );
    _setlinestyle( 0x8080 );
    _rectangle( _GBORDER, x1, y1, x2, y2 );
    _moveto( ( x1 + x2 ) / 2, ( y1 + y2 ) / 2 );
    _lineto( x3, y3 );
    _moveto( ( x1 + x2 ) / 2, ( y1 + y2 ) / 2 );
    _lineto( x4, y4 );
    _settextposition( 6, 8 ); _outtext( "(x1,y1)" );
    _settextposition( 25, 67 ); _outtext( "(x2,y2)" );
    _settextposition( 2, 64 ); _outtext( "(x3,y3)" );
    _settextposition( 29, 57 ); _outtext( "(x4,y4)" );
/*
    _settextposition( 16, 43 ); _outtext( "((x1+x2)/2,(y1+y2)/2)" );
*/
    getch();
    _setvideomode( _DEFAULTMODE );
}
