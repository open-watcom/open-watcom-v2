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

/*
 * Global - perform global command
 */
int Global( linenum n1, linenum n2, char *data, int dmt )
{
    char        *sstr,*cmd,*linedata;
    int         i,todo;
    int         ccol,rc;
    long        changecnt=0;
    linenum     clineno,llineno,ll;
    fcb         *cfcb;
    line        *cline;
    regexp      crx;

    /*
     * get search string and command
     */
    if( rc = ModificationTest() ) {
        return( rc );
    }
    sstr = alloca( MAX_INPUT_LINE );
    cmd = alloca( MAX_INPUT_LINE );
    if( cmd == NULL || sstr == NULL ) {
        return( ERR_NO_STACK );
    }
    RemoveLeadingSpaces( data );
    if( NextWordSlash( data, sstr ) < 0 ) {
        return( ERR_INVALID_GLOBAL_CMD );
    }
    RemoveLeadingSpaces( data );
    EliminateFirstN( data,1 );

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
    i = CurrentRegComp( sstr );
    if( i ) {
        return( i );
    }
    SaveCurrentFilePos();
    llineno = n1-1;
    clineno = n1;
    ccol = 0;
    StartUndoGroup( UndoStack );
    EditFlags.DisplayHold = TRUE;
    strcpy( sstr, data );

    /*
     * pass one - find all matches
     */
    while( TRUE ) {

        /*
         * go thorugh file, marking global lines
         */
        i = FindRegularExpression( NULL, &clineno, ccol, &linedata, n2, FALSE );
        if( !i ) {
            ccol = GetCurrRegExpColumn( linedata );
        } else {
            if( i == ERR_FIND_PAST_TERM_LINE || i == ERR_FIND_NOT_FOUND || i == ERR_FIND_END_OF_FILE )  {
                break;
            }
            RestoreCurrentFilePos();
            EditFlags.DisplayHold = FALSE;
            return( i );
        }
        if( clineno > n2 ) {
            break;
        }

        /*
         * go to appropriate spot in file
         */
        i = GoToLineNoRelCurs( clineno );
        if( i ) {
            RestoreCurrentFilePos();
            EditFlags.DisplayHold = FALSE;
            return( i );
        }

        /*
         * mark fcb and line for a match
         */
        CurrentFcb->globalmatch = TRUE;
        CurrentLine->inf.ld.globmatch = TRUE;
        if( EditFlags.Verbose && EditFlags.EchoOn ) {
            // WPrintfLine( MessageWindow,1,"Match on line %l",clineno );
            Message1( "Match on line %l", clineno );
        }

        clineno++;
        if( clineno > n2 ) {
            break;
        }
        ccol = 0;

    }


    /*
     * negate range, if needed
     */
    if( dmt ) {
        /*
         * run through each line, flipping globmatch flag on lines
         */
        CGimmeLinePtr( n1, &CurrentFcb, &CurrentLine );
        CurrentLineNumber = n1;
        while( TRUE ) {
            i = FALSE;
            while( CurrentLine != NULL && CurrentLineNumber <= n2 ) {
                if( CurrentLine->inf.ld.globmatch ) {
                    CurrentLine->inf.ld.globmatch = FALSE;
                } else {
                    i = TRUE;
                    CurrentLine->inf.ld.globmatch = TRUE;
                }
                CurrentLine = CurrentLine->next;
                CurrentLineNumber++;
            }
            CurrentFcb->globalmatch = i;
            if( CurrentLineNumber > n2 ) {
                break;
            }
            CurrentFcb = CurrentFcb->next;
            FetchFcb( CurrentFcb );
            CurrentLine = CurrentFcb->line_head;
        }

    }

    /*
     * Pass 2: do all changes
     */
    EditFlags.GlobalInProgress = TRUE;
    memcpy( &crx, CurrentRegularExpression, sizeof( crx ) );
    while( TRUE ) {

        /*
         * get fcb with a change to do
         */
        todo = FALSE;
        CurrentFcb = CurrentFile->fcb_head;
        while( CurrentFcb != NULL ) {

            if( CurrentFcb->globalmatch ) {

                /*
                 * find a line
                 */
                FetchFcb( CurrentFcb );
                CurrentLineNumber = CurrentFcb->start_line;
                CurrentLine = CurrentFcb->line_head;
                while( CurrentLine != NULL ) {
                    if( CurrentLine->inf.ld.globmatch ) {
                        todo = TRUE;
                        break;
                    }
                    CurrentLineNumber++;
                    CurrentLine = CurrentLine->next;
                }

                if( !todo ) {
                    CurrentFcb->globalmatch = FALSE;
                }
                else break;

            }
            CurrentFcb = CurrentFcb->next;

        }

        if( !todo ) {
            break;
        }

        /*
         * reset info
         */
        CurrentLine->inf.ld.globmatch = FALSE;
        cfcb = CurrentFcb;
        CurrentColumn = 1;

        /*
         * build command line
         */
        changecnt++;
        strcpy( cmd, sstr );
        ProcessingMessage( CurrentLineNumber );

        rc = RunCommandLine( cmd );
        if( rc > 0 ) {
            break;
        }

    }

    /*
     * we have an error, so fix up fcbs
     */
    if( rc > 0 ) {

        cfcb = CurrentFile->fcb_head;
        while( cfcb != NULL ) {
            if( cfcb->globalmatch ) {
                cfcb->globalmatch = FALSE;
                cfcb->non_swappable = FALSE;
                cline = cfcb->line_head;
                while( cline != NULL ) {
                    cline->inf.ld.globmatch = FALSE;
                    cline = cline->next;
                }
            }
            cfcb = cfcb->next;
        }

    }

    /*
     * display results
     */
    EditFlags.GlobalInProgress = FALSE;
    EditFlags.DisplayHold = FALSE;
    EndUndoGroup( UndoStack );
    RestoreCurrentFilePos();
    i = SetCurrentLine( CurrentLineNumber );
    if( i ) {
        if( i == ERR_NO_SUCH_LINE ) {
            SetCurrentLine( 1 );
        } else {
            return( i );
        }
    }
    Message1( "%l matches found",changecnt );
    DCDisplayAllLines();
    return( rc );

} /* Global */

/*
 * ProcessingMessage - display what line is being processed
 */
void ProcessingMessage( linenum cln )
{
    if( EditFlags.Verbose && EditFlags.EchoOn ) {
        // WPrintfLine( MessageWindow,1,"Processing line %l",cln );
        Message1( "Processing line %l", cln );
    }
} /* ProcessingMessage */
