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
#include "vi.h"

extern long TabCnt;

/*
 * doCompressExpand - convert tabs to spaces, and spaces to tabs
 */
static int doCompressExpand( bool compress )
{
    int         i,k;
    long        bytes_saved=0;
    long        bytes_added=0;
    long        otabcnt;
    linenum     linecnt=0;
    char        *tmp;

    /*
     * init
     */
    if( i = ModificationTest() ) {
        return( i );
    }
    if( i = SaveAndResetFilePos( 1 ) ) {
        return( i );
    }
    tmp = StaticAlloc();

    /*
     * process all lines
     */
    TabCnt = 0;
    while( TRUE ) {

        if( compress ) {
            otabcnt = TabCnt;
            ExpandTabsInABuffer( CurrentLine->data, CurrentLine->len, tmp,
                        MaxLine );
            TabCnt = otabcnt;
            k = strlen( tmp );
            ConvertSpacesToTabsUpToColumn( k, tmp, k, WorkLine->data, MaxLine );
            WorkLine->len = strlen( WorkLine->data );
            bytes_saved += CurrentLine->len - WorkLine->len;
        } else {
            ExpandTabsInABuffer( CurrentLine->data, CurrentLine->len,
                        WorkLine->data, MaxLine );
            WorkLine->len = strlen( WorkLine->data );
            bytes_added += WorkLine->len - CurrentLine->len;
        }
        ReplaceCurrentLine();

        /*
         * get next line
         */
        linecnt++;
        i = CGimmeNextLinePtr( &CurrentFcb, &CurrentLine );
        if( i ) {
            if( i == ERR_NO_MORE_LINES ) {
                break;
            }
            RestoreCurrentFilePos();
            StaticFree( tmp );
            return( i );
        }

    }

    StaticFree( tmp );
    RestoreCurrentFilePos();

    Modified( TRUE );
    DCDisplayAllLines();
    Message1( "%l lines processed in \"%s\"", linecnt, CurrentFile->name );
    if( compress ) {
        Message2( " %l tabs added (%l bytes saved)", TabCnt, bytes_saved );
    } else {
        Message2( " %l tabs removed (%l bytes added)", TabCnt, bytes_added );
    }
    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* doCompressExpand */

/*
 * ExpandWhiteSpace - replace tabs with white space
 */
int ExpandWhiteSpace( void )
{
    return( doCompressExpand( FALSE ) );

} /* ExpandWhiteSpace */

/*
 * CompressWhiteSpace - replace white space with tabs
 */
int CompressWhiteSpace( void )
{
    return( doCompressExpand( TRUE ) );

} /* CompressWhiteSpace */
