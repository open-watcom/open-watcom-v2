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
* Description:  Instruction scoring helper functions.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "score.h"
#include "zoiks.h"
#include "data.h"
#include "utils.h"

static  pointer SegConstSymbol;/*no other pointer points to here*/
static  pointer HighAddrSymbol;/*ditto*/
static  pointer HighAddrConst; /*ditto*/


static  bool    ScoreSame( score_info *x, score_info *y ) {
/*********************************************************/

    if( x->class != y->class ) return( FALSE );
    if( x->base != y->base ) return( FALSE );
    if( x->index_reg != y->index_reg ) return( FALSE );
    if( x->class == N_TEMP
     && x->symbol.t->v.id == y->symbol.t->v.id ) return( TRUE );
    if( x->class == N_VOLATILE ) return( FALSE );
    if( x->class == N_INITIAL ) return( FALSE );
    if( x->class == N_INDEXED
     && x->scale != y->scale ) return( FALSE );
    if( x->symbol.p == y->symbol.p ) return( TRUE );
    return( FALSE );
}


static  bool    ScoreStomp( score_info *x, score_info *y ) {
/**********************************************************/

    score_info  *tmp;

    /* get the 'free' index into x if there is one*/
    if( y->class == N_INDEXED && y->base == NULL ) {
        tmp = x;
        x = y;
        y = tmp;
    }
    if( x->class == N_INDEXED && x->base == NULL ) {
        switch( y->class ) {
        case N_MEMORY:
            if( _IsModel( RELAX_ALIAS ) )
            return( FALSE );
            /* fall through */
        case N_INDEXED:
            return( TRUE );
        case N_TEMP:
            if( y->symbol.v->usage & USE_ADDRESS )
            return( TRUE );
            return( FALSE );
        }
    }
    /* get the 'bound' index into x if there is one*/
    if( y->class == N_INDEXED ) {
        tmp = x;
        x = y;
        y = tmp;
    }
    if( x->class == N_INDEXED && x->base != NULL ) {
        switch( y->class ) {
        case N_TEMP:
            if( x->base->n.class == N_TEMP ) {
                if( y->symbol.t->v.id == x->base->t.v.id )
            return( TRUE );
            }
            break;
        case N_MEMORY:
            if( x->base->n.class == N_MEMORY ) {
                if( y->symbol.p == x->base->v.symbol )
            return( TRUE );
            }
            break;
        case N_INDEXED:
            if( y->base == NULL )
            return( TRUE );
            if( y->base->n.class != x->base->n.class )
            return( FALSE );
            switch( x->base->n.class ) {
            case N_TEMP:
                return( y->base->t.v.id == x->base->t.v.id );
            case N_MEMORY:
                return( y->base->v.symbol == x->base->v.symbol );
            }
            break;
        }
    }
    return( FALSE );
}

extern  bool    ScoreLookup( score *p, score_info *info ) {
/*********************************************************/

    score_list  *curr;

    if( info->class == N_VOLATILE ) return( FALSE );
    curr = *p->list;
    for(;;) {
        if( curr == NULL )
        break;
        if( ScoreSame( &curr->info, info ) != FALSE
         && curr->info.offset == info->offset )
        return( TRUE );
        curr = curr->next;
    }
    return( FALSE );
}


extern  bool    ScoreEqual( score *p, int index, score_info *info ) {
/*******************************************************************/

    if( ScoreLookup( &p[ index ], info ) )
        return( TRUE );
    if( _IsModel( SUPER_OPTIMAL ) ) {
        score_reg   *entry;
        bool        is_equal;
        type_length half_size;

        entry = ScoreList[  index  ];
        if( entry->high == NO_INDEX || entry->low == NO_INDEX )
        return( FALSE );
            /*  See if low parts & high parts of register pair contain*/
            /*  the right information*/
        if( info->class == N_CONSTANT )
        return( FALSE );
        if( ScoreLookup( &p[  entry->low  ], info ) == FALSE )
        return( FALSE );
        half_size = entry->size / 2;
        info->offset += half_size;
        is_equal = ScoreLookup( &p[  entry->high  ], info );
        info->offset -= half_size;
        return( is_equal );
    }
    return( FALSE );
}


static  void    ScoreInsert(  score *p,  int i,  score_info  *info ) {
/********************************************************************/

    score_list  *new;
    int         j;

    if( info->class == N_VOLATILE )
        return;
    if( HW_Ovlap( ScoreList[ i ]->reg, CurrProc->state.unalterable ) )
        return;
    new = NewScListEntry();
    Copy( info, &new->info, sizeof( score_info ) );
    new->next = *p[ i ].list;
    *p[ i ].list = new;
    j = ScoreCount;
    for(;;) {
       if( --j < 0 )
           return;
       if( ( j != i ) && ( ScoreEqual( p, j, info ) != FALSE ) )
           break;
    }
    RegAdd( p, i, j );
}


static  void    ScoreAdd( score *p, int i, score_info *info ) {
/*************************************************************/

    if( _IsModel( SUPER_OPTIMAL ) ) {
        score       *first;
        score       *curr;

        if( (info->class == N_INDEXED) && (info->index_reg != NO_INDEX) ) {
            first = &p[ info->index_reg ];
            curr = first;
            for(;;) {
                info->index_reg = ScoreList[ curr->index ]->reg_name->r.reg_index;
                if( ScoreLookup( &p[ i ], info ) == FALSE ) {
                    ScoreInsert( p, i, info );
                }
                curr = curr->next_reg;
                if( curr == first ) break;
            }
        } else {
            if( ScoreLookup( &p[ i ], info ) == FALSE ) {
                ScoreInsert( p, i, info );
            }
        }
    } else {
        if( ScoreLookup( &p[ i ], info ) == FALSE ) {
            ScoreInsert( p, i, info );
        }
    }
}


