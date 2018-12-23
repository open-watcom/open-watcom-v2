/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Trie implementation for UI lib.
*
****************************************************************************/


#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
#ifdef __QNX__
#include <sys/kernel.h>
#include <sys/dev.h>
#include <sys/sidinfo.h>
#include <sys/psinfo.h>
#include <sys/proxy.h>
#include <sys/qioctl.h>
#include <sys/console.h>
#endif
#include "walloca.h"
#include "uidef.h"
#include "uishift.h"
#include "uivirts.h"
#include "ctkeyb.h"
#include "uiextrn.h"
#include "trie.h"


/* The following types are for use with the keymap-trie. The keymap trie
 * is used to store all of the terminfo keypresses. The trie is fairly
 * compact, while also very fast. It also lends itself quite well to
 * key-lookup.
 */

#define EV_UNUSED               ((ui_event)-1)

#define TRIE_ARRAY_GROWTH       4

#define TRIE_TOP                16

typedef struct eTrie eTrie;

typedef struct eNode{
    eTrie       *trie;          // the child sub-trie
    char        c;              // the character associated with it
    ui_event    ui_ev;          // the event associated with the string
} eNode;

struct eTrie{
    eNode       *child;         // array of childeren
    int         num_child;      // number of children in array
    int         alc_child;      // allocated size of array
};

int             KeyTrieDepth = 1;

static eTrie    KeyTrie;

bool TrieInit( void )
{

    KeyTrie.child = uimalloc( TRIE_TOP * sizeof( eNode ) );
    if( KeyTrie.child == NULL )
        return( false );
    KeyTrie.alc_child = TRIE_TOP;
    KeyTrie.num_child = 0;
    return( true );
}


static void free_subtrie( eTrie *trie )
{
    int         i;

    if( trie != NULL ) {
        for( i = 0; i < trie->num_child; i++ ) {
            free_subtrie( trie->child[i].trie );
        }
        uifree( trie->child );
        uifree( trie );
    }
}

/* frees the trie.
 */
void TrieFini( void )
{
    int         i;

    for( i = 0; i < KeyTrie.num_child; i++ ) {
        free_subtrie( KeyTrie.child[i].trie );
    }
    uifree( KeyTrie.child );
}

/* Search for children. Will return position
 * where child *should* be (this is why bsearch isn't good enough).
 * Currently only linear, but seems to be good enough. Note that
 * this is only used in initialization. Bsearch *is* used during
 * actual key parsing.
 */
static int child_search( char key, eTrie *trie )
{
    int         num;
    int         x;
    eNode       *ary;

    num = trie->num_child;
    if( num < 1 )
        return( 0 );

    ary = trie->child;
    if( key>ary[num - 1].c )
        return( num );

    for( x = 0; x < num; x++ ) {
        if( ary[x].c >= key ) {
            return( x );
        }
    }

    return( num );
}

/* This function will add a string and matching event to KeyTrie. It adds
 * with the trie arrays sorted using linear insertion. It may be possible
 * to improve the performance, but I think any improvements would be slight,
 * as this function is only called at initialization.
 */
bool TrieAdd( ui_event ui_ev, const char *str )
{
    eTrie       *trie = &KeyTrie;
    int         i;
    int         depth = 1;

    if( str == NULL || *str == '\0' )
        return( true );

    for( ;; ) {
        i = child_search( *str, trie );

        if( i == trie->num_child || trie->child[i].c != *str ) {
            // the char's not in the list, so we'd better add it

            trie->num_child++;

            if( trie->alc_child < trie->num_child ) {
                //eNode                 *tmp;

                // the array isn't big enough, expand it a bit
                trie->alc_child += TRIE_ARRAY_GROWTH;
                trie->child = uirealloc( trie->child, trie->alc_child * sizeof( eNode ) );
                if( trie->child == NULL ) {
                    trie->alc_child = 0;
                    return( false );
                }
            }

            if( i < ( trie->num_child - 1 ) ) {
                // We're in the middle of the list, so clear a spot
                memmove( &(trie->child[i + 1]), &(trie->child[i]),
                                (trie->num_child-i-1)*sizeof(eNode) );
            }

            trie->child[i].c = *str;
            trie->child[i].ui_ev = EV_UNUSED;
            trie->child[i].trie = NULL;
        }

        // advance current char pointer
        str++;

        // by this point, "i" is set to the index of a matching sub-trie.
        if( *str == '\0' ) {
            // at the end of the string, so insert the event
            trie->child[i].ui_ev = ui_ev;
            return( true );
        }

        if( trie->child[i].trie == NULL ) {
            // our "matching sub-trie" does not yet exist...
            trie->child[i].trie = uimalloc( sizeof( eTrie ) );
            if( trie->child[i].trie == NULL ) {
                return( false );
            }
            trie->child[i].trie->child = NULL;
            trie->child[i].trie->num_child = 0;
            trie->child[i].trie->alc_child = 0;
        }

        // go down a level, and work on the next char
        trie = trie->child[i].trie;
        depth++;
        if( depth > KeyTrieDepth ) {
            KeyTrieDepth = depth;
        }
    }
}

static int child_comp( const int *pkey, const eNode *pbase )
{
    return( *pkey - pbase->c );
}

ui_event TrieRead( void )
{
    eTrie           *trie;
    char            *buf;
    int             c;
    size_t          cpos = 0;
    ui_event        ui_ev = EV_UNUSED;
    size_t          ui_ev_pos = 0;
    eNode           *node;
    int             timeout;

    buf = walloca( KeyTrieDepth + 1 );

    trie = &KeyTrie;
    buf[0] = '\0';
    timeout = 0;
    for( ;; ) {
        c = nextc(timeout);
        if( c <= 0 )
            break;
#ifdef __UNIX__
        if( c == 256 )
            return( EV_MOUSE_PRESS );
#endif
        buf[cpos++] = c;

        if( trie->num_child == 0 )
            break;
        node = bsearch( &c, trie->child, trie->num_child, sizeof( eNode ),
                            (int (*)(const void *, const void *))child_comp );
        if( node == NULL )
            break;
        if( node->ui_ev != EV_UNUSED ) {
            ui_ev = node->ui_ev;
            ui_ev_pos = cpos;
        }
        trie = node->trie;
        if( trie == NULL )
            break;
        timeout = 3;
    }
    if( ui_ev == EV_UNUSED ) {
        ui_ev = (unsigned char)buf[0];
        ui_ev_pos = 1;
    }

    // when we get down here cpos will be the number of chars in buf
    // note that the nul-char is not considered to actually be there
    // (the nul is sent on time-outs, and is guaranteed to never appear
    // in a terminfo keysequence as they are all nul-terminated.)

    if( cpos > ui_ev_pos ) {
        nextc_unget( &buf[ui_ev_pos], cpos - ui_ev_pos );
    }
    return( ui_ev );
}
