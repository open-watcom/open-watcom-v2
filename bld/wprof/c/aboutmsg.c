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

extern char     *AboutMessage[];
extern int      AboutSize;

STATIC bool aboutEventProc( a_window *, gui_event, void * );
STATIC int  AboutNumRows( a_window * );
STATIC bool aboutGetLine( a_window *, wnd_row, int, wnd_line_piece * );

static a_window     *aboutWindow = NULL;
static bint         aboutOn = TRUE;



wnd_info AboutInfo = {
    aboutEventProc,
    NoRefresh,
    aboutGetLine,
    NoMenuItem,
    NoScroll,
    NoBegPaint,
    NoEndPaint,
    NoModify,
    AboutNumRows,
    NoNextRow,
    NoNotify,
    NULL,
    0,
    NoPopUp,
};



extern void AboutOpen( void )
/***************************/
{
    if( aboutWindow == NULL ) {
        aboutWindow = WndCreate(
                                LIT( WPROF_TITLE ),
                                &AboutInfo,
                                 WND_NO_CLASS,
                                 NULL );
    }
    if( aboutWindow != NULL ) {
        WndShowWindow( aboutWindow );
    }
}



extern void AboutClose( void )
/****************************/
{
    a_window *  wnd;

    if( aboutWindow != NULL ) {
        wnd = aboutWindow;
        WndClose( wnd );
    }
}



STATIC int AboutNumRows( a_window * wnd )
/***************************************/
{
    int     ret_size;

    wnd=wnd;
    if( aboutOn ) {
        ret_size = AboutSize;
//        if( GUIIsGUI() ) {
//            ret_size++;
//        }
        return( ret_size );
    }
    return( 0 );
}



STATIC bool aboutGetLine( a_window * wnd, wnd_row row, int piece,
                                      wnd_line_piece * line )
/***********************************************************/
{
//    gui_coord           size;

    wnd=wnd;
    if( piece != 0 || !aboutOn ) return( P_FALSE );
    if( row >= AboutSize ) {
/* the following code fragment was ripped from the debugger */
/* Something like this can be done for the splash page?? */
//        if( row > AboutSize || !GUIIsGUI() ) return( P_FALSE );
//    WndSetGadgetLine( wnd, line, GADGET_SPLASH, MaxGadgetLength );
//        WndGetGadgetSize( GADGET_SPLASH, &size );
//        line->indent = ( WndWidth( wnd ) - size.x ) / 2;
//        return( P_TRUE );
        return( P_FALSE );
    }
    line->text = AboutMessage[ row ];
    line->indent = ( WndWidth( wnd ) - WndExtentX( wnd, line->text ) ) / 2;
    return( P_TRUE );
}



STATIC bool aboutEventProc( a_window * wnd, gui_event gui_ev, void * parm )
/*************************************************************************/
{
    wnd=wnd;
    parm=parm;
    switch( gui_ev ) {
    case GUI_INIT_WINDOW:
        return( TRUE );
    case GUI_DESTROY :
        aboutWindow = NULL;
        return( TRUE );
    }
    return( FALSE );
}



extern void AboutSetOff( void )
/*****************************/
{
    aboutOn = P_FALSE;
    if( aboutWindow != NULL ) {
        WndZapped( aboutWindow );
    }
}



extern void DlgAbout( void )
/**************************/
{
    char        *about_data;
    char        *about_rover;
    int         about_len;
    int         index;

    about_len = 0;
    for( index = 0; index < AboutSize; ++index ) {
        about_len += strlen( AboutMessage[index] ) + 1;
    }
    about_data = ProfAlloc( about_len+1 );
    about_rover = about_data;
    for( index = 0; index < AboutSize; ++index ) {
        about_len = strlen( AboutMessage[index] );
        memcpy( about_rover, AboutMessage[index], about_len );
        about_rover += about_len;
        *about_rover++ = '\r';
    }
    *about_rover = NULLCHAR;
    WndDisplayMessage( about_data, LIT( About_WPROF ), GUI_INFORMATION );
    ProfFree( about_data );
}
