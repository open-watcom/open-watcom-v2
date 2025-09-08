/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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


/*
 *  OBJNODE:   "node" management routines
 *
 */

#include <string.h>
#include "linkstd.h"
#include "distrib.h"
#include "wcomdef.h"
#include "strtab.h"
#include "carve.h"
#include "permdata.h"
#include "objnode.h"


#define NODE_ARRAY_SIZE 256
#define MAX_NUM_NODES   128
#define NEED_NEW_ARRAY( x ) (((x) & 0xFF) == 0)
#define ARRAY_NUM( x )      ((x) >> 8 )
#define ELEMENT_NUM( x )    ((x) & 0xFF)

typedef struct nodearray {
    unsigned    num;            // number of nodes inserted
    unsigned    elsize;         // size of individual element in array.
    unsigned    arraymax;       // the number of node arrays allocated - 1
    char        *array[MAX_NUM_NODES];   // the array.
} nodearray;

nodearray_handle    ExtNodes;   // ptr to obj file import list
nodearray_handle    SegNodes;   // ptr to obj file segment list
nodearray_handle    GrpNodes;   // ptr to obj file group list
nodearray_handle    NameNodes;  // ptr to obj file lname list

static void *MakeArray( unsigned size )
/*************************************/
{
    nodearray_handle    nodes;

    _ChkAlloc( nodes, sizeof( nodearray ) );
    nodes->num = 0;
    nodes->elsize = size;
    nodes->arraymax = 0;
    size *= NODE_ARRAY_SIZE;
    _ChkAlloc( nodes->array[0], size );
    memset( nodes->array[0], 0, size );
    return( nodes );
}

void InitNodes( void )
/***************************/
// initialize the node array structures.
{
    GrpNodes = MakeArray( sizeof( grpnode ) );
    SegNodes = MakeArray( sizeof( segnode ) );
    ExtNodes = MakeArray( sizeof( extnode ) );
    NameNodes = MakeArray( sizeof( list_of_names * ) );
}

static void BurnNodeArray( nodearray_handle list )
/************************************************/
{
    unsigned    index;

    for( index = 0; index <= list->arraymax; index++ ) {
        _LnkFree( list->array[index] );
    }
    _LnkFree( list );
}

void BurnNodes( void )
/***************************/
{
    BurnNodeArray( GrpNodes );
    BurnNodeArray( SegNodes );
    BurnNodeArray( ExtNodes );
    BurnNodeArray( NameNodes );
}

void *FindNode( nodearray_handle list, unsigned index )
/*****************************************************/
{
    index--;            // index is base 1
    return( list->array[ARRAY_NUM( index )] + ELEMENT_NUM( index ) * list->elsize );
}

static void AllocNewArray( nodearray_handle list )
/************************************************/
{
    unsigned    size;

    list->arraymax++;
    size = list->elsize * NODE_ARRAY_SIZE;
    _ChkAlloc( list->array[list->arraymax], size );
    memset( list->array[list->arraymax], 0, size );
}

void *AllocNode( nodearray_handle list )
/**************************************/
{
    if( ARRAY_NUM( list->num ) > list->arraymax ) {
        AllocNewArray( list );
    }
    list->num++;
    return( FindNode( list, list->num ) );
}

void *AllocNodeIdx( nodearray_handle list, unsigned index )
/*********************************************************/
{
    if( list->num < index ) {
        list->num = index;
        while( ARRAY_NUM( index - 1 ) > list->arraymax ) {
            AllocNewArray( list );
        }
    }
    return( FindNode( list, index ) );
}

mod_entry *NewModEntry( void )
/****************************/
/* Allocate a new object file entry structure and initialize it */
{
    mod_entry           *entry;

    entry = CarveZeroAlloc( CarveModEntry );
#ifdef _EXE
    if( LinkFlags & LF_STRIP_CODE ) {
        InitArcList( entry );
    }
#endif
    return( entry );
}

void FreeModEntry( mod_entry *mod )
/****************************************/
{
    CarveFree( CarveModEntry, mod );
}

void FreeNodes( nodearray_handle nodes )
/**************************************/
{
    unsigned    index;

    for( index = 0; index <= nodes->arraymax; index++ ) {
        memset( nodes->array[index], 0, nodes->elsize * NODE_ARRAY_SIZE );
    }
    nodes->num = 0;
}

