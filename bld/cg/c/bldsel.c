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
* Description:  Build code for a selection (switch) statement.
*
****************************************************************************/


#include "_cgstd.h"
#include "coderep.h"
#include "cgmem.h"
#include "seldef.h"
#include "tree.h"
#include "zoiks.h"
#include "data.h"
#include "types.h"
#include "bldins.h"
#include "makeins.h"
#include "makeaddr.h"
#include "bldsel.h"
#include "bldselco.h"
#include "makeblk.h"
#include "cgsrtlst.h"
#include "generate.h"
#include "i64.h"


static const type_def   *SortTipe;

static  select_list *NewCase( const signed_64 *lo, const signed_64 *hi, label_handle label )
/******************************************************************************************/
{
    select_list     *list;

    list = CGAlloc( sizeof( select_list ) );
    list->low = *lo;
    list->high = *hi;
    U64Sub( &list->count, hi, lo );
    U64Inc( list->count );
    list->label = label;
    list->next = NULL;
    return( list );
}


sel_handle  BGSelInit( void )
/***************************/
{
    sel_handle      s_node;

    s_node = CGAlloc( sizeof( select_node ) );
    Set64ValZero( s_node->num_cases );
    s_node->other_wise = NULL;
    s_node->list = NULL;
#ifdef DEVBUILD
    s_node->useinfo.hdltype = NO_HANDLE;
    s_node->useinfo.used = false;
#endif
    return( s_node );
}


void    BGSelCase( sel_handle s_node, label_handle label, const signed_64 *value )
/********************************************************************************/
{
    BGSelRange( s_node, value, value, label );
}


void    BGSelRange( sel_handle s_node, const signed_64 *lo, const signed_64 *hi, label_handle label )
/***************************************************************************************************/
{
    select_list     *list;

    /*
     *  lo sign hi sign status
     *     -       -      ok
     *     -       +      ok
     *     +       -      error
     *     +       +      ok
     */
    if( lo->u.sign.v < hi->u.sign.v )
        _Zoiks( ZOIKS_089 );
    list = NewCase( lo, hi, label );
    list->next = s_node->list;
    s_node->list = list;
    U64AddEq( &s_node->num_cases, &list->count );
}


void    BGSelOther( sel_handle s_node, label_handle other )
/*********************************************************/
{
    s_node->other_wise = other;
}


int SelCompare( const signed_64 *lo1, const signed_64 *lo2 )
/**********************************************************/
{
    if( SortTipe->attr & TYPE_SIGNED ) {
        return( I64Cmp( lo1, lo2 ) );
    }
    return( U64Cmp( lo1, lo2 ) );
}


static bool     NodeLess( void *s1, void *s2 )
/********************************************/
{
    return( SelCompare( &((const select_list *)s1)->low, &((const select_list *)s2)->low ) < 0 );
}



static void     SortNodeList( an node, sel_handle s_node, bool is_signed )
/************************************************************************/
{
    const select_list   *list;

    SortTipe = SelNodeType( node, is_signed );
    s_node->list = SortList( s_node->list, offsetof( select_list, next ), NodeLess );
    list = s_node->list;
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

static  void    MergeListEntries( sel_handle s_node )
/***************************************************/
{
    select_list     *list;
    select_list     *next;
    signed_64       tmp;

    for( list = s_node->list, next = list->next; next != NULL; next = list->next ) {
        tmp = list->high;
        U64Inc( tmp );
        if( U64isEq( tmp, next->low ) && ( list->label == next->label ) ) {
            /*
             * add/merge second range to first range
             */
            list->high = next->high;
            U64AddEq( &list->count, &next->count );
            /*
             * remove second range
             */
            list->next = next->next;
            CGFree( next );
        } else {
            list = next;
        }
    }
}


static cost_val DistinctIfCost( sel_handle s_node )
/*************************************************/
{
    const select_list   *list;
    const select_list   *next;
    uint_32             entries;
    signed_64           tmp;

    entries = 1;
    for( list = s_node->list, next = list->next; next != NULL; next = next->next ) {
        tmp = list->high;
        U64Inc( tmp );
        if( U64isntEq( tmp, next->low ) || ( list->label != next->label ) ) {
            ++entries;
            list = next;
        }
    }
    return( IfCost( s_node, entries ) );
}


cg_type SelType( const unsigned_64 *value_range )
/***********************************************/
{
    cg_type     tipe;

    if( U64High( *value_range ) ) {
        tipe = TY_UINT_8;
    } else if( U64HighWord( *value_range ) ) {
        tipe = TY_UINT_4;
    } else if( U64HighByte( *value_range ) ) {
        tipe = TY_UINT_2;
    } else {
        tipe = TY_UINT_1;
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
        case TY_UINT_8:
        case TY_INT_8:
            tipe = TY_UINT_8;
            break;
        }
    }
    return( tipe );
}


