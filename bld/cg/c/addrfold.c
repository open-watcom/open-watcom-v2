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


#include <stdlib.h>
#include "cgstd.h"
#include "coderep.h"
#include "model.h"
#include "addrname.h"
#include "procdef.h"
#include "zoiks.h"
#include "freelist.h"
#include "cfloat.h"
#include "x87.h"
#include "makeins.h"
#include "makeaddr.h"
#include "addrfold.h"
#include "namelist.h"

extern  type_class_def  TypeClass(type_def*);
extern  void            AddIns(instruction*);
extern  name            *AllocIndex(name*,name*,type_length,type_class_def);
extern  name            *ScaleIndex(name*,name*,type_length,type_class_def,type_length,int,i_flags);
extern  cg_type         NamePtrType(name*);
extern  void            BGDone(an);
extern  i_flags         AlignmentToFlags( type_length );

static  void    LoadTempInt( an addr );

typedef enum {
        /* actions which require further actions*/

        UNEXPECTED,     /* unexpected mode -- two addresses*/
        TEMP_L,         /* move value of left into temp*/
        TEMP_R,         /* move value of right into temp*/
#if WORD_SIZE == 2
        SHRNK_L,        /* convert left from I4 to I2*/
        SHRNK_R,        /* convert left from I4 to I2*/
#endif
        XPAND_L,        /* expand left from I2 to I4*/
        XPAND_R,        /* expand right from I2 to I4*/
#if WORD_SIZE == 2
        XP_LC,          /* expand left & move constant*/
        XP_RC,          /* expand right & move constant*/
#endif

        /* actions which result in a finished address*/

        IL,             /* index by left temporary*/
        IR,             /* index by right temporary*/
        ADD_LA,         /* add right constant to left address*/
        ADD_RA,         /* add left constant to right address*/
        ADD_LI,         /* add right index to left*/
        ADD_RI,         /* add left index to right*/
        ADD_I,          /* add integers, creating*/
        CODE            /* generate normal code*/
} mode_action;

#if WORD_SIZE == 2
    #define CL_VALUE            CL_VALUE2
#else
    #define CL_VALUE            CL_VALUE4
#endif

#define ___     UNEXPECTED

#define _HaveMode( action )     ( (action) >= IL )

static  mode_action     AddTable[][9] = {
/***************************************/
/*                       left argument*/
#if WORD_SIZE == 2
/* &g+C  &g+C[I] &t+C    &t+C[I] &X->C   TI2+C   I2      I4      C2*/
{___,    ___,    ___,    ___,    ___,    IL,     TEMP_L, SHRNK_L,ADD_RA},//&g+C
{___,    ___,    ___,    ___,    ___,    ADD_RI, ADD_RI, SHRNK_L,ADD_RA},//&g+Ci
{___,    ___,    ___,    ___,    ___,    IL,     TEMP_L, SHRNK_L,ADD_RA},//&t+C
{___,    ___,    ___,    ___,    ___,    ADD_RI, ADD_RI, SHRNK_L,ADD_RA},//&t+Ci
{___,    ___,    ___,    ___,    ___,    ADD_RI, ADD_RI, SHRNK_L,ADD_RA},//&X->C
{IR,     ADD_LI, IR,     ADD_LI, ADD_LI, ADD_I,  ADD_I,  XPAND_R,ADD_RA},//TI2+C
{TEMP_R, ADD_LI, TEMP_R, ADD_LI, ADD_LI, ADD_I,  CODE,   XPAND_R,TEMP_R},//I2
{SHRNK_R,SHRNK_R,SHRNK_R,SHRNK_R,SHRNK_R,XPAND_L,XPAND_L,CODE,   CODE},  //I4
{ADD_LA, ADD_LA, ADD_LA, ADD_LA, ADD_LA, ADD_LA, TEMP_L, CODE,   ADD_LA} //C2
#else
/* &g+C  &g+C[I] &t+C    &t+C[I] &X->C   TI4+C   I4      I2      C2*/
{___,    ___,    ___,    ___,    ___,    IL,     TEMP_L, XPAND_L,ADD_RA},//&g+C
{___,    ___,    ___,    ___,    ___,    ADD_RI, ADD_RI, XPAND_L,ADD_RA},//&g+Ci
{___,    ___,    ___,    ___,    ___,    IL,     TEMP_L, XPAND_L,ADD_RA},//&t+C
{___,    ___,    ___,    ___,    ___,    ADD_RI, ADD_RI, XPAND_L,ADD_RA},//&t+Ci
{___,    ___,    ___,    ___,    ___,    ADD_RI, ADD_RI, XPAND_L,ADD_RA},//&X->C
{IR,     ADD_LI, IR,     ADD_LI, ADD_LI, ADD_I,  ADD_I,  XPAND_L,ADD_RA},//TI4+C
{TEMP_R, ADD_LI, TEMP_R, ADD_LI, ADD_LI, ADD_I,  CODE,   CODE,   TEMP_R},//I4
{XPAND_R,XPAND_R,XPAND_R,XPAND_R,XPAND_R,XPAND_R,CODE,   CODE,   CODE},  //I2
{ADD_LA, ADD_LA, ADD_LA, ADD_LA, ADD_LA, ADD_LA, TEMP_L, CODE,   ADD_LA} //C2
#endif
};

