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
#include "source.h"
#include "win.h"
#include "fts.h"
#include <assert.h>

/*
 * NextFileDammit - done with current file, get next one
 */
vi_rc NextFileDammit( void )
{
    info        *cinfo, *tinfo;
    vi_ushort   lastid;
    int         dont_exit_this_time = 0;

    RemoveFromAutoSaveList();

    /*
     * get rid of current info item and get next
     */
    if( CurrentInfo != NULL ) {
        // decrease ref count on language info
        LangFini( CurrentInfo->fsi.Language );
        DCDestroy();

        if( EditFlags.QuitMovesForward ) {
            cinfo = CurrentInfo->next;
        } else {
            cinfo = CurrentInfo->prev;
        }
        lastid = CurrentInfo->DuplicateID;
        MemFree( DeleteLLItem( (ss **)&InfoHead, (ss **)&InfoTail, (ss *)CurrentInfo ) );
        CurrentInfo = NULL;

        /*
         * free data associated with this file
         */
        FreeMarkList();
        if( CurrentFile->dup_count > 0 ) {
            CurrentFile->dup_count--;
            for( tinfo = InfoHead; tinfo != NULL; tinfo = tinfo->next ) {
                if( tinfo->CurrentFile == CurrentFile ) {
                    if( tinfo->DuplicateID > lastid ) {
                        tinfo->DuplicateID--;
                        SetFileWindowTitle( tinfo->CurrentWindow, tinfo, false );
                    } else if( CurrentFile->dup_count == 0 ) {
                        SetFileWindowTitle( tinfo->CurrentWindow, tinfo, false );
                    }
                }
            }
        } else {
            FreeUndoStacks();
            FreeEntireFile( CurrentFile );
            // CurrentFile = NULL;
        }
#ifdef __WIN__
        CloseAChildWindow( CurrentWindow );
        CurrentWindow = NO_WINDOW;
#endif
        dont_exit_this_time = 1;

    } else {
        cinfo = NULL;
    }
    if( cinfo == NULL ) {
        if( EditFlags.QuitMovesForward ) {
            cinfo = InfoHead;
        } else {
            cinfo = InfoTail;
        }
    }

    if( cinfo == NULL ) {
        if( EditFlags.QuitAtLastFileExit || !dont_exit_this_time ) {
            CurrentWindow = NO_WINDOW;
            // EditFlags.Quiet = true;
            if( CommandBuffer ) {
                MemFree( CommandBuffer );
            }
            QuitEditor( ERR_NO_ERR );
        }
    }
#ifndef __WIN__
    else {
        CloseAWindow( CurrentWindow );
        CurrentWindow = NO_WINDOW;
    }
#endif
    /*
     * now, set up with next file
     */
    if( cinfo ) {
        BringUpFile( cinfo, true );
    } else {
        CurrentFile = NULL;
    }

    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* NextFileDammit */

/*
 * NextFile - go to next file, checking modified flag
 */
vi_rc NextFile( void )
{
    if( CurrentFile != NULL ) {
        if( CurrentFile->dup_count == 0 ) {
            if( CurrentFile->modified ) {
                return( ERR_FILE_MODIFIED );
            }
        }
    }
    return( NextFileDammit() );

} /* NextFile */


/*
 * nextFile - go to next file
 */
static vi_rc nextFile( info *cinfo )
{
    SaveCurrentInfo();
    BringUpFile( cinfo, false );
    if( cinfo ) {
        // FTSRunCmds( cinfo->CurrentFile->name );
    }
    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* nextFile */

/*
 * RotateFileForward - rotate forward through current file list
 */
vi_rc RotateFileForward( void )
{
    info        *cinfo;

    if( CurrentInfo == NULL ) {
        return( ERR_NO_ERR );
    }
    cinfo = CurrentInfo->next;
    if( cinfo == NULL ) {
        cinfo = InfoHead;
    }
    return( nextFile( cinfo ) );

} /* RotateFileForward */

/*
 * RotateFileBackwards - rotate forward through current file list
 */
vi_rc RotateFileBackwards( void )
{
    info        *cinfo;

    if( CurrentInfo == NULL ) {
        return( ERR_NO_ERR );
    }
    cinfo = CurrentInfo->prev;
    if( cinfo == NULL ) {
        cinfo = InfoTail;
    }
    return( nextFile( cinfo ) );

} /* RotateFileBackwards */

/*
 * GotoFile - bring up file in filelist with given window id
 */
vi_rc GotoFile( window_id id )
{
    info        *cinfo;

    if( InfoHead == NULL ) {
        return( ERR_NO_ERR );
    }

    for( cinfo = InfoHead; cinfo->next != NULL; cinfo = cinfo->next ) {
        if( cinfo->CurrentWindow == id ) {
            break;
        }
    }
    assert( cinfo != NULL );

    return( nextFile( cinfo ) );
}

/*
 * BringUpFile - bring up a new file
 */
void BringUpFile( info *ci, bool runCmds )
{
    window_id   wn;
    static bool recursive = false;

    if( recursive ) {
        return;
    }

    SourceHook( SRC_HOOK_BUFFOUT, ERR_NO_ERR );
    wn = CurrentWindow;
    if( RestoreInfo( ci ) ) {
        LineNumbersSetup();
    }
    EditFlags.Dragging = false;
    MoveWindowToFrontDammit( CurrentWindow, false );
    if( EditFlags.LineNumbers ) {
        MoveWindowToFrontDammit( CurrNumWindow, false );
    }
    DisplayFileStatus();
    UpdateStatusWindow();
    SetWindowCursor();
    if( wn != CurrentWindow && wn != NO_WINDOW ) {
        InactiveWindow( wn );
    }
    ActiveWindow( CurrentWindow );
    UnselectRegion();
    if( ci != NULL && ci->CurrentFile->dup_count > 0 ) {
        DCDisplayAllLines();
    }
    FileSPVAR();
    SourceHook( SRC_HOOK_BUFFIN, ERR_NO_ERR );

    // be careful when runCmds true!  Some commands redraw the screen,
    // which calls BringUpFile, which (if runCmds = true) will run cmds...
    if( runCmds && ci != NULL ) {
        recursive = true;
        FTSRunCmds( ci->CurrentFile->name );
        recursive = false;
    }

#ifdef __WIN__
    if( CurrentInfo && AllowDisplay ) {
        DCDisplayAllLines();
        DCUpdate();
        // set cursor after display cache complete
        SetWindowCursor();
        SetWindowCursorForReal();
    }
#endif

} /* BringUpFile */