static const type_def   *UnSignedIntTipe( const type_def *tipe )
/**************************************************************/
{
    switch( tipe->length ) {
    case 1:
        return( TypeAddress( TY_UINT_1 ) );
    case 2:
        return( TypeAddress( TY_UINT_2 ) );
    case 4:
        return( TypeAddress( TY_UINT_4 ) );
    case 8:
        return( TypeAddress( TY_UINT_8 ) );
    }
    _Zoiks( ZOIKS_102 );  /* if we get here bug */
    return( NULL );
}

static an BGIntegerSel( const signed_64 *value, const type_def *tipe )
{
    if( tipe->length == 8 ) {
        return( BGInt64( *value, tipe ) );
    } else {
        return( BGInteger( I64Low( *value ), tipe ) );
    }
}

static  void    ScanBlock( tbl_control *table, an node, type_class_def type_class, label_handle other )
/*****************************************************************************************************/
{
    uint                i;
    block_num           targets;
    name                *value;

    value = GenIns( node );
    MkSelOp( ScanCall( table, value, type_class ), type_class );
    i = 0;
    targets = 0;
    for( ;; ) {
        if( table->cases[i] != other ) {
            ++targets;
        }
        i++;
        if( i == table->size ) {
            break;
        }
    }
    if( other != NULL ) {
        ++targets;
    }
    GenBlock( BLK_SELECT, targets );
    i = 0;
    for( ;; ) {
        if( table->cases[i] != other ) {
            AddTarget( table->cases[i], false );
        }
        i++;
        if( i == table->size ) {
            break;
        }
    }
    if( other != NULL ) {
        AddTarget( other, false );
    }
    Generate( false );
    EnLink( AskForNewLabel(), true );
}


static  an      GenScanTable( an node, sel_handle s_node, const type_def *tipe )
/******************************************************************************/
{
    an              lt;
    cg_type         value_type;
    cg_type         real_type;
    unsigned_64     tmp;

    U64Sub( &tmp, &s_node->upper, &s_node->lower );
    value_type = SelType( &tmp );
    real_type = tipe->refno;
    if( real_type != value_type ) {
        node = BGBinary( O_MINUS, node, BGIntegerSel( &s_node->lower, tipe ), tipe, true );
        if( s_node->other_wise != NULL ) {
            lt = BGCompare( O_LE, BGDuplicate(node), BGIntegerSel( &tmp, tipe ),
                            NULL, UnSignedIntTipe( tipe ) );
            BGControl( O_IF_FALSE, lt, s_node->other_wise );
        }
    }
    ScanBlock( MakeScanTab( s_node, value_type, real_type ),
               node, (type_class_def)value_type, s_node->other_wise );
    return( node );
}


static  void    SelectBlock( tbl_control *table, an node, label_handle other )
/****************************************************************************/
{
    uint                i;
    block_num           targets;

    MkSelOp( SelIdx( table, node ), U2 );
    i = 0;
    targets = 0;
    for( ;; ) {
        if( table->cases[i] != other ) {
            ++targets;
        }
        i++;
        if( i == table->size ) {
            break;
        }
    }
    if( other != NULL ) {
        ++targets;
    }
    GenBlock( BLK_SELECT, targets );
    i = 0;
    for( ;; ) {
        if( table->cases[i] != other ) {
            AddTarget( table->cases[i], false );
        }
        i++;
        if( i == table->size ) {
            break;
        }
    }
    if( other != NULL ) {
        AddTarget( other, false );
    }
    Generate( false );
    EnLink( AskForNewLabel(), true );
}


