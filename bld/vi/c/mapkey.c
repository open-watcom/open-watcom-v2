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
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "vi.h"
#include "keys.h"
#include "win.h"

static bool     keysRead = FALSE;
static char     *charTokens;
static int      *charVals;

/*
 * readKeyData - do just that
 */
static int readKeyData( void )
{
    int         *vals;
    int         rc,cnt;
    char        *buff;

    if( keysRead ) {
        return( ERR_NO_ERR );
    }
#ifdef __WIN__
    rc = ReadDataFile( "winkeys.dat", &cnt, &buff, &vals, TRUE );
#else
    rc = ReadDataFile( "keys.dat", &cnt, &buff, &vals, TRUE );
#endif
    if( rc ) {
        return( rc );
    }
    charTokens = buff;
    charVals = vals;
    keysRead = TRUE;
    return( ERR_NO_ERR );

} /* readKeyData */

/*
 * MapKey - set up a key mapping
 */
int MapKey( int flag, char *data )
{
    char        key[MAX_STR];
    key_map     *maps;
    int         rc,j,k;

    if( !EditFlags.ScriptIsCompiled || (flag & MAPFLAG_UNMAP) ) {
        rc = readKeyData();
        if( rc ) {
            return( rc );
        }
    }

    /*
     * get if it is an input/regular key mapping
     */
    if( flag & MAPFLAG_DAMMIT ) {
        maps = InputKeyMaps;
    } else {
        maps = KeyMaps;
    }

    if( NextWord1( data, key ) <= 0 ) {
        return( ERR_INVALID_MAP );
    }
    RemoveLeadingSpaces( data );

    /*
     * get key we are using
     */
    if( !EditFlags.ScriptIsCompiled || (flag & MAPFLAG_UNMAP) ) {
        j = Tokenize( charTokens, key, TRUE );
        if( j<0 ) {
            k = (int) key[0];
        } else {
            k = charVals[j];
        }
    } else {
        k = atoi( key );
    }

    if( EditFlags.CompileScript ) {
        if( !(flag & MAPFLAG_UNMAP)) {
            key_map     scr;

            rc = AddKeyMap( &scr, data, strlen( data ) );
            if( !rc ) {
                if( scr.no_input_window ) {
                    MySprintf( WorkLine->data,"%d \\x%s", k, scr.data );
                } else {
                    MySprintf( WorkLine->data,"%d %s", k, scr.data );
                }
            }
            MemFree( scr.data );
            return( rc );

        } else {
            MySprintf( WorkLine->data,"%d %s", k, data );
            return( ERR_NO_ERR );
        }
    }

    if( k < 0 || k >= EventCount ) {
        return( ERR_INVALID_KEY );
    }
    maps[k].inuse = FALSE;
    maps[k].is_base = FALSE;
    MemFree( maps[k].data );
    maps[k].data = NULL;
    if( !(flag & MAPFLAG_UNMAP )) {
        if( flag & MAPFLAG_BASE ) {
            maps[k].is_base = TRUE;
        }
        return( AddKeyMap( &maps[k], data, strlen( data ) ) );
    }
    return( ERR_NO_ERR );

} /* MapKey */

/*
 * DoKeyMap - process a key mapping
 */
int DoKeyMap( int scr )
{
    int         rc;
    long        total;
    int         i;
    bool        was_base=FALSE;

    KeyMaps[ scr ].inuse = TRUE;
    if( KeyMaps[ scr ].is_base ) {
        was_base = TRUE;
        for( i=0;i<EventCount;i++ ) {
            KeyMaps[i].was_inuse = KeyMaps[i].inuse;
            KeyMaps[i].inuse = TRUE;
        }
    }
    total = GetRepeatCount();

    rc = RunKeyMap( &KeyMaps[ scr ], total );

    if( was_base ) {
        for( i=0;i<EventCount;i++ ) {
            KeyMaps[i].inuse = KeyMaps[i].was_inuse;
        }
    }
    KeyMaps[ scr ].inuse = FALSE;

    return( rc );

} /* DoKeyMap */

