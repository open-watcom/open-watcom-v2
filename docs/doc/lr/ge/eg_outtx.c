#include <conio.h>
#include <graph.h>

main()
{
    _setvideomode( _TEXTC80 );
    _settextposition( 10, 30 );
    _outtext( "WATCOM Graphics" );
    getch();
    _setvideomode( _DEFAULTMODE );
}
