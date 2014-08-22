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
#include <setjmp.h>
#include "source.h"
#include "win.h"
#ifdef __WIN__
    #include "winrtns.h"
#endif
#include <assert.h>

static event *nextEvent( void )
{
    LastEvent = GetNextEvent( true );
    return( &EventList[LastEvent] );
}

static void defaultRange( range *range )
{
    range->start = CurrentPos;
    range->end = CurrentPos;
    range->line_based = false;
    range->highlight = false;
    range->fix_range = false;
}

/*
 * doOperator - process an operator event
 */
static vi_rc doOperator( event *ev )
{
    event       *next;
    vi_rc       rc;
    long        count;
    range       range;
    int         next_type;
    bool        is_op_alias;

    rc = ERR_INVALID_OP;
    defaultRange( &range );
    count = GetRepeatCount();
    is_op_alias = ev->b.is_op_alias;
    if( is_op_alias ) {
        ev->rtn.alias( &ev, &next );
    } else {
        next = nextEvent();
        // a count is the only thing allowed to interfere between an op and
        // its argument so we just keep chewing...
        if( next != &EventList['0'] ) {
            while( next->b.is_number ) {
                if( EditFlags.Modeless ) {
                    next->alt_rtn.old();
                } else {
                    next->rtn.old();
                }
                next = nextEvent();
            }
        }
        count *= GetRepeatCount();
    }
    KillRepeatWindow();
    if( EditFlags.Modeless ) {
        range.fix_range = next->alt_b.fix_range;
        next_type = next->alt_b.type;
    } else {
        range.fix_range = next->b.fix_range;
        next_type = next->b.type;
    }
    EditFlags.OperatorWantsMove = true; // was: if( count==1 )
    if( next_type == EVENT_OP ) {
        // op/op only valid when ops are equal
        if( next == ev ) {
            rc = GetLineRange( &range, count, CurrentPos.line );
        }
    } else {
        // it had better be a move operation
        if( next_type == EVENT_REL_MOVE || next_type == EVENT_ABS_MOVE ) {
            /*
             * Kluge! want 'ce' instead of 'cw' - this sucks.
             *
             * - even better kludge: if we are currently on white
             *   space, then treat 'cw' as 'cw'. arrrrr. cge.
             */
            if( ( ev == &EventList['c'] || ev == &EventList[VI_KEY( ALT_F1 )] ) &&
                        LastEvent == 'w' ) {
                EditFlags.IsChangeWord = true;
                if( CurrentLine != NULL ) {
                    if( !isspace( CurrentLine->data[CurrentPos.column - 1] ) ) {
                        next = &EventList['e'];
                        range.fix_range = false;
                    }
                }
            }
            if( EditFlags.Modeless ) {
                rc = next->alt_rtn.move( &range, count );
            } else {
                rc = next->rtn.move( &range, count );
            }
            EditFlags.IsChangeWord = false;
        } else {
            /*
             * Kluge! treat 'r' as a movement command.
             */
            if( LastEvent == 'r' ) {
                rc = GetSelectedRegion( &range );
            } else if( LastEvent == '0' ) {
                rc = MoveLineBegin( &range, 1 );
            } else if( LastEvent == VI_KEY( ESC ) ) {
                rc = RANGE_REQUEST_CANCELLED;
            }
        }
    }
    EditFlags.OperatorWantsMove = false;
    if( rc == ERR_NO_ERR ) {
        if( ev->b.modifies ) {
            rc = ModificationTest();
        }
        if( rc == ERR_NO_ERR ) {
            NormalizeRange( &range );
            if( EditFlags.Modeless ) {
                rc = ev->alt_rtn.op( &range );
            } else {
                rc = ev->rtn.op( &range );
            }
        }
    }
    return( rc );

} /* doOperator */

/*
 * DoMove - handle a movement command
 */
