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


#include "as.h"

static expr_tree *etAlloc( et_class type ) {
/*******************************************
    Alloc an expression node of the given
    type. No fields are initialized.
*/
    expr_tree   *node;

    node = MemAlloc( sizeof( expr_tree ) );
    node->type = type;
    return( node );
}

static void etFree( expr_tree *node ) {
/**************************************
    Free an expression node.
*/

    MemFree( node );
}

extern expr_tree *ETReloc( asm_reloc_type rtype, sym_handle sym ) {
/******************************************************************
    Allocate a leaf node for a relocatable constant.
*/
    expr_tree   *node;

    node = etAlloc( ET_RELOCATABLE );
    RELOC_SYMBOL( node ) = sym;
    RELOC_TYPE( node ) = rtype;
    RELOC_DISP( node ) = 0;
    return( node );
}

extern expr_tree *ETNumLabelReloc( asm_reloc_type rtype, int_32 label_num ) {
/****************************************************************************
    Allocate a leaf node for a reference to a numeric label (eg. 2f)
*/
    expr_tree   *node;

    node = etAlloc( ET_UNNAMED_RELOCATABLE );
    RELOC_LABELNUM( node ) = label_num;
    RELOC_TYPE( node ) = rtype;
    RELOC_DISP( node ) = 0;
    return( node );
}

extern expr_tree *ETConst( signed_32 value ) {
/*********************************************
    Allocate a leaf node for an integer constant.
*/
    expr_tree   *node;

    node = etAlloc( ET_CONSTANT );
    CONST_VALUE( node ) = value;
    return( node );
}

extern expr_tree *ETFPConst( double value ) {
/********************************************
    Allocate a leaf node for an floating point constant.
*/
    expr_tree   *node;

    node = etAlloc( ET_FP_CONSTANT );
    FP_CONST_VALUE( node ) = value;
    return( node );
}

extern expr_tree *ETUnary( et_class type, expr_tree *child ) {
/*************************************************************
    Allocate a unary node of the given type.
*/
    expr_tree   *node;

    assert( _IsUnary( type ) );
    node = etAlloc( type );
    UNARY_CHILD( node ) = child;
    return( node );
}

extern expr_tree *ETBinary( et_class type, expr_tree *left, expr_tree *right ) {
/*******************************************************************************
    Allocate a binary node of the given type.
*/
    expr_tree   *node;

    assert( _IsBinary( type ) );
    node = etAlloc( type );
    BINARY_LEFT( node ) = left;
    BINARY_RIGHT( node ) = right;
    return( node );
}

static expr_tree *unaryFold( expr_tree *tree ) {
/***********************************************
    To burn down a unary node, we burn our child and
    replace ourselves with it's value if it turns out
    to be a leaf. Note that NOT and UNARY_MINUS are
    not allowed on a relocatable constant, but we still
    need to check for them and report errors.
*/
    expr_tree   *left;

    assert( _IsUnary( tree->type ) );
    left = ETBurn( UNARY_CHILD( tree ) );
    if( _IsLeaf( left->type ) ) {
        switch( tree->type ) {
        case ET_NOT:
            if( left->type == ET_CONSTANT ) {
                CONST_VALUE( tree ) = ~CONST_VALUE( left );
            } else if( left->type == ET_FP_CONSTANT ) {
                Error( ILLEGAL_UNARY_EXPR );
            } else {    // reloc
                Error( ILLEGAL_UNARY_EXPR );
                // need to house-keep
                RELOC_TARGET( tree ) = RELOC_TARGET( left );
                RELOC_TYPE( tree ) = RELOC_TYPE( left );
                RELOC_DISP( tree ) = ~RELOC_DISP( left );
            }
            tree->type = left->type;
            break;
        case ET_UNARY_MINUS:
            if( left->type == ET_CONSTANT ) {
                CONST_VALUE( tree ) = -CONST_VALUE( left );
            } else if( left->type == ET_FP_CONSTANT ) {
                FP_CONST_VALUE( tree ) = -FP_CONST_VALUE( left );
            } else {
                Error( ILLEGAL_UNARY_EXPR );
                // need to house-keep
                RELOC_TARGET( tree ) = RELOC_TARGET( left );
                RELOC_TYPE( tree ) = RELOC_TYPE( left );
                RELOC_DISP( tree ) = -RELOC_DISP( left );
            }
            tree->type = left->type;
            break;
        case ET_PARENS:
            tree->type = left->type;
            if( left->type == ET_FP_CONSTANT ) {
                FP_CONST_VALUE( tree ) = FP_CONST_VALUE( left );
                break;
            }
            CONST_VALUE( tree ) = CONST_VALUE( left );
            if( left->type == ET_RELOCATABLE || left->type == ET_UNNAMED_RELOCATABLE ) {
                RELOC_TARGET( tree ) = RELOC_TARGET( left );
                RELOC_TYPE( tree ) = RELOC_TYPE( left );
                RELOC_DISP( tree ) = RELOC_DISP( left );
            }
            break;
        default:
            Error( UNEXPECTED_UNARY_CLASS );
        }
        etFree( left );
    }
    return( tree );
}

