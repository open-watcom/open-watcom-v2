#include <conio.h>
#include <graph.h>
#include <stdio.h>
#include <stdlib.h>

main()
{
    int mode;
    struct videoconfig vc;
    char buf[ 80 ];

    _getvideoconfig( &vc );
    /* select "best" video mode */
    switch( vc.adapter ) {
    case _VGA :
    case _SVGA :
	mode = _VRES16COLOR;
	break;
    case _MCGA :
	mode = _MRES256COLOR;
	break;
    case _EGA :
	if( vc.monitor == _MONO ) {
	    mode = _ERESNOCOLOR;
	} else {
	    mode = _ERESCOLOR;
	}
	break;
    case _CGA :
	mode = _MRES4COLOR;
	break;
    case _HERCULES :
	mode = _HERCMONO;
	break;
    default :
	puts( "No graphics adapter" );
	exit( 1 );
    }
    if( _setvideomode( mode ) ) {
	_getvideoconfig( &vc );
	sprintf( buf, "%d x %d x %d\n", vc.numxpixels,
			 vc.numypixels, vc.numcolors );
	_outtext( buf );
	getch();
	_setvideomode( _DEFAULTMODE );
    }
}
