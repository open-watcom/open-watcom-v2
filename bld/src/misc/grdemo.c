#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <math.h>
#include <dos.h>
#if defined( __PC98__ )
  #include <graph98.h>
#else
  #include <graph.h>
#endif


#define PI          3.141592654

struct videoconfig  VC;
char            Main_Title[] = "Business Applications";
char            Y_Axis_Title[] = "Net Gain (x $1000)";
char            X_Axis_Title[] = "Fiscal Year";
int             TextColour;
int             TextColour2;
int             BorderColour;
int             TitleColour;
unsigned char   Masks[ 8 ][ 8 ] = {
    { 0xff, 0x81, 0xff, 0x42, 0xff, 0x24, 0xff, 0x18 },
    { 0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81 },
    { 0x99, 0x18, 0x24, 0xc3, 0xc3, 0x24, 0x18, 0x99 },
    { 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55 },
    { 0x88, 0x44, 0x22, 0x11, 0x88, 0x44, 0x22, 0x11 },
    { 0x18, 0xdb, 0x3c, 0x18, 0x18, 0x3c, 0xdb, 0x18 },
    { 0x11, 0x22, 0x44, 0x88, 0x11, 0x22, 0x44, 0x88 },
    { 0x18, 0x18, 0x18, 0xff, 0xff, 0x18, 0x18, 0x18 }
};

#define NUMSECT     10
int                 Values[ NUMSECT ] = {   /* Scaled with max of 100 */
    20, 30, 40, 35, 50, 60, 75, 70, 80, 90
};


int main( void )
/*============*/
{
    if( !InitScreen() ) {
        puts( "No graphics adapter present" );
        return( 1 );
    }
    Do_Demo1();
    Press_any_key();
    Do_Demo2();
    Press_any_key();
    _setvideomode( _DEFAULTMODE );      /* reset the screen */
    return( 0 );
}


static void Do_Demo1()
/*==================*/
{
    int                 width, y;

/*  sweep text in from top & bottom of screen, gradually increasing
    character size as the center of the screen is approached. */

    _setcolor( TextColour );
    width = 0;
    for( y = 5; y < VC.numypixels / 2 - 10; y += 5, ++width ) {
        DrawText( width, y );
    }

/*  draw text over final positions using a different color index */

    _setcolor( TextColour2 );
    DrawText( width, y );

/*  draw a border around the screen */

    _setcolor( BorderColour );
    _rectangle( _GBORDER, 0, 0, VC.numxpixels - 1, VC.numypixels - 1 );
#if defined( __PC98__ )
    FadeColors();
#else
    if( VC.adapter > _MCGA ) {
        FadeColors();
    }
#endif
}


static void Press_any_key()
/*=======================*/

/*  wait for keyboard input */
{
    _settextposition( VC.numtextrows, VC.numtextcols - 16 );
    _outtext( "Press any key..." );
    getch();
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

#if defined( __PC98__ )
    mode = _MAXCOLORMODE;
#else
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
#endif

    if( _setvideomode( mode ) == 0 ) {
        return( 0 );
    }
    _getvideoconfig( &VC );
    if( VC.numcolors < 4 ) {
        TextColour = 1;
        TextColour2 = 1;
        BorderColour = 1;
    } else {
        TextColour = 1;
        TextColour2 = 3;
        BorderColour = 2;
    }
#if defined( __PC98__ )
    /* set up new colours */
    _remappalette( TextColour, _98BLUE );       /* light blue */
    _remappalette( TextColour2, _98BLUE );      /* light blue */
    _remappalette( BorderColour, _98BLACK );    /* black      */
#else
    if( VC.adapter >= _MCGA ) {
        /* set up new colours */
        _remappalette( TextColour, 0x3f0000 );  /* light blue */
        _remappalette( TextColour2, 0x3f0000 ); /* light blue */
        _remappalette( BorderColour, _BLACK );  /* black      */
    }
#endif
    return( 1 );
}


#if defined( __PC98__ )
  #define _MAX 15   // 4 colour bits
#else
  #define _MAX 63   // 6 colour bits
#endif


static void FadeColors( void )
/*============================

    This routine gradually fades the background text, brightening
    the foreground text and the border at the same time. */

{
    int                 i;
    long                red, blue, green;

    for( i = 1; i <= _MAX; i++ ) {
        red = i;
        green = i << 8;
        blue = (long) ( _MAX - i ) << 16;
        _remappalette( TextColour, blue );
        _remappalette( TextColour2, blue + green );
        _remappalette( BorderColour, red );
#if defined( __PC98__ )
        delay( 125 );
#endif
    }
}


void Do_Demo2( void )
/*===================

    This program draws bar and pie graphs for the
    data specified above. */

