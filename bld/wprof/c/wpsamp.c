/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Display profile samples in the GUI.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include "walloca.h"

#include "common.h"
#include "aui.h"
#include "wpaui.h"
#include "dip.h"
#include "sampinfo.h"
#include "wpsrcfil.h"
#include "wpasmfil.h"
#include "msg.h"
#include "memutil.h"


extern image_info *SImageGetImage(a_window *wnd,int row);
extern mod_info *SModGetModule(a_window *wnd,int row);
extern file_info *SFileGetFile(a_window *wnd,int row);
extern rtn_info *SRtnGetRoutine(a_window *wnd,int row);
extern int WPGetRow(sio_data *curr_sio);
extern wp_srcfile *WPSourceOpen(sio_data *curr_sio,bint quiet);
extern void WPSourceClose(wp_srcfile *wpsrc_file);
extern char *WPSourceGetLine(a_window *wnd,int line);
extern gui_ord WPGetClientHeight(a_window *wnd);
extern gui_ord WPGetClientWidth(a_window *wnd);
extern void WPAdjustRowHeight(a_window *wnd,bint initial_set);
extern void WPSetRowHeight(a_window *wnd);
extern gui_ord WPPixelTruncWidth(gui_ord width);
extern gui_ord WPPixelWidth(a_window *wnd);
extern void ClearSample(sio_data *curr_sio);
extern bint GetCurrentGather(sio_data *curr_sio);
extern void FlipCurrentGather(sio_data *curr_sio);
extern void GatherCurrent(sio_data *curr_sio);
extern int GetCurrentSort(sio_data *curr_sio);
extern void SetCurrentSort(sio_data *curr_sio,int sort_type);
extern void SortCurrent(sio_data *curr_sio);
extern void WPDipSetProc(process_info *dip_proc);
extern wp_asmfile *WPAsmOpen(sio_data *curr_sio,int src_row,bint quiet);
extern void WPAsmClose(wp_asmfile *wpasm_file);
extern char *WPAsmGetLine(a_window *wnd,int line);
extern int WPAsmFindSrcLine(sio_data *curr_sio,int line);
extern wp_asmline *WPGetAsmLoc(wp_asmfile *wpasm_file,int row,int *group_loc,int *row_loc);
extern bint GetCurrentAbsBar(sio_data *curr_sio);
extern bint GetCurrentRelBar(sio_data *curr_sio);
extern bint GetCurrentMaxBar(sio_data *curr_sio);
extern void FlipCurrentAbsBar(sio_data *curr_sio);
extern void FlipCurrentRelBar(sio_data *curr_sio);
extern void FlipCurrentMaxBar(sio_data *curr_sio);
extern void ErrorMsg(char *msg,... );
extern int SampleNumRows( a_window * wnd );
extern void SetCurrentMAD( mad_handle );


extern sio_data *       SIOData;
extern sio_data *       CurrSIOData;

STATIC void *           sampleCreateWin( void );
STATIC void             sampleOpenMainImage( void );
STATIC bint             sampleProcTopStatus( a_window *, int, int, wnd_line_piece * );
STATIC bint             sampleProcBotStatus( a_window *, int, int, wnd_line_piece * );
STATIC bint             sampleProcStatus( a_window *, int, int, wnd_line_piece * );
STATIC bint             sampleProcOverview( a_window *, int, int, wnd_line_piece * );
STATIC bool             sampleEventProc( a_window *, gui_event, void * );
STATIC bint             sampleSetLine( a_window *, int, int, wnd_line_piece * );
STATIC bool             sampleGetLine( a_window *, wnd_row, int, wnd_line_piece * );
STATIC int              simageDetailLine( a_window *, int, bint );
STATIC int              smodDetailLine( a_window *, int, bint );
STATIC int              sfileDetailLine( a_window *, int, bint );
STATIC int              srtnDetailLine( a_window *, int, bint );
STATIC int              ssrcDetailLine( a_window *, int, bint );
STATIC int              sasmDetailLine( a_window *, int, bint );
STATIC int              srtnOpenDetail( sio_data *, bint );
STATIC void             sampleRefresh( a_window * );
STATIC void             sampleMenuItem( a_window *, unsigned, int, int );
STATIC void             sampFixDirtyCurr( a_window * );
STATIC bint             simageGetLine( a_window *, int );
STATIC bint             smodGetLine( a_window *, int );
STATIC bint             sfileGetLine( a_window *, int );
STATIC bint             srtnGetLine( a_window *, int );
STATIC bint             ssrcGetLine( a_window *, int );
STATIC bint             sasmGetLine( a_window *, int );
STATIC void             gatherSort( sio_data * );
STATIC void             setDisplay( a_window *, sio_data *, bint );

typedef bint (SAMPLEGETRTNS)( a_window *wnd, int row );
typedef int (SAMPLEDETAILRTNS)( a_window *wnd, int row, bint multi_level );

enum {
    PIECE_MOUSE_CATCHER,
    PIECE_BAR,
    PIECE_SEPARATOR,
    PIECE_HOOK,
    PIECE_DETAIL_NAME = PIECE_HOOK,
    PIECE_NAME_TITLE,
    PIECE_NAME_TEXT,
    PIECE_LAST
};

enum {
    PIECE_REL_HEADER=PIECE_MOUSE_CATCHER+1,
    PIECE_REL_PERCENT,
    PIECE_ABS_HEADER,
    PIECE_ABS_PERCENT,
    PIECE_PERCENT_SEPARATOR,
    PIECE_DETAIL_TITLE,
    PIECE_HEADER_LAST
};

enum {
    PIECE_DRAW_LINE=PIECE_MOUSE_CATCHER+1,
};

#define STATUS_ROW          8
#define BAR_TAIL_POINT      2500
#define SEPARATOR_POINT     2500

static char * overviewHeaders[] = {
    LIT( Sample_Header ),
    LIT( Image_Header ),
    LIT( Module_Header ),
    LIT( File_Header ),
    LIT( Routine_Header ),
    LIT( Source_Header ),
    LIT( Empty_Str ),
};

static char * statusHeaders[] = {
    LIT( Image_Names ),
    LIT( Module_Names ),
    LIT( File_Names ),
    LIT( Routine_Names ),
    LIT( Source_Line ),
    LIT( Assembler_Instructions ),
};

static SAMPLEGETRTNS * sampleGetRtns[] = {
    &simageGetLine,
    &smodGetLine,
    &sfileGetLine,
    &srtnGetLine,
    &ssrcGetLine,
    &sasmGetLine,
};

static SAMPLEDETAILRTNS * overviewDetailRtns[] = {
    &simageDetailLine,
    &smodDetailLine,
    &sfileDetailLine,
    &srtnDetailLine,
    &ssrcDetailLine,
    &sasmDetailLine,
};

