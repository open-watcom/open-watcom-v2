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
#include <malloc.h>
#include "vi.h"
#include "rxsupp.h"
#include "win.h"
#ifdef __WIN__
#include "winvi.h"
#endif

// LastSubstituteCancelled is a global used to perform an interactive
// search and replace in 2 parts
int LastSubstituteCancelled;
int LastChangeCount;
int LastLineCount;

/* TwoPartSubstitute - goes from current line to current line minus 1
 *                     doing substitute in 2 parts if it has to, that
 *                     appear as 1
 */
int TwoPartSubstitute( char *find, char *replace, int prompt, int wrap ){
    int rc;
    long changecnt, linecnt;
    linenum end_line;

    char *cmd = MemAlloc( MAX_INPUT_LINE );

    StartUndoGroup( UndoStack );

    // search from current position forward to end of doc
    sprintf( cmd, "/%s/%s/g%c", find, replace, ( prompt == TRUE ) ? 'i' : '\0' );

    end_line = CurrentFile->fcb_tail->end_line;
    rc = Substitute( CurrentLineNumber, end_line, cmd );
    changecnt = LastChangeCount;
    linecnt = LastLineCount;
    if( wrap && !LastSubstituteCancelled && CurrentLineNumber != 1 && rc == ERR_NO_ERR ) {
        // search from beginning of do to here
        sprintf( cmd, "/%s/%s/g%c", find, replace, ( prompt == TRUE ) ? 'i' : '\0' );
        rc = Substitute( 1,CurrentLineNumber-1, cmd );
        linecnt += LastLineCount;
        changecnt += LastChangeCount;
    }
    if( rc == ERR_NO_ERR ) {
        Message1( "%l changes on %l lines",changecnt,linecnt );
    }
    EndUndoGroup( UndoStack );

    MemFree( cmd );
    return( rc );
} /* TwoPartSubstitute */

/*
 * Substitute - perform substitution
 */
