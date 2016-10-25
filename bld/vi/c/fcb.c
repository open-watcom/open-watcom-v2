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
* Description:  Editor interface to FCBs.
*
****************************************************************************/


#include "vi.h"
#include "posix.h"
#include <fcntl.h>
#include <errno.h>

#include "clibext.h"


static int      extraDataSize;
static void     *extraData = NULL;

/*
 * ReadFcbData - read fcb data
 */
vi_rc ReadFcbData( file *f, bool *crlf_reached )
{
    int         cnt, used, linecnt;
    bool        eofflag;
    fcb         *cfcb;
    vi_rc       rc;

    f->bytes_pending = false;
    /*
     * go to appropriate location in file
     */
    if( f->is_stdio ) {
        if( extraData != NULL ) {
            memcpy( ReadBuffer, extraData, extraDataSize );
            MemFreePtr( &extraData );
        }
    } else {
        rc = FileSeek( f->handle, f->curr_pos );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
    }

    /*
     * read file data
     */
    if( f->is_stdio ) {
        cnt = fread( ReadBuffer + extraDataSize, 1, MAX_IO_BUFFER - extraDataSize, stdin );
        cnt += extraDataSize;
        extraDataSize = 0;
        if( ferror( stdin ) ) {
            return( ERR_READ );
        }
    } else {
        cnt = read( f->handle, ReadBuffer, MAX_IO_BUFFER );
        if( cnt == -1 ) {
            return( ERR_READ );
        }
    }

    /*
     * get new fcb
     */
    cfcb = FcbAlloc( f );
    AddLLItemAtEnd( (ss **)&(f->fcbs.head), (ss **)&(f->fcbs.tail), (ss *)cfcb );

    /*
     * create lines from buffer info
     */
    eofflag = CreateLinesFromFileBuffer( cnt, &cfcb->lines, &used, &linecnt, &(cfcb->byte_cnt), crlf_reached );

    if( used == 0 ) {
        CreateNullLine( cfcb );
        eofflag = true;
    } else {

        /*
         * update position
         */
        f->curr_pos += used;
        if( f->fcbs.tail->prev == NULL ) {
            cfcb->start_line = 1;
        } else {
            cfcb->start_line = f->fcbs.tail->prev->end_line + 1;
        }
        if( f->is_stdio && feof( stdin ) && used >= cnt || !f->is_stdio && f->curr_pos >= f->size ) {
            eofflag = true;
            if( !EditFlags.LastEOL && ReadBuffer[used - 1] == LF ) {
                ++linecnt;
                AddLLItemAtEnd( (ss **)&(cfcb->lines.head), (ss **)&(cfcb->lines.tail), (ss *)LineAlloc( NULL, 0 ) );
            }
        }
        /*
         * update line numbers
         */
        cfcb->end_line = cfcb->start_line + linecnt - 1;
        cfcb->non_swappable = false;
    }

    if( eofflag ) {
        if( !f->is_stdio ) {
            close( f->handle );
            f->handle = -1;
        }
        return( END_OF_FILE );
    }

    f->bytes_pending = true;
    if( f->is_stdio ) {
        extraDataSize = cnt - used;
        extraData = MemAlloc( extraDataSize );
        memcpy( extraData, ReadBuffer + used, extraDataSize );
    }
    return( ERR_NO_ERR );

} /* ReadFcbData */

/*
 * FindFcbWithLine - find the fcb with the specified line
 */
vi_rc FindFcbWithLine( linenum lineno, file *cfile, fcb **fb )
{
    bool    lastflag = false;
    fcb     *tfcb, *ofcb;
    vi_rc   rc;

    /*
     * are we looking for the last line?
     */
    if( lineno < 0 ) {
        lastflag = true;
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
    tfcb = cfile->fcbs.head;
    if( tfcb == NULL ) {
        return( ERR_NO_SUCH_LINE );
    }
    for( ;; ) {

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
            rc = ReadFcbData( cfile, NULL );
            if( rc != ERR_NO_ERR && rc != END_OF_FILE ) {
                return( rc );
            }
            tfcb = cfile->fcbs.tail;
        }
    }

} /* FindFcbWithLine */

/*
 * CreateFcbData - create fcb with data from specified buffer
 */
void CreateFcbData( file *f, int cnt )
{
    int used, linecnt;
    fcb *cfcb;

    /*
     * get new fcb
     */
    cfcb = FcbAlloc( f );
    AddLLItemAtEnd( (ss **)&(f->fcbs.head), (ss **)&(f->fcbs.tail), (ss *)cfcb );

    /*
     * create lines from buffer info
     */
    CreateLinesFromBuffer( cnt, &cfcb->lines, &used, &linecnt, &(cfcb->byte_cnt) );

    /*
     * update position and line numbers
     */
    if( f->fcbs.tail->prev == NULL ) {
        cfcb->start_line = 1;
    } else {
        cfcb->start_line = f->fcbs.tail->prev->end_line + 1;
    }
    cfcb->end_line = cfcb->start_line + linecnt - 1;
    cfcb->non_swappable = false;

} /* CreateFcbData */

vi_rc OpenFcbData( file *f )
{
    int         handle;
    vi_rc       rc;
    fcb         *cfcb;

    /*
     * open file handle if we need to
     */
    rc = ERR_NO_ERR;
    if( !f->is_stdio ) {
        handle = -1;
        ConditionalChangeDirectory( f->home );
        rc = FileOpen( f->name, false, O_BINARY | O_RDONLY, 0, &handle );
        if( rc != ERR_NO_ERR ) {
            return( ERR_FILE_OPEN );
        }
        if( handle == -1 ) {
            rc = ERR_FILE_NOT_FOUND;
        } else if( f->size == 0 ) {
            close( handle );
            rc = END_OF_FILE;
        } else {
            f->handle = handle;
        }
        if( rc != ERR_NO_ERR ) {
            cfcb = FcbAlloc( f );
            AddLLItemAtEnd( (ss **)&(f->fcbs.head), (ss **)&(f->fcbs.tail), (ss *)cfcb );
            CreateNullLine( cfcb );
        }
    }
    return( rc );
}
 
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
    i = cfcb->byte_cnt + 3 * ((int)(cfcb->end_line-cfcb->start_line + 1));
    return( i );

} /* FcbSize */