static gui_menu_struct graphBarMenu[] = {
    { "&Stretch", MENU_SAMP_BAR_MAX_TIME, GUI_ENABLED,
                            "Stretch the largest value to the edge of the window" },
    { "&Absolute Bars", MENU_SAMP_ABS, GUI_ENABLED,
                            "Show Absolute Graph Bars" },
    { "&Relative Bars", MENU_SAMP_REL, GUI_ENABLED,
                            "Show Relative Graph Bars" }
};

static gui_menu_struct sortMenu[] = {
    { "&Sample Count", MENU_SORT_COUNT, GUI_ENABLED,
                            "Sort by the number of samples" },
    { "&Name", MENU_SORT_NAME, GUI_ENABLED,
                            "Sort by the name" },
};

static gui_menu_struct sampleMenu[] = {
    { "Zoom &In\tF3", MENU_SAMP_ZOOM_IN, GUI_ENABLED,
                            "Zoom in form more detail" },
    { "Back &Out\tF4", MENU_SAMP_BACK_OUT, GUI_ENABLED,
                            "Back out from the current display" },
    { "&Gather Small Values", MENU_SAMP_GATHER, GUI_ENABLED,
                            "Gather small samples together" },
    { "&Bar Graph", MENU_SAMP_BAR, GUI_ENABLED,
                             "Make adjustments to the bar graph",
                             ArraySize( graphBarMenu ), graphBarMenu },
    { "&Sort", MENU_SAMP_SORT, GUI_ENABLED,
                             "Sort the values",
                             ArraySize( sortMenu ), sortMenu },
};

static char *       nameBuff = NULL;
static wnd_bar_info barData;
static char *       dispName;
static clicks_t     dispCount;
static clicks_t     localTicks;
static clicks_t     maxTime;
static gui_ord      barExtent;
static gui_ord      bar2Extent;
static gui_ord      indentPiece = 0;
static gui_ord      relPctStatusIndent;
static gui_ord      absPctStatusIndent;
static bint         sampNewRow;
static bint         barMaxTime;
static bint         dispHighLight;
static bint         absGraphBar;
static bint         relGraphBar;
static char         relData[30];
static char         absData[30];
static char         lineData[96];


wnd_info WPSampleInfo = {
    sampleEventProc,
    sampleRefresh,
    sampleGetLine,
    sampleMenuItem,
    NoScroll,
    NoBegPaint,
    NoEndPaint,
    WndFirstMenuItem,
    SampleNumRows,
    NoNextRow,
    NoNotify,
    NULL,
    0,
    DefPopUp( sampleMenu )
};



extern void WPSampleOpen( void )
/******************************/
{
    if( CurrSIOData->sample_window == NULL ) {
        CurrSIOData->sample_window = sampleCreateWin();
        if( CurrSIOData->sample_window != NULL ) {
            sampleOpenMainImage();
            WndSetThumb( CurrSIOData->sample_window );
        }
    }
    if( CurrSIOData->sample_window != NULL ) {
        WndToFront( CurrSIOData->sample_window );
        WndShowWindow( CurrSIOData->sample_window );
        WPSetRowHeight( CurrSIOData->sample_window );
    }
}



STATIC void *sampleCreateWin( void )
/**********************************/
{
    a_window            *wnd;
    wnd_create_struct   info;
    char                title[512];

    WndInitCreateStruct( &info );
    sprintf( title, LIT( Sample_Data ), CurrSIOData->samp_file_name );
    info.title = title;
    info.info = &WPSampleInfo;
    info.extra = CurrSIOData;
//    info.colour = GetWndColours( class );
    info.title_size = STATUS_ROW + 1;
    info.style |= GUI_INIT_INVISIBLE;
    wnd = WndCreateWithStruct( &info );
    if( wnd == NULL )
        return( wnd );
//    WndSetFontInfo( wnd, GetWndFont( wnd ) );
//-//    WndSetSysFont( wnd, P_TRUE );
    WndClrSwitches( wnd, WSW_MUST_CLICK_ON_PIECE|WSW_ONLY_MODIFY_TABSTOP );
    WndSetSwitches( wnd, WSW_RBUTTON_CHANGE_CURR );
    return( wnd );
}



STATIC void sampleOpenMainImage( void )
/*************************************/
{
    image_info      *curr_image;
    int             count;

    gatherSort( CurrSIOData );
    count = CurrSIOData->image_count;
    while( count-- > 0 ) {
        curr_image = CurrSIOData->images[count];
        if( curr_image->main_load ) {
            if( curr_image->dip_handle != NO_MOD && curr_image->mod_count > 2 ) {
                CurrSIOData->curr_image = curr_image;
                CurrSIOData->level_open = LEVEL_IMAGE;
                gatherSort( CurrSIOData );
            }
            break;
        }
    }
    WndNewCurrent( CurrSIOData->sample_window, 0, PIECE_DETAIL_NAME );
    WndDirty( CurrSIOData->sample_window );
}



STATIC bool sampleEventProc( a_window *wnd, gui_event gui_ev, void *parm )
/************************************************************************/
{
    sio_data        *curr_sio;

    parm=parm;
    switch( gui_ev ) {
    case GUI_INIT_WINDOW:
        return( TRUE );
    case GUI_FONT_CHANGED:
        WPSetRowHeight( wnd );
        return( TRUE );
    case GUI_RESIZE:
        WPAdjustRowHeight( wnd, P_FALSE );
        return( TRUE );
    case GUI_NOW_ACTIVE:
        curr_sio = WndExtra( wnd );
        curr_sio->curr_proc_row = -WND_MAX_ROW;
        curr_sio->curr_display_row = -WND_MAX_ROW;
        WPDipSetProc( curr_sio->dip_process );
        SetCurrentMAD( curr_sio->config.mad );
        CurrSIOData = curr_sio;
        return( TRUE );
    case WND_CURRENT_CHANGED:
        sampFixDirtyCurr( wnd );
        return( P_TRUE );
    case GUI_DESTROY:
        curr_sio = WndExtra( wnd );
        if( curr_sio != NULL ) {
            ClearSample( curr_sio );
        }
        return( P_FALSE );
    }
    return( P_FALSE );
}



STATIC bool sampleGetLine( a_window *wnd, wnd_row row, int piece,
                                        wnd_line_piece *line )
/*************************************************************/
{
    sio_data        *curr_sio;

    if( row <= -4  ) {
        return( sampleProcOverview( wnd, row, piece, line ) );
    }
    if( row == -3 ) {
        return( sampleProcTopStatus( wnd, row, piece, line ) );
    }
    if( row == -2  ) {
        return( sampleProcStatus( wnd, row, piece, line ) );
    }
    if( row == -1 ) {
        return( sampleProcBotStatus( wnd, row, piece, line ) );
    }
    curr_sio = WndExtra( wnd );
    if( !sampleGetRtns[curr_sio->level_open]( wnd, row ) ) return( P_FALSE );
    return( sampleSetLine( wnd, row, piece, line ) );
}



