#include <stdio.h>
#include <conio.h>
#include <graph.h>
#include <time.h>


struct videoconfig      VC;
int                     TextColor;
int                     TextColor2;
int                     BorderColor;


int main( void )
/*============*/

{
    int                 width, y;

    if( !InitScreen() ) {
        puts( "No graphics adapter present" );
        return( 1 );
    }

/*  sweep text in from top & bottom of screen, gradually increasing
    character size as the center of the screen is approached. */

    _setcolor( TextColor );
    width = 0;
    for( y = 5; y < VC.numypixels / 2 - 10; y += 5, ++width ) {
        DrawText( width, y );
    }

/*  draw text over final positions using a different color index */

    _setcolor( TextColor2 );
    DrawText( width, y );

/*  draw a border around the screen */

    _setcolor( BorderColor );
    _rectangle( _GBORDER, 0, 0,
                          VC.numxpixels - 1, VC.numypixels - 1 );

    if( VC.adapter > _MCGA ) {
        FadeColors();
    }

/*  wait for keyboard input and then reset the screen. */

    _settextposition( VC.numtextrows, VC.numtextcols - 16 );
    _outtext( "Press any key..." );
    getch();
    _setvideomode( _DEFAULTMODE );
    return( 0 );
}


static void DrawText( short width, short y )
/*==========================================

    This routine displays the text strings. */

{
    int                 xc;

    xc = VC.numxpixels / 2;
    _setcharsize( width, width * 3 / 2 );
    _settextalign( _CENTER, _BOTTOM );
    _grtext( xc, y, "WATCOM C" );
    _setcharsize( width, width );
    _settextalign( _CENTER, _TOP );
    _grtext( xc, VC.numypixels - y, "GRAPHICS" );
}


static int InitScreen( void )
/*===========================

    This routine selects the best video mode for a given adapter. */

{
    int                 mode;

    _getvideoconfig( &VC );
    switch( VC.adapter ) {
    case _VGA :
    case _SVGA :
        mode = _VRES16COLOR;
        break;
    case _MCGA :
        mode = _MRES256COLOR;
        break;
    case _EGA :
        if( VC.monitor == _MONO ) {
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
        return( 0 );          /* report insufficient hardware */
    }

    if( _setvideomode( mode ) == 0 ) {
        return( 0 );
    }
    _getvideoconfig( &VC );
    if( VC.numcolors < 4 ) {
        TextColor = 1;
        TextColor2 = 1;
        BorderColor = 1;
    } else {
        TextColor = 1;
        TextColor2 = 3;
        BorderColor = 2;
    }
    if( VC.adapter >= _MCGA ) {
        NewColors();
    }
    return( 1 );
}


static void NewColors( void )
/*===========================

    This routine sets the default colors for the program. */

{
    _remappalette( TextColor, 0x3f0000 );  /* light blue */
    _remappalette( TextColor2, 0x3f0000 ); /* light blue */
    _remappalette( BorderColor, _BLACK );  /* black      */
}


static void FadeColors( void )
/*============================

    This routine gradually fades the background text, brightening
    the foreground text and the border at the same time. */

{
    int                 i;
    long                red, blue, green;

    for( i = 1; i < 64; i++ ) {
        red = i;
        green = i << 8;
        blue = (long) ( 63 - i ) << 16;
        _remappalette( TextColor, blue );
        _remappalette( TextColor2, blue + green );
        _remappalette( BorderColor, red );
    }
}