vi_rc DoMove( event *ev )
{
    range       range;
    vi_rc       rc;
    int         curcol;
    int         type;

    defaultRange( &range );
    if( EditFlags.Modeless ) {
        rc = ev->alt_rtn.move( &range, GetRepeatCount() );
        type = ev->alt_b.type;
    } else {
        rc = ev->rtn.move( &range, GetRepeatCount() );
        type = ev->b.type;
    }
    if( rc == ERR_NO_ERR ) {
        curcol = CurrentPos.column;
        if( range.start.line != CurrentPos.line ) {
            if( type == EVENT_REL_MOVE ) {
                GoToLineRelCurs( range.start.line );
            } else {
                curcol = -1;
                MemorizeCurrentContext();
                GoToLineNoRelCurs( range.start.line );
            }
        }
        if( curcol != range.start.column ) {
            GoToColumnOK( range.start.column );
        }
#ifndef __WIN__
        if( range.highlight ) {
            // don't handle multi-line highlights yet
            assert( range.hi_start.line == range.hi_end.line );
            EditFlags.ResetDisplayLine = true;
            DCUpdate();
            HiliteAColumnRange( range.hi_start.line,
                range.hi_start.column, range.hi_end.column );
        }
#endif
    }
    return( rc );
}

static void ensureCursorDisplayed( void )
{
    int         len, wc, diff;

    if( EditFlags.Modeless && ( CurrentFile != NULL ) ) {
        len = WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_LINES );
        if( CurrentPos.line < LeftTopPos.line ||
            CurrentPos.line > LeftTopPos.line + len - 1 ) {
            SetCurrentLine( CurrentPos.line );
        }

        wc = VirtualColumnOnCurrentLine( CurrentPos.column ) - LeftTopPos.column;
        if( !ColumnInWindow( wc, &diff ) ) {
            SetCurrentColumn( CurrentPos.column );
        }
    }
}

/*
 * DoLastEvent - process the last keystroke event
 */
vi_rc DoLastEvent( void )
{
    event       *event;
    vi_rc       rc;
    bool        keep_sel;

    if( LastEvent >= MAX_EVENTS ) {
        rc = InvalidKey();
    } else if( !EditFlags.Modeless && EditFlags.InsertModeActive ) {
//        UnselectRegion();
        if( EditFlags.EscapedInsertChar ) {
            rc = IMChar();
        } else if( InputKeyMaps[LastEvent].data != NULL && !InputKeyMaps[LastEvent].inuse ) {
            rc = StartInputKeyMap( LastEvent );
        } else {
            rc = (EventList[LastEvent].ins)();
        }
    } else if( !EditFlags.Modeless && KeyMaps[LastEvent].data != NULL && !KeyMaps[LastEvent].inuse ) {
        rc = DoKeyMap( LastEvent );
    } else {
        event = &EventList[LastEvent];
        keep_sel = event->b.keep_selection;
        if( event->b.keep_selection_maybe ) {
            if( SelRgn.selected ) {
                keep_sel = true;
            }
        }
        if( !keep_sel && !EditFlags.ScrollCommand ) {
            UnselectRegion();
        }
        if( !EditFlags.ScrollCommand ) {
            ensureCursorDisplayed();
        }
        rc = ERR_NO_ERR;
        switch( event->b.type ) {
        case EVENT_INS:
            if( EditFlags.Modeless ) {
                /* don't allow following cursor movements to affect seln
                 */
                keep_sel = SelRgn.selected;
                DeleteSelectedRegion();
                SelRgn.selected = keep_sel;
                InsertLikeLast();
                rc = event->rtn.ins();
                DoneCurrentInsert( false );
                SelRgn.selected = false;
            } else {
                if( !EditFlags.InsertModeActive ) {
                    InsertLikeLast();
                }
                rc = event->rtn.ins();
            }
            break;
        case EVENT_OP:
            DoneCurrentInsert( true );
            rc = doOperator( event );
            break;
        case EVENT_REL_MOVE:
        case EVENT_ABS_MOVE:
            DoneCurrentInsert( true );
            rc = DoMove( event );
            break;
        case EVENT_MISC:
            DoneCurrentInsert( true );
            rc = event->rtn.old();
            break;
        }
    }
    return( rc );

} /* DoLastEvent */