STATIC bint sampleProcTopStatus( a_window *wnd, int row, int piece,
                                             wnd_line_piece *line )
/*****************************************************************/
{
    gui_point           start;
    gui_point           end;
    gui_ord             vertical_x;
    gui_ord             max_x;
    gui_ord             max_y;
    gui_ord             client_width;
    gui_ord             cross_y;

    row=row;
    piece=piece;
    line=line;
    if( piece > PIECE_DRAW_LINE ) return( P_FALSE );
    if( piece == PIECE_MOUSE_CATCHER ) {
        line->indent = 0;
        line->tabstop = P_FALSE;
        line->attr = WPA_PLAIN;
        line->text = LIT( Empty_Str );
        line->extent = WndWidth( wnd );
        return( P_TRUE );
    }
    max_y = WndMaxCharY( wnd );
    max_x = WndAvgCharX( wnd );
    vertical_x = SEPARATOR_POINT + max_x / 2;
    client_width = WPGetClientWidth( wnd );
    cross_y = max_y * (STATUS_ROW-1) - max_y/4;
    start.x = 0;
    end.x = client_width;
    start.y = cross_y;
    end.y = start.y;
    GUIDrawLine( WndGui( wnd ), &start, &end, GUI_PEN_SOLID, 0, WPA_PLAIN );
    start.x = vertical_x;
    end.x = start.x;
    start.y = 0;
    end.y = cross_y;
    GUIDrawLine( WndGui( wnd ), &start, &end, GUI_PEN_SOLID, 0, WPA_PLAIN );
    return( P_TRUE );
}



STATIC bint sampleProcBotStatus( a_window *wnd, int row, int piece,
                                               wnd_line_piece *line )
/*******************************************************************/
{
    gui_point           start;
    gui_point           end;
    gui_ord             vertical_x;
    gui_ord             max_x;
    gui_ord             max_y;
    gui_ord             client_height;
    gui_ord             client_width;
    gui_ord             cross_y;

    row=row;
    piece=piece;
    line=line;
    if( piece > PIECE_DRAW_LINE ) return( P_FALSE );
    if( piece == PIECE_MOUSE_CATCHER ) {
        line->indent = 0;
        line->tabstop = P_FALSE;
        line->attr = WPA_PLAIN;
        line->text = LIT( Empty_Str );
        line->extent = WndWidth( wnd );
        return( P_TRUE );
    }
    max_y = WndMaxCharY( wnd );
    max_x = WndAvgCharX( wnd );
    vertical_x = SEPARATOR_POINT + max_x / 2;
    client_height = WPGetClientHeight( wnd );
    client_width = WPGetClientWidth( wnd );
    cross_y = max_y * STATUS_ROW + max_y/4;
    start.x = 0;
    end.x = client_width;
    start.y = cross_y;
    end.y = start.y;
    GUIDrawLine( WndGui( wnd ), &start, &end, GUI_PEN_SOLID, 0, WPA_PLAIN );
    start.x = vertical_x;
    end.x = vertical_x;
    start.y = cross_y;
    end.y = client_height;
    GUIDrawLine( WndGui( wnd ), &start, &end, GUI_PEN_SOLID, 0, WPA_PLAIN );
    return( P_TRUE );
}



STATIC bint sampleProcStatus( a_window *wnd, int row, int piece,
                                           wnd_line_piece *line )
/***************************************************************/
{
    sio_data        *curr_sio;
    clicks_t        abs_count;
    clicks_t        rel_count;
    gui_ord         point_adjust;

    row=row;
    if( piece >= PIECE_HEADER_LAST ) {
        return( P_FALSE );
    }
    curr_sio = WndExtra( wnd );
    if( piece == PIECE_MOUSE_CATCHER ) {
        line->indent = 0;
        line->tabstop = P_FALSE;
        line->attr = WPA_PLAIN;
        line->text = LIT( Empty_Str );
        abs_count = curr_sio->abs_count;
        rel_count = curr_sio->rel_count;
        sprintf( relData, "%ld.%ld%%", rel_count/10, rel_count-((rel_count/10)*10) );
        sprintf( absData, "%ld.%ld%%", abs_count/10, abs_count-((abs_count/10)*10) );
        if( WPPixelTruncWidth( WndMaxCharX( wnd ) / 2 ) == 0 ) {
            point_adjust = WndMaxCharX( wnd ) / 2;
        } else {
            point_adjust = 0;
        }
        absPctStatusIndent = BAR_TAIL_POINT - WndExtentX( wnd, absData )
                           - point_adjust;
        indentPiece = BAR_TAIL_POINT - WndExtentX( wnd, "199.9%" )
                    - (2 * WndExtentX( wnd, " " )) - point_adjust;
        relPctStatusIndent = indentPiece - WndExtentX( wnd, relData );
        indentPiece -= WndExtentX( wnd, "199.9%" );
        if( GUIIsGUI() ) {
            indentPiece -= WndExtentX( wnd, LIT( Rel_Header ) )
                         + WndExtentX( wnd, LIT( Abs_Header ) );
            relPctStatusIndent -= WndExtentX( wnd, LIT( Abs_Header ) );
            line->extent = indentPiece;
        }
    } else if( piece == PIECE_REL_HEADER ) {
        line->indent = indentPiece;
        if( GUIIsGUI() ) {
            line->text = LIT( Rel_Header );
        } else {
            line->text = LIT( Empty_Str );
        }
        line->tabstop = P_FALSE;
        if( relGraphBar ) {
            line->attr = WPA_REL_BAR;
        } else {
            line->attr = WPA_PLAIN_INACTIVE;
        }
    } else if( piece == PIECE_REL_PERCENT ) {
        line->text = relData;
        line->indent = relPctStatusIndent;
        line->tabstop = P_FALSE;
        if( curr_sio->rel_on_screen ) {
            line->attr = WPA_REL_BAR;
        } else {
            line->attr = WPA_PLAIN_INACTIVE;
        }
    } else if( piece == PIECE_ABS_HEADER ) {
        indentPiece = BAR_TAIL_POINT - WndExtentX( wnd, "199.9%" );
        if( GUIIsGUI() ) {
            line->text = LIT( Abs_Header );
            indentPiece -= WndExtentX( wnd, LIT( Abs_Header ) );
        } else {
            line->text = LIT( Empty_Str );
        }
        line->indent = indentPiece;
        line->tabstop = P_FALSE;
        if( curr_sio->abs_on_screen ) {
            line->attr = WPA_ABS_BAR;
        } else {
            line->attr = WPA_PLAIN_INACTIVE;
        }
    } else if( piece == PIECE_ABS_PERCENT ) {
        line->text = absData;
        line->indent = absPctStatusIndent;
        line->tabstop = P_FALSE;
        if( curr_sio->abs_on_screen ) {
            line->attr = WPA_ABS_BAR;
        } else {
            line->attr = WPA_PLAIN_INACTIVE;
        }
    } else if( piece == PIECE_PERCENT_SEPARATOR ) {
        if( !GUIIsGUI() ) {
            line->vertical_line = P_TRUE;
        }
        line->indent = SEPARATOR_POINT;
        line->text = LIT( Empty_Str );
        line->attr = WPA_PLAIN;
        line->tabstop = P_FALSE;
    } else if( piece == PIECE_DETAIL_TITLE ) {
        line->indent = SEPARATOR_POINT + WndMaxCharX( wnd );
        curr_sio = WndExtra( wnd );
        if( curr_sio->level_open == LEVEL_ROUTINE ) {
            sprintf( lineData, "%s: %.5d", statusHeaders[curr_sio->level_open], curr_sio->curr_display_row+1 );
            line->text = lineData;
        } else {
            line->text = statusHeaders[curr_sio->level_open];
        }
        line->tabstop = P_FALSE;
        line->attr = WPA_PLAIN;
    }
    return( P_TRUE );
}



