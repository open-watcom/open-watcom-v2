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

static char *buffPtr;

/*
 * create a line from a string of chars
 */
static int createLine( char *res )
{
    char        c;
    char        *buff;
    int         len;

    len = 0;
    buff = buffPtr;
    while( 1 ) {

        c = *buff;
        if( c < 32 ) {
            if( c == '\t' ) {
                if( !EditFlags.RealTabs ) {
                    int tb, i;

                    tb = Tab( len + 1, HardTab );
                    if( len + tb >= MaxLinem1 ) {
                        *res = 0;
                        buffPtr = buff;
                        return( len );
                    }
                    len += tb;
                    for( i = 0; i < tb; i++ ) {
                        *res++ = ' ';
                    }
                    buff++;
                    continue;
                }
            } else {
                if( c == CR ) {
                    if( EditFlags.CRLFAutoDetect ) {
                        EditFlags.WriteCRLF = TRUE;
                    }
                    buff++;
                    continue;
                }
                if( c == CTLZ && !EditFlags.IgnoreCtrlZ ) {
                    buffPtr = buff;
                    *res = 0;
                    return( len );
                }
                if( c == LF || c == 0 ) {
                    buffPtr = buff + 1;
                    *res = 0;
                    return( len );
                }
            }
        }
        if( len == MaxLinem1 ) {
            *res = 0;
            buffPtr = buff;
            return( len );
        }
        *res++ = c;
        len++;
        buff++;

    }

} /* createLine */

/*
 * CreateLinesFromBuffer - create a set of lines from specified buffer
 */
bool CreateLinesFromBuffer( int cnt, line_list *linelist, int *used,
                            int *lcnt, short *bytecnt )
{
    int         curr, copylen, total, tmpmio;
    short       bcnt, llcnt;
    char        *tmpbuff;

    total = bcnt = copylen = llcnt = curr = 0;
    linelist->tail = linelist->head = NULL;
    tmpmio = MAX_IO_BUFFER - 2;
    tmpbuff = StaticAlloc();

    /*
     * this zero makes sure that the file will always have a termination
     */
    ReadBuffer[cnt] = 0;

    while( TRUE ) {

        /*
         * check if we are past the end of the buffer
         */
        if( curr >= cnt ) {
            StaticFree( tmpbuff );
            *used = curr;
            *bytecnt = bcnt;
            *lcnt = llcnt;
            return( FALSE );
        }

        /*
         * get next string
         */
        buffPtr = &ReadBuffer[curr];
        if( *buffPtr == CTLZ && !EditFlags.IgnoreCtrlZ ) {
            StaticFree( tmpbuff );
            *bytecnt = bcnt;
            *used = curr;
            *lcnt = llcnt;
            return( TRUE );
        } else {
            copylen = createLine( tmpbuff );

            /*
             * try to add in these new bytes: +4 because of swap file
             * considerations: +2 for c/r,l/f and +2 for extra line data
             */
            total += copylen + LINE_EXTRA;
            if( total >= tmpmio ) {
                StaticFree( tmpbuff );
                *bytecnt = bcnt;
                *used = curr;
                *lcnt = llcnt;
                return( FALSE );
            }

            /*
             * update line counts and buffer pointer
             */
            InsertNewLine( linelist->tail, linelist, tmpbuff, copylen, INSERT_AFTER );
            llcnt++;
            bcnt += copylen + 1;
            curr = (int) (buffPtr - ReadBuffer);
        }

    }

} /* CreateLinesFromBuffer */
