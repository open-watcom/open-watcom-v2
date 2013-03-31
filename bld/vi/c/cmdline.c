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
* Description:  Editor command line processing.
*
****************************************************************************/


#include "vi.h"
#include <time.h>
#include <setjmp.h>
#ifdef _M_I86
    #include <i86.h>
#endif
#include "parsecl.h"
#include "source.h"
#include "rxsupp.h"
#include "ex.h"
#include "win.h"
#include "expr.h"
#include "menu.h"
#include "source.h"
#include "sstyle.h"
#include "fts.h"
#include "rcscli.h"

#if defined( __WINDOWS__ ) && defined( VI_RCS )
static bool isOS2( void )
{
    union {
        DWORD   dVersion;
        struct {
            char    winMajor;
            char    winMinor;
            char    dosMinor;
            char    dosMajor;
        } v;
    } v;
    v.dVersion = GetVersion();
    if( v.v.dosMajor >= 20 ) {
        return( TRUE );
    }
    return( FALSE );
}
#endif

static window_info  *wInfo = NULL;
static char         strLoad[] = "loaded";
static char         strCompile[] = "compiled";
static char         *dataBuff;

static vi_rc        setWDimension( char * );
static vi_rc        setWHilite( char * );
static vi_rc        setWText( char * );
static vi_rc        setWBorder( char * );
static vi_rc        setSyntaxStyle( syntax_element, char * );

/*
 * InitCommandLine - initialize command line processing
 */
void InitCommandLine( void )
{
    dataBuff = MemAlloc( MaxLine );

} /* InitCommandLine */

/*
 * FiniCommandLine - free memory
 */
void FiniCommandLine( void )
{
    MemFree( dataBuff );

} /* FiniCommandLine */

/*
 * doProcessCommandLine - handle getting and processing a command line
 */
static vi_rc doProcessCommandLine( bool is_fancy )
{
    vi_rc       rc;
    char        *st;

    /*
     * open the window and get the string
     */
    st = MemAllocUnsafe( MaxLine );
    if( st == NULL ) {
        return( ERR_NO_MEMORY );
    }
    is_fancy = is_fancy;
#ifdef __WIN__
    if( is_fancy ) {
        if( !GetCmdDialog( st, MaxLine ) ) {
            MemFree( st );
            return( ERR_NO_ERR );
        }
    } else {
#endif
        rc = PromptForString( ":", st, MaxLine, &CLHist );
        if( rc != ERR_NO_ERR ) {
            MemFree( st );
            if( rc == NO_VALUE_ENTERED ) {
                return( ERR_NO_ERR );
            }
            return( rc );
        }
#ifdef __WIN__
    }
#endif
    CommandBuffer = st;
    rc = SourceHook( SRC_HOOK_COMMAND, ERR_NO_ERR );
    if( rc == ERR_NO_ERR ) {
        rc = RunCommandLine( st );
    }
    CommandBuffer = NULL;
    MemFree( st );
    return( rc );

} /* doProcessCommandLine */

/*
 * ProcessCommandLine - do just that
 */
vi_rc ProcessCommandLine( void )
{
    return( doProcessCommandLine( FALSE ) );

} /* ProcessCommandLine */

/*
 * FancyProcessCommandLine - do just that
 */
vi_rc FancyProcessCommandLine( void )
{
    return( doProcessCommandLine( TRUE ) );

} /* FancyProcessCommandLine */

/*
 * TryCompileableToken - process token that can also be compiled
 */
vi_rc TryCompileableToken( int token, char *data, bool iscmdline,
                         bool dmt )
{
    vi_rc       rc = ERR_INVALID_COMMAND;
    bool        mflag;
    char        *tmp;

    switch( token ) {
    case PCL_T_MAPBASE:
    case PCL_T_MAP:
        if( iscmdline ) {
            mflag = MAPFLAG_MESSAGE;
        } else {
            mflag = 0;
        }
        if( token == PCL_T_MAPBASE ) {
            mflag |= MAPFLAG_BASE;
        }
        tmp = data;
        if( iscmdline ) {
            if( dmt ) {
                mflag |= MAPFLAG_DAMMIT;
            }
        } else {
            if( data[0] == '!' ) {
                mflag |= MAPFLAG_DAMMIT;
                tmp = &data[1];
            }
        }
        rc = MapKey( mflag, tmp );
        break;
    case PCL_T_MENUFILELIST:
        rc = MenuItemFileList();
        break;
    case PCL_T_MENULASTFILES:
        rc = MenuItemLastFiles();
        break;
    case PCL_T_MENU:
        rc = StartMenu( data );
        break;
    case PCL_T_MENUITEM:
        rc = MenuItem( data );
        break;
    case PCL_T_FLOATMENU:
        rc = ActivateFloatMenu( data );
        break;
    case PCL_T_ENDMENU:
        rc = ViEndMenu();
        break;
    case PCL_T_FILETYPESOURCE:
        rc = FTSStart( data );
        break;
    case PCL_T_ADDTOOLBARITEM:
#ifdef __WIN__
        rc = AddBitmapToToolBar( data );
#endif
        break;
    case PCL_T_DELETETOOLBARITEM:
#ifdef __WIN__
        rc = DeleteFromToolBar( data );
#endif
        break;
    case PCL_T_ACTIVATE_IDE:
#ifdef __WIN__
        StartIDE( InstanceHandle, TRUE );
        rc = ERR_NO_ERR;
#endif
        break;
    case PCL_T_ALIAS:
        rc = SetAlias( data );
        break;
    case PCL_T_ABBREV:
        rc = Abbrev( data );
        break;
    case PCL_T_SET:
        if( iscmdline ) {
            Expand( data, NULL );
        }
        rc = Set( data );
        break;
    case PCL_T_SETCOLOR:
        rc = SetAColor( data );
        break;
    case PCL_T_SETFONT:
        rc = SetFont( data );
        break;
    case PCL_T_MATCH:
        rc = AddMatchString( data );
        break;
    case PCL_T_LOCATE:
        rc = LocateCmd( data );
        break;
    default:
        rc = NOT_COMPILEABLE_TOKEN;
        break;
    }
    return( rc );

} /* TryCompileableToken */

