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
#include "source.h"
#include "win.h"
#include "fts.h"

/*
 * createNewFile - create new file entry
 */
static vi_rc createNewFile( char *name, bool same_file )
{
    int         height;
    window_id   cw;
    info        *tmp;
    vi_rc       rc;

    /*
     * test that we can create this file
     */
    tmp = NULL;
    if( !same_file ) {
        rc = FileExists( name );
        if( !(rc == ERR_READ_ONLY_FILE || rc == ERR_NO_ERR || rc == ERR_FILE_EXISTS) ) {
            return( rc );
        }
    } else {
        if( name != NULL ) {
            for( tmp = InfoHead; tmp != NULL; tmp = tmp->next ) {
                if( !strcmp( tmp->CurrentFile->name, name ) ) {
                    break;
                }
            }
        }
        if( tmp == NULL )  {
            return( ERR_FILE_NOT_FOUND );
        }
        if( tmp->CurrentFile->dup_count > MAX_DUPLICATE_FILES ) {
            return( ERR_WIND_NO_MORE_WINDOWS );
        }
    }

    /*
     * get new window
     */
    rc = NewWindow2( &cw, &editw_info );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
#ifdef __WIN__
    if( !strncmp( name, "untitled", 8 ) ) {
        // better yet, pass normal/maximize flag to NewWindow2...
        ShowWindow( cw, SW_SHOWMAXIMIZED );
    }
#endif
    SetBorderGadgets( cw, EditFlags.WindowGadgets );

    /*
     * get new file entry, and read the data
     */

    if( same_file ) {
        CurrentFile = tmp->CurrentFile;
        CurrentFile->dup_count++;
        SetFileWindowTitle( CurrentWindow, CurrentInfo, true );
        tmp = CurrentInfo;
        CurrentInfo = MemAlloc( sizeof( *CurrentInfo ) );
        FTSRunCmds( name );

        rc = ERR_NO_ERR;
    } else {
        bool crlf_reached;

        crlf_reached = false;
        tmp = CurrentInfo;
        CurrentInfo = MemAlloc( sizeof( *CurrentInfo ) );
        FTSRunCmds( name );
        height = editw_info.y2 - editw_info.y1 + 1;

        CurrentFile = FileAlloc( name );
        rc = OpenFcbData( CurrentFile );
        for( ; rc == ERR_NO_ERR; ) {
            rc = ReadFcbData( CurrentFile, &crlf_reached );
            if( rc == ERR_NO_ERR && !CurrentFile->is_stdio ) {
                if( EditFlags.BreakPressed || !EditFlags.ReadEntireFile ) {
                    if( CurrentFile->fcbs.tail->end_line > height ) {
                        break;
                    }
                }
            }
        }
        if( rc != ERR_NO_ERR && rc != ERR_FILE_NOT_FOUND && rc != END_OF_FILE ) {
            MemFree( CurrentInfo );
            CurrentInfo = tmp;
            FileFree( CurrentFile );
            CloseAWindow( cw );
            return( rc );
        }
        if( rc == ERR_FILE_NOT_FOUND ) {
            rc = NEW_FILE;
            EditFlags.NewFile = true;
            CurrentFile->write_crlf = FileSysNeedsCR( CurrentFile->handle );
#ifdef __UNIX__
            CurrentFile->attr = PMODE_RW;
#endif
        } else {
            rc = ERR_NO_ERR;
            CurrentFile->write_crlf = crlf_reached;
            EditFlags.NewFile = false;
#ifdef __UNIX__
            {
                struct stat     sb;
                stat( name, &sb );
                CurrentFile->attr = sb.st_mode & ~S_IFMT;
            }
#endif
        }
        CurrentFile->check_readonly = true;
    }

    /*
     * create info entry
     */
    CurrentPos.line = 0;
    CurrentPos.column = 1;
    VirtualColumnDesired = 1;
    LeftTopPos.line = 1;
    LeftTopPos.column = 0;
    if( !same_file ) {
        AllocateUndoStacks();
    }
    AllocateMarkList();
    CurrentWindow = cw;
    CurrentInfo->DuplicateID = CurrentFile->dup_count;
    CurrentInfo->CurrentWindow = cw;
    LangInit( CurrentInfo->fsi.Language );
#ifdef __WIN__
    {
        window_data     *wd;
        wd = DATA_FROM_ID( cw );
        wd->info = CurrentInfo;
    }
#endif
    DCCreate();
    SetFileWindowTitle( CurrentWindow, CurrentInfo, true );

    /*
     * set current file info
     */
    CurrentFcb = CurrentFile->fcbs.head;
    CurrentLine = CurrentFcb->lines.head;

    if( EditFlags.LineNumbers ) {
        LineNumbersSetup();
    }

    if( tmp != NULL ) {
        InsertLLItemAfter( (ss **)&InfoTail, (ss *)tmp, (ss *)CurrentInfo );
    } else {
        AddLLItemAtEnd( (ss **)&InfoHead, (ss **)&InfoTail, (ss *)CurrentInfo );
    }

    return( rc );

} /* createNewFile */

