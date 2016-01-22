/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Editor initialization.
*
****************************************************************************/


#include "vi.h"
#include "rxsupp.h"
#include "fcbmem.h"
#include "win.h"
#include "menu.h"
#include "getopt.h"
#include "sstyle.h"
#include "fts.h"
#ifdef __WIN__
    #include "subclass.h"
    #include "utils.h"
    #include "color.h"
#endif
#ifdef __NT__
    #include <windows.h>
    #include <fcntl.h>
    #include <io.h>
#endif
#include "rcs.h"
#include "autoenv.h"
#include "iopath.h"

#include "clibext.h"
#include "clibint.h"


static char     nullFN[] = "no_name";
static char     *cFN = NULL;
static char     *cfgFN = NULL;
static char     *cTag = NULL;
static char     *keysToPush = NULL;
#ifdef __WIN__
    static int  lineToGoTo = 0;
#endif
static char     goCmd[10] = {0};
static bool     wantNoReadEntireFile = false;
char            *WordDefnDefault = "::..\\\\__09AZaz";

#ifdef __WIN__
/*
 * SetConfigFileName
 */
void SetConfigFileName( const char *fn )
{
    ReplaceString( &cfgFN, fn );

} /* SetConfigFileName */

/*
 * GetConfigFileName
 */
char *GetConfigFileName( void )
{
    return( cfgFN );

} /* GetConfigFileName */
#endif

