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
* Description:  Build code for a selection (switch) statement.
*
****************************************************************************/


#include "cgstd.h"
#include "cgdefs.h"
#include "coderep.h"
#include "cgmem.h"
#include "seldef.h"
#include "tree.h"
#include "zoiks.h"
#include "data.h"
#include "types.h"
#include "bldins.h"
#include "makeins.h"

extern  void            AddTarget(code_lbl *,bool);
extern  signed_32       IfCost(select_node*,int);
extern  void            EnLink(code_lbl *,bool);
extern  name            *ScanCall(tbl_control*,name*,type_class_def);
extern  void            AddIns(instruction*);
extern  signed_32       JumpCost(select_node*);
extern  void            Generate(bool);
extern  signed_32       ScanCost(select_node*);
extern  void            GenBlock( block_class, int );
extern  name            *GenIns(an);
extern  tbl_control     *MakeScanTab(select_list*,signed_32,code_lbl *,cg_type,cg_type);
extern  tbl_control     *MakeJmpTab(select_list*,signed_32,signed_32,code_lbl *);
extern  name            *SelIdx(tbl_control*,an);
extern  type_def        *SelNodeType(an,bool);
extern  void            *SortList(void *,unsigned,bool (*)(void*,void*) );
extern  void            MkSelOp( name *idx, type_class_def class );

/* forward declarations */
extern  void    BGSelRange( select_node *s_node, signed_32 lo, signed_32 hi, code_lbl *label );
static  void    ScanBlock( tbl_control *table, an node, type_class_def class, code_lbl *other );
static  void    SelectBlock( tbl_control *table, an node, code_lbl *other );

static  select_list *NewCase( signed_32 lo, signed_32 hi, code_lbl *label ) {
/******************************************************************************/

    select_list         *new_entry;

    new_entry = CGAlloc( sizeof( select_list ) );
    new_entry->low = lo;
    new_entry->high = hi;
    new_entry->count = hi - lo + 1;
    new_entry->label = label;
    new_entry->next = NULL;
    return( new_entry );
}


extern  select_node     *BGSelInit( void ) {
/************************************/

    select_node         *s_node;

    s_node = CGAlloc( sizeof( select_node ) );
    s_node->num_cases = 0;
    s_node->other_wise = NULL;
    s_node->list = NULL;
#ifndef NDEBUG
    s_node->useinfo.hdltype = NO_HANDLE;
    s_node->useinfo.used = FALSE;
#endif
    return( s_node );
}


extern  void    BGSelCase( select_node *s_node, code_lbl *label,
                           signed_32 value ) {
/**************************************************************/

    BGSelRange( s_node, value, value, label );
}


extern  void    BGSelRange( select_node *s_node, signed_32 lo,
                            signed_32 hi, code_lbl *label ) {
/*************************************************************/

    select_list         *new_entry;

    if( ( hi ^ lo ) < 0 ) _Zoiks( ZOIKS_089 );
    new_entry = NewCase( lo, hi, label );
    new_entry->next = s_node->list;
    s_node->list = new_entry;
    s_node->num_cases += hi - lo + 1;
}


extern  void    BGSelOther( select_node *s_node, code_lbl *other ) {
/********************************************************************/

    s_node->other_wise = other;
}


static type_def         *SortTipe;

extern int SelCompare( signed_32 lo1, signed_32 lo2 ) {
/******************************************************/

    if( lo1 == lo2 ) return( 0 );
    if( SortTipe->attr & TYPE_SIGNED ) {
        if( lo1 < lo2 ) return( -1 );
    } else {
        if( (unsigned_32)lo1 < (unsigned_32)lo2 ) return( -1 );
    }
    return( 1 );
}


static  bool            NodeLess( void *s1, void *s2 ) {
/******************************************************/

    return( SelCompare( ((select_list *)s1)->low, ((select_list *)s2)->low ) < 0 );
}



static  void    SortNodeList( an node, select_node *s_node, bool is_signed ) {
/****************************************************************************/

    select_list *list;

    SortTipe = SelNodeType( node, is_signed );
    list = SortList( s_node->list, offsetof( select_list, next ), NodeLess );
    s_node->list = list;
    s_node->lower = list->low;
    while( list->next != NULL ) {
        list = list->next;
    }
    s_node->upper = list->high;
}


typedef enum sel_kind {
    U_SCAN,
    U_JUMP,
    U_IF,
    S_SCAN,
    S_JUMP,
    S_IF
} sel_kind;

static  void    MergeListEntries( select_node *s_node ) {
/*******************************************************/

    select_list *curr;
    select_list *next;

    for( curr = s_node->list, next = curr->next; next != NULL; next = curr->next ) {
        if( ( curr->high + 1 == next->low ) && ( curr->label == next->label ) ) {
            curr->high = next->high;
            curr->next = next->next;
            CGFree( next );
        } else {
            curr = next;
        }
    }
}


