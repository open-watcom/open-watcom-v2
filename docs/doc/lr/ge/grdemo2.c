#include <graph.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>


#define PI          3.141592654


struct videoconfig  VC;
char                Main_Title[] = "Business Applications";
char                Y_Axis_Title[] = "Net Gain (x $1000)";
char                X_Axis_Title[] = "Fiscal Year";
int                 TitleColour;
int                 BorderColour;
int                 AxisColour;
unsigned char       Masks[ 8 ][ 8 ] = {
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
int                 Values[ NUMSECT ] = { /* Scaled with max of 100 */
    20, 30, 40, 35, 50, 60, 75, 70, 80, 90
};


int main( void )
/*==============

    This program draws bar and pie graphs for the
    data specified above. */

{
    if( !_setvideomode( _MAXCOLORMODE ) ) {
	puts( "No graphics adapter present" );
	return( 1 );
    }
    _getvideoconfig( &VC ); /* fill videoconfig structure */
    NewColours();

    Title();
    BarGraph();
    PieGraph();

    _settextposition( VC.numtextrows, VC.numtextcols - 16 );
    _outtext( "Press any key..." );
    getch();
    _setvideomode( _DEFAULTMODE );
    return( 0 );
}


static long         NewColourSet[] = {
    _BLACK, _LIGHTCYAN, _LIGHTMAGENTA, _BRIGHTWHITE,
    _GREEN, _LIGHTBLUE, _GRAY, _LIGHTRED,
    _CYAN, _YELLOW, _RED, _LIGHTGREEN,
    _BROWN, _MAGENTA, _BLUE, _WHITE
};


static void NewColours( void )
/*============================

    Select a new colour set. */

{
    int             i;

    if( VC.adapter >= _MCGA ) {
	for( i = 0; i < 16; ++i ) {
	    _remappalette( i, NewColourSet[ i ] );
	}
    }
    if( VC.numcolors == 2 ) {
	AxisColour = 1;
	TitleColour = 1;
	BorderColour = 1;
    } else {
	AxisColour = 1;
	TitleColour = 2;
	BorderColour = 3;
    }
}


static void Title( void )
/*=======================

    Draw main title and graph boxes. */

{
    _setcolor( BorderColour );
    _settextalign( _CENTER, _TOP );
    _setcharsize_w( 0.08, 1.0 / strlen( Main_Title ) );
    _grtext_w( 0.5, 1.0, Main_Title );
    _rectangle_w( _GBORDER, 0.00, 0.00, 0.49, 0.90 );   /* left half */
    _rectangle_w( _GBORDER, 0.51, 0.00, 1.00, 0.90 );   /* right half */
}


static void DoAxes( float xleft, float ybottom,
		    float xlen, float ylen )
/*==========================================

    Draw axes of bar graph. */

{
    float               xright, ytop;
    float               y, yinc;

    xright  = xleft + xlen;
    ytop = ybottom + ylen;

/*  Draw the axes */

    _setcolor( AxisColour );
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

    _setcolor( TitleColour );
    _settextalign( _CENTER, _HALF );
    _settextorient( 0, 1 );
    _setcharsize_w( 0.06, ylen / strlen( Y_Axis_Title ) *
			( (float) VC.numypixels / VC.numxpixels ) );
    _grtext_w( xleft - 0.05, ybottom + ylen / 2, Y_Axis_Title );
    _setcharsize_w( 0.06, xlen / strlen( X_Axis_Title ) );
    _settextorient( 1, 0 );
    _grtext_w( xleft + xlen / 2, ybottom - 0.05, X_Axis_Title );
}


static void DoBars( float xleft, float ybottom,
		    float xlen, float ylen )
/*==========================================

    Draw bars of graph. */

{
    int                 i;
    float               x1, y1;
    float               x2, y2;
    float               bar_width;

    bar_width = ( 2 * xlen ) / ( 3 * NUMSECT + 1 );
    y1 = ybottom;
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
