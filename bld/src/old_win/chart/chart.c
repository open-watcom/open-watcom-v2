#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include "chart.h"

#define MAX_BARS        20              // maximum # bars in a chart

typedef struct {
    RECT        bar_rects[MAX_BARS];    // screen coords of each bar in chart
    BOOL        (FAR PASCAL *fetch_data)( int, float FAR* ); // data fetch rtn
    COLORREF    bar_colors[MAX_BARS];   // colors of each bar in the chart
    int         num_bars;               // # of bars in this chart
} chart_def;

#define NUM_COLORS 16
static PALETTEENTRY MyPalette[NUM_COLORS]={
    { 255,      255,    255,    0       },
    { 0,        0,      0,      0       },
    { 0,        0,      128,    0       },
    { 0,        0,      255,    0       },
    { 128,      0,      0,      0       },
    { 255,      0,      0,      0       },
    { 128,      0,      128,    0       },
    { 255,      0,      255,    0       },
    { 0,        128,    0,      0       },
    { 0,        255,    0,      0       },
    { 0,        128,    128,    0       },
    { 0,        255,    255,    0       },
    { 128,      128,    128,    0       },
    { 192,      192,    192,    0       },
    { 128,      128,    0,      0       },
    { 255,      255,    0,      0       }
};

HPALETTE ChartPalette;

#define BAR_SPACE .15           // % of bar spacing constant

static int Num_instances=0;

void WINEXP ChartInitUse()
/************************/
/* must be called by any application at the start of that application */

{
    ++Num_instances;
}

void WINEXP ChartCloseUse()
/*************************/
/* must be called by any application using the charting DLL which is
   terminating. Normally, the 'LibMain' and 'WEP' can be used to
   this ends, but WINDOWS calls the WEP function AFTER terminating
   the DLL. So, any memory freeing would fail in the WEP function. */

{
    --Num_instances;
    if( Num_instances == 0 ) {
        DeleteObject( ChartPalette );
    }
}

int WINEXP LibMain( HINSTANCE inst, WORD data, WORD heap_size, LPSTR cmd_line )
/**************************************************************************/
/* Entry routine to the CHART DLL. Called by windows when the first application
   to use the DLL is started */

{
    LOGPALETTE far      *pal;
    HGLOBAL             pal_hld;

    inst = inst;
    data = data;
    heap_size = heap_size;
    cmd_line = cmd_line;

    pal_hld = GlobalAlloc( GMEM_MOVEABLE, sizeof( LOGPALETTE ) +
                               ( NUM_COLORS - 1 ) * sizeof( PALETTEENTRY ) );
    if( pal_hld != NULL ) {
        pal = (void far *)GlobalLock( pal_hld );
        memcpy( pal->palPalEntry, MyPalette,
                                         sizeof( PALETTEENTRY ) * NUM_COLORS );

        pal->palVersion = 0x300;
        pal->palNumEntries = NUM_COLORS;

        ChartPalette = CreatePalette( pal );

        GlobalUnlock( pal_hld );

        GlobalFree( pal_hld );

        return( TRUE );
    }

    return( FALSE );
}

int FAR PASCAL WEP( int sys_exit )
/********************************/
/* This function is called when the last application to use the CHART DLL
   is closed */

{
    sys_exit = sys_exit;

    return( 1 );
}

chart_hld WINEXP ChartAdd( FARPROC fetch_data )
/*********************************************/
/* Create a chart structure and return global handle. ChartDraw must still
   be called to get the chart displayed */

{
    chart_def far               *chart;
    HANDLE                      hld;
    int                         i;
    PALETTEENTRY                *pal;


    hld = GlobalAlloc( GMEM_MOVEABLE, sizeof( chart_def ) );
    if( hld != NULL ) {
        chart = (chart_def far *) GlobalLock( hld );
        if( chart != NULL ) {
            chart->fetch_data = (BOOL (FAR PASCAL *)(int, float *))fetch_data;
            for( i = 0; i < MAX_BARS; ++i ) {
                pal = &MyPalette[(i + 1) % NUM_COLORS];
                chart->bar_colors[i] = RGB( pal->peRed, pal->peGreen,
                                                        pal->peBlue );
            }

            GlobalUnlock( hld );

            return( hld );
        }
    }

    return( NULL );
}

BOOL WINEXP ChartDelete( chart_hld hld )
/**************************************/
/* delete a chart: get rid of associated memory */

{
    return( NULL == GlobalFree( hld ) );
}

COLORREF WINEXP ChartGetBarColor( int bar, chart_hld hld )
/********************************************************/
/* return the current colour of a bar */

{
    chart_def far               *chart;
    COLORREF                    color;

    chart = (void far *)GlobalLock( hld );

    if( chart != NULL ) {
        color = chart->bar_colors[bar];
        GlobalUnlock( hld );

        return( color );
    }

    return( NULL );
}


BOOL WINEXP ChartSetBarColor( int bar, COLORREF color, chart_hld hld )
/********************************************************************/
/* Call this function to change the color of a bar */

{
    chart_def far               *chart;

    chart = (void far *)GlobalLock( hld );

    if( chart != NULL ) {
        chart->bar_colors[bar] = color;

        GlobalUnlock( hld );

        return( TRUE );
    }

    return( FALSE );
}