static int      inReadHook;

/*
 * NewFile - load up a new file
 */
vi_rc NewFile( char *name, bool same_file )
{
    vi_rc       rc;
    bool        dup;
    status_type oldstatus;

    dup = EditFlags.DuplicateFile;
    EditFlags.DuplicateFile = false;
    oldstatus = UpdateCurrentStatus( CSTATUS_READING );

    ScreenPage( 1 );
#ifdef __WIN__
    EditFlags.ResizeableWindow = true;
#endif
    rc = createNewFile( name, same_file );
    if( rc != ERR_NO_ERR && rc != NEW_FILE ) {
        ScreenPage( -1 );
        if( !EditFlags.Starting ) {
            MoveWindowToFrontDammit( MessageWindow, true );
            MoveWindowToFrontDammit( CurrentWindow, true );
        }
        UpdateCurrentStatus( oldstatus );
        return( rc );
    }
    GoToLineNoRelCurs( 1 );
    GoToColumnOnCurrentLine( 1 );
    FileSPVAR();
    SaveCurrentInfo();
    if( !same_file ) {
        inReadHook++;
        rc = SourceHook( SRC_HOOK_READ, rc );
        inReadHook--;
    }

    /*
     * back from hook, so all loadings are done
     * (who should have priority - hook or fts commands?)
     */
#if 0
    rc = FTSRunCmds( CurrentFile->name );
    FTSRunCmds( CurrentFile->name );
#endif

    /*
     * reset the screen to the display page, display everything
     */
    ScreenPage( -1 );
    MoveWindowToFrontDammit( CurrentWindow, true );
    UpdateStatusWindow();
    SetWindowCursor();
    DCDisplayAllLines();
    EditFlags.DuplicateFile = dup;
    DisplayFileStatus();
    SaveCurrentInfo();
    ActiveWindow( CurrentWindow );
    VarAddRandC();
    SetModifiedVar( false );
    UpdateCurrentStatus( oldstatus );
    if( !same_file && !inReadHook ) {
        UpdateLastFileList( CurrentFile->name );
    }
#ifdef __WIN__
    DCUpdateAll();
    ResetEditWindowCursor( CurrentWindow );
    SetWindowCursorForReal();
    GotoFile( CurrentWindow );
#endif
    return( rc );

} /* NewFile */

/*
 * FileAlloc - allocate a file entry
 */
file *FileAlloc( char *name )
{
    file        *cfile;

    /*
     * get new file entry
     */
    cfile = MemAlloc( FILE_SIZE );

    /*
     * initialize file info
     */
    if( name != NULL ) {
        AddString( &(cfile->name), name );
        GetCWD1( &(cfile->home) );
        MyGetFileSize( name, &(cfile->size) );
    }
    cfile->handle = -1;
    if( EditFlags.ViewOnly ) {
        cfile->viewonly = true;
        EditFlags.ViewOnly = false;
    }
    if( EditFlags.StdIOMode ) {
        cfile->is_stdio = true;
        EditFlags.StdIOMode = false;
    }
    return( cfile );

} /* FileAlloc */

/*
 * FileFree - free a file entry (but none of the data)
 */
void FileFree( file *f )
{
    MemFree( f->name );
    MemFree( f->home );
    MemFree( f );

} /* FileFree */

/*
 * FreeEntireFile - free all fcbs in a file
 */
void FreeEntireFile( file *cfile  )
{
    FreeFcbList( cfile->fcbs.head );
    FileFree( cfile );

} /* FreeEntireFile */
