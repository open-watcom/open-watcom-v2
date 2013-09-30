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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "pattern.h"
#include "procdef.h"
#include "cgdefs.h"
#include "cgmem.h"
#include "model.h"
#include "ocentry.h"
#include "objrep.h"
#include "zoiks.h"
#include "cgaux.h"
#include "typedef.h"
#include "types.h"
#include "dbgstrct.h"
#include "feprotos.h"
#ifndef NDEBUG
#include "echoapi.h"
#endif

extern  seg_id          AskOP(void);
extern  name            *DeAlias(name*);
extern  name            *AllocUserTemp(pointer,type_class_def);
extern  seg_id          SetOP(seg_id);
extern  offset          AskLocation();
extern  void            DataInt(short_offset);
#if _TARGET & _TARG_IAPX86
extern  hw_reg_set      Low32Reg(hw_reg_set);
#elif _TARGET & _TARG_80386
extern  hw_reg_set      Low64Reg(hw_reg_set);
#endif
extern  void            DataBytes(unsigned_32,byte*);
extern  void            DoBigBckPtr(back_handle,offset);
extern  type_length     NewBase(name*);
extern  int             ParmsAtPrologue( void ) ;


static  dbg_loc         LocCreate( dbg_loc loc, unsigned typ ) {
/**************************************************************/

    dbg_loc     new;

    new = CGAlloc( sizeof( location ) );
    new->next = loc;
    new->class = typ;
    new->use = 1;
    return( new );
}

extern  dbg_loc         LocReg( dbg_loc loc, name *reg) {
/*******************************************************/

    loc = LocCreate( loc, LOC_REG );
    loc->u.be_sym = reg;
    return( loc );
}

extern dbg_loc          LocParm( dbg_loc loc, name *tmp ) {
/*****************************************************************/


    if( tmp->n.class == N_REGISTER  ){
        loc = LocCreate( loc, LOC_REG );
        loc->u.be_sym = tmp;
    }else{
        loc = LocCreate( loc, LOC_CONST_4 );
        loc->u.val = tmp->t.location;
        loc->u.val += ParmsAtPrologue();
    }
    return( loc );
}

extern  dbg_loc         LocDupl( dbg_loc loc ) {
/**********************************************/

    dbg_loc     first;

    first = loc;
    while( loc != NULL ) {
        loc->use++;
        loc = loc->next;
    }
    return( first );
}

#define NO_OFFSET (-1)

extern  offset          LocSimpField( dbg_loc loc ) {
/***************************************************/

    if( loc == NULL ) return( 0 );
    if( loc->class != LOC_OPER+LOP_ADD ) return( NO_OFFSET );
    loc = loc->next;
    if( loc == NULL ) return( NO_OFFSET );
    if( loc->next != NULL ) return( NO_OFFSET );
    if( loc->class != LOC_CONST_1 ) return( NO_OFFSET );
    return( loc->u.val );
}


extern  sym_handle      LocSimpStatic( dbg_loc loc ) {
/****************************************************/

    if( loc == NULL ) return( NULL );
    if( loc->next != NULL ) return( NULL );
    if( loc->class != LOC_MEMORY ) return( NULL );
    return( loc->u.fe_sym );
}


extern  dbg_loc  _CGAPI DBLocInit() {
/***********************************/
#ifndef NDEBUG
    dbg_loc retn;
    EchoAPI( "DBLocInit()" );
    retn = NULL;
    EchoAPI( " -> %i\n", retn );
    return( retn );
#else

    return( NULL );
#endif
}


extern dbg_loc  _CGAPI DBLocSym( dbg_loc loc,   sym_handle sym ) {
/****************************************************************/

    fe_attr     attr;
    name        *tmp;

    attr = FEAttr( sym );
    if( attr & FE_STATIC ) {
        loc = LocCreate( loc, LOC_MEMORY );
        loc->u.fe_sym = sym;
    } else {
        loc = LocCreate( loc, LOC_BP_OFFSET );
        tmp = DeAlias( AllocUserTemp( sym, XX ) );
        tmp->v.usage |= VAR_VOLATILE|NEEDS_MEMORY|USE_IN_ANOTHER_BLOCK|USE_ADDRESS;
        loc->u.be_sym = tmp;
    }
    return( loc );
}


