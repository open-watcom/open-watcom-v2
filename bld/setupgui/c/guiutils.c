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
* Description:  Functions controlling GUI attributes.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wio.h"
#include "gui.h"
#include "guidlg.h"
#include "guistr.h"
#include "setup.h"
#include "setupinf.h"
#include "resource.h"
#include "banner.h"
#include "genvbl.h"
#include "utils.h"
#include "guiutils.h"

#include "clibext.h"


extern gui_colour_set   MainColours[];
extern void             GUISetJapanese();
extern bool             Invisible;
gui_window              *MainWnd = NULL;
int                     NominalButtonWidth = 11;

const char *Bolt[] = {
    "",
    "               ________                          ________              ",
    "              /:::::::/                         /:::::::/              ",
    "             /:::::::/__                       /:::::::/__             ",
    "            /::::::::::/                      /::::::::::/             ",
    "            -----/::::/__                     -----/::::/__            ",
    "                /......./                         /......./            ",
    "               -----/./                          -----/./              ",
    "                   /./                               /./               ",
    "                  //                                //                 ",
    "                 /                                 /                   ",
    "                                                                       ",
    "                ________                          ________             ",
    "               /:::::::/                         /:::::::/             ",
    "              /:::::::/__                       /:::::::/__            ",
    "             /::::::::::/                      /::::::::::/            ",
    "             -----/::::/__                     -----/::::/__           ",
    "                 /......./                         /......./           ",
    "                -----/./                          -----/./             ",
    "                   /./                               /./               ",
    "                  //                                //                 ",
    "                 /                                 /                   ",
    "                                                                       "
};

#define WND_APPROX_SIZE 10000

gui_resource WndGadgetArray[] = {
    BITMAP_SPLASH, "splash",
};

gui_ord     BitMapBottom;
gui_coord   BitMapSize;


static bool MainSetupWndGUIEventProc( gui_window *gui, gui_event gui_ev, void *parm )
{
    int                 i;

    /* unused parameters */ (void)parm;

    switch( gui_ev ) {
    case GUI_PAINT:
        if( GUIIsGUI() ) {
            gui_rect            rect;
            gui_text_metrics    metrics;
            gui_ord             indent;
            gui_ord             topdent;
            int                 row;

            gui_rgb             rgb, foreg;
            int                 row_count;

            GUIGetClientRect( gui, &rect );
            GUIGetTextMetrics( gui, &metrics );
            indent = (rect.width - BitMapSize.x) / 2;
            if( BitMapSize.x > rect.width )
                indent = 0;
            topdent = metrics.avg.y; // (rect.height - BitMapSize.y) / 2;
            BitMapBottom = BitMapSize.y + metrics.avg.y;
            if( BitMapSize.y > rect.height )
                topdent = 0;
            row = topdent / metrics.max.y;
            GUIDrawHotSpot( gui, 1, row, indent, GUI_BACKGROUND );

            /*
             *  Do copyright stuff. There is a chance that we could overwrite the
             *  bitmap's graphics section if this stuff became too big, but that's a
             *  risk I'll have to take for now. I can't be bothered to actually calculate
             *  the clean space within the bitmap.
             */

            if( BitMapSize.y ) {
                row_count = BitMapSize.y / metrics.max.y;
            } else {
                /* If there is no bitmap attached - such as virgin.exe - then just
                 * copyright to upper screen */
                row_count = 3;
                indent = 16;
            }

            GUIGetRGB( GUI_BRIGHT_BLUE, &rgb ); /* background - no effect */
            GUIGetRGB( GUI_BLACK, &foreg );     /* foreground */

            /* Start at bottom left of hotspot and use neagtive offset */
            GUIDrawTextRGB( gui, banner2, sizeof( banner2 ) - 1, row_count - 2, indent, foreg, rgb );
            GUIDrawTextRGB( gui, banner2a( 1984 ), sizeof( banner2a( 1984 ) ) - 1, row_count - 1, indent, foreg, rgb );

        } else {
            for( i = 0; i < sizeof( Bolt ) / sizeof( Bolt[0] ); ++i ) {
                GUIDrawTextExtent( gui, Bolt[i], strlen( Bolt[i] ), i, 0, GUI_BACKGROUND, WND_APPROX_SIZE );
            }
        }
        return( true );
    default:
        break;
    }
    return( false );
}

gui_coord               GUIScale;

