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
#include <stdlib.h>
#include <string.h>
#include "posix.h"
#include <fcntl.h>
#include <errno.h>
#include "vi.h"

static int      extraDataSize;
static void     *extraData;

/*
 * ReadFcbData - read fcb data
 */
int ReadFcbData( file *f )
{
    int         handle;
    int         cnt,used,linecnt,i;
    bool        eofflag=FALSE;
    fcb         *cfcb;

    /*
     * get new fcb
     */
    f->bytes_pending = FALSE;
    cfcb = FcbAlloc( f );
    AddLLItemAtEnd( &(f->fcb_head), &(f->fcb_tail), cfcb );

    /*
     * open file handle if we need to
     */
    if( f->handle < 0 ) {
        ConditionalChangeDirectory( f->home );
        if( f->is_stdio ) {
            handle = fileno( stdin );
        } else {
            i = FileOpen( f->name, FALSE, O_BINARY | O_RDONLY, 0, &handle );
            if( i ) {
                return( ERR_FILE_OPEN );
            }
        }
        f->handle = handle;
    } else {
        handle = f->handle;
    }
    if( f->handle == -1 ) {
        CreateNullLine(cfcb);
        return( ERR_FILE_NOT_FOUND );
    }
    if( f->size == 0 && !f->is_stdio ) {
        CreateNullLine( cfcb );
        close( handle );
        f->handle = -1;
        return( END_OF_FILE );
    }

    /*
     * go to appropriate location in file
     */
    if( !f->is_stdio ) {
        i = FileSeek( handle, f->curr_pos );
        if( i ) {
            return( i );
        }
    } else {
        if( extraData != NULL ) {
            memcpy( ReadBuffer, extraData, extraDataSize );
            MemFree2( &extraData );
        }
    }

    /*
     * read file data
     */
    if( f->is_stdio ) {
        cnt = fread( ReadBuffer+extraDataSize, 1, MAX_IO_BUFFER-extraDataSize,
                        stdin );
        cnt += extraDataSize;
        extraDataSize = 0;
        if( ferror( stdin ) ) {
            return( ERR_READ );
        }
    } else {
        cnt = read( handle, ReadBuffer, MAX_IO_BUFFER );
        if( cnt == -1 ) {
            return( ERR_READ );
        }
    }

    /*
     * create lines from buffer info
     */
    eofflag = CreateLinesFromBuffer( cnt, &(cfcb->line_head),
                &(cfcb->line_tail), &used, &linecnt,
                &(cfcb->byte_cnt) );

    if( used == 0 ) {
        CreateNullLine( cfcb );
        close( handle );
        f->handle = -1;
        return( END_OF_FILE );
    }

    /*
     * update position and line numbers
     */
    f->curr_pos += used;
    if( !f->is_stdio ) {
        if( f->curr_pos >= f->size ) {
            eofflag = TRUE;
        }
    } else {
        if( feof( stdin ) && used == cnt ) {
            eofflag = TRUE;
        } else {
            extraDataSize = cnt - used;
            extraData = MemAlloc( extraDataSize );
            memcpy( extraData, ReadBuffer+used, extraDataSize );
        }
    }
    if( f->fcb_tail->prev == NULL ) {
        cfcb->start_line = 1;
    } else {
        cfcb->start_line = f->fcb_tail->prev->end_line + 1;
    }
    cfcb->end_line = cfcb->start_line + linecnt-1;
    cfcb->non_swappable = FALSE;

    if( eofflag) {
        if( !f->is_stdio ) {
            close( handle );
        }
        f->handle = -1;
        return( END_OF_FILE );
    }
    f->bytes_pending = TRUE;
    return( ERR_NO_ERR );

} /* ReadFcbData */

/*
 * FindFcbWithLine - find the fcb with the specified line
 */
int FindFcbWithLine( linenum lineno, file *cfile, fcb **fb )
{
    int lastflag = FALSE,i;
    fcb *tfcb,*ofcb;

    /*
     * are we looking for the last line?
     */
    if( lineno < 0 ) {
        lastflag = TRUE;
        lineno = MAX_LONG;
    }
    if( lineno < 1 ) {
        return( ERR_NO_SUCH_LINE );
    }
    if( cfile == NULL ) {
        return( ERR_NO_FILE );
    }

    /*
     * run through all possible fcb's
     */
    tfcb = cfile->fcb_head;
    if( tfcb == NULL ) {
        return( ERR_NO_SUCH_LINE );
    }
    while( TRUE ) {

        if( tfcb->end_line >= lineno ) {
            *fb = tfcb;
            FetchFcb( tfcb );
            return( ERR_NO_ERR );
        }
        ofcb = tfcb;
        tfcb = ofcb->next;
        if( tfcb == NULL ) {
            if( !cfile->bytes_pending ) {
                if( lastflag ) {
                    *fb = ofcb;
                    FetchFcb( ofcb );
                    return( ERR_NO_ERR );
                }
                return( ERR_NO_SUCH_LINE );
            }
            if( EditFlags.Verbose ) {
                Message1( "At line %l", ofcb->end_line );
            }
            if( (i=ReadFcbData( cfile )) > 0 ) {
                return( i );
            }
            tfcb = cfile->fcb_tail;
        }

    }

} /* FindFcbWithLine */

/*
 * CreateFcbData - create fcb with data from specified buffer
 */
void CreateFcbData( file *f, int cnt )
{
    int used,linecnt;
    fcb *cfcb;

    /*
     * get new fcb
     */
    cfcb = FcbAlloc( f );
    AddLLItemAtEnd( &(f->fcb_head), &(f->fcb_tail), cfcb );

    /*
     * create lines from buffer info
     */
    CreateLinesFromBuffer( cnt, &(cfcb->line_head), &(cfcb->line_tail),
                               &used, &linecnt,&(cfcb->byte_cnt) );

    /*
     * update position and line numbers
     */
    if( f->fcb_tail->prev == NULL ) {
        cfcb->start_line = 1;
    } else {
        cfcb->start_line = f->fcb_tail->prev->end_line + 1;
    }
    cfcb->end_line = cfcb->start_line + linecnt-1;
    cfcb->non_swappable = FALSE;

} /* CreateFcbData */

/*
 * FcbSize - get the size (in bytes) of an fcb
 */
int FcbSize( fcb *cfcb )
{
    int i;

    /*
     * multiply number of lines by 3 to get extra bytes.  Why?
     * byte count includes trailing zeros, but when we swap, we need to
     * put out a c/r and a l/f, so we must add one extra for
     * each line.  As well, each line has 2 bytes of information,
     * which are also swapped.
     */
    i = cfcb->byte_cnt + 3*((int)(cfcb->end_line-cfcb->start_line + 1));
    return( i );

} /* FcbSize */