void FiniConfigFileName( void )
{
    MemFree( cfgFN );

} /* FiniConfigFileName */

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

    for( ;; ) {
#ifdef __WIN__
        ch = GetOpt( argc, argv, "#-ndvqzirIP:c:k:p:s:t:", NULL );
#else
        ch = GetOpt( argc, argv, "#-ndvqzirc:k:p:s:t:", NULL );
#endif
        if( ch == -1 ) {
            break;
        }
        switch( ch ) {
        case '#':
#ifdef __WIN__
            lineToGoTo = atoi( OptArg );
#else
            strncpy( goCmd, OptArg, sizeof( goCmd ) -2 );
            goCmd[sizeof( goCmd ) - 2] = 0;
            len = strlen( goCmd );
            goCmd[len] = 'G';
            goCmd[len + 1] = 0;
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
            EditFlags.UseIDE = true;
            break;
#endif
        case '-':
            EditFlags.StdIOMode = true;
            EditFlags.NoInitialFileLoad = true;
            break;
        case 'r':
            EditFlags.RecoverLostFiles = true;
            EditFlags.NoInitialFileLoad = true;
            break;
        case 'i':
            EditFlags.IgnoreLostFiles = true;
            break;
        case 'z':
            EditFlags.IgnoreCtrlZ = true;
            break;
        case 'q':
            EditFlags.Quiet = true;
            break;
        case 'v':
            EditFlags.ViewOnly = true;
            break;
        case 'c':
            EditFlags.BoundData = false;
            ReplaceString( &cfgFN, OptArg );
            break;
        case 'd':
            EditFlags.BoundData = false;
            ReplaceString( &cfgFN, "" );
            break;
        case 'k':
            keysToPush = OptArg;
            break;
        case 'p':
            if( *startcnt <= 0 ) {
                Quit( NULL, "No script to give parm list\n" );
            }
            parms[(*startcnt - 1)] = OptArg;
            break;
        case 's':
            if( *startcnt < MAX_STARTUP ) {
                start[*startcnt] = OptArg;
                parms[*startcnt] = NULL;
                (*startcnt)++;
            } else {
                Quit( NULL, "Too many scripts\n" );
            }
            break;
        case 't':
            cTag = OptArg;
            break;
        case 'n':
            wantNoReadEntireFile = true;
            break;
        }
    }

    /*
     * now, check for null file name
     */
    if( cFN == NULL ) {
        if( (*argc) == 1 ) {
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
    int         i, arg, cnt, ocnt, startcnt = 0;
    srcline     sline;
    int         k, j;
    char        tmp[FILENAME_MAX], c[1];
    char        buff[MAX_STR], file[MAX_STR], **list;
    char        cmd[MAX_STR * 2];
    char        *parm;
    char        *startup[MAX_STARTUP];
    char        *startup_parms[MAX_STARTUP];
    vi_rc       rc;
    vi_rc       rc1;

    /*
     * Make sure WATCOM is setup and if it is not, make a best guess.
     */
    watcom_setup_env();

    /*
     * If EDPATH is not set, use system default %WATCOM%\EDDAT.
     */
    if( getenv( "EDPATH" ) == NULL ) {
        char *watcom;

        watcom = getenv( "WATCOM" );
        if( watcom != NULL ) {
            char edpath[FILENAME_MAX];

            sprintf( edpath, "%s%c%s", watcom, FILE_SEP, "eddat" );

            if( setenv( "EDPATH", edpath, 0 ) != 0 ) {
                /*
                 * Bail out silently on error, as we will get error message later on.
                 */
            }
        }
    }

    /*
     * misc. set up
     */
    MaxMemFree = MemSize();
    StaticStart();
    FTSInit();
    BoundDataInit();
    EditFlags.Starting = true;
    InitCommandLine();
    ChkExtendedKbd();
    SSInitBeforeConfig();

    GetCWD1( &HomeDirectory );
    GetCWD1( &CurrentDirectory );
    SetCWD( HomeDirectory );
    if( cfgFN == NULL ){
        cfgFN = DupString( CFG_NAME );
    }

    checkFlags( &argc, argv, startup, startup_parms, &startcnt );
    ScreenInit();
    SetWindowSizes();
    EditFlags.ClockActive = false;
    SetInterrupts();
#ifdef __WIN__
    InitClrPick();
    InitFtPick();
    SubclassGenericInit();
    CursorOp( COP_INIT );
#else
    InitColors();
#endif
    InitSavebufs();
    InitKeyMaps();

    /*
     * initial configuration
     */
    EditVars.Majick = MemStrDup( "()~@" );
    EditVars.FileEndString = MemStrDup( "[END_OF_FILE]" );
    MatchInit();
    SetGadgetString( NULL );
    WorkLine = MemAlloc( sizeof( line ) + EditVars.MaxLine + 2 );
    WorkLine->len = -1;

    sline = 0;
    if( cfgFN[0] != 0 ) {
        c[0] = 0;
        rc = Source( cfgFN, c, &sline );
        if( rc == ERR_FILE_NOT_FOUND ) {
#ifdef __WIN__
            CloseStartupDialog();
            MessageBox( NO_WINDOW, "Could not locate configuration information; please make sure your EDPATH environment variable is set correctly",
                        EditorName, MB_OK );
            ExitEditor( -1 );
#else
            rc = ERR_NO_ERR;
#endif
        }
    } else {
        rc = ERR_NO_ERR;
    }
    if( wantNoReadEntireFile ) {
        EditFlags.ReadEntireFile = false;
    }
    VerifyTmpDir();
    while( LostFileCheck() );
    HookScriptCheck();

    if( EditFlags.Quiet ) {
        EditFlags.Spinning = false;
        EditFlags.Clock = false;
    }
    ExtendedMemoryInit();

    /*
     * more misc. setup
     */
    if( EditVars.WordDefn == NULL ) {
        EditVars.WordDefn = DupString( &WordDefnDefault[6] );
        InitWordSearch( EditVars.WordDefn );
    }
    if( EditVars.WordAltDefn == NULL ) {
        EditVars.WordAltDefn = DupString( WordDefnDefault );
    }
    if( EditVars.TagFileName == NULL ) {
        EditVars.TagFileName = DupString( "tags" );
    }
    DotBuffer = MemAlloc( (maxdotbuffer + 2) * sizeof( vi_key ) );
    AltDotBuffer = MemAlloc( (maxdotbuffer + 2) * sizeof( vi_key ) );
    DotCmd = MemAlloc( (maxdotbuffer + 2) * sizeof( vi_key ) );
    SwapBlockInit( EditVars.MaxSwapBlocks );
    ReadBuffer = MemAlloc( MAX_IO_BUFFER + 6 );
    WriteBuffer = MemAlloc( MAX_IO_BUFFER + 6 );
    FindHistInit( EditVars.FindHist.max );
    FilterHistInit( EditVars.FilterHist.max );
    CLHistInit( EditVars.CLHist.max );
    LastFilesHistInit( EditVars.LastFilesHist.max );
    GetClockStart();
    GetSpinStart();
    SelRgnInit();
    SSInitAfterConfig();
#if defined( VI_RCS )
    ViRCSInit();
#endif

    /*
     * create windows
     */
    StartWindows();
    InitMouse();
    rc1 = NewMessageWindow();
    if( rc1 != ERR_NO_ERR ) {
        FatalError( rc1 );
    }
    DoVersion();
    rc1 = InitMenu();
    if( rc1 != ERR_NO_ERR ) {
        FatalError( rc1 );
    }
    EditFlags.SpinningOurWheels = true;
    EditFlags.ClockActive = true;
    EditFlags.DisplayHold = true;
    rc1 = NewStatusWindow();
    if( rc1 != ERR_NO_ERR ) {
        FatalError( rc1 );
    }
    EditFlags.DisplayHold = false;

    MaxMemFreeAfterInit = MemSize();

    /*
     * look for a tag: if there is one, set it up as the file to start
     */
    EditFlags.WatchForBreak = true;
    if( cTag != NULL && !EditFlags.NoInitialFileLoad ) {
#if defined( __NT__ ) && !defined( __WIN__ )
        {
            if( !EditFlags.Quiet ) {
                SetConsoleActiveScreenBuffer( OutputHandle );
            }
        }
#endif
        rc1 = LocateTag( cTag, file, buff );
        cFN = file;
        if( rc1 != ERR_NO_ERR ) {
            if( rc1 == ERR_TAG_NOT_FOUND ) {
                Error( GetErrorMsg( rc1 ), cTag );
                ExitEditor( 0 );
            }
            FatalError( rc1 );
        }
    }

    /*
     * start specified file(s)
     */
    cmd[0] = 'e';
    cmd[1] = 0;

    arg = argc - 1;
    k = 1;
    while( !EditFlags.NoInitialFileLoad ) {

        if( cFN == nullFN && !EditFlags.UseNoName ) {
            break;
        }

#ifdef __NT__
        {
            int     k2;
            int     arg2;
            char    path[_MAX_PATH];
            int     found;
            int     fd;
            size_t  len;
            size_t  len1;
            char    *p;

            /*
             * check for the existence of a file name containing spaces, and open it if
             * there is one
             */
            len = _MAX_PATH - 1;
            found = 0;
            p = path;
            arg2 = arg;
            for( k2 = k; argv[k2] != NULL; ) {
                len1 = strlen( argv[k2] );
                if( len1 > len )
                    break;
                memcpy( p, argv[k2], len1 );
                p += len1;
                *p = '\0';
                len -= len1;
                --arg2;
                ++k2;
                fd = open( path, O_RDONLY );
                if( fd != -1 ) {
                    close( fd );
                    k = k2;
                    arg = arg2;
                    found = 1;
                    break;
                }
                *p++ = ' ';
            }
            if( found ) {
#ifndef __UNIX__
                len1 = strlen( path );
                if( path[len1 - 1] == '.' )
                    path[len1 - 1] = '\0';
#endif
                rc1 = NewFile( path, false );
                if( rc1 != ERR_NO_ERR ) {
                    FatalError( rc1 );
                }
                cFN = argv[k];
                if( arg < 1 ) {
                    break;
                }
                continue;
            }
        }
#endif

        strcat( cmd, SingleBlank );
        strcat( cmd, cFN );
        ocnt = cnt = ExpandFileNames( cFN, &list );
        if( cnt == 0 ) {
            cnt = 1;
        } else {
            cFN = list[0];
        }

        for( j = 0; j < cnt; j++ ) {
            rc1 = NewFile( cFN, false );
            if( rc1 != ERR_NO_ERR && rc1 != NEW_FILE ) {
                FatalError( rc1 );
            }
            if( EditFlags.BreakPressed ) {
                break;
            }
            if( cnt > 0 && j < cnt - 1 ) {
                cFN = list[j + 1];
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
        cFN = argv[k];
    }
    if( EditFlags.StdIOMode ) {
        rc1 = NewFile( "stdio", false );
        if( rc1 != ERR_NO_ERR ) {
            FatalError( rc1 );
        }
    }
    EditFlags.WatchForBreak = false;
    EditFlags.Starting = false;

    /*
     * if there was a tag, do the appropriate search
     */
    if( cTag != NULL && !EditFlags.NoInitialFileLoad ) {
        if( buff[0] != '/' ) {
            i = atoi( buff );
            rc1 = GoToLineNoRelCurs( i );
        } else {
            rc1 = FindTag( buff );
        }
        if( rc1 > 0 ) {
            Error( GetErrorMsg( rc1 ) );
        }
    }

    /*
     * try to run startup file
     */
    if( EditFlags.RecoverLostFiles ) {
        startcnt = 0;
    }
    for( i = 0; i < startcnt; i++ ) {
        GetFromEnv( startup[i], tmp );
        ReplaceString( &cfgFN, tmp );
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
                    SetConsoleActiveScreenBuffer( OutputHandle );
                }
            }
#endif
            sline = 0;
            rc = Source( cfgFN, parm, &sline );
        }
    }
    if( rc > ERR_NO_ERR ) {
        Error( "%s on line %u of \"%s\"", GetErrorMsg( rc ), sline, cfgFN );
    }
    if( argc == 1 ) {
        LoadHistory( NULL );
    } else {
        LoadHistory( cmd );
    }
    if( EditVars.GrepDefault == NULL ) {
        EditVars.GrepDefault = DupString( "*.(c|h)" );
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
        NewCursor( current_window_id, EditVars.NormalCursorType );
    }
#endif
    AutoSaveInit();
    HalfPageLines = WindowAuxInfo( current_window_id, WIND_INFO_TEXT_LINES ) / 2 - 1;
#if defined( _M_X64 )
    VarAddGlobalStr( "OSX64", "1" );
#elif defined( _M_IX86 ) && !defined( _M_I86 )
    VarAddGlobalStr( "OS386", "1" );
#endif
    if( EditVars.StatusString == NULL ) {
        EditVars.StatusString = DupString( "L:$6L$nC:$6C" );
    }
    UpdateStatusWindow();
#ifdef __WIN__
    if( CurrentInfo == NULL ) {
        // no file loaded - screen is disconcertenly empty - reassure
        DisplayFileStatus();
    }
#endif
    NewCursor( current_window_id, EditVars.NormalCursorType );
#if defined( __NT__ ) && !defined( __WIN__ )
    {
        SetConsoleActiveScreenBuffer( OutputHandle );
    }
#endif

} /* doInitializeEditor */

/*
 * InitializeEditor - start up
 */
void InitializeEditor( void )
{
#if !defined( __WATCOMC__ ) && defined( __NT__ )
    _argc = __argc;
    _argv = __argv;
#endif
    doInitializeEditor( _argc, _argv );

} /* InitializeEditor */
