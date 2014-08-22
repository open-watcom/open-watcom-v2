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
#include "win.h"

/*
 * Shift - shove a tab in/out over a line range
 */
vi_rc Shift( linenum s, linenum e, char dir, bool msgflag )
{
    int         shv;
    linenum     fullcnt = 0;
    vi_rc       rc;

    /*
     * set up undo
     */
    rc = ModificationTest();
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    rc = UndoReplaceLines( s, e );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }

    /*
     * now, point to start line
     */
    rc = SaveAndResetFilePos( s );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }

    /*
     * process all lines
     */
    for( CurrentPos.line = s; CurrentPos.line <= e; CurrentPos.line++ ) {

        /*
         * Add/Subtract leading tab space
         */
        GetCurrentLine();
        shv = EditVars.ShiftWidth;
        if( dir != '>' ) {
            shv *= -1;
        }
        if( AddLeadingTabSpace( &WorkLine->len, WorkLine->data, shv ) ) {
            ++fullcnt;
        }
        ReplaceCurrentLine();

        if( CurrentPos.line != e ) {
            rc = CGimmeNextLinePtr( &CurrentFcb, &CurrentLine );
            if( rc != ERR_NO_ERR ) {
                RestoreCurrentFilePos();
                return( rc );
            }
        }

    }

    /*
     * done, say so and go back
     */
    RestoreCurrentFilePos();
    if( msgflag ) {
        Message1( "%l lines %c'ed", e - s + 1, dir );
        if( fullcnt > 0 ) {
            Message2( "%l full lines not processed", fullcnt );
        }
    }
    if( CurrentPos.line >= s && CurrentPos.line <= e ) {
        CheckCurrentColumn();
    }
    DCDisplayAllLines();
    SetWindowCursor();
    Modified( true );
    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* Shift */