static  signed_32       DistinctIfCost( select_node *s_node ) {
/*************************************************************/

    select_list *curr;
    select_list *next;
    int         entries;

    entries = 1;
    for( curr = s_node->list, next = curr->next; next != NULL; next = next->next ) {
        if( ( curr->high + 1 != next->low ) || ( curr->label != next->label ) ) {
            ++entries;
            curr = next;
        }
    }
    return( IfCost( s_node, entries ) );
}


extern  cg_type SelType( unsigned_32 value_range ) {
/**************************************************/

    cg_type     tipe;

    if( ( value_range & 0xFFFF0000 ) == 0 ) {
        if( ( value_range & 0xFF00 ) == 0 ) {
            tipe = TY_UINT_1;
        } else {
            tipe = TY_UINT_2;
        }
    } else {
        tipe = TY_UINT_4;
    }
    if( tipe > SortTipe->refno ) {
        switch( SortTipe->refno ) {
        case TY_UINT_1:
        case TY_INT_1:
            tipe = TY_UINT_1;
            break;
        case TY_UINT_2:
        case TY_INT_2:
            tipe = TY_UINT_2;
            break;
        case TY_UINT_4:
        case TY_INT_4:
            tipe = TY_UINT_4;
            break;
        }
    }
    return( tipe );
}


static  type_def        *UnSignedIntTipe( type_def *tipe ) {
/*******************************************************/

    switch( tipe->length ) {
    case 1:
        return( TypeAddress( TY_UINT_1 ) );
    case 2:
        return( TypeAddress( TY_UINT_2 ) );
    case 4:
        return( TypeAddress( TY_UINT_4 ) );
    }
    _Zoiks( ZOIKS_102 );  /* if we get here bug */
    return( NULL );
}


static  an      GenScanTable( an node, select_node *s_node, type_def *tipe )
/**************************************************************************/
{
    bn          lt;
    cg_type     value_type;
    cg_type     real_type;

    value_type = SelType( s_node->upper - s_node->lower );
    real_type = tipe->refno;
    if( real_type != value_type ) {
        node = BGBinary( O_MINUS, node, BGInteger( s_node->lower, tipe ), tipe, TRUE );
        if( s_node->other_wise != NULL ) {
            lt = BGCompare( O_LE, BGDuplicate(node), BGInteger( s_node->upper - s_node->lower, tipe ),
                            NULL, UnSignedIntTipe( tipe ) );
            BGControl( O_IF_FALSE, lt, s_node->other_wise );
        }
    }
    ScanBlock( MakeScanTab( s_node->list, s_node->upper, s_node->other_wise, value_type, real_type ),
               node, (type_class_def)value_type, s_node->other_wise );
    return( node );
}


static  an      GenSelTable( an node, select_node *s_node, type_def *tipe) {
/**************************************************************************/

    bn          lt;

    if( s_node->lower != 0 ) {
        node = BGBinary( O_MINUS, node,
                          BGInteger( s_node->lower, tipe ), tipe , TRUE );
    }
    /* generate if's to check if index in table*/
    if( s_node->other_wise != NULL ) {
        lt = BGCompare( O_LE, BGDuplicate(node),
                        BGInteger( s_node->upper - s_node->lower, tipe ), NULL,
                        UnSignedIntTipe( tipe ) );
        BGControl( O_IF_FALSE, lt, s_node->other_wise );
    }
    /* generate table*/
    /* index into table*/
    node = BGConvert( node, UnSignedIntTipe( tipe ) ); /* value an unsigned index */
    SelectBlock( MakeJmpTab( s_node->list, s_node->lower, s_node->upper,
                             s_node->other_wise ),
                 node, s_node->other_wise );
    return( node );
}


