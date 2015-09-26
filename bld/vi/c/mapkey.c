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

static bool     keysRead = false;
static char     *charTokens;
static vi_key   *keyVals;


static bool key_alloc( int cnt )
{
    keyVals = MemAlloc( cnt * sizeof( vi_key ) );
    return( true );
}

static bool key_save( int i, const char *buff )
{
    keyVals[i] = atoi( buff );
    return( true );
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
#ifdef VICOMP
    rc = ReadDataFile( "keys.dat", &charTokens, key_alloc, key_save );
#else
    rc = ReadDataFile( "keys.dat", &charTokens, key_alloc, key_save, true );
#endif
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    keysRead = true;
    return( ERR_NO_ERR );

} /* readKeyData */

/*
 * MapKey - set up a key mapping
 */
vi_rc MapKey( int flag, const char *data )
{
    char        keystr[MAX_STR];
#ifndef VICOMP
    key_map     *maps;
#endif
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

    /*
     * get if it is an input/regular key mapping
     */
    if( flag & MAPFLAG_DAMMIT ) {
        maps = InputKeyMaps;
    } else {
        maps = KeyMaps;
    }
#endif
    data = GetNextWord1( data, keystr );
    if( *keystr == '\0' ) {
        return( ERR_INVALID_MAP );
    }
    data = SkipLeadingSpaces( data );

    /*
     * get key we are using
     */
#ifndef VICOMP
    if( !EditFlags.ScriptIsCompiled || (flag & MAPFLAG_UNMAP) ) {
#endif
        j = Tokenize( charTokens, keystr, true );
        if( j == TOK_INVALID ) {
            key = C2VIKEY( keystr[0] );
        } else {
            key = keyVals[j];
        }
        if( key >= MAX_EVENTS ) {
            return( ERR_INVALID_KEY );
        }
#ifndef VICOMP
    } else {
        long    keyl;

        keyl = atol( keystr );
        if( keyl < 0 || keyl >= MAX_EVENTS ) {
            return( ERR_INVALID_KEY );
        }
        key = (vi_key)keyl;
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

    maps[key].inuse = false;
    maps[key].is_base = false;
    MemFree( maps[key].data );
    maps[key].data = NULL;
    if( !(flag & MAPFLAG_UNMAP ) ) {
        if( flag & MAPFLAG_BASE ) {
            maps[key].is_base = true;
        }
        return( AddKeyMap( &maps[key], data ) );
    }
    return( ERR_NO_ERR );
#endif

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
    bool        was_base = false;

    KeyMaps[key].inuse = true;
    if( KeyMaps[key].is_base ) {
        was_base = true;
        for( i = 0; i < MAX_EVENTS; i++ ) {
            KeyMaps[i].was_inuse = KeyMaps[i].inuse;
            KeyMaps[i].inuse = true;
        }
    }
    total = GetRepeatCount();

    rc = RunKeyMap( &KeyMaps[key], total );

    if( was_base ) {
        for( i = 0; i < MAX_EVENTS; i++ ) {
            KeyMaps[i].inuse = KeyMaps[i].was_inuse;
        }
    }
    KeyMaps[key].inuse = false;

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
    for( ; total > 0; --total ) {
        /*
         * set up key map to run
         */
        CurrentKeyMap = scr->data;
        if( scr->no_input_window ) {
            EditFlags.NoInputWindow = true;
        }
        // max = strlen( CurrentKeyMap );
        for( max = 0; CurrentKeyMap[max] != VI_KEY( NULL ); max++ )
            ;

        EditFlags.KeyMapInProgress = true;
        EditFlags.KeyMapMode = true;
        LastError = ERR_NO_ERR;

        /*
         * run until done
         */
        for( CurrentKeyMapCount = 0; CurrentKeyMapCount < max; ) {
            LastEvent = GetNextEvent( false );
            rc = DoLastEvent();
            if( rc > ERR_NO_ERR || LastError != ERR_NO_ERR ) {
                break;
            }
            DoneLastEvent( rc, true );
        }

        EditFlags.KeyMapMode = false;
        EditFlags.NoInputWindow = false;
        EditFlags.KeyMapInProgress = false;
        if( rc > ERR_NO_ERR || LastError != ERR_NO_ERR ) {
            break;
        }
    }

    TryEndUndoGroup( cstack );
    EditFlags.Dotable = false;
    return( rc );

} /* doRunKeyMap */

/*
 * RunKeyMap - run a key mapping
 */
vi_rc RunKeyMap( key_map *scr, long total )
{
    int         oldcount = 0;
    bool        restore = false;
    vi_key      *oldmap = NULL;
    vi_rc       rc;

    /*
     * check if we are already running a key mapping; if so, save it
     */
    if( EditFlags.KeyMapInProgress ) {
        restore = true;
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
        EditFlags.KeyMapInProgress = true;
        EditFlags.KeyMapMode = true;
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
    EditFlags.InputKeyMapMode = true;
    currUndoStack = UndoStack;
    StartUndoGroup( currUndoStack );
    if( InputKeyMaps[key].no_input_window ) {
        EditFlags.NoInputWindow = true;
    }
    return( ERR_NO_ERR );

} /* StartInputKeyMap */

/*
 * DoneInputKeyMap - finished with undo stack
 */
void DoneInputKeyMap( void )
{
    EditFlags.InputKeyMapMode = false;
    TryEndUndoGroup( currUndoStack );

} /* DoneInputKeyMap */
#endif /* VICOMP */

/*
 * extractViKeyToken - extract the character token from a data string,
 *                    assumes we are pointing at <CHAR>
 */
static vi_key extractViKeyToken( const char **data )
{
    char            str[MAX_STR];
    int             j;
    vi_rc           rc;
    char            c;

    for( (*data)++, j = 0; (c = **data) != '\0'; ++j ) {
        if( c == '>' )
            break;
        str[j] = c;
        (*data)++;
    }
    str[j] = '\0';
    rc = readKeyData();
    if( rc != ERR_NO_ERR ) {
        return( VI_KEY( ESC ) );
    }
    j = Tokenize( charTokens, str, true );
    if( j == TOK_INVALID ) {
        return( C2VIKEY( str[0] ) );
    } else {
        return( keyVals[j] );
    }

} /* extractViKeyToken */

/*
 * AddKeyMap - add a specified key mapping
 */
vi_rc AddKeyMap( key_map *scr, const char *data )
{
    char            c;
    vi_key          *sdata;
    int             len;

    /*
     * get storage for key map
     */
    len = strlen( data );
    scr->data = MemAlloc( (len + 1) * sizeof( vi_key ) );
    scr->inuse = false;

    /*
     * copy in key map data
     */
    for( sdata = scr->data; (c = *data) != '\0'; ++data, ++sdata ) {
        if( c == '\\' ) {
            ++data;
            c = *data;
            switch( c ) {
            case '\0':
                return( ERR_INVALID_MAP );
            case '<':
                *sdata = extractViKeyToken( &data );
                break;
            case 'h':
                *sdata = NO_ADD_TO_HISTORY_KEY;
                break;
            case 'x':
                scr->no_input_window = true;
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
                    *sdata++ = VI_KEY( BACKSLASH );
#ifndef VICOMP
                }
#endif
                *sdata = C2VIKEY( c );
                break;
            }
        } else {
            *sdata = C2VIKEY( c );
        }
    }
    *sdata = VI_KEY( NULL );

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

    rc = ModificationTest();
    if( rc != ERR_NO_ERR ) {
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
        case VI_KEY( BACKSLASH ):
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
