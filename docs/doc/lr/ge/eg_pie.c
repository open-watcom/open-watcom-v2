#include <conio.h>
#include <graph.h>

main()
{
    _setvideomode( _VRES16COLOR );
    _pie( _GBORDER, 120, 90, 520, 390,
                    140, 20, 190, 460 );
    getch();
    _setvideomode( _DEFAULTMODE );
}
