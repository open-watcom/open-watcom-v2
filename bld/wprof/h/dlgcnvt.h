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

    CTL_NAME,
    CTL_BROWSE,

    CTL_DIF_FMT,
    CTL_COMMA_FMT,
};

#define C0 0
#define C1 38

#define R0 0
#define R1 3
#define R2 8

#define BW 10
#define W 50
#define B1 BUTTON_POS( 1, 3, W, BW )
#define B2 BUTTON_POS( 2, 3, W, BW )
#define B3 BUTTON_POS( 3, 3, W, BW )

#define DLG_CNVT_ROWS   10
#define DLG_CNVT_COLS   50

static gui_control_info convertControls[] =
{
    DLG_BOX( "Output File Name",                        C0, R0, C1-2, R0+2 ),
    DLG_EDIT( "", CTL_NAME,                             C0+1, R0+1, C1-3 ),
    DLG_BUTTON( "&Browse ...", CTL_BROWSE,              C1,  R0+1, W-1 ),

    DLG_BOX( "Format Type",                             C0, R1, C1-2, R1+3 ),
    DLG_RADIO_START( "&DIF Format", CTL_DIF_FMT,        C0+1, R1+1, C1-3 ),
    DLG_RADIO_END( "&Comma Format", CTL_COMMA_FMT,      C0+1, R1+2, C1-3 ),

    DLG_DEFBUTTON( "OK", CTL_OK,                        B1,  R2, B1+BW ),
    DLG_BUTTON( "&Defaults", CTL_DEFAULTS,              B2,  R2, B2+BW ),
    DLG_BUTTON( "Cancel", CTL_CANCEL,                   B3,  R2, B3+BW ),
};
