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
#include "ex.h"

static bool beforeFlag;
/*
 * Append - start appending
 */
vi_rc Append( linenum n1, bool startundo )
{
    vi_rc   rc;

    /*
     * initialize
     */
    rc = ModificationTest();
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    if( n1 == 0 || CurrentFcb->nullfcb ) {
        beforeFlag = true;
        n1 = 1;
    } else {
        beforeFlag = false;
    }
    rc = SetCurrentLine( n1 );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    Modified( true );
    if( startundo ) {
        StartUndoGroup( UndoStack );
    }
    EditFlags.Appending = true;
    return( ERR_NO_ERR );

} /* Append */

/*
 * AppendAnother
 */
vi_rc AppendAnother( char *data )
{
    bool        dontmove = false;
    int         i;
    vi_rc       rc;
    linenum     cln;

    i = strlen( data );
    if( i == 1 && data[0] == '.' )  {
        EndUndoGroup( UndoStack );
        EditFlags.Appending = false;
        return( ERR_NO_ERR );
    }

    if( CurrentFcb->nullfcb ) {
        dontmove = true;
    }

    cln = CurrentPos.line;
    if( !beforeFlag ) {
        cln++;
    }

    UndoInsert( cln, cln, UndoStack );

    if( !beforeFlag ) {
        AddNewLineAroundCurrent( data, i, INSERT_AFTER );
    } else {
        beforeFlag = false;
        AddNewLineAroundCurrent( data, i, INSERT_BEFORE );
    }


    if( !dontmove ) {
        rc = SetCurrentLine( cln );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
    }
    return( ERR_NO_ERR );

} /* AppendAnother */
