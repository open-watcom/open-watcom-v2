#include <conio.h>
#include <graph.h>
#include <malloc.h>

main()
{
    char *buf;
    int y;

    _setvideomode( _VRES16COLOR );
    _ellipse( _GFILLINTERIOR, 100, 100, 200, 200 );
    buf = (char*) malloc(
		  _imagesize( 100, 100, 201, 201 ) );
    if( buf != NULL ) {
	_getimage( 100, 100, 201, 201, buf );
	_putimage( 260, 200, buf, _GPSET );
	_putimage( 420, 100, buf, _GPSET );
	for( y = 100; y < 300; ) {
	    _putimage( 420, y, buf, _GXOR );
	    y += 20;
	    _putimage( 420, y, buf, _GXOR );
	}
	free( buf );
    }
    getch();
    _setvideomode( _DEFAULTMODE );
}
