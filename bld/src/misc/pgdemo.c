#include <graph.h>
#include <pgchart.h>
#include <string.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>


#if defined( __386__ )
    #define _FAR
#else
    #define _FAR        __far
#endif


#define NUM_SERIES      2
#define NUM_VALUES      4


char _FAR               *categories[ NUM_VALUES ] = {
                            "Jan", "Feb", "Mar", "Apr" };
char _FAR               *labels[ NUM_SERIES ] = {
                            "Apples", "Oranges" };
float                   values[ NUM_SERIES ][ NUM_VALUES ] = {
                            20, 45, 30, 25, 30, 25, 40, 15 };
float                   x[ NUM_SERIES ][ NUM_VALUES ] = {
                            5, 15, 30, 40, 10, 20, 30, 45 };
float                   y[ NUM_SERIES ][ NUM_VALUES ] = {
                            10, 15, 30, 45, 40, 30, 15, 5 };
short                   explode[ NUM_VALUES ] = {
                            1, 0, 0, 0 };
struct videoconfig      vconfig;
int                     xmax, ymax, xmid, ymid;


int main( int argc, char *argv[] )
//================================

//  Initialize graphics library and presentation graphics
//  system and then display several sample charts.

{
    int                 mode;

    if( argc == 2 ) {
        mode = atoi( argv[ 1 ] );
    } else {
        mode = _MAXRESMODE;
    }
    if( _setvideomode( mode ) == 0 ) {
        puts( "Cannot initialize video mode" );
        return( 1 );
    }
    _getvideoconfig( &vconfig );
    xmax = vconfig.numxpixels;
    ymax = vconfig.numypixels;
    xmid = xmax / 2;
    ymid = ymax / 2;
    _pg_initchart();
    simple_chart();
    press_key();
    _clearscreen( _GCLEARSCREEN );
    more_charts();
    press_key();
    _clearscreen( _GCLEARSCREEN );
    scatter_charts();
    press_key();
    _setvideomode( _DEFAULTMODE );
    return( 0 );
}


void press_key()
//==============

//  Display a message and wait for the user to press a key

{
    int                 len;
    char                *msg;
    struct _fontinfo    info;

    _getfontinfo( &info );
    msg = " Press any key ";
    len = _getgtextextent( msg );
    _setcolor( 4 );
    _rectangle( _GFILLINTERIOR, xmax - len, 0,
                                xmax - 1, info.pixheight + 2 );
    _setcolor( 15 );
    _moveto( xmax - len, 1 );
    _outgtext( msg );
    getch();
}


void simple_chart()
//=================

//  Display a simple column chart

{
    chartenv            env;

    _pg_defaultchart( &env, _PG_COLUMNCHART, _PG_PLAINBARS );
    strcpy( env.maintitle.title, "Column Chart" );
    _pg_chart( &env, categories, values, NUM_VALUES );
}


void more_charts()
//================

//  Display several sample charts on different areas of the screen

{
    chartenv            env;

    // use only left half of screen
    _pg_defaultchart( &env, _PG_COLUMNCHART, _PG_PLAINBARS );
    strcpy( env.maintitle.title, "Column Chart" );
    env.chartwindow.x1 = 0;
    env.chartwindow.y1 = 0;
    env.chartwindow.x2 = xmid - 1;
    env.chartwindow.y2 = ymax - 1;
    _pg_chart( &env, categories, values, NUM_VALUES );

    // use top right corner
    _pg_defaultchart( &env, _PG_BARCHART, _PG_PLAINBARS );
    strcpy( env.maintitle.title, "Bar Chart" );
    env.chartwindow.x1 = xmid;
    env.chartwindow.y1 = 0;
    env.chartwindow.x2 = xmax - 1;
    env.chartwindow.y2 = ymid - 1;
    env.legend.place = _PG_BOTTOM;
    _pg_chartms( &env, categories, values, NUM_SERIES,
                 NUM_VALUES, NUM_VALUES, labels );

    // use lower right corner
    _pg_defaultchart( &env, _PG_PIECHART, _PG_NOPERCENT );
    strcpy( env.maintitle.title, "Pie Chart" );
    env.chartwindow.x1 = xmid;
    env.chartwindow.y1 = ymid;
    env.chartwindow.x2 = xmax - 1;
    env.chartwindow.y2 = ymax - 1;
    _pg_chartpie( &env, categories,
                  values, explode, NUM_VALUES );
}


void scatter_charts()
//===================

//  Display two scatter charts beside each other;
//  one using default options, the other customized

{
    chartenv            env;

    // use left half of screen
    _pg_defaultchart( &env, _PG_SCATTERCHART, _PG_POINTANDLINE );
    strcpy( env.maintitle.title, "Scatter Chart" );
    env.chartwindow.x1 = 0;
    env.chartwindow.y1 = 0;
    env.chartwindow.x2 = xmid - 1;
    env.chartwindow.y2 = ymax - 1;
    _pg_chartscatterms( &env, x, y, NUM_SERIES,
                        NUM_VALUES, NUM_VALUES, labels );

    // use right half of screen
    _pg_defaultchart( &env, _PG_SCATTERCHART, _PG_POINTANDLINE );
    strcpy( env.maintitle.title, "Scatter Chart" );
    strcpy( env.subtitle.title, "(1991 Production)" );
    env.chartwindow.x1 = xmid;
    env.chartwindow.y1 = 0;
    env.chartwindow.x2 = xmax - 1;
    env.chartwindow.y2 = ymax - 1;
    env.chartwindow.background = 2;
    env.datawindow.background = 8;
    env.legend.legendwindow.background = 8;
    env.legend.place = _PG_BOTTOM;
    env.maintitle.justify = _PG_LEFT;
    _pg_analyzescatterms( &env, x, y, NUM_SERIES,
                        NUM_VALUES, NUM_VALUES, labels );
    env.yaxis.autoscale = 0;
    env.yaxis.scalemin = 0.0;
    env.yaxis.scalemax = 60.0;
    env.yaxis.ticinterval = 20.0;
    env.yaxis.grid = 1;
    env.yaxis.gridstyle = 2;
    _pg_chartscatterms( &env, x, y, NUM_SERIES,
                        NUM_VALUES, NUM_VALUES, labels );
}
