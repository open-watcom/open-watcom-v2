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
 * CGimmeLinePtr - give a pointer to a line in the current file
 */
vi_rc CGimmeLinePtr( linenum lineno, fcb **cfcb, line **cline )
{
    return( GimmeLinePtr( lineno, CurrentFile, cfcb, cline ) );

} /* CGimmeLinePtr */

/*
 * GimmeLinePtr - give a pointer to line data
 */
vi_rc GimmeLinePtr( linenum lineno, file *cfile, fcb **cfcb, line **cline )
{
    vi_rc       rc;
    fcb         *tfcb;

    rc = FindFcbWithLine( lineno, cfile, &tfcb );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    rc = GimmeLinePtrFromFcb( lineno, tfcb, cline );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    *cfcb = tfcb;
    return( ERR_NO_ERR );

} /* GimmeLinePtr */

/*
 * CGimmeNextLinePtr - get pointer to next line in current file
 */
vi_rc CGimmeNextLinePtr( fcb **cfcb, line **cline )
{
    return( GimmeNextLinePtr( CurrentFile, cfcb, cline ) );

} /* CGimmeNextLinePtr */

/*
 * CAdvanceToLine - advance to given line in file
 */
vi_rc CAdvanceToLine( linenum l )
{
    fcb         *cfcb;
    line        *cline;
    vi_rc       rc;

    rc = CGimmeLinePtr( l, &cfcb, &cline );
    return( rc );

} /* CAdvanceToLine */

/*
 * GimmeNextLinePtr - get pointer to next line
 */
vi_rc GimmeNextLinePtr( file *cfile, fcb **cfcb, line **cline )
{
    vi_rc   rc;
    fcb     *ofcb;

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
        *cline = (*cfcb)->lines.head;
        return( ERR_NO_ERR );
    }

    /*
     * get next fcb if can; then get first line and go back
     */
    if( cfile->bytes_pending ) {
        ofcb = cfile->fcbs.tail;
        rc = ReadFcbData( cfile, NULL );
        if( rc != ERR_NO_ERR && rc != END_OF_FILE ) {
            return( rc );
        }
        *cfcb = cfile->fcbs.tail;
        if( *cfcb != ofcb ) {
            while( (*cfcb)->prev != ofcb ) {
                *cfcb = (*cfcb)->prev;
            }
        }
        FetchFcb( *cfcb );
        *cline = (*cfcb)->lines.head;
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
vi_rc GimmePrevLinePtr( fcb **cfcb, line **cline )
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
        *cline = (*cfcb)->lines.tail;
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
vi_rc GimmeLinePtrFromFcb( linenum lineno, fcb *cfcb , line **res )
{
    linenum     linecnt;
    line        *tmp;

    if( lineno < 1 ) {
        return( ERR_NO_SUCH_LINE );
    }
    FetchFcb( cfcb );
    linecnt = cfcb->start_line;
    tmp = cfcb->lines.head;

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
vi_rc CFindLastLine( linenum *ll )
{
    vi_rc   rc;
    fcb     *cfcb;

    if( CurrentFile == NULL ) {
        return( ERR_NO_FILE );
    }

    if( !CurrentFile->bytes_pending ) {
        *ll = CurrentFile->fcbs.tail->end_line;
        return( ERR_NO_ERR );
    }

    rc = FindFcbWithLine( -1, CurrentFile, &cfcb );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    *ll = cfcb->end_line;
    return( ERR_NO_ERR );

} /* CFindLastLine */

/*
 * IsPastLastLine - test if something is past the last line
 */
bool IsPastLastLine( linenum l )
{
    linenum     ll;

    if( CurrentFile == NULL ) {
        return( true );
    }

    ll = CurrentFile->fcbs.tail->end_line;
    if( l <= ll ) {
        return( false );
    }
    CFindLastLine( &ll );
    if( l > ll ) {
        return( true );
    }
    return( false );

} /* IsPastLastLine */

/*
 * ValidateCurrentLine - make sure current line is valid
 */
vi_rc ValidateCurrentLine( void )
{
    vi_rc   rc;

    if( CurrentPos.line < 1 ) {
        return( SetCurrentLine( 1 ) );
    }
    if( CurrentPos.line > CurrentFile->fcbs.tail->end_line ) {
        rc = CFindLastLine( &CurrentPos.line );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
    }
    return( SetCurrentLine( CurrentPos.line ) );

} /* ValidateCurrentLine */