static  an      GenSelTable( an node, sel_handle s_node, const type_def *tipe )
/*****************************************************************************/
{
    an              lt;
    unsigned_64     tmp;

    if( U64isntZero( s_node->lower ) ) {
        node = BGBinary( O_MINUS, node,
                          BGIntegerSel( &s_node->lower, tipe ), tipe , true );
    }
    /*
     * generate if's to check if index in table
     */
    if( s_node->other_wise != NULL ) {
        U64Sub( &tmp, &s_node->upper, &s_node->lower );
        lt = BGCompare( O_LE, BGDuplicate( node ),
                        BGIntegerSel( &tmp, tipe ), NULL,
                        UnSignedIntTipe( tipe ) );
        BGControl( O_IF_FALSE, lt, s_node->other_wise );
    }
    /*
     * generate table
     * index into table
     */
    node = BGConvert( node, UnSignedIntTipe( tipe ) ); /* value an unsigned index */
    SelectBlock( MakeJmpTab( s_node ), node, s_node->other_wise );
    return( node );
}


static void DoBinarySearch( an node, const select_list *list, const type_def *tipe,
                               int lo, int hi, label_handle other,
                               const signed_64 *lobound, const signed_64 *hibound,
                               bool have_lobound, bool have_hibound )
/*************************************************************************/
{
    int                 num;
    int                 mid;
    const select_list   *mid_list;
    an                  cmp;
    label_handle        lt;
    unsigned_64         tmp;

    mid_list = list;
    num = mid = lo + ( hi - lo ) / 2;
    while( num-- > 0 ) {
        mid_list = mid_list->next;
    }
    if( lo == hi ) {
        if( have_lobound && U64isEq( *lobound, mid_list->low )
          && have_hibound && U64isEq( *hibound, mid_list->high ) ) {
             BGControl( O_GOTO, NULL, mid_list->label );
             return;
        } else if( U64isEq( mid_list->low, mid_list->high ) ) {
            cmp = BGCompare( O_EQ, BGDuplicate( node ),
                             BGIntegerSel( &mid_list->low, tipe ), NULL, tipe );
            BGControl( O_IF_TRUE, cmp, mid_list->label );
            BGControl( O_GOTO, NULL, other );
            return;
        }
    }
    if( hi == mid + 1 && U64isEq( mid_list->next->low, mid_list->next->high ) ) {
        /*
         * a linear sequence for three different non-sequential cases where
         * c1<c2<c3, looks like:
         *
         *   if( a == c3 ) goto l3;
         *   if( a == c2 ) goto l2;
         *   if( a != c1 ) goto default;
         * l1: ...
         *
         * a binary sequence for these three cases looks like:
         *   if( a < c2 goto lt;    \
         *   if( a <= c2 ) goto l2; /only one cmp ins on x86
         *   if( a == c3 ) goto l3;
         *   goto default;
         * lt:
         *   if( a != c1 ) goto default;
         * l1: ...
         *
         * Advantage of the linear search:
         *   3 goto's instead of 5, resulting in smaller code.
         *
         * Advantage of the binary search:
         *   Execution time for all the cases is more balanced. which one is
         * really faster depends a lot on the CPU's branch prediction and
         * other things that are very hard to measure here.
         *
         * Using a linear search here for <= 3 cases to save on code size
         * with negligible performance loss or gain.
         */
        mid_list = mid_list->next;
        cmp = BGCompare( O_EQ, BGDuplicate( node ),
                         BGIntegerSel( &mid_list->low, tipe ), NULL, tipe );
        BGControl( O_IF_TRUE, cmp, mid_list->label );
        /*
         * Because we only compared for equality, it is only possible to
         * decrease the upper bound if it was already set and equal to
         * the value we are comparing to. Otherwise the incoming value
         * may still be higher, where the inner call may produce an
         * unconditional O_GOTO to a specific case label!
         */
        tmp = *hibound;
        if( have_hibound && U64isEq( tmp, mid_list->low ) )
            U64Dec( tmp );
        DoBinarySearch( node, list, tipe, lo, mid, other,
                        lobound, &tmp, have_lobound, have_hibound );
        return;
    }
    lt = AskForNewLabel();
    if( !have_lobound || SelCompare( lobound, &mid_list->low ) < 0 ) {
        if( have_hibound && SelCompare( hibound, &mid_list->low ) < 0 ) {
            BGControl( O_GOTO, NULL, lt );
        } else {
            cmp = BGCompare( O_LT, BGDuplicate( node ),
                             BGIntegerSel( &mid_list->low, tipe ), NULL, tipe );
            BGControl( O_IF_TRUE, cmp, lt );
        }
    }
    if( !have_lobound || SelCompare( lobound, &mid_list->high ) <= 0 ) {
        if( have_hibound && SelCompare( hibound, &mid_list->high ) <= 0 ) {
            BGControl( O_GOTO, NULL, mid_list->label );
        } else {
            cmp = BGCompare( O_LE, BGDuplicate( node ),
                             BGIntegerSel( &mid_list->high, tipe ), NULL, tipe );
            BGControl( O_IF_TRUE, cmp, mid_list->label );
        }
    }
    if( mid < hi ) {
        tmp = mid_list->high;
        U64Inc( tmp );
        DoBinarySearch( node, list, tipe, mid + 1, hi, other,
                        &tmp, hibound, true, have_hibound );
    } else if( other != NULL ) {
        BGControl( O_GOTO, NULL, other );
    }
    BGControl( O_LABEL, NULL, lt );
    if( lo < mid ) {
        tmp = mid_list->low;
        U64Dec( tmp );
        DoBinarySearch( node, list, tipe, lo, mid - 1, other,
                        lobound, &tmp, have_lobound, true );
    } else if( other != NULL ) {
        BGControl( O_GOTO, NULL, other );
    }
}


