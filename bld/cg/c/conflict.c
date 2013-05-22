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
* Description:  Conflict manager functions (selection of best register set
*               for variable).
*
****************************************************************************/


#include "cgstd.h"
#include "cgdefs.h"
#include "coderep.h"
#include "freelist.h"
#include "zoiks.h"
#include "data.h"
#include "feprotos.h"

extern  reg_set_index   IndexIntersect(reg_set_index,type_class_def,bool);
extern  reg_set_index   RegIntersect(reg_set_index,reg_set_index);
extern  reg_set_index   SegIndex(void);
extern  name            *DeAlias(name*);

static  pointer         *ConfFrl;
static  pointer         *ConfAliasVarsFrl;

/* forward declarations */
extern  void    MarkPossible( instruction *ins,
                              name *opnd, reg_set_index idx );
extern  void    FreeAConflict( conflict_node *conf );

extern  conflict_node   *AddConflictNode( name *opnd )
/****************************************************/
{
    conflict_node       *new;
    name                *scan;
    t_flags             flags;
    fe_attr             attr;

    if( opnd->n.class == N_TEMP ) opnd = DeAlias( opnd );
    new = AllocFrl( &ConfFrl, sizeof( conflict_node ) );
    new->name           = opnd;
    new->next_conflict  = ConfList;
    new->next_for_name  = opnd->v.conflict;
    _GBitInit( new->id.out_of_block, EMPTY );
    _LBitInit( new->id.within_block, EMPTY );
    new->start_block    = NULL;
    new->ins_range.first= NULL;
    new->ins_range.last = NULL;
    new->num_constrained= 0;
    new->available      = 0;
    new->possible       = RL_NUMBER_OF_SETS;
    new->savings        = 0;
    new->tree           = NULL;
    new->state          = EMPTY;
    new->possible_for_alias_list = NULL;
    if( opnd->n.class == N_TEMP ) {
        flags = HAD_CONFLICT;
        if( new->next_for_name != NULL ) {
            flags |= CROSSES_BLOCKS;
        }
        scan = opnd;
        for(;;) {
            scan->v.conflict = new;
            scan->t.temp_flags |= flags;
            scan = scan->t.alias;
            if( scan == opnd ) break;
        }
    } else {
        if( opnd->m.memory_type == CG_FE ) {
            attr = FEAttr( opnd->v.symbol );
            if( ( attr & FE_CONSTANT ) ||
                ( ( ( attr & (FE_GLOBAL|FE_VISIBLE) ) == 0 ) &&
                    _IsModel( RELAX_ALIAS ) ) ) {
                new->state |= OK_ACROSS_CALLS;
            }
        }
        opnd->v.conflict      = new;
    }
    ConfList            = new;
    return( new );
}


static  conflict_node   *AddOne( name *opnd, block *blk )
/*******************************************************/
{
    if( opnd->v.usage & USE_MEMORY ) {
        if( opnd->n.class == N_TEMP ) {
            if( !( opnd->v.usage & USE_IN_ANOTHER_BLOCK ) ) {
                if( !( opnd->t.temp_flags & HAD_CONFLICT ) ) {
                    if( opnd->t.temp_flags & CROSSES_BLOCKS ) {
                        opnd->t.u.block_id = NO_BLOCK_ID;
                    } else {
                        opnd->t.u.block_id = blk->id;
                    }
                }
            }
        }
        return( NULL );
    }
    return( AddConflictNode( opnd ) );
}


static  conflict_node   *FindConf( name *opnd,
                                  block *blk, instruction *ins )
/**************************************************************/
{
    conflict_node *conf;

    conf = opnd->v.conflict;
    if( conf == NULL ) return( AddOne( opnd, blk ) );
    if( conf->start_block == NULL ) return( conf ); /* not filled in yet */
    for(;;) {                                       /* find the right one */
        _INS_NOT_BLOCK( conf->ins_range.first );
        _INS_NOT_BLOCK( conf->ins_range.last );
        if( conf->start_block != NULL
         && ins->id >= conf->ins_range.first->id
         && ins->id <= conf->ins_range.last->id ) return( conf );
        conf = conf->next_for_name;
        if( conf == NULL ) break;
    }
    conf = opnd->v.conflict;
    if( ( opnd->v.usage & USE_IN_ANOTHER_BLOCK ) == 0 ) {
        for(;;) {
            if( conf->start_block == blk ) break;
            conf = conf->next_for_name;
            if( conf == NULL ) return( AddOne( opnd, blk ) );
        }
    }
    return( conf );
}


