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
#include <stdlib.h>
#include "vi.h"

/*
 * CGimmeLinePtr - give a pointer to a line in the current file
 */
int CGimmeLinePtr( linenum lineno, fcb **cfcb, line **cline )
{
    return( GimmeLinePtr( lineno, CurrentFile, cfcb, cline ) );
} /* CGimmeLinePtr */

/*
 * GimmeLinePtr - give a pointer to line data
 */
int GimmeLinePtr( linenum lineno, file *cfile, fcb **cfcb, line **cline )
{
    int         i;
    fcb         *tfcb;

    i = FindFcbWithLine( lineno, cfile, &tfcb );
    if( i ) {
        return( i );
    }
    i = GimmeLinePtrFromFcb( lineno, tfcb, cline );
    if( i ) {
        return( i );
    }
    *cfcb = tfcb;
    return( ERR_NO_ERR );

} /* GimmeLinePtr */

/*
 * CGimmeNextLinePtr - get pointer to next line in current file
 */
int CGimmeNextLinePtr( fcb **cfcb, line **cline )
{
    return( GimmeNextLinePtr( CurrentFile, cfcb, cline ) );
} /* CGimmeNextLinePtr */

/*
 * CAdvanceToLine - advance to given line in file
 */
int CAdvanceToLine( linenum l )
{
    fcb         *cfcb;
    line        *cline;
    int         rc;

    rc = CGimmeLinePtr( l, &cfcb, &cline );
    return( rc );

} /* CAdvanceToLine */

/*
 * GimmeNextLinePtr - get pointer to next line
 */
int GimmeNextLinePtr( file *cfile, fcb **cfcb, line **cline )
{
    int i;
    fcb *ofcb;

    /*
     * get next line pointer; if not null, go back
     */
    *cline = (*cline)->next;
    if( *cline != NULL ) {
        return( ERR_NO_ERR );
    }

    /*
     * get next fcb pointer; if not null, get first line and go back
     */
    *cfcb = (*cfcb)->next;
    if( *cfcb != NULL ) {
        FetchFcb( *cfcb );
        *cline = (*cfcb)->line_head;
        return( ERR_NO_ERR );
    }

    /*
     * get next fcb if can; then get first line and go back
     */
    if( cfile->bytes_pending ) {
        ofcb = cfile->fcb_tail;
        i = ReadFcbData( cfile );
        if( i > 0 ) {
            return( i );
        }
        *cfcb = cfile->fcb_tail;
        if( *cfcb != ofcb ) {
            while( (*cfcb)->prev != ofcb ) {
                *cfcb = (*cfcb)->prev;
            }
        }
        FetchFcb( *cfcb );
        *cline = (*cfcb)->line_head;
        return( ERR_NO_ERR );
    }

    /*
     * no such line
     */
    *cfcb = NULL;
    *cline = NULL;
    return( ERR_NO_MORE_LINES );

} /* GimmeNextLinePtr */

/*
 * GimmePrevLinePtr - get pointer to previous line
 */
int GimmePrevLinePtr( fcb **cfcb, line **cline )
{

    /*
     * get next line pointer; if not null, go back
     */
    *cline = (*cline)->prev;
    if( *cline != NULL ) {
        return( ERR_NO_ERR );
    }

    /*
     * get next fcb pointer; if not null, get first line and go back
     */
    *cfcb = (*cfcb)->prev;
    if( *cfcb != NULL ) {
        FetchFcb( *cfcb );
        *cline = (*cfcb)->line_tail;
        return( ERR_NO_ERR );
    }

    /*
     * no such line
     */
    *cfcb = NULL;
    *cline = NULL;
    return( ERR_NO_MORE_LINES );

} /* GimmePrevLinePtr */

/*
 * GimmeLinePtrFromFcb - get a line pointer from a specified fcb
 */
int GimmeLinePtrFromFcb( linenum lineno, fcb *cfcb , line **res )
{
    linenum     linecnt;
    line        *tmp;

    if( lineno < 1 ) {
        return( ERR_NO_SUCH_LINE );
    }
    FetchFcb( cfcb );
    linecnt = cfcb->start_line;
    tmp = cfcb->line_head;

    while( linecnt != lineno ) {
        linecnt++;
        tmp = tmp->next;
    }

    *res = tmp;

    return( ERR_NO_ERR );

} /* GimmeLinePtrFromFcb */

/*
 * CFindLastLine - find last line in file
 */
int CFindLastLine( linenum *ll )
{
    int i;
    fcb *cfcb;

    if( CurrentFile == NULL ) {
        return( ERR_NO_FILE );
    }

    if( !CurrentFile->bytes_pending ) {
        *ll = CurrentFile->fcb_tail->end_line;
        return( ERR_NO_ERR );
    }

    i = FindFcbWithLine( -1, CurrentFile, &cfcb );
    if( i ) {
        return( i );
    }
    *ll = cfcb->end_line;
    return( ERR_NO_ERR );

} /* CFindLastLine */

/*
 * IsPastLastLine - test if something is past the last line
 */
int IsPastLastLine( linenum l )
{
    linenum     ll;

    if( CurrentFile == NULL ) {
        return( TRUE );
    }

    ll = CurrentFile->fcb_tail->end_line;
    if( l <= ll ) {
        return( FALSE );
    }
    CFindLastLine( &ll );
    if( l > ll ) {
        return( TRUE );
    }
    return( FALSE );

} /* IsPastLastLine */

/*
 * ValidateCurrentLine - make sure current line is valid
 */
int ValidateCurrentLine( void )
{
    int i;

    if( CurrentLineNumber < 1 ) {
        return( SetCurrentLine( 1 ) );
    }
    if( CurrentLineNumber > CurrentFile->fcb_tail->end_line ) {
        i = CFindLastLine( &CurrentLineNumber );
        if( i ) {
            return(i);
        }
    }
    return( SetCurrentLine( CurrentLineNumber ) );

} /* ValidateCurrentLine */
