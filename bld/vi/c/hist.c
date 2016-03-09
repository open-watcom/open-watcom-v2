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

typedef enum {
    READ_NONE,
    READ_CMD,
    READ_FIND,
    READ_FILTER,
    READ_LASTFILES,
    READ_LAST
} read_state;


/*
 * updateHist - add a string to a history list
 */
static void updateHist( history_data *hist, const char *str )
{
    if( hist->curr < hist->max ) {
        ReplaceString( &hist->data[hist->curr], str );
        hist->curr++;
    }

} /* updateHist */

/*
 * LoadHistory - load history from file
 */
void LoadHistory( const char *cmd )
{
    FILE        *f;
    char        str[MAX_INPUT_LINE];
    int         cnt;
    read_state  rs;
    int         i;

    historyLoaded = true;

    while( EditVars.HistoryFile != NULL ) {
        f = fopen( EditVars.HistoryFile, "rt" );
        if( f == NULL ) {
            break;
        }
        cnt = 0;
        rs = READ_NONE;
        while( fgets( str, MAX_INPUT_LINE, f ) != NULL ) {
            for( i = strlen( str ); i && isWSorCtrlZ( str[i - 1 ]); --i ) {
                str[i - 1] = '\0';
            }
            if( cnt == 0 ) {
                cnt = atoi( str );
                rs++;
                if( rs >= READ_LAST ) {
                    break;
                }
                continue;
            }
            switch( rs ) {
            case READ_CMD:
                updateHist( &EditVars.CLHist, str );
                break;
            case READ_FIND:
                updateHist( &EditVars.FindHist, str );
                break;
            case READ_FILTER:
                updateHist( &EditVars.FilterHist, str );
                break;
            case READ_LASTFILES:
                updateHist( &EditVars.LastFilesHist, str );
                break;
            }
            cnt--;
        }
        fclose( f );
        break;
    }
    if( cmd != NULL ) {
        ReplaceString( &EditVars.CLHist.data[EditVars.CLHist.curr % EditVars.CLHist.max], cmd );
        EditVars.CLHist.curr++;
    }

} /* LoadHistory */

/*
 * getHistCount - get the number of items in a history
 */
static int getHistCount( history_data *hist )
{
    int i, j;
    int cnt;

    /*
     * get number of items in find history
     */
    j = hist->curr;
    cnt = 0;
    for( i = 0; i < hist->max; i++ ) {
        if( hist->data[j % hist->max] != NULL ) {
            cnt++;
        }
        j++;
    }
    return( cnt );

} /* getHistCount */

/*
 * writeHistory - write out history to a file
 */
static void writeHistory( FILE *f, history_data *hist )
{
    int i, j;

    MyFprintf( f, "%d\n", getHistCount( hist ) );
    j = hist->curr;
    for( i = 0; i < hist->max; i++ ) {
        if( hist->data[j % hist->max] != NULL ) {
            MyFprintf( f, "%s\n", hist->data[j % hist->max] );
            DeleteString( &hist->data[j % hist->max] );
        }
        j++;
    }

} /* writeHistory */

/*
 * SaveHistory - save history to file
 */
void SaveHistory( void )
{
    FILE        *f;

    if( EditVars.HistoryFile == NULL || !historyLoaded ) {
        return;
    }

    f = fopen( EditVars.HistoryFile, "wt" );
    if( f == NULL ) {
        return;
    }
    writeHistory( f, &EditVars.CLHist );
    writeHistory( f, &EditVars.FindHist );
    writeHistory( f, &EditVars.FilterHist );
    writeHistory( f, &EditVars.LastFilesHist );
    fclose( f );

} /* SaveHistory */

/*
 * FilterHistInit - initial bang history structure
 */
void FilterHistInit( int max )
{
    EditVars.FilterHist.max = max;
    EditVars.FilterHist.curr = 0;
    EditVars.FilterHist.data = MemReAlloc( EditVars.FilterHist.data,
        (EditVars.FilterHist.max + 1) * sizeof( char * ) );

} /* FilterHistInit */

/*
 * CLHistInit - initial command line history structure
 */
void CLHistInit( int max )
{
    EditVars.CLHist.max = max;
    EditVars.CLHist.curr = 0;
    EditVars.CLHist.data = MemReAlloc( EditVars.CLHist.data, (EditVars.CLHist.max + 1) * sizeof( char * ) );

} /* CLHistInit */

/*
 * FindHistInit - initial find history structure
 */
void FindHistInit( int max )
{
    EditVars.FindHist.max = max;
    EditVars.FindHist.curr = 0;
    EditVars.FindHist.data = MemReAlloc( EditVars.FindHist.data, (EditVars.FindHist.max + 1) * sizeof( char * ) );

} /* FindHistInit */

/*
 * LastFilesHistInit - initial find history structure
 */
void LastFilesHistInit( int max )
{
    EditVars.LastFilesHist.max = max;
    EditVars.LastFilesHist.curr = 0;
    EditVars.LastFilesHist.data = MemReAlloc( EditVars.LastFilesHist.data,
        (EditVars.LastFilesHist.max + 1) * sizeof( char * ) );

} /* LastFilesHistInit */

void HistFini( void )
{
    MemFree( EditVars.LastFilesHist.data );
    MemFree( EditVars.FindHist.data );
    MemFree( EditVars.CLHist.data );
    MemFree( EditVars.FilterHist.data );
    MemFree( EditVars.HistoryFile );
}
