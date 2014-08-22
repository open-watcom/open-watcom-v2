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

extern long TabCnt;

/*
 * doCompressExpand - convert tabs to spaces, and spaces to tabs
 */
static vi_rc doCompressExpand( bool compress )
{
    int         k;
    long        bytes_saved = 0;
    long        bytes_added = 0;
    long        otabcnt;
    linenum     linecnt = 0;
    char        *tmp;
    vi_rc       rc;

    /*
     * init
     */
    rc = ModificationTest();
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    rc = SaveAndResetFilePos( 1 );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    tmp = StaticAlloc();

    /*
     * process all lines
     */
    TabCnt = 0;
    for( ;; ) {

        if( compress ) {
            otabcnt = TabCnt;
            ExpandTabsInABuffer( CurrentLine->data, CurrentLine->len, tmp, EditVars.MaxLine );
            TabCnt = otabcnt;
            k = strlen( tmp );
            ConvertSpacesToTabsUpToColumn( k, tmp, k, WorkLine->data, EditVars.MaxLine );
            WorkLine->len = strlen( WorkLine->data );
            bytes_saved += CurrentLine->len - WorkLine->len;
        } else {
            ExpandTabsInABuffer( CurrentLine->data, CurrentLine->len, WorkLine->data, EditVars.MaxLine );
            WorkLine->len = strlen( WorkLine->data );
            bytes_added += WorkLine->len - CurrentLine->len;
        }
        ReplaceCurrentLine();

        /*
         * get next line
         */
        linecnt++;
        rc = CGimmeNextLinePtr( &CurrentFcb, &CurrentLine );
        if( rc != ERR_NO_ERR ) {
            if( rc == ERR_NO_MORE_LINES ) {
                break;
            }
            RestoreCurrentFilePos();
            StaticFree( tmp );
            return( rc );
        }

    }

    StaticFree( tmp );
    RestoreCurrentFilePos();

    Modified( true );
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
vi_rc ExpandWhiteSpace( void )
{
    return( doCompressExpand( false ) );

} /* ExpandWhiteSpace */

/*
 * CompressWhiteSpace - replace white space with tabs
 */
vi_rc CompressWhiteSpace( void )
{
    return( doCompressExpand( true ) );

} /* CompressWhiteSpace */
