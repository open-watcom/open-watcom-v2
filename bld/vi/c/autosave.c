/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Autosave support.
*
****************************************************************************/


#include "vi.h"
#include <time.h>
#include <errno.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
  #include <process.h>
#endif
#include "sopen.h"
#include "posix.h"
#include "win.h"
#include "fts.h"
#include "tempio.h"
#ifdef __WIN__
  #include "winrtns.h"
#endif
#include "myio.h"

#include "clibext.h"


/*
 * note that the lock file and the data file had better have the
 * same name length!
 */
#define AS_LOCK             "alock_"
#define AS_FILE             "asave_"
#define AS_FILE_EXT         ".fil"
#ifdef __UNIX__
    #define EXTRA_EXT       "0000_"
    #define EXTRA_EXT_OFF   6
    #define CHAR_OFF        16
#else
    #define CHAR_OFF        6
    #define EXTRA_EXT       ""
#endif
#define TMP_FNAME_LEN       (TMP_NAME_LEN - 6)

#define START_CHAR          'a'
#define END_CHAR            'h'


static bool     noEraseFileList;
static char     currTmpName[TMP_NAME_LEN];
static char     checkFileName[] = AS_FILE EXTRA_EXT "a" AS_FILE_EXT;
static char     checkFileTmpName[] = AS_FILE EXTRA_EXT "at" EXTRA_EXT AS_FILE_EXT;
static char     lockFileName[] = AS_LOCK EXTRA_EXT "a" EXTRA_EXT AS_FILE_EXT;
static int      lockFileHandle = -1;

/*
 * GetCurrentFilePath - build backup path from file name
 */
void GetCurrentFilePath( char *path )
{
    vars        *v;

    v = GlobVarFind( GLOBVAR_FILEDRIVE );
    if( v != NULL ) {
        strcpy( path, v->value );
        v = GlobVarFind( GLOBVAR_FILEPATH );
        if( v != NULL ) {
            strcat( path, v->value );
            v = GlobVarFind( GLOBVAR_FILENAME );
            if( v != NULL ) {
                strcat( path, v->value );
                v = GlobVarFind( GLOBVAR_FILEEXT );
                if( v != NULL ) {
                    strcat( path, v->value );
                }
            }
        }
    }

} /* GetCurrentFilePath */

/*
 * getTmpName - get tmp name in path
 */
static void getTmpName( char *path, char *tmpname )
{
    char        tmp[FILENAME_MAX];
    int         i;

    for( ;; ) {
        strcpy( tmp, path );
        strcat( tmp, currTmpName );
        if( access( tmp, F_OK ) == -1 ) {
            break;
        }
        for( i = 0; i < TMP_FNAME_LEN; i++ ) {
            currTmpName[i]++;
            if( currTmpName[i] < 'z' ) {
                break;
            }
            currTmpName[i] = 'a';
        }
    }
    strcpy( tmpname, currTmpName );

} /* getTmpName */

/*
 * DoAutoSave - try to do autosave of current file
 */
void DoAutoSave( void )
{
    char        path[FILENAME_MAX];
    char        path2[FILENAME_MAX];
    char        tmp[FILENAME_MAX];
    bool        quiet;
    FILE        *fp;
    vi_rc       rc;
    status_type lastst;

    if( EditVars.AutoSaveInterval == 0 ) {
        return;
    }
    if( clock() < NextAutoSave ) {
        return;
    }
    if( CurrentFile == NULL ) {
        return;
    }
    if( CurrentFile->is_stdio || CurrentFile->viewonly || !CurrentFile->need_autosave ) {
        SetNextAutoSaveTime();
        return;
    }

    MakeTmpPath( path, "" );
    if( !CurrentFile->been_autosaved ) {
        getTmpName( path, CurrentFile->as_name );
    }
    strcat( path, CurrentFile->as_name );

    quiet = EditFlags.Quiet;
    EditFlags.Quiet = true;
    lastst = UpdateCurrentStatus( CSTATUS_AUTOSAVE );
    rc = SaveFile( path, -1, -1, true );
    EditFlags.Quiet = quiet;
    UpdateCurrentStatus( lastst );
    if( rc != ERR_NO_ERR ) {
        SetNextAutoSaveTime();
        return;
    }

    /*
     * update history file
     */
    CurrentFile->need_autosave = false;
    if( !CurrentFile->been_autosaved ) {
        GetCurrentFilePath( path2 );
        CurrentFile->been_autosaved = true;
        MakeTmpPath( tmp, checkFileName );
        fp = fopen( tmp, "a" );
        if( fp != NULL ) {
            MyFprintf( fp, "%s %s\n", path, path2 );
            fclose( fp );
        }
    }

    SetNextAutoSaveTime();

} /* DoAutoSave */