/*
 * doRunKeyMap - execute a key map a specified number of times
 */
static int doRunKeyMap( key_map *scr, long total  )
{
    int         max;
    int         rc=ERR_NO_ERR;
    undo_stack  *cstack;

    if( EditFlags.InputKeyMapMode ) {
        return( ERR_INPUT_KEYMAP_RUNNING );
    }
    cstack = UndoStack;
    StartUndoGroup( cstack );
    while( total > 0 ) {
        /*
         * set up key map to run
         */
        CurrentKeyMap = scr->data;
        if( scr->no_input_window ) {
            EditFlags.NoInputWindow = TRUE;
        }
        // max = strlen( CurrentKeyMap );
        for( max=0; CurrentKeyMap[max] != 0; max++ );

        CurrentKeyMapCount = 0;
        EditFlags.KeyMapInProgress = TRUE;
        EditFlags.KeyMapMode = TRUE;
        LastError = ERR_NO_ERR;

        /*
         * run until done
         */
        while( TRUE ) {
            if( CurrentKeyMapCount >= max ) {
                break;
            }
            LastEvent = (vi_key) GetNextEvent( FALSE );
            rc = DoLastEvent();
            if( rc > 0 || LastError ) {
                break;
            }
            DoneLastEvent( rc, TRUE );
        }

        EditFlags.KeyMapMode = FALSE;
        EditFlags.NoInputWindow = FALSE;
        EditFlags.KeyMapInProgress = FALSE;
        if( rc > 0 || LastError ) {
            break;
        }
        total--;

    }

    TryEndUndoGroup( cstack );
    EditFlags.Dotable = FALSE;
    return( rc );

} /* doRunKeyMap */

/*
 * RunKeyMap - run a key mapping
 */
int RunKeyMap( key_map *scr, long total  )
{
    int         oldcount,rc;
    bool        restore=FALSE;
    vi_key      *oldmap;

    /*
     * check if we are already running a key mapping; if so, save it
     */
    if( EditFlags.KeyMapInProgress ) {
        restore = TRUE;
        oldmap = CurrentKeyMap;
        oldcount = CurrentKeyMapCount;
    }

    rc = doRunKeyMap( scr, total );

    /*
     * if we were running one already, restore it
     */
    if( restore ) {
        CurrentKeyMap = oldmap;
        CurrentKeyMapCount = oldcount;
        EditFlags.KeyMapInProgress = TRUE;
        EditFlags.KeyMapMode = TRUE;
    }
    return( rc );

} /* RunKeyMap */

undo_stack *currUndoStack;
/*
 * StartInputKeyMap - start up input key map
 */
int StartInputKeyMap( int num )
{

    if( EditFlags.InputKeyMapMode || EditFlags.KeyMapMode ) {
        return( ERR_INPUT_KEYMAP_RUNNING );
    }
    CurrentKeyMap = InputKeyMaps[ num ].data;
    CurrentKeyMapCount = 0;
    EditFlags.InputKeyMapMode = TRUE;
    currUndoStack = UndoStack;
    StartUndoGroup( currUndoStack );
    if( InputKeyMaps[num].no_input_window ) {
        EditFlags.NoInputWindow = TRUE;
    }
    return( ERR_NO_ERR );

} /* StartInputKeyMap */

/*
 * DoneInputKeyMap - finished with undo stack
 */
void DoneInputKeyMap( void  )
{
    EditFlags.InputKeyMapMode = FALSE;
    TryEndUndoGroup( currUndoStack );

} /* DoneInputKeyMap */

/*
 * extractViKeyToken - extract the character token from a data string,
 *                    assumes we are pointing at <CHAR>
 */
vi_key extractViKeyToken( char *data, int *off )
{
    char        str[MAX_STR];
    int         i,j;
    int         rc;
    vi_key      c;

    i = *off;

    j = 0;
    while( 1 ) {
        i++;
        c = data[i];
        if( c == '>' || c == 0 ) {
            str[j] = 0;
            break;
        }
        str[j] = data[i];
        j++;
    }
    rc = readKeyData();
    if( rc ) {
        return( VI_KEY( ESC ) );
    }
    j = Tokenize( charTokens, str, TRUE );
    if( j < 0 ) {
        c = str[0];
    } else {
        c = charVals[j];
    }
    *off = i;
    return( c );

} /* extractViKeyToken */