/*
 * This is a kludge. I was too lazy to rearrange row and column in the
 * tables so I just used this table to map CL constants onto the tables
 */

static  byte    Idx[] = {
        0,        /* CL_ADDR_GLOBAL */
        2,        /* CL_ADDR_TEMP */
        4,        /* CL_POINTER */
#if WORD_SIZE == 2
        6,        /* CL_VALUE2 */
        7,        /* CL_VALUE4 */
#else
        7,        /* CL_VALUE2 */
        6,        /* CL_VALUE4 */
#endif
        8,        /* CL_CONS2 */
#if WORD_SIZE == 2
        0xFF,     /* CL_CONS4 */
#else
        8,        /* CL_CONS4 */
#endif
        1,        /* CL_GLOBAL_INDEX */
        3,        /* CL_TEMP_INDEX */
        5         /* CL_TEMP_OFFSET */
};

extern  name    *Points( an addr, type_def *tipe ) {
/**************************************************/

    type_class_def      class;
    type_length         size;
    name                *result;
    i_flags             flags;

    CheckPointer( addr );
    if( addr->format != NF_ADDR && addr->format != NF_CONS ) {
        _Zoiks( ZOIKS_024 );
    }
    class = TypeClass( tipe );
    size = tipe->length;
    if( addr->u.n.base != NULL ) {
        flags = X_FAKE_BASE;
    } else {
        flags = EMPTY;
    }
    if( addr->flags & FL_VOLATILE ) {
        flags |= X_VOLATILE;
    }
    if( addr->flags & FL_CONSTANT ) {
        flags |= X_CONSTANT;
    }
    if( addr->u.n.alignment != 0 ) {
        flags |= AlignmentToFlags( addr->u.n.alignment );
    }
    switch( addr->class ) {
    case CL_ADDR_GLOBAL:
        if( addr->u.n.name->v.offset == addr->u.n.offset
          && addr->u.n.name->n.name_class == class
          && addr->u.n.name->n.size == size ) {
            result = addr->u.n.name;
        } else {
            result = (name *)SAllocMemory( addr->u.n.name->v.symbol,
                                   addr->u.n.offset,
                                   addr->u.n.name->m.memory_type,
                                   class, size );
        }
        if( addr->u.n.alignment != 0 ) {
            // NOTE: This means all references to a memory operand must
            // have the same alignment in a function (be conservative)
            result->m.alignment = addr->u.n.alignment;
        }
        break;
    case CL_ADDR_TEMP:
        if( addr->u.n.name->v.offset == addr->u.n.offset
          && addr->u.n.name->n.name_class == class
          && addr->u.n.name->n.size == size ) {
            result = addr->u.n.name;
        } else {
            result = STempOffset( addr->u.n.name, addr->u.n.offset, class, size );
        }
        break;
    case CL_POINTER:
        result = ScaleIndex( addr->u.n.index, addr->u.n.base, addr->u.n.offset, class, size, 0, flags );
        break;
    case CL_GLOBAL_INDEX:
    case CL_TEMP_INDEX:
        result = ScaleIndex( addr->u.n.index, addr->u.n.name, addr->u.n.offset, class, size, 0, flags & ~X_FAKE_BASE );
        break;
    case CL_TEMP_OFFSET:
        result = ScaleIndex( addr->u.n.name, addr->u.n.base, addr->u.n.offset, class, size, 0, flags );
        break;
    case CL_CONS2:
#if WORD_SIZE != 2
    case CL_CONS4:
#endif
        addr->u.n.name = AllocIntConst( addr->u.n.offset );
        addr->class = CL_VALUE;
        LoadTempInt( addr );
        result = ScaleIndex( addr->u.n.name, addr->u.n.base, 0, class, size, 0, flags );
        break;
    default:
        LoadTempInt( addr );
        result = ScaleIndex( addr->u.n.name, addr->u.n.base, 0, class, size, 0, flags );
        break;
    }
    if( addr->flags & FL_VOLATILE ) {
        if( result->n.class != N_INDEXED ) {
            result->v.usage |= ( VAR_VOLATILE | USE_MEMORY | NEEDS_MEMORY );
        }
    }
    if( addr->flags & FL_CONSTANT ) {
        if( result->n.class != N_INDEXED ) {
            result->v.usage |= VAR_CONSTANT;
        }
    }
    return( result );
}