int WINEXP ChartBarClick( int x, int y, chart_hld hld )
/*****************************************************/
/* Find out which bar a position is on. The last drawing of the chart
   is used for the bar positions. -1 is returned if no bar was
   selected */
{
    chart_def far               *chart;
    int                         i;
    POINT                       pt;

    chart = (void far *)GlobalLock( hld );

    if( chart != NULL ) {
        pt.x = x;
        pt.y = y;
        for( i = chart->num_bars - 1; i >= 0; --i ) {
            if( PtInRect( &chart->bar_rects[i], pt ) ) {
                break;
            }
        }

        GlobalUnlock( hld );

        return( i );
    }

    return( -1 );
}

static float draw_scales( HDC win_dc, RECT *area, float max_value )
/*****************************************************************/
/* draw the scales on the chart */

{
    int                 step;
    int                 value;
    int                 y_pos;
    int                 x_pos;
    int                 win_step;
    char                buf[20];
    int                 num_incs;

    step = max_value / 8;
    if( step < 1 ) {
        step = 1;
    }

    for( num_incs = 1, value = 0; value < max_value; value += step, ++num_incs);

    win_step = ( area->bottom - area->top ) / num_incs;

    SelectObject( win_dc, GetStockObject( BLACK_PEN ) );
    SelectObject( win_dc, GetStockObject( SYSTEM_FONT ) );

    /* draw the scale lines */
    MoveTo( win_dc, area->left, area->bottom );
    LineTo( win_dc, area->right, area->bottom );
    MoveTo( win_dc, area->left, area->bottom );
    LineTo( win_dc, area->left, area->top );


    /* draw the Y scale tick marks and labels */
    x_pos = area->left - 3;
    for( value = 0, y_pos = area->bottom; num_incs > 0;
                            --num_incs, value += step, y_pos -= win_step ) {
        MoveTo( win_dc, x_pos, y_pos );
        LineTo( win_dc, x_pos + 6, y_pos );
        itoa( value, buf, 10 );
        SetTextAlign( win_dc, TA_RIGHT | TA_BASELINE );
        TextOut( win_dc, x_pos - 2, y_pos, buf, strlen( buf ) );
    }

    return( value );
}

static void draw_bars( chart_def far *chart, HDC win_dc, RECT *area,
                                                        float max_scale )
/***********************************************************************/
/* draw the bars on the chart */

{
    int                 bar_width;
    int                 space;
    int                 pos;
    int                 num_bars;
    RECT                rect;
    int                 height;
    int                 which;
    float               value;
    LOGBRUSH            brush;
    HBRUSH              brush_hld;
    HANDLE              old_object;

    num_bars = chart->num_bars;
    bar_width =  ( area->right - area->left ) / ( num_bars +
                                    ( num_bars + 1 ) * BAR_SPACE );
    space = BAR_SPACE * bar_width;

    height = area->bottom - area->top;

    SelectObject( win_dc, GetStockObject( NULL_PEN ) );

    pos = space + area->left;

    for( which = 0; num_bars > 0; --num_bars, ++which ) {
        rect.left = pos;
        rect.right = pos + bar_width;
        rect.bottom = area->bottom;
        (chart->fetch_data)( which, &value );
        rect.top = area->bottom - height * ( (float)value / max_scale );

        brush.lbStyle = BS_SOLID;
        brush.lbColor = chart->bar_colors[which];

        brush_hld = CreateBrushIndirect( &brush );

        old_object = SelectObject( win_dc, brush_hld );

        Rectangle( win_dc, rect.left, rect.top, rect.right, rect.bottom );

        SelectObject( win_dc, old_object );

        DeleteObject( brush_hld );

        chart->bar_rects[which] = rect;

        pos += bar_width + space;
    }
}

BOOL WINEXP ChartDraw( chart_hld hld, HWND win_hld, HDC win_dc )
/**************************************************************/
/* Draw a chart in the client area of the window */

{
    chart_def far               *chart;
    int                         num_bars;
    float                       value;
    float                       max_value;
    HPALETTE                    old_palette;
    RECT                        area;
    int                         width;
    int                         height;

    chart = (void far *)GlobalLock( hld );

    if( chart != NULL ) {
        /* count the number of bars first */
        for( max_value = 0, num_bars = 0;; ++num_bars) {
            if( !(chart->fetch_data)( num_bars, &value ) ) {
                break;
            }
            if( value > max_value ) {
                max_value = value;
            }
        }
        if( num_bars > MAX_BARS ) {
            num_bars = MAX_BARS;
        }
        chart->num_bars = num_bars;

        old_palette = SelectPalette( win_dc, ChartPalette, FALSE );
        RealizePalette( win_dc );
        SetBkColor( win_dc, RGB( 255, 255, 255 ) );
        SetBkMode( win_dc, OPAQUE );

        GetClientRect( win_hld, &area );

        width = area.right - area.left;
        height = area.bottom - area.top;

        area.left += width * .1;
        area.right -= width * .1;
        area.top += height * .1;
        area.bottom -= height * .1;

        draw_bars( chart, win_dc, &area, draw_scales( win_dc,
                                                        &area, max_value ) );

        SelectPalette( win_dc, old_palette, FALSE );

        GlobalUnlock( hld );

        return( TRUE );
    }

    return( FALSE );
}