STATIC bint sampleProcOverview( a_window * wnd, int row, int piece,
                                             wnd_line_piece * line )
/******************************************************************/
{
    sio_data *      curr_sio;
    clicks_t        tick_count;
    clicks_t        total_ticks;
    char *          text;

    if( piece >= PIECE_LAST ) {
        return( P_FALSE );
    }
    curr_sio = WndExtra( wnd );
    row += STATUS_ROW + 1;
    text = LIT( Empty_Str );
    tick_count = 0;
    if( row <= curr_sio->level_open ) {
        if( row == 0 ) {
            text = curr_sio->samp_file_name;
            tick_count = curr_sio->total_samples;
        } else if( row == 1 ) {
            text = curr_sio->curr_image->name;
            tick_count = curr_sio->curr_image->agg_count;
        } else if( row == 2 ) {
            text = curr_sio->curr_mod->name;
            tick_count = curr_sio->curr_mod->agg_count;
        } else if( row == 3 ) {
            text = curr_sio->curr_file->name;
            tick_count = curr_sio->curr_file->agg_count;
        } else if( row == 4 ) {
            text = curr_sio->curr_rtn->name;
            tick_count = curr_sio->curr_rtn->tick_count;
        }
    }
    if( piece == PIECE_MOUSE_CATCHER ) {
        line->indent = 0;
        line->tabstop = P_FALSE;
        line->master_tabstop = P_TRUE;
        line->attr = WPA_PLAIN;
        line->text = LIT( Empty_Str );
        if( row <= curr_sio->level_open ) {
            line->extent = BAR_TAIL_POINT;
        }
    } else if( piece == PIECE_BAR ) {
        line->indent = BAR_TAIL_POINT;
        if( curr_sio->total_samples == 0 ) {
            curr_sio->total_samples = 1;
        }
        total_ticks = curr_sio->total_samples;
        line->attr = WPA_PLAIN;
        line->tabstop = P_FALSE;
        line->master_tabstop = P_TRUE;
        if( row > curr_sio->level_open ) {
            line->text = LIT( Empty_Str );
        } else {
            line->draw_bar = P_TRUE;
            barData.bar_style = GUI_BAR_SHADOW;
            barData.bar_colour = WPA_ABS_BAR;
            barData.bar_group = P_FALSE;
            barData.bar_selected = P_FALSE;
            tick_count *= BAR_TAIL_POINT - (WndMaxCharX( wnd ) / 2);
            line->extent = tick_count / total_ticks;
            if( line->extent == 0 && tick_count != 0 ) {
                line->extent = 1;
            }
            line->indent -= line->extent;
            line->text = (char *)&barData;
        }
    } else if( piece == PIECE_SEPARATOR ) {
        indentPiece = SEPARATOR_POINT;
        line->indent = indentPiece;
        line->vertical_line = P_TRUE;
        line->text = LIT( Empty_Str );
        line->attr = WPA_PLAIN;
        line->tabstop = P_FALSE;
        line->master_tabstop = P_TRUE;
    } else if( piece == PIECE_HOOK ) {
        if( row > curr_sio->level_open ) {
            return( P_FALSE );
        }
        line->tabstop = P_FALSE;
        line->master_tabstop = P_TRUE;
        line->text = LIT( Empty_Str );
        if( row == 0 ) {
            indentPiece += WndMaxCharX( wnd );
        } else {
            indentPiece += (row*2 - 1) * WndMaxCharX( wnd );
            line->indent = indentPiece;
            indentPiece += 2 * WndMaxCharX( wnd );
            line->draw_hook = P_TRUE;
            line->attr = WPA_PLAIN;
        }
    } else if( piece == PIECE_NAME_TITLE ) {
        line->indent = indentPiece;
        nameBuff = ProfRealloc( nameBuff, strlen( overviewHeaders[row] ) + 1 );
        strcpy( nameBuff, overviewHeaders[row] );
        line->text = nameBuff;
        line->tabstop = P_FALSE;
        line->master_tabstop = P_TRUE;
        if( curr_sio->level_open == row ) {
            line->attr = WPA_OVERVIEW_NAME;
        } else {
            line->attr = WPA_PLAIN;
        }
        indentPiece += WndExtentX( wnd, nameBuff );
    } else {
        line->indent = indentPiece;
        nameBuff = ProfRealloc( nameBuff, strlen( text ) + 1 );
        strcpy( nameBuff, text );
        line->text = nameBuff;
        line->tabstop = P_FALSE;
        line->master_tabstop = P_TRUE;
        if( curr_sio->level_open == row ) {
            line->attr = WPA_OVERVIEW_NAME;
        } else {
            line->attr = WPA_PLAIN;
        }
    }
    return( P_TRUE );
}



STATIC bint simageGetLine( a_window * wnd, int row )
/**************************************************/
{
    sio_data *      curr_sio;
    image_info *    image;

    curr_sio = WndExtra( wnd );
    sampNewRow = row != curr_sio->curr_proc_row;
    if( sampNewRow ) {
        curr_sio->curr_proc_row = row;
        image = SImageGetImage( wnd, row );
        if( image == NULL ) {
            return( P_FALSE );
        }
        curr_sio->curr_image = image;
        dispHighLight = image->main_load;
        dispName = image->name;
        dispCount = image->agg_count;
        localTicks = curr_sio->total_samples;
        maxTime = curr_sio->max_time;
        barMaxTime = curr_sio->bar_max;
        absGraphBar = curr_sio->abs_bar;
        relGraphBar = curr_sio->rel_bar;
    }
    return( P_TRUE );
}



