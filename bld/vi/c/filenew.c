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
#include <string.h>
#include "posix.h"
#include "vi.h"
#include "source.h"
#include "win.h"
#ifdef __WIN__
#include "winvi.h"
#endif
#include "lang.h"
#include "fts.h"

/*
 * createNewFile - create new file entry
 */
static int createNewFile( char *name, bool same_file )
{
    int         i,rc,height;
    window_id   cw;
    info        *tmp;

    /*
     * test that we can create this file
     */
    if( !same_file ) {
        i = FileExists( name );
        if( !( i == ERR_READ_ONLY_FILE || i==ERR_NO_ERR || i == ERR_FILE_EXISTS ) ) {
            return( i );
        }
    } else {
        if( name == NULL ) {
            tmp = CurrentInfo;
        } else {
            tmp = InfoHead;
            while( tmp != NULL ) {
                if( !strcmp( tmp->CurrentFile->name, name ) ) {
                    break;
                }
                tmp = tmp->next;
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
    i = NewWindow2( &cw, &editw_info );
    if( i ) {
        return( i );
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
        SetFileWindowTitle( CurrentWindow, CurrentInfo, TRUE );
        tmp = CurrentInfo;
        CurrentInfo = MemAlloc( INFO_SIZE );
        FTSRunCmds( name );

        rc = ERR_NO_ERR;
    } else {
        tmp = CurrentInfo;
        CurrentInfo = MemAlloc( INFO_SIZE );
        FTSRunCmds( name );

        CurrentFile = FileAlloc( name );
        height = editw_info.y2-editw_info.y1+1;
        if( EditFlags.CRLFAutoDetect ) {
            EditFlags.WriteCRLF = FALSE;
        }
        for(;;) {
            i = ReadFcbData( CurrentFile );
            if( i ) {
                break;
            }
            if( !CurrentFile->is_stdio ) {
                if( EditFlags.BreakPressed || !EditFlags.ReadEntireFile ) {
                    if( CurrentFile->fcb_tail->end_line > height ) {
                        break;
                    }
                }
            }
        }
        if( i && i != ERR_FILE_NOT_FOUND && i != END_OF_FILE ) {
            MemFree( CurrentInfo );
            CurrentInfo = tmp;
            FileFree( CurrentFile );
            CloseAWindow( cw );
            return( i );
        }
        if( i == ERR_FILE_NOT_FOUND ) {
            rc = NEW_FILE;
            EditFlags.NewFile = TRUE;
            if( EditFlags.CRLFAutoDetect ) {
                CurrentFile->check_for_crlf = TRUE;
            }
        #ifdef __QNX__
            CurrentFile->attr = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                                    S_IROTH | S_IWOTH ;
        #endif
        } else {
            rc = ERR_NO_ERR;
            EditFlags.NewFile = FALSE;
            #ifdef __QNX__
            {
                struct stat     sb;
                stat( name, &sb );
                CurrentFile->attr = sb.st_mode & ~S_IFMT;
            }
            #endif
        }
        CurrentFile->check_readonly = TRUE;
    }

    /*
     * create info entry
     */
    CurrentLineNumber = 0;
    ColumnDesired = CurrentColumn = 1;
    TopOfPage = 1;
    LeftColumn = 0;
    if( !same_file ) {
        AllocateUndoStacks();
    }
    AllocateMarkList();
    CurrentWindow = cw;
    CurrentInfo->DuplicateID = CurrentFile->dup_count;
    CurrentInfo->CurrentWindow = cw;
    LangInit( CurrentInfo->Language );
#ifdef __WIN__
    {
        window_data     *wd;
        wd = DATA_FROM_ID( cw );
        wd->info = CurrentInfo;
    }
#endif
    DCCreate();
    SetFileWindowTitle( CurrentWindow, CurrentInfo, TRUE );

    /*
     * set current file info
     */
    CurrentFcb = CurrentFile->fcb_head;
    CurrentLine = CurrentFcb->line_head;

    if( EditFlags.LineNumbers ) {
        LineNumbersSetup();
    }

    if( tmp != NULL ) {
        InsertLLItemAfter( &InfoTail, tmp, CurrentInfo );
    } else {
        AddLLItemAtEnd( &InfoHead, &InfoTail, CurrentInfo );
    }

    return( rc );

} /* createNewFile */

static int      inReadHook;

/*
 * NewFile - load up a new file
 */
int NewFile( char *name, bool same_file )
{
    int rc;
    int dup;
    int oldstatus;

    dup = EditFlags.DuplicateFile;
    EditFlags.DuplicateFile = FALSE;
    oldstatus = UpdateCurrentStatus( CSTATUS_READING );

    ScreenPage( 1 );
#ifdef __WIN__
    EditFlags.ResizeableWindow = TRUE;
#endif
    rc = createNewFile( name, same_file );
    if( rc && rc != NEW_FILE ) {
        ScreenPage( -1 );
        if( !EditFlags.Starting ) {
            MoveWindowToFrontDammit( MessageWindow, TRUE );
            MoveWindowToFrontDammit( CurrentWindow, TRUE );
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
#else
#endif

    /*
     * reset the screen to the display page, display everything
     */
    ScreenPage( -1 );
    MoveWindowToFrontDammit( CurrentWindow, TRUE );
    UpdateStatusWindow();
    SetWindowCursor();
    DCDisplayAllLines();
    EditFlags.DuplicateFile = dup;
    DisplayFileStatus();
    SaveCurrentInfo();
    ActiveWindow( CurrentWindow );
    VarAddRandC();
    SetModifiedVar( FALSE );
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
        cfile->viewonly = TRUE;
        EditFlags.ViewOnly = FALSE;
    }
    if( EditFlags.StdIOMode ) {
        cfile->is_stdio = TRUE;
        EditFlags.StdIOMode = FALSE;
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
    fcb *cfcb,*tfcb;

    cfcb = cfile->fcb_head;
    while( cfcb != NULL ) {
        tfcb = cfcb->next;
        FreeEntireFcb( cfcb );
        cfcb = tfcb;
    }
    FileFree( cfile );

} /* FreeEntireFile */
