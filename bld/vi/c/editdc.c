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

/*
 * DeleteRangeOnCurrentLine - perform the deletion
 */
vi_rc DeleteRangeOnCurrentLine( int scol, int ecol, int savebuf_flag )
{
    int     i;
    vi_rc   rc;

    /*
     * verify range
     */
    if( scol > ecol ) {
        i = scol;
        scol = ecol;
        ecol = i;
    }

    /*
     * go delete block and set up undo
     */
    CurrentLineReplaceUndoStart();
    rc = DeleteBlockFromCurrentLine( scol, ecol, savebuf_flag );
    if( rc != ERR_NO_ERR ) {
        CurrentLineReplaceUndoCancel();
        return( rc );
    }
    DisplayWorkLine( TRUE );
    ReplaceCurrentLine();
    CurrentLineReplaceUndoEnd( TRUE );
    EditFlags.Dotable = TRUE;
    if( savebuf_flag ) {
#ifdef __WIN__
        if( LastSavebuf == 0 ) {
            Message1( "%d characters deleted into the clipboard", ecol - scol + 1 );
        } else {
#endif
            Message1( "%d %s%s%c", ecol - scol + 1, MSG_CHARACTERS,
                      MSG_DELETEDINTOBUFFER, LastSavebuf );
#ifdef __WIN__
        }
#endif
    }
    return( ERR_NO_ERR );

} /* DeleteRangeOnCurrentLine */

/*
 * DeleteBlockFromCurrentLine - remove chars from line, leave result in work line
 */
vi_rc DeleteBlockFromCurrentLine( int scol, int ecol, int saveb_flag )
{
    int i;

    /*
     * check if we can do this on the current line
     */
    if( scol > ecol ) {
        i = scol;
        scol = ecol;
        ecol = i;
    }
    if( scol < 0 || ecol >= CurrentLine->len ) {
        return( ERR_CANNOT_DELETE_CHAR );
    }

    if( saveb_flag ) {
        AddLineToSavebuf( CurrentLine->data, scol, ecol );
    }

    /*
     * remove chars
     */
    GetCurrentLine();
    for( i = ecol + 1; i <= CurrentLine->len; i++ ) {
        WorkLine->data[scol + (i - (ecol + 1))] = WorkLine->data[i];
    }
    WorkLine->len -= ecol - scol + 1;
    return( ERR_NO_ERR );

} /* DeleteBlockFromCurrentLine */