STATIC bint smodGetLine( a_window *wnd, int row )
/***********************************************/
{
    sio_data        *curr_sio;
    mod_info        *mod;

    curr_sio = WndExtra( wnd );
    sampNewRow = row != curr_sio->curr_proc_row;
    if( sampNewRow ) {
        curr_sio->curr_proc_row = row;
        mod = SModGetModule( wnd, row );
        if( mod == NULL ) {
            return( P_FALSE );
        }
        curr_sio->curr_mod = mod;
        dispHighLight = P_FALSE;
        dispName = mod->name;
        dispCount = mod->agg_count;
        localTicks = curr_sio->curr_image->agg_count;
        maxTime = curr_sio->curr_image->max_time;
        barMaxTime = curr_sio->curr_image->bar_max;
        absGraphBar = curr_sio->curr_image->abs_bar;
        relGraphBar = curr_sio->curr_image->rel_bar;
    }
    return( P_TRUE );
}



STATIC bint sfileGetLine( a_window *wnd, int row )
/************************************************/
{
    sio_data        *curr_sio;
    file_info       *curr_file;

    curr_sio = WndExtra( wnd );
    sampNewRow = row != curr_sio->curr_proc_row;
    if( sampNewRow ) {
        curr_sio->curr_proc_row = row;
        curr_file = SFileGetFile( wnd, row );
        if( curr_file == NULL ) {
            return( P_FALSE );
        }
        dispHighLight = P_FALSE;
        dispName = curr_file->name;
        dispCount = curr_file->agg_count;
        localTicks = curr_sio->curr_mod->agg_count;
        maxTime = curr_sio->curr_mod->max_time;
        barMaxTime = curr_sio->curr_mod->bar_max;
        absGraphBar = curr_sio->curr_mod->abs_bar;
        relGraphBar = curr_sio->curr_mod->rel_bar;
    }
    return( P_TRUE );
}



STATIC bint srtnGetLine( a_window *wnd, int row )
/***********************************************/
{
    sio_data        *curr_sio;
    rtn_info        *curr_rtn;

    curr_sio = WndExtra( wnd );
    sampNewRow = row != curr_sio->curr_proc_row;
    if( sampNewRow ) {
        curr_sio->curr_proc_row = row;
        curr_rtn = SRtnGetRoutine( wnd, row );
        if( curr_rtn == NULL ) {
            return( P_FALSE );
        }
        dispHighLight = P_FALSE;
        dispName = curr_rtn->name;
        dispCount = curr_rtn->tick_count;
        localTicks = curr_sio->curr_file->agg_count;
        maxTime = curr_sio->curr_file->max_time;
        barMaxTime = curr_sio->curr_file->bar_max;
        absGraphBar = curr_sio->curr_file->abs_bar;
        relGraphBar = curr_sio->curr_file->rel_bar;
    }
    return( P_TRUE );
}



STATIC bint ssrcGetLine( a_window *wnd, int row )
/***********************************************/
{
    sio_data        *curr_sio;
    wp_srcfile      *wp_src;
    wp_srcline      *lines;
    int             index;
    int             adjusted_row;

    curr_sio = WndExtra( wnd );
    if( curr_sio->src_file == NULL ) {
        return( P_FALSE );
    }
    adjusted_row = row + 1;
    sampNewRow = row != curr_sio->curr_proc_row;
    if( sampNewRow ) {
        curr_sio->curr_proc_row = row;
        dispName = WPSourceGetLine( wnd, adjusted_row );
        if( dispName == NULL ) {
            return( P_FALSE );
        }
        dispHighLight = P_FALSE;
        wp_src = curr_sio->src_file;
        lines = wp_src->src_lines;
        dispCount = 0;
        index = 0;
        while( index < wp_src->wp_line_count ) {
            if( adjusted_row == lines[index].line ) {
                dispCount = lines[index].tick_count;
                break;
            }
            index++;
        }
        localTicks = curr_sio->curr_file->agg_count;
        maxTime = wp_src->max_time;
        barMaxTime = curr_sio->asm_src_info.bar_max;
        absGraphBar = curr_sio->asm_src_info.abs_bar;
        relGraphBar = curr_sio->asm_src_info.rel_bar;
    }
    return( P_TRUE );
}



STATIC bint sasmGetLine( a_window *wnd, int row )
/***********************************************/
{
    sio_data        *curr_sio;
    wp_asmfile      *wpasm_file;
    wp_asmline      *asm_line;
    int             asm_group;
    int             asm_row;

    curr_sio = WndExtra( wnd );
    if( curr_sio->asm_file == NULL ) {
        return( P_FALSE );
    }
    sampNewRow = row != curr_sio->curr_proc_row;
    if( sampNewRow ) {
        curr_sio->curr_proc_row = row;
        dispName = WPAsmGetLine( wnd, row );
        if( dispName == NULL ) {
            return( P_FALSE );
        }
        wpasm_file = curr_sio->asm_file;
        asm_line = WPGetAsmLoc( wpasm_file, row, &asm_group, &asm_row );
        dispHighLight = asm_line->source_line;
        if( dispHighLight ) {
            dispCount = 0;
        } else {
            dispCount = asm_line->u.asm_line.tick_count;
        }
        localTicks = curr_sio->curr_mod->agg_count;
        maxTime = wpasm_file->max_time;
        barMaxTime = curr_sio->asm_src_info.bar_max;
        absGraphBar = curr_sio->asm_src_info.abs_bar;
        relGraphBar = curr_sio->asm_src_info.rel_bar;
    } else if( dispName == NULL ) {
        return( P_FALSE );
    }
    return( P_TRUE );
}



STATIC bint sampleSetLine( a_window *wnd, int row, int piece,
                                        wnd_line_piece *line )