/*
 * DoneLastEvent - finished with the last event
 */
void DoneLastEvent( vi_rc rc, bool is_dotmode )
{
    if( !EditFlags.InsertModeActive ) {
        /*
         * save last command for '.'
         */
        if( !is_dotmode ) {
            if( EditFlags.DotMode ) {
                EditFlags.DotMode = false;
            } else if( EditFlags.Dotable && !EditFlags.MemorizeMode ) {
                SaveDotCmd();
            }
        }

        /*
         * reset number and and buffer number
         */
        if( rc != GOT_A_SAVEBUF ) {
            if( rc != GOT_A_DIGIT ) {
                /* repeat count should not reset save buf */
                SavebufNumber = NO_SAVEBUF;
                DoneRepeat();
            }
        }

        if( !is_dotmode ) {
            EditFlags.Dotable = false;
        }
    }

} /* DoneLastEvent */

/*
 * EditMain - main driver for editor (command mode)
 */
void EditMain( void )
{
    vi_rc       rc;
    char        *msg = NULL;
    bool        doclear;

    /*
     * loop forever, or at least until all done
     */
    for( ;; ) {

#if 0
#ifdef __WIN__
        PushMode();
        UpdateFiles();
        PopMode();
#endif
#endif
        if( !EditFlags.InsertModeActive || EditFlags.Modeless ) {
            if( EditFlags.Modeless ) {
                UpdateEditStatus();
                EditFlags.NoCapsLock = false;
            } else {
                UpdateCurrentStatus( CSTATUS_COMMAND );
                EditFlags.NoCapsLock = true;
            }

            if( !EditFlags.Modeless && EditFlags.ReturnToInsertMode &&
                                !NonKeyboardEventsPending() ) {
                EditFlags.ReturnToInsertMode = false;
                if( EditFlags.WasOverstrike ) {
                    LastEvent = 'R';
                } else {
                    LastEvent = 'i';
                }
            } else {
                DCUpdateAll();
#ifdef __WIN__
                SetWindowCursorForReal();
#endif
                LastEvent = GetNextEvent( true );
            }
            EditFlags.NoCapsLock = false;
            doclear = true;
            if( LastEvent == VI_KEY( MOUSEEVENT ) ) {
                if( LastMouseEvent == MOUSE_MOVE ) {
                    doclear = false;
                }
            }
            if( doclear ) {
                if( EditFlags.AutoMessageClear ) {
                    ClearWindow( MessageWindow );
                }
#ifndef __WIN__
                ResetDisplayLine();
#endif
            }
        } else {
            // Cannot do a RestoreInfo while we are in insert mode
            // because it will call ValidateCurrentColumn which will
            // do something stupid on us... PushMode/PopMode solution
            // not working yet... this needs a little work
            DCUpdate();
#ifdef __WIN__
            SetWindowCursorForReal();
#endif
            LastEvent = GetNextEvent( true );
        }

        rc = DoLastEvent();

        if( EditFlags.ReadOnlyError && rc <= ERR_NO_ERR ) {
            EditFlags.ReadOnlyError = false;
            rc = ERR_READ_ONLY_FILE_MODIFIED;
        }
        if( rc > ERR_NO_ERR ) {
            msg = GetErrorMsg( rc );
        }

        DoneLastEvent( rc, false );

        if( rc > ERR_NO_ERR ) {
            Error( msg );
        }

    }

} /* EditMain */

/*
 * AbsoluteNullResponse - give no response
 */
vi_rc AbsoluteNullResponse( void )
{
    return( ERR_NO_ERR );
}

/*
 * NullResponse - give null response for keystroke
 */