extern  conflict_node   *FindConflictNode( name *opnd,
                                          block *blk, instruction *ins )
/**********************************************************************/
{
    conflict_node       *conf;
    name                *old;
    name                *scan;

    old = opnd;
    if( opnd->n.class == N_TEMP ) {
        opnd = DeAlias( opnd );
    } else if( ( opnd->n.class != N_MEMORY || _IsntModel( RELAX_ALIAS ) ) ) {
        return( NULL );
    }
    _INS_NOT_BLOCK( ins );
    conf = FindConf( opnd, blk, ins );
    if( conf == NULL ) return( NULL );
    if( conf->start_block == NULL ) {
        conf->ins_range.first = ins;
        conf->ins_range.last = ins;
        conf->start_block = blk;
    } else {
        if( ins->id < conf->ins_range.first->id ) {
            conf->ins_range.first = ins;
            conf->start_block = blk;
        }
        if( ins->id > conf->ins_range.last->id ) {
            conf->ins_range.last = ins;
        }
    }
    if( opnd->n.class == N_TEMP ) {
        scan = opnd;
        for(;;) {
            scan->v.conflict = opnd->v.conflict;
            scan->t.temp_flags |= HAD_CONFLICT;
            scan = scan->t.alias;
            if( scan == opnd ) break;
        }
    } else {
        old->v.conflict = opnd->v.conflict;
    }
    return( conf );
}


extern  void    MarkSegment( instruction *ins, name *opnd )
/*********************************************************/
{
    MarkPossible( ins, opnd, SegIndex() );
}


extern  conflict_node   *NameConflict( instruction *ins, name *opnd )
/*******************************************************************/
{
    conflict_node       *conf;

    if( opnd->n.class == N_TEMP ) {
        opnd = DeAlias( opnd );
    } else if( opnd->n.class != N_MEMORY ) {
        return( NULL );
    }
    _INS_NOT_BLOCK( ins );
    conf = opnd->v.conflict;
    while( conf != NULL ) {
        _INS_NOT_BLOCK( conf->ins_range.first );
        _INS_NOT_BLOCK( conf->ins_range.last );
        if( conf->ins_range.first->id <= ins->id
         && conf->ins_range.last->id  >= ins->id ) return( conf );
        conf = conf->next_for_name;
    }
    return( NULL );
}


/*
 * Find "possible" list entry for alias temp variable, or return NULL if
 * variable is not in list.
 */
static  possible_for_alias *FindPossibleForAlias( conflict_node *conf, name *opnd )
{
    possible_for_alias   *aposs;

    aposs = conf->possible_for_alias_list;
    while( aposs != NULL ) {
        if( aposs->temp == opnd ) {
            break;
        }
        aposs = aposs->next;
    }
    return aposs;
}


/*
 * Return "possible" list entry for alias temp variable. If none found 
 * (new variable), create new entry and assign RL_NUMBER_OF_SETS
 * (no restictions yet) to it.
 */
static  possible_for_alias *MakePossibleForAlias( conflict_node *conf, name *opnd )
{
    possible_for_alias   *aposs;

    aposs = FindPossibleForAlias( conf, opnd );
    if( aposs == NULL ) {
        aposs = AllocFrl( &ConfAliasVarsFrl, sizeof( possible_for_alias ) );
        aposs->next     = conf->possible_for_alias_list;
        aposs->temp     = opnd;
        aposs->possible = RL_NUMBER_OF_SETS;
        conf->possible_for_alias_list = aposs;
    }
    return aposs;
}


/*
 * Return "possible" field (reg_set_index of possible registers) for temp
 * variable. For master variables, it's kept in "conf->possible", aliases
 * are stored in "conf->possible_for_alias_list" list.
 */
extern  reg_set_index   GetPossibleForTemp(conflict_node *conf, name *temp)
{
    reg_set_index       possible;
    possible_for_alias  *aposs;

    if( temp->t.temp_flags & ALIAS ) {
        aposs = FindPossibleForAlias( conf, temp );
        if( aposs == NULL ) {
            possible = RL_NUMBER_OF_SETS; /* not referenced, no restrictions yet */
        } else {
            possible = aposs->possible;
        }
    }
    else {
        possible = conf->possible;
    }

    return possible;
}