/*
 * handleKey - handle a lost file recover check keystroke
 */
static bool handleKey( vi_key key )
{
    if( key == VI_KEY( i ) ) {
        EditFlags.IgnoreLostFiles = true;
    } else if( key == VI_KEY( r ) ) {
        EditFlags.RecoverLostFiles = true;
        EditFlags.NoInitialFileLoad = true;
    } else if( key == VI_KEY( q ) ) {
        noEraseFileList = true;
        ExitEditor( -1 );
    } else {
        return( false );
    }
    return( true );

} /* handleKey */

/*
 * LostFileCheck - check if there are any lost files out there
 */
bool LostFileCheck( void )
{
    char        path[FILENAME_MAX];
    vi_key      key;
    char        ch;
    size_t      off;
    int         handle = -1;

    MakeTmpPath( path, lockFileName );
    off = strlen( path ) - 5;
    for( ch = START_CHAR; ch <= END_CHAR; ch++ ) {
        path[off] = ch;
        handle = _sopen3( path, O_RDONLY | O_TEXT, SH_DENYRW );
        if( handle < 0 )
            continue;
        MakeTmpPath( path, checkFileName );
        path[off] = ch;
        if( access( path, F_OK ) == -1 ) {
            MakeTmpPath( path, lockFileName );
            path[off] = ch;
            close( handle );
            handle = -1;
            remove( path );
        } else {
            break;
        }
    }
    if( handle >= 0 ) {
        close( handle );
        if( !EditFlags.RecoverLostFiles ) {
            if( !EditFlags.IgnoreLostFiles ) {
#ifdef __WIN__
                key = GetAutosaveResponse();
                handleKey( key );
                return( true );
#else
                SetPosToMessageLine();
                MyPrintf( "Files have been lost since your last session, do you wish to:\n" );
                MyPrintf( "\ti)gnore\n\tr)ecover\n\tq)uit\n" );
                for( ;; ) {
                    key = GetKeyboard();
                    if( handleKey( key ) ) {
                        return( true );
                    }
                }
#endif
            } else {
                remove( path );
                return( false );
            }
        }
    } else {
        if( EditFlags.RecoverLostFiles ) {
            EditFlags.RecoverLostFiles = false;
            EditFlags.NoInitialFileLoad = false;
        }
    }
    return( false );

} /* LostFileCheck */

/*
 * AutoSaveInit - initialize for auto-save
 */
void AutoSaveInit( void )
{
    char        path[FILENAME_MAX];
    char        path2[FILENAME_MAX];
    char        as_path[FILENAME_MAX];
    char        asl_path[FILENAME_MAX];
    size_t      len;
    int         cnt;
    FILE        *fp;
    int         pid;
    int         ch;
    int         handle;
    size_t      off;
//    int         old_len;
    const char  *p;

    /*
     * initialize tmpname
     */
#ifdef __UNIX__
    strcpy( currTmpName,"aaaaaaaaaaaa.tmp" );
#else
    strcpy( currTmpName,"aaaaaaaa.tmp" );
#endif
    pid = getpid();
    itoa( pid, path, 36 );
    len = strlen( path );
    memcpy( &currTmpName[TMP_FNAME_LEN - len], path, len );
#ifdef __QNX__
    {
        size_t  len2, len3;
        int     nid, uid;

        nid = getnid();
        itoa( nid, path, 36 );
        len2 = strlen( path );
        memcpy( &currTmpName[TMP_FNAME_LEN - len - len2], path, len2 );

        uid = getuid();
        itoa( uid, path, 36 );
        len3 = strlen( path );
        memcpy( &currTmpName[TMP_FNAME_LEN - len - len2 - len3], path, len3 );
        memcpy( &checkFileName[EXTRA_EXT_OFF], path, len3 );
        memcpy( &checkFileTmpName[EXTRA_EXT_OFF], path, len3 );
        memcpy( &lockFileName[EXTRA_EXT_OFF], path, len3 );
    }
#endif

    /*
     * check if we need to recover lost files
     */
    if( EditFlags.RecoverLostFiles ) {
        cnt = 0;
        MakeTmpPath( as_path, checkFileName );
        MakeTmpPath( asl_path, lockFileName );
        off = strlen( as_path ) - 5;
        for( ch = START_CHAR; ch <= END_CHAR; ch++ ) {
            as_path[off] = (char)ch;
            asl_path[off] = (char)ch;
            handle = _sopen3( as_path, O_RDONLY | O_TEXT, SH_DENYRW );
            if( handle < 0 )
                continue;
            fp = fdopen( handle, "r" );
            if( fp != NULL ) {
                while( (p = myfgets( path2, sizeof( path2 ), fp )) != NULL ) {
                    p = GetNextWord1( p, path );
                    NewFile( path, false );
                    ReplaceString( &(CurrentFile->name), p );
                    SetFileWindowTitle( current_window_id, CurrentInfo, true );
                    FileSPVAR();
                    CurrentFile->modified = true;
                    CurrentFile->check_readonly = true;
                    FTSRunCmds( p );
                    cnt++;
                }
                fclose( fp );       // close handle
                remove( as_path );
            } else {
                close( handle );
            }
            remove( asl_path );
        }
        if( cnt == 0 ) {
            MyPrintf( "No files recovered!\n" );
            ExitEditor( -1 );
        }
        Message1( "%d files recovered", cnt );

    }
    if( EditVars.AutoSaveInterval == 0 ) {
        return;
    }

//    old_len = strlen( lockFileName );
    MakeTmpPath( path, lockFileName );
    len = strlen( path ) - strlen( lockFileName );
    off = len + CHAR_OFF;
    for( ch = START_CHAR; ch <= END_CHAR; ch++ ) {
        path[off] = (char)ch;
        lockFileHandle = _sopen4( path, O_CREAT | O_TRUNC | O_RDWR | O_TEXT, SH_DENYRW, PMODE_RW );
        if( lockFileHandle >= 0 ) {
            break;
        }
    }
    if( lockFileHandle < 0 ) {
        // MyPrintf( "Too many editors running!\n" );
        MyPrintf( "Error opening temp file - '%s'\n", strerror( errno ) );
        ExitEditor( -1 );
    }
    lockFileName[CHAR_OFF] = (char)ch;
    checkFileName[CHAR_OFF] = (char)ch;
    checkFileTmpName[CHAR_OFF] = (char)ch;

} /* AutoSaveInit */

