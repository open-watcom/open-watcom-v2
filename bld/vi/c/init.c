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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __AXP__
#include <i86.h>
#endif
#include "vi.h"
#include "rxsupp.h"
#include "fcbmem.h"
#include "source.h"
#include "win.h"
#include "menu.h"
#include "getopt.h"
#include "sstyle.h"
#include "fts.h"
#ifdef __WIN__
#include "winvi.h"
#include "subclass.h"
#include "utils.h"
#endif
#ifdef __NT__
#include "windows.h"
#endif
#include "rcs.h"

static char     nullFN[] = "no_name";
static char     *cFN;
static char     *cfgFN=NULL;
static char     *cTag;
static char     *keysToPush;
#ifdef __WIN__
    static int  lineToGoTo = 0;
#endif
static char     goCmd[10];
static bool     wantNoReadEntireFile;
char            *WordDefnDefault = "::..\\\\__09AZaz";

#ifdef __WIN__
/*
 * SetConfigFileName
 */
void SetConfigFileName( char *fn )
{
    AddString2( &cfgFN, fn );

} /* SetConfigFileName */

/*
 * GetConfigFileName
 */
char *GetConfigFileName( void )
{
    return( cfgFN );

} /* GetConfigFileName */
#endif

void FiniCFName( void )
{
    MemFree( cfgFN );

} /* SetConfigFileName */

/*
 * checkFlags - check for command line flags
 */
static void checkFlags( int *argc, char *argv[], char *start[],
                        char *parms[], int *startcnt )
{
    int         ch;
    #ifndef __WIN__
    int         len;
    #endif

    cFN = NULL;
    while( 1 ) {
        #ifndef __WIN__
            ch = GetOpt( argc, argv, "#-ndvqzirc:k:p:s:t:", NULL );
        #else
            ch = GetOpt( argc, argv, "#-ndvqzirIP:c:k:p:s:t:", NULL );
        #endif
        if( ch == -1 ) {
            break;
        }
        switch( ch ) {
        case '#':
            #ifndef __WIN__
            strncpy( goCmd, OptArg, sizeof( goCmd ) -2 );
            goCmd[ sizeof( goCmd ) - 2 ] = 0;
            len = strlen( goCmd );
            goCmd[len] = 'G';
            goCmd[len+1] = 0;
            #else
                lineToGoTo = atoi( OptArg );
            #endif
            break;
        #ifdef __WIN__
            case 'P':
                SetInitialWindowSize( OptArg );
                break;
            case 'I':
                cFN = GetInitialFileName();
                break;
        #endif
        #ifdef __IDE__
            case 'X':
                IDEInit();
                EditFlags.UseIDE = TRUE;
                break;
        #endif
        case '-':
            EditFlags.StdIOMode = TRUE;
            EditFlags.NoInitialFileLoad = TRUE;
            break;
        case 'r':
            EditFlags.RecoverLostFiles = TRUE;
            EditFlags.NoInitialFileLoad = TRUE;
            break;
        case 'i':
            EditFlags.IgnoreLostFiles = TRUE;
            break;
        case 'z':
            EditFlags.IgnoreCtrlZ = TRUE;
            break;
        case 'q':
            EditFlags.Quiet = TRUE;
            break;
        case 'v':
            EditFlags.ViewOnly = TRUE;
            break;
        case 'c':
            EditFlags.BoundData = FALSE;
            AddString2( &cfgFN, OptArg );
            break;
        case 'd':
            EditFlags.BoundData = FALSE;
            AddString2( &cfgFN, "" );
            break;
        case 'k':
            keysToPush = OptArg;
            break;
        case 'p':
            if( *startcnt <= 0 ) {
                Quit( NULL, "No script to give parm list\n" );
            }
            parms[ (*startcnt-1)] = OptArg;
            break;
        case 's':
            if( *startcnt < MAX_STARTUP ) {
                start[ *startcnt ] = OptArg;
                parms[ *startcnt ] = NULL;
                (*startcnt)++;
            } else {
                Quit( NULL, "Too many scripts\n" );
            }
            break;
        case 't':
            cTag = OptArg;
            break;
        case 'n':
            wantNoReadEntireFile = TRUE;
            break;
        }
    }

    /*
     * now, check for null file name
     */
    if( cFN == NULL ) {
        if( (*argc)==1 ) {
            cFN = nullFN;
        } else {
//          cFN = argv[(*argc)-1];
            cFN = argv[1];
        }
    }

} /* checkFlags */

/*
 * doInitializeEditor - do just that
 */
