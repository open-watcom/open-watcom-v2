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
* Description:  Hash function used by make utility.
*
****************************************************************************/


#include "make.h"
#include "mmemory.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mhash.h"

#include "clibext.h"


HASH_T Hash( const char *s, HASH_T prime )
/************************************************
 * Hash is modelled on hashpjw() function, from the book "Compilers:
 * Principles, Techniques, and Tools ", A.V.Aho, R.Sethi, J.D.Ullman, pg 437
 * Basically just a 16-bit version of that 32-bit function.
 */
{
    const UINT16    *p;
    HASH_T          h;
    UINT16          w;

    h = 0;
    p = (const void *)s;
#if 0
    /* I compared this hash (pjw) with the simple hash that follows using
       wfc386, watfor77, codegen, and nethack.  In all cases the simple hash
       was faster (no suprise) and had approx. the same number of collisions.
       So, I hereby retire hashpjw until a sunnier day... 07-jul-91 DJG */
    for( ;; ) {
        HASH_T  g;

        w = *p;
        ++p;
        if( (UINT8)(w & 0xff) == NULLCHAR ) {
            break;
        }
        w |= ' ';           /* convert to upper case */
        h = ( h << 2 ) + (UINT8)( w & 0xff );
        g = h & 0xf000;
        if( g != 0U ) {
            h ^= g;
            h ^= g >> 12;
        }
        if( (UINT8)(w >> 8) == NULLCHAR ) {
            break;
        }
        w |= (' ' << 8);    /* convert to upper case */
        h = (h << 2) + (UINT8)(w >> 8);
        g = h & 0xf000;
        if( g != 0U ) {
            h ^= g;
            h ^= g >> 12;
        }
    }
#else
    for( ;; ) {
        w = *p;
#ifdef __BIG_ENDIAN__
        /* we have to swap the bytes! */
        w = ((w & 0xff) << 8) + (w >> 8);
#endif
        ++p;
        if( (w & 0xff) == NULLCHAR ) {
            break;
        }
        h += w | ( (' '<< 8) | ' ');    /* convert to upper case */
        if( (UINT8)(w >> 8) == NULLCHAR ) {
            break;
        }
    }
#endif
    return( h % prime );
}


HASHTAB *NewHashTab( HASH_T prime )
/*****************************************
 * allocate a new table, with prime entries
 */
{
    HASHTAB     *tab;

    tab = CallocSafe( sizeof( *tab ) + prime * sizeof( HASHNODE ) );
    tab->prime = prime;

    return( tab );
}


void AddHashNode( HASHTAB *tab, HASHNODE *node )
/******************************************************
 * add a node to a table in proper hash order
 */
{
    HASH_T      h;

    assert( tab != NULL && node != NULL );

    h = Hash( node->name, tab->prime );

    node->next = tab->nodes[h];
    tab->nodes[h] = node;
}


BOOLEAN WalkHashTab( HASHTAB *tab,
    BOOLEAN (*func)( void *node, void *ptr ), void *ptr )
/********************************************************
 * walk a table applying func to each node.  If func returns TRUE,
 * this function aborts, returning TRUE.  func can do whatever it
 * wants to node, since this routine saves its next state before
 * calling func.  ptr is passed to func without modification.
 */
{
    HASH_T      h;
    HASHNODE    *walk;
    HASHNODE    *cur;

    assert( tab != NULL && func != NULL );

    h = 0;
    while( h < tab->prime ) {
        walk = tab->nodes[h];
        while( walk != NULL ) {
            cur = walk;
            walk = walk->next;
            if( func( cur, ptr ) ) {
                return( TRUE );
            }
        }
        ++h;
    }
    return( FALSE );
}


void FreeHashTab( HASHTAB *tab )
/**************************************
 * Deallocate a HASHTAB structure.  It does NOT do anything
 * with any existing nodes.
 */
{
    FreeSafe( tab );
}


HASHNODE *FindHashNode( HASHTAB *tab, const char *name,
    BOOLEAN caseSensitive )
/*************************************************************
 * Find node named name
 */
{
    HASH_T      h;
    HASHNODE    *cur;
    int         (*cmp)( const char *s1, const char *s2 );

    cmp = ( caseSensitive ) ? strcmp : stricmp;
    h = Hash( name, tab->prime );
    
    for( cur = tab->nodes[h]; cur != NULL; cur = cur->next ) {
        if( cmp( cur->name, name ) == 0 ) {
            return( cur );
        }
    }
    return( NULL );
}


HASHNODE *RemHashNode( HASHTAB *tab, const char *name,
                              BOOLEAN caseSensitive)
/************************************************************
 * unlink a node named name, and return pointer to unlinked node.
 * return NULL if node doesn't exist.
 */
{
    HASH_T      h;
    HASHNODE    **cur;
    HASHNODE    *old;
    int         (*cmp)( const char *s1, const char *s2 );

    cmp = ( caseSensitive ) ? strcmp : stricmp;

    h = Hash( name, tab->prime );

    for( cur = &(tab->nodes[h]); *cur != NULL; cur = &(*cur)->next ) {
        if( cmp( (*cur)->name, name ) == 0 ) {
            break;
        }
    }

    if( *cur == NULL ) {
        return( NULL );     /* not found */
    }

    old = *cur;
    *cur = (*cur)->next;    /* delink from list */

    return( old );
}