/*
 * RunCommandLine - run a command line command
 */
vi_rc RunCommandLine( char *cl )
{
    int         i, x, y, x2, y2;
    int         n2f, n1f, dmt, tkn, flag;
    bool        test1;
    linenum     n1, n2;
    char        st[FILENAME_MAX];
    info        *cinfo;
    long        val;
    jmp_buf     jmpaddr;
    vi_rc       rc;

    /*
     * parse command string
     */
    rc = ParseCommandLine( cl, &n1, &n1f, &n2, &n2f, &tkn, dataBuff, &dmt );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    if( !n2f ) {
        if( !n1f ) {
            n1 = n2 = CurrentPos.line;
        } else {
            n2 = n1;
        }
    }

    /*
     * process tokens
     */
    rc = ERR_INVALID_COMMAND;
    test1 = n1f || n2f;
    switch( tkn ) {
    case PCL_T_ABOUT:
        rc = DoAboutBox();
        break;
    case PCL_T_PUSH:
        rc = PushFileStackAndMsg();
        break;
    case PCL_T_POP:
        rc = PopFileStack();
        break;
    case PCL_T_EXECUTE:
        RemoveLeadingSpaces( dataBuff );
        if( *dataBuff != '\0' ) {
            key_map     scr;

            rc = AddKeyMap( &scr, dataBuff );
            if( rc != ERR_NO_ERR ) {
                break;
            }
            rc = RunKeyMap( &scr, 1L );
            MemFree( scr.data );
        }
        break;

    case PCL_T_DELETEMENU:
        rc = DoMenuDelete( dataBuff );
        break;
    case PCL_T_DELETEMENUITEM:
        rc = DoItemDelete( dataBuff );
        break;
    case PCL_T_ADDMENUITEM:
        rc = AddMenuItem( dataBuff );
        break;
    case PCL_T_MAXIMIZE:
        rc = MaximizeCurrentWindow();
        break;
    case PCL_T_MINIMIZE:
        rc = MinimizeCurrentWindow();
        break;
    case PCL_T_EXITFILESONLY:
        if( ExitWithPrompt( FALSE ) == FALSE ) {
            rc = ERR_EXIT_ABORTED;
        } else {
            rc = ERR_NO_ERR;
        }
        break;
    case PCL_T_EXITALL:
        if( ExitWithPrompt( TRUE ) == FALSE ) {
            rc = ERR_EXIT_ABORTED;
        } else {
            rc = ERR_NO_ERR;
        }
        break;
    case PCL_T_QUITALL:
        ExitWithVerify();
        rc = ERR_NO_ERR;
        break;
    case PCL_T_KEYADD:
        RemoveLeadingSpaces( dataBuff );
        KeyAddString( dataBuff );
        rc = ERR_NO_ERR;
        break;

    case PCL_T_UNALIAS:
        rc = UnAlias( dataBuff );
        break;

    case PCL_T_UNABBREV:
        rc = UnAbbrev( dataBuff );
        break;

    case PCL_T_UNMAP:
        flag = MAPFLAG_MESSAGE + MAPFLAG_UNMAP;
        if( dmt ) {
            flag |= MAPFLAG_DAMMIT;
        }
        rc = MapKey( flag, dataBuff );
        break;

    case PCL_T_EVAL:
        Expand( dataBuff, NULL );
        i = setjmp( jmpaddr );
        if( i != 0 ) {
            rc = (vi_rc)i;
        } else {
            StartExprParse( dataBuff, jmpaddr );
            val = GetConstExpr();
            ltoa( val, st, Radix );
            Message1( "%s", st );
            rc = ERR_NO_ERR;
        }
        break;

    case PCL_T_COMPILE:
    case PCL_T_SOURCE:
    case PCL_T_LOAD:
        {
            char *tstr;

            if( NextWord1( dataBuff, st ) <= 0 ) {
                rc = ERR_NO_FILE_SPECIFIED;
                break;
            }

            if( tkn == PCL_T_COMPILE ) {
                EditFlags.CompileScript = TRUE;
                if( st[0] == '-' ) {
                    if( st[1] == 'a' || st[1] == 'A' ) {
                        EditFlags.CompileAssignments = TRUE;
                        if( st[1] == 'A' ) {
                            EditFlags.CompileAssignmentsDammit = TRUE;
                        }
                        if( NextWord1( dataBuff, st) <= 0 ) {
                            rc = ERR_NO_FILE_SPECIFIED;
                            break;
                        }
                    }
                }
            }
            if( tkn == PCL_T_LOAD ) {
                EditFlags.LoadResidentScript = TRUE;
            }

            rc = Source( st, dataBuff, &i );

            EditFlags.LoadResidentScript = FALSE;
            EditFlags.CompileScript = FALSE;
            EditFlags.CompileAssignments = FALSE;
            EditFlags.CompileAssignmentsDammit = FALSE;
            if( EditFlags.SourceScriptActive ) {
                LastError = rc;
            }
            if( rc > ERR_NO_ERR ) {
                Error( "%s on line %d of \"%s\"", GetErrorMsg( rc ), i, st );
            } else {
                if( rc != DO_NOT_CLEAR_MESSAGE_WINDOW ) {
                    if( tkn != PCL_T_SOURCE ) {
                        if( tkn == PCL_T_LOAD ) {
                            tstr = strLoad;
                        } else {
                            tstr = strCompile;
                        }
                        Message1( "Script \"%s\" %s, %d lines generated, %d errors",
                                        st, tstr, i, SourceErrCount );
                        rc = DO_NOT_CLEAR_MESSAGE_WINDOW;
                    }
                }
            }
            break;
        }

    case PCL_T_GENCONFIG:
#ifndef __WIN__
        if( NextWord1( dataBuff,st ) >= 0 ) {
            rc = GenerateConfiguration( st, TRUE );
        } else {
            rc = GenerateConfiguration( NULL, TRUE );
        }
#else
        {
            int temp = EditFlags.SaveConfig;
            EditFlags.SaveConfig = TRUE;
            WriteProfile();
            EditFlags.SaveConfig = temp;
            rc = ERR_NO_ERR;
        }
#endif
        break;

    case PCL_T_COMPRESS:
        rc = CompressWhiteSpace();
        break;

    case PCL_T_EXPAND:
        rc = ExpandWhiteSpace();
        break;

    case PCL_T_SHOVE:
        rc = Shift( n1, n2, '>', TRUE );
        break;

    case PCL_T_SUCK:
        rc = Shift( n1, n2, '<', TRUE );
        break;

    case PCL_T_FILES:
        if( EditFlags.LineDisplay ) {
            rc = DisplayFileStatus();
        } else {
            rc = EditFileFromList();
        }
        break;

    case PCL_T_NEXT:
        rc = RotateFileForward();
        break;

    case PCL_T_PREV:
        rc = RotateFileBackwards();
        break;

    case PCL_T_HELP:
        rc = DoHelp( dataBuff );
        break;

    case PCL_T_VIEW:
        EditFlags.ViewOnly = TRUE;
    case PCL_T_EDIT:
        rc = EditFile( dataBuff, dmt );
        EditFlags.ViewOnly = FALSE;
        break;
    case PCL_T_OPEN:
        rc = OpenWindowOnFile( dataBuff );
        break;

    case PCL_T_HIDE:
        rc = HideLineRange( n1, n2, dmt );
        break;

    case PCL_T_DELETE:
        rc = SetSavebufNumber( dataBuff );
        if( rc != ERR_NO_ERR ) {
            break;
        }
        if( SelRgn.selected && !EditFlags.LineBased ) {
            AddSelRgnToSavebufAndDelete();
            rc = ERR_NO_ERR;
            // @ may have turned this on - it is now definitely off
            SelRgn.selected = FALSE;
        } else {
            rc = DeleteLineRange( n1, n2, SAVEBUF_FLAG );
        }
        if( rc == ERR_NO_ERR ) {
            DCDisplayAllLines();
            LineDeleteMessage( n1, n2 );
        }
        break;

    case PCL_T_SAVEANDEXIT:
        if( NextWord1( dataBuff, st ) >= 0 ) {
            rc = SaveAndExit( st );
        } else {
            rc = SaveAndExit( NULL );
        }
        break;

    case PCL_T_PUT:
        rc = SetSavebufNumber( dataBuff );
        if( rc != ERR_NO_ERR ) {
            break;
        }
        rc = SaveAndResetFilePos( n1 );
        if( rc == ERR_NO_ERR ) {
            if( !dmt ) {
                rc = InsertSavebufAfter();
            } else {
                rc = InsertSavebufBefore();
            }
            RestoreCurrentFilePos();
        }
        break;

    case PCL_T_YANK:
        rc = SetSavebufNumber( dataBuff );
        if( rc != ERR_NO_ERR ) {
            break;
        }
        if( SelRgn.selected && !EditFlags.LineBased ) {
            rc = YankSelectedRegion();
            // @ may have turned this on - it is now definitely off
            SelRgn.selected = FALSE;
        } else {
            rc = YankLineRange( n1, n2 );
        }
        break;

    case PCL_T_SUBSTITUTE:
        RemoveLeadingSpaces( dataBuff );
        rc = Substitute( n1, n2, dataBuff );
        break;

    case PCL_T_GLOBAL:
        if( !test1 ) {
            n1 = 1;
            rc = CFindLastLine( &n2 );
            if( rc != ERR_NO_ERR ) {
                break;
            }
        }
        rc = Global( n1,n2, dataBuff, dmt );
        break;

    case PCL_T_WRITEQUIT:
        if( CurrentFile == NULL ) {
            rc = NextFile();
        } else {
            CurrentFile->modified = TRUE;
            if( NextWord1( dataBuff, st ) >= 0 ) {
                rc = SaveAndExit( st );
            } else {
                rc = SaveAndExit( NULL );
            }
        }
        break;

    case PCL_T_WRITE:
        if( test1 ) {
            if( NextWord1( dataBuff, st ) <= 0 ) {
                rc = ERR_NO_FILE_SPECIFIED;
            } else {
                rc = SaveFile( st, n1, n2, dmt );
            }
        } else {
            if( NextWord1( dataBuff, st ) >= 0 ) {
#ifdef __WIN__
                if( st[0] == '?' && st[1] == '\0' ) {
                    rc = SaveFileAs();
                    break;
                } else {
                    rc = SaveFile( st, -1, -1, dmt );
                }
#else
                rc = SaveFile( st, -1, -1, dmt );
#endif
            } else {
                rc = SaveFile( NULL, -1, -1, dmt );
                if( rc == ERR_NO_ERR ) {
                    Modified( FALSE );
                }
            }
        }
        break;

    case PCL_T_READ:
        rc = ReadAFile( n1, dataBuff );
        break;

    case PCL_T_QUIT:
        if( dmt ) {
            rc = NextFileDammit();
        } else {
#ifdef __WIN__
            rc = CurFileExitOptionSaveChanges();
#else
            rc = NextFile();
#endif
        }
        break;

    case PCL_T_DATE:
        GetDateTimeString( st );
        Message1( st );
        rc = DO_NOT_CLEAR_MESSAGE_WINDOW;
        break;

    case PCL_T_CD:
        if( NextWord1( dataBuff, st ) > 0 ) {
            rc = SetCWD( st );
        } else {
            rc = ERR_NO_ERR;
        }
        if( rc == ERR_NO_ERR ) {
            Message1( "Current directory is %s",CurrentDirectory );
        }
        break;

    case PCL_T_SHELL:
    EVIL_SHELL:
        {
#if defined( __NT__ ) && !defined( __WIN__ )
            ExecCmd( NULL, NULL, NULL );
#else
            char foo[FILENAME_MAX];

            strcpy( foo, Comspec );
            ExecCmd( NULL, NULL, foo );
#endif
            DoVersion();
            rc = ERR_NO_ERR;
        }
        break;

    case PCL_T_SYSTEM:
        if( n1f && n2f ) {
            rc = DoGenericFilter( n1, n2, dataBuff );
        } else {
            RemoveLeadingSpaces( dataBuff );
            if( dataBuff[0] == 0 ) {
                goto EVIL_SHELL;
            }
            ExecCmd( NULL, NULL, dataBuff );
            rc = ERR_NO_ERR;
        }
        break;

    case PCL_T_RESIZE:
        rc = ResizeCurrentWindowWithKeys();
        break;

    case PCL_T_TILE:
        if( NextWord1( dataBuff, st ) > 0 ) {
            if( st[0] == 'v' ) {
                y = 1;
                for( x = 0, cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
                    x++;
                }
            } else if( st[0] == 'h' ) {
                x = 1;
                for( y = 0, cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
                    y++;
                }
            } else {
                x = atoi( st );
                if( NextWord1( dataBuff, st ) < 0 ) {
                    break;
                } else {
                    y = atoi( st );
                }
            }
        } else {
            x = MaxWindowTileX;
            y = MaxWindowTileY;
        }
        if( x > 0 && y > 0) {
            rc = WindowTile( x, y );
        }
        break;

    case PCL_T_CASCADE:
        rc = WindowCascade();
        break;

    case PCL_T_MOVEWIN:
        rc = MoveCurrentWindowWithKeys();
        break;

    case PCL_T_TAG:
        if( NextWord1( dataBuff, st ) > 0 ) {
            rc = TagHunt( st );
        }
        break;

    case PCL_T_FGREP:
        {
            bool        ci;

            RemoveLeadingSpaces( dataBuff );
            ci = EditFlags.CaseIgnore;
            if( dataBuff[0] == '-' ) {
                if( dataBuff[1] == 'c' ) {
                    ci = FALSE;
                    EliminateFirstN( dataBuff, 2 );
                    RemoveLeadingSpaces( dataBuff );
                    rc = GetStringWithPossibleQuote( dataBuff, st );
                } else if( dataBuff[1] == 'i' ) {
                    ci = TRUE;
                    EliminateFirstN( dataBuff, 2 );
                    RemoveLeadingSpaces( dataBuff );
                    rc = GetStringWithPossibleQuote( dataBuff, st );
                } else if( dataBuff[1] == 'f' ) {
                    EliminateFirstN( dataBuff, 2 );
                    RemoveLeadingSpaces( dataBuff );
#ifdef __WIN__
                    // call fancy grep window
                    {
                        fancy_find      *ff;
                        /* ff will be set to point at a static fancy find struct
                         * in the snoop module */
                        char snoopbuf[FILENAME_MAX];

                        if( !GetSnoopStringDialog( &ff ) ) {
                            return( ERR_NO_ERR );
                        }

                        strcpy( snoopbuf, ff->path );
                        /* assume no string means current directory */
                        if( strlen( snoopbuf ) &&
                            snoopbuf[strlen( snoopbuf ) - 1] != '\\' ){
                            strcat( snoopbuf, "\\" );
                        }
                        MySprintf( st, "%s", ff->find );
                        strcat( snoopbuf, ff->ext );
                        ci = ff->case_ignore;
                        if( !ff->use_regexp ) {
                            //MakeExpressionNonRegular( st );
                            rc = DoFGREP( snoopbuf, st, ci );
                        } else {
                            rc = DoEGREP( snoopbuf, st );
                        }
                        break;
                    }
#endif
                }
            } else {
                rc = GetStringWithPossibleQuote( dataBuff, st );
            }
            if( rc != ERR_NO_STRING ) {
                rc = DoFGREP( dataBuff, st, ci );
            }
        }
        break;

    case PCL_T_EGREP:
        rc = GetStringWithPossibleQuote( dataBuff, st );
        if( rc != ERR_NO_STRING ) {
            rc = DoEGREP( dataBuff, st );
        }
        break;

    case PCL_T_SIZE:
        if( NextWord1( dataBuff, st ) <= 0 ) {
            break;
        }
        x = atoi( st );
        if( NextWord1( dataBuff, st ) <= 0 ) {
            break;
        }
        y = atoi( st );
        if( NextWord1( dataBuff, st ) <= 0 ) {
            break;
        }
        x2 = atoi( st );
        if( NextWord1( dataBuff, st ) <= 0 ) {
            break;
        }
        y2 = atoi( st );
        rc = CurrentWindowResize( x, y, x2, y2 );
        break;

    case PCL_T_ECHO:
        if( NextWord1( dataBuff, st ) <= 0 ) {
            break;
        }
        rc = ERR_NO_ERR;
        if( !stricmp( st, "on" ) ) {
            EditFlags.EchoOn = TRUE;
            break;
        } else if( !stricmp( st, "off" ) ) {
            EditFlags.EchoOn = FALSE;
            break;
        }
        x = atoi( st );
        RemoveLeadingSpaces( dataBuff );
        /*
         * FIXME: This is not good - I will definately have to
         * fix this code up. But right now I have to get the
         * editor ready for tomorrow. Brad.
         */
        if( dataBuff[0] == '"' || dataBuff[0] == '/' ) {
            GetStringWithPossibleQuote( dataBuff, st );
            if( x > 2 ) {
                /* this is obviously a sick individual */
                Error( "Invalid Echo" );
            } else if( x == 1 ) {
                Message1( st );
            } else if( x == 2 ) {
                Message2( st );
            }
            // DisplayLineInWindow( MessageWindow, x, st );
        } else {
            if( x > 2 ) {
                /* this is obviously a sick individual */
                Error( "Invalid Echo" );
            } else if( x == 1 ) {
                Message1( dataBuff );
            } else if( x == 2 ) {
                Message2( dataBuff );
            }
            // DisplayLineInWindow( MessageWindow, x, dataBuff );
        }
        break;
#ifdef VI_RCS
    case PCL_T_CHECKOUT:
        rc = ERR_NO_ERR;
#ifdef __WINDOWS__
        if( isOS2() ) break; // OS/2 shell returns before checkout finishes
#endif
        if( CurrentFile == NULL ) break;
        if( RCSInit == NULL || RCSRegisterBatchCallback == NULL ||
            RCSSetPause == NULL || RCSCheckout == NULL || RCSFini == NULL ) {
        } else {
            extern int RCSAPI Batcher( char *cmd, void *cookie );
            linenum row;
            int     col;
            rcsdata r;
#ifdef __WIN__
            FARPROC fp;
            r = RCSInit( (unsigned long)Root, getenv( "WATCOM" ) );
            fp = MakeProcInstance( (FARPROC)(&Batcher), InstanceHandle );
            RCSRegisterBatchCallback( r, (BatchCallback *)fp, NULL );
#else
            r = RCSInit( 0, getenv( "WATCOM" ) );
            RCSRegisterBatchCallback( r, (BatchCallback *)&Batcher, NULL );
#endif
            if( RCSQuerySystem( r ) != 0 ) {
                if( GenericQueryBool( "File is read only, check out?" ) ) {
                    char full1[FILENAME_MAX];

                    _fullpath( full1, CurrentFile->name, FILENAME_MAX );
                    RCSSetPause( r, TRUE );
                    if( RCSCheckout( r, full1, NULL, NULL ) ) {
                        strcpy( dataBuff, CurrentFile->name );
                        rc = ERR_NO_ERR;
                        row = CurrentPos.line;
                        col = CurrentPos.column;
                        EditFile( dataBuff, TRUE );
                        GoToLineNoRelCurs( row );
                        GoToColumnOnCurrentLine( col );
                    }
                }
            }
#ifdef __WIN__
            FreeProcInstance( (FARPROC) fp );
#endif
            RCSFini( r );
            break;
        }
    case PCL_T_CHECKIN:
        if( CurrentFile == NULL ) break;
        if( RCSInit == NULL || RCSRegisterBatchCallback == NULL ||
            RCSSetPause == NULL || RCSCheckin == NULL || RCSFini == NULL ) break;
        {
            extern int RCSAPI Batcher( char *cmd, void *cookie );
            linenum row;
            int     col;
            rcsdata r;

#ifdef __WIN__
            FARPROC fp;
            r = RCSInit( (unsigned long)Root, getenv( "WATCOM" ) );
            fp = MakeProcInstance( (FARPROC)(&Batcher), InstanceHandle );
            RCSRegisterBatchCallback( r, (BatchCallback *)fp, NULL );
#else
            r = RCSInit( 0, getenv( "WATCOM" ) );
            RCSRegisterBatchCallback( r, (BatchCallback *)&Batcher, NULL );
#endif
            RCSSetPause( r, TRUE );
            if( CurrentFile->modified ) {
                FilePromptForSaveChanges( CurrentFile );
            }
            if( RCSCheckin( r, CurrentFile->name, NULL, NULL ) ) {
                rc = ERR_NO_ERR;
                strcpy( dataBuff, CurrentFile->name );
                row = CurrentPos.line;
                col = CurrentPos.column;
                EditFile( dataBuff, TRUE );
                GoToLineNoRelCurs( row );
                GoToColumnOnCurrentLine( col );
            }
#ifdef __WIN__
            FreeProcInstance( (FARPROC) fp );
#endif
            RCSFini( r );
            break;
        }
#endif
    default:
        if( tkn >= 1000 ) {
            rc = ProcessEx( n1, n2, n2f, dmt, tkn - 1000, dataBuff );
            break;
        }
        rc = TryCompileableToken( tkn, dataBuff, TRUE, dmt );
        if( rc != NOT_COMPILEABLE_TOKEN ) {
            break;
        }
        rc = ProcessWindow( tkn, dataBuff );
        if( rc >= ERR_NO_ERR ) {
            break;
        }
        if( n1f && !n2f ) {
            if( !n1 ) {
                n1 = 1;
            }
            MemorizeCurrentContext();
            rc = GoToLineNoRelCurs( n1 );
            if( rc == ERR_NO_ERR ) {
                GoToColumnOnCurrentLine( 1 );
                if( EditFlags.LineDisplay ) {
                    MyPrintf( "%s\n", CurrentLine->data );
                }
            }
            return( rc );
        }
        rc = ERR_INVALID_COMMAND;
        break;
    }

    if( rc == ERR_NO_ERR ) {
        rc = DO_NOT_CLEAR_MESSAGE_WINDOW;
    }
    return( rc );

} /* RunCommandLine */