static expr_tree *binaryFold( expr_tree *tree ) {
/************************************************
    Try and fold a binary node. This routine does
    not even try to handle leafs with relocatable
    consts in them, see relocFold instead.
*/
    expr_tree   *left;
    expr_tree   *right;

    assert( _IsBinary( tree->type ) );
    left = ETBurn( BINARY_LEFT( tree ) );
    right = ETBurn( BINARY_RIGHT( tree ) );
    if( _IsConstant( left->type ) && _IsConstant( right->type ) ) {
        if( left->type == ET_CONSTANT && right->type == ET_CONSTANT ) {
            signed_32   l_val, r_val, result;

            l_val = CONST_VALUE( left );
            r_val = CONST_VALUE( right );
            switch( tree->type ) {
            case ET_TIMES:
                result = l_val * r_val;
                break;
            case ET_DIVIDE:
                result = l_val / r_val;
                break;
            case ET_MOD:
                result = l_val % r_val;
                break;
            case ET_PLUS:
                result = l_val + r_val;
                break;
            case ET_MINUS:
                result = l_val - r_val;
                break;
            case ET_SHIFT_L:
                result = l_val << r_val;
                break;
            case ET_SHIFT_R:
                result = l_val >> r_val;
                break;
            case ET_OR:
                result = l_val | r_val;
                break;
            case ET_XOR:
                result = l_val ^ r_val;
                break;
            case ET_AND:
                result = l_val & r_val;
                break;
            default:
                Error( UNEXPECTED_BINARY_CLASS );
            }
            CONST_VALUE( tree ) = result;
            tree->type = ET_CONSTANT;
        } else {
            double      l_val, r_val, result;

            if( left->type == ET_CONSTANT ) {
                l_val = (double)CONST_VALUE( left );
            } else {
                l_val = FP_CONST_VALUE( left );
            }
            if( right->type == ET_CONSTANT ) {
                r_val = (double)CONST_VALUE( right );
            } else {
                r_val = FP_CONST_VALUE( right );
            }
            switch( tree->type ) {
            case ET_TIMES:
                result = l_val * r_val;
                break;
            case ET_DIVIDE:
                result = l_val / r_val;
                break;
            case ET_MOD:
                Error( ILLEGAL_BINARY_EXPR );
                break;
            case ET_PLUS:
                result = l_val + r_val;
                break;
            case ET_MINUS:
                result = l_val - r_val;
                break;
            case ET_SHIFT_L:
                Error( ILLEGAL_BINARY_EXPR );
                break;
            case ET_SHIFT_R:
                Error( ILLEGAL_BINARY_EXPR );
                break;
            case ET_OR:
                Error( ILLEGAL_BINARY_EXPR );
                break;
            case ET_XOR:
                Error( ILLEGAL_BINARY_EXPR );
                break;
            case ET_AND:
                Error( ILLEGAL_BINARY_EXPR );
                break;
            default:
                Error( UNEXPECTED_BINARY_CLASS );
            }
            FP_CONST_VALUE( tree ) = result;
            tree->type = ET_FP_CONSTANT;
        }
        etFree( left );
        etFree( right );
    }
    return( tree );
}