/*
 * AutoSaveFini - clean up auto-saved files
 */
void AutoSaveFini( void )
{
    char        path[FILENAME_MAX];
    info        *cinfo;

    if( EditVars.AutoSaveInterval == 0 ) {
        return;
    }
    if( !noEraseFileList ) {
        close( lockFileHandle );
        MakeTmpPath( path, checkFileName );
        remove( path );
        MakeTmpPath( path, lockFileName );
        remove( path );
    }
    for( cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
        if( cinfo->CurrentFile->been_autosaved ) {
            MakeTmpPath( path, cinfo->CurrentFile->as_name );
            remove( path );
        }
    }

} /* AutoSaveFini */

/*
 * SetNextAutoSaveTime - set up when next auto save should take place
 */
void SetNextAutoSaveTime( void )
{
    NextAutoSave = clock() + EditVars.AutoSaveInterval * (long)CLOCKS_PER_SEC;

} /* SetNextAutoSaveTime */

/*
 * RemoveFromAutoSaveList - take a file that we are quitting out of the list
 */
void RemoveFromAutoSaveList( void )
{
    FILE        *fpi, *fpo;
    char        as_path[FILENAME_MAX];
    char        as2_path[FILENAME_MAX];
    char        path[FILENAME_MAX];
    char        path2[FILENAME_MAX];
    char        data[FILENAME_MAX];
//    bool        found;
    const char  *p;

    if( EditVars.AutoSaveInterval == 0 ) {
        return;
    }
    if( CurrentFile == NULL ) {
        return;
    }
    if( !CurrentFile->been_autosaved ) {
        return;
    }

    MakeTmpPath( as_path, checkFileName );
    MakeTmpPath( as2_path, checkFileTmpName );

    GetCurrentFilePath( path );

//    found = false;
    fpi = fopen( as_path, "r" );
    if( fpi == NULL ) {
        return;
    }
    fpo = fopen( as2_path, "w" );
    if( fpo == NULL ) {
        fclose( fpi );
        return;
    }
    while( (p = myfgets( path2, sizeof( path2 ), fpi )) != NULL ) {
        p = GetNextWord1( p, data );
        if( strcmp( path, p ) == 0 ) {
            p = MakeTmpPath( path2, CurrentFile->as_name );
            if( strcmp( data, p ) == 0 ) {
//                found = true;
                remove( p );
                while( myfgets( data, sizeof( data ), fpi ) != NULL ) {
                    MyFprintf( fpo, "%s\n", data );
                }
                break;
            }
        }
        MyFprintf( fpo, "%s %s\n", data, p );
    }
    fclose( fpi );
    fclose( fpo );
    remove( as_path );
    rename( as2_path, as_path );

} /* RemoveFromAutoSaveList */