/************************************************************/
{
    sio_data        *curr_sio;
    clicks_t        bar_range;
    gui_ord         catcher_extent;
    gui_ord         slacker;
    int             wnd_rows;

    if( piece >= PIECE_LAST ) {
        return( P_FALSE );
    }
    curr_sio = WndExtra( wnd );
    wnd_rows = SampleNumRows( wnd );
    if( row > wnd_rows-1 ) {
        Ring();
        return( P_FALSE );
    }
    if( sampNewRow ) {
        catcher_extent = BAR_TAIL_POINT - (WndMaxCharX( wnd ) / 2);
        catcher_extent = WPPixelTruncWidth( catcher_extent );
        bar_range = catcher_extent * dispCount;
        if( curr_sio->total_samples == 0 ) {
            curr_sio->total_samples = 1;
        }
        if( localTicks == 0 ) {
            localTicks = 1;
        }
        if( maxTime == 0 ) {
            maxTime = 1;
        }
        slacker = WPPixelWidth( wnd );
        if( !absGraphBar ) {
            bar2Extent = 0;
        } else {
            if( !barMaxTime || relGraphBar ) {
                bar2Extent = bar_range / curr_sio->total_samples;
            } else {
                bar2Extent = bar_range / maxTime;
            }
            if( bar2Extent < slacker && bar_range != 0 ) {
                bar2Extent = slacker;
            }
        }
        if( !relGraphBar ) {
            barExtent = 0;
        } else {
            if( barMaxTime ) {
                barExtent = bar_range / maxTime;
            } else {
                barExtent = bar_range / localTicks;
            }
            if( barExtent < slacker && bar_range != 0 ) {
                barExtent = slacker;
            }
        }
        barExtent = WPPixelTruncWidth( barExtent );
        bar2Extent = WPPixelTruncWidth( bar2Extent );
        if( barExtent < bar2Extent ) {
            barExtent = bar2Extent;
        }
    }
    if( piece == PIECE_MOUSE_CATCHER ) {
        line->indent = 0;
        line->text = LIT( Empty_Str );
        line->extent = catcher_extent - barExtent;
        line->tabstop = P_FALSE;
        line->master_tabstop = P_TRUE;
    } else if( piece == PIECE_BAR ) {
        line->extent = barExtent;
        line->indent = BAR_TAIL_POINT - barExtent;
        if( WPPixelTruncWidth( WndMaxCharX( wnd ) / 2 ) == 0 ) {
            line->indent -= WndMaxCharX( wnd ) / 2;
        }
        if( barExtent || bar2Extent ) {
            barData.bar_style = GUI_BAR_SHADOW;
            barData.bar_colour = WPA_REL_BAR;
            barData.bar_colour2 = WPA_ABS_BAR;
            barData.bar_size2 = bar2Extent;
            barData.bar_group = P_TRUE;
            barData.bar_selected = P_FALSE;
            line->draw_bar = P_TRUE;
            line->text = (char *)&barData;
        } else {
            line->text = LIT( Empty_Str );
        }
        line->tabstop = P_FALSE;
        line->master_tabstop = P_TRUE;
    } else if( piece == PIECE_SEPARATOR ) {
        line->indent = SEPARATOR_POINT;
        line->vertical_line = P_TRUE;
        line->text = LIT( Empty_Str );
        line->attr = WPA_PLAIN;
        line->tabstop = P_FALSE;
        line->master_tabstop = P_TRUE;
    } else if( piece == PIECE_DETAIL_NAME ) {
        line->indent = SEPARATOR_POINT + WndMaxCharX( wnd );
        line->text = dispName;
        if( dispHighLight ) {
            line->attr = WPA_STANDOUT;
        } else {
            line->attr = WPA_PLAIN;
        }
        line->tabstop = P_TRUE;
        line->master_tabstop = P_TRUE;
    } else {
        return( P_FALSE );
    }
    return( P_TRUE );
}



STATIC void findRtnFromRow( sio_data *curr_sio, int row )
/*******************************************************/
{
    file_info           *curr_file;
    rtn_info            *curr_rtn;
    cue_handle          *ch;
    sym_handle          *sh;
    int                 index;
    mod_handle          mh;
    address             addr;

    index = 0;
    ch = alloca( DIPHandleSize( HK_CUE ) );
    curr_file = curr_sio->curr_file;
    mh = curr_sio->curr_mod->mh;
    if( LineCue( mh, curr_sio->curr_file->fid, row, 0, ch ) == SR_NONE ) {
        if( LineCue( mh, curr_sio->curr_file->fid, 0, 0, ch ) == SR_NONE ) return;
    }
    sh = alloca( DIPHandleSize( HK_SYM ) );
    addr = CueAddr( ch );
    if( AddrSym( mh, addr, sh ) == SR_NONE ) return;
    while( index < curr_file->rtn_count ) {
        curr_rtn = curr_file->routine[index];
        if( curr_rtn->sh != NULL
         && SymCmp( curr_rtn->sh, sh ) == 0 ) {
            curr_sio->curr_rtn = curr_rtn;
            break;
        }
        index++;
    }
}



STATIC void sampFixDirtyCurr( a_window *wnd )
/*******************************************/
{
    sio_data        *curr_sio;
    wp_srcfile      *src_file;
    rtn_info        *curr_rtn;
    int             src_line;
    int             row;
    int             piece;

    curr_sio = WndExtra( wnd );
    WndGetCurrent( wnd, &row, &piece );
    if( row == WND_NO_ROW ) return;
    if( curr_sio->level_open == LEVEL_SAMPLE ) {
        if( !simageGetLine( wnd, row ) ) return;
    } else if( curr_sio->level_open == LEVEL_IMAGE ) {
        if( !smodGetLine( wnd, row ) ) return;
    } else if( curr_sio->level_open == LEVEL_MODULE ) {
        if( !sfileGetLine( wnd, row ) ) return;
    } else if( curr_sio->level_open == LEVEL_FILE ) {
        if( !srtnGetLine( wnd, row ) ) return;
    } else {
        if( curr_sio->level_open == LEVEL_ROUTINE ) {
            if( !ssrcGetLine( wnd, row ) ) return;
        } else {
            if( !sasmGetLine( wnd, row ) ) return;
        }
        src_line = row + 1;
        if( curr_sio->level_open == LEVEL_SOURCE ) {
            src_line = WPAsmFindSrcLine( curr_sio, src_line );
        }
        curr_rtn = curr_sio->curr_rtn;
        findRtnFromRow( curr_sio, src_line );
        if( curr_rtn != curr_sio->curr_rtn ) {
            WndRowDirty( wnd, LEVEL_ROUTINE-STATUS_ROW-1+WndTop(wnd) );
        }
        src_file = curr_sio->src_file;
        if( src_file != NULL ) {
            src_file->samp_line = src_line;
        }
    }
    curr_sio->curr_proc_row = -WND_MAX_ROW;
    if( row != curr_sio->curr_display_row ) {
        curr_sio->curr_display_row = row;
        if( curr_sio->total_samples == 0 || localTicks == 0 ) {
            curr_sio->abs_count = 0;
            curr_sio->rel_count = 0;
        } else {
            curr_sio->abs_count = (dispCount*1000) / curr_sio->total_samples;
            curr_sio->rel_count = (dispCount*1000) / localTicks;
        }
        curr_sio->abs_on_screen = absGraphBar;
        curr_sio->rel_on_screen = relGraphBar;
        if( dispCount > 0 ) {
            if( curr_sio->rel_count == 0 ) {
                curr_sio->rel_count = 1;
            }
            if( curr_sio->abs_count == 0 ) {
                curr_sio->abs_count = 1;
            }
        }
        WndRowDirty( wnd, -2+WndTop(wnd) );
    }
}