static expr_tree *relocFold( expr_tree *tree ) {
/***********************************************
    Any time we have a '+' or '-' node, one of the operands
    could be a relocatable constant. In order to handle this,
    all adds and subtracts come through this routine instead
    of binaryFold.
*/
    expr_tree   *left;
    expr_tree   *right;
    signed_32   factor;

    assert( tree->type == ET_PLUS || tree->type == ET_MINUS );
    factor = ( tree->type == ET_PLUS ? 1 : -1 );
    left = ETBurn( BINARY_LEFT( tree ) );
    right = ETBurn( BINARY_RIGHT( tree ) );
    if( _IsLeaf( left->type ) && _IsLeaf( right->type ) ) {
        if( left->type == ET_RELOCATABLE || left->type == ET_UNNAMED_RELOCATABLE ) {
            if( right->type == ET_CONSTANT ) {
                /* reloc +/- constant -> ok */
                RELOC_TARGET( tree ) = RELOC_TARGET( left );
                RELOC_TYPE( tree ) = RELOC_TYPE( left );
                RELOC_DISP( tree ) = RELOC_DISP( left );
                RELOC_DISP( tree ) += CONST_VALUE( right ) * factor;
                tree->type = left->type;
                etFree( left );
                etFree( right );
            } else {
                /* reloc +/- reloc -> ok if - */
                /* FIXME - need some symbol table magic here */
            }
        } else {
            if( right->type == ET_RELOCATABLE || right->type == ET_UNNAMED_RELOCATABLE ) {
                /* constant +/- reloc -> ok if + */
                if( tree->type == ET_PLUS ) {
                    RELOC_TARGET( tree ) = RELOC_TARGET( right );
                    RELOC_TYPE( tree ) = RELOC_TYPE( right );
                    RELOC_DISP( tree ) = RELOC_DISP( right );
                    RELOC_DISP( tree ) += CONST_VALUE( left );
                    tree->type = right->type;
                    etFree( left );
                    etFree( right );
                }
            } else {
                /* constant +- constant -> call binary fold */
                tree = binaryFold( tree );
            }
        }
    }
    return( tree );
}

extern expr_tree *ETBurn( expr_tree *tree ) {
/********************************************
    Burn down the given tree as much as possible, returning
    whatever is left. Ideally, we will be left with a leaf
    but this is not always possible (an unresolved reloc which
    was part of the "reloc - reloc -> abs" deal).
*/
    switch( tree->type ) {
    case ET_CONSTANT:
    case ET_FP_CONSTANT:
    case ET_UNNAMED_RELOCATABLE:
    case ET_RELOCATABLE:
        break;
    case ET_NOT:
    case ET_UNARY_MINUS:
    case ET_PARENS:
        tree = unaryFold( tree );
        break;
    case ET_PLUS:
    case ET_MINUS:
        tree = relocFold( tree );
        break;
    case ET_TIMES:
    case ET_MOD:
    case ET_DIVIDE:
    case ET_SHIFT_L:
    case ET_SHIFT_R:
    case ET_AND:
    case ET_OR:
    case ET_XOR:
        tree = binaryFold( tree );
        break;
    }
    return( tree );
}