extern dbg_loc _CGAPI DBLocTemp( dbg_loc loc, temp_handle sym ) {
/*****************************************************************/

    name        *tmp;

#ifndef NDEBUG
    EchoAPI( "DBLocTemp( %i, %i )", loc, sym );
#endif
    loc = LocCreate( loc, LOC_BP_OFFSET );
    tmp = DeAlias( sym );
    tmp->v.usage |= VAR_VOLATILE|NEEDS_MEMORY|USE_IN_ANOTHER_BLOCK|USE_ADDRESS;
    loc->u.be_sym = tmp;
#ifndef NDEBUG
    EchoAPI( " -> %i\n", loc );
#endif
    return( loc );
}

extern void             DBSetSymLoc(  sym_handle sym, long off ) {
/*****************************************************************/

    name        *tmp;

    tmp = DeAlias( AllocUserTemp( sym, XX ) );
    tmp->v.usage |= VAR_VOLATILE|NEEDS_MEMORY|USE_IN_ANOTHER_BLOCK|USE_ADDRESS;
    tmp->t.location = off;
}

extern  dbg_loc _CGAPI DBLocConst( dbg_loc loc, unsigned_32 val ) {
/******************************************************************/
#ifndef NDEBUG
    EchoAPI( "DBLocConst( %i, %i  )", loc, val );
#endif

    loc = LocCreate( loc, LOC_CONST_1 );
    loc->u.val = val;
#ifndef NDEBUG
    EchoAPI( " -> %i\n", loc );
#endif
    return( loc );
}


extern  dbg_loc _CGAPI DBLocOp(dbg_loc loc, dbg_loc_op op, unsigned other) {
/***************************************************************************/

    unsigned    stkop;

#ifndef NDEBUG
    EchoAPI( "DBLocOp( %i, %i, %i )", loc, op, other );
#endif
    stkop = 0;
    switch( op ) {
    case DB_OP_POINTS:
        switch( TypeAddress( other )->refno ) {
        #if _TARGET & _TARG_IAPX86
            case TY_NEAR_POINTER:
            case TY_NEAR_CODE_PTR:
        #endif
        case TY_UINT_2:
        case TY_INT_2:
            stkop = LOC_OPER+LOP_IND_2;
            break;
        #if !( _TARGET & _TARG_IAPX86 )
            case TY_NEAR_POINTER:
            case TY_NEAR_CODE_PTR:
        #endif
        case TY_UINT_4:
        case TY_INT_4:
            stkop = LOC_OPER+LOP_IND_4;
            break;
        case TY_LONG_POINTER:
        case TY_HUGE_POINTER:
        case TY_LONG_CODE_PTR:
            #if  _TARGET & _TARG_80386
                stkop = LOC_OPER+LOP_IND_ADDR386;
            #else
                stkop = LOC_OPER+LOP_IND_ADDR286;
            #endif
            break;
        default:
            Zoiks( ZOIKS_085 );
            break;
        }
        loc = LocCreate( loc, stkop );
        break;
    case DB_OP_ZEX:
        switch( TypeAddress( other )->length ) {
        case 1:
            stkop = LOC_OPER+LOP_ZEB;
            break;
        case 2:
            stkop = LOC_OPER+LOP_ZEW;
            break;
        default:
            Zoiks( ZOIKS_084 );
            break;
        }
        loc = LocCreate( loc, stkop );
        break;
    case DB_OP_XCHG:
        loc = LocCreate( loc, LOC_OPER+LOP_XCHG );
        loc->u.stk = other;
        break;
    case DB_OP_MK_FP:
        loc = LocCreate( loc, LOC_OPER+LOP_MK_FP );
        break;
    case DB_OP_ADD:
        loc = LocCreate( loc, LOC_OPER+LOP_ADD );
        break;
    case DB_OP_DUP:
        loc = LocCreate( loc, LOC_OPER+LOP_DUP );
        break;
    case DB_OP_POP:
        loc = LocCreate( loc, LOC_OPER+LOP_POP );
        break;
    default:
        Zoiks( ZOIKS_083 );
        break;
    }
#ifndef NDEBUG
    EchoAPI( " -> %i\n", loc );
#endif
    return( loc );
}


extern  void _CGAPI DBLocFini( dbg_loc loc ) {
/************************************************/

    dbg_loc     *owner;
    dbg_loc     curr;

#ifndef NDEBUG
    EchoAPI( "DBLocFini( %i )\n", loc );
#endif
    owner = &loc;
    for( ;; ) {
        curr = *owner;
        if( curr == NULL ) break;
        curr->use--;
        if( curr->use == 0 ) {
            *owner = curr->next;
            CGFree( curr );
        } else {
            owner = &curr->next;
        }
    }
}
