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
#include <time.h>
#include <process.h>
#include <fcntl.h>
#include <share.h>
#include <sys\stat.h>
#include "posix.h"
#include "vi.h"
#include "win.h"
#include "source.h"
#include "fts.h"
#ifdef __WIN__
#include "winrtns.h"
#endif

/*
 * note that the lock file and the data file had better have the
 * same name length!
 */
#define AS_LOCK         "alock_"
#define AS_FILE         "asave_"
#define AS_FILE_EXT     ".fil"
#ifdef __QNX__
#define EXTRA_EXT "0000_"
#define LOCK_NAME_LEN   22
#define EXTRA_EXT_OFF   6
#define CHAR_OFF        16
#else
#define CHAR_OFF        6
#define EXTRA_EXT ""
#define LOCK_NAME_LEN   14
#endif
#define TMP_FNAME_LEN   (TMP_NAME_LEN-6)

#define START_CHAR      'a'
#define END_CHAR        'h'


static bool     noEraseFileList;
static char     currTmpName[TMP_NAME_LEN];
static char     checkFileName[LOCK_NAME_LEN] = AS_FILE EXTRA_EXT "a" AS_FILE_EXT;
static char     checkFileTmpName[LOCK_NAME_LEN] = AS_FILE EXTRA_EXT "at" EXTRA_EXT AS_FILE_EXT;
static char     lockFileName[LOCK_NAME_LEN] = AS_LOCK EXTRA_EXT "a" EXTRA_EXT AS_FILE_EXT;
static int      lockFileHandle;

/*
 * GetCurrentFilePath - build backup path from file name
 */
void GetCurrentFilePath( char *path )
{
    vars        *v;

    v = VarFind( "D", NULL );
    if( v ){
        strcpy( path, v->value );
        v = VarFind( "P", NULL );
        if( v ){
            strcat( path, v->value );
            v = VarFind( "N", NULL );
            if( v ){
                strcat( path, v->value );
                v = VarFind( "E", NULL );
                if( v ){
                    strcat( path, v->value );
                }
            }
        }
    }

} /* GetCurrentFilePath */

/*
 * getTmpName - get tmp name in path
 */
