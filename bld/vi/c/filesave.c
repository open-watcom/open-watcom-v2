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


#include "vi.h"
#include "posix.h"
#include <fcntl.h>
#include <errno.h>
#include "source.h"
#ifdef __WIN__
    #include "utils.h"
#endif

static int fileHandle;

/*
 * writeRange - write a range of lines in an fcb to current file
 */
static vi_rc writeRange( linenum s, linenum e, fcb *cfcb, long *bytecnt )
{
    line        *cline;
    int         i, len = 0;
    char        *buff, *data;
    vi_rc       rc;

    if( s > e ) {
        return( ERR_NO_ERR );
    }

    rc = GimmeLinePtrFromFcb( s, cfcb, &cline );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    buff = WriteBuffer;

    /*
     * copy data into buffer
     */
    while( s <= e ) {

        data = cline->data;
        while( *data != 0 ) {
            *buff = *data;
            buff++;
            data++;
        }
        len += cline->len;
        if( EditFlags.WriteCRLF ) {
            *buff++ = 13;
            len++;
        }
        *buff++ = 10;
        len++;
        cline = cline->next;
        s++;

    }

    /*
     * now write the buffer
     */
    if( fileHandle == 0 ) {
        i = fwrite( WriteBuffer, 1, len, stdout );
    } else {
        i = write( fileHandle, WriteBuffer, len );
    }
    if( i != len ) {
        return( ERR_FILE_WRITE );
    }

    *bytecnt += (long) len;
    return( ERR_NO_ERR );

} /* writeRange */

/*
 * readOnlyCheck - check if writing read-only file okay
 */
static int readOnlyCheck( void )
{
    char        tmp[MAX_STR];
#ifndef __WIN__
    char        st[MAX_STR];
#endif

    MySprintf( tmp, "\"%s\" is read-only, overwrite?", CurrentFile->name );
#ifdef __WIN__
    if( MessageBox( Root, tmp, EditorName, MB_YESNO | MB_TASKMODAL ) == IDYES ) {
        return( ERR_NO_ERR );
    }
    return( ERR_READ_ONLY_FILE );
#else
    if( GetResponse( tmp, st ) == GOT_RESPONSE && st[0] == 'y' ) {
        return( ERR_NO_ERR );
    }
    return( ERR_READ_ONLY_FILE );
#endif

} /* readOnlyCheck */

#ifdef __WIN__
/*
 * SaveFileAs - save data from current file
 */
vi_rc SaveFileAs( void )
{
    char    fn[FILENAME_MAX];
    char    cmd[14 + FILENAME_MAX];
    vi_rc   rc;

    rc = SelectFileSave( fn );
    if( rc != ERR_NO_ERR || fn[0] == '\0' ) {
        return( rc );
    }
    // rename current file
#ifndef __NT__   // this is stupid for all case-preserving systems like NT
    FileLower( fn );          
#endif    
    sprintf( cmd, "set filename \"%s\"", fn );
    RunCommandLine( cmd );
    UpdateLastFileList( fn );

    // flag dammit as user must have already said overwrite ok
    return( SaveFile( fn, -1L, -1L, TRUE ) );

} /* SaveFileAs */
#endif

/*
 * SaveFile - save data from current file
 */
vi_rc SaveFile( char *name, linenum start, linenum end, int dammit )
{
    int         i;
    bool        existflag = FALSE, restpath = FALSE, makerw = FALSE;
    char        *fn;
    fcb         *cfcb, *sfcb, *efcb;
    linenum     s, e, lc;
    long        bc = 0;
    status_type lastst;
    vi_rc       rc;

    if( CurrentFile == NULL ) {
        return( ERR_NO_FILE );
    }

    /*
     * get file name
     */
    if( name == NULL ) {
        if( CurrentFile->viewonly ) {
            return( ERR_FILE_VIEW_ONLY );
        }
        if( CFileReadOnly() ) {
            rc = readOnlyCheck();
            if( rc != ERR_NO_ERR ) {
                return( ERR_READ_ONLY_FILE );
            }
            makerw = TRUE;
        }
        fn = CurrentFile->name;
        restpath = TRUE;
    } else {
        existflag = TRUE;
        fn = name;
    }
    if( fn[0] == 0 ) {
        return( ERR_NO_FILE_NAME );
    }
    if( SameFile( fn, CurrentFile->name ) ) {
        if( CurrentFile->viewonly ) {
            return( ERR_FILE_VIEW_ONLY );
        }
    }
    if( dammit ) {
        existflag = FALSE;
    }

    /*
     * get range and fcbs
     */
    if( start == -1L && end == -1L ) {
        s = 1L;
        rc = CFindLastLine( &e );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
    } else {
        s = start;
        e = end;
    }
    lc = e - s + 1;
    rc = FindFcbWithLine( s, CurrentFile, &sfcb );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    rc = FindFcbWithLine( e, CurrentFile, &efcb );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }

    if( restpath ) {
        rc = ChangeDirectory( CurrentFile->home );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
    }
    if( !CurrentFile->is_stdio ) {
        if( makerw ) {
            chmod( fn, S_IWRITE | S_IREAD );
        }
        rc = FileOpen( fn, existflag, O_TRUNC | O_WRONLY | O_BINARY | O_CREAT,
                      WRITEATTRS, &fileHandle);
        if( rc != ERR_NO_ERR ) {
            Message1( strerror( errno ) );
            return( rc );
        }
    } else {
        fileHandle = 0;
#ifdef __WATCOMC__
        setmode( fileno( stdout ), O_BINARY );
#endif
    }

    /*
     * start writing fcbs
     */
