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
#include "win.h"

static bool     keysRead = FALSE;
static char     *charTokens;
static vi_key   *keyVals;


static bool key_alloc( int cnt )
{
    keyVals = MemAlloc( cnt * sizeof( vi_key ) );
    return( TRUE );
}

static bool key_save( int i, char *buff )
{
    keyVals[i] = atoi( buff );
    return( TRUE );
}

/*
 * readKeyData - do just that
 */
static vi_rc readKeyData( void )
{
    vi_rc       rc;

    if( keysRead ) {
        return( ERR_NO_ERR );
    }
    rc = ReadDataFile( "keys.dat", &charTokens, key_alloc, key_save );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    keysRead = TRUE;
    return( ERR_NO_ERR );

} /* readKeyData */

/*
 * MapKey - set up a key mapping
 */
vi_rc MapKey( int flag, char *data )
{
    char        keystr[MAX_STR];
    key_map     *maps;
    int         j;
    vi_key      key;
    vi_rc       rc;

#ifndef VICOMP
    if( !EditFlags.ScriptIsCompiled || (flag & MAPFLAG_UNMAP) ) {
#endif
        rc = readKeyData();
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
#ifndef VICOMP
    }
#endif

    /*
     * get if it is an input/regular key mapping
     */
    if( flag & MAPFLAG_DAMMIT ) {
        maps = InputKeyMaps;
    } else {
        maps = KeyMaps;
    }

    if( NextWord1( data, keystr ) <= 0 ) {
        return( ERR_INVALID_MAP );
    }
    RemoveLeadingSpaces( data );

    /*
     * get key we are using
     */
#ifndef VICOMP
    if( !EditFlags.ScriptIsCompiled || (flag & MAPFLAG_UNMAP) ) {
#endif
        j = Tokenize( charTokens, keystr, TRUE );
        if( j < 0 ) {
            key = (unsigned char)keystr[0];
        } else {
            key = keyVals[j];
        }
#ifndef VICOMP
    } else {
        key = atoi( keystr );
    }
#endif

#ifndef VICOMP
    if( EditFlags.CompileScript ) {
#endif
        if( !(flag & MAPFLAG_UNMAP) ) {
            key_map     scr;

            rc = AddKeyMap( &scr, data );
            if( rc == ERR_NO_ERR ) {
                if( scr.no_input_window ) {
                    MySprintf( WorkLine->data, "%d \\x%s", key, scr.data );
                } else {
                    MySprintf( WorkLine->data, "%d %s", key, scr.data );
                }
            }
            MemFree( scr.data );
            return( rc );

        } else {
            MySprintf( WorkLine->data, "%d %s", key, data );
            return( ERR_NO_ERR );
        }
#ifndef VICOMP
    }
#endif

    if( key < 0 || key >= MAX_EVENTS ) {
        return( ERR_INVALID_KEY );
    }
    maps[key].inuse = FALSE;
    maps[key].is_base = FALSE;
    MemFree( maps[key].data );
    maps[key].data = NULL;
    if( !(flag & MAPFLAG_UNMAP ) ) {
        if( flag & MAPFLAG_BASE ) {
            maps[key].is_base = TRUE;
        }
        return( AddKeyMap( &maps[key], data ) );
    }
    return( ERR_NO_ERR );

} /* MapKey */

#ifndef VICOMP
/*
 * DoKeyMap - process a key mapping
 */
vi_rc DoKeyMap( vi_key key )
{
    vi_rc       rc;
    long        total;
    int         i;
    bool        was_base = FALSE;

    KeyMaps[key].inuse = TRUE;
    if( KeyMaps[key].is_base ) {
        was_base = TRUE;
        for( i = 0; i < MAX_EVENTS; i++ ) {
            KeyMaps[i].was_inuse = KeyMaps[i].inuse;
            KeyMaps[i].inuse = TRUE;
        }
    }
    total = GetRepeatCount();

    rc = RunKeyMap( &KeyMaps[key], total );

    if( was_base ) {
        for( i = 0; i < MAX_EVENTS; i++ ) {
            KeyMaps[i].inuse = KeyMaps[i].was_inuse;
        }
    }
    KeyMaps[key].inuse = FALSE;

    return( rc );

} /* DoKeyMap */

/*
 * doRunKeyMap - execute a key map a specified number of times
 */