static getTmpName( char *path, char *tmpname )
{
    char        tmp[_MAX_PATH];
    int         i;

    while( 1 ) {
        strcpy( tmp, path );
        strcat( tmp, currTmpName );
        if( access( tmp, F_OK ) == -1 ) {
            break;
        }
        for( i=0;i<TMP_FNAME_LEN;i++ ) {
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
    char        path[_MAX_PATH];
    char        path2[_MAX_PATH];
    char        tmp[_MAX_PATH];
    bool        quiet;
    FILE        *f;
    int         rc;
    int         lastst;

    if( !AutoSaveInterval ) {
        return;
    }
    if( clock() < NextAutoSave ) {
        return;
    }
    if( CurrentFile == NULL ) {
        return;
    }
    if( CurrentFile->is_stdio || CurrentFile->viewonly ||
                !CurrentFile->need_autosave ) {
        SetNextAutoSaveTime();
        return;
    }

    MakeTmpPath( path, "" );
    if( !CurrentFile->been_autosaved ) {
        getTmpName( path, CurrentFile->as_name );
    }
    strcat( path, CurrentFile->as_name );

    quiet = EditFlags.Quiet;
    EditFlags.Quiet = TRUE;
    lastst = UpdateCurrentStatus( CSTATUS_AUTOSAVE );
    rc = SaveFile( path, -1, -1, TRUE );
    EditFlags.Quiet = quiet;
    UpdateCurrentStatus( lastst );
    if( rc != ERR_NO_ERR ) {
        SetNextAutoSaveTime();
        return;
    }

    /*
     * update history file
     */
    CurrentFile->need_autosave = FALSE;
    if( !CurrentFile->been_autosaved ) {
        GetCurrentFilePath( path2 );
        CurrentFile->been_autosaved = TRUE;
        MakeTmpPath( tmp, checkFileName );
        f = fopen( tmp,"a" );
        if( f != NULL ) {
            MyFprintf( f, "%s %s\n", path, path2 );
            fclose( f );
        }
    }

    SetNextAutoSaveTime();

} /* DoAutoSave */

/*
 * handleKey - handle a lost file recover check keystroke
 */
static bool handleKey( char ch )
{

    if( ch == 'i' ) {
        EditFlags.IgnoreLostFiles = TRUE;
    } else if( ch == 'r' ) {
        EditFlags.RecoverLostFiles = TRUE;
        EditFlags.NoInitialFileLoad = TRUE;
    } else if( ch == 'q' ) {
        noEraseFileList = TRUE;
        ExitEditor( -1 );
    } else {
        return( FALSE );
    }
    return( TRUE );

} /* handleKey */

/*
 * LostFileCheck - check if there are any lost files out there
 */
bool LostFileCheck( void )
{
    char        path[_MAX_PATH];
    int         ch;
    int         off;
    int         handle;

    MakeTmpPath( path, lockFileName );
    off = strlen( path ) - 5;
    for( ch =START_CHAR;ch<=END_CHAR;ch++ ) {
        path[ off ] = ch;
        handle = sopen( path, O_RDONLY | O_TEXT, SH_DENYRW );
        if( handle > 0 ) {
            MakeTmpPath( path, checkFileName );
            path[ off ] = ch;
            if( access( path, F_OK ) == -1 ) {
                MakeTmpPath( path, lockFileName );
                path[ off ] = ch;
                close( handle );
                handle = -1;
                remove( path );
            } else {
                break;
            }
        }
    }
    if( handle > 0 ) {
        close( handle );
        if( !EditFlags.RecoverLostFiles ) {
            if( !EditFlags.IgnoreLostFiles ) {
                #ifdef __WIN__
                    CloseStartupDialog();
                    ch = GetAutosaveResponse();
                    handleKey( ch );
                    ShowStartupDialog();
                    return( TRUE );
                #else
                    SetCursorOnScreen( (int) WindMaxHeight - 1, 0 );
                    MyPrintf( "Files have been lost since your last session, do you wish to:\n" );
                    MyPrintf( "\ti)gnore\n\tr)ecover\n\tq)uit\n" );
                    while( 1 ) {
                        ch = GetKeyboard( NULL );
                        if( handleKey( ch ) ) {
                            return( TRUE );
                        }
                    }
                #endif
            } else {
                remove( path );
                return( FALSE );
            }
        }
    } else {
        if( EditFlags.RecoverLostFiles ) {
            EditFlags.RecoverLostFiles = FALSE;
            EditFlags.NoInitialFileLoad = FALSE;
        }
    }
    return( FALSE );

} /* LostFileCheck */

/*
 * AutoSaveInit - initialize for auto-save
 */
void AutoSaveInit( void )
{
    char        path[_MAX_PATH];
    char        path2[_MAX_PATH];
    char        as_path[_MAX_PATH];
    char        asl_path[_MAX_PATH];
    int         len;
    int         cnt;
    FILE        *f;
    int         pid;
    int         ch;
    int         handle;
    int         off;
    int         old_len;

    /*
     * initialize tmpname
     */
    #ifdef __QNX__
        strcpy( currTmpName,"aaaaaaaaaaaa.tmp" );
    #else
        strcpy( currTmpName,"aaaaaaaa.tmp" );
    #endif
    pid = getpid();
    itoa( pid, path, 36 );
    len = strlen( path );
    memcpy( &currTmpName[TMP_FNAME_LEN-len], path, len );
    #ifdef __QNX__
    {
        int     len2,len3;
        int     nid,uid;

        nid = getnid();
        itoa( nid, path, 36 );
        len2 = strlen( path );
        memcpy( &currTmpName[TMP_FNAME_LEN-len-len2], path, len2 );

        uid = getuid();
        itoa( uid, path, 36 );
        len3 = strlen( path );
        memcpy( &currTmpName[TMP_FNAME_LEN-len-len2-len3], path, len3 );
        memcpy( &checkFileName[ EXTRA_EXT_OFF ], path, len3 );
        memcpy( &checkFileTmpName[ EXTRA_EXT_OFF ], path, len3 );
        memcpy( &lockFileName[ EXTRA_EXT_OFF ], path, len3 );
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
        for( ch =START_CHAR;ch<=END_CHAR;ch++ ) {
            as_path[ off ] = ch;
            asl_path[ off ] = ch;
            handle = sopen( as_path, O_RDONLY | O_TEXT, SH_DENYRW );
            if( handle < 0 ) {
                continue;
            }
            f = fdopen( handle, "r" );
            if( f != NULL ) {
                while( fgets( path2, _MAX_PATH, f ) != NULL ) {
                    path2[ strlen( path2 ) - 1 ] = 0;
                    NextWord1( path2, path );
                    RemoveLeadingSpaces( path2 );
                    NewFile( path, FALSE );
                    AddString2( &(CurrentFile->name), path2 );
                    SetFileWindowTitle( CurrentWindow, CurrentInfo, TRUE );
                    FileSPVAR();
                    CurrentFile->modified = TRUE;
                    CurrentFile->check_readonly = TRUE;
                    FTSRunCmds( path2 );
                    cnt++;
                }
                fclose( f );
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
    if( !AutoSaveInterval ) {
        return;
    }

    old_len = strlen( lockFileName );
    MakeTmpPath( path, lockFileName );
    len = strlen( path ) - strlen( lockFileName );
    off = len + CHAR_OFF;
    for( ch =START_CHAR;ch<=END_CHAR;ch++ ) {
        path[ off ] = ch;
        lockFileHandle = sopen( path, O_CREAT | O_TRUNC | O_RDWR |O_TEXT,
                                        SH_DENYRW, S_IREAD | S_IWRITE );
        if( lockFileHandle > 0 ) {
            break;
        }
    }
    if( lockFileHandle < 0 ) {
        // MyPrintf( "Too many editors running!\n" );
        MyPrintf( "Error opening temp file - '%s'\n", strerror( errno ) );
        ExitEditor( -1 );
    }
    lockFileName[ CHAR_OFF ] = ch;
    checkFileName[ CHAR_OFF ] = ch;
    checkFileTmpName[ CHAR_OFF ] = ch;
} /* AutoSaveInit */

/*
 * AutoSaveFini - clean up auto-saved files
 */
void AutoSaveFini( void )
{
    char        path[_MAX_PATH];
    info        *cinfo;

    if( !AutoSaveInterval ) {
        return;
    }
    if( !noEraseFileList ) {
        close( lockFileHandle );
        MakeTmpPath( path, checkFileName );
        remove( path );
        MakeTmpPath( path, lockFileName );
        remove( path );
    }
    cinfo = InfoHead;
    while( cinfo != NULL ) {
        if( cinfo->CurrentFile->been_autosaved ) {
            MakeTmpPath( path, cinfo->CurrentFile->as_name );
            remove( path );
        }
        cinfo = cinfo->next;
    }

} /* AutoSaveFini */

/*
 * SetNextAutoSaveTime - set up when next auto save should take place
 */
void SetNextAutoSaveTime( void )
{
    NextAutoSave = clock() + AutoSaveInterval * (long) CLOCKS_PER_SEC;

} /* SetNextAutoSaveTime */

/*
 * RemoveFromAutoSaveList - take a file that we are quitting out of the list
 */
void RemoveFromAutoSaveList( void )
{
    FILE        *f,*f2;
    char        as_path[_MAX_PATH];
    char        as2_path[_MAX_PATH];
    char        path[_MAX_PATH];
    char        path2[_MAX_PATH];
    char        data[_MAX_PATH];
    bool        found;

    if( !AutoSaveInterval ) {
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

    found = FALSE;
    f = fopen( as_path, "r" );
    if( f == NULL ) {
        return;
    }
    f2 = fopen( as2_path, "w" );
    if( f2 == NULL ) {
        fclose( f );
        return;
    }
    while( fgets( path2, _MAX_PATH, f ) != NULL ) {
        path2[ strlen(path2) - 1 ] = 0;
        NextWord1( path2, data );
        RemoveLeadingSpaces( path2 );
        if( !strcmp( path, path2 ) ) {
            MakeTmpPath( path2, CurrentFile->as_name );
            if( !strcmp( data, path2 ) ) {
                found = TRUE;
                remove( path2 );
                while( fgets( data, _MAX_PATH, f ) != NULL ) {
                    MyFprintf( f2, "%s", data );
                }
                break;
            }
        }
        MyFprintf( f2, "%s %s\n", data, path2 );
    }
    fclose( f );
    fclose( f2 );
    remove( as_path );
    rename( as2_path, as_path );

} /* RemoveFromAutoSaveList */