static  void    DoBinarySearch( an node, select_list *list, type_def *tipe,
                               int lo, int hi, code_lbl *other,
                               signed_32 lobound, signed_32 hibound,
                               bool have_lobound, bool have_hibound ) {
/****************************************************************/

    int                 num;
    int                 mid;
    select_list         *mid_list;
    bn                  cmp;
    code_lbl            *lt;

    mid = lo + ( hi - lo ) / 2;
    mid_list = list;
    for( num = mid; num > 0; --num ) {
        mid_list = mid_list->next;
    }
    if( lo == hi ) {
        if( have_lobound && lobound == mid_list->low
         && have_hibound && hibound == mid_list->high ) {
             BGControl( O_GOTO, NULL, mid_list->label );
             return;
        } else if( mid_list->low == mid_list->high ) {
            cmp = BGCompare( O_EQ, BGDuplicate( node ),
                             BGInteger( mid_list->low, tipe ), NULL, tipe );
            BGControl( O_IF_TRUE, cmp, mid_list->label );
            BGControl( O_GOTO, NULL, other );
            return;
        }
    }
    if( hi == mid + 1 && mid_list->next->low == mid_list->next->high ) {
        /* a linear sequence for three different non-sequential cases where
           c1<c2<c3, looks like:
        if( a == c3 ) goto l3;
        if( a == c2 ) goto l2;
        if( a != c1 ) goto default;
        l1: ...

           a binary sequence for these three cases looks like:
        if( a < c2 goto lt;    \
        if( a <= c2 ) goto l2; /only one cmp ins on x86
        if( a == c3 ) goto l3;
        goto default;
        lt:
        if ( a != c1 ) goto default;
        l1: ...

        Advantage of the linear search:
        * 3 goto's instead of 5, resulting in smaller code.
        Advantage of the binary search:
        * Execution time for all the cases is more balanced. which one is
          really faster depends a lot on the CPU's branch prediction and
          other things that are very hard to measure here.

        Using a linear search here for <= 3 cases to save on code size
        with negligible performance loss or gain.
        */
        mid_list = mid_list->next;
        cmp = BGCompare( O_EQ, BGDuplicate( node ),
                         BGInteger( mid_list->low, tipe ), NULL, tipe );
        BGControl( O_IF_TRUE, cmp, mid_list->label );
        /* Because we only compared for equality, it is only possible to
           decrease the upper bound if it was already set and equal to
           the value we are comparing to. Otherwise the incoming value
           may still be higher, where the inner call may produce an
           unconditional O_GOTO to a specific case label!
        */
        if( have_hibound && hibound == mid_list->low )
            hibound--;
        DoBinarySearch( node, list, tipe, lo, mid, other,
                        lobound, hibound, have_lobound, have_hibound );
        return;
    }
    lt = AskForNewLabel();
    if( !have_lobound || SelCompare( lobound, mid_list->low ) < 0 ) {
        if( have_hibound && SelCompare( hibound, mid_list->low ) < 0 ) {
            BGControl( O_GOTO, NULL, lt );
        } else {
            cmp = BGCompare( O_LT, BGDuplicate( node ),
                             BGInteger( mid_list->low, tipe ), NULL, tipe );
            BGControl( O_IF_TRUE, cmp, lt );
        }
    }
    if( !have_lobound || SelCompare( lobound, mid_list->high ) <= 0 ) {
        if( have_hibound && SelCompare( hibound, mid_list->high ) <= 0 ) {
            BGControl( O_GOTO, NULL, mid_list->label );
        } else {
            cmp = BGCompare( O_LE, BGDuplicate( node ),
                             BGInteger( mid_list->high, tipe ), NULL, tipe );
            BGControl( O_IF_TRUE, cmp, mid_list->label );
        }
    }
    if( mid < hi ) {
        DoBinarySearch( node, list, tipe, mid+1, hi, other,
                        mid_list->high+1, hibound, TRUE, have_hibound );
    } else if( other != NULL ) {
        BGControl( O_GOTO, NULL, other );
    }
    BGControl( O_LABEL, NULL, lt );
    if( lo < mid ) {
        DoBinarySearch( node, list, tipe, lo, mid-1, other,
                        lobound, mid_list->low-1, have_lobound, TRUE );
    } else if( other != NULL ) {
        BGControl( O_GOTO, NULL, other );
    }
}


static  an      GenIfStmts( an node, select_node *s_node, type_def *tipe ) {
/**************************************************************************/

    select_list *list;
    int         nodes;

    nodes = 0;
    for( list = s_node->list; list != NULL; list = list->next ) {
        ++nodes;
    }
    DoBinarySearch( node, s_node->list, tipe, 0, nodes-1, s_node->other_wise,
                    0, 0, FALSE, FALSE );
    return( node );
}


extern  signed_32       NumValues( select_list *list, signed_32 hi ) {
/********************************************************************/

    signed_32           cases;

    cases = 0;
    for( ; list != NULL; list = list->next ) {
        if( SelCompare( list->high, hi ) > 0 ) {
            break;
        }
        cases += list->high - list->low + 1;
    }
    return( cases );
}

static  void    ScanBlock( tbl_control *table, an node, type_class_def class, code_lbl *other )
/************************************************************************************************/
{
    uint                i;
    uint                targets;
    name                *value;

    value = GenIns( node );
    MkSelOp( ScanCall( table, value, class ), class );
    i = 0;
    targets = 0;
    for( ;; ) {
        if( table->cases[ i ] != other ) {
            ++targets;
        }
        if( ++i == table->size ) break;
    }
    if( other != NULL ) {
        ++targets;
    }
    GenBlock( SELECT, targets );
    i = 0;
    for( ;; ) {
        if( table->cases[ i ] != other ) {
            AddTarget( table->cases[ i ], FALSE );
        }
        if( ++i == table->size ) break;
    }
    if( other != NULL ) {
        AddTarget( other, FALSE );
    }
    Generate( FALSE );
    EnLink( AskForNewLabel(), TRUE );
}