STATIC int simageDetailLine( a_window *wnd, int row, bint multi_level )
/*********************************************************************/
{
    sio_data        *curr_sio;
    image_info      *image;

    curr_sio = WndExtra( wnd );
    image = SImageGetImage( wnd, row );
    if( image == NULL ) {
        if( !multi_level ) {
            Ring();
        }
        return( row );
    }
    if( image->exe_not_found ) {
        if( !multi_level ) {
            ErrorMsg( LIT( Exe_Not_Found ), image->name );
        }
        return( row );
    }
    if( image->dip_handle == NO_MOD ) {
        if( !multi_level ) {
            ErrorMsg( LIT( No_Symbol_Info ), image->name );
        }
        return( row );
    }
    if( image->exe_changed ) {
        if( !multi_level ) {
            ErrorMsg( LIT( Exe_Has_Changed ), image->name );
            image->exe_changed = P_FALSE;
        }
    }
    curr_sio->level_open++;
    curr_sio->curr_image = image;
    gatherSort( curr_sio );
    return( 0 );
}



STATIC int smodDetailLine( a_window *wnd, int row, bint multi_level )
/*******************************************************************/
{
    sio_data        *curr_sio;
    mod_info        *mod;

    curr_sio = WndExtra( wnd );
    mod = SModGetModule( wnd, row );
    if( mod->agg_count == 0 && mod->file_count == 2 ) {
        if( !multi_level ) {
            ErrorMsg( LIT( No_Symbol_Info ), mod->name );
        }
        return( row );
    }
    curr_sio->level_open++;
    curr_sio->curr_mod = mod;
    gatherSort( curr_sio );
    return( 0 );
}



STATIC int sfileDetailLine( a_window *wnd, int row, bint multi_level )
/********************************************************************/
{
    sio_data        *curr_sio;
    file_info       *curr_file;

    curr_sio = WndExtra( wnd );
    curr_file = SFileGetFile( wnd, row );
    if( curr_file->rtn_count == 0 ) {
        if( !multi_level ) {
            ErrorMsg( LIT( No_Routine_Names ), curr_file->name );
        }
        return( row );
    }
    curr_sio->level_open++;
    curr_sio->curr_file = curr_file;
    gatherSort( curr_sio );
    return( 0 );
}



STATIC int srtnDetailLine( a_window *wnd, int row, bint multi_level )
/*******************************************************************/
{
    sio_data        *curr_sio;
    rtn_info        *curr_rtn;
    int             line;

    multi_level = multi_level;
    curr_sio = WndExtra( wnd );
    curr_rtn = SRtnGetRoutine( wnd, row );
    curr_sio->curr_rtn = curr_rtn;
    line = srtnOpenDetail( curr_sio, P_TRUE );
    return( line );
}



STATIC int srtnOpenDetail( sio_data *curr_sio, bint go_down )
/***********************************************************/
{
    a_window        *wnd;
    wp_srcfile      *src_file;
    int             line;
    int             top_line;

    wnd = curr_sio->sample_window;
    src_file = curr_sio->src_file;
    if( src_file == NULL ) {
        src_file = WPSourceOpen( curr_sio, P_TRUE );
        if( src_file == NULL ) {
            if( go_down ) {
                curr_sio->level_open = LEVEL_ROUTINE;
                line = ssrcDetailLine( wnd, 0, P_TRUE );
                if( curr_sio->level_open == LEVEL_ROUTINE ) {
                    curr_sio->level_open = LEVEL_FILE;
                }
                return( line );
            }
            curr_sio->level_open = LEVEL_FILE;
            return( 0 );
        }
    }
    curr_sio->level_open = LEVEL_ROUTINE;
    line = src_file->samp_line;
    if( line < 1 ) {
        line = src_file->rtn_line;
    }
    top_line = line - 1 - WndRows( wnd ) / 2;
    if( top_line >= 0 ) {
        WndSetTop( wnd, top_line );
    }
    return( line-1 );
}



STATIC int ssrcDetailLine( a_window *wnd, int row, bint multi_level )
/*******************************************************************/
{
    sio_data        *curr_sio;
    wp_asmfile      *asm_file;
    int             top_line;

    curr_sio = WndExtra( wnd );
    asm_file = WPAsmOpen( curr_sio, row+1, multi_level );
    if( asm_file == NULL ) {
        return( row );
    }
    curr_sio->level_open++;
    curr_sio->asm_file = asm_file;
    top_line = asm_file->entry_line - WndRows( wnd ) / 2;
    if( top_line >= 0 ) {
        WndSetTop( wnd, top_line );
    }
    return( asm_file->entry_line );
}



STATIC int sasmDetailLine( a_window *wnd, int row, bint multi_level )
/*******************************************************************/
{
    wnd=wnd;
    multi_level=multi_level;
    Ring();
    return( row );
}



STATIC void sampleRefresh( a_window *wnd )
/***************************************/
{
    WndZapped( wnd );
}



extern void WPZoomIn( a_window *wnd, int row )
/********************************************/
{
    sio_data        *curr_sio;
    int             detail_rows;
    int             top_line;
    int             old_level;
    int             curr_line;
    bint            multi_level;

    curr_sio = WndExtra( wnd );
    if( row >= curr_sio->level_open && row < STATUS_ROW ) {
        Ring();
        return;
    }
    if( row < curr_sio->level_open ) {
        curr_sio->level_open = row;
        WndSetTop( wnd, 0 );
        gatherSort( curr_sio );
        if( curr_sio->level_open == LEVEL_ROUTINE ) {
            curr_line = srtnOpenDetail( curr_sio, P_FALSE );
        } else {
            curr_line = WPGetRow( curr_sio );
        }
    } else {
        row = row - STATUS_ROW - 1;
        curr_line = row;
        detail_rows = SampleNumRows( wnd );
        if( detail_rows < row+1 ) {
            Ring();
            return;
        }
        multi_level = P_FALSE;
        for( ;; ) {
            old_level = curr_sio->level_open;
            curr_line = overviewDetailRtns[curr_sio->level_open](
                                           wnd, row, multi_level );
            if( old_level == curr_sio->level_open ) break;
            detail_rows = SampleNumRows( wnd );
            if( detail_rows != 1 ) break;
            row = 0;
            multi_level = P_TRUE;
        }
    }
    curr_sio->curr_proc_row = -WND_MAX_ROW;
    curr_sio->curr_display_row = -WND_MAX_ROW;
    detail_rows = SampleNumRows( wnd );
    top_line = WndTop( wnd );
    row = curr_line - top_line;
    if( row >= WndRows( wnd ) ) {
        top_line = curr_line - WndRows( wnd ) / 2;
    }
    if( row < 0 ) {
        top_line = curr_line;
    }
    if( detail_rows-top_line < WndRows( wnd ) ) {
        top_line = detail_rows - WndRows( wnd );
        if( top_line < 0 ) {
            top_line = 0;
        }
    }
    WndSetTop( wnd, top_line );
    WndNewCurrent( wnd, curr_line, PIECE_DETAIL_NAME );
    WndDirty( wnd );
    if( curr_sio->level_open < LEVEL_SOURCE
     && curr_sio->asm_file != NULL ) {
        WPAsmClose( curr_sio->asm_file );
        curr_sio->asm_file = NULL;
    }
    if( curr_sio->level_open < LEVEL_ROUTINE
     && curr_sio->src_file != NULL ) {
        WPSourceClose( curr_sio->src_file );
        curr_sio->src_file = NULL;
    }
}



