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
* Description:  Text substitution.
*
****************************************************************************/


#include "vi.h"
#include "walloca.h"
#include "rxsupp.h"
#include "win.h"

typedef enum {
    CHANGE_OK,
    CHANGE_NO,
    CHANGE_CANCEL,
    CHANGE_ALL
} change_resp;

// LastSubstituteCancelled is a global used to perform an interactive
// search and replace in 2 parts
int LastSubstituteCancelled;
int LastChangeCount;
int LastLineCount;


#ifdef __WIN__
static HHOOK    hhookMB = 0;
static int      MB_posx = -1;
static int      MB_posy = -1;

LRESULT CALLBACK MyMessageBoxWndFunc( int ncode, WPARAM wparam, LPARAM lparam )
{
    char        className[10];
    HWND        hWnd;

    if( ncode == HCBT_ACTIVATE || ncode == HCBT_MOVESIZE ) {
        hWnd = (HWND)wparam;
        GetClassName( hWnd, className, 10 );
        if( strcmp( className, "#32770" ) == 0 ) {
            if( ncode == HCBT_MOVESIZE ) {
                LPRECT  pos = (LPRECT)lparam;

                MB_posx = pos->left;
                MB_posy = pos->top;
            } else {
                if( MB_posx != -1 || MB_posy != -1 ) {
                    SetWindowPos( hWnd, (HWND)NULLHANDLE, MB_posx, MB_posy, 0, 0,
                        SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOREDRAW | SWP_NOZORDER );
                }
            }
        }
    }
    return( CallNextHookEx( hhookMB, ncode, wparam, lparam ) );
}

static int MyMessageBox( window_id hWnd, char _FAR *lpText, char _FAR *lpCaption, unsigned uType )
{
    HOOKPROC    lpfn;
    int         rc;

    lpfn = (HOOKPROC)MakeProcInstance( (FARPROC)MyMessageBoxWndFunc, InstanceHandle );
#if defined(__NT__)
    hhookMB = SetWindowsHookEx( WH_CBT, lpfn, 0, GetCurrentThreadId() );
#else
    hhookMB = SetWindowsHookEx( WH_CBT, lpfn, InstanceHandle, GetCurrentTask() );
#endif
    rc = MessageBox( hWnd, lpText, lpCaption, uType );
    UnhookWindowsHookEx( hhookMB );
    FreeProcInstance( (FARPROC)lpfn );
    return( rc );
}
#endif

static change_resp ChangePrompt( void )
{
#ifdef __WIN__
    int     i;

    i = MyMessageBox( Root, "Change this occurence?", "Replace Text",
                      MB_ICONQUESTION | MB_YESNOCANCEL );
    if( i == IDNO ) {
        return( CHANGE_NO );
    } else if( i == IDCANCEL ) {
        return( CHANGE_CANCEL );
    } else {
        return( CHANGE_OK );
    }
#else
    vi_key      key = 0;

    Message1( "Change? (y)es/(n)o/(a)ll/(q)uit" );
    while( TRUE ) {
        key = GetNextEvent( FALSE );
        if( key == VI_KEY( y ) ) {
            return( CHANGE_OK );
        } else if( key == VI_KEY( n ) ) {
            return( CHANGE_NO );
        } else if( key == VI_KEY( a ) ) {
            return( CHANGE_ALL );
        } else if( key == VI_KEY( q ) ) {
            return( CHANGE_CANCEL );
        }
    }
#endif
}

/* TwoPartSubstitute - goes from current line to current line minus 1
 *                     doing substitute in 2 parts if it has to, that
 *                     appear as 1
 */
vi_rc TwoPartSubstitute( char *find, char *replace, int prompt, int wrap )
{
    vi_rc   rc;
    long    changecnt, linecnt;
    linenum end_line;

    char *cmd = MemAlloc( MAX_INPUT_LINE );

    StartUndoGroup( UndoStack );

    // search from current position forward to end of doc
    sprintf( cmd, "/%s/%s/g%c", find, replace, ( prompt == TRUE ) ? 'i' : '\0' );

    end_line = CurrentFile->fcbs.tail->end_line;
    rc = Substitute( CurrentPos.line, end_line, cmd );
    changecnt = LastChangeCount;
    linecnt = LastLineCount;
    if( wrap && !LastSubstituteCancelled && CurrentPos.line != 1 &&
        rc == ERR_NO_ERR ) {
        // search from beginning of do to here
        sprintf( cmd, "/%s/%s/g%c", find, replace, (prompt == TRUE) ? 'i' : '\0' );
        rc = Substitute( 1, CurrentPos.line - 1, cmd );
        linecnt += LastLineCount;
        changecnt += LastChangeCount;
    }
    if( rc == ERR_NO_ERR ) {
        Message1( "%l changes on %l lines", changecnt, linecnt );
    }
    EndUndoGroup( UndoStack );

    MemFree( cmd );
    return( rc );

} /* TwoPartSubstitute */

