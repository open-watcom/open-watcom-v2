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
* Description:  A hodgepodge of miscellaneous functions.
*
****************************************************************************/


#include "vi.h"
#include <stdarg.h>
#ifdef _M_I86
  #include <i86.h>
#endif
#include <errno.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
  #include <process.h>
#endif
#include "posix.h"
#include "win.h"
#ifdef __WIN__
  #include "color.h"
  #include "winctl.h"
#endif

#include "clibext.h"


static char *oldPrompt;

static void setPrompt( void )
{
    char        *tmp;

    if( EditVars.SpawnPrompt != NULL && EditVars.SpawnPrompt[0] != '\0' ) {
        tmp = getenv( PROMPT_ENVIRONMENT_VARIABLE );
        if( tmp != NULL ) {
            oldPrompt = MemAlloc( strlen( tmp ) + 1 );
            strcpy( oldPrompt, tmp );
        } else {
            oldPrompt = NULL;
        }
        setenv( PROMPT_ENVIRONMENT_VARIABLE, EditVars.SpawnPrompt, 1 );
    }
}

static void restorePrompt( void )
{
    if( EditVars.SpawnPrompt != NULL && EditVars.SpawnPrompt[0] != '\0' ) {
        setenv( PROMPT_ENVIRONMENT_VARIABLE, oldPrompt, 1 );
        if( oldPrompt != NULL ) {
            MemFree( oldPrompt );
            oldPrompt = NULL;
        }
    }
}

static bool clockActive;

static void preSpawn( void )
{
    info        *cinfo;

    clockActive = EditFlags.ClockActive;
    EditFlags.ClockActive = false;
#ifndef __WIN__
    FiniColors();
    if( !EditFlags.LineDisplay ) {
        ClearScreen();
    }
#endif

    /*
     * after a system command, all files could potentially have their
     * read/write attributes changed
     */
    for( cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
        cinfo->CurrentFile->check_readonly = true;
    }
    setPrompt();
}

static void postSpawn( long rc )
{
    restorePrompt();
    VarAddGlobalLong( "Sysrc", rc );
    UpdateCurrentDirectory();

#ifndef __WIN__
    ResetColors();
    // if( (EditFlags.PauseOnSpawnErr && rc != 0 ) ||
    //          !EditFlags.SourceScriptActive ) {
    if( EditFlags.PauseOnSpawnErr && rc != 0 ) {
        MyPrintf( "[%s]\n", MSG_PRESSANYKEY );
        GetNextEvent( false );
    }
    ResetSpawnScreen();
    if( !EditFlags.LineDisplay ) {
        ReDisplayScreen();
    }
#endif
    EditFlags.ClockActive = clockActive;
}
#if 0
// ifdef __NT__
#include "batcher.h"
#include <conio.h>
long ExecCmd( const char *file_in, const char *file_out, const char *cmd )
{
    int                 len;
    unsigned long       stat;
    char                *err;
    char                buff[256];
    int                 linked;
    err = BatchLink( NULL );
    if( err != NULL ) {
        printf( "link error: %s\n", err );
        exit( 1 );
    }
    BatchSpawn( cmd );
    for( ;; ) {
        len = BatchCollect( buff, sizeof( buff ), &stat );
        if( len == -1 ) {
            printf( "done: status = %d\n", stat );
            break;
        } else if( kbhit() ) {
            if( getch() == 'a' ) {
                BatchAbort();
            } else {
                BatchCancel();
            }
        } else if( len != 0 ) {
            buff[len] = '\0';
            printf( "%s", buff );
            fflush( stdout );
        }
    }
    return( 1 );
}
#endif

#ifdef __WIN__
long ExecCmd( const char *file_in, const char *file_out, const char *cmd )
{
    file_in = file_in;
    file_out = file_out;
    preSpawn();
    // fixme - should not ignore file_in and file_out
    SystemRC = MySpawn( cmd );
    postSpawn( SystemRC );
    return( SystemRC );
}
#else
#ifndef __NT__
static int doRedirect( int original, const char *filename, int mode )
{
    int fh;

    fh = open( filename, mode, PMODE_RW );
    if( fh != -1 ) {
        close( original );
        if( dup2( fh, original ) == 0 ) {
            return( fh );
        }
    }
    return( -1 );
}
#endif

#define MAX_ARGS        128