extern  void    MarkPossible( instruction *ins,
                              name *opnd, reg_set_index idx )
/***********************************************************/
{
    conflict_node       *conf;
    possible_for_alias  *aposs;

    conf = NameConflict( ins, opnd );
    if( conf != NULL ) {
        if( opnd->n.class == N_TEMP && ( opnd->t.temp_flags & ALIAS ) ) {
            aposs = MakePossibleForAlias( conf, opnd );
            aposs->possible = RegIntersect( aposs->possible, idx );
        } else {
            conf->possible = RegIntersect( conf->possible, idx );
        }
    }
}


extern  reg_set_index   MarkIndex( instruction *ins,
                                   name *opnd, bool is_temp_index )
/*****************************************************************/
{
    conflict_node       *conf;
    reg_set_index       possible;
    type_class_def      class;
    possible_for_alias  *aposs;

    conf = NameConflict( ins, opnd );
    if( conf == NULL ) return( RG_ );
    class = opnd->n.name_class;
    if( opnd->n.class == N_TEMP && ( opnd->t.temp_flags & ALIAS ) ) {
        aposs = MakePossibleForAlias( conf, opnd );
        possible = IndexIntersect( aposs->possible, class, is_temp_index );
        aposs->possible = possible;
    } else {
        possible = IndexIntersect( conf->possible, class, is_temp_index );
        conf->possible = possible;
    }
    return( possible );
}

/*
 * Free list of "possible" records for aliased temp vars
 */
extern  void    FreePossibleForAlias( conflict_node *conf )
{
    possible_for_alias  *aposs, *next;

    aposs = conf->possible_for_alias_list;
    while ( aposs != NULL ) {
        next = aposs->next;
        FrlFreeSize( &ConfAliasVarsFrl, (pointer *)aposs, sizeof( possible_for_alias ) );
        aposs = next;
    }
    conf->possible_for_alias_list = NULL;
}


extern  void    FreeConflicts( void )
/***********************************/
{
    while( ConfList != NULL ) {
        FreeAConflict( ConfList );
    }
}


extern  void    FreeAConflict( conflict_node *conf )
/**************************************************/
{
    name                *opnd;
    name                *scan;
    conflict_node       *prev;

    if( conf->start_block != NULL ) {
        _LBitTurnOn( conf->start_block->available_bit, conf->id.within_block );
    }
    opnd = conf->name;
    prev = opnd->v.conflict;
    if( prev == conf ) {
        if( opnd->n.class == N_TEMP ) {
            scan = opnd;
            for(;;) {
                scan->v.conflict = conf->next_for_name;
                scan->t.temp_flags |= HAD_CONFLICT;
                scan = scan->t.alias;
                if( scan == opnd ) break;
            }
        } else {
            opnd->v.conflict = conf->next_for_name;
        }
    } else {
        while( prev->next_for_name != conf ) {
            prev = prev->next_for_name;
        }
        prev->next_for_name = conf->next_for_name;
    }
    prev = ConfList;
    if( prev == conf ) {
        ConfList = conf->next_conflict;
    } else {
        while( prev->next_conflict != conf ) {
            prev = prev->next_conflict;
        }
        prev->next_conflict = conf->next_conflict;
    }
    if( opnd->n.class == N_TEMP ) {
        opnd->t.u.block_id = NO_BLOCK_ID;
        if( !_FrontEndTmp( opnd )
         && opnd->v.conflict == NULL
         && ( opnd->v.usage & USE_IN_ANOTHER_BLOCK ) == 0
         && conf->start_block != NULL
         && ( opnd->t.temp_flags & CROSSES_BLOCKS ) == 0 ) {
            opnd->t.u.block_id = conf->start_block->id;
        }
    }
    FreePossibleForAlias( conf );
    FrlFreeSize( &ConfFrl, (pointer *)conf, sizeof( conflict_node ) );
}


extern  void    InitConflict( void )
/**********************************/
{
    InitFrl( &ConfFrl );
    InitFrl( &ConfAliasVarsFrl );
    ConfList = NULL;
}


extern  bool    ConfFrlFree( void )
/*********************************/
{
    bool    ret;

    ret  = FrlFreeAll( &ConfFrl, sizeof( conflict_node ) );
    ret |= FrlFreeAll( &ConfAliasVarsFrl, sizeof( possible_for_alias ) );
    return ret;
}
