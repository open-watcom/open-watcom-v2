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
#include "rxsupp.h"
#include "ex.h"
#include "win.h"
#include "expr.h"
#include "menu.h"
#include "sstyle.h"
#include "fts.h"
#include "rcs.h"

#include "clibext.h"


static window_info  *wInfo = NULL;
static char         strLoad[] = "loaded";
static char         strCompile[] = "compiled";
static char         *dataBuff;
static char         *tmpBuff;

static vi_rc        setWDimension( const char * );
static vi_rc        setWHilite( const char * );
static vi_rc        setWText( const char * );
static vi_rc        setWBorder( const char * );
static vi_rc        setSyntaxStyle( syntax_element, const char * );

#if defined( VI_RCS ) && defined( __WINDOWS__ )
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
        return( true );
    }
    return( false );
}
#endif

/*
 * InitCommandLine - initialize command line processing
 */
void InitCommandLine( void )
{
    dataBuff = MemAlloc( EditVars.MaxLine );
    tmpBuff = MemAlloc( EditVars.MaxLine );

} /* InitCommandLine */

/*
 * FiniCommandLine - free memory
 */
void FiniCommandLine( void )
{
    MemFree( dataBuff );
    MemFree( tmpBuff );

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
    st = MemAllocUnsafe( EditVars.MaxLine );
    if( st == NULL ) {
        return( ERR_NO_MEMORY );
    }
    is_fancy = is_fancy;
#ifdef __WIN__
    if( is_fancy ) {
        if( !GetCmdDialog( st, EditVars.MaxLine ) ) {
            MemFree( st );
            return( ERR_NO_ERR );
        }
    } else {
#endif
        rc = PromptForString( ":", st, EditVars.MaxLine, &EditVars.CLHist );
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
    return( doProcessCommandLine( false ) );

} /* ProcessCommandLine */

/*
 * FancyProcessCommandLine - do just that
 */
vi_rc FancyProcessCommandLine( void )
{
    return( doProcessCommandLine( true ) );

} /* FancyProcessCommandLine */

/*
 * TryCompileableToken - process token that can also be compiled
 */
vi_rc TryCompileableToken( int token, const char *data, bool iscmdline )
{
    vi_rc       rc = ERR_INVALID_COMMAND;
    bool        mflag;

    switch( token ) {
    case PCL_T_MAPBASE_DMT:
    case PCL_T_MAP_DMT:
    case PCL_T_MAPBASE:
    case PCL_T_MAP:
        if( iscmdline ) {
            mflag = MAPFLAG_MESSAGE;
        } else {
            mflag = 0;
        }
        if( token == PCL_T_MAPBASE || token == PCL_T_MAPBASE_DMT ) {
            mflag |= MAPFLAG_BASE;
        }
        if( token == PCL_T_MAP_DMT || token == PCL_T_MAPBASE_DMT ) {
            mflag |= MAPFLAG_DAMMIT;
        }
        rc = MapKey( mflag, data );
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
            data = Expand( tmpBuff, data, NULL );
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
vi_rc RunCommandLine( const char *cl )
{
    int         i, x, y, x2, y2;
    bool        n1f, n2f;
    int         tkn, flag;
    bool        test1;
    linenum     n1, n2;
    char        st[FILENAME_MAX];
    info        *cinfo;
    long        val;
    jmp_buf     jmpaddr;
    vi_rc       rc;
    const char  *data;

    /*
     * parse command string
     */
    tkn = TOK_INVALID;
    rc = ParseCommandLine( cl, &n1, &n1f, &n2, &n2f, &tkn, dataBuff );
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
    data = dataBuff;
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
        data = SkipLeadingSpaces( data );
        if( *data != '\0' ) {
            key_map     scr;

            rc = AddKeyMap( &scr, data );
            if( rc != ERR_NO_ERR ) {
                break;
            }
            rc = RunKeyMap( &scr, 1L );
            MemFree( scr.data );
        }
        break;

    case PCL_T_DELETEMENU:
        rc = DoMenuDelete( data );
        break;
    case PCL_T_DELETEMENUITEM:
        rc = DoItemDelete( data );
        break;
    case PCL_T_ADDMENUITEM:
        rc = AddMenuItem( data );
        break;
    case PCL_T_MAXIMIZE:
        rc = MaximizeCurrentWindow();
        break;
    case PCL_T_MINIMIZE:
        rc = MinimizeCurrentWindow();
        break;
    case PCL_T_EXITFILESONLY:
        if( !ExitWithPrompt( false, false ) ) {
            rc = ERR_EXIT_ABORTED;
        } else {
            rc = ERR_NO_ERR;
        }
        break;
    case PCL_T_EXITALL:
        if( !ExitWithPrompt( true, false ) ) {
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
        data = SkipLeadingSpaces( data );
        KeyAddString( data );
        rc = ERR_NO_ERR;
        break;

    case PCL_T_UNALIAS:
        rc = UnAlias( data );
        break;

    case PCL_T_UNABBREV:
        rc = UnAbbrev( data );
        break;

    case PCL_T_UNMAP:
    case PCL_T_UNMAP_DMT:
        flag = MAPFLAG_MESSAGE + MAPFLAG_UNMAP;
        if( tkn == PCL_T_UNMAP_DMT ) {
            flag |= MAPFLAG_DAMMIT;
        }
        rc = MapKey( flag, data );
        break;

    case PCL_T_EVAL:
        data = Expand( tmpBuff, data, NULL );
        i = setjmp( jmpaddr );
        if( i != 0 ) {
            rc = (vi_rc)i;
        } else {
            StartExprParse( data, jmpaddr );
            val = GetConstExpr();
            ltoa( val, st, EditVars.Radix );
            Message1( "%s", st );
            rc = ERR_NO_ERR;
        }
        break;

    case PCL_T_COMPILE:
    case PCL_T_SOURCE:
    case PCL_T_LOAD:
        {
            char        *tstr;
            srcline     sline;

            data = GetNextWord1( data, st );
            if( *st == '\0' ) {
                rc = ERR_NO_FILE_SPECIFIED;
                break;
            }

            if( tkn == PCL_T_COMPILE ) {
                EditFlags.CompileScript = true;
                if( st[0] == '-' ) {
                    if( st[1] == 'a' || st[1] == 'A' ) {
                        EditFlags.CompileAssignments = true;
                        if( st[1] == 'A' ) {
                            EditFlags.CompileAssignmentsDammit = true;
                        }
                        data = GetNextWord1( data, st);
                        if( *st == '\0' ) {
                            rc = ERR_NO_FILE_SPECIFIED;
                            break;
                        }
                    }
                }
            }
            if( tkn == PCL_T_LOAD ) {
                EditFlags.LoadResidentScript = true;
            }
            sline = 0;
            rc = Source( st, data, &sline );

            EditFlags.LoadResidentScript = false;
            EditFlags.CompileScript = false;
            EditFlags.CompileAssignments = false;
            EditFlags.CompileAssignmentsDammit = false;
            if( EditFlags.SourceScriptActive ) {
                LastError = rc;
            }
            if( rc > ERR_NO_ERR ) {
                Error( "%s on line %u of \"%s\"", GetErrorMsg( rc ), sline, st );
            } else {
                if( rc != DO_NOT_CLEAR_MESSAGE_WINDOW ) {
                    if( tkn != PCL_T_SOURCE ) {
                        if( tkn == PCL_T_LOAD ) {
                            tstr = strLoad;
                        } else {
                            tstr = strCompile;
                        }
                        Message1( "Script \"%s\" %s, %u lines generated, %d errors",
                                        st, tstr, sline, SourceErrCount );
                        rc = DO_NOT_CLEAR_MESSAGE_WINDOW;
                    }
                }
            }
            break;
        }

    case PCL_T_GENCONFIG:
#ifndef __WIN__
        data = GetNextWord1( data,st );
        if( *st != '\0' ) {
            rc = GenerateConfiguration( st, true );
        } else {
            rc = GenerateConfiguration( NULL, true );
        }
#else
        {
            bool temp = EditFlags.SaveConfig;
            EditFlags.SaveConfig = true;
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
        rc = Shift( n1, n2, '>', true );
        break;

    case PCL_T_SUCK:
        rc = Shift( n1, n2, '<', true );
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
        rc = DoHelp( data );
        break;

    case PCL_T_VIEW:
    case PCL_T_VIEW_DMT:
        EditFlags.ViewOnly = true;
    case PCL_T_EDIT:
    case PCL_T_EDIT_DMT:
        rc = EditFile( data, ( tkn == PCL_T_VIEW_DMT || tkn == PCL_T_EDIT_DMT ) );
        EditFlags.ViewOnly = false;
        break;

    case PCL_T_OPEN:
        rc = OpenWindowOnFile( data );
        break;

    case PCL_T_HIDE:
    case PCL_T_HIDE_DMT:
        rc = HideLineRange( n1, n2, ( tkn == PCL_T_HIDE_DMT ) );
        break;

    case PCL_T_DELETE:
        rc = SetSavebufNumber( data );
        if( rc != ERR_NO_ERR ) {
            break;
        }
        if( SelRgn.selected && !EditFlags.LineBased ) {
            AddSelRgnToSavebufAndDelete();
            rc = ERR_NO_ERR;
            // @ may have turned this on - it is now definitely off
            SelRgn.selected = false;
        } else {
            rc = DeleteLineRange( n1, n2, SAVEBUF_FLAG );
        }
        if( rc == ERR_NO_ERR ) {
            DCDisplayAllLines();
            LineDeleteMessage( n1, n2 );
        }
        break;

    case PCL_T_SAVEANDEXIT:
        data = GetNextWord1( data, st );
        if( *st == '\0' ) {
            rc = SaveAndExit( st );
        } else {
            rc = SaveAndExit( NULL );
        }
        break;

    case PCL_T_PUT:
    case PCL_T_PUT_DMT:
        rc = SetSavebufNumber( data );
        if( rc != ERR_NO_ERR ) {
            break;
        }
        rc = SaveAndResetFilePos( n1 );
        if( rc == ERR_NO_ERR ) {
            if( tkn == PCL_T_PUT ) {
                rc = InsertSavebufAfter();
            } else {
                rc = InsertSavebufBefore();
            }
            RestoreCurrentFilePos();
        }
        break;

    case PCL_T_YANK:
        rc = SetSavebufNumber( data );
        if( rc != ERR_NO_ERR ) {
            break;
        }
        if( SelRgn.selected && !EditFlags.LineBased ) {
            rc = YankSelectedRegion();
            // @ may have turned this on - it is now definitely off
            SelRgn.selected = false;
        } else {
            rc = YankLineRange( n1, n2 );
        }
        break;

    case PCL_T_SUBSTITUTE:
        rc = Substitute( n1, n2, data );
        break;

    case PCL_T_GLOBAL:
    case PCL_T_GLOBAL_DMT:
        if( !test1 ) {
            n1 = 1;
            rc = CFindLastLine( &n2 );
            if( rc != ERR_NO_ERR ) {
                break;
            }
        }
        rc = Global( n1,n2, data, ( tkn == PCL_T_GLOBAL_DMT ) );
        break;

    case PCL_T_WRITEQUIT:
        if( CurrentFile == NULL ) {
            rc = NextFile();
        } else {
            CurrentFile->modified = true;
            data = GetNextWord1( data, st );
            if( *st != '\0' ) {
                rc = SaveAndExit( st );
            } else {
                rc = SaveAndExit( NULL );
            }
        }
        break;

    case PCL_T_WRITE:
    case PCL_T_WRITE_DMT:
        if( test1 ) {
            data = GetNextWord1( data, st );
            if( *st == '\0' ) {
                rc = ERR_NO_FILE_SPECIFIED;
            } else {
                rc = SaveFile( st, n1, n2, ( tkn == PCL_T_WRITE_DMT ) );
            }
        } else {
            data = GetNextWord1( data, st );
            if( st[0] != '\0' ) {
#ifdef __WIN__
                if( st[0] == '?' && st[1] == '\0' ) {
                    rc = SaveFileAs();
                    break;
                } else {
                    rc = SaveFile( st, -1, -1, ( tkn == PCL_T_WRITE_DMT ) );
                }
#else
                rc = SaveFile( st, -1, -1, ( tkn == PCL_T_WRITE_DMT ) );
#endif
            } else {
                rc = SaveFile( NULL, -1, -1, ( tkn == PCL_T_WRITE_DMT ) );
                if( rc == ERR_NO_ERR ) {
                    Modified( false );
                }
            }
        }
        break;

    case PCL_T_READ:
        rc = ReadAFile( n1, data );
        break;

    case PCL_T_QUIT:
#ifdef __WIN__
        rc = CurFileExitOptionSaveChanges();
#else
        rc = NextFile();
#endif
        break;
    case PCL_T_QUIT_DMT:
        rc = NextFileDammit();
        break;

    case PCL_T_DATE:
        GetDateTimeString( st );
        Message1( st );
        rc = DO_NOT_CLEAR_MESSAGE_WINDOW;
        break;

    case PCL_T_CD:
        data = GetNextWord1( data, st );
        if( *st != '\0' ) {
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
            rc = DoGenericFilter( n1, n2, data );
        } else {
            data = SkipLeadingSpaces( data );
            if( *data == 0 ) {
                goto EVIL_SHELL;
            }
            ExecCmd( NULL, NULL, data );
            rc = ERR_NO_ERR;
        }
        break;

    case PCL_T_RESIZE:
        rc = ResizeCurrentWindowWithKeys();
        break;

    case PCL_T_TILE:
        data = GetNextWord1( data, st );
        if( st[0] != '\0' ) {
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
                data = GetNextWord1( data, st );
                if( *st == '\0' ) {
                    break;
                } else {
                    y = atoi( st );
                }
            }
        } else {
            x = EditVars.MaxWindowTileX;
            y = EditVars.MaxWindowTileY;
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
        data = GetNextWord1( data, st );
        if( *st != '\0' ) {
            rc = TagHunt( st );
        }
        break;

    case PCL_T_FGREP:
        {
            bool        ci;

            data = SkipLeadingSpaces( data );
            ci = EditFlags.CaseIgnore;
            if( data[0] == '-' ) {
                if( data[1] == 'c' ) {
                    ci = false;
                    data += 2;
                    data = SkipLeadingSpaces( data );
                    rc = GetStringWithPossibleQuoteC( &data, st );
                } else if( data[1] == 'i' ) {
                    ci = true;
                    data += 2;
                    data = SkipLeadingSpaces( data );
                    rc = GetStringWithPossibleQuoteC( &data, st );
                } else if( data[1] == 'f' ) {
                    data += 2;
                    data = SkipLeadingSpaces( data );
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
                rc = GetStringWithPossibleQuoteC( &data, st );
            }
            if( rc != ERR_NO_STRING ) {
                rc = DoFGREP( data, st, ci );
            }
        }
        break;

    case PCL_T_EGREP:
        rc = GetStringWithPossibleQuoteC( &data, st );
        if( rc != ERR_NO_STRING ) {
            rc = DoEGREP( data, st );
        }
        break;

    case PCL_T_SIZE:
        data = GetNextWord1( data, st );
        if( *st == '\0' ) {
            break;
        }
        x = atoi( st );
        data = GetNextWord1( data, st );
        if( *st == '\0' ) {
            break;
        }
        y = atoi( st );
        data = GetNextWord1( data, st );
        if( *st == '\0' ) {
            break;
        }
        x2 = atoi( st );
        data = GetNextWord1( data, st );
        if( *st == '\0' ) {
            break;
        }
        y2 = atoi( st );
        rc = CurrentWindowResize( x, y, x2, y2 );
        break;

    case PCL_T_ECHO:
        data = GetNextWord1( data, st );
        if( *st == '\0' ) {
            break;
        }
        rc = ERR_NO_ERR;
        if( !stricmp( st, "on" ) ) {
            EditFlags.EchoOn = true;
            break;
        } else if( !stricmp( st, "off" ) ) {
            EditFlags.EchoOn = false;
            break;
        }
        x = atoi( st );
        data = SkipLeadingSpaces( data );
        /*
         * FIXME: This is not good - I will definately have to
         * fix this code up. But right now I have to get the
         * editor ready for tomorrow. Brad.
         */
        if( data[0] == '"' || data[0] == '/' ) {
            GetStringWithPossibleQuoteC( &data, st );
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
                Message1( data );
            } else if( x == 2 ) {
                Message2( data );
            }
            // DisplayLineInWindow( MessageWindow, x, data );
        }
        break;
#ifdef VI_RCS
    case PCL_T_CHECKOUT:
        rc = ERR_NO_ERR;
#ifdef __WINDOWS__
        if( isOS2() ) break; // OS/2 shell returns before checkout finishes
#endif
        if( CurrentFile != NULL ) {
            rc = ViRCSCheckout( rc );
        }
        break;
    case PCL_T_CHECKIN:
        if( CurrentFile != NULL ) {
            rc = ViRCSCheckin( rc );
        }
        break;
#endif
    default:
        if( tkn >= 1000 ) {
            rc = ProcessEx( n1, n2, n2f, tkn - 1000, data );
            break;
        }
        rc = TryCompileableToken( tkn, data, true );
        if( rc != NOT_COMPILEABLE_TOKEN ) {
            break;
        }
        rc = ProcessWindow( tkn, data );
        if( rc >= ERR_NO_ERR ) {
            break;
        }
    case TOK_INVALID:
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
vi_rc ProcessWindow( int tkn, const char *data )
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
static vi_rc setWBorder( const char *data )
{
    int         btype, bc1, bc2;
    bool        has_border;
    char        token[MAX_STR];

    if( wInfo == NULL ) {
        return( ERR_WIND_INVALID );
    }
    data = GetNextWord1( data, token );
    if( *token == '\0' ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    btype = atoi( token );
    has_border = ( btype >= 0 );
    data = GetNextWord1( data, token );
    if( *token == '\0' ) {
        if( !has_border ) {
            wInfo->has_border = false;
            return( ERR_NO_ERR );
        }
        return( ERR_INVALID_WINDOW_SETUP );
    }
    bc1 = atoi( token );
    data = GetNextWord1( data, token );
    if( *token == '\0' ) {
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
static vi_rc setWText( const char *data )
{
    int         tc1, tc2, tc3;
    char        token[MAX_STR];

    if( wInfo == NULL ) {
        return( ERR_WIND_INVALID );
    }
    data = GetNextWord1( data, token );
    if( *token == '\0' ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    tc1 = atoi( token );
    data = GetNextWord1( data, token );
    if( *token == '\0' ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    tc2 = atoi( token );
    tc3 = FONT_DEFAULT;
    data = GetNextWord1( data, token );
    if( *token != '\0' ) {
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
static vi_rc setWHilite( const char *data )
{
    int         tc1, tc2, tc3;
    char        token[MAX_STR];

    if( wInfo == NULL ) {
        return( ERR_WIND_INVALID );
    }
    data = GetNextWord1( data, token );
    if( *token == '\0' ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    tc1 = atoi( token );
    data = GetNextWord1( data, token );
    if( *token == '\0' ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    tc2 = atoi( token );
    tc3 = FONT_DEFAULTBOLD;
    data = GetNextWord1( data, token );
    if( *token != '\0' ) {
        tc3 = atoi( token );
    }
    setStyle( &wInfo->hilight, tc1, tc2, tc3 );
    return( ERR_NO_ERR );

} /* setWHilite */

/*
 * setWDimension - set window dimension
 */
static vi_rc setWDimension( const char *data )
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

    data = GetNextWord1( data, token );
    if( *token == '\0' ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    StartExprParse( token, jmpaddr );
    x1 = GetConstExpr();

    data = GetNextWord1( data, token );
    if( *token == '\0' ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    StartExprParse( token, jmpaddr );
    y1 = GetConstExpr();

    data = GetNextWord1( data, token );
    if( *token == '\0' ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    StartExprParse( token, jmpaddr );
    x2 = GetConstExpr();

    data = GetNextWord1( data, token );
    if( *token == '\0' ) {
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
    if( x >= EditVars.WindMaxWidth || y >= EditVars.WindMaxHeight ) {
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
static vi_rc setSyntaxStyle( syntax_element style, const char *data )
{
    int         tc1, tc2, tc3;
    char        token[MAX_STR];

    if( wInfo != &editw_info ) {
        return( ERR_WIND_INVALID );
    }
    data = GetNextWord1( data, token );
    if( *token == '\0' ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    tc1 = atoi( token );
    data = GetNextWord1( data, token );
    if( *token == '\0' ) {
        return( ERR_INVALID_WINDOW_SETUP );
    }
    tc2 = atoi( token );
    tc3 = FONT_DEFAULTBOLD;
    data = GetNextWord1( data, token );
    if( *token != '\0' ) {
        tc3 = atoi( token );
    }

    setStyle( &SEType[style], tc1, tc2, tc3 );
    return( ERR_NO_ERR );
}

void EditRCSCurrentFile( void )
{
    linenum row;
    int     col;

    row = CurrentPos.line;
    col = CurrentPos.column;
    EditFile( CurrentFile->name, true );
    GoToLineNoRelCurs( row );
    GoToColumnOnCurrentLine( col );
}
