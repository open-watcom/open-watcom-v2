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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


enum {
    CTL_OK = 100,
    CTL_CANCEL,
    CTL_DEFAULTS,

    CTL_STRETCH,
    CTL_ABS_BAR,
    CTL_REL_BAR,

    CTL_SORT_COUNT,
    CTL_SORT_NAME,

    CTL_GATHER,
    CTL_GATHER_CUT,

    CTL_GATHER_TEXT,
};

#define C0 0
#define C1 1

#define R0 0
#define R1 5
#define R2 9
#define R3 15

#define BW 10
#define W 50
#define B1 BUTTON_POS( 1, 3, W, BW )
#define B2 BUTTON_POS( 2, 3, W, BW )
#define B3 BUTTON_POS( 3, 3, W, BW )

#define SMALL_CUT_WIDTH 10
#define DLG_OPTS_ROWS   17
#define DLG_OPTS_COLS   50

static gui_control_info optionControls[] =
{
    DLG_BOX( "Bar Graph",                               C1+1, R0, W-2, R0+4 ),
    DLG_CHECK( "&Stretch the graph", CTL_STRETCH,       C1+2, R0+1, W-3 ),
    DLG_CHECK( "Show &Absolute Bar Graphs", CTL_ABS_BAR,C1+2, R0+2, W-3 ),
    DLG_CHECK( "Show &Relative Bar Graphs", CTL_REL_BAR,C1+2, R0+3, W-3 ),

    DLG_BOX( "Sort by",                                 C1+1, R1, W-2, R1+3 ),
    DLG_RADIO_START( "Sample &Count", CTL_SORT_COUNT,   C1+2, R1+1, W-3 ),
    DLG_RADIO_END( "&Name", CTL_SORT_NAME,              C1+2, R1+2, W-3 ),

    DLG_BOX( "Gather",                                  C1+1, R2, W-2, R2+4 ),
    DLG_DYNSTRING( "Cutoff Percentage", CTL_GATHER_TEXT,C1+2, R2+1, W-3 ),
    DLG_EDIT( "", CTL_GATHER_CUT,                       W-13,  R2+1, W-3 ),
    DLG_CHECK( "Gather Small &Values", CTL_GATHER,      C1+2, R2+3, W-3 ),

    DLG_DEFBUTTON( "OK", CTL_OK,                        B1,  R3, B1+BW ),
    DLG_BUTTON( "&Defaults", CTL_DEFAULTS,              B2,  R3, B2+BW ),
    DLG_BUTTON( "Cancel", CTL_CANCEL,                   B3,  R3, B3+BW ),
};
