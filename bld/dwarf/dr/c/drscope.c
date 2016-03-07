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
* Description:  DWARF debug info object scope management.
*
****************************************************************************/


#include "drpriv.h"
#include "drutils.h"
#include "drgettab.h"
#include "drscope.h"

static void ScopeBlockInit( dr_scope_block *block )
//************************************************************
// link list of free entries together with entries[0] as first
//************************************************************
{
    int             i;
    dr_scope_entry  *curr;
    dr_scope_entry  *last;

    block->next = NULL;
    last = NULL;
    curr = &block->entries[SCOPE_BLOCK_SIZE-1];
    for( i = SCOPE_BLOCK_SIZE; i > 0; --i ) {
        curr->next = last;
        last = curr;
        --curr;
    }
}

static dr_scope_entry *AllocScopeEntry( dr_scope_ctl *ctl )
//****************************************************
// get a free one alloc a new block if needed
//****************************************************
{
    dr_scope_entry  *new;

    if( ctl->free == NULL ) {
        dr_scope_block *block;

        block = DWRALLOC( sizeof( dr_scope_block ) );
        ScopeBlockInit( block );
        block->next = ctl->next;
        ctl->next = block;
        ctl->free = block->entries;
    }
    new = ctl->free;
    ctl->free = new->next;
    new->next = NULL;
    return( new );
}

static void ScopeCtlInit( dr_scope_ctl *ctl )
//**************************************************
// Init local block, set free list, init result
//**************************************************
{
    ScopeBlockInit( ctl->first );
    ctl->free = ctl->first[0].entries;
    ctl->next = NULL;
}

static void ScopeCtlFini( dr_scope_ctl *ctl )
//*****************************************
// Free any allocated blocks
//*****************************************
{
    dr_scope_block *curr;

    curr = ctl->next;
    while( curr != NULL ) {
        dr_scope_block *next;
        next = curr->next;
        DWRFREE( curr );
        curr = next;
    }
}

static bool AContainer( dr_handle enclose, int index, void *_df )
//***************************************************************
// Add entry to list stop when found search entry
//***************************************************************
{
    dr_scope_entry     *new;
    bool            cont;
    dr_scope_trail     *df = _df;

    index = index;
    new = AllocScopeEntry( &df->ctl );
    new->next = df->head;
    df->head = new;
    new->handle = enclose;
    if( enclose == df->target ) {
        cont = FALSE;
    } else {
        cont = TRUE;
    }
    return( cont );
}

extern void DRGetScopeList( dr_scope_trail *container, dr_handle of )
//*********************************************************************
// Walk in to of starting at ccu
//*********************************************************************
{
    compunit_info   *compunit;

    compunit = DWRFindCompileInfo( of );
    ScopeCtlInit( &container->ctl );
    container->target = of;
    container->head = NULL;
    if( compunit != NULL ) {
        DWRWalkContaining(  compunit->start + sizeof( compuhdr_prologue ), of, AContainer, container );
    }
}

extern void DREndScopeList( dr_scope_trail *container )
//******************************************************
// Free list
//******************************************************
{
    ScopeCtlFini( &container->ctl );
}
