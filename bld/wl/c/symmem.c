/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
#include "strtab.h"
#include "carve.h"
#include "permdata.h"
#include "symmem.h"


#define ALLOC_ALIGN         sizeof( void  * )

#define SYM_BLOCK_MIN       32
#define SYM_BLOCK_HDR_SIZE  __ROUND_UP_SIZE( sizeof( sym_block ), ALLOC_ALIGN )
#define SYM_BLOCK_SIZE(x)   (SYM_BLOCK_HDR_SIZE + (x))
#define SYM_BLOCK_DATA(b,x) ((char *)(b) + SYM_BLOCK_HDR_SIZE + (x))

typedef struct sym_block {
    struct sym_block    *next;       /* NOTE: this *must* be the first field */
    size_t              size;
//    char                block[];
} sym_block;

typedef struct block_data {
    sym_block       *list;
    size_t          currbrk;
} block_data;

static block_data Pass1Blocks;
static block_data PermBlocks;

void ResetPermBlocks( void )
/**************************/
/* allocate memory for symbol table allocation and code */
{
    PermBlocks.list = NULL;
}

void ResetPass1Blocks( void )
/***************************/
{
    Pass1Blocks.list = NULL;
}

static bool ShrinkBlock( block_data *block_set )
/**********************************************/
{
#ifdef __WATCOMC__
    sym_block   *block;

    if( block_set->list == NULL )
        return( false );
    if( block_set->currbrk >= block_set->list->size )
        return( false );
    block = MemReallocSafe( block_set->list, SYM_BLOCK_SIZE( block_set->currbrk ) );
    block->size = block_set->currbrk;
    /* assuming that a shrinkage will not move the block */
  #ifdef DEVBUILD
    if( block != block_set->list ) {
        LnkMsg( FTL+MSG_INTERNAL, "s", "realloc moved shrinked block!" );
    }
  #endif
    return( true );
#else
    /* There is no guarantee realloc() won't move memory - just don't do it */

    /* unused parameters */ (void)block_set;

    return( false );
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

static void GetNewBlock( block_data *block_set, size_t size )
/***********************************************************/
{
    size_t              try;
    sym_block           *block;

    ShrinkBlock( block_set );
    try = _16K;     /* default block size */
    if( try < size )
        try = size;
    for( ;; ) {
        block = MemAlloc( SYM_BLOCK_SIZE( try ) );
        if( block != NULL )
            break;
        try /= 2;
        if( try < size || try < SYM_BLOCK_MIN ) {
            LnkMsg( FTL + MSG_NO_DYN_MEM, NULL );
        }
    }
    block->size = try;
    block->next = block_set->list;
    block_set->list = block;
    block_set->currbrk = 0;
}

static void *AllocBlock( size_t size, block_data *block_set )
/***********************************************************/
{
    void            *ptr;
    size_t          newbrk;

    size = __ROUND_UP_SIZE( size, ALLOC_ALIGN );
    newbrk = block_set->currbrk + size;
    if( block_set->list == NULL ) {
        GetNewBlock( block_set, size );
    } else if( newbrk > block_set->list->size ) {
        GetNewBlock( block_set, size );
    }
    ptr = SYM_BLOCK_DATA( block_set->list, block_set->currbrk );
    block_set->currbrk += size;
    return( ptr );
}

void *Pass1Alloc( size_t size )
/*****************************/
{
    return( AllocBlock( size, &Pass1Blocks ) );
}

void *PermAlloc( size_t size )
/****************************/
/* allocate a thunk of permanently allocated memory */
{
    return( AllocBlock( size, &PermBlocks ) );
}

void BasicInitSym( symbol *sym )
/*************************************/
{
    sym->hash = NULL;
    sym->link = NULL;
    sym->publink = NULL;
    SET_ADDR_UNDEFINED( sym->addr );
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
    BasicInitSym( sym );
    if( LastSym != NULL )
        LastSym->link = sym;
    LastSym = sym;
    if( HeadSym == NULL )
        HeadSym = sym;
    return( sym );
}

void ReleasePass1Blocks( void )
/*****************************/
/* free pass1 block allocations */
{
    FreeList( Pass1Blocks.list );
    Pass1Blocks.list = NULL;
}

void ReleasePermBlocks( void )
/****************************/
/* free memory used for symbol table allocation and code */
{
    FreeList( PermBlocks.list );
    PermBlocks.list = NULL;
}