static void nextSearchStartPos( i_mark *pos, bool gflag, int rlen )
{
    pos->column += rlen;
    if( gflag == 0 || CurrentLine->data[pos->column] == '\0' ) {
        pos->line++;
        pos->column = 0;
    }
}

/*
 * Substitute - perform substitution
 */
vi_rc Substitute( linenum n1, linenum n2, char *data )
{
    char        *sstr, *rstr, *newr;
    char        flag[20], *linedata;
    bool        iflag = FALSE, gflag = FALSE, undoflag = FALSE, restline = FALSE;
    bool        splitpending = FALSE, undoline = FALSE;
    int         i, rlen, slen;
    int         splitme;
    long        changecnt = 0, linecnt = 0;
    linenum     llineno, ll, lastline = 0, extra;
    i_mark      pos;
    vi_rc       rc;

    LastSubstituteCancelled = 0;
    LastChangeCount = 0;
    LastLineCount = 0;

    sstr = alloca( MAX_INPUT_LINE );
    if( sstr == NULL ) {
        return( ERR_NO_STACK );
    }
    strcpy( sstr, data );
    if( rc = ModificationTest() ) {
        return( rc );
    }
    strcpy( data, sstr );
    rstr = alloca( MAX_INPUT_LINE  );
    if( rstr == NULL ) {
        return( ERR_NO_STACK );
    }
    if( NextWordSlash( data, sstr ) < 0 ) {
        return( ERR_INVALID_SUBS_CMD );
    }
    if( NextWordSlash( data, rstr ) < 0 ) {
        return( ERR_INVALID_SUBS_CMD );
    }
    slen = NextWord1( data, flag );
    for( i = 0; i < slen; i++ ) {
        switch( flag[i] ) {
        case 'g':
            gflag = TRUE;
            break;
        case 'i':
        case 'c':
            iflag = TRUE;
            break;
        }
    }
    rc = CurrentRegComp( sstr );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }

    /*
     * verify last line
     */
    if( n2 > CurrentFile->fcbs.tail->end_line ) {
        rc = CFindLastLine( &ll );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
        if( n2 > ll ) {
            return( ERR_INVALID_LINE_RANGE );
        }
    }

    /*
     * set for start of search
     */
    if( EditFlags.Verbose && EditFlags.EchoOn ) {
        ClearWindow( MessageWindow );
    }
    SaveCurrentFilePos();
    llineno = n1 - 1;

    EditFlags.AllowRegSubNewline = TRUE;
    newr = StaticAlloc();
    for( pos.column = 0, pos.line = n1;
        pos.line <= n2;
        nextSearchStartPos( &pos, gflag, rlen ) ) {

        /*
         * get regular expression, and build replacement string
         */
        rc = FindRegularExpression( NULL, &pos, &linedata, n2, 0 );
        if( rc != ERR_NO_ERR || pos.line > n2 ) {
            break;
        }

        slen = GetCurrRegExpLength();
        splitme = RegSub( CurrentRegularExpression, rstr, newr, pos.line );
        rlen = strlen( newr );

        ProcessingMessage( pos.line );

        /*
         * if in global mode, see if we already have an undo for
         * this line
         */
        if( gflag ) {
            if( lastline != pos.line ) {
                undoline = FALSE;
            }
        }

        /*
         * interactive mode? yes, then display text and ask to change
         */
        if( iflag ) {
            change_resp rsp;

            if( !restline ) {
                ClearWindow( MessageWindow );
            }
            restline = TRUE;
            GoToLineNoRelCurs( pos.line );
            if( EditFlags.GlobalInProgress ) {
                EditFlags.DisplayHold = FALSE;
                DCDisplayAllLines();
                EditFlags.DisplayHold = TRUE;
            }
            HilightSearchString( &pos, slen );
            rsp = ChangePrompt();
            if( rsp == CHANGE_NO ) {
                ResetDisplayLine();
                rlen = 1;
                continue;
            } else if( rsp == CHANGE_CANCEL ) {
                ResetDisplayLine();
                LastSubstituteCancelled = 1;
                break;
            } else if( rsp == CHANGE_ALL ) {
                ResetDisplayLine();
                iflag = FALSE;
            }
        }

        /*
         * set up for global undo if we haven't already
         */
        if( !undoflag ) {
            StartUndoGroup( UndoStack );
            undoflag = TRUE;
        }

        /*
         * bump change counts
         */
        changecnt++;
        if( llineno != pos.line ) {
            if( splitpending ) {
                splitpending = FALSE;
                extra = SplitUpLine( llineno );
                n2 += extra;
                pos.line += extra;
            }
            linecnt++;
            llineno = pos.line;
        }

        /*
         * get copy of line, and verify that new stuff fits
         */
        CurrentPos.line = pos.line;
        rc = CGimmeLinePtr( pos.line, &CurrentFcb, &CurrentLine );
        if( rc != ERR_NO_ERR ) {
            break;
        }
        if( CurrentLine->len + rlen - slen >= MaxLine ) {
            rc = ERR_LINE_FULL;
            break;
        }

        /*
         * now build the individual undo
         */
        CurrentFcb->non_swappable = TRUE;
        if( !undoline ) {
            CurrentLineReplaceUndoStart();
            CurrentLineReplaceUndoEnd( TRUE );
            if( gflag ) {
                undoline = TRUE;
                lastline = pos.line;
            }
        }

        /*
         * remove the old string
         */
        GetCurrentLine();
        WorkLine->len = ReplaceSubString( WorkLine->data, WorkLine->len,
                                          pos.column, pos.column + slen - 1, newr, rlen );
        if( iflag ) {
            DisplayWorkLine( TRUE );
        }
        ReplaceCurrentLine();

        /*
         * if not global, only do this change on this line
         */
        if( splitme ) {
            splitpending = TRUE;
        }
        CurrentFcb->non_swappable = FALSE;
    }
    StaticFree( newr );
    EditFlags.AllowRegSubNewline = FALSE;
    /*
    * is there still a split line pending?
    */
    if( splitpending ) {
        SplitUpLine( llineno );
    }
    RestoreCurrentFilePos();
    if( restline ) {
        SetCurrentLine( CurrentPos.line );
        GoToColumnOK( CurrentPos.column );
    }
    if( undoflag ) {
        EndUndoGroup( UndoStack );
    }
    switch( rc ) {
    case ERR_NO_ERR:
    case ERR_LINE_FULL:
    case ERR_FIND_PAST_TERM_LINE:
    case ERR_FIND_NOT_FOUND:
    case ERR_FIND_END_OF_FILE:
        /*
        * display results
        */
        if( rc == ERR_LINE_FULL ) {
            Message1( "Stopped at line %l - line full", pos.line );
        } else {
            Message1( "%l changes on %l lines", changecnt, linecnt );
            LastLineCount = linecnt;
            LastChangeCount = changecnt;
        }
        DCDisplayAllLines();
        rc = ERR_NO_ERR;
        break;
    default:
        break;
    }
    return( rc );

} /* Substitute */

