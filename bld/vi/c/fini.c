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
* Description:  Editor shutdown.
*
****************************************************************************/


#include "vi.h"
#ifdef __WIN__
    #include "winrtns.h"
    #include "subclass.h"
    #include "utils.h"
#endif
#include "menu.h"
#include "win.h"
#include "fcbmem.h"
#include "sstyle.h"
#include "fts.h"
#include "rxsupp.h"
#include "rcs.h"
#include "myprtf.h"

#ifdef __WIN__
    #define T1      ""
    #define T2      "\t"
#else
    #define T1      "\t"
    #define T2      "\t\t"
#endif

char * UsageMsg[] = {
#ifdef __WIN__
    "viw [-?-dinqrvz] +<n> -k\"keys\" [-s<scr> [-p\"prm\"]] [-t<tag>]",
    "    [-c<cfg>] files",
#else
    "Usage: vi [-?-dinqrvz] +<n> -k\"keys\" [-s<scr> [-p\"prm\"]] [-t<tag>]\n          [-c<cfg>] files",
#endif
    T1 "files             : files to edit (may contain wild cards)",
    T1 "Options: -?       : print this list",
    T2 " --       : file is read from stdin, and written to stdout",
    T2 " -d       : use default configuration (do not invoke ed.cfg)",
    T2 " -i       : ignore lost files",
    T2 " -n       : no readentirefile",
    T2 " -q       : quiet mode (no screen output)",
    T2 " -r       : recover lost files",
    T2 " -v       : set view-only mode",
    T2 " -z       : do not terminate file read when finding a ctrl-z",
    T2 " +<n>     : go to line number <n> in first file loaded",
    T2 " -k \"keys\": keystrokes to execute after the editor has started",
    T2 " -s <scr> : execute source script <scr> after editing files",
    T2 " -p \"prm\" : parameters associated with source script",
    T2 " -t <tag> : edit file with specified <tag>",
    T2 " -c <cfg> : use <cfg> as the configuration file"
};

char *OptEnvVar = "VI";

/*
 * Quit - print usage messages
 */
void Quit( const char **usage_msg, const char *str, ... )
{
    va_list     al;

    usage_msg = usage_msg;
#ifdef __WIN__
    {
        char    buff[MAX_STR];

        if( str != NULL ) {
            va_start( al, str );
            MyVSprintf( buff, str, al );
            va_end( al );
        } else {
            buff[0] = 0;
        }
        CloseStartupDialog();
        UsageDialog( UsageMsg, buff,  sizeof( UsageMsg ) / sizeof( char *) );
    }
#else
    {
        int     i;
        int     cnt;

        if( str != NULL ) {
            va_start( al, str );
            MyVPrintf( str, al );
            va_end( al );
            cnt = 1;
        } else {
            cnt = sizeof( UsageMsg ) / sizeof( char *);
        }

        for( i = 0; i < cnt; i++ ) {
            MyPrintf( "%s\n", UsageMsg[i] );
        }
    }
#endif
    // can't do an ExitEditor because we will not have initialized anything
    // yet (this is always called from checkFlags)
    // ExitEditor( 0 );
    ChangeDirectory( HomeDirectory );
    FiniMem();
    exit( 0 );

} /* Usage */

// free globals allocated directly in InitEditor
static void miscGlobalsFini( void )
{
    MemFree( WorkLine );
    MemFree( DotBuffer );
    MemFree( AltDotBuffer );
    MemFree( DotCmd );
    MemFree( ReadBuffer );
    MemFree( WriteBuffer );
    MemFree( EditVars.TileColors );
    MemFree( EditVars.GrepDefault );
    MemFree( EditVars.FileEndString );
    MemFree( EditVars.Majick );
    MemFree( EditVars.GadgetString );
    MemFree( EditVars.WordDefn );
    MemFree( EditVars.WordAltDefn );
    MemFree( EditVars.TagFileName );
    MemFree( EditVars.StatusString );
    MemFree( EditVars.StatusSections );
    MemFree( EditVars.TmpDir );
    MemFree( EditVars.SpawnPrompt );
    MemFree( CurrentRegularExpression );
}

/*
 * ExitEditor - do just that
 */
void ExitEditor( int rc )
{
#ifdef __WIN__
    WriteProfile();
#endif
#ifdef __IDE__
    IDEFini();
#endif
#ifdef __WIN__
    DDEFini();
#endif
    SaveHistory();
    RestoreInterrupts();
    SwapFileClose();
    WindowSwapFileClose();
    SwapBlockFini();
    ExtendedMemoryFini();
    SelRgnFini();
    LangFiniAll();
    FiniMouse();
    FiniMenu();
    FiniSavebufs();
    FindCmdFini();
    DirFini();
    CurrentWindow = NO_WINDOW;
    FinishWindows();
    ScreenFini();
#ifdef __WIN__
    FiniClrPick();
    FiniFtPick();
    CursorOp( COP_FINI );
    SubclassGenericFini();
    FiniProfile();
#endif
    FiniFileStack();
    DeleteResidentScripts();
    MatchFini();
    FiniKeyMaps();
    ErrorFini();
    FiniCommandLine();
    SSFini();
    HistFini();
    BoundDataFini();
    FTSFini();
    StaticFini();
    VarFini();
    AutoSaveFini();
    FiniCFName();
    miscGlobalsFini();
    ChangeDirectory( HomeDirectory );
#if defined( __NT__ ) && !defined( __WIN__ )
    {
        SetConsoleActiveScreenBuffer( GetStdHandle( STD_OUTPUT_HANDLE ) );
    }
#endif
    MemFree( HomeDirectory );
    MemFree( CurrentDirectory );
#if defined( VI_RCS )
    ViRCSFini();
#endif
    FiniMem();
    exit( rc );

} /* ExitEditor */

/*
 * QuitEditor - quit the editor
 */
void QuitEditor( vi_rc rc )
{
#ifndef __WIN__
    ScreenPage( -1000 );
    EditFlags.NoSetCursor = FALSE;
    SetPosToMessageLine();
#endif
    ExitEditor( ( rc == ERR_NO_ERR ) ? 0 : -1 );

} /* QuitEditor */