extern void WPBackOut( a_window * wnd )
/*************************************/
{
    sio_data *      curr_sio;

    curr_sio = WndExtra( wnd );
    if( curr_sio->level_open == 0 ) {
        Ring();
        return;
    }
    WPZoomIn( wnd, curr_sio->level_open-1 );
}



extern void WPDoPopUp( a_window * wnd, gui_menu_struct * gui_menu )
/*****************************************************************/
{
    sio_data *      curr_sio;

    WndPopUp( wnd, gui_menu );
    WndNoSelect( wnd );
    curr_sio = WndExtra( wnd );
    curr_sio->curr_proc_row = -WND_MAX_ROW;
    curr_sio->curr_display_row = -WND_MAX_ROW;
}



extern void WPFindDoPopUp( a_window * wnd, int id )
/*************************************************/
{
    gui_menu_struct *   gui_menu;
    int                 index;

    index = 0;
    for( ;; ) {
        gui_menu = &sampleMenu[index++];
        if( gui_menu->id == id ) break;
    }
    WPDoPopUp( wnd, gui_menu );
}



STATIC void gatherSort( sio_data * curr_sio )
/*******************************************/
{
    GatherCurrent( curr_sio );
    SortCurrent( curr_sio );
}



STATIC void setDisplay( a_window * wnd, sio_data * curr_sio, bint do_top )
/************************************************************************/
{
    curr_sio->curr_proc_row = -WND_MAX_ROW;
    curr_sio->curr_display_row = -WND_MAX_ROW;
    if( do_top ) {
        WndSetTop( wnd, 0 );
        WndNewCurrent( wnd, 0, PIECE_DETAIL_NAME );
    }
    WndDirty( wnd );
}



STATIC void sampleMenuItem( a_window * wnd, unsigned id, int row, int piece )
/***************************************************************************/
{
    sio_data *      curr_sio;
    int             sort_type;

    piece=piece;
    curr_sio = WndExtra( wnd );
    row += STATUS_ROW + 1;
    switch( id ) {
    case MENU_INITIALIZE:
        if( row <= STATUS_ROW ) {
            WndMenuGrayAll( wnd );
            if( row < 0 || row-1 >= curr_sio->level_open ) break;
            WndMenuEnable( wnd, MENU_SAMP_ZOOM_IN, P_TRUE );
            break;
        }
        WndMenuEnableAll( wnd );
        WndMenuCheck( wnd, MENU_SAMP_GATHER, GetCurrentGather( curr_sio ) );
        WndMenuCheck( wnd, MENU_SAMP_BAR_MAX_TIME, GetCurrentMaxBar( curr_sio ) );
        WndMenuCheck( wnd, MENU_SAMP_ABS, GetCurrentAbsBar( curr_sio ) );
        WndMenuCheck( wnd, MENU_SAMP_REL, GetCurrentRelBar( curr_sio ) );
        sort_type = GetCurrentSort( curr_sio );
        WndMenuCheck( wnd, MENU_SORT_COUNT, sort_type==SORT_COUNT );
        WndMenuCheck( wnd, MENU_SORT_NAME, sort_type==SORT_NAME );
        if( row <= STATUS_ROW || curr_sio->level_open >= LEVEL_ROUTINE ) {
            WndMenuEnable( wnd, MENU_SAMP_GATHER, P_FALSE );
            WndMenuEnable( wnd, MENU_SAMP_SORT, P_FALSE );
            WndMenuEnable( wnd, MENU_SORT_COUNT, P_FALSE );
            WndMenuEnable( wnd, MENU_SORT_NAME, P_FALSE );
            if( row <= STATUS_ROW ) {
                WndMenuEnable( wnd, MENU_SAMP_BAR_MAX_TIME, P_FALSE );
                WndMenuEnable( wnd, MENU_SAMP_ABS, P_FALSE );
                WndMenuEnable( wnd, MENU_SAMP_REL, P_FALSE );
            }
        }
        break;
    case MENU_SAMP_ZOOM_IN:
        WPZoomIn( wnd, row );
        break;
    case MENU_SAMP_BACK_OUT:
        WPBackOut( wnd );
        break;
//    case MENU_SAMP_DATA:
//        WPSImageOpen( curr_sio );
//        break;
    case MENU_SAMP_GATHER:
        FlipCurrentGather( curr_sio );
        WndMenuCheck( wnd, MENU_SAMP_GATHER, GetCurrentGather( curr_sio ) );
        gatherSort( curr_sio );
        setDisplay( wnd, curr_sio, P_TRUE );
        break;
    case MENU_SAMP_BAR_MAX_TIME:
        FlipCurrentMaxBar( curr_sio );
        WndMenuCheck( wnd, MENU_SAMP_BAR_MAX_TIME, GetCurrentMaxBar( curr_sio ) );
        setDisplay( wnd, curr_sio, P_FALSE );
        break;
    case MENU_SAMP_ABS:
        FlipCurrentAbsBar( curr_sio );
        WndMenuCheck( wnd, MENU_SAMP_ABS, GetCurrentAbsBar( curr_sio ) );
        setDisplay( wnd, curr_sio, P_FALSE );
        break;
    case MENU_SAMP_REL:
        FlipCurrentRelBar( curr_sio );
        WndMenuCheck( wnd, MENU_SAMP_ABS, GetCurrentRelBar( curr_sio ) );
        setDisplay( wnd, curr_sio, P_FALSE );
        break;
    case MENU_SORT_COUNT:
    case MENU_SORT_NAME:
        WndMenuCheck( wnd, MENU_SORT_COUNT, id==MENU_SORT_COUNT );
        WndMenuCheck( wnd, MENU_SORT_NAME, id==MENU_SORT_COUNT );
        if( id == MENU_SORT_COUNT ) {
            SetCurrentSort( curr_sio, SORT_COUNT );
        } else {
            SetCurrentSort( curr_sio, SORT_NAME );
        }
        SortCurrent( curr_sio );
        setDisplay( wnd, curr_sio, P_TRUE );
        break;
    }
}
