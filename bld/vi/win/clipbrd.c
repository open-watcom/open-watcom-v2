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
* Description:  Handles the interaction to the Windows clipboard.
*
****************************************************************************/

/*
  Modified:     By:             Reason:
  ---------     ---             -------
  July 28 02    ff              changed AddFcbsToClipboard():
                                  the last \n\r is not written to the clipboard
                                changed GetClipboardSavebuf():
                                  an extra \n\r is added to the buffer read from the clipboard
                                (this seems to be the easiest fix to a complicated crash bug,
                                 which happens if you paste a line with exactly one \n\r at the
                                 end copied from another application)
*/

#include "vi.h"
#include <dos.h>

#if defined( __NT__ )

#define _HUGE_
#define GET_POINTER( a, b ) (&a[b])
#define INC_POINTER( a )    (a++)

#elif defined( __WINDOWS_386__ )

#define _HUGE_              __far
#define GET_POINTER( a, b ) getHugePointer( a, b )
#define INC_POINTER( a )    (a = getHugePointer( a, 1 ))

#else

#define _HUGE_              __huge
#define GET_POINTER( a, b ) (&a[b])
#define INC_POINTER( a )    (a++)

#endif

#ifdef __WINDOWS_386__
/*
 * getHugePointer - given a 16-bit far pointer and an offset, return the
 *                  far pointer
 */
void far *getHugePointer( void far *ptr, unsigned off )
{
    DWORD       poff;
    WORD        pseg;

    pseg = FP_SEG( ptr );
    poff = FP_OFF( ptr );
    poff += off;
    pseg += (poff >> 16) * 8;
    poff &= 0xffff;
    return( MK_FP( pseg, poff ) );

} /* getHugePointer */
#endif

/*
 * openClipboardForRead - try to open the clipboard for read access
 */
static bool openClipboardForRead( void )
{
    if( OpenClipboard( Root ) ) {
        if( IsClipboardFormatAvailable( CF_TEXT ) ||
            IsClipboardFormatAvailable( CF_OEMTEXT )) {
                return( TRUE );
        }
        CloseClipboard();
    }
    return( FALSE );

} /* openClipboardForRead */

/*
 * openClipboardForWrite - try to open the clipboard for read access
 */
static bool openClipboardForWrite( void )
{
    if( OpenClipboard( Root ) ) {
        if( EmptyClipboard() ) {
            return( TRUE );
        }
        CloseClipboard();
    }
    return( FALSE );

} /* openClipboardForWrite */

/*
 * AddLineToClipboard - add data on specified line to the clipboard
 */
int AddLineToClipboard( char *data, int scol, int ecol )
{
    char __FAR__        *ptr;
    GLOBALHANDLE        hglob;
    int                 len;

    if( !openClipboardForWrite() ) {
        return( ERR_CLIPBOARD );
    }

    /*
     * get memory for line
     */
    len = ecol - scol + 2;
    hglob = GlobalAlloc( GMEM_MOVEABLE, len );
    if( hglob == NULL ) {
        CloseClipboard();
        return( ERR_CLIPBOARD );
    }

    ptr = MAKEPTR( GlobalLock( hglob ) );
    if( ptr == NULL ) {
        CloseClipboard();
        return( ERR_CLIPBOARD );
    }

    /*
     * copy line data and put to clipboard
     */
    MEMCPY( ptr, &data[scol], len - 1 );
    ptr[len - 1] = 0;
    GlobalUnlock( hglob );
    SetClipboardData( CF_TEXT, hglob );
    CloseClipboard();

    return( ERR_NO_ERR );

} /* AddLineToClipboard */

/*
 * AddFcbsToClipboard - add all lines in a given set of fcbs to the clipboard
 */
int AddFcbsToClipboard( fcb_list *fcblist )
{
    fcb                 *cfcb;
    line                *cline;
    char                _HUGE_ *ptr;
    long                size;
    int                 i;
    GLOBALHANDLE        hglob;
    BOOL                crlf_left_to_write = 0;

    if( !openClipboardForWrite() ) {
        return( ERR_CLIPBOARD );
    }

    /*
     * compute the number of bytes in total
     */
    size = 1;   // for trailing null char
    for( cfcb = fcblist->head; cfcb != NULL; cfcb = cfcb->next ) {
        size += (long)cfcb->byte_cnt +
                (long)(cfcb->end_line-cfcb->start_line + 1);
        if( cfcb == fcblist->tail ) {
            break;
        }
    }

    /*
     * get the memory to store this stuff
     */
    hglob = GlobalAlloc( GMEM_MOVEABLE, size );
    if( hglob == NULL ) {
        CloseClipboard();
        return( ERR_CLIPBOARD );
    }

    ptr = MAKEPTR( GlobalLock( hglob ) );
    if( ptr == NULL ) {
        CloseClipboard();
        return( ERR_CLIPBOARD );
    }

    /*
     * copy all lines into this pointer
     */
    for( cfcb = fcblist->head; cfcb != NULL; cfcb = cfcb->next ) {
        FetchFcb( cfcb );
        for( cline = cfcb->lines.head; cline != NULL; cline = cline->next ) {
            // one CRLF left to write?
            if( crlf_left_to_write ) {
                // yes: write it
                crlf_left_to_write = 0;
                *ptr = CR;
                INC_POINTER( ptr );
                *ptr = LF;
                INC_POINTER( ptr );
            }
            for( i = 0; i < cline->len; i++ ) {
                *ptr = cline->data[i];
                INC_POINTER( ptr );
            }
            // remember to write one CRLF next time
            crlf_left_to_write = 1;
        }
        if( cfcb == fcblist->tail ) {
            break;
        }
    }
    // the last CRLF is omitted
    *ptr = 0;
    GlobalUnlock( hglob );
    SetClipboardData( CF_TEXT, hglob );
    CloseClipboard();
    return( ERR_NO_ERR );

} /* AddFcbsToClipboard */


