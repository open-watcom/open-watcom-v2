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


#define isWSorCtrlZ(x)  (isspace( x ) || (x == 0x1A))

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
    size_t          i;
    int             j;
    history_data    *h;

    historyLoaded = true;
    while( EditVars.HistoryFile != NULL ) {
        fp = fopen( EditVars.HistoryFile, "rt" );
        if( fp == NULL ) {
            break;
        }
        cnt = 0;
        h = EditVars.Hist - 1;
        while( fgets( str, MAX_INPUT_LINE, fp ) != NULL ) {
            for( i = strlen( str ); i && isWSorCtrlZ( str[i - 1] ); --i ) {
                str[i - 1] = '\0';
            }
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
        break;
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
static int getHistCount( history_data *h )
{
    int     i;
    int     j;
    int     cnt;

    /*
     * get number of items in find history
     */
    j = h->curr;
    cnt = 0;
    for( i = 0; i < h->max; i++ ) {
        if( h->data[j % h->max] != NULL ) {
            cnt++;
        }
        j++;
    }
    return( cnt );

} /* getHistCount */

/*
 * writeHistory - write out history to a file
 */
static void writeHistory( FILE *fp, history_data *h )
{
    int     i;
    int     j;

    MyFprintf( fp, "%d\n", getHistCount( h ) );
    j = h->curr;
    for( i = 0; i < h->max; i++ ) {
        if( h->data[j % h->max] != NULL ) {
            MyFprintf( fp, "%s\n", h->data[j % h->max] );
            DeleteString( &h->data[j % h->max] );
        }
        j++;
    }

} /* writeHistory */

/*
 * SaveHistory - save history to file
 */
void SaveHistory( void )
{
    FILE            *fp;
    history_data    *h;

    if( historyLoaded ) {
        if( EditVars.HistoryFile != NULL ) {
            fp = fopen( EditVars.HistoryFile, "wt" );
            if( fp != NULL ) {
                for( h = EditVars.Hist; h - EditVars.Hist < MAX_HIST; h++ ) {
                    writeHistory( fp, h );
                }
                fclose( fp );
            }
        }
    }

} /* SaveHistory */

/*
 * HistInit - initial bang history structure
 */
void HistInit( history_data *h, int max )
{
    h->max = max;
    h->curr = 0;
    h->data = MemReAllocList( h->data, h->max + 1 );

} /* HistInit */


void HistFini( void )
{
    history_data    *h;

    for( h = EditVars.Hist; h - EditVars.Hist < MAX_HIST; h++ ) {
        MemFree( h->data );
    }
    MemFree( EditVars.HistoryFile );
}