extern  an      AddrName( name *op, type_def *tipe ) {
/******************************************************/

    an  addr;

    addr = NewAddrName();
    addr->tipe = tipe;
    addr->u.n.name = op;
    if( op->n.class == N_CONSTANT && op->c.const_type == CONS_ABSOLUTE ) {
        addr->format = NF_CONS;
        if( CFIsI16( op->c.value ) && tipe->length <= WORD_SIZE ) {
            addr->class = CL_CONS2;
            addr->u.n.offset = CFCnvF16( op->c.value );
        } else {
            addr->class = CL_VALUE4;
            addr->u.n.offset = 0;
#if WORD_SIZE >= 4
            if( CFIsI32( op->c.value ) ) {
                addr->class = CL_CONS4;
                addr->u.n.offset = CFCnvF32( op->c.value );
            }
#endif
        }
    } else {
        if( tipe->attr & TYPE_POINTER ) {
            addr->u.n.name = op;
            addr->format = NF_NAME;
        } else if( tipe->length == WORD_SIZE ) {
            addr->u.n.name = op;
            if( op->n.class == N_TEMP ) {
                addr->class = CL_TEMP_OFFSET;
            } else {
                addr->class = CL_VALUE;
            }
#if WORD_SIZE == 2
        } else if( tipe->length == 4 ) {
            addr->u.n.name = op;
            addr->class = CL_VALUE4;
#endif
        } else {
            addr->format = NF_NAME;
        }
    }
    return( addr );
}


static  void    FixIndexClass( an addr ) {
/***********************************/

    if( addr->u.n.name->n.class == N_TEMP ) {
        addr->class = CL_TEMP_INDEX;
    } else {
        addr->class = CL_GLOBAL_INDEX;
    }
}


static  void    LoadTempInt( an addr ) {
/**************************************/

    addr->u.n.name = LoadTemp( addr->u.n.name, TypeClass( addr->tipe ) );
    addr->u.n.index = NULL;
    addr->u.n.offset = 0;
    if( addr->class == CL_VALUE ) {
        addr->class = CL_TEMP_OFFSET;
    }
}