static  void    SelectBlock( tbl_control *table, an node, code_lbl *other ) {
/*****************************************************************************/

    uint                i;
    uint                targets;

    MkSelOp( SelIdx( table, node ), U2 );
    i = 0;
    targets = 0;
    for(;;) {
        if( table->cases[ i ] != other ) {
            ++targets;
        }
        if( ++i == table->size ) break;
    }
    if( other != NULL ) {
        ++targets;
    }
    GenBlock( SELECT, targets );
    i = 0;
    for(;;) {
        if( table->cases[ i ] != other ) {
            AddTarget( table->cases[ i ], FALSE );
        }
        if( ++i == table->size ) break;
    }
    if( other != NULL ) {
        AddTarget( other, FALSE );
    }
    Generate( FALSE );
    EnLink( AskForNewLabel(), TRUE );
}


extern  void    FreeTable( tbl_control *table ) {
/***********************************************/

    CGFree( table );
}


static  void    FreeSelectNode( select_node *s_node ) {
/*****************************************************/

    select_list         *list;
    select_list         *next;

    for( list = s_node->list; list != NULL; list = next ) {
        next = list->next;
        CGFree( list );
    }
    CGFree( s_node );
}


extern  void    BGSelect( select_node *s_node, an node, cg_switch_type allowed ) {
/********************************************************************************/

    signed_32   cost;
    signed_32   best;
    sel_kind    kind;

    if( ( allowed & CG_SWITCH_ALL ) == 0 ) {
        _Zoiks( ZOIKS_090 );
        allowed = CG_SWITCH_ALL;
    }
    kind = 0;
    node = Arithmetic( node, TypeInteger );
    if( s_node->num_cases != 0 ) {
        best = 0x7FFFFFFF;
        SortNodeList( node, s_node, TRUE ); /* sort signed */
        if( allowed & CG_SWITCH_SCAN ) {
            cost = ScanCost( s_node );
            if( cost <= best ) {
                best = cost;
                kind = S_SCAN;
            }
        }
        if( allowed & CG_SWITCH_TABLE ) {
            cost = JumpCost( s_node );
            if( cost <= best ) {
                best = cost;
                kind = S_JUMP;
            }
        }
        if( allowed & CG_SWITCH_BSEARCH ) {
            cost = DistinctIfCost( s_node );
            if( cost <= best ) {
                best = cost;
                kind = S_IF;
            }
        }
        SortNodeList( node, s_node, FALSE ); /* sort unsigned */
        if( allowed & CG_SWITCH_SCAN ) {
            cost = ScanCost( s_node );
            if( cost <= best ) {
                best = cost;
                kind = U_SCAN;
            }
        }
        if( allowed & CG_SWITCH_TABLE ) {
            cost = JumpCost( s_node );
            if( cost <= best ) {
                best = cost;
                kind = U_JUMP;
            }
        }
        if( allowed & CG_SWITCH_BSEARCH ) {
            cost = DistinctIfCost( s_node );
            if( cost <= best ) {
                best = cost;
                kind = U_IF;
            }
        }
        switch( kind ) {
        case S_SCAN:
        case S_JUMP:
        case S_IF:
            SortNodeList( node, s_node, TRUE ); /* sort signed */
            break;
        }
        node = BGConvert( node, SortTipe );

        /*
         * We generate this bogus add 0 node so that we have a temporary
         * for the actual value to switch on. If we don't do this, a
         * problem could occur if the switch variable was volatile and
         * we loaded it once to decide whether to use a scan table and
         * once to index into the scan table. This would be bad if it
         * changed in between.
         */
        node = BGBinary(O_PLUS, node, BGInteger( 0, SortTipe ), SortTipe, TRUE );

        MergeListEntries( s_node );
        switch( kind ) {
        case S_SCAN:
        case U_SCAN:
            node = GenScanTable( node, s_node, SortTipe );
            break;
        case S_JUMP:
        case U_JUMP:
            node = GenSelTable( node, s_node, SortTipe );
            break;
        case S_IF:
        case U_IF:
            node = GenIfStmts( node, s_node, SortTipe );
            break;
        }
    } else if( s_node->other_wise != NULL ) {
        BGControl( O_GOTO, NULL, s_node->other_wise );
    }
    BGDone( node );
    FreeSelectNode( s_node );
}