static  an      GenIfStmts( an node, sel_handle s_node, const type_def *tipe )
/****************************************************************************/
{
    const select_list   *list;
    int                 nodes;
    signed_64           tmp;

    nodes = 0;
    for( list = s_node->list; list != NULL; list = list->next ) {
        ++nodes;
    }
    Set64ValZero( tmp );
    DoBinarySearch( node, s_node->list, tipe, 0, nodes - 1, s_node->other_wise,
                    &tmp, &tmp, false, false );
    return( node );
}


void    FreeTable( tbl_control *table )
/*************************************/
{
    CGFree( table );
}


static  void    FreeSelectNode( sel_handle s_node )
/*************************************************/
{
    select_list         *list;
    select_list         *next;

    for( list = s_node->list; list != NULL; list = next ) {
        next = list->next;
        CGFree( list );
    }
    CGFree( s_node );
}


void    BGSelect( sel_handle s_node, an node, cg_switch_type allowed )
/********************************************************************/
{
    cost_val    cost;
    cost_val    best;
    sel_kind    kind;
    unsigned_64 tmp;

    if( ( allowed & CG_SWITCH_ALL ) == 0 ) {
        _Zoiks( ZOIKS_090 );
        allowed = CG_SWITCH_ALL;
    }
    kind = 0;
    node = Arithmetic( node, TypeInteger );
    if( U64isntZero( s_node->num_cases ) ) {
        best = MAX_COST;
        /*
         * sort signed
         */
        SortNodeList( node, s_node, true );
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
        /*
         * sort unsigned
         */
        SortNodeList( node, s_node, false );
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
            /*
             * sort signed
             */
            SortNodeList( node, s_node, true );
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
        Set64ValZero( tmp );
        node = BGBinary( O_PLUS, node, BGIntegerSel( &tmp, SortTipe ), SortTipe, true );

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