{
    _setvideomode( _MAXCOLORMODE );
    _getvideoconfig( &VC ); /* fill videoconfig structure */
    TitleColour = ( VC.numcolors - 1 ) % 16;
    Title();
    BarGraph();
    PieGraph();
}


static void Title( void )
/*=======================

    Draw main title and graph boxes. */

{
    _setcolor( TitleColour );
    _settextalign( _CENTER, _TOP );
    _setcharsize_w( 0.08, 1.0 / strlen( Main_Title ) );
    _grtext_w( 0.5, 1.0, Main_Title );
    _rectangle_w( _GBORDER, 0.00, 0.00, 0.49, 0.90 );     // left half
    _rectangle_w( _GBORDER, 0.51, 0.00, 1.00, 0.90 );     // right half
}


static void DoAxes( float xleft, float ybottom, float xlen, float ylen )
/*======================================================================

    Draw axes of bar graph. */

{
    float               xright, ytop;
    float               y, yinc;

    xright  = xleft + xlen;
    ytop = ybottom + ylen;

/*  Draw the axes */

    _moveto_w( xleft,  ytop );
    _lineto_w( xleft,  ybottom );
    _lineto_w( xright, ybottom );

/*  Draw the tick marks on the y-axis */

    yinc = ylen / 10;
    for( y = ybottom; y < ytop; y += yinc ) {
        _moveto_w( xleft, y );
        _lineto_w( xleft - 0.01, y );
    }

/*  Draw the x-axis and y-axis titles */

    _settextalign( _CENTER, _HALF );
    _settextorient( 0, 1 );
    _setcharsize_w( 0.06, ylen / strlen( Y_Axis_Title ) *
                        ( (float) VC.numypixels / VC.numxpixels ) );
    _grtext_w( xleft - 0.05, ybottom + ylen / 2, Y_Axis_Title );
    _setcharsize_w( 0.06, xlen / strlen( X_Axis_Title ) );
    _settextorient( 1, 0 );
    _grtext_w( xleft + xlen / 2, ybottom - 0.05, X_Axis_Title );
}


static void DoBars( float xleft, float ybottom, float xlen, float ylen )
/*======================================================================

    Draw bars of graph. */

{
    int                 i;
    float               x1, y1;
    float               x2, y2;
    float               bar_width;

    bar_width = ( 2 * xlen ) / ( 3 * NUMSECT + 1 );
    y1 = ybottom + 1.0 / VC.numypixels;
    for( i = 0; i < NUMSECT; ++i ) {
        x1 = xleft + ( 3 * i + 1 ) * bar_width / 2;
        x2 = x1 + bar_width;
        y2 = y1 + ylen * Values[ i ] / 100;
        _setcolor( i % ( VC.numcolors - 1 ) + 1 );
        _setfillmask( Masks[ i % 8 ] );
        _rectangle_w( _GFILLINTERIOR, x1, y1, x2, y2 );
        _rectangle_w( _GBORDER, x1, y1, x2, y2 );
    }
}


static void BarGraph( void )
/*==========================

    Draw bar graph on left side of the screen. */

{
    DoAxes( 0.10, 0.15, 0.35, 0.7 );
    DoBars( 0.10, 0.15, 0.35, 0.7 );
}


static void PieGraph( void )
/*==========================

    Draw pie graph. */

{
    int                 i;
    float               x1, y1;
    float               x2, y2;
    float               x3, y3;
    float               x4, y4;
    float               xc, yc;
    float               xradius, yradius;
    float               theta;
    long                total;

/*  Calculate data for pie graph. */

    total = 0;
    for( i = 0; i < NUMSECT; ++i ) {
        total += Values[ i ];
    }

/*  Calculate centre and radius of pie */

    xc = 0.75;
    yc = 0.45;
    xradius = 0.20;
    yradius = 0.20 * 4 / 3;

/*  Calculate bounding rectangle */

    x1 = xc - xradius;
    y1 = yc - yradius;
    x2 = xc + xradius;
    y2 = yc + yradius;

/*  Draw the slices */

    x3 = xc + xradius;
    y3 = yc;
    theta = 0.0;
    for( i = 0; i < NUMSECT; ++i ) {
        theta += Values[ i ] * 2 * PI / total;
        x4 = xc + xradius * cos( theta );
        y4 = yc + yradius * sin( theta );
        _setcolor( i % ( VC.numcolors - 1 ) + 1 );
        _setfillmask( Masks[ i % 8 ] );
        _pie_w( _GFILLINTERIOR, x1, y1, x2, y2, x3, y3, x4, y4 );
        _pie_w( _GBORDER, x1, y1, x2, y2, x3, y3, x4, y4 );
        x3 = x4;
        y3 = y4;
    }
}
