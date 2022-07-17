/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
#include "myio.h"
#include "myprintf.h"


static bool historyLoaded;

/*
 * updateHist - add a string to a history list
 */
static void updateHist( history_data *h, const char *str )
{
    if( h->curr < h->max ) {
        ReplaceString( &h->data[h->curr], str );
        h->curr++;
    }

} /* updateHist */

/*
 * LoadHistory - load history from file
 */
void LoadHistory( const char *cmd )
{
    FILE            *fp;
    char            str[MAX_INPUT_LINE];
    int             cnt;
    int             j;
    history_data    *h;

    historyLoaded = true;
    if( EditVars.HistoryFile != NULL ) {
        fp = fopen( EditVars.HistoryFile, "rt" );
        if( fp != NULL ) {
            cnt = 0;
            h = EditVars.Hist - 1;
            while( myfgets( str, sizeof( str ), fp ) != NULL ) {
                if( cnt == 0 ) {
                    h++;
                    if( h - EditVars.Hist >= MAX_HIST ) {
                        break;
                    }
                    for( j = 0; j < h->max; j++ ) {
                        h->data[j] = NULL;
                    }
                    cnt = atoi( str );
                    h->curr = 0;
                    continue;
                }
                updateHist( h, str );
                cnt--;
            }
            fclose( fp );
        }
    }
    if( cmd != NULL ) {
        h = &EditVars.Hist[HIST_CMD];
        ReplaceString( &h->data[h->curr % h->max], cmd );
        h->curr++;
    }

} /* LoadHistory */

/*
 * getHistCount - get the number of items in a history
 */
static int getHistCount( hist_type ht )
{
    int     i;
    int     j;
    int     cnt;

    /*
     * get number of items in find history
     */
    j = EditVars.Hist[ht].curr;
    cnt = 0;
    for( i = 0; i < EditVars.Hist[ht].max; i++ ) {
        if( EditVars.Hist[ht].data[j % EditVars.Hist[ht].max] != NULL ) {
            cnt++;
        }
        j++;
    }
    return( cnt );

} /* getHistCount */

/*
 * writeHistory - write out history to a file
 */
static void writeHistory( FILE *fp, hist_type ht )
{
    int     i;
    int     j;
    int     k;

    MyFprintf( fp, "%d\n", getHistCount( ht ) );
    j = EditVars.Hist[ht].curr;
    for( i = 0; i < EditVars.Hist[ht].max; i++ ) {
        k = j % EditVars.Hist[ht].max;
        if( EditVars.Hist[ht].data[k] != NULL ) {
            MyFprintf( fp, "%s\n", EditVars.Hist[ht].data[k] );
            _MemFreeArray( EditVars.Hist[ht].data[k] );
            EditVars.Hist[ht].data[k] = NULL;
        }
        j++;
    }

} /* writeHistory */

/*
 * SaveHistory - save history to file
 */
void SaveHistory( void )
{
    FILE        *fp;
    hist_type   ht;

    if( historyLoaded ) {
        if( EditVars.HistoryFile != NULL ) {
            fp = fopen( EditVars.HistoryFile, "wt" );
            if( fp != NULL ) {
                for( ht = 0; ht < MAX_HIST; ht++ ) {
                    writeHistory( fp, ht );
                }
                fclose( fp );
            }
        }
    }

} /* SaveHistory */

/*
 * HistInitSingle - single initial bang history structure
 */
void HistInitSingle( hist_type ht, int max )
{
    EditVars.Hist[ht].max = max;
    EditVars.Hist[ht].curr = 0;
    EditVars.Hist[ht].data = _MemReallocPtrArray( EditVars.Hist[ht].data, char, max + 1 );

} /* HistInitSingle */

/*
 * HistInit - init all history structures
 */
void HistInit( void )
{
    hist_type ht;

    for( ht = 0; ht < MAX_HIST; ht++ ) {
        HistInitSingle( ht, EditVars.Hist[ht].max );
    }
}

/*
 * HistFini - deallocate all history structures
 */
void HistFini( void )
{
    hist_type ht;

    for( ht = 0; ht < MAX_HIST; ht++ ) {
        // array elements are freed in WriteHistory/SaveHistory
        _MemFreePtrArray( EditVars.Hist[ht].data, 0, NULL );
    }
    MemFree( EditVars.HistoryFile );
    EditVars.HistoryFile = NULL;
}
