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
* Description:  Global commands execution.
*
****************************************************************************/


#include "vi.h"
#include "walloca.h"
#include "rxsupp.h"
#include "win.h"

/*
 * Global - perform global command
 */
vi_rc Global( linenum n1, linenum n2, const char *data, int dmt )
{
    char        *sstr, *linedata;
    bool        match;
    vi_rc       rc;
    vi_rc       rc1;
    long        changecnt = 0;
    linenum     ll;
    fcb         *cfcb;
    line        *cline;
    regexp      crx;
    i_mark      pos;

    /*
     * get search string and command
     */
    rc = ModificationTest();
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    sstr = alloca( MAX_INPUT_LINE );
    if( sstr == NULL ) {
        return( ERR_NO_STACK );
    }
    data = SkipLeadingSpaces( data );
    data = GetNextWord( data, sstr, SingleSlash );
    if( *sstr == '\0' ) {
        return( ERR_INVALID_GLOBAL_CMD );
    }
    if( *data == '/' )
        ++data;
    data = SkipLeadingSpaces( data );

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
    rc = CurrentRegComp( sstr );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    SaveCurrentFilePos();
    StartUndoGroup( UndoStack );
    EditFlags.DisplayHold = true;

    /*
     * pass one - find all matches
     */
    for( pos.line = n1; pos.line <= n2; pos.line++ ) {

        /*
         * go thorugh file, marking global lines
         */
        pos.column = 0;
        rc = FindRegularExpression( NULL, &pos, &linedata, n2, 0 );
        if( rc != ERR_NO_ERR ) {
            if( rc == ERR_FIND_PAST_TERM_LINE || rc == ERR_FIND_NOT_FOUND ||
                rc == ERR_FIND_END_OF_FILE ) {
                break;
            }
            RestoreCurrentFilePos();
            EditFlags.DisplayHold = false;
            return( rc );
        }
        if( pos.line > n2 ) {
            break;
        }

        /*
         * go to appropriate spot in file
         */
        rc = GoToLineNoRelCurs( pos.line );
        if( rc != ERR_NO_ERR ) {
            RestoreCurrentFilePos();
            EditFlags.DisplayHold = false;
            return( rc );
        }

        /*
         * mark fcb and line for a match
         */
        CurrentFcb->globalmatch = true;
        CurrentLine->u.ld.globmatch = true;
        if( EditFlags.Verbose && EditFlags.EchoOn ) {
            // WPrintfLine( MessageWindow,1,"Match on line %l",clineno );
            Message1( "Match on line %l", pos.line );
        }
    }


    /*
     * negate range, if needed
     */
    if( dmt ) {
        /*
         * run through each line, flipping globmatch flag on lines
         */
        CGimmeLinePtr( n1, &CurrentFcb, &CurrentLine );
        match = false;
        for( CurrentPos.line = n1; CurrentPos.line <= n2; CurrentPos.line++ ) {
            if( CurrentLine->u.ld.globmatch ) {
                CurrentLine->u.ld.globmatch = false;
            } else {
                match = true;
                CurrentLine->u.ld.globmatch = true;
            }
            CurrentLine = CurrentLine->next;
            if( CurrentLine == NULL ) {
                CurrentFcb->globalmatch = match;
                CurrentFcb = CurrentFcb->next;
                FetchFcb( CurrentFcb );
                CurrentLine = CurrentFcb->lines.head;
                match = false;
            }
        }
    }

    /*
     * Pass 2: do all changes
     */
    rc = ERR_NO_ERR;
    EditFlags.GlobalInProgress = true;
    memcpy( &crx, CurrentRegularExpression, sizeof( crx ) );

    for( CurrentFcb = CurrentFile->fcbs.head; CurrentFcb != NULL; CurrentFcb = CurrentFcb->next ) {
        if( !CurrentFcb->globalmatch )
            continue;
        FetchFcb( CurrentFcb );
        CurrentPos.line = CurrentFcb->start_line;
        for( CurrentLine = CurrentFcb->lines.head; CurrentLine != NULL; CurrentLine = CurrentLine->next, CurrentPos.line++ ) {
            if( !CurrentLine->u.ld.globmatch )
                continue;
            CurrentLine->u.ld.globmatch = false;
            changecnt++;

            CurrentPos.column = 1;
            ProcessingMessage( CurrentPos.line );
            /*
            * build command line
            */
            rc = RunCommandLine( data );
            if( rc > ERR_NO_ERR ) {
                break;
            }
        }
        if( rc > ERR_NO_ERR ) {
            break;
        }
        CurrentFcb->globalmatch = false;
    }

    /*
     * we have an error, so fix up fcbs
     */
    if( rc > ERR_NO_ERR ) {
        for( cfcb = CurrentFile->fcbs.head; cfcb != NULL; cfcb = cfcb->next ) {
            if( cfcb->globalmatch ) {
                cfcb->globalmatch = false;
                cfcb->non_swappable = false;
                for( cline = cfcb->lines.head; cline != NULL; cline = cline->next ) {
                    cline->u.ld.globmatch = false;
                }
            }
        }
    }

    /*
     * display results
     */
    EditFlags.GlobalInProgress = false;
    EditFlags.DisplayHold = false;
    EndUndoGroup( UndoStack );
    RestoreCurrentFilePos();
    rc1 = SetCurrentLine( CurrentPos.line );
    if( rc1 != ERR_NO_ERR ) {
        if( rc1 == ERR_NO_SUCH_LINE ) {
            SetCurrentLine( 1 );
        } else {
            return( rc1 );
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