/*
 * SplitUpLine - split up a line with SPLIT_CHAR's in them
 */
linenum SplitUpLine( linenum cl )
{
    linenum     extra = 0;
    int         j, i, k;
    char        *buff;

    /*
     * run through, and for every 0x01, make a new line
     */
    while( 1 ) {

        /*
         * get current line
         */
        CurrentPos.line = cl + extra;
        CGimmeLinePtr( CurrentPos.line, &CurrentFcb, &CurrentLine );
        GetCurrentLine();

        for( i = 0; i <= WorkLine->len; i++ ) {
            /*
             * found a place to split.  make this line shorter,
             * and create a new line with the rest of the data
             * for this line
             */
            if( WorkLine->data[i] == SPLIT_CHAR ) {
                buff = StaticAlloc();
                k = 0;
                for( j = i + 1; j <= WorkLine->len; j++ ) {
                    buff[k++] = WorkLine->data[j];
                }
                WorkLine->data[i] = 0;
                WorkLine->len = i;
                ReplaceCurrentLine();
                AddNewLineAroundCurrent( buff, k - 1, INSERT_AFTER );
                extra++;
                StaticFree( buff );
                break;
            }

            /*
             * at the very end, undo what we did and go back
             */
            if( WorkLine->data[i] == 0 ) {
                ReplaceCurrentLine();
                UndoInsert( cl + 1, cl + extra, UndoStack );
                return( extra );
            }
        }
    }

} /* SplitUpLine */