static void doInitializeEditor( int argc, char *argv[] )
{
    int         i,rc,arg,cnt,ocnt,ln,startcnt=0;
    int         k,j;
    char        tmp[_MAX_PATH],c[1];
    char        buff[MAX_STR],file[MAX_STR],**list;
    char        cmd[MAX_STR*2];
    char        *parm;
    char        *startup[MAX_STARTUP];
    char        *startup_parms[MAX_STARTUP];

    /*
     * misc. set up
     */
    SpawnPrompt[ 0 ] = 0;
    MaxMemFree = MemSize();
    StaticStart();
    FTSInit();
    CheckForBoundData();
    EditFlags.Starting = TRUE;
    InitCommandLine();
    ChkExtendedKbd();
    SSInitBeforeConfig();

    GetCWD1( &HomeDirectory );
    GetCWD1( &CurrentDirectory );
    SetCWD( HomeDirectory );
    if( cfgFN == NULL ){
        AddString( &cfgFN, CFG_NAME );
    }

    checkFlags( &argc, argv, startup, startup_parms, &startcnt );
    ScreenInit();
    SetWindowSizes();
    EditFlags.ClockActive = FALSE;
    SetInterrupts();
#ifndef __WIN__
    InitColors();
#else
    InitClrPick();
    InitFtPick();
    SubclassGenericInit();
    CursorOp( COP_INIT );
#endif
    InitSavebufs();
    InitKeyMaps();

    /*
     * initial configuration
     */
    SetMajickString( NULL );
    FileEndString = MemStrDup( "[END_OF_FILE]" );

    MatchData[0] = MemStrDup( "{" );
    MatchData[1] = MemStrDup( "}" );
    MatchData[2] = MemStrDup( "\\(" );
    MatchData[3] = MemStrDup( "\\)" );

    if( cfgFN[0] != 0 ) {
        c[0] = 0;
        ln = 0;
        rc = Source( cfgFN, c, &ln );
        if( rc == ERR_FILE_NOT_FOUND ) {
            #ifdef __WIN__
                CloseStartupDialog();
                MessageBox( (HWND) NULL, "Could not locate configuration information; please make sure your EDPATH environment variable is set correctly",
                                        EditorName, MB_OK );
                ExitEditor( -1 );
            #else
                rc = ERR_NO_ERR;
            #endif
        }
    } else {
        rc = ERR_NO_ERR;
    }
    SetGadgetString( NULL );
    if( wantNoReadEntireFile ) {
        EditFlags.ReadEntireFile = FALSE;
    }
    VerifyTmpDir();
    while( LostFileCheck() );
    HookScriptCheck();

    if( EditFlags.Quiet ) {
        EditFlags.Spinning = EditFlags.Clock = FALSE;
    }
    ExtendedMemoryInit();

    /*
     * more misc. setup
     */
    if( WordDefn == NULL ) {
        AddString( &WordDefn, &WordDefnDefault[6] );
        InitWordSearch( WordDefn );
    }
    if( WordAltDefn == NULL ) {
        AddString( &WordAltDefn, WordDefnDefault );
    }
    if( TagFileName == NULL ) {
        AddString( &TagFileName, "tags" );
    }
    WorkLine = MemAlloc( LINE_SIZE + MaxLine+2 );
    DotBuffer = MemAlloc( ( maxdotbuffer + 2 ) * sizeof( vi_key ) );
    AltDotBuffer = MemAlloc( ( maxdotbuffer + 2 ) * sizeof( vi_key ) );
    DotCmd = MemAlloc( ( maxdotbuffer + 2 ) * sizeof( vi_key ) );
    WorkLine->len = -1;
    SwapBlockInit( MaxSwapBlocks );
    ReadBuffer = MemAlloc( MAX_IO_BUFFER+6 );
    WriteBuffer = MemAlloc( MAX_IO_BUFFER+6 );
    FindHistInit( FindHist.max );
    FilterHistInit( FilterHist.max );
    CLHistInit( CLHist.max );
    LastFilesHistInit( LastFilesHist.max );
    GetClockStart();
    GetSpinStart();
    SelRgnInit();
    SSInitAfterConfig();
    ViRCSInit();

    /*
     * create windows
     */
    StartWindows();
    InitMouse();
    i = NewMessageWindow();
    if( i ) {
        FatalError( i );
    }
    DoVersion();
    i = InitMenu();
    if( i ) {
        FatalError( i );
    }
    EditFlags.SpinningOurWheels = TRUE;
    EditFlags.ClockActive = TRUE;
    EditFlags.DisplayHold = TRUE;
    i = NewStatusWindow();
    if( i ) {
        FatalError( i );
    }
    EditFlags.DisplayHold = FALSE;
    MaxMemFreeAfterInit = MemSize();

    /*
     * start specified file(s)
     */
    arg = argc-1;
    k = 1;
    cmd[0] = 'e';
    cmd[1] = 0;
    EditFlags.WatchForBreak = TRUE;

    /*
     * look for a tag: if there is one, set it up as the file to start
     */
    if( cTag != NULL && !EditFlags.NoInitialFileLoad ) {
        #if defined( __NT__ ) && !defined( __WIN__ )
        {
            if( !EditFlags.Quiet ) {
                extern HANDLE OutputHandle;
                SetConsoleActiveScreenBuffer( OutputHandle );
            }
        }
        #endif
        i = LocateTag( cTag, file, buff );
        cFN = file;
        if( i ) {
            if( i == ERR_TAG_NOT_FOUND ) {
                Error( GetErrorMsg(i), cTag );
                ExitEditor( 0 );
            }
            FatalError( i );
        }
    }

    while( TRUE && !EditFlags.NoInitialFileLoad ) {

        if( cFN == nullFN && !EditFlags.UseNoName ) {
            break;
        }

        strcat( cmd, SingleBlank );
        strcat( cmd, cFN );
        ocnt = cnt = ExpandFileNames( cFN, &list );
        if( !cnt ) {
            cnt = 1;
        } else {
            cFN = list[0];
        }

        for( j=0;j<cnt;j++ ) {

            i = NewFile( cFN, FALSE );
            if( i && i != NEW_FILE ) {
                FatalError( i );
            }
            if( EditFlags.BreakPressed ) {
                break;
            }
            if( cnt > 0 && j < cnt-1 ) {
                cFN = list[j+1];
            }
        }
        if( ocnt > 0 ) {
            MemFreeList( ocnt, list );
        }
        if( EditFlags.BreakPressed ) {
            ClearBreak();
            break;
        }
        k++;
        arg--;
        if( cTag != NULL || arg < 1 ) {
            break;
        }
        cFN = argv[ k ];
    }
    if( EditFlags.StdIOMode ) {
        i = NewFile( "stdio", FALSE );
        if( i ) {
            FatalError( i );
        }
    }
    EditFlags.WatchForBreak = EditFlags.Starting = FALSE;

    /*
     * if there was a tag, do the appropriate search
     */
    if( cTag != NULL && !EditFlags.NoInitialFileLoad ) {
        if( buff[0] != '/' ) {
            i = atoi( buff );
            i = GoToLineNoRelCurs( i );
        } else {
            i = FindTag( buff );
        }
        if( i > 0 ) {
            Error( GetErrorMsg( i ) );
        }
    }

    /*
     * try to run startup file
     */
    if( EditFlags.RecoverLostFiles ) {
        startcnt = 0;
    }
    for( i=0;i<startcnt;i++ ) {
        GetFromEnv( startup[i], tmp );
        AddString2( &cfgFN, tmp );
        if( cfgFN[0] != 0 ) {
            if( startup_parms[i] != NULL ) {
                parm = startup_parms[i];
            } else {
                c[0] = 0;
                parm = c;
            }
            #if defined( __NT__ ) && !defined( __WIN__ )
            {
                if( !EditFlags.Quiet ) {
                    extern HANDLE OutputHandle;
                    SetConsoleActiveScreenBuffer( OutputHandle );
                }
            }
            #endif
            rc = Source( cfgFN, parm, &ln );
        }
    }
    if( rc > 0 ) {
        Error( "%s on line %d of \"%s\"", GetErrorMsg( rc ), ln, cfgFN );
    }
    if( argc == 1 ) {
        LoadHistory( NULL );
    } else {
        LoadHistory( cmd );
    }
    if( GrepDefault == NULL ) {
        AddString( &GrepDefault, "*.(c|h)" );
    }
    if( goCmd[0] != 0 ) {
        KeyAddString( goCmd );
    }
    if( keysToPush != NULL ) {
        KeyAddString( keysToPush );
    }
    #ifdef __WIN__
        if( lineToGoTo != 0 ) {
            SetCurrentLine( lineToGoTo );
            NewCursor( CurrentWindow, NormalCursorType );
        }
    #endif
    AutoSaveInit();
    HalfPageLines = WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_LINES )/2-1;
    #ifdef __386__
        VarAddGlobal( "OS386", "1" );
    #endif
    if( StatusString == NULL ) {
        AddString( &StatusString, "L:$6L$nC:$6C" );
    }
    UpdateStatusWindow();
    #ifdef __WIN__
        if( CurrentInfo == NULL ) {
            // no file loaded - screen is disconcertenly empty - reassure
            DisplayFileStatus();
        }
    #endif
    NewCursor( CurrentWindow, NormalCursorType );
    #if defined( __NT__ ) && !defined( __WIN__ )
    {
        extern HANDLE OutputHandle;
        SetConsoleActiveScreenBuffer( OutputHandle );
    }
    #endif

} /* doInitializeEditor */

/*
 * InitializeEditor - start up
 */
void InitializeEditor( void )
{
    extern int          _argc;
    extern char **      _argv;

    doInitializeEditor( _argc, _argv );

} /* InitializeEditor */