static void ReleaseNamelist( void *node, void *dummy )
/****************************************************/
{
    /* unused parameters */ (void)dummy;

    _LnkFree( *((void **)node) );
}

static void IterateNodeArray( char *narray, void (*fn)(void *, void *),
                              unsigned elsize, int limit, void *cookie )
/**********************************************************************/
{
    int index;

    for( index = 0; index < limit; index++ ) {
        fn( narray, cookie );
        narray += elsize;
    }
}

void IterateNodelist( nodearray_handle list, void (*fn)(void *, void *),
                             void *cookie )
/***********************************************************************/
{
    int index;
    int limit;

    if( list->num == 0 )
        return;
    limit = ARRAY_NUM( list->num );
    for( index = 0; index < limit; index++ ) {
        IterateNodeArray( list->array[index], fn, list->elsize, NODE_ARRAY_SIZE, cookie );
    }
    IterateNodeArray( list->array[limit], fn, list->elsize, ELEMENT_NUM( list->num ), cookie );
}

void ReleaseNames( void )
/******************************/
/* Free list of names. */
{
    IterateNodelist( NameNodes, ReleaseNamelist, NULL );
    FreeNodes( NameNodes );
}

static void CollapseLazy( void *node, void *dummy )
/*************************************************/
{
    extnode     *curr;

    /* unused parameters */ (void)dummy;

    curr = node;
    if( IS_SYM_A_REF( curr->entry ) && !curr->isweak ) {
        ClearSymUnion( curr->entry );
        SET_SYM_TYPE( curr->entry, SYM_REGULAR );
        if( LinkState & LS_SEARCHING_LIBRARIES ) {
            curr->entry->info &= ~SYM_CHECKED;
            LinkState |= LS_LIBRARIES_ADDED;    // force another pass thru libs
        }
    }
}

void CollapseLazyExtdefs( void )
/*************************************/
{
    IterateNodelist( ExtNodes, CollapseLazy, NULL );
}

static void *ListFindValue( char *node, unsigned limit, unsigned elsize,
                             void *target, bool (*compare_fn)(void *, void *) )
/*****************************************************************************/
{
    unsigned            index;

    for( index = 0; index < limit; index++ ) {
        if( compare_fn( node, target ) )
            return( node );
        node += elsize;
    }
    return( NULL );
}

static void *IterateFindValue( nodearray_handle list, void *target, bool (*fn)(void *, void *) )
/**********************************************************************************************/
{
    unsigned    index;
    void        *retval;
    unsigned    limit;

    if( list->num == 0 )
        return( NULL );
    limit = ARRAY_NUM( list->num );
    for( index = 0; index < limit; index++ ) {
        retval = ListFindValue( list->array[index], NODE_ARRAY_SIZE, list->elsize, target, fn );
        if( retval ) {
            return( retval );
        }
    }
    return( ListFindValue( list->array[limit], ELEMENT_NUM( list->num ), list->elsize, target, fn ) );
}

static bool DoesExtHandleMatch( void *curr, void *target )
/********************************************************/
{
    return( ((extnode *)curr)->handle == target );
}

extnode *FindExtHandle( orl_symbol_handle sym_handle )
/****************************************************/
{
    return( (extnode *)IterateFindValue( ExtNodes, (void *)sym_handle, DoesExtHandleMatch ) );
}

segdata *AllocSegData( void )
/***************************/
{
    segdata *sdata;

    sdata = CarveZeroAlloc( CarveSegData );
    sdata->o.mod = CurrMod;
    return( sdata );
}

void FreeSegData( void *sdata )
/*****************************/
/* put a segdata on the list of free segdatas */
{
    CarveFree( CarveSegData, sdata );
}

list_of_names *MakeListName( char *name, size_t len )
/***************************************************/
{
    list_of_names   *new;

    _ChkAlloc( new, sizeof( list_of_names ) + len );
    new->next_name = NULL;
    memcpy( new->name, name, len );
    new->name[len] = '\0';
    return( new );
}

unsigned long BadObjFormat( void )
/***************************************/
{
    LnkMsg( FTL+MSG_BAD_OBJECT, "s", CurrMod->f.source->infile->name );
    return( 0 );
}

