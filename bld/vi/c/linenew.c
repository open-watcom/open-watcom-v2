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
#include <stddef.h>
#include "win.h"

/*
 * AddNewLineAroundCurrent - put a new line on either side of the current line
 */
void AddNewLineAroundCurrent( char *data, int copylen, insert_dir dir )
{
    bool        wasnull;
    /*
     * if inserting into a null fcb, clean it up
     */
    FetchFcb( CurrentFcb );
    wasnull = CurrentFcb->nullfcb;
    if( wasnull ) {
        MemFree( CurrentFcb->lines.head );
        CurrentFcb->lines.head = CurrentFcb->lines.tail = NULL;
        CurrentFcb->nullfcb = FALSE;
        CurrentFcb->byte_cnt = 0;
        CurrentFcb->end_line = 0;
    }

    /*
     * add the line
     */
    InsertNewLine( CurrentLine, &CurrentFcb->lines, data, copylen,dir );
    CurrentFcb->byte_cnt += copylen + 1;
    CurrentFcb->end_line += 1;

    /*
     * update line info
     */
    if( wasnull ) {
        CurrentLine = CurrentFcb->lines.head;
        SetCurrentLineNumber( 1 );
    } else {
        if( dir == INSERT_BEFORE ) {
            SetCurrentLineNumber( CurrentPos.line + 1 );
        }
        UpdateLineNumbers( 1L, CurrentFcb->next );
    }
    CheckCurrentFcbCapacity();

} /* AddNewLineAroundCurrent */

/*
 * InsertNewLine - do just that
 */
void InsertNewLine( line *who, line_list *linelist, char *data, int copylen,
                    insert_dir dir )
{
    line        *cl;

    if( copylen <= 0 ) {
        data = NULL;
        copylen = 0;
    }
    cl = LineAlloc( data, copylen );
    if( linelist->head == NULL ) {
        AddLLItemAtEnd( (ss **)&linelist->head, (ss **)&linelist->tail, (ss *)cl );
    } else {
        if( dir == INSERT_AFTER ) {
            InsertLLItemAfter( (ss **)&linelist->tail, (ss *)who, (ss *)cl );
        } else {
            InsertLLItemBefore( (ss **)&linelist->head, (ss *)who, (ss *)cl );
        }
    }

} /* InsertNewLine */

/*
 * LineAlloc - allocate a line
 */
line *LineAlloc( char *data, int len )
{
    line        *tmp;

    tmp = MemAlloc( offsetof( line, data ) + len + 1 );
    if( data != NULL ) {
        memcpy( tmp->data, data, len );
    }
    tmp->data[len] = 0;
    tmp->len = len;

    return( tmp );

} /* LineAlloc */

/*
 * CreateNullLine - put a single null line in an fcb
 */
void CreateNullLine( fcb *cfcb )
{
    line        *cline;

    cline = LineAlloc( NULL, 0 );
    FetchFcb( cfcb );
    AddLLItemAtEnd( (ss **)&(cfcb->lines.head), (ss **)&(cfcb->lines.tail), (ss *)cline );
    cfcb->byte_cnt = 1;
    cfcb->start_line = cfcb->end_line = 1;
    cfcb->nullfcb = TRUE;

} /* CreateNullLine */
