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
* Description:  Assembler instruction operand parsing.
*
****************************************************************************/


#include "as.h"

static void setConstant( ins_operand *op, signed_32 c )
//*****************************************************
{
    op->constant = c;
    op->flags |= CONSTANT;
}

static void setUnnamedReloc( ins_operand *op, int_32 num, asm_reloc_type rtype )
//******************************************************************************
{
    op->reloc.target.label = num;
    op->reloc.type = rtype;
    op->flags |= UNNAMED_RELOC;
}

static void setReloc( ins_operand *op, sym_handle reloc, asm_reloc_type rtype )
//*****************************************************************************
{
    op->reloc.target.ptr = reloc;
    op->reloc.type = rtype;
    op->flags |= RELOC;
}

static ins_operand *initOp( void )
//********************************
{
    ins_operand *op;

    op = MemAlloc( sizeof( ins_operand ) );
    op->flags = NOTHING;
    op->reloc.target.ptr = NULL;
    op->constant = 0;
    op->reg = NULL_REG;
    return( op );
}

#define _Munge( a, b )          ((((int)(a)&0x0f)<<8)|((int)(b)&0x0f))

static void opConstant( ins_operand *op, expr_tree *expr )
//********************************************************
// If we can't create a valid operand from this expression
// tree, we use a constant of 0 and emit an error message.
{
    expr_tree   *l;
    expr_tree   *r;

    if( expr == NULL ) return;
    expr = ETBurn( expr );
    switch( expr->type ) {
    case ET_CONSTANT:
        setConstant( op, CONST_VALUE( expr ) );
        break;
    case ET_UNNAMED_RELOCATABLE:
        setUnnamedReloc( op, RELOC_LABELNUM( expr ), RELOC_TYPE( expr ) );
        setConstant( op, RELOC_DISP( expr ) );
        break;
    case ET_RELOCATABLE:
        setReloc( op, RELOC_SYMBOL( expr ), RELOC_TYPE( expr ) );
        setConstant( op, RELOC_DISP( expr ) );
        break;
    case ET_MINUS:
        r = BINARY_RIGHT( expr );
        l = BINARY_LEFT( expr );
        switch( _Munge( l->type, r->type ) ) {
        case _Munge( ET_RELOCATABLE, ET_CONSTANT ):
            setReloc( op, RELOC_SYMBOL( l ), RELOC_TYPE( l ) );
            setConstant( op, RELOC_DISP( l ) - CONST_VALUE( r ) );
            break;
        case _Munge( ET_CONSTANT, ET_CONSTANT ):
            Warning( INTERNAL_ETBURN_FAILED );
            setConstant( op, CONST_VALUE( l ) - CONST_VALUE( r ) );
            break;
        case _Munge( ET_CONSTANT, ET_RELOCATABLE ):
        default:
            Error( IMPROPERLY_FORMED_INSOP );
        }
        break;
    case ET_PLUS:
        r = BINARY_RIGHT( expr );
        l = BINARY_LEFT( expr );
        switch( _Munge( l->type, r->type ) ) {
        case _Munge( ET_CONSTANT, ET_RELOCATABLE ):
            setReloc( op, RELOC_SYMBOL( r ), RELOC_TYPE( r ) );
            setConstant( op, CONST_VALUE( l ) + RELOC_DISP( r ) );
            break;
        case _Munge( ET_RELOCATABLE, ET_CONSTANT ):
            setReloc( op, RELOC_SYMBOL( l ), RELOC_TYPE( l ) );
            setConstant( op, CONST_VALUE( r ) + RELOC_DISP( l ) );
            break;
        case _Munge( ET_CONSTANT, ET_CONSTANT ):
            Warning( INTERNAL_ETBURN_FAILED );
            setConstant( op, CONST_VALUE( l ) + CONST_VALUE( r ) );
            break;
        default:
            Error( IMPROPERLY_FORMED_INSOP );
        }
        break;
    default:
        Error( IMPROPERLY_FORMED_INSOP );
    }
    ETFree( expr );
}

extern ins_operand *OpImmed( expr_tree *expr )
//********************************************
{
    ins_operand *op;

    assert( expr != NULL );
    op = initOp();
    op->type = OP_IMMED;
    opConstant( op, expr );
    return( op );
}

static op_type RegClassOpTypes[] = {
    #define PICK( a, b )        OP_##a,
    #include "regclass.inc"
    #undef PICK
    0
};

static void atRegCheck( reg r )
//*****************************
{
    if( r == AT_REG && _DirIsSet( AT ) ) {
        Warning( AT_REG_USED_WITHOUT_REQ );
    }
}

extern ins_operand *OpRegister( reg r )
//*************************************
{
    ins_operand *op;

    assert( RegClass( r ) >= 0 && RegClass( r ) < RC_LAST );
    op = initOp();
    op->type = RegClassOpTypes[ RegClass( r ) ];
    op->reg = r;
    atRegCheck( r );
    return( op );
}

extern ins_operand *OpRegIndirect( reg r, expr_tree *expr )
//*********************************************************
{
    ins_operand *op;

    assert( RegClass( r ) >= 0 && RegClass( r ) < RC_LAST );
    op = initOp();
    op->type = OP_REG_INDIRECT;
    op->reg = r;
    atRegCheck( r );
    if( expr != NULL ) {
        opConstant( op, expr );
    }
    return( op );
}

#ifdef AS_PPC
extern ins_operand *OpBI( uint_32 cr, uint_32 bit_offset )
//********************************************************
// This operand type specifies which of the 32 bits in the CR
// represents the condition to test. (PPC specific)
{
    ins_operand *op;

    assert( bit_offset <= BI_LAST );
    op = initOp();
    op->type = OP_BI;
    setConstant( op, cr + bit_offset );
    return( op );
}
#endif

#ifdef _STANDALONE_
#ifdef AS_DEBUG_DUMP
static char *reloc_modifier[] = {
    "",
    "",
    "h^",
    "ha^",
    "l^",
    "j^",
    "b^"
};

extern void DumpOperand( ins_operand *op )
//****************************************
{
    switch( op->type ) {
    case OP_IMMED:
    case OP_REG_INDIRECT:
#ifdef AS_PPC
    case OP_BI:
#endif
        if( ( op->flags & (RELOC|CONSTANT) ) == (RELOC|CONSTANT) ) {
            printf( "%s%s+%d", reloc_modifier[op->reloc.type], ((sym_handle)op->reloc.target.ptr)->name, op->constant );
        } else if( ( op->flags & (UNNAMED_RELOC|CONSTANT) ) == (UNNAMED_RELOC|CONSTANT) ) {
            printf( "%s\"%d\"+%d", reloc_modifier[op->reloc.type], op->reloc.target.label, op->constant );
        } else if( op->flags & CONSTANT ) {
            printf( "%d", op->constant );
        } else if( op->flags & RELOC ) {
            printf( "%s%s", reloc_modifier[op->reloc.type], ((sym_handle)op->reloc.target.ptr)->name );
        } else if( op->flags & UNNAMED_RELOC ) {
            printf( "%s\"%d\"", reloc_modifier[op->reloc.type], op->reloc.target.label );
        } else if( op->type == OP_IMMED ) {
            printf( "*strange-constant*" );
        }
        if( op->type == OP_REG_INDIRECT ) {
            printf( "(%s)", RegName( op->reg ) );
        }
        break;
    default:
        printf( "%s", RegName( op->reg ) );
        break;
    }
}
#endif
#endif