extern void ETFree( expr_tree *tree ) {
/**************************************
    Free up the entire sub tree.
*/

    switch( tree->type ) {
    case ET_CONSTANT:
    case ET_FP_CONSTANT:
    case ET_UNNAMED_RELOCATABLE:
    case ET_RELOCATABLE:
        break;
    case ET_NOT:
    case ET_UNARY_MINUS:
    case ET_PARENS:
        ETFree( UNARY_CHILD( tree ) );
        break;
    case ET_PLUS:
    case ET_MINUS:
    case ET_TIMES:
    case ET_MOD:
    case ET_DIVIDE:
    case ET_SHIFT_L:
    case ET_SHIFT_R:
    case ET_AND:
    case ET_OR:
    case ET_XOR:
        ETFree( BINARY_LEFT( tree ) );
        ETFree( BINARY_RIGHT( tree ) );
        break;
    }
    etFree( tree );
}

#ifdef _STANDALONE_
#ifndef NDEBUG
static char *nodeNames[] = {
    "Constant",         /* ET_CONSTANT */
    "FP Constant",      /* ET_FP_CONSTANT */
    "Unnamed Reloc",    /* ET_UNNAMED_RELOCATABLE */
    "Relocation",       /* ET_RELOCATABLE */
    "Not",              /* ET_NOT */
    "Parens",           /* ET_PARENS */
    "Unary Minus",      /* ET_UNARY_MINUS */
    "Times",            /* ET_TIMES */
    "Mod",              /* ET_MOD */
    "Divide",           /* ET_DIVIDE */
    "Plus",             /* ET_PLUS */
    "Minus",            /* ET_MINUS */
    "Shift_l",          /* ET_SHIFT_L */
    "Shift_r",          /* ET_SHIFT_R */
    "And",              /* ET_AND */
    "Xor",              /* ET_XOR */
    "Or"                /* ET_OR */
};

static void outIndent( int level ) {
/***********************************
    Tab the text out a little ways.
*/
    while( level-- ) {
        printf( "    " );
    }
}

static char *relocString[] = {
    "ASM_RELOC_WORD",
    "ASM_RELOC_HALF_HI",
    "ASM_RELOC_HALF_HA",
    "ASM_RELOC_HALF_LO",
    "ASM_RELOC_JUMP"
};

static void doDump( expr_tree *tree, int level ) {
/*************************************************
    Print a node at the given level of the tree.
*/
    switch( tree->type ) {
    case ET_CONSTANT:
        outIndent( level );
        printf( "%s(%ld)\n", nodeNames[ tree->type ], CONST_VALUE( tree ) );
        break;
    case ET_FP_CONSTANT:
        outIndent( level );
        printf( "%s(%lf)\n", nodeNames[ tree->type ], FP_CONST_VALUE( tree ) );
        break;
    case ET_UNNAMED_RELOCATABLE:
        outIndent( level );
        printf( "%s(#%d,%s,%ld)\n", nodeNames[ tree->type ], RELOC_LABELNUM( tree ), relocString[RELOC_TYPE( tree )], RELOC_DISP( tree ) );
        break;
    case ET_RELOCATABLE:
        outIndent( level );
        printf( "%s(%lx,%s,%ld)\n", nodeNames[ tree->type ], RELOC_SYMBOL( tree ), relocString[RELOC_TYPE( tree )], RELOC_DISP( tree ) );
        break;
    case ET_NOT:
    case ET_UNARY_MINUS:
    case ET_PARENS:
        outIndent( level );
        printf( "%s\n", nodeNames[ tree->type ] );
        doDump( UNARY_CHILD( tree ), level + 1 );
        break;
    default:
        doDump( BINARY_RIGHT( tree ), level + 1 );
        outIndent( level );
        printf( "%s\n", nodeNames[ tree->type ] );
        doDump( BINARY_LEFT( tree ), level + 1 );
        break;
    }
}

extern void ETDump( expr_tree *tree ) {
/**************************************
    Dump a copy of the tree to stdout.
*/
    doDump( tree, 0 );
    fflush( stdout );
}
#endif
#endif