/*
 * ProcessWindow - process window commands.
 */
vi_rc ProcessWindow( int tkn, char *data )
{
    vi_rc       rc;

    rc = ERR_NO_ERR;
    switch( tkn ) {
    /*
     * commands follow
     */
    case PCL_T_COMMANDWINDOW: wInfo = &cmdlinew_info; break;
    case PCL_T_STATUSWINDOW: wInfo = &statusw_info; break;
    case PCL_T_COUNTWINDOW: wInfo = &repcntw_info; break;
    case PCL_T_EDITWINDOW: wInfo = &editw_info; break;
    case PCL_T_EXTRAINFOWINDOW: wInfo = &extraw_info; break;
    case PCL_T_FILECWINDOW: wInfo = &filecw_info; break;
    case PCL_T_LINENUMBERWINDOW: wInfo = &linenumw_info; break;
    case PCL_T_DIRWINDOW: wInfo = &dirw_info; break;
    case PCL_T_FILEWINDOW: wInfo = &filelistw_info; break;
    case PCL_T_SETWINDOW: wInfo = &setw_info; break;
    case PCL_T_SETVALWINDOW: wInfo = &setvalw_info; break;
    case PCL_T_MESSAGEWINDOW: wInfo = &messagew_info; break;
    case PCL_T_MENUWINDOW: wInfo = &menuw_info; break;
    case PCL_T_MENUBARWINDOW: wInfo = &menubarw_info; break;
    case PCL_T_DEFAULTWINDOW: wInfo = &defaultw_info; break;
    case PCL_T_ACTIVEMENUWINDOW: wInfo = &activemenu_info; break;
    case PCL_T_GREYEDMENUWINDOW: wInfo = &greyedmenu_info; break;
    case PCL_T_ACTIVEGREYEDMENUWINDOW: wInfo = &activegreyedmenu_info; break;

    case PCL_T_ENDWINDOW:
        if( wInfo == &defaultw_info ) {
            cmdlinew_info = defaultw_info;
            statusw_info = defaultw_info;
            repcntw_info = defaultw_info;
            editw_info = defaultw_info;
            extraw_info = defaultw_info;
            filecw_info = defaultw_info;
            linenumw_info = defaultw_info;
            dirw_info = defaultw_info;
            filelistw_info = defaultw_info;
            setw_info = defaultw_info;
            setvalw_info = defaultw_info;
            messagew_info = defaultw_info;
            menuw_info = defaultw_info;
            menubarw_info = defaultw_info;
            greyedmenu_info = defaultw_info;
            activegreyedmenu_info = defaultw_info;
            activemenu_info = defaultw_info;
        } else if( wInfo == &messagew_info ) {
            rc = NewMessageWindow();
        } else if( wInfo == &statusw_info ) {
            rc = NewStatusWindow();
        } else if( wInfo == &menubarw_info ) {
            rc = InitMenu();
        } else if ( wInfo == &editw_info ) {
            ResetAllWindows();
        }
        wInfo = NULL;
        break;
    case PCL_T_BORDER:
        rc = setWBorder( data );
        break;
    case PCL_T_HILIGHT:
        rc = setWHilite( data );
        break;
    case PCL_T_TEXT:
        rc = setWText( data );
        break;
    case PCL_T_WHITESPACE:
        rc = setSyntaxStyle( SE_WHITESPACE, data );
        break;
    case PCL_T_SELECTION:
        rc = setSyntaxStyle( SE_SELECTION, data );
        break;
    case PCL_T_EOFTEXT:
        rc = setSyntaxStyle( SE_EOFTEXT, data );
        break;
    case PCL_T_KEYWORD:
        rc = setSyntaxStyle( SE_KEYWORD, data );
        break;
    case PCL_T_OCTAL:
        rc = setSyntaxStyle( SE_OCTAL, data );
        break;
    case PCL_T_HEX:
        rc = setSyntaxStyle( SE_HEX, data );
        break;
    case PCL_T_INTEGER:
        rc = setSyntaxStyle( SE_INTEGER, data );
        break;
    case PCL_T_CHAR:
        rc = setSyntaxStyle( SE_CHAR, data );
        break;
    case PCL_T_PREPROCESSOR:
        rc = setSyntaxStyle( SE_PREPROCESSOR, data );
        break;
    case PCL_T_SYMBOL:
        rc = setSyntaxStyle( SE_SYMBOL, data );
        break;
    case PCL_T_INVALIDTEXT:
        rc = setSyntaxStyle( SE_INVALIDTEXT, data );
        break;
    case PCL_T_IDENTIFIER:
        rc = setSyntaxStyle( SE_IDENTIFIER, data );
        break;
    case PCL_T_JUMPLABEL:
        rc = setSyntaxStyle( SE_JUMPLABEL, data );
        break;
    case PCL_T_COMMENT:
        rc = setSyntaxStyle( SE_COMMENT, data );
        break;
    case PCL_T_STRING:
        rc = setSyntaxStyle( SE_STRING, data );
        break;
    case PCL_T_VARIABLE:
        rc = setSyntaxStyle( SE_VARIABLE, data );
        break;
    case PCL_T_REGEXP:
        rc = setSyntaxStyle( SE_REGEXP, data );
        break;
    case PCL_T_FLOAT:
        rc = setSyntaxStyle( SE_FLOAT, data );
        break;
    case PCL_T_DIMENSION:
        rc = setWDimension( data );
        break;
    default:
        rc = -1;
        break;
    }
    return( rc );

} /* ProcessWindow */