int Substitute( linenum n1, linenum n2, char *data )
{
    char        *sstr,*rstr,*newr;
    char        flag[20],*linedata;
    bool        iflag=FALSE,gflag=FALSE,undoflag=FALSE,restline=FALSE;
    bool        splitpending=FALSE,undoline=FALSE;
    int         i,rlen,slen,key;
    int         ccol,rc,splitme,k;
    long        changecnt=0,linecnt=0;
    linenum     clineno,llineno,ll,lastline=0,extra;

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
    if( (k=NextWord1( data, flag ) ) >= 0 ) {
        for( i=0;i<k;i++ ) {
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
    }
    i = CurrentRegComp( sstr );
    if( i ) {
        return( i );
    }

    /*
     * verify last line
     */
    if( n2 > CurrentFile->fcb_tail->end_line ) {
        i = CFindLastLine( &ll );
        if( i ) {
            return( i );
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
    llineno = n1-1;
    clineno = n1;
    ccol = 0;

    EditFlags.AllowRegSubNewline = TRUE;
    newr = StaticAlloc();
    while( TRUE ) {

        /*
         * get regular expression, and build replacement string
         */
        i = FindRegularExpression( NULL, &clineno, ccol, &linedata, n2, FALSE );
        if( !i ) {
            ccol = GetCurrRegExpColumn( linedata );
            slen = GetCurrRegExpLength();
        } else {
            if( i == ERR_FIND_PAST_TERM_LINE || i == ERR_FIND_NOT_FOUND || i == ERR_FIND_END_OF_FILE ) {
                break;
            }
            RestoreCurrentFilePos();
            EditFlags.AllowRegSubNewline = FALSE;
            return( i );
        }

        if( clineno > n2 ) {
            break;
        }

        splitme = RegSub( CurrentRegularExpression, rstr, newr, clineno );
        rlen = strlen( newr );

        ProcessingMessage( clineno );

        /*
         * if in global mode, see if we already have an undo for
         * this line
         */
        if( gflag ) {
            if( lastline != clineno ) {
                undoline = FALSE;
            }
        }

        /*
         * interactive mode? yes, then display text and ask to change
         */
        if( iflag ) {
            if( !restline ) {
                ClearWindow( MessageWindow );
            }
            restline = TRUE;
            GoToLineNoRelCurs( clineno );
            if( EditFlags.GlobalInProgress ) {
                EditFlags.DisplayHold = FALSE;
                DCDisplayAllLines();
                EditFlags.DisplayHold = TRUE;
            }
            HilightSearchString( clineno, ccol, slen );
            #ifdef __WIN__
                key = MessageBox( Root, "Change this occurence?", "Replace Text",
                                  MB_ICONQUESTION | MB_YESNOCANCEL );
                switch( key ) {
                case IDNO:
                    ResetDisplayLine();
                    rlen = 1;
                    goto TRYNEXTMATCH;
                case IDCANCEL:
                    ResetDisplayLine();
                    LastSubstituteCancelled = 1;
                    goto DONEALLREPLACEMENTS;
                }
                if( key == 0 ) {
            #endif
                    Message1( "Change? (y)es/(n)o/(a)ll/(q)uit" );
                    key = 0;
                    while( key != 'y' ) {
                        key = GetNextEvent( FALSE );
                        switch( key ) {
                        case 'a':
                                ResetDisplayLine();
                                iflag = FALSE;
                                key='y';
                                break;
                        case 'q':
                                ResetDisplayLine();
                                goto DONEALLREPLACEMENTS;
                        case 'n':
                                ResetDisplayLine();
                                rlen = 1;
                                goto TRYNEXTMATCH;
                        }
                    }
            #ifdef __WIN__
                }
            #endif
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
        if( llineno != clineno ) {
            if( splitpending ) {
                splitpending = FALSE;
                extra = SplitUpLine( llineno );
                n2 += extra;
                clineno += extra;
            }
            linecnt++;
            llineno = clineno;
        }

        /*
         * get copy of line, and verify that new stuff fits
         */
        CurrentLineNumber = clineno;
        i = CGimmeLinePtr( clineno, &CurrentFcb, &CurrentLine );
        if( i ) {
            RestoreCurrentFilePos();
            EditFlags.AllowRegSubNewline = FALSE;
            StaticFree( newr );
            return( i );
        }
        if( CurrentLine->len + rlen - slen >= MaxLine ) {
            rc=ERR_LINE_FULL;
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
                lastline = clineno;
            }
        }

        /*
         * remove the old string
         */
        GetCurrentLine();
        WorkLine->len = ReplaceSubString( WorkLine->data, WorkLine->len,
                                    ccol, ccol+slen-1, newr, rlen );
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
TRYNEXTMATCH:
        if( gflag ) {
            ccol += rlen;
            if( (slen == 0 && rlen == 0) || CurrentLine->data[ ccol ] == 0 ) {
                clineno++;
                if( clineno > n2 ) {
                    break;
                }
                ccol = 0;
            }
        } else {
            clineno++;
            if( clineno > n2 ) {
                break;
            }
            ccol = 0;
        }

    }

    /*
     * is there still a split line pending?
     */
DONEALLREPLACEMENTS:
    if( splitpending ) {
        SplitUpLine( llineno );
    }

    /*
     * display results
     */
    RestoreCurrentFilePos();
    EditFlags.AllowRegSubNewline = FALSE;
    if( restline ) {
        SetCurrentLine( CurrentLineNumber );
        GoToColumnOK( CurrentColumn );
    }
    if( undoflag ) {
        EndUndoGroup( UndoStack );
    }
    if( rc == ERR_LINE_FULL ) {
        Message1( "Stopped at line %l - line full", clineno );
    } else {
        Message1( "%l changes on %l lines",changecnt,linecnt );
        LastLineCount = linecnt;
        LastChangeCount = changecnt;
    }
    DCDisplayAllLines();
    StaticFree( newr );
    return( ERR_NO_ERR );

} /* Substitute */

/*
 * SplitUpLine - split up a line with SPLIT_CHAR's in them
 */
linenum SplitUpLine( linenum cl )
{
    linenum     extra = 0;
    int         j,i,k;
    char        *buff;

    /*
     * run through, and for every 0x01, make a new line
     */
    while( 1 ) {

        /*
         * get current line
         */
        CurrentLineNumber = cl+extra;
        CGimmeLinePtr( CurrentLineNumber, &CurrentFcb, &CurrentLine );
        GetCurrentLine();

        for( i=0;i<=WorkLine->len;i++ ) {
            /*
             * found a place to split.  make this line shorter,
             * and create a new line with the rest of the data
             * for this line
             */
            if( WorkLine->data[i] == SPLIT_CHAR ) {
                buff = StaticAlloc();
                k = 0;
                for( j=i+1;j<=WorkLine->len;j++ ) {
                    buff[k++] = WorkLine->data[j];
                }
                WorkLine->data[i] = 0;
                WorkLine->len = i;
                ReplaceCurrentLine();
                AddNewLineAroundCurrent( buff,k-1, INSERT_AFTER );
                extra++;
                StaticFree( buff );
                break;
            }

            /*
             * at the very end, undo what we did and go back
             */
            if( WorkLine->data[i] == 0 ) {
                ReplaceCurrentLine();
                UndoInsert( cl+1, cl+extra, UndoStack );
                return( extra );
            }

        }

    }

} /* SplitUpLine */