/*
 * AddKeyMap - add a specified key mapping
 */
int AddKeyMap( key_map *scr, char *data, int len )
{
    int         i;
    char        c;
    vi_key      *sdata;

    /*
     * get storage for key map
     */
    scr->data = MemAlloc( ( len + 1 ) * sizeof( vi_key ) );
    scr->inuse = FALSE;

    /*
     * copy in key map data
     */
    sdata = scr->data;
    for( i=0;i<len;i++ ) {
        c = data[i];
        if( c == '\\' ) {
            i++;
            c = data[i];
            switch( c ) {
            case 0:
                return( ERR_INVALID_MAP );
            case '<':
                *sdata = extractViKeyToken( data, &i );
                break;
            case 'h':
                *sdata = NO_ADD_TO_HISTORY_KEY;
                break;
            case 'x':
                scr->no_input_window = TRUE;
                sdata--;
                break;
            case 'e':
                *sdata = VI_KEY( ESC );
                break;
            case 'n':
                *sdata = VI_KEY( ENTER );
                break;
            case 't':
                *sdata = VI_KEY( TAB );
                break;
            default:
                if( EditFlags.CompileScript ) {
                    *sdata = '\\';
                    sdata++;
                }
                *sdata = c;
                break;
            }
        } else {
            *sdata = c;
        }
        sdata++;
    }
    *sdata = 0;

    return( ERR_NO_ERR );

} /* AddKeyMap */

/*
 * InitKeyMaps - allocate key mapping arrays
 */
void InitKeyMaps( void )
{

    KeyMaps = MemAlloc( EventCount * sizeof( key_map ) );
    InputKeyMaps = MemAlloc( EventCount * sizeof( key_map ) );

} /* InitKeyMaps */

/*
 * InitKeyMaps - allocate key mapping arrays
 */
void FiniKeyMaps( void )
{
    int i;

    MemFree( charVals );
    MemFree( charTokens );

    // assuming Keymaps and InputKeymaps are inited to 0
    // this should be OK
    for( i = 0; i<EventCount; i++){
        if( KeyMaps[i].data != NULL ) MemFree( KeyMaps[i].data );
        if( InputKeyMaps[i].data != NULL ) MemFree( InputKeyMaps[i].data );
    }
    MemFree( KeyMaps );
    MemFree( InputKeyMaps );
} /* InitKeyMaps */

/*
 * ExecuteBuffer - run a key mapping from a named buffer
 */
int ExecuteBuffer( void )
{
    int         rc;
    char        *data;
    key_map     scr;

    if( rc = ModificationTest() ) {
        return( rc );
    }
    rc = GetSavebufString( &data );
    if( rc ) {
        return( rc );
    }
    rc = AddKeyMap( &scr, data, strlen( data ) );
    if( rc ) {
        return( rc );
    }
    rc = RunKeyMap( &scr, 1L );
    MemFree( scr.data );
    return( rc );

} /* ExecuteBuffer */

/*
 * LookUpCharToken - look up to token for a specified character
 */
char *LookUpCharToken( int ch, bool want_single )
{
    int         i;
    static int  num=0;

    if( num == 0 ) {
        if( readKeyData() != ERR_NO_ERR ) {
            return( NULL );
        }
        num = GetNumberOfTokens( charTokens );
    }
    if( want_single ) {
        switch( ch ) {
        case VI_KEY( ESC ):
            return( "e" );
        case VI_KEY( ENTER ):
            return( "n" );
        case VI_KEY( TAB ):
            return( "t" );
        case NO_ADD_TO_HISTORY_KEY:
            return( "h" );
        case '\\':
            return( "\\" );
        }
    }
    for( i=0;i<num;i++ ) {
        if( ch == charVals[i] ) {
            return( GetTokenString( charTokens, i ) );
        }
    }
    return( NULL );

} /* LookUpCharToken */