vi_rc NullResponse( void )
{
    if( !EditFlags.EscapeMessage ) {
        ClearWindow( MessageWindow );
    } else {
        DisplayFileStatus();
    }
    UnselectRegion();
    return( ERR_NO_ERR );

} /* NullResponse */

static window_id        repeatWindow = NO_WINDOW;

/*
 * KillRepeatWindow - just like it says
 */
void KillRepeatWindow( void )
{
    if( repeatWindow != NO_WINDOW ) {
        CloseAWindow( repeatWindow );
        repeatWindow = NO_WINDOW;
    }
}

/*
 * DoneRepeat - done getting repeat count
 */
void DoneRepeat( void )
{
    RepeatDigits = 0;
    if( !EditFlags.MemorizeMode ) {
        DotDigits = 0;
    }
    KillRepeatWindow();

} /* DoneRepeat */

/*
 * SetRepeatCount - set up a fake repeat count
 */
void SetRepeatCount( long num )
{
    sprintf( RepeatString, "%ld", num );
    RepeatDigits = strlen( RepeatString );

} /* SetRepeatCount */

/*
 * GetRepeatCount - return repeat count and reset it
 */
long GetRepeatCount( void )
{
    long        i;

    if( RepeatDigits == 0 ) {
        NoRepeatInfo = true;
        return( 1L );
    }
    i = atol( RepeatString );
    RepeatDigits = 0;
    NoRepeatInfo = false;
    return( i );

} /* GetRepeatCount */

#ifndef __WIN__
#define UpdateRepeatString( str ) DisplayLineInWindow( repeatWindow, 1, str )
#else
extern void UpdateRepeatString( char *str );
#endif

/*
 * DoDigit - process a digit typed in
 */
vi_rc DoDigit( void )
{
    vi_rc   rc;

    if( LastEvent == '0' && RepeatDigits == 0 ) {
        LeftTopPos.column = 0;
        GoToColumnOK( 1 );
        DCDisplayAllLines();
        return( ERR_NO_ERR );
    }

    if( RepeatDigits == MAX_REPEAT_STRING - 1 ) {
        DoneRepeat();
        return( ERR_REPEAT_STRING_TOO_LONG );
    }

    if( repeatWindow == NO_WINDOW && EditFlags.RepeatInfo ) {
        rc = NewWindow2( &repeatWindow, &repcntw_info );
        if( rc != ERR_NO_ERR ) {
            DoneRepeat();
            return( rc );
        }
        WindowTitle( repeatWindow, "Repeat Count" );
    }

    RepeatString[RepeatDigits++] = LastEvent;
    RepeatString[RepeatDigits] = 0;
    if( repeatWindow != NO_WINDOW ) {
        UpdateRepeatString( RepeatString );
    }
    return( GOT_A_DIGIT );

} /* DoDigit */

/*
 * InvalidKey - process invalid keystroke
 */
vi_rc InvalidKey( void )
{
    Error( GetErrorMsg( ERR_INVALID_KEY ), LastEvent );
    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* InvalidKey */

/*
 * Modified - set file modified flag
 */
void Modified( bool f )
{
    if( f ) {
        CurrentFile->need_autosave = true;
        if( EditFlags.ReadOnlyCheck ) {
            if( CFileReadOnly() ) {
                EditFlags.ReadOnlyError = true;
            }
        }
    }
    if( CurrentFile->modified != f  ) {
        CurrentFile->modified = f;
        SetModifiedVar( f );
    }
    UnselectRegion();

} /* Modified */

/*
 * ResetDisplayLine - reset display line, if required (after hilight)
 */
void ResetDisplayLine( void )
{
    if( EditFlags.ResetDisplayLine ) {
        memcpy( WorkLine->data, CurrentLine->data, CurrentLine->len + 1 );
        WorkLine->len = CurrentLine->len;
        DisplayWorkLine( false );
        DCUpdate();
        WorkLine->len = -1;
        EditFlags.ResetDisplayLine = false;
    }

} /* ResetDisplayLine */