/*
 * setStyle - set style parameters
 */
static void setStyle( type_style *style, vi_color tc1, vi_color tc2, font_type tc3 )
{
    style->foreground = tc1;
    style->background = tc2;
    style->font = tc3;
}

/*
 * setWBorder - set window border
 */
static vi_rc setWBorder( char *data )
{
    int         btype, bc1, bc2;
    bool        has_border;
    char        token[MAX_STR];

    if( wInfo == NULL ) {
        return( ERR_WIND_INVALID );
    }
    if( NextWord1( data,token ) <= 0 ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    btype = atoi( token );
    if( btype < 0 ) {
        has_border = FALSE;
    } else {
        has_border = TRUE;
    }
    if( NextWord1( data, token ) <= 0 ) {
        if( !has_border ) {
            wInfo->has_border = FALSE;
            return( ERR_NO_ERR );
        }
        return( ERR_INVALID_WINDOW_SETUP );
    }
    bc1 = atoi( token );
    if( NextWord1( data, token ) <= 0 ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    bc2 = atoi( token );

    wInfo->has_border = has_border;
    wInfo->border_color1 = bc1;
    wInfo->border_color2 = bc2;
    return( ERR_NO_ERR );

} /* setWBorder */

/*
 * setWText - set window text color
 */
static vi_rc setWText( char *data )
{
    int         tc1, tc2, tc3;
    char        token[MAX_STR];

    if( wInfo == NULL ) {
        return( ERR_WIND_INVALID );
    }
    if( NextWord1( data, token ) <= 0 ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    tc1 = atoi( token );
    if( NextWord1( data, token ) <= 0 ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    tc2 = atoi( token );
    tc3 = FONT_DEFAULT;
    if( NextWord1( data, token ) > 0 ) {
        tc3 = atoi( token );
    }

    setStyle( &wInfo->text, tc1, tc2, tc3 );
    /* want an SE_TEXT entry when SS turned off - steal it from here */
    if( wInfo == &editw_info ) {
        setStyle( &SEType[SE_TEXT], tc1, tc2, tc3 );
    }
    return( ERR_NO_ERR );

} /* setWText */

/*
 * setWHilite - set window hilighting color
 */
static vi_rc setWHilite( char *data )
{
    int         tc1, tc2, tc3;
    char        token[MAX_STR];

    if( wInfo == NULL ) {
        return( ERR_WIND_INVALID );
    }
    if( NextWord1( data, token ) <= 0 ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    tc1 = atoi( token );
    if( NextWord1( data, token ) <= 0 ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    tc2 = atoi( token );
    tc3 = FONT_DEFAULTBOLD;
    if( NextWord1( data, token ) > 0 ) {
        tc3 = atoi( token );
    }

    setStyle( &wInfo->hilight, tc1, tc2, tc3 );
    return( ERR_NO_ERR );

} /* setWHilite */

/*
 * setWDimension - set window dimension
 */
static vi_rc setWDimension( char *data )
{
    int         x1, y1, x2, y2;
    char        token[MAX_STR];
    jmp_buf     jmpaddr;
    int         i;
    int         x, y;

    if( wInfo == NULL ) {
        return( ERR_WIND_INVALID );
    }
    i = setjmp( jmpaddr );
    if( i != 0 ) {
        return( (vi_rc)i );
    }

    if( NextWord1( data, token ) <= 0 ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    StartExprParse( token, jmpaddr );
    x1 = GetConstExpr();

    if( NextWord1( data, token ) <= 0 ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    StartExprParse( token, jmpaddr );
    y1 = GetConstExpr();

    if( NextWord1( data, token ) <= 0 ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    StartExprParse( token, jmpaddr );
    x2 = GetConstExpr();

    if( NextWord1( data, token ) <= 0 ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    StartExprParse( token, jmpaddr );
    y2 = GetConstExpr();

    if( x1 < 0 || y1 < 0 ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    x = x2 - x1;
    y = y2 - y1;
    if( x < 0 || y < 0 ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    if( x >= WindMaxWidth || y >= WindMaxHeight ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    wInfo->x1 = x1;
    wInfo->y1 = y1;
    wInfo->x2 = x2;
    wInfo->y2 = y2;
    return( ERR_NO_ERR );

} /* setWDimension */

/*
 * setSyntaxStyle - set syntax style color
 */
static vi_rc setSyntaxStyle( syntax_element style, char *data )
{
    int         tc1, tc2, tc3;
    char        token[MAX_STR];

    if( wInfo != &editw_info ) {
        return( ERR_WIND_INVALID );
    }
    if( NextWord1( data, token ) <= 0 ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    tc1 = atoi( token );
    if( NextWord1( data, token ) <= 0 ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    tc2 = atoi( token );
    tc3 = FONT_DEFAULTBOLD;
    if( NextWord1( data, token ) > 0 ) {
        tc3 = atoi( token );
    }

    setStyle( &SEType[style], tc1, tc2, tc3 );
    return( ERR_NO_ERR );
}

#ifdef VI_RCS
int RCSAPI Batcher( char *cmd, void *cookie )
{
    cookie = cookie;
    if( ExecCmd( NULL, NULL, cmd ) == 0 ) return( 1 );
    return( 0 );
}
#endif