extern  void    ScoreAssign( score *p, int index, score_info *info ) {
/********************************************************************/


    ScoreAdd( p, index, info );
    if( _IsModel( SUPER_OPTIMAL ) ) {
        score_reg   *entry;
        uint        hi_off;
        uint        lo_off;
        uint        offset;

        entry = ScoreList[  index  ];
        if( entry->high != NO_INDEX && entry->low != NO_INDEX ) {
            if( info->class != N_CONSTANT ) {
                hi_off = info->offset + entry->size / 2;
                lo_off = info->offset;
            } else {
                if( info->symbol.p != NULL ) return; /* relocatable const*/
                hi_off = info->offset >> ( entry->size * 4 );
                lo_off = info->offset & ~( hi_off << ( entry->size * 4 ) );
            }
            offset = info->offset;
            info->offset = hi_off;
            ScoreAdd( p, entry->high, info );
            info->offset = lo_off;
            ScoreAdd( p, entry->low, info );
            info->offset = offset;
        }
    }
}


extern  void    ScoreInfo( score_info *info, name *op ) {
/*********************************************************/

    if( op->n.class == N_INDEXED
     && op->i.index_flags ==( X_FAKE_BASE | X_BASE_IS_INDEX) ) {
        op = op->i.base; /* track memory location */
    }
    info->class = op->n.class;
    info->scale = 0;
    info->base  = NULL;
    info->index_reg = NO_INDEX;
    switch( op->n.class ) {
    case N_CONSTANT:
        switch( op->c.const_type ) {
        case CONS_ABSOLUTE:
            info->symbol.p = NULL;
            info->offset = op->c.int_value;
            break;
        case CONS_SEGMENT:
            info->symbol.p = &SegConstSymbol;
            info->offset = op->c.int_value;
            break;
        case CONS_OFFSET:
        case CONS_ADDRESS:
            info->symbol.p = op->c.value;
            info->offset = op->c.int_value;
            break;
        case CONS_HIGH_ADDR:
            /* FIXME: not sure what to do here */
            if( op->c.value != NULL ) {
                info->symbol.p = &HighAddrSymbol;
                info->offset = (signed_32)(pointer_int)op->c.value;
            } else {
                info->symbol.p = &HighAddrConst;
                info->offset = (signed_32)op->c.int_value;
            }
            break;
        default:
            _Zoiks( ZOIKS_046 );
            break;
        }
        break;
    case N_TEMP:
        if( op->v.usage & VAR_VOLATILE ) {
            info->class = N_VOLATILE;
        }
        info->symbol.t = &(op->t);
        info->offset = op->v.offset;
        break;
    case N_MEMORY:
        if( op->v.usage & VAR_VOLATILE ) {
            info->class = N_VOLATILE;
        }
        info->symbol.p = op->v.symbol;
        info->offset = op->v.offset;
        break;
    case N_INDEXED:
        if( op->i.index_flags & X_VOLATILE ) {
            info->class = N_VOLATILE;
        }
        info->symbol.p = NULL;
        info->offset = op->i.constant;
        info->index_reg = op->i.index->r.reg_index;
        info->base = op->i.base;
    info->scale = op->i.scale;
        break;
    }
}


extern  bool    ScoreLAInfo( score_info *info, name *op ) {
/*********************************************************/

    switch( op->n.class ) {
    case N_TEMP:
    case N_MEMORY:
        info->class = N_ADDRESS;
        info->symbol.p = op;
        info->offset = 0;
        info->index_reg = NO_INDEX;
        info->base = NULL;
        return( TRUE );
    default:
        return( FALSE );
    }
}


extern  void    ScoreKillInfo( score *scoreboard, name *op,
                               score_info *info, hw_reg_set except ) {
/********************************************************************/

    score_list  *curr;
    score_list  **owner;
    score_reg   *entry;
    int         last_offset;
    int         i;


    /*   Memory from info+offset up to (not including) info+last_offset*/
    /*   has been overwritten*/

    last_offset = info->offset + op->n.size;
    entry = ScoreList[ 0 ];
    for( i = ScoreCount; i-- > 0; ) {
        if( !HW_Subset( except, entry->reg ) ) {
            owner = scoreboard->list;
            for(;;) {
                curr = *owner;
                if( curr == NULL )
            break;
                /*   Currently looking at memory from*/
                /*   curr->info+offset to curr->info+offset+entry->size*/
                /*   If the names 'info' and 'curr' match, then have an*/
                /*   overlap if*/
                /*       info.start < curr.end AND info.end > curr.start*/
                /**/
                /* e.g.  info       |-------|*/
                /*       curr            |-----|*/

/* impossible!       if( info->base == op ) break;*/
                if( ScoreStomp( info, &curr->info ) == FALSE ) {
                    if( ScoreSame( info, &curr->info )
                     && last_offset > curr->info.offset
                     && info->offset < curr->info.offset+entry->size ) {
                        *owner = curr->next;
                        FreeScListEntry( curr );
                        continue;
                    }
                    owner = &curr->next;
                } else {
                    *owner = curr->next;
                    FreeScListEntry( curr );
                }
            }
        }
        ++ entry;
        ++ scoreboard;
    }
}