static  void    AddIndex( an addr, name *index, name *addend ) {
/**************************************************************/

    instruction         *ins;
    type_class_def      class;

    class = index->n.name_class;
    if( class == XX )
        class = addend->n.name_class; /* if index is a const ! */
    ins = MakeBinary( OP_ADD, index, addend, AllocTemp( class ), class );
    index = ins->result;
    AddIns( ins );
    addr->u.n.index = index;
}


extern  bool    CypAddrPlus( an l_addr, an r_addr, type_def *tipe ) {
/*******************************************************************/

    mode_action action;

    if( tipe->refno == TY_HUGE_POINTER )
        return( false );
    CheckPointer( l_addr );
    if( l_addr->format == NF_NAME )
        return( false );
    CheckPointer( r_addr );
    if( r_addr->format == NF_NAME )
        return( false );
    action = AddTable[ Idx[ r_addr->class ] ][ Idx[ l_addr->class ] ];
    if( action == ADD_LA )
        return( true );
    if( action == ADD_RA )
        return( true );
#if WORD_SIZE == 2
    if( action == SHRNK_R )
        return( true );
    if( action == SHRNK_L )
        return( true );
#else
    if( action == XPAND_R )
        return( true );
    if( action == XPAND_L )
        return( true );
#endif
    return( false );
}


static        bool    AddToTypeLength( type_length x, type_length y ) {
/*********************************************************************/

    signed_32 lx,ly;

    lx = x; ly = y;
    x += y; lx += ly;
    if( lx != x ) return( false );
#if _TARGET & _TARG_370
    if( lx < 0 || lx >= 4096 )
        return( false );
#endif
    return( true );
}