#ifdef __WIN__
    ToggleHourglass( TRUE );
#endif
    if( CurrentFile->check_for_crlf ) {
        if( fileHandle ) {
            EditFlags.WriteCRLF = FALSE;
#ifndef __LINUX__
            if( FileSysNeedsCR( fileHandle ) ) {
                EditFlags.WriteCRLF = TRUE;
            }
#endif 
        } else {
#ifdef __UNIX__
            EditFlags.WriteCRLF = FALSE;
#else
            EditFlags.WriteCRLF = TRUE;
#endif
        }
    }
    lastst = UpdateCurrentStatus( CSTATUS_WRITING );
    cfcb = sfcb;
    while( cfcb != efcb ) {

        rc = writeRange( s, cfcb->end_line, cfcb, &bc );
        if( rc != ERR_NO_ERR ) {
#ifdef __WIN__
            ToggleHourglass( FALSE );
#endif
            UpdateCurrentStatus( lastst );
            return( rc );
        }
        s = cfcb->end_line + 1;
        cfcb = cfcb->next;

    }

    /*
     * last bit
     */
    rc = writeRange( s, e, efcb, &bc );
#ifdef __WIN__
    ToggleHourglass( FALSE );
#endif
    UpdateCurrentStatus( lastst );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    if( !CurrentFile->is_stdio ) {
        i = close( fileHandle );
        if( makerw ) {
            chmod( fn, S_IREAD );
        }
        if( i == -1 ) {
            Message1( strerror( errno ) );
            return( ERR_FILE_CLOSE );
        }
    }
    if( restpath ) {
        rc = ChangeDirectory( CurrentDirectory );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
    }

    FileIOMessage( fn, lc, bc );
    return( ERR_NO_ERR );

} /* SaveFile */

/*
 * StartSaveExit - prepare to do save & exit of file
 */
vi_rc StartSaveExit( void )
{
    vi_key  key;
    vi_key  levent;

    /*
     * get the next key
     */
    levent = LastEvent;
    key = GetNextEvent( FALSE );
    if( key != levent ) {
        if( key == VI_KEY( ESC ) ) {
            return( ERR_NO_ERR );
        }
        return( InvalidKey() );
    }
    return( SaveAndExit( NULL ) );

} /* StartSaveExit */

/*
 * SaveAndExit - save and exit a file
 */
vi_rc SaveAndExit( char *fname )
{
    vi_rc   rc;

    /*
     * save file and get next one
     */
    if( CurrentFile != NULL ){
        if( CurrentFile->modified ) {
            rc = SourceHook( SRC_HOOK_WRITE, ERR_NO_ERR );
            if( rc != ERR_NO_ERR ) {
                return( rc );
            }
            rc = SaveFile( fname, -1, -1, FALSE );
            if( rc != ERR_NO_ERR ) {
                return( rc );
            }
            Modified( FALSE );
        }
    }
    return( NextFile() );

} /* SaveAndExit */

/*
 * FilePromptForSaveChanges - give option to save file if modified
 */