/*
 * addAnFcb - add a new fcb and its data
 */
static int addAnFcb( fcb_list *fcblist, int numbytes )
{
    fcb         *cfcb;
    int         linecnt;
    int         used;

    cfcb = FcbAlloc( NULL );
    AddLLItemAtEnd( (ss **)&fcblist->head, (ss **)&fcblist->tail, (ss *)cfcb );
    CreateLinesFromBuffer( numbytes, &cfcb->lines, &used, &linecnt, &(cfcb->byte_cnt) );
    if( fcblist->tail->prev == NULL ) {
        cfcb->start_line = 1;
    } else {
        cfcb->start_line = fcblist->tail->prev->end_line + 1;
    }
    cfcb->end_line = cfcb->start_line + linecnt - 1;
    return( used );

} /* addAnFcb */

/*
 * GetClipboardSavebuf - gets data from the clipboard, and builds a
 *                       temporary savebuf from it
 */
int GetClipboardSavebuf( savebuf *clip )
{
    GLOBALHANDLE        hglob;
    char                _HUGE_ *ptr;
    char                _HUGE_ *cpos;
    fcb_list            fcblist;
    int                 i;
    bool                is_flushed;
    bool                has_lf;
    bool                record_done;
    char                ch;
    int                 used;

    if( !openClipboardForRead() ) {
        return( ERR_CLIPBOARD_EMPTY );
    }
    hglob = GetClipboardData( CF_TEXT );
    if( hglob == NULL ) {
        return( ERR_CLIPBOARD );
    }
    ptr = MAKEPTR( GlobalLock( hglob ) );
    cpos = ptr;
    i = 0;
    is_flushed = FALSE;
    has_lf = FALSE;
    fcblist.head = NULL;
    fcblist.tail = NULL;
    record_done = FALSE;

    /*
     * add all characters to ReadBuffer.  Each time this fills,
     * create a new FCB
     */
    while( (ch = *ptr) != '\0' ) {
        INC_POINTER( ptr );
        ReadBuffer[i++] = ch;
        if( ch == LF ) {
            has_lf = TRUE;
        }
        if( i >= MAX_IO_BUFFER ) {
            is_flushed = TRUE;
            used = addAnFcb( &fcblist, i );
            ptr = GET_POINTER( cpos, used );
            cpos = ptr;
            i = 0;
        }
    }

    /*
     * after we are done, see if any characters are left unprocessed
     */
    if( i != 0 ) {
        /*
         * check if this is a partial line
         */
        if( !is_flushed && !has_lf ) {
            clip->type = SAVEBUF_LINE;
            ReadBuffer[i] = 0;
            clip->u.data = MemAlloc( i + 1 );
            strcpy( clip->u.data, ReadBuffer );
            record_done = TRUE;
        } else {
            // add CRLF to end of buffer
            if( i >= MAX_IO_BUFFER - 2 ) {
                addAnFcb( &fcblist, i );
                i = 0;
            }
            ReadBuffer[i++] = CR;
            ReadBuffer[i++] = LF;
            addAnFcb( &fcblist, i );
        }
    } else if( !is_flushed ) {
        clip->type = SAVEBUF_NOP;
        record_done = TRUE;
    }

    if( !record_done ) {
        clip->type = SAVEBUF_FCBS;
        clip->u.fcbs.head = fcblist.head;
        clip->u.fcbs.tail = fcblist.tail;
    }

    GlobalUnlock( hglob );
    CloseClipboard();

    return( ERR_NO_ERR );

} /* GetClipboardSavebuf */

/*
 * IsClipboardEmpty - check if the clipboard is empty or not
 */
bool IsClipboardEmpty( void )
{
    if( !openClipboardForRead() ) {
        return( TRUE );
    }
    CloseClipboard();
    return( FALSE );

} /* IsClipboardEmpty */
