/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  'About' dialog for wprof.
*
****************************************************************************/


#include <string.h>

#include "common.h"
#include "banner.h"
#include "aui.h"
#include "guidlg.h"
#include "msg.h"
#include "memutil.h"
#include "aboutmsg.h"


static char *AboutMessage[] = {
    "",
    banner1w1( "Execution Profiler" STR_BITNESS ),
    banner1w2( _WPROF_VERSION_ ),
    banner2,
    banner2a( 1987 ),
    "",
    banner3,
    banner3a,
/* NB: it is disabled due to incompatibility with DOS/4G 2.x */
#if 0
#ifdef D32_NAME
    "",
    D32_NAME " " D32_VERSION,
    D32_COPYRIGHT,
#endif
#endif
};

/* NB: DOS4GOPTIONS export is disabled due to incompatibility with DOS/4G 2.x */
#if 0
#ifdef __DOS__
char DOS4GOPTIONS[] =
        "[dos4g-global]\n"
        "Include=WPROFOPT.INI\n"
        "[dos4g-kernel]\n"
        "StartupBanner=FALSE\n"
;
#endif
#endif

static a_window     aboutWindow = NULL;
static bool         aboutOn = true;


STATIC wnd_row AboutNumRows( a_window wnd )
/*****************************************/
{
    wnd_row     rows;

    /* unused parameters */ (void)wnd;

    if( aboutOn ) {
        rows = ArraySize( AboutMessage );
//        if( GUIIsGUI() ) {
//            rows++;
//        }
        return( rows );
    }
    return( 0 );
}


STATIC bool aboutGetLine( a_window wnd, wnd_row row, wnd_piece piece, wnd_line_piece *line )
/******************************************************************************************/
{
//    gui_coord           size;

    /* unused parameters */ (void)wnd;

    if( piece != 0 || !aboutOn )
        return( false );
    if( row >= ArraySize( AboutMessage ) ) {
/* the following code fragment was ripped from the debugger */
/* Something like this can be done for the splash page?? */
//        if( row > ArraySize( AboutMessage ) || !GUIIsGUI() )
//            return( false );
//    WndSetGadgetLine( wnd, line, GADGET_SPLASH, MaxGadgetLength );
//        WndGetGadgetSize( GADGET_SPLASH, &size );
//        line->indent = ( WndWidth( wnd ) - size.x ) / 2;
//        return( true );
        return( false );
    }
    line->text = AboutMessage[ row ];
    line->indent = ( WndWidth( wnd ) - WndExtentX( wnd, line->text ) ) / 2;
    return( true );
}


STATIC bool aboutWndEventProc( a_window wnd, gui_event gui_ev, void *parm )
/*************************************************************************/
{
    /* unused parameters */ (void)wnd; (void)parm;

    switch( gui_ev ) {
    case GUI_INIT_WINDOW:
        return( true );
    case GUI_DESTROY :
        aboutWindow = NULL;
        return( true );
    }
    return( false );
}


static wnd_info     AboutInfo = {
    aboutWndEventProc,
    NoRefresh,
    aboutGetLine,
    NoMenuItem,
    NoVScroll,
    NoBegPaint,
    NoEndPaint,
    NoModify,
    AboutNumRows,
    NoNextRow,
    NoNotify,
    NoChkUpdate,
    NoPopUp,
};


void AboutOpen( void )
/********************/
{
    if( aboutWindow == NULL ) {
        aboutWindow = WndCreate( LIT( WPROF_TITLE ), &AboutInfo, WND_NO_CLASS, NULL );
    }
    if( aboutWindow != NULL ) {
        WndShowWindow( aboutWindow );
    }
}



void AboutClose( void )
/*********************/
{
    a_window    wnd;

    if( aboutWindow != NULL ) {
        wnd = aboutWindow;
        WndClose( wnd );
    }
}



void AboutSetOff( void )
/**********************/
{
    aboutOn = false;
    if( aboutWindow != NULL ) {
        WndZapped( aboutWindow );
    }
}



void DlgAbout( void )
/*******************/
{
    char        *about_data;
    char        *about_rover;
    size_t      about_len;
    int         index;

    about_len = 0;
    for( index = 0; index < ArraySize( AboutMessage ); ++index ) {
        about_len += strlen( AboutMessage[index] ) + 1;
    }
    about_data = ProfAlloc( about_len+1 );
    about_rover = about_data;
    for( index = 0; index < ArraySize( AboutMessage ); ++index ) {
        about_len = strlen( AboutMessage[index] );
        memcpy( about_rover, AboutMessage[index], about_len );
        about_rover += about_len;
        *about_rover++ = '\r';
    }
    *about_rover = NULLCHAR;
    WndDisplayMessage( about_data, LIT( About_WPROF ), GUI_INFORMATION );
    ProfFree( about_data );
}