#ifdef __NT__
static long doExec( const char *std_in, const char *std_out, const char *cmd )
{
    HANDLE      cp;
    long        st;
    HANDLE      old_in;
    HANDLE      new_in;
    HANDLE      old_out;
    HANDLE      new_out;

    old_in = INVALID_HANDLE_VALUE;
    new_in = INVALID_HANDLE_VALUE;
    old_out = INVALID_HANDLE_VALUE;
    new_out = INVALID_HANDLE_VALUE;
    preSpawn();
    cp = GetCurrentProcess();
    if( std_in != NULL ) {
        old_in = GetStdHandle( STD_INPUT_HANDLE );
        new_in = CreateFile( std_in, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
        if( new_in == INVALID_HANDLE_VALUE ) {
            return( -1L );
        }
        SetStdHandle( STD_INPUT_HANDLE, new_in );
    }
    if( std_out != NULL ) {
        old_out = GetStdHandle( STD_INPUT_HANDLE );
        new_out = CreateFile( std_out, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING | CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL );
        if( new_out == INVALID_HANDLE_VALUE ) {
            if( std_in != NULL ) {
                SetStdHandle( STD_INPUT_HANDLE, old_in );
                CloseHandle( new_in );
            }
            return( -1L );
        }
        SetStdHandle( STD_OUTPUT_HANDLE, new_out );
    }

    if( cmd == NULL ) {
        st = MySpawn( Comspec );
    } else {
        SetConsoleActiveScreenBuffer( GetStdHandle( STD_OUTPUT_HANDLE ) );
        st = system( cmd );
    }

    if( std_in != NULL ) {
        CloseHandle( new_in );
        SetStdHandle( STD_INPUT_HANDLE, old_in );
    }
    if( std_out != NULL ) {
        CloseHandle( new_out );
        SetStdHandle( STD_OUTPUT_HANDLE, old_out );
    }
    postSpawn( st );
    return( st );
}
#else
static long doExec( const char *std_in, const char *std_out, const char *cmd )
{
    long        st;
    int         save_in, new_in;
    int         save_out, new_out;
#if 0
    char        buffer[MAX_INPUT_LINE];
    char        *argv[MAX_ARGS];
    char        *s;
    int         i;
#endif

    save_in = -1;
    new_in = -1;
    save_out = -1;
    new_out = -1;
    preSpawn();
    if( std_in != NULL ) {
        save_in = dup( STDIN_FILENO );
        new_in = doRedirect( STDIN_FILENO, std_in, O_RDONLY | O_BINARY );
        if( new_in == -1 ) {
            close( save_in );
            return( -1L );
        }
    }
    if( std_out != NULL ) {
        save_out = dup( STDOUT_FILENO );
        new_out = doRedirect( STDOUT_FILENO, std_out,
                              O_WRONLY | O_BINARY | O_CREAT | O_TRUNC );
        if( new_out == -1 ) {
            close( save_out );
            if( std_in != NULL ) {
                close( new_in );
                dup2( save_in, STDIN_FILENO );
                close( save_in );
            }
            return( -1L );
        }
    }

#if 0
    strcpy( buffer, cmd );
    s = buffer;
    for( i = 0; i < MAX_ARGS; i++ ) {
        while( isspace( *s ) )
            s++;
        if( *s == '\0' ) {
            argv[i] = NULL;
            break;
        }
        argv[i] = s;
        while( *s != '\0' && !isspace( *s ) ) {
            s++;
        }
        if( *s != '\0' ) {
            *s++ = '\0';
        } else {
            argv[i + 1] = NULL;
            break;
        }
    }

    st = spawnvp( P_WAIT, argv[0], argv );

#else
#if defined( __NT__ )
    if( cmd == NULL ) {
        st = MySpawn( Comspec );
    } else {
        SetConsoleActiveScreenBuffer( GetStdHandle( STD_OUTPUT_HANDLE ) );
        st = system( cmd );
    }
#elif defined( __UNIX__ ) || defined( __OS2__ )
    st = MySpawn( cmd );
#else
    st = system( cmd );
#endif
#endif

    if( std_in != NULL ) {
        close( new_in );
        dup2( save_in, STDIN_FILENO );
        close( save_in );
    }
    if( std_out != NULL ) {
        close( new_out );
        dup2( save_out, STDOUT_FILENO );
        close( save_out );
    }
    postSpawn( st );
    return( st );
}
#endif

#if defined( __DOS__ )
long ExecCmd( const char *file_in, const char *file_out, const char *cmd )
{
    if( file_in != NULL || file_out != NULL ) {
        SystemRC = doExec( file_in, file_out, cmd );
    } else {
        preSpawn();
        SystemRC = MySpawn( cmd );
        postSpawn( SystemRC );
    }
    return( SystemRC );
}
#else
long ExecCmd( const char *file_in, const char *file_out, const char *cmd )
{
    return( doExec( file_in, file_out, cmd ) );
}
#endif
#endif

/*
 * GetResponse - get a response from the user
 */
vi_rc GetResponse( char *str, char *res )
{
    vi_rc   rc;

    rc = PromptForString( str, res, MAX_STR, NULL );
    if( rc == ERR_NO_ERR ) {
        return( GOT_RESPONSE );
    }
    return( rc );

} /* GetResponse */

/*
 * PromptFilesForSave - prompt to save for each file which has
 * been modified.
 */
bool PromptFilesForSave( void )
{
#ifdef __WIN__
    info        *cinfo;
    int         i;
    int         num = 0;
    HWND        hwnd_old = NO_WINDOW;

    if( !EditFlags.SaveOnBuild ) {
        return( true );
    }

    for( cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
        num++;
    }

    BringUpFile( InfoHead, true );
    for( i = 0; i < num; i++ ) {
        if( CurrentFile != NULL && CurrentFile->dup_count == 0 &&
            CurrentFile->modified ) {

            /* we have a modified file, so bring to the front */
            BringWindowToTop( root_window_id );
            hwnd_old = SetFocus( root_window_id );

            // file modified -- so prompt for save
            FilePromptForSaveChanges( CurrentFile );
        }
        RotateFileForward();
    }
    if( !BAD_ID( hwnd_old ) ) {
        SetWindowPos( root_window_id, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        SetFocus( hwnd_old );
    }
#endif
    return( true );

} /* PromptFilesForSave */

/*
 * PromptThisFileForSave
 */
bool PromptThisFileForSave( const char *filename )
{
#ifdef __WIN__
    info        *cinfo;
    HWND        hwnd_old = NO_WINDOW;

    while( isspace( *filename ) ) {
        filename++;
    }
    for( cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
        if( SameFile( cinfo->CurrentFile->name, filename ) ) {
            if( cinfo->CurrentFile != NULL && cinfo->CurrentFile->dup_count == 0 &&
                cinfo->CurrentFile->modified ) {

                BringUpFile( cinfo, true );

                /* we have a modified file, so bring to the front */
                BringWindowToTop( root_window_id );
                hwnd_old = SetFocus( root_window_id );

                // file modified -- so prompt for save
                FilePromptForSaveChanges( CurrentFile );
            }
        }
    }
    if( !BAD_ID( hwnd_old ) ) {
        SetWindowPos( root_window_id, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        SetFocus( hwnd_old );
    }
#else
    filename = filename;
#endif
    return( true );

} /* PromptThisFileForSave */

/*
 * QueryFile
 */
bool QueryFile( const char *filename )
{
    info        *cinfo;

    while( isspace( *filename ) ) {
        filename++;
    }
    for( cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
        if( SameFile( cinfo->CurrentFile->name, filename ) ) {
            return( true );
        }
    }
    return( false );

} /* QueryFile */

/*
 * ExitWithPrompt - try to exit, verifying for every file which has
 * been modified.
 */
bool ExitWithPrompt( bool do_quit, bool push_pop )
{
    info        *cinfo;
    int         i;
    int         num = 0;
    bool        rc = true;

    if( push_pop ) {
        PushMode();
    }
    for( cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
        num++;
    }
    BringUpFile( InfoHead, true );
    for( i = 0; i < num; i++ ){
        if( NextFile() > ERR_NO_ERR ) {
            // file modified ask
            if( FileExitOptionSaveChanges( CurrentFile ) ) {
                /* user hit cancel - always allow this! */
                rc = false;
                break;
            }
        }
    }
    if( push_pop ) {
        PopMode();
    }
    if( rc && do_quit ) {
        QuitEditor( ERR_NO_ERR );
    }
    return( rc );

} /* ExitWithPrompt */

/*
 * ExitWithVerify - try to exit, verifying first
 */
void ExitWithVerify( void )
{
    int         num = 0;
    static bool entered = false;
    info        *cinfo;
    bool        modified;
#ifndef __WIN__
    char        st[MAX_STR];
#endif

    if( entered ) {
        return;
    }
    entered = true;
    modified = false;
    for( cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
        modified |= cinfo->CurrentFile->modified;
        num++;
    }
    if( modified ) {
#ifdef __WIN__
        if( MessageBox( root_window_id, "Files are modified, really exit?",
                         EditorName, MB_YESNO | MB_TASKMODAL ) == IDYES ) {
            BringUpFile( InfoHead, true );
            EditFlags.QuitAtLastFileExit = true;
            for( ;; ) {
                NextFileDammit();
            }
        }
#else
        if( GetResponse( "Files are modified, really exit?", st )
                                    == GOT_RESPONSE && st[0] == 'y' ) {
            BringUpFile( InfoHead, true );
            EditFlags.QuitAtLastFileExit = true;
            for( ;; ) {
                NextFileDammit();
            }
        }
#endif
    } else {
        BringUpFile( InfoHead, true );
        EditFlags.QuitAtLastFileExit = true;
        for( ;; ) {
            NextFileDammit();
        }
    }
    entered = false;

} /* ExitWithVerify */

#if 0
// the old way (super lossy)

bool ExitWithPrompt( bool do_quit )
{
    info        *cinfo, *next;
    int         rc;

    for( cinfo = InfoHead; cinfo != NULL; cinfo = next ) {
        next = cinfo->next;
        if( cinfo->CurrentFile->modified ) {
            /* have to bring up the file first */
            BringUpFile( cinfo, true );
            rc = FileExitOptionSaveChanges( cinfo->CurrentFile );
            if( rc ) {
                /* user hit cancel - always allow this!
                */
                return( false );
            }
        }
    }
    if( do_quit ) {
        QuitEditor( ERR_NO_ERR );
    }
    return( true );

} /* ExitWithPrompt */

/*
 * ExitWithVerify - try to exit, verifying first
 */
void ExitWithVerify( void )
{
    int         i;
    static bool entered = false;
    info        *cinfo;
    bool        modified;
#ifndef __WIN__
    char        st[MAX_STR];
#endif

    if( entered ) {
        return;
    }
    entered = true;
    modified = false;
    for( cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
        modified |= cinfo->CurrentFile->modified;
    }
    if( modified ) {
#ifdef __WIN__
        i = MessageBox( root_window_id, "Files are modified, really exit?",
                        EditorName, MB_YESNO | MB_TASKMODAL );
        if( i == IDYES ) {
            QuitEditor( ERR_NO_ERR );
        }
#else
        i = GetResponse( "Files are modified, really exit?", st );
        if( i == GOT_RESPONSE && st[0] == 'y' ) {
            QuitEditor( ERR_NO_ERR );
        }
#endif
    } else {
        QuitEditor( ERR_NO_ERR );
    }
    entered = false;

} /* ExitWithVerify */
#endif

/*
 * PrintHexValue - print hex value of char under cursor
 */
vi_rc PrintHexValue( void )
{
    int i;

    if( CurrentFile != NULL ) {
        i = CurrentLine->data[CurrentPos.column - 1];
        if( i == '\0' ) {
            // of not on data, pretend are 'on' newline
            i = '\n';
        }
        Message1( "Char '%c': 0x%Z (%d)", (char) i, i, i );
    }

    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* PrintHexValue */

/*
 * EnterHexKey - enter a hexidecimal key stroke and insert it into the text
 */
vi_rc EnterHexKey( void )
{
    int         i;
    char        st[MAX_STR], val;
    vi_rc       rc;
    const char  *ptr;

    rc = ModificationTest();
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    if( CurrentLine->len >= EditVars.MaxLine - 1 ) {
        return( ERR_LINE_FULL );
    }

    rc = PromptForString( "Enter the number of char to insert:", st, sizeof( st ) - 1, NULL );
    if( rc != ERR_NO_ERR ) {
        if( rc == NO_VALUE_ENTERED ) {
            return( ERR_NO_ERR );
        }
        return( rc );
    }

    /*
     * get value
     */
    ptr = SkipLeadingSpaces( st );
    val = (char)strtol( ptr, NULL, 0 );
    if( val == '\0' ) {
        return( ERR_INVALID_VALUE );
    }

    /*
     * build undo record
     */
    StartUndoGroup( UndoStack );
    CurrentLineReplaceUndoStart();
    CurrentLineReplaceUndoEnd( true );
    EndUndoGroup( UndoStack );

    /*
     * add the char
     */
    GetCurrentLine();
    for( i = WorkLine->len; i >= CurrentPos.column - 1; i-- ) {
        WorkLine->data[i + 1] = WorkLine->data[i];
    }
    WorkLine->data[CurrentPos.column - 1] = val;
    WorkLine->len++;
    DisplayWorkLine( true );
    if( CurrentPos.column < WorkLine->len ) {
        GoToColumn( CurrentPos.column + 1, WorkLine->len + 1 );
    }
    ReplaceCurrentLine();
    EditFlags.Dotable = true;
    return( ERR_NO_ERR );

} /* EnterHexKey */

/*
 * DoVersion - display version info
 */
vi_rc DoVersion( void )
{
    Message1( "%s", BANNER1 );
    Message2( "%s", BANNER2 );
    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* DoVersion */

/*
 * StrMerge - merge a number of strings together
 */
char *StrMerge( int cnt, char *str, ... )
{
    va_list     arg;
    char        *n;

    va_start( arg, str );
    for( ; cnt > 0; --cnt ) {
        n = va_arg( arg, char * );
        if( n != NULL ) {
            strcat( str, n );
        }
    }
    va_end( arg );
    return( str );

} /* StrMerge */

/*
 * ModificationTest - test a file as it is about to be modified
 */
vi_rc ModificationTest( void )
{
    vi_rc       rc;
    bool        olddm;
    int         olddotdigits;

    if( CurrentFile == NULL ) {
        return( ERR_NO_FILE );
    }

    if( CurrentFile->viewonly ) {
        return( ERR_FILE_VIEW_ONLY );
    }
    if( !CurrentFile->modified ) {
        olddm = EditFlags.DotMode;
        EditFlags.DotMode = false;
        EditFlags.NoAddToDotBuffer = true;
        olddotdigits = DotDigits;
        rc = SourceHook( SRC_HOOK_MODIFIED, ERR_NO_ERR );
        DotDigits = olddotdigits;
        EditFlags.NoAddToDotBuffer = false;
        EditFlags.DotMode = olddm;
        return( rc );
    }
    return( ERR_NO_ERR );

} /* ModificationTest */

/*
 * CurFileExitOptionSaveChanges - exit current file, opt save if modified
 */
vi_rc CurFileExitOptionSaveChanges( void )
{
    if( NextFile() > ERR_NO_ERR ) {
        FileExitOptionSaveChanges( CurrentFile );
    }
    return( ERR_NO_ERR );
}

/*
 * UpdateCurrentDirectory - update the current directory variable
 */
void UpdateCurrentDirectory( void )
{

    MemFreePtr( (void **)&CurrentDirectory );
    GetCWD1( &CurrentDirectory );

} /* UpdateCurrentDirectory */

#ifndef __WIN__
/*
 * DoAboutBox - do an about box
 */
vi_rc DoAboutBox( void )
{
    return( ERR_NO_ERR );

} /* DoAboutBox */
#endif

/*
 * NextBiggestPrime - the 'lowest common denominator' version (ie simple)
 */
int NextBiggestPrime( int start )
{
    int n = start;
    int i;

    for( ;; ) {
        for( i = 2; i < (int)(n / 2); i++ ) {
            if( i * (n / i) == n ) {
                break;
            }
        }
        if( i == (int)(n / 2) ) {
            break;
        }
        n++;
    }
    return( n );
}

vi_rc FancySetFS( void )
{
#ifdef __WIN__
    GetSetFSDialog();
#endif
    return( ERR_NO_ERR );
}

vi_rc FancySetScr( void )
{
#ifdef __WIN__
    GetSetScrDialog();
#endif
    return( ERR_NO_ERR );
}

vi_rc FancySetGen( void )
{
#ifdef __WIN__
    GetSetGenDialog();
#endif
    return( ERR_NO_ERR );
}

vi_rc ToggleToolbar( void )
{
    char    cmd[14];
    sprintf( cmd, "set%stoolbar", EditFlags.Toolbar ? " no" : " " );
    return( RunCommandLine( cmd ) );
}

vi_rc ToggleStatusbar( void )
{
    char    cmd[17];
    sprintf( cmd, "set%sstatusinfo", EditFlags.StatusInfo ? " no" : " " );
    return( RunCommandLine( cmd ) );
}

vi_rc ToggleColorbar( void )
{
    char    cmd[15];
    sprintf( cmd, "set%scolorbar", EditFlags.Colorbar ? " no" : " " );
    return( RunCommandLine( cmd ) );
}

vi_rc ToggleSSbar( void )
{
    char    cmd[15];
    sprintf( cmd, "set%sssbar", EditFlags.SSbar ? " no" : " " );
    return( RunCommandLine( cmd ) );
}

vi_rc ToggleFontbar( void )
{
    char    cmd[14];
    sprintf( cmd, "set%sfontbar", EditFlags.Fontbar ? " no" : " " );
    return( RunCommandLine( cmd ) );
}

bool GenericQueryBool( char *str )
{
#ifdef __WIN__
    return( MessageBox( root_window_id, str, EditorName, MB_OKCANCEL ) == IDOK );
#else
    #define BUFLEN 10
    char buffer[BUFLEN];
    PromptForString( str, buffer, BUFLEN, NULL );
    return( tolower( buffer[0] ) == 'y' );
#endif
}