extern  an      AddrPlus( an l_addr, an r_addr, type_def *tipe ) {
/****************************************************************/

/*       Returns NULL if code needs to be generated*/

    mode_action action;
    an          addr;

    CheckPointer( l_addr );
    CheckPointer( r_addr );
    if( !AddToTypeLength( l_addr->u.n.offset, r_addr->u.n.offset ) )
        return( NULL );
    if( l_addr->format == NF_NAME )
        return( NULL );
    if( r_addr->format == NF_NAME )
        return( NULL );
    if( tipe->refno == TY_HUGE_POINTER )
        return( NULL );
    addr = NewAddrName();
    for(;;) {
        action = AddTable[ Idx[ r_addr->class ] ][ Idx[ l_addr->class ] ];
        switch( action ) {
        case UNEXPECTED:
            AddrFree( addr );
            return( NULL );
        case TEMP_L:
            LoadTempInt( l_addr );
            break;
        case TEMP_R:
            LoadTempInt( r_addr );
            break;
#if WORD_SIZE == 2
        case SHRNK_L:
            Convert( l_addr, I2 );
            l_addr->class = CL_TEMP_OFFSET;
            break;
        case SHRNK_R:
            Convert( r_addr, I2 );
            r_addr->class = CL_TEMP_OFFSET;
            break;
#endif
        case XPAND_L:
            Convert( l_addr, I4 );
            l_addr->class = CL_VALUE4;
            break;
        case XPAND_R:
            Convert( r_addr, I4 );
            r_addr->class = CL_VALUE4;
            break;
#if WORD_SIZE == 2
        case XP_LC:
            r_addr->u.n.offset += l_addr->u.n.offset;
            l_addr->u.n.offset = 0;
            Convert( l_addr, I4 );
            l_addr->class = CL_VALUE4;
            break;
        case XP_RC:
            l_addr->u.n.offset += r_addr->u.n.offset;
            r_addr->u.n.offset = 0;
            Convert( r_addr, I4 );
            r_addr->class = CL_VALUE4;
            break;
#endif
        case IL:
            MoveAddress( r_addr, addr );
            addr->u.n.index = l_addr->u.n.name;
            addr->u.n.offset += l_addr->u.n.offset;
            FixIndexClass( addr );
            break;
        case IR:
            MoveAddress( l_addr, addr );
            addr->u.n.index = r_addr->u.n.name;
            addr->u.n.offset += r_addr->u.n.offset;
            FixIndexClass( addr );
            break;
        case ADD_LA:
            MoveAddress( l_addr, addr );
            addr->u.n.offset += r_addr->u.n.offset;
            break;
        case ADD_RA:
            MoveAddress( r_addr, addr );
            addr->u.n.offset += l_addr->u.n.offset;
            break;
        case ADD_LI:
            if( PointLess( l_addr, r_addr ) ) {
                AddrFree( addr );
                return( NULL );
            }
            MoveAddress( l_addr, addr );
            addr->u.n.offset += r_addr->u.n.offset;
            AddIndex( addr, l_addr->u.n.index, r_addr->u.n.name );
            break;
        case ADD_RI:
            if( PointLess( l_addr, r_addr ) ) {
                AddrFree( addr );
                return( NULL );
            }
            MoveAddress( r_addr, addr );
            addr->u.n.offset += l_addr->u.n.offset;
            AddIndex( addr, r_addr->u.n.index, l_addr->u.n.name );
            break;
        case ADD_I:
            if( PointLess( l_addr, r_addr ) ) {
                AddrFree( addr );
                return( NULL );
            }
            addr->u.n.offset = l_addr->u.n.offset + r_addr->u.n.offset;
            addr->class = CL_TEMP_OFFSET;
            AddIndex( addr, l_addr->u.n.name, r_addr->u.n.name );
            addr->u.n.name = addr->u.n.index;
            addr->u.n.index = NULL;
            break;
        case CODE:
            AddrFree( addr );
            return( NULL );
        }
        if( _HaveMode( action ) ) {
            break;
        }
    }
    BGDone( l_addr );
    BGDone( r_addr );
    addr->tipe = tipe;
    return( addr );
}


static  bool    ShiftConst( an r_addr ) {
/***************************************/

    if( r_addr->format != NF_CONS )
        return( false );
    if( r_addr->class != CL_CONS2 )
        return( false );
#if WORD_SIZE == 2
    if( r_addr->u.n.offset < 0 || r_addr->u.n.offset > 16 )
        return( false );
    if( _IsTargetModel( BIG_DATA ) && _IsntTargetModel( CHEAP_POINTER ) )
        return( false );
#else
    if( r_addr->u.n.offset < 0 || r_addr->u.n.offset > 8 )
        return( false );
#endif
    return( true );
}


extern  bool    CypAddrShift( an l_addr, an r_addr, type_def *tipe ) {
/********************************************************************/

    tipe = tipe;
    CheckPointer( l_addr );
    if( l_addr->format == NF_NAME )
        return( false );
    if( l_addr->class != CL_TEMP_OFFSET )
        return( false );
    if( l_addr->u.n.offset == 0 )
        return( false );
    if( !ShiftConst( r_addr ) )
        return( false );
    return( true );
}


static  bool    ShiftToTypeLength( type_length x, unsigned_16 y ) {
/*****************************************************************/

    signed_32 lx;

    lx = x;
    x <<= y; lx <<= y;
    if( lx != x )
        return( false );
#if _TARGET & _TARG_370
    if( lx < 0 || lx >= 4096 )
        return( false );
#endif
    return( true );
}