static vi_rc doRunKeyMap( key_map *scr, long total )
{
    int         max;
    vi_rc       rc = ERR_NO_ERR;
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
        for( max = 0; CurrentKeyMap[max] != 0; max++ );

        EditFlags.KeyMapInProgress = TRUE;
        EditFlags.KeyMapMode = TRUE;
        LastError = ERR_NO_ERR;

        /*
         * run until done
         */
        for( CurrentKeyMapCount = 0; CurrentKeyMapCount < max; ) {
            LastEvent = GetNextEvent( FALSE );
            rc = DoLastEvent();
            if( rc > ERR_NO_ERR || LastError != ERR_NO_ERR ) {
                break;
            }
            DoneLastEvent( rc, TRUE );
        }

        EditFlags.KeyMapMode = FALSE;
        EditFlags.NoInputWindow = FALSE;
        EditFlags.KeyMapInProgress = FALSE;
        if( rc > ERR_NO_ERR || LastError != ERR_NO_ERR ) {
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
vi_rc RunKeyMap( key_map *scr, long total )
{
    int         oldcount;
    bool        restore = FALSE;
    vi_key      *oldmap;
    vi_rc       rc;

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
vi_rc StartInputKeyMap( vi_key key )
{
    if( EditFlags.InputKeyMapMode || EditFlags.KeyMapMode ) {
        return( ERR_INPUT_KEYMAP_RUNNING );
    }
    CurrentKeyMap = InputKeyMaps[key].data;
    CurrentKeyMapCount = 0;
    EditFlags.InputKeyMapMode = TRUE;
    currUndoStack = UndoStack;
    StartUndoGroup( currUndoStack );
    if( InputKeyMaps[key].no_input_window ) {
        EditFlags.NoInputWindow = TRUE;
    }
    return( ERR_NO_ERR );

} /* StartInputKeyMap */

/*
 * DoneInputKeyMap - finished with undo stack
 */
void DoneInputKeyMap( void )
{
    EditFlags.InputKeyMapMode = FALSE;
    TryEndUndoGroup( currUndoStack );

} /* DoneInputKeyMap */
#endif /* VICOMP */

/*
 * extractViKeyToken - extract the character token from a data string,
 *                    assumes we are pointing at <CHAR>
 */
vi_key extractViKeyToken( unsigned char **p )
{
    char            str[MAX_STR];
    int             j;
    vi_rc           rc;
    int             c;

    
    for( j = 0; (c = **p) != '\0'; ++j ) {
        (*p)++;
        if( c == '>' )
            break;
        str[j] = c;
    }
    str[j] = '\0';
    rc = readKeyData();
    if( rc != ERR_NO_ERR ) {
        return( VI_KEY( ESC ) );
    }
    j = Tokenize( charTokens, str, TRUE );
    if( j < 0 ) {
        return( (unsigned char)str[0] );
    } else {
        return( keyVals[j] );
    }

} /* extractViKeyToken */

/*
 * AddKeyMap - add a specified key mapping
 */
vi_rc AddKeyMap( key_map *scr, char *data )
{
    int             c;
    vi_key          *sdata;
    int             len;
    unsigned char   *p;

    p = (unsigned char *)data;
    /*
     * get storage for key map
     */
    len = strlen( data );
    scr->data = MemAlloc( (len + 1) * sizeof( vi_key ) );
    scr->inuse = FALSE;

    /*
     * copy in key map data
     */
    sdata = scr->data;
    while( *p != '\0' ) {
        c = *p++;
        if( c == '\\' ) {
            c = *p++;
            switch( c ) {
            case 0:
                return( ERR_INVALID_MAP );
            case '<':
                *sdata = extractViKeyToken( &p );
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
#ifndef VICOMP
                if( EditFlags.CompileScript ) {
#endif
                    *sdata = '\\';
                    sdata++;
#ifndef VICOMP
                }
#endif
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

#ifndef VICOMP
/*
 * InitKeyMaps - allocate key mapping arrays
 */
void InitKeyMaps( void )
{
    KeyMaps = MemAlloc( MAX_EVENTS * sizeof( key_map ) );
    InputKeyMaps = MemAlloc( MAX_EVENTS * sizeof( key_map ) );

} /* InitKeyMaps */

/*
 * InitKeyMaps - allocate key mapping arrays
 */
void FiniKeyMaps( void )
{
    int i;

    MemFree( keyVals );
    MemFree( charTokens );

    // assuming Keymaps and InputKeymaps are inited to 0
    // this should be OK
    for( i = 0; i < MAX_EVENTS; i++){
        if( KeyMaps[i].data != NULL ) {
            MemFree( KeyMaps[i].data );
        }
        if( InputKeyMaps[i].data != NULL ) {
            MemFree( InputKeyMaps[i].data );
        }
    }
    MemFree( KeyMaps );
    MemFree( InputKeyMaps );

} /* InitKeyMaps */

/*
 * ExecuteBuffer - run a key mapping from a named buffer
 */
vi_rc ExecuteBuffer( void )
{
    vi_rc       rc;
    char        *data;
    key_map     scr;

    if( (rc = ModificationTest()) != ERR_NO_ERR ) {
        return( rc );
    }
    rc = GetSavebufString( &data );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    rc = AddKeyMap( &scr, data );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    rc = RunKeyMap( &scr, 1L );
    MemFree( scr.data );
    return( rc );

} /* ExecuteBuffer */

/*
 * LookUpCharToken - look up to token for a specified character
 */
char *LookUpCharToken( vi_key key, bool want_single )
{
    int         i;
    static int  num = 0;

    if( num == 0 ) {
        if( readKeyData() != ERR_NO_ERR ) {
            return( NULL );
        }
        num = GetNumberOfTokens( charTokens );
    }
    if( want_single ) {
        switch( key ) {
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
    for( i = 0; i < num; i++ ) {
        if( key == keyVals[i] ) {
            return( GetTokenString( charTokens, i ) );
        }
    }
    return( NULL );

} /* LookUpCharToken */
#endif /* VICOMP */
