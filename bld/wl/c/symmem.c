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
* Description:  Symbol table extra memory routines.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "linkstd.h"
#include "pcobj.h"
#include "msg.h"
#include "alloc.h"
#include "wlnkmsg.h"
#include "strtab.h"
#include "carve.h"
#include "permdata.h"


#define SYM_BLOCK_SIZE      (16*1024)
#define SYM_BLOCK_MIN       32

typedef struct sym_block {
    struct sym_block *  next;       /* NOTE: this *must* be the first field */
    unsigned            size;
    char                block[ 1 ];
} sym_block;

#define ALLOC_SIZE  (sizeof( sym_block )-1)

typedef struct block_data {
    sym_block *     list;
    unsigned        currbrk;
} block_data;

static block_data Pass1Blocks;
static block_data PermBlocks;

static void * AllocBlock( unsigned, block_data * );

void GetSymBlock( void )
/**********************/
/* allocate memory for symbol table allocation and code */
{
    PermBlocks.list = NULL;
}

void MakePass1Blocks( void )
/**************************/
{
    Pass1Blocks.list = NULL;
}

static bool ShrinkBlock( block_data *block )
/******************************************/
{
#ifdef __WATCOMC__
    sym_block   *new;

    if( block->list == NULL )
        return( FALSE );
    if( block->currbrk >= block->list->size )
        return( FALSE );
    _LnkReAlloc( new, block->list, block->currbrk + ALLOC_SIZE );
    new->size = block->currbrk;
    /* assuming that a shrinkage will not move the block */
#ifdef _DEVELOPMENT
    if( new != block->list ) {
        LnkMsg( FTL+MSG_INTERNAL, "s", "realloc moved shrinked block!" );
    }
#endif
    return( TRUE );
#else
    /* There is no guarantee realloc() won't move memory - just don't do it */
    block = block;
    return( FALSE );
#endif
}

bool PermShrink( void )
/*********************/
/* shrink down the current permanent allocation blocks */
{
    bool    ret;

    ret = ShrinkBlock( &PermBlocks );
    if( !ret ) {
        ret = ShrinkBlock( &Pass1Blocks );
    }
    return( ret );
}

void *Pass1Alloc( size_t size )
/*****************************/
{
    return( AllocBlock( size, &Pass1Blocks ) );
}

void *PermAlloc( size_t size )
/****************************/
/* allocate a hunk of permanently allocated memory */
{
    return( AllocBlock( size, &PermBlocks ) );
}

static void GetNewBlock( block_data *block, unsigned size )
/*********************************************************/
{
    unsigned            try;
    sym_block           *new;

    ShrinkBlock( block );
    try = SYM_BLOCK_SIZE;
    if( try < size )
        try = size;
    for( ;; ) {
        _LnkAlloc( new, try + ALLOC_SIZE );
        if( new != NULL )
            break;
        try /= 2;
        if( try < size || try < SYM_BLOCK_MIN ) {
            LnkMsg( FTL + MSG_NO_DYN_MEM, NULL );
        }
    }
    new->next = block->list;
    block->list = new;
    new->size = try;
    block->currbrk = 0;
}

static void * AllocBlock( unsigned size, block_data *block )
/**********************************************************/
{
    void *              ptr;
    unsigned long       newbrk;

#define ROUND (sizeof(int)-1)

    size = (size + ROUND) & ~ROUND;
    newbrk = (unsigned long) block->currbrk + size;
    if( block->list == NULL ) {
        GetNewBlock( block, size );
    } else if( newbrk > block->list->size ) {
#ifdef __WATCOMC__
        ptr = NULL;
        if( newbrk < UINT_MAX - ALLOC_SIZE  ) {
            /* try to expand block without moving it */
            _LnkExpand( ptr, block->list, ALLOC_SIZE + newbrk );
        }
        if( ptr != NULL ) {
            block->list->size = newbrk;
        } else {
            GetNewBlock( block, size );
        }
#else
        GetNewBlock( block, size );
#endif
    }
    ptr = block->list->block + block->currbrk;
    block->currbrk += size;
    return( ptr );
}

void BasicInitSym( symbol *sym )
/*************************************/
{
    sym->hash = NULL;
    sym->link = NULL;
    sym->publink = NULL;
    sym->addr.off = 0;
    sym->addr.seg = UNDEFINED;
    sym->mod = NULL;
    sym->p.seg = NULL;
    sym->info = SYM_REGULAR | SYM_IN_CURRENT;
    sym->u.altdefs = NULL;      // this sets all union members to zero.
    sym->e.def = NULL;          // ditto
    sym->prefix = NULL;
}

symbol * AddSym( void )
/****************************/
/* allocate and initialize a new symbol */
{
    symbol              *sym;

    sym = CarveAlloc( CarveSymbol );
    BasicInitSym(sym);
    if( LastSym != NULL ) LastSym->link = sym;
    LastSym = sym;
    if( HeadSym == NULL ) HeadSym = sym;
    return( sym );
}

void ReleasePass1( void )
/******************************/
/* free pass1 block allocations */
{
    FreeList( Pass1Blocks.list );
    Pass1Blocks.list = NULL;
}

void RelSymBlock( void )
/*****************************/
/* free memory used for symbol table allocation and code */
{
    FreeList( PermBlocks.list );
    PermBlocks.list = NULL;
}