#if defined( __WINDOWS__ )
static bool CheckForSetup32( int argc, char **argv )
{
    DWORD       version = GetVersion();
    int         winver;
    char        *buff;
    size_t      mem_needed;
    int         i;
    char        new_exe[_MAX_PATH];
    char        drive[_MAX_DRIVE];
    char        path[_MAX_PATH];
    char        name[_MAX_FNAME];
    char        ext[_MAX_EXT];
    char        *os;

    winver = LOBYTE( LOWORD( version ) ) * 100 + HIBYTE( LOWORD( version ) );
    os = getenv( "OS" );
    if( winver >= 390 || ( os != NULL && stricmp( os, "Windows_NT" ) == 0 ) ) {
        _splitpath( argv[0], drive, path, name, ext );
        _makepath( new_exe, drive, path, "SETUP32", ext );
        mem_needed = strlen( new_exe );
        for( i = 1; i < argc; i++ ) {
            mem_needed += strlen( argv[i] ); // command line arguments
        }
        mem_needed += i; // spaces between arguments + terminating null
        buff = malloc( mem_needed );
        if( buff == NULL ) {
            return( false );
        }
        strcpy( buff, new_exe );
        if( access( buff, F_OK ) == 0 ) {
            for( i = 1; i < argc; i++ ) {
                strcat( buff, " " );
                strcat( buff, argv[i] );
            }
            WinExec( buff, SW_SHOW );
            return( true );
        }
    }
    return( false );
}
#elif defined( __NT__ ) && !defined( _M_X64 )
static bool CheckWin95Uninstall( int argc, char **argv )
{
// The Windows 95 version of setup gets installed as the
// uninstall utility. So that it can erase itself, the setup
// program gets copied to the Windows directory, and run from
// there. The version in the Windows directory gets erased by
// the WININIT program.

    size_t              len;
    char                buff[2 * _MAX_PATH];
    char                drive[_MAX_DRIVE];
    char                dir[_MAX_DIR];
    char                name[_MAX_FNAME];

    if( argc > 1 && stricmp( argv[1], "-u" ) == 0 ) {
        // copy setup program to unsetup.exe in system directory
        GetWindowsDirectory( buff, _MAX_PATH );
        strcat( buff, "\\UnSetup.exe" );
        if( DoCopyFile( argv[0], buff, false ) == CFE_NOERROR ) {
            // add entry to wininit.ini to erase unsetup.exe
            WritePrivateProfileString( "rename", "NUL", buff, "wininit.ini" );
            // setup.inf should be in same directory as setup.exe
            len = strlen( buff );
            buff[len] = ' ';
            buff[len + 1] = '\"';
            _splitpath( argv[0], drive, dir, name, NULL );
            _makepath( &buff[len + 2], drive, dir, name, "inf" );
            strcat( buff, "\"" );
            // execute unsetup
            WinExec( buff, SW_SHOW );
            return( true );
        }
    }
    return( false );
}

static bool CheckWow64( void )
{
    DWORD   version = GetVersion();
    if( version < 0x80000000 && LOBYTE( LOWORD( version ) ) >= 5 && IsWOW64() ) {
        char *msg = "You are using 32-bit installer on 64-bit host\n"
                    "It is recommended to use 64-bit installer\n"
                    "\ton 64-bit host\n"
                    "Press OK button to continue with installation\n"
                    "\tor Cancel button to abort it\n";

        InitGlobalVarList();
        SetVariableByName( "IDS_USEINST64BIT", "%s");
        if( MsgBox( NULL, "IDS_USEINST64BIT", GUI_OK_CANCEL, msg ) != GUI_RET_OK ) {
            /* return true to terminate installer */
            return( true );
        }
    }
    return( false );
}
#endif

bool SetupPreInit( int argc, char **argv )
/****************************************/
{
    gui_rect            rect;

#if defined( __WINDOWS__ )
    if( CheckForSetup32( argc, argv ) )
        return false;
#elif defined( __NT__ ) && !defined( _M_X64 )
    if( CheckWin95Uninstall( argc, argv ) )
        return false;
    if( CheckWow64() )
        return false;
#else
    (void)argc; (void)argv;
#endif

    /* Cancel button may be wider in other languages */
    NominalButtonWidth = strlen( LIT( Cancel ) ) + 5;

    /* Initialize enough of the GUI lib to let us show message boxes etc. */
    GUIWndInit( 300, GUI_PLAIN ); // 300 uS mouse dbl click rate, no char remapping
    GUISetCharacter( GUI_SCROLL_SLIDER, 177 );
    GUISetBetweenTitles( 2 );
    GUIScale.x = WND_APPROX_SIZE;
    GUIScale.y = WND_APPROX_SIZE;
    GUIGetRoundScale( &GUIScale );
    rect.x = 0;
    rect.y = 0;
    rect.width = GUIScale.x;
    rect.height = GUIScale.y;
    GUISetScale( &rect );

    return( true );
}

bool SetupInit( void )
/********************/
{
    gui_rect            rect;
    gui_create_info     init;

    GUIGetScale( &rect );
    memset( &init, 0, sizeof( init ) );
    init.rect = rect;
    init.scroll = 0;
    init.style = 0;
    init.title = "";
    if( Invisible ) {
        init.style |= GUI_INIT_INVISIBLE;
    } else {
        init.style |= GUI_VISIBLE | GUI_MAXIMIZE | GUI_MINIMIZE;
    }
#ifdef _UI
    init.style |= GUI_NOFRAME;
#endif
    init.parent = NULL;
    init.menu.num_items = 0;
    init.menu.menu = NULL;
    init.num_attrs = WND_NUMBER_OF_COLORS;
    init.colours = MainColours;
    init.gui_call_back = MainSetupWndGUIEventProc;
    init.extra = NULL;

    GUIInitHotSpots( 1, WndGadgetArray );
    GUIGetHotSpotSize( 1, &BitMapSize );

    MainWnd = GUICreateWindow( &init );

    /* remove GUI toolkit adjustment here as it is no longer required */

    return( true );
}


void SetupTitle( void )
/*********************/
{
    char        buff[MAXBUF];

    ReplaceVars( buff, sizeof( buff ), GetVariableStrVal( "AppName" ) );
    GUISetWindowText( MainWnd, buff );
}


void SetupFini( void )
/***************************/
{
    if( MainWnd != NULL ) {
        GUIDestroyWnd( MainWnd );
    }
}