extern  an      AddrShift( an l_addr, an r_addr, type_def *tipe ) {
/*****************************************************************/

/*       Returns NULL if code needs to be generated*/

    instruction         *ins;
    signed_16           rv;
    an                  addr;
    type_class_def      class;

    addr = NULL;
    if( !ShiftConst( r_addr ) )
        return( NULL );
    CheckPointer( l_addr );
    if( l_addr->format == NF_NAME )
        return( NULL );
    if( l_addr->class != CL_TEMP_OFFSET )
        return( NULL );
    if( l_addr->u.n.offset == 0 )
        return( NULL );
    rv = r_addr->u.n.offset;
    if( !ShiftToTypeLength( l_addr->u.n.offset, rv ) )
        return( NULL );
    addr = NewAddrName();
    addr->u.n.offset = l_addr->u.n.offset << rv;  /* new constant after shift*/
    class = TypeClass( tipe );
    ins = MakeBinary( OP_LSHIFT, l_addr->u.n.name,
            AllocIntConst( rv ), AllocTemp( class ), class );
    addr->u.n.name = ins->result;
    AddIns( ins );
    addr->class = CL_TEMP_OFFSET;
    BGDone( l_addr );
    BGDone( r_addr );
    addr->tipe = tipe;
    return( addr );
}


extern  name    *GetValue( an addr, name *suggest ) {
/***************************************************/

    name        *op = NULL;
    instruction *ins;

    InsToAddr( addr );
    if( addr->format == NF_CONS || addr->format == NF_NAME ) {
        op = addr->u.n.name;
    } else if( addr->format == NF_ADDR ) {
        switch( addr->class ) {
        case CL_VALUE2:
        case CL_VALUE4:
            op = addr->u.n.name;
            break;
        case CL_TEMP_OFFSET:
            if( addr->u.n.offset != 0 ) {
                op = MaybeTemp( suggest, addr->u.n.name->n.name_class );
                ins = MakeBinary( OP_ADD, addr->u.n.name,
                                    AllocS32Const( addr->u.n.offset ),
                                    op, TypeClass( addr->tipe ) );
                if( addr->flags & FL_ADDR_DEMOTED ) ins->ins_flags |= INS_DEMOTED;
                AddIns( ins );
            } else {
                op = addr->u.n.name;
            }
            break;
        case CL_ADDR_GLOBAL:
            op = AllocMemory( addr->u.n.name->v.symbol,
                     addr->u.n.offset, addr->u.n.name->m.memory_type, XX );
            op = LoadAddress( op, suggest, addr->tipe );
            break;
        case CL_ADDR_TEMP:
            op = TempOffset( addr->u.n.name, addr->u.n.offset, XX );
            op = LoadAddress( op, suggest, addr->tipe );
            break;
        case CL_GLOBAL_INDEX:
        case CL_TEMP_INDEX:
            op = AllocIndex( addr->u.n.index, addr->u.n.name,
                                addr->u.n.offset, XX );
            op = LoadAddress( op, suggest, addr->tipe );
            break;
        case CL_POINTER:
            op = AllocIndex( addr->u.n.index, addr->u.n.name,
                                addr->u.n.offset, XX );
            op = LoadAddress( op, suggest, addr->tipe );
            break;
        default:
            break;
        }
    } else if( addr->format == NF_BOOL ) {
        _Zoiks( ZOIKS_025 );
    }
    if( op->n.class == N_TEMP
     && op->v.symbol == NULL
     && ( addr->flags & FL_STACKABLE )
     && (addr->flags & FL_NEVER_STACK) == 0 ) {
        FPSetStack( op );
    }
    return( op );
}


extern  name    *AddrToName( an addr ) {
/**************************************/

    name        *op;

    op = NULL;
    if( addr->format == NF_CONS || addr->format == NF_NAME ) {
        op = addr->u.n.name;
    } else if( addr->format == NF_ADDR ) {
        switch( addr->class ) {
        case CL_VALUE2:
        case CL_VALUE4:
            op = addr->u.n.name;
            break;
        case CL_TEMP_OFFSET:
            if( addr->u.n.offset == 0 ) {
                op = addr->u.n.name;
            }
            break;
        default:
            break;
        }
    }
    return( op );
}
