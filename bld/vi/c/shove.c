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
#include <ctype.h>
#include <string.h>
#include "vi.h"
#include "keys.h"
#include "win.h"

/*
 * Shift - shove a tab in/out over a line range
 */
int Shift( linenum s, linenum e, char dir, bool msgflag )
{
    int         i,shv;
    linenum     fullcnt=0;

    /*
     * set up undo
     */
    if( i = ModificationTest() ) {
        return( i );
    }
    i = UndoReplaceLines( s,e );
    if( i ) {
        return( i );
    }

    /*
     * now, point to start line
     */
    i = SaveAndResetFilePos( s );
    if( i ) {
        return( i );
    }

    /*
     * process all lines
     */
    for( CurrentLineNumber=s;CurrentLineNumber<=e;CurrentLineNumber++ ) {

        /*
         * Add/Subtract leading tab space
         */
        GetCurrentLine();
        shv = ShiftWidth;
        if( dir != '>' ) {
            shv *= -1;
        }
        fullcnt += AddLeadingTabSpace( &WorkLine->len, WorkLine->data, shv );
        ReplaceCurrentLine();

        if( CurrentLineNumber != e ) {
            i = CGimmeNextLinePtr( &CurrentFcb, &CurrentLine );
            if( i ) {
                RestoreCurrentFilePos();
                return( i );
            }
        }

    }

    /*
     * done, say so and go back
     */
    RestoreCurrentFilePos();
    if( msgflag ) {
        Message1( "%l lines %c'ed",e-s+1, dir );
        if( fullcnt > 0 ) {
            Message2( "%l full lines not processed", fullcnt );
        }
    }
    if( CurrentLineNumber >= s && CurrentLineNumber <= e ) {
        CheckCurrentColumn();
    }
    DCDisplayAllLines();
    SetWindowCursor();
    Modified( TRUE );
    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* Shift */
