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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "auipvt.h"
#include <string.h>

#define _ModIndex( ind, adj )   (ind += adj, ind &= (SAVE_SIZE-1));

/*
 * InitHistory -- initialize retrieving of commands
 */

void *WndInitHistory()
{
    save_area   *new;

    new = WndAlloc( sizeof( *new ) );
    if( new == NULL ) return( NULL );
    new->first_cmd = 0;
    new->curr_cmd = 0;
    new->area[ 0 ] = 0;
    new->area[ 1 ] = 0;
    new->last_was_next = FALSE;
    new->first_free = 2;
    return( new );
}

void WndFiniHistory( void *save )
{
    WndFree( save );
}


/*
 * SaveByte -- save a single byte
 */

static void SaveByte( save_area *save, char ch )
{

    if( save->first_free == save->first_cmd ) {
        _ModIndex( save->first_cmd, save->area[ save->first_cmd ] + 2 );
    }
    save->area[ save->first_free ] = ch;
    _ModIndex( save->first_free, 1 );
}


static void SaveIt( save_area *save, char *cmd, unsigned len )
{
    unsigned  save_len;

    save->curr_cmd = save->first_free;
    save_len = len;
    SaveByte( save, save_len );
    do {
        SaveByte( save, *cmd );
        ++cmd;
    } while( --len != 0 );
    SaveByte( save, save_len );
}



/*
 * SaveCmd -- save a command
 */

void WndSaveToHistory( save_area *save, char *cmd )
{
    unsigned  chk, chk_len, len;

    if( save == NULL ) return;
    if( cmd == NULL ) return;
    len = strlen( cmd );
    save->last_was_next = FALSE;
    save->curr_cmd = save->first_free;
    _ModIndex( save->curr_cmd, -1 );
    _ModIndex( save->curr_cmd, -(save->area[ save->curr_cmd ] + 1) );
    if( len != 0 ) {
        chk = save->curr_cmd;
        if( save->area[ chk ] != len ) {
            SaveIt( save, cmd, len );
        } else {
            for( chk_len = 0; chk_len < len; ++chk_len ) {
                _ModIndex( chk, 1 );
                if( save->area[ chk ] != cmd[ chk_len ] ) {
                    SaveIt( save, cmd, len );
                    return;
                }
            }
        }
    }
}


/*
 * PrevCmd -- get previous command
 */

bool WndPrevFromHistory( save_area *save, char *cmd )
{
    unsigned  len;
    unsigned  cnt;
    unsigned  curr;

    if( save == NULL ) return( 0 );
    if( save->curr_cmd == save->first_cmd ) {
        save->last_was_next = TRUE;
        return( 0 );
    }
    curr = save->curr_cmd;
    _ModIndex( save->curr_cmd, -1 );
    _ModIndex( save->curr_cmd, -(save->area[ save->curr_cmd ] + 1) );
    if( save->last_was_next ) {
        if( save->curr_cmd == save->first_cmd ) return( 0 );
        curr = save->curr_cmd;
        _ModIndex( save->curr_cmd, -1 );
        _ModIndex( save->curr_cmd, -(save->area[ save->curr_cmd ] + 1) );
    }
    save->last_was_next = FALSE;
    len = save->area[ curr ];
    cnt = len;
    do {
        _ModIndex( curr, 1 );
        *cmd = save->area[ curr ];
        ++cmd;
    } while( !( --cnt == 0 ) );
    *cmd = '\0';
    return( len != 0 );
}



/*
 * NextCmd -- get next command
 */

bool WndNextFromHistory( save_area *save, char *cmd )
{
    unsigned  len;
    unsigned  cnt;
    unsigned  curr;

    if( save == NULL ) return( 0 );
    curr = save->curr_cmd;
    _ModIndex( curr, save->area[ curr ] + 2 );
    if( !save->last_was_next ) {
        if( curr == save->first_free ) return( 0 );
        save->curr_cmd = curr;
        _ModIndex( curr, save->area[ curr ] + 2 );
    }
    save->last_was_next = FALSE;
    if( curr == save->first_free ) return( 0 );
    save->last_was_next = TRUE;
    save->curr_cmd = curr;
    len = save->area[ curr ];
    cnt = len;
    do {
        _ModIndex( curr, 1 );
        *cmd = save->area[ curr ];
        ++cmd;
    } while( !( --cnt == 0 ) );
    *cmd = '\0';
    return( len != 0 );
}