bool FilePromptForSaveChanges( file *f )
{
    char    buffer[MAX_STR];
    vi_rc   rc;

#ifdef __WIN__
    MySprintf( buffer, "\"%s\" has been modified - save changes?",
        f->name );
    BringWindowToTop( Root );
    SetWindowPos( Root, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
    if( MessageBox( Root, buffer, EditorName, MB_YESNO | MB_TASKMODAL ) == IDYES ) {
        rc = SaveFile( NULL, -1, -1, FALSE );
        if( rc != ERR_NO_ERR ) {
            MySprintf( buffer, "Error saving \"%s\"", f->name );
            MessageBox( Root, buffer, EditorName, MB_OK | MB_TASKMODAL );
        } else {
            Modified( FALSE );
        }
    }
    SetWindowPos( Root, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
    SetWindowPos( Root, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
#else
    char    response[MAX_SRC_LINE];

//  MySprintf( buffer, "\"%s\" has been modified - save changes (yes|no|cancel)?",
    MySprintf( buffer, "\"%s\" has been modified - save changes (yes|no)?",
        f->name );
    if( GetResponse( buffer, response ) == GOT_RESPONSE ) {
        switch( response[0] ) {
        case 0:
            // if the user hit ENTER then the buffer will be
            // a string of 0 chars so act as if y had been hit
        case 'y':
        case 'Y':
            rc = SaveFile( NULL, -1, -1, FALSE );
            if( rc != ERR_NO_ERR ) {
                MySprintf( buffer, "Error saving \"%s\"", f->name );
                Message1( buffer );
            } else {
                Modified( FALSE );
            }
            break;
        }
    }
#endif
    /* would return TRUE if we supported a CANCEL option
     * the same as the FileExitOptionSaveChanges function below */
    return( FALSE );

} /* FilePromptForSaveChanges */

/*
 * FileExitOptionSaveChanges - exit file, giving option to save if modified
 */
bool FileExitOptionSaveChanges( file *f )
{
    bool        aborted = FALSE;
    char        buffer[MAX_STR];
#ifdef __WIN__
    int         resp;
    vi_rc       rc;

    MySprintf( buffer, "\"%s\" has been modified - save changes?", f->name );
    resp = MessageBox( Root, buffer, EditorName, MB_YESNOCANCEL | MB_TASKMODAL );
    if( resp == IDYES ) {
        rc = SaveFile( NULL, -1, -1, FALSE );
        if( rc != ERR_NO_ERR ) {
            MySprintf( buffer, "Error saving \"%s\"", f->name );
            MessageBox( Root, buffer, EditorName, MB_OK | MB_TASKMODAL );
            aborted = TRUE;
        } else {
            NextFileDammit();
        }
    } else if( resp == IDCANCEL ) {
        aborted = TRUE;
    } else {
        NextFileDammit();
    }
#else
    char response[MAX_SRC_LINE];

    MySprintf( buffer, "\"%s\" has been modified - save changes (yes|no|cancel)?",
        f->name );
    if( GetResponse( buffer, response ) == GOT_RESPONSE ) {
        switch( response[0] ) {
        case 0:
            // if the user hit ENTER then the buffer will be
            // a string of 0 chars so act as if y had been hit
        case 'y':
        case 'Y':
            SaveAndExit( NULL );
            break;
        case 'n':
        case 'N':
            NextFileDammit();
            break;
        case 'c':
        case 'C':
        default:
            aborted = TRUE;
            // return( FALSE );
        }
    } else {
        aborted = TRUE;
    }
#endif

    return aborted;

} /* FileOptionExitSaveChanges */

/*
 * FancyFileSave
 */
vi_rc FancyFileSave( void )
{
    vi_rc       rc;

    if( CurrentFile == NULL ) {
        return( ERR_NO_FILE );
    }
    rc = SaveFile( CurrentFile->name, -1, -1, TRUE );
    if( rc == ERR_NO_ERR ) {
        CurrentFile->modified = FALSE;
        UpdateLastFileList( CurrentFile->name );
    }
    return( rc );

} /* FancyFileSave */

/*
 * DoKeyboardSave - handle the CTRL+S keyboard shortcut
 */
vi_rc DoKeyboardSave( void )
{
#ifdef __WIN__
    vi_rc   rc;
    char    fname[_MAX_FNAME];

    if( CurrentFile != NULL ) {
        _splitpath( CurrentFile->name, NULL, NULL, fname, NULL );
    } else {
        fname[0] = 0;
    }

    if( strcmp( fname, "untitled" ) == 0 ) {
        rc = SaveFileAs();
    } else {
        rc = FancyFileSave();
    }
    return( rc );
#else
    return( FancyFileSave() );
#endif

} /* DoKeyboardSave */

