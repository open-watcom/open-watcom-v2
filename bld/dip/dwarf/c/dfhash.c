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
* Description:  DWARF hash table utility functions.
*
****************************************************************************/


#include "dfdip.h"
#include <ctype.h>
#include "dfhash.h"


enum {
    NAME_BLKSIZE = 128,
    NAME_BUCKETS = 256,
    NAME_STRBLK  = 1024 * 4
} name_defs;

typedef struct {
    dr_handle   sym;            // DWARF handle
    uint_32     key;            // hash key
    char        *name;          // name from pubnames
} name_entry;

typedef struct str_blk  str_blk;
struct str_blk {
    str_blk     *next;          // next blk  in chain
    char        strs[1];        // strings
};

typedef struct str_ctl  str_ctl;
struct str_ctl {
    str_blk     *blks;          // stack of blocks
    int         rem;            // amount left in blk
    char        *strs;          // pointer in
};


static void StrInit( str_ctl *ctl )
/*********************************/
{
    ctl->rem = 0;
    ctl->blks = NULL;
    ctl->strs = NULL;
}


static void StrFini( str_ctl *ctl )
/*********************************/
{
    str_blk     *blks;

    blks = ctl->blks;
    while( blks != NULL ) {
        str_blk *curr;

        curr = blks;
        blks = blks->next;
        DCFree( curr );
    }
    ctl->rem = 0;
    ctl->blks = NULL;
    ctl->strs = NULL;
}


static char *StrAlloc( str_ctl *ctl, int len )
/********************************************/
{
    char    *ptr;

    if( ctl->rem < len ) {
        str_blk     *new;
        int         size;

        size =  NAME_STRBLK;
        if( size < len ) {
            size += len;
        }

        new = DCAlloc( sizeof( str_blk )+size );
        new->next = ctl->blks;
        ctl->blks = new;
        ctl->rem  = size;
        ctl->strs = new->strs;
    }
    ptr = ctl->strs;
    ctl->strs += len;
    ctl->rem -= len;
    return( ptr );
}


typedef struct name_blk name_blk;
struct name_blk {
    name_blk    *next;                  // next blk  in chain
    name_entry  entry[NAME_BLKSIZE];    // entries
};

struct name_ctl {
    int         count;
    str_ctl     str_pool;
    struct {
        name_blk    *head;      // start of chain
        uint_16     rem;        // num entries left in head (rest assume filled)
    } bucket[NAME_BUCKETS];
};


// hashpjw out of dragon book
static uint_32 elf_hash( char const *name )
/*****************************************/
{
    uint_32     h;
    uint_32     g;

    h = 0;
    while( *name != '\0' ) {
        h = (h << 4 ) + tolower( *name );
        ++name;
        if( (g = h & 0xf000000) != 0 ) {
            h ^= g >> 24;
        }
        h &= ~g;
    }
    return( h );
}

#define BNUM( key )  ((key >> 4) % NAME_BUCKETS)

extern name_ctl *InitHashName( void )
/***********************************/
{
    int         bnum;
    name_ctl    *ctl;

    ctl = DCAlloc( sizeof( name_ctl ) );
    ctl->count = 0;
    StrInit( &ctl->str_pool );
    for( bnum = 0; bnum < NAME_BUCKETS; ++bnum ) {
        ctl->bucket[bnum].head = NULL;
        ctl->bucket[bnum].rem = 0;
    }
    return( ctl );
}


extern void FiniHashName( name_ctl *ctl )
/***************************************/
// Kill hash table
{
    int     bnum;
    int     vacant, max, size;

    vacant= 0;  max = 0;
    for( bnum = 0; bnum < NAME_BUCKETS; ++bnum ) {
        name_blk    *blk;
        name_blk    *next;

        blk = ctl->bucket[bnum].head;
        if( ctl->bucket[bnum].rem == 0 ) {
            ++vacant;
        } else {
            size =  NAME_BLKSIZE-ctl->bucket[bnum].rem;
            if( size > max ) {
                max = size;
            }
        }
        while( blk != NULL ) {
            next = blk->next;
            DCFree( blk );
            blk = next;
        }
    }
    StrFini( &ctl->str_pool );
    DCFree( ctl );
}


extern void AddHashName( name_ctl *ctl, char *name, dr_handle sym )
/*****************************************************************/
// Add name, sym to foray
{
    uint_32     key;
    int         bnum;
    uint_16     rem;
    name_blk    *blk;
    name_entry  *curr;
    int         len;

    len = strlen( name );
    key = elf_hash( name );
    bnum = BNUM( key );
    rem = ctl->bucket[bnum].rem;
    if( rem == 0 ) {
        blk = DCAlloc( sizeof( *blk ) );
        blk->next = ctl->bucket[bnum].head;
        ctl->bucket[bnum].head = blk;
        rem = NAME_BLKSIZE;
    } else {
        blk = ctl->bucket[bnum].head;
    }
    curr = &blk->entry[NAME_BLKSIZE - rem];
    ctl->bucket[bnum].rem = rem-1;
    ++ctl->count;
    curr->sym = sym;
    curr->key = key;
    curr->name = StrAlloc( &ctl->str_pool, len + 1 );
    strcpy( curr->name, name );
}


extern int FindHashWalk( name_ctl *ctl, name_wlk *wlk )
/*****************************************************/
// Walk all entries that hash to same key
{
    uint_32     key;
    int         bnum;
    uint_16     count;
    name_blk    *blk;
//    int         ret;

    key = elf_hash( wlk->name );
    bnum = BNUM( key );
    count = NAME_BLKSIZE-ctl->bucket[bnum].rem;
    blk = ctl->bucket[bnum].head;
//    ret = TRUE;
    while( blk != NULL ) {
        name_entry  *curr;

        curr = &blk->entry[0];
        while( count > 0 ) {
            if( curr->key == key ) {
                if( !wlk->fn( wlk->d, curr->sym, curr->name ) ) {
                    return( FALSE );
                }
            }
            ++curr;
            --count;
        }
        blk = blk->next;
        count = NAME_BLKSIZE;
    }
    return( TRUE );
}
