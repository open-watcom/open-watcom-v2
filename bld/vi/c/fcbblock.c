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
#include "fcbmem.h"

/*
 * GiveBackBlock - return a block to the swap file
 */
void GiveBackBlock( long offset, unsigned char *blocks )
{
    int i, j, k, l;

    /*
     * get block index
     */
    i = (int) ( offset / (long) MAX_IO_BUFFER );

    /*
     * compute array element
     */
    j = i / 8;

    /*
     * compute which bit needs to be turned on
     */
    k = (i % 8);
    k = 7 - k;
    l = 1 << k;

    /*
     * turn bit on
     */
    blocks[j] |= l;

} /* GiveBackBlock */

/*
 * GetNewBlock - find free space in some storage area
 */
bool GetNewBlock( long *p, unsigned char *blocks, int size )
{
    int i, j, k, l;

    /*
     * hunt for an empty block
     */
    for( i = 0; i < size; i++ ) {
        if( blocks[i] ) {
            /*
             * now, hunt for an single position
             */
            k = blocks[i];
            j = 0x80;
            l = 8 * i;
            for( ;; ) {
                /*
                 * if we find a position, return it
                 */
                if( j & k ) {
                    blocks[i] &= (0xff - j);
                    *p = (long) l * (long) MAX_IO_BUFFER;
                    return( true );
                }
                j >>= 1;
                l++;
            }
        }
    }

    return( false );

} /* GetNewBlock */

/*
 * MakeWriteBlock - create a writeable block from the fcb data
 */
int MakeWriteBlock( fcb *fb )
{
    line        *cline, *tline;
    char        *buff;
    int         len = 0;

    /*
     * build a block
     */
    buff = WriteBuffer;
    for( cline = fb->lines.head; cline != NULL; cline = cline->next ) {
        memcpy( buff, cline->data, cline->len );
        buff += cline->len;
        *buff++ = CR;
        *buff++ = LF;
        len += cline->len;
    }

    /*
     * swap line data
     */
    for( cline = fb->lines.head; cline != NULL; cline = tline ) {
        *(linedata_t *)buff = cline->u.ld_word;
        buff += sizeof( linedata_t );
        tline = cline->next;
        MemFree( cline );
        len += LINE_EXTRA; /* 2 for these and 2 for CR,LF */
    }

    if( len != FcbSize( fb ) ) {
        AbandonHopeAllYeWhoEnterHere( 666 );
    }

    return( len );

} /* MakeWriteBlock */
