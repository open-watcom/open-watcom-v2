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
* Description:  Functions to manipulate the expression tree.
*
****************************************************************************/


#include "standard.h"
#include "coderep.h"
#include "addrname.h"
#include "model.h"
#include "cgdefs.h"
#include "procdef.h"
#include "tree.h"
#include "hostsys.h"
#include "zoiks.h"
#include "opcodes.h"
#include "freelist.h"
#include "cfloat.h"
#include "targsys.h"
#include "cgaux.h"
#include "feprotos.h"
#include "cgprotos.h"
#ifndef NDEBUG
    #include "echoapi.h"
#endif

#include "addrfold.h"
#include "bgcall.h"
#include "bldcall.h"
#include "bldins.h"
#include "blips.h"
#include "data.h"
#include "display.h"
#include "foldins.h"
#include "inline.h"
#include "makeaddr.h"
#include "namelist.h"
#include "optask.h"
#include "patch.h"
#include "rgtbl.h"
#include "stack.h"
#include "treeprot.h"
#include "treeconv.h"
#include "treefold.h"
#include "typemap.h"
#include "types.h"
#include "i64.h"

#if _TARGET & ( _TARG_80386 | _TARG_IAPX86 )
    #include "i86segs.h"
#endif

static  void    FreeTreeNode( tn node );
static  void    Control( cg_op op, tn node, label_handle lbl, bool gen );

static  pointer *TreeFrl;

static type_class_def BinMat[] = {
/*********************************
    What is the result type of a binary operation
*/
/*  U1 I1 U2 I2 U4 I4 U8 I8 CP PT FS FD FL*/
    U1,U1,U2,I2,U4,I4,U8,I8,CP,PT,FS,FD,FL,/* U1*/
    U1,I1,U2,I2,U4,I4,U8,I8,CP,PT,FS,FD,FL,/* I1*/
    U2,U2,U2,U2,U4,I4,U8,I8,CP,PT,FS,FD,FL,/* U2*/
    I2,I2,U2,I2,U4,I4,U8,I8,CP,PT,FS,FD,FL,/* I2*/
    U4,U4,U4,U4,U4,U4,U8,I8,CP,PT,FS,FD,FL,/* U4*/
    I4,I4,I4,I4,U4,I4,U8,I8,CP,PT,FS,FD,FL,/* I4*/
    U8,U8,U8,U8,U8,U8,U8,U8,CP,PT,FD,FD,FL,/* U8*/
    I8,I8,I8,I8,I8,I8,U8,I8,CP,PT,FD,FD,FL,/* I8*/
    CP,CP,CP,CP,CP,CP,CP,CP,CP,PT,ER,ER,ER,/* CP*/
    PT,PT,PT,PT,PT,PT,PT,PT,PT,PT,ER,ER,ER,/* PT*/
    FS,FS,FS,FS,FS,FS,FD,FD,ER,ER,FS,FD,FL,/* FS*/
    FD,FD,FD,FD,FD,FD,FD,FD,ER,ER,FD,FD,FL,/* FD*/
    FL,FL,FL,FL,FL,FL,FL,FL,ER,ER,FL,FL,FL /* FL*/
};

static type_class_def SubMat[] = {
/*********************************
    what is the result type of a subtraction
*/
/*  U1 I1 U2 I2 U4 I4 U8 I8 CP PT FS FD FL*/
    U1,U1,U2,I2,U4,I4,U8,I8,CP,PT,FS,FD,FL,/* U1*/
    U1,I1,U2,I2,U4,I4,U8,I8,CP,PT,FS,FD,FL,/* I1*/
    U2,U2,U2,U2,U4,I4,U8,I8,CP,PT,FS,FD,FL,/* U2*/
    I2,I2,U2,I2,U4,I4,U8,I8,CP,PT,FS,FD,FL,/* I2*/
    U4,U4,U4,U4,U4,U4,U8,I8,CP,PT,FS,FD,FL,/* U4*/
    I4,I4,I4,I4,U4,I4,U8,I8,CP,PT,FS,FD,FL,/* I4*/
    U8,U8,U8,U8,U8,U8,U8,U8,CP,PT,FD,FD,FL,/* U8*/
    I8,I8,I8,I8,I8,I8,U8,I8,CP,PT,FD,FD,FL,/* I8*/
    CP,CP,CP,CP,CP,CP,CP,CP,XX,PT,ER,ER,ER,/* CP*/
    PT,PT,PT,PT,PT,PT,PT,PT,PT,XX,ER,ER,ER,/* PT*/
    FS,FS,FS,FS,FS,FS,FD,FD,ER,ER,FS,FD,FL,/* FS*/
    FD,FD,FD,FD,FD,FD,FD,FD,ER,ER,FD,FD,FL,/* FD*/
    FL,FL,FL,FL,FL,FL,FL,FL,ER,ER,FL,FL,FL,/* FL*/
};

/*
    Routines TGxxxx build up the expression trees. TNxxx tear them down.
*/

static  tn  NewTreeNode( void )
/******************************
    gimme a new tree node
*/
{
    tn  new;

    new = AllocFrl( &TreeFrl, sizeof( either_tree_node ) );
    new->base = NULL;
    new->alignment = 0;
#ifndef NDEBUG
    new->useinfo.hdltype = NO_HANDLE;
    new->useinfo.used = 0;
#endif
    return( new );
}


extern  tn  TGLeaf( an addr )
/****************************
    create a leaf node for "addr"
*/
{
    tn  node;

    node = NewTreeNode();
    node->op = O_NOP;
    node->u.addr = addr;
    node->rite = NULL;
    node->kids = 0;
    node->flags = TF_USED;
    node->tipe = addr->tipe;
    node->class = TN_LEAF;
    if( addr->format == NF_BOOL ) {
        node->tipe = TypeBoolean;
    } else if( addr->format == NF_CONS ) {
        node->class = TN_CONS;
        node->u.name = addr->u.name;
        BGDone( addr );
    }
    return( node );
}


extern  btn TGBitMask( tn left, byte start, byte len, type_def *tipe )
/*********************************************************************
    return a tree node for bits "start" for "len" selected from "left"
    whose type is "tipe".  Takes and yields an lvalue.
*/
{
    btn new;

    new = (btn)NewTreeNode();
    new->u.left = left;
    new->tipe = tipe;
    new->flags = TF_USED;
#if _TARGET & _TARG_370
    new->start = tipe->length*8 - start - len;
#else
    new->start = start;
#endif
    new->len = len;
    new->is_signed = FALSE;
    new->kids = left->kids + 1;
    new->class = TN_BIT_LVALUE;
#ifndef NDEBUG
    new->useinfo.hdltype = NO_HANDLE;
    new->useinfo.used = 0;
#endif
    return( new );
}


extern  tn  TGNode( tn_class class, cg_op op,
            tn left, tn rite, type_def *tipe )
/*********************************************
    create a general node
*/
{
    tn  node;

    node = NewTreeNode();
    node->class = class;
    node->op = op;
    node->u.left = left;
    node->rite = rite;
    node->tipe = tipe;
    node->flags = TF_USED;
    node->kids = 0;
    if( rite != NULL && class != TN_PARM ) {
        node->kids += rite->kids + 1;
    }
    if( left != NULL ) {
        node->kids  += left->kids + 1;
        node->flags |= TF_HAS_LEFT;
    }
    return( node );
}



extern  tn  TGWarp( tn before, label_handle label, tn after )
/************************************************************
    evaluate "before", call label "label" and yield value "after".
*/
{
    tn  result;

    result = TGNode( TN_COLON, O_NOP, NULL, before, after->tipe );
    result->u.handle = label;
    result = TGNode( TN_WARP, O_NOP, result, after, after->tipe );
    return( result );
}

extern  tn  TGHandle( void *ptr )
/********************************
    make a handle node - this is a leaf which holds a ptr
*/
{
    tn  node;

    node = NewTreeNode();
    node->tipe = TypeAddress( TY_DEFAULT );
    node->class = TN_HANDLE;
    node->op = O_NOP;
    node->u.handle = ptr;
    return( node );
}

extern  tn  TGCallback( cg_callback rtn, callback_handle ptr )
/*************************************************************
    make a special callback node which will be used to communicate with
    the front end
*/
{
    tn      node;

    node = TGNode( TN_CALLBACK, O_NOP, NULL, TGHandle( ptr ), TypeAddress( TY_DEFAULT ) );
    node->u.handle = TGHandle( rtn );
    return( node );
}

static  type_def    *ResultType( tn left, tn rite, type_def *tipe,
                     type_class_def *mat, bool demote_const )
/*****************************************************************
    What is the resulting type of "left" op "rite" given that the front
    end says it should be "tipe" (T_DEFAULT if its not sure).
    "demote_const" is true if we are allowed to do demotion of type even
    if the front end said it wanted a different type.  For example if
    the front end says do ( (char)i & 3 ) as an integer, we know better.
*/
{
    tn              temp;
    type_def        *ltipe;
    type_def        *rtipe;

#if _TARGET & 0
    if( tipe->length < WORD_SIZE ) {
        if( tipe->attr & TYPE_SIGNED ) {
            return( TypeAddress( TY_INTEGER ) );
        }
        return( TypeAddress( TY_UNSIGNED ) );
    }
#endif
    if( !demote_const && tipe->refno != TY_DEFAULT ) return( tipe );
    if( left->class == TN_CONS ) {
        temp = left;
        left = rite;
        rite = temp;
    }
    ltipe = left->tipe;
    if( ltipe == TypeBoolean ) {
        ltipe = TypeInteger;
    }
    rtipe = rite->tipe;
    if( rtipe == TypeBoolean ) {
        rtipe = TypeInteger;
    }
    if( tipe->refno == TY_DEFAULT ) {
        return( ClassType( mat[ TypeClass(ltipe)*XX + TypeClass(rtipe) ] ));
    }
#if _TARGET & 0
    return( tipe );
#else
    if( left->tipe->length >= tipe->length ) return( tipe );
    if( rite->class != TN_CONS ) return( tipe );
    if( left->tipe->attr & TYPE_FLOAT ) return( tipe );
    if( rite->tipe->attr & TYPE_FLOAT ) return( tipe );
    if( tipe->length > TypeClassSize[ U4 ] ) return( tipe );
    if( left->tipe->attr & TYPE_SIGNED ) {
       if( !CFSignedSize( rite->u.name->c.value, left->tipe->length ) ) {
           return( tipe );
       }
       if( rite->u.name->c.int_value < 0 ) return( tipe );
       if( !( tipe->attr & TYPE_SIGNED ) ) return( tipe );
    } else {
       if( !CFUnSignedSize( rite->u.name->c.value, left->tipe->length ) )  {
           return( tipe );
       }
    }
    return( left->tipe );
#endif
}

#if _TARGET & ( _TARG_80386 | _TARG_IAPX86 )
static bool RHSLongPointer( tn rite )
{
    if( rite->class == TN_LEAF && rite->u.addr->format == NF_ADDR ) {
        switch( rite->u.addr->class ) {
        case CL_ADDR_GLOBAL:
        case CL_ADDR_TEMP:
            return( TRUE );
        default:
            break;
        }
    } else {
        switch( rite->tipe->refno ) {
        case TY_LONG_POINTER:
        case TY_HUGE_POINTER:
        case TY_LONG_CODE_PTR:
            return( TRUE );
            break;
        }
    }
    return( FALSE );
}
#endif


extern  tn  TGCompare(  cg_op op,  tn left,  tn rite,  type_def  *tipe )
/***********************************************************************
    build a relational operator node
*/
{
    tn      new;
    tn_btn  left_tn_btn;
    bool    can_demote;

    can_demote = TRUE;
#if _TARGET & _TARG_AXP
    // FIXME: bad assumption being covered here
    if( tipe->length < 4 ) {
        tipe = TypeAddress( TY_INTEGER );
        can_demote = FALSE;
    }
#endif
    if( ( left->tipe == rite->tipe )
        && ( left->tipe != TypeBoolean )
        && ( ( left->tipe->attr & ~TYPE_SIGNED ) == ( tipe->attr & ~TYPE_SIGNED ) ) ) {
        tipe = left->tipe;
    } else {
        tipe = ResultType( left, rite, tipe, BinMat, can_demote );
    }
    left = TGConvert( left, tipe );
    rite = TGConvert( rite, tipe );
    new = FoldCompare( op, left, rite, tipe );
    if( new != NULL ) return( new );
    left_tn_btn.t = left;
    new = FoldBitCompare( op, left_tn_btn, rite );
    if( new != NULL ) return( new );
    new = FoldPostGetsCompare( op, left, rite, tipe );
    if( new != NULL ) return( new );
    new = TGNode( TN_COMPARE, op, left, rite, TypeBoolean );
    return( new );
}


static  an  Int( unsigned_32 num )
/*********************************
    return an address name for an integer
*/
{
    return( BGInteger( num, TypeInteger ) );
}


static  an  Int64( unsigned_64 num )
/***********************************
    return an address name for a 64-bit integer
*/
{
    return( BGInt64( num, TypeLongLongInteger ) );
}

extern  unsigned_32    Mask( btn node )
/**************************************
    return a mask of 1's in the positions a bit field occupies.
*/
{
    unsigned_32     mask;
    uint            len;
    unsigned_32     bit;

    len = node->len;
    mask = 0;
    bit = (unsigned_32)1 << node->start;
    for( ;; ) {
        mask |= bit;
        bit <<= 1;
        if( --len == 0 ) break;
    }
    return( mask );
}


static  unsigned_64    Mask64( btn node )
/****************************************
    like Mask(), only 64-bit
*/
{
    unsigned_64     mask;
    uint            len;
    unsigned_64     bit;
    unsigned_64     tmp;

    len = node->len;
    U32ToU64( 0, &mask );
    U32ToU64( 1, &tmp );
    U64ShiftL( &tmp, node->start, &bit );
    for( ;; ) {
        U64Or( &mask, &bit, &tmp );
        mask = tmp;
        U64ShiftL( &bit, 1, &tmp );
        bit = tmp;
        if( --len == 0 ) break;
    }
    return( mask );
}


extern  tn  TGConvert( tn name, type_def *tipe )
/***********************************************
    convert "name" to "tipe".  This may require turning a short circuit
    boolean expression into an integer 0 or 1.
*/
{
    type_def    *node_type;
    tn          new;

    node_type = name->tipe;
    new = name;
    if( tipe->refno != TY_DEFAULT && tipe != node_type ) {
        if( tipe == TypeBoolean ) {
            if( node_type != TypeBoolean ) {
                new = TGCompare( O_NE, new, TGLeaf( Int( 0 ) ), node_type );
            }
        } else {
            if( node_type == TypeBoolean ) {
                new = TGNode( TN_FLOW_OUT, O_NOP, new, NULL, tipe );
            } else {
                new = FoldCnvRnd( O_CONVERT, new, tipe );
                if( new == NULL ) {
                    new = TGNode( TN_UNARY, O_CONVERT, name, NULL, tipe );
                }
            }
        }
    }
    return( new );
}


static  type_def  *BinResult( cg_op op, tn *l, tn *r, type_def *tipe,
                  int commie )
/********************************************************************
    Calculate the resulting type of a binary operation "l" op "r".  Tipe
    is what the front end thinks is should be.  Sometimes we can do
    better.  Also, convert the operands to the type of the result if
    necessary before the operation is performed. There are some wacky cases
    like pointer + int where we don't want the integer operand promoted
    prior to the operation, hence all the jiggery pokery.
    Finally if PREGETS don't commute operands for 370
    cause PreGets gets confused.
*/
{
    tn          rite;
    tn          left;
    tn          temp;
    type_def    *otipe;

    commie = commie; /* shut up the compiler */
    rite = *r;
    left = *l;
    switch( op ) {
    case O_LSHIFT:
    case O_RSHIFT:
        left = TGConvert( left, tipe );
        tipe = left->tipe;
        rite = TGConvert( rite, TypeUnsigned );
        break;
    case O_DIV:
    case O_MOD:
        otipe = tipe;
        tipe = ResultType( left, rite, tipe, BinMat, FALSE );
        if( otipe == TypeNone ) { /* do integer divide to make C happy.*/
            if( tipe->refno == TY_UINT_1 || tipe->refno == TY_INT_1 ) {
                tipe = TypeInteger;
            }
        }
        left = TGConvert( left, tipe );
        rite = TGConvert( rite, tipe );
        break;
#if _TARGET & ( _TARG_80386 | _TARG_IAPX86 )
    case O_CONVERT: /* based pointer junk */
        left = TGConvert( left, TypeAddress( TY_NEAR_POINTER ) );
        if( !RHSLongPointer( rite ) ) {
            rite = TGConvert( rite, TypeAddress( TY_UINT_2 ) );
        }
        switch( tipe->refno ) {
        case TY_DEFAULT:
        case TY_NEAR_POINTER:
            tipe = TypeAddress( TY_LONG_POINTER );
            break;
        case TY_LONG_POINTER:
        case TY_HUGE_POINTER:
            /* OK */
            break;
        default:
            Zoiks( ZOIKS_082 );
            break;
        }
        break;
#endif
    case O_PLUS: /* pointer arithmetic is a pain*/
        if( left->class == TN_CALL ) { /* address of return value*/
            tipe = TypePtr;
            if( tipe->refno == TY_HUGE_POINTER ) {
                rite = TGConvert( rite, TypeHugeInteger );
            } else if( tipe->refno == TY_LONG_POINTER ) {
                rite = TGConvert( rite, TypeLongInteger );
            } else {
                rite = TGConvert( rite, TypeNearInteger );
            }
        } else {
            tipe = ResultType( left, rite, tipe, BinMat, FALSE );
            if( tipe->attr & TYPE_POINTER ) {
                if( !( left->tipe->attr & TYPE_POINTER ) ) {
                    temp = rite;
                    rite = left;
                    left = temp;
                }
                if( tipe->refno == TY_HUGE_POINTER ) {
                    rite = TGConvert( rite, TypeHugeInteger );
                } else if( tipe->refno == TY_LONG_POINTER ) {
                    rite = TGConvert( rite, TypeLongInteger );
                } else {
                    rite = TGConvert( rite, TypeNearInteger );
                }
                if( left->class == TN_LEAF ) {
                    if( NeedPtrConvert( left->u.addr, tipe ) ) {
                        left = TGConvert( left, tipe );
                    }
                } else {
                    left = TGConvert( left, tipe );
                }
            } else { /* non pointer add */
#if _TARGET & _TARG_370  /* don't want miss I4= I4 op I2 instructions */
                if( tipe->refno == TY_INT_4 ) {
                    if( commie && left->tipe->refno == TY_INT_2 ) {
                        temp = rite;
                        rite = left;
                        left = temp;
                    }
                    if( rite->tipe->refno != TY_INT_2 ) {
                        rite = TGConvert( rite, tipe );
                    }
                } else {
                    rite = TGConvert( rite, tipe );
                }
                left = TGConvert( left, tipe );
#else
                rite = TGConvert( rite, tipe );
                if( left->class == TN_LEAF ) {
                    if( NeedPtrConvert( left->u.addr, tipe ) ) {
                        left = TGConvert( left, tipe );
                    }
                } else {
                    left = TGConvert( left, tipe );
                }
#endif
            }
        }
        break;
    case O_MINUS:
        tipe = ResultType( left, rite, tipe, SubMat, FALSE );
        /* pointer subtraction yields a different result type than ops!*/
        if( tipe->refno == TypeHugeInteger->refno
            && left->tipe->refno == TY_HUGE_POINTER
            && rite->tipe->refno == TY_HUGE_POINTER ) {
            /* nothing*/
        } else if( tipe->refno == TypeLongInteger->refno
            && left->tipe->refno == TY_LONG_POINTER
            && rite->tipe->refno == TY_LONG_POINTER ) {
             /* nothing*/
        } else if( tipe->refno == TypeNearInteger->refno
            && left->tipe->refno == TY_NEAR_POINTER
            && rite->tipe->refno == TY_NEAR_POINTER ) {
             /* nothing*/
        } else if( tipe->refno == TY_HUGE_POINTER ) {
            rite = TGConvert( rite, TypeHugeInteger );
        } else if( tipe->refno == TY_LONG_POINTER ) {
            rite = TGConvert( rite, TypeLongInteger );
        } else if( tipe->refno == TY_NEAR_POINTER ) {
            rite = TGConvert( rite, TypeNearInteger );
        } else {
#if _TARGET & _TARG_370
            if( tipe->refno != TY_INT_4 || rite->tipe->refno != TY_INT_2 ) {
                 rite = TGConvert( rite, tipe );
            }
#else
            rite = TGConvert( rite, tipe );
#endif
        }
        left = TGConvert( left, tipe );
        break;
    case O_TIMES:
        tipe = ResultType( left, rite, tipe, BinMat, FALSE );
#if _TARGET & _TARG_IAPX86
        if( tipe->refno == TY_INT_4 &&
            left->tipe->length <= 2 && rite->tipe->length <= 2 ) {
            left = TGConvert( left, TypeInteger );
        } else {
            left = TGConvert( left, tipe );
            rite = TGConvert( rite, tipe );
        }
#elif _TARGET & _TARG_370
        if( tipe->refno == TY_INT_4 ) {
            if( commie && left->tipe->refno == TY_INT_2 ) {
                temp = rite;
                rite = left;
                left = temp;
            }
            if( rite->tipe->refno != TY_INT_2 ) {
                 rite = TGConvert( rite, tipe );
            }
        } else {
            rite = TGConvert( rite, tipe );
        }
        left = TGConvert( left, tipe );
#else
        left = TGConvert( left, tipe );
        rite = TGConvert( rite, tipe );
#endif
        break;
    case O_AND:
    case O_OR:
    case O_XOR:
        tipe = ResultType( left, rite, tipe, BinMat, TRUE );
        left = TGConvert( left, tipe );
        rite = TGConvert( rite, tipe );
        break;
    case O_POW:
    case O_ATAN2:
    case O_FMOD:
        tipe = ResultType( left, rite, tipe, BinMat, FALSE );
        left = TGConvert( left, tipe );
        rite = TGConvert( rite, tipe );
        break;
    case O_COMMA:
        tipe = ResultType( left, rite, tipe, BinMat, TRUE );
        break;
    case O_SIDE_EFFECT:
        tipe = ResultType( rite, left, tipe, BinMat, TRUE );
        break;
    default:
        _Zoiks( ZOIKS_054 );
        break;
    }
    *l = left;
    *r = rite;
    return( tipe );
}


static  tn  BinFold( cg_op op, tn left, tn rite, type_def *tipe )
/****************************************************************
    Try to fold "left" "op" "rite".  Return NULL if it is not possible,
    a new tree node if it is possible (freeing "left" and "rite").
*/
{
    switch( op ) {
    case O_PLUS:
        return( FoldPlus( left, rite, tipe ) );
    case O_MINUS:
        return( FoldMinus( left, rite, tipe ) );
    case O_TIMES:
        return( FoldTimes( left, rite, tipe ) );
    case O_DIV:
        return( FoldDiv( left, rite, tipe ) );
    case O_MOD:
        return( FoldMod( left, rite, tipe ) );
    case O_LSHIFT:
        return( FoldLShift( left, rite, tipe ) );
    case O_RSHIFT:
        return( FoldRShift( left, rite, tipe ) );
    case O_AND:
        return( FoldAnd( left, rite, tipe ) );
    case O_OR:
        return( FoldOr( left, rite, tipe ) );
    case O_XOR:
        return( FoldXor( left, rite, tipe ) );
    case O_COMMA:
        return( TGNode( TN_COMMA, O_NOP, left, rite, rite->tipe ) );
    case O_SIDE_EFFECT:
        return( TGNode( TN_SIDE_EFFECT, O_NOP, left, rite, left->tipe ) );
    case O_POW:
        return( FoldPow( left, rite, tipe ) );
    default:
        break;
    }
    return( NULL );
}


extern  tn  TGBinary( cg_op op, tn left, tn rite, type_def *tipe )
/*****************************************************************
    build a binary operator tree node
*/
{
    tn      result;
    tn      l;
    tn      r;

    l = left;
    r = rite;
    tipe = BinResult( op, &l, &r, tipe, TRUE );
    result = BinFold( op, l, r, tipe );
    if( result == NULL ) {
        result = TGNode( TN_BINARY, op, l, r, tipe );
    }
    return( result );
}


extern  tn  TGUnary( cg_op op, tn left, type_def *tipe )
/*******************************************************
    build a unary operator tree node
*/
{
    tn  new;

    new = NULL;

    if( op != O_POINTS ) { /* for O_POINTS, the tipe given is always correct*/
        if( tipe == TypeNone ) {
            tipe = left->tipe;
        }
    }

    switch( op ) {
    case O_UMINUS:
        left = TGConvert( left, tipe );
        new = FoldUMinus( left, tipe );
        break;
    case O_COMPLEMENT:
        left = TGConvert( left, tipe );
        new = Fold1sComp( left, tipe );
        break;
    case O_POINTS:
        if( left->class == TN_BIT_LVALUE ) {
            new = left;
            new->class = TN_BIT_RVALUE;
            if( tipe->attr & TYPE_SIGNED ) {
                ((btn)new)->is_signed = TRUE;
            }
        }
        break;
    case O_ROUND:
        new = FoldCnvRnd( op, left, tipe );
        break;
    case O_CONVERT:
        new = FoldCnvRnd( op, left, tipe );
        if( new == NULL ) {
            new = TGConvert( left, tipe );
        }
        break;
    case O_SQRT:
        left = TGConvert( left, tipe );
        new = FoldSqrt( left, tipe );
        break;
    case O_LOG:
    case O_LOG10:
        left = TGConvert( left, tipe );
        new = FoldLog( op, left, tipe );
        break;
    case O_COS:
    case O_SIN:
    case O_TAN:
    case O_FABS:
    case O_ACOS:
    case O_ASIN:
    case O_ATAN:
    case O_COSH:
    case O_SINH:
    case O_TANH:
    case O_EXP:
        left = TGConvert( left, tipe );
        break;
    case O_PARENTHESIS:
    /*
        This is to stop optimizations from happening across the tree
        such as constant folding and the like. Since the code generator
        does not currently do anything like that, we can just NOP the
        sucker. If we start getting more agressive, this will have to
        actually do something.
    */
        new = left;
        break;
    case O_PTR_TO_NATIVE:
    case O_PTR_TO_FOREIGN:
        break;
    case O_STACK_ALLOC:
        break;
    default:
        _Zoiks( ZOIKS_055 );
        break;
    }
    if( new == NULL ) {
        new = TGNode( TN_UNARY, op, left, NULL, tipe );
    }
    return( new );
}

extern  tn  TGInitCall( tn left, type_def *tipe, sym_handle aux )
/****************************************************************
    Return a tree node for a call to "left".  TGAddParm may add parms to
    the call node.  TGCall finalizes the call node.
*/
{
    tn          node;
    call_class  *pclass;

    node = TGNode( TN_PARM, O_NOP, left, aux, NULL );
    node = TGNode( TN_CALL, O_NOP, node, NULL, tipe );
    pclass = FEAuxInfo( (pointer *)aux, CALL_CLASS );
    if( *pclass & REVERSE_PARMS ) {
        node->flags |= TF_REVERSE;
    }
#if ( _TARGET & _TARG_370 )
    node->flags |= TF_REVERSE;
#endif
    return( node );
}


extern  tn  TGAddParm( tn to, tn parm, type_def *tipe )
/******************************************************
    see TGInitCall ^
*/
{
    tn  new;
    tn  scan;

    if( tipe->refno == TY_DEFAULT ) {
        tipe = parm->tipe;
        if( tipe == TypeBoolean ) {
            tipe = TypeInteger;
        }
    }
    parm = TGConvert( parm, tipe );
    new = TGNode( TN_PARM, O_NOP, parm, NULL, tipe );
    if( to->flags & TF_REVERSE ) {
        new->rite = to->rite;
        to->rite = new;
    } else {
        scan = to;
        while( scan->rite != NULL ) {
            scan = scan->rite;
        }
        scan->rite = new;
    }
    return( to );
}


extern  tn  TGCall( tn what )
/****************************
    see TGInitCall ^
*/
{
    return( what );
}


extern  tn  TGIndex( tn left, tn rite, type_def *tipe, type_def *ptipe )
/***********************************************************************
    return a tree for &left[rite].  "ptipe" is the pointer type of
    "left".  "tipe" is the type of the object pointed to by "left".
    Resulting node is a pointer node.
*/
{
    /*   For machines with an indexed addressing mode (like the VAX)*/
    /*   the following would probably not be done*/

    if( tipe->length != 1 ) {
        if( ptipe->refno == TY_HUGE_POINTER ) {
            rite = CGBinary( O_TIMES, rite,
                  CGInteger( tipe->length, TY_INTEGER ), TY_INT_4 );
        } else {
            rite = CGBinary( O_TIMES, rite,
                  CGInteger( tipe->length, TY_INTEGER ), TY_INTEGER );
        }
    }
    rite = CGBinary( O_PLUS, left, rite, ptipe->refno );
    return( rite );
}


extern  tn  DoTGAssign( tn dst, tn src, type_def *tipe, tn_class class )
/***********************************************************************
    build dst = src
*/
{
    tn          node;
    type_def    *node_tipe;

    if( tipe->refno == TY_DEFAULT ) {
        tipe = src->tipe;
    }
    src = TGConvert( src, tipe );
    node_tipe = tipe;
    if( class == TN_LV_ASSIGN ) {
        node_tipe = dst->tipe;
    }
    node = TGNode( class, O_NOP, dst, src, node_tipe );
    node->optipe = tipe;
    return( node );
}


extern  tn  TGAssign( tn dst, tn src, type_def *tipe )
/*****************************************************
    build dst = src
*/
{
    return( DoTGAssign( dst, src, tipe, TN_ASSIGN ) );
}


extern  tn  TGLVAssign( tn dst, tn src, type_def *tipe )
/*******************************************************
    build dst = src
*/
{
    return( DoTGAssign( dst, src, tipe, TN_LV_ASSIGN ) );
}


extern  bool    TGCanDuplicate( tn node )
/****************************************
    return TRUE if node safe to duplicate? (Has no side effects)
*/
{
    switch( node->class ) {
    case TN_LEAF:
    case TN_CONS:
        return( TRUE );
    case TN_UNARY:
    case TN_BIT_LVALUE:
    case TN_BIT_RVALUE:
        return( TGCanDuplicate( node->u.left ) );
    case TN_BINARY:
        if( !TGCanDuplicate( node->u.left ) ) return( FALSE );
        if( !TGCanDuplicate( node->rite ) ) return( FALSE );
        return( TRUE );
    default:
        return( FALSE );
    }
}


extern  name    *TGetName( tn node )
/***********************************
    given a leaf/cons node, return the associated "name"
*/
{
    name    *opnd;

    opnd = NULL;
    if( node->class == TN_CONS ) {
        opnd = node->u.name;
    } else if( node->class == TN_LEAF ) {
        opnd = AddrToName( node->u.addr );
    }
    BurnTree( node );
    return( opnd );
}


extern  tn  TGReLeaf( an addr )
/******************************
    create another leaf node for "addr".  This means it has been used
    twice in the same expression tree, and cannot be a "push/pop" style
    temporary.
*/
{
    tn      node;

    addr->flags |= NEVER_STACK;
    node = TGLeaf( addr );
    node->base = addr->base;
    return( node );
}


extern  tn  TGTmpLeaf( an addr )
/*******************************
*/
{
    name    *base;

    base = addr->base;
    addr = AddrEval( addr );
    addr->base = base;
    return( TGReLeaf( addr ) );
}

extern  tn  TGConst( pointer cons, type_def *tipe )
/**************************************************
    return a leaf node for "cons" (a cfloat pointer)
*/
{
    return( TGLeaf( MakeConst( cons, tipe ) ) );
}


extern  tn  TName( name *name, type_def *tipe )
/**********************************************
    return a leaf node for "name"
*/
{
    return( TGLeaf( AddrName( name, tipe ) ) );
}


extern  tn  TGDuplicate( tn node )
/*********************************
    Return a duplicate for tree "node"
*/
{
    tn  new;

    if( node != NULL ) {
        switch( node->class ) {
        case TN_CONS:
            new = TGConst( CFCopy( node->u.name->c.value ), node->tipe );
            break;
        case TN_LEAF:
            new = TGLeaf( AddrCopy( node->u.addr ) );
            break;
        case TN_BIT_LVALUE:
        case TN_BIT_RVALUE:
            new = (tn)TGBitMask( TGDuplicate( ((btn)node)->u.left ),
                       ((btn)node)->start, ((btn)node)->len,
                       ((btn)node)->tipe );
            break;
        default:
            new = TGNode( node->class, node->op, TGDuplicate( node->u.left ),
                    TGDuplicate( node->rite ), node->tipe );
            break;
        }
        new->class = node->class;
    } else {
        new = NULL;
    }
    return( new );
}


extern  tn  DoTGPreGets( cg_op op, tn left, tn rite, type_def *tipe,
                 tn_class class, tn_class assn_class )
/*******************************************************************
    Build a node for left op= right.  We try to turn it into "left =
    left op rite" by duplicating the node for left, (but we can't do
    this if left contains a call), and then constant fold "left op
    rite".  If this works out, it'll turn into a straight assignment.
    Notice that "Pre" refers to the position of the operator (++x), not
    whether we are yielding the value before the operation.
*/
{
    tn          leftp;
    tn          result;
    tn          dupleft;
    type_def    *optipe;
    tn          l;
    tn          r;

    switch( op ) {
    case O_DIV:
    case O_MOD:
    case O_RSHIFT:
        break;
    default:
        TGDemote( rite, tipe );
        break;
    }
    if( TGCanDuplicate( left ) ) {
        dupleft = TGDuplicate( left );
    } else {
        dupleft = NULL;
    }
    leftp = TGUnary( O_POINTS, left, tipe );
    l = leftp;
    r = rite;
    /* ok to use defaults here since we're assining the result to left*/
    optipe = BinResult( op, &l, &r, TypeNone, FALSE );
    rite = r;
    leftp = l;
    if( dupleft == NULL ) {
        result = NULL;
    } else {
        result = BinFold( op, leftp, rite, optipe );
    }
    if( result == NULL ) {
        // rite = TGUnary( O_CONVERT, rite, tipe );
        result = TGNode( class, op, left, rite, tipe );
        result->optipe = optipe;
        if( tipe != optipe ) { /* someone might have put a convert onto leftp*/
            if( leftp != left ) {
                if( leftp->u.left != left ) {
                    FreeTreeNode( leftp->u.left );
                }
                FreeTreeNode( leftp );
            }
        } else if( leftp != left ) {
            FreeTreeNode( leftp );
        }
        if( left->class == TN_BIT_RVALUE ) {
            left->class = TN_BIT_LVALUE;
        }
        if( dupleft != NULL ) {
            BurnTree( dupleft );
        }
    } else if( result->class == TN_BINARY
     && result->u.left == leftp
     && leftp->class == TN_UNARY
     && leftp->op == O_POINTS
     && leftp->u.left == left ) {
        result->class = class;
        result->u.left = left;
        result->optipe = optipe;
        if( left->class == TN_BIT_RVALUE ) {
            left->class = TN_BIT_LVALUE;
        } else {
            FreeTreeNode( leftp );
        }
        BurnTree( dupleft );
    } else {
        result = DoTGAssign( dupleft, result, tipe, assn_class );
        if( leftp == left ) {
            result->u.left->class = TN_BIT_LVALUE;
        }
    }
    return( result );
}


extern  tn  TGPreGets( cg_op op, tn left, tn rite, type_def *tipe )
/******************************************************************
*/
{
    return( DoTGPreGets( op, left, rite, tipe, TN_PRE_GETS, TN_ASSIGN ) );
}


extern  tn  TGLVPreGets( cg_op op, tn left, tn rite, type_def *tipe )
/********************************************************************
*/
{
    return( DoTGPreGets( op, left, rite, tipe, TN_LV_PRE_GETS, TN_LV_ASSIGN ) );
}


extern  tn  TGPostGets( cg_op op, tn left, tn rite, type_def *tipe )
/*******************************************************************
    node for left op= right, but yields the rvalue of left before the
    assignment took place.  (for x++) Notice that "Post" refers to the
    position of the operator (x++), not whether we are yielding the
    value after the operation.
*/
{
    rite = TGConvert( rite, tipe );
    return( TGNode( TN_POST_GETS, op, left, rite, tipe ) );
}


extern  cg_type TGType( tn node )
/********************************
*/
{
    return( node->tipe->refno );
}


extern  tn  TGPatch( patch_handle hdl, type_def *tipe )
/******************************************************
    create a patch node for "hdl"
*/
{
    tn  node;

    node = NewTreeNode();
    node->op = O_NOP;
    node->u.handle = hdl;
    node->rite = NULL;
    node->kids = 0;
    node->flags = TF_USED;
    node->tipe = tipe;
    node->class = TN_PATCH;
    return( node );
}

extern  tn  TGFlow( cg_op op, tn left, tn rite )
/***********************************************
    create a short circuit boolean expression node
*/
{
    tn  result;

    switch( op ) {
    case O_FLOW_AND:
        result = FoldFlAnd( left, rite );
        break;
    case O_FLOW_OR:
        result = FoldFlOr( left, rite );
        break;
    case O_FLOW_NOT:
        result = FoldFlNot( left );
        break;
    default:
        _Zoiks( ZOIKS_056 );
        break;
    }
    if( result != NULL ) return( result );
    left = TGConvert( left, TypeBoolean );
    if( rite != NULL ) {  /* O_FLOW_NOT*/
        rite = TGConvert( rite, TypeBoolean );
    }
    return( TGNode( TN_FLOW, op, left, rite, TypeBoolean ) );
}


extern  tn  TGTrash( tn node )
/*****************************
    evaluate "node", then throw away the resulting value
*/
{
    node->flags &= ~TF_USED;
    if( node->class == TN_UNARY ) {
        if( node->op == O_POINTS ) {
            node->u.left->flags &= ~TF_USED;
        }
    }
    return( node );
}


extern tn   TGAttr( tn node, cg_sym_attr attr )
/**********************************************
    mark the tree node as having a particular attribute.
    It must be a pointer to the location with the desired attr.
*/
{
    switch( attr ) {
    case CG_SYM_VOLATILE:
        node->flags |= TF_VOLATILE;
        break;
    case CG_SYM_CONSTANT:
        node->flags |= TF_CONSTANT;
        break;
    case CG_SYM_UNALIGNED:
        node->alignment = 1;
        break;
    }
    return( node );
}

extern  tn  TGAlign( tn node, uint align )
/*****************************************
    mark the tree node as aligned on an 'align' byte boundary.
    This must be done just prior to a fetch or LVAssign of some
    form.
*/
{
    node->alignment = align;
    return( node );
}


extern  tn  TGVolatile( tn node )
/********************************
    mark the tree node as volatile.  It must be a pointer to the
    volatile location. Here for history's sake.
*/
{
    return( TGAttr( node, CG_SYM_VOLATILE ) );
}

static  bool    IsAddress;

static  bool    SetAddress( bool value )
/***************************************
    Indicate we're processing an address expression.  Return the old
    value of IsAddress.
*/
{
    bool    old_value;

    old_value = IsAddress;
    IsAddress = value;
    return( old_value );
}


static  an  NotAddrGen( tn node )
/********************************
    generate a node which is NOT an address expression.
*/
{
    bool    was_address;
    an      retv;

    was_address = SetAddress( FALSE );
    retv = TreeGen( node );
    SetAddress( was_address );
    return( retv );
}


extern  void    TG3WayControl( tn node, label_handle lt,
                   label_handle eq, label_handle gt )
/*******************************************************
    for FORTRAN if( x ) 10,20,30
*/
{
    BG3WayControl( NotAddrGen( node ), lt, eq, gt );
}


extern  void    TGControl( cg_op op, tn node, label_handle lbl )
/***************************************************************
    generate a simple flow of control. The tree must be complete when this is called.
*/
{
    switch( op ) {
    case O_IF_TRUE:
        if( FoldIfTrue( node, lbl ) == FALSE ) {
            Control( op, node, lbl, TRUE );
        }
        break;
    case O_IF_FALSE:
        if( FoldIfFalse( node, lbl ) == FALSE ) {
            Control( op, node, lbl, TRUE );
        }
        break;
    default:
        BGGenCtrl( op, NULL, lbl, TRUE );
        break;
    }
}


static  void    Control( cg_op op, tn node, label_handle lbl, bool gen )
/***********************************************************************
    see TGControl ^
*/
{
    an  addr;

    addr = NotAddrGen( TGConvert( node, TypeBoolean ) );
    if( addr->format == NF_CONS ) { /* will either be NF_CONS or NF_BOOL*/
        if( CFTest( addr->u.name->c.value ) != 0 ) {
            if( op == O_IF_TRUE ) {
                BGGenCtrl( O_GOTO, NULL, lbl, gen );
            }
        } else {
            if( op == O_IF_FALSE ) {
                BGGenCtrl( O_GOTO, NULL, lbl, gen );
            }
        }
        BGDone( addr );
    } else {
        BGGenCtrl( op, (bn)addr, lbl, gen );
    }
}


static  name    *TNGetLeafName( tn node )
/****************************************
    given leaf "node", return the "name" associated with it.
*/
{
    an      addr;

    addr = node->u.addr;
    if( addr->format != NF_ADDR ) return( NULL );
    switch( addr->class ) {
    case CL_ADDR_TEMP:
    case CL_ADDR_GLOBAL:
        return( addr->u.name );
    case CL_POINTER:
        if( _IsModel( FORTRAN_ALIASING ) ) return( addr->u.name );
        return( NULL );
    default:
        return( NULL );
    }
}


static  pointer  TNFindBase( pointer nod )
/*****************************************
    Given "node", return the "name" which is the base of the expression.
    For example, the base of x[i+j] is x.  We use this base for aliasing
    information.  In fortran, a variable that has had its address taken
    may only be modified by a routine call if the address (base) is
    passed directly to the routine.  We generate an instruction that
    simulates this modification for live information and don't assume
    that it is destroyed by all calls. Go left first for comma operators,
    since fortran generates stuff like  *(c,x) = 7. This means *x = 7,
    but I'm really modifying c.
*/
{
    name    *op;
    tn      node = nod;

    if( node == NULL ) return( NULL );
    if( node->base != NULL ) return( node->base );
    switch( node->class ) {
    case TN_LEAF:
        return( TNGetLeafName( node ) );
    case TN_COMMA:
    case TN_SIDE_EFFECT:
        if( !( node->tipe->attr & TYPE_POINTER ) ) return( NULL );
        op = SafeRecurse( TNFindBase, ( node->class == TN_COMMA ) ? node->u.left : node->rite );
        if( op != NULL ) return( op );
    /* fall through */
    case TN_BINARY:
        // creating a based pointer via a binary convert - don't want a fake base
        if( node->class == TN_BINARY && node->op == O_CONVERT ) return( NULL );
    /* fall through */
    case TN_LV_ASSIGN:
    case TN_LV_PRE_GETS:
        if( !( node->tipe->attr & TYPE_POINTER ) ) return( NULL );
        op = SafeRecurse( TNFindBase, node->rite );
        if( op != NULL ) return( op );
        if( node->op == O_CONVERT ) return( NULL );
        if( node->class != TN_BINARY ) return( NULL );
        return( SafeRecurse( TNFindBase, node->u.left ) );
    case TN_UNARY:
        if( !( node->tipe->attr & TYPE_POINTER ) ) return( NULL );
        switch( node->op ) {
        case O_PTR_TO_NATIVE:
        case O_PTR_TO_FOREIGN:
        case O_CONVERT:
            return( SafeRecurse( TNFindBase, node->u.left ) );
        default:
            break;
        }
        if( _IsntModel( FORTRAN_ALIASING ) ) return( NULL );
        if( node->op != O_POINTS ) return( NULL );
        switch( node->u.left->class ) {
        case TN_SIDE_EFFECT:
            return( SafeRecurse( TNFindBase, node->u.left->rite ) );
        case TN_COMMA:
            return( SafeRecurse( TNFindBase, node->u.left->u.left ) );
        case TN_LV_ASSIGN:
        case TN_LV_PRE_GETS:
            return( SafeRecurse( TNFindBase, node->u.left->rite ) );
        default:
            break;
        }
        node = node->u.left;
        if( node->class != TN_LEAF ) return( NULL );
        op = TNGetLeafName( node );
        if( op == NULL ) return( NULL );
        if( op->n.class == N_TEMP ) {
            if( op->v.symbol == NULL ) return( NULL );
            op = SAllocUserTemp( FEAuxInfo( (pointer *)op->v.symbol,
                            SHADOW_SYMBOL ),
                            op->n.name_class, op->n.size );
        }
        return( op );
    default:
        break;
    }
    return( NULL );
}

extern  an  TGen( tn node, type_def *tipe )
/******************************************
    generate basic blocks (call BG routines) for "node"
*/
{
    an      retv;
    name    *base;

    TGBlip();
    base = TNFindBase( node );
    node = TGConvert( node, tipe );
    retv = TreeGen( node );
    if( retv->format != NF_BOOL ) {
        retv->flags &= ~STACKABLE;
        retv->base = base;
    }
    return( retv );
}


extern  an  TGReturn( tn node, type_def *tipe )
/**********************************************
    make the current procedure return the value of "node"
*/
{
    an  retv;

    retv = TGen( node, tipe );
    retv->flags |= STACKABLE;
    return( retv );
}


static  an  AddrGen( tn node )
/*****************************
    generate a tree node which we know is an address expression.  (may
    be folded by AddrPlus and friends)
*/
{
    bool        was_address;
    an          retv;
    name        *base;
    tn_flags    flags;
    type_length alignment;

    was_address = SetAddress( TRUE );
    base = TNFindBase( node );
    flags = node->flags;
    alignment = node->alignment;
#if _TARGET & _TARG_RISC
    if( alignment == 0 ) {
        if( node->tipe->refno >= TY_FIRST_FREE ) {
            // it's a user-defined struct/type
            alignment = node->tipe->align;
        }
    }
#endif
    retv = TreeGen( node );
    if( flags & TF_VOLATILE ) {
        if( retv->format != NF_BOOL )
            retv->flags |= VOLATILE;
    }
    if( alignment != 0 ) {
        if( retv->format != NF_BOOL )
            retv->alignment = alignment;
    }
    if( retv->format != NF_BOOL )
        retv->base = base;
    SetAddress( was_address );
    return( retv );
}


static  an  TNCallback( tn node )
/********************************
    call the front end supplied function
*/
{
    cg_callback     rtn;
    callback_handle parm;

    rtn = (cg_callback)node->u.left->u.handle;
    parm = (callback_handle)node->rite->u.handle;
    if( rtn != NULL ) {
#ifndef NDEBUG
        EchoAPICallBack( node, rtn, parm, "started\n\n" );
#endif
        rtn( parm );
#ifndef NDEBUG
        EchoAPICallBack( node, rtn, parm, "ended\n\n" );
#endif
    }
    FreeTreeNode( node->u.left );
    FreeTreeNode( node->rite );
    return( BGInteger( 0, TypeAddress( TY_UINT_2 ) ) );
}



an  TNFlow( tn node )
/********************
    generate basic blocks for a flow of control node
*/
{
    an      retv;
    an      left;
    an      rite;

    left = NotAddrGen( node->u.left );
    if( node->rite != NULL ) {
        rite = NotAddrGen( node->rite );
    }
    retv = (an)BGFlow( node->op, (bn)left, (bn)rite );
    return( retv );
}


#if _TARGET != _TARG_IAPX86
    #define JUST_USE_SHIFTS
#endif

static  an  TNBitShift( an retv, btn node, bool already_masked )
/***************************************************************
    Yield the integer value of the bit field of rvalue "retv" based on
    the bit field specification in "node". For an unsigned bit field
    we can leave the high order bits turned on.
*/
{
    type_def    *tipeu;
    type_def    *tipes;
    unsigned_64 mask;

    tipeu = node->tipe;
    switch( tipeu->length ) {
    case 1:
        tipeu = TypeAddress( TY_UINT_1 );
        tipes = TypeAddress( TY_INT_1 );
        break;
    case 2:
        tipeu = TypeAddress( TY_UINT_2 );
        tipes = TypeAddress( TY_INT_2 );
        break;
    case 4:
        tipeu = TypeAddress( TY_UINT_4 );
        tipes = TypeAddress( TY_INT_4 );
        break;
    case 8:
        tipeu = TypeAddress( TY_UINT_8 );
        tipes = TypeAddress( TY_INT_8 );
        break;
    default:
        _Zoiks( ZOIKS_092 );
        break;
    }
    mask = Mask64( node );
#ifdef JUST_USE_SHIFTS
    if( !node->is_signed && node->start == 0 ) {
        if( !already_masked ) {
            retv = BGBinary( O_AND, retv, Int64( mask ), tipeu, TRUE );
        }
    } else {
        retv = BGBinary( O_LSHIFT, retv,
                 Int( 8*tipeu->length - node->len - node->start ),
                 tipeu, TRUE );
        retv = BGBinary( O_RSHIFT, retv,
                 Int( 8*tipeu->length - node->len ),
                 node->is_signed ? tipes : tipeu, TRUE );
    }
#else
    if( node->is_signed ) {
        retv = BGBinary( O_RSHIFT, retv, Int( node->start ), tipeu, TRUE );
        U64ShiftR( &mask, node->start, &mask );
        retv = BGBinary( O_AND, retv, Int64( mask ), tipeu, TRUE );
        U64ShiftR( &mask, 1, &mask );
        U64Not( &mask, &mask );
        if( mask.u._32[I64LO32] == 0xffffffff ) { /* a one-bit signed bit field */
			unsigned_64 one;
			I32ToI64( 1, &one );
            retv = BGUnary( O_COMPLEMENT, retv, tipeu );
            retv = BGBinary( O_PLUS, retv, Int64( one ), tipes, TRUE );
        } else if( mask.u._32[I64LO32] == 0xfffff80 ) { /* an eight-bit signed bit field */
            switch( tipeu->length ) {
            case 1:
                break;
            case 2:
                retv = BGConvert( retv, TypeAddress( TY_INT_1 ) );
                break;
            case 4:
                retv = BGConvert( retv, TypeAddress( TY_INT_1 ) );
                retv = BGConvert( retv, TypeAddress( TY_INT_2 ) );
                break;
            case 8:
                retv = BGConvert( retv, TypeAddress( TY_INT_1 ) );
                retv = BGConvert( retv, TypeAddress( TY_INT_2 ) );
                retv = BGConvert( retv, TypeAddress( TY_INT_4 ) );
                break;
            }
            retv = BGConvert( retv, tipes );
        } else if( mask.u._32[I64LO32] == 0xffff8000 ) { /* a sixteen-bit signed bit field */
            switch( tipeu->length ) {
            case 2:
                break;
            case 4:
                retv = BGConvert( retv, TypeAddress( TY_INT_2 ) );
                break;
            case 8:
                retv = BGConvert( retv, TypeAddress( TY_INT_2 ) );
                retv = BGConvert( retv, TypeAddress( TY_INT_4 ) );
                break;
            }
            retv = BGConvert( retv, tipes );
        } else if( mask.u._32[I64LO32] == 0x80000000 ) { /* a 32-bit signed bit field */
            switch( tipeu->length ) {
            case 4:
                break;
            case 8:
                retv = BGConvert( retv, TypeAddress( TY_INT_4 ) );
                break;
            }
            retv = BGConvert( retv, tipes );
        } else {
            retv = BGBinary( O_XOR, retv, Int64( mask ), tipeu, TRUE );
            retv = BGBinary( O_MINUS, retv, Int64( mask ), tipes, TRUE );
        }
    } else {
        retv = BGBinary( O_RSHIFT, retv, Int( node->start ), node->tipe, TRUE );
        if( !already_masked && ( node->tipe->length * 8 - node->start - node->len != 0 )) {
            U64ShiftR( &mask, node->start, &mask );
            retv = BGBinary( O_AND, retv, Int64( mask ), node->tipe, TRUE );
        }
    }
#endif
    return( retv );
}


static  an  TNBitRVal( an retv, btn node )
/*****************************************
    Yield the integer value of the bit field of lvalue "retv" based on
    the bit field specification in "node".
*/
{
    retv = BGUnary( O_POINTS, retv, node->tipe );
    retv = TNBitShift( retv, node, FALSE );
    return( retv );
}


static  void    DoAnd( an left, unsigned_64 mask, tn node )
/**********************************************************
Turn off bits "mask" in address name "left"
*/
{
    BGDone( BGOpGets( O_AND, AddrCopy( left ), Int( ~(mask.u._32[I64LO32]) ),
              node->tipe, node->tipe ) );
}

static  void    DoAnd64( an left, unsigned_64 mask, tn node )
/************************************************************
Turn off bits "mask" in address name "left"
*/
{
    unsigned_64     tmp;

    U64Not( &mask, &tmp );
    BGDone( BGOpGets( O_AND, AddrCopy( left ), Int64( tmp ),
              node->tipe, node->tipe ) );
}

static  an  TNBitOpGets( tn node, type_def *tipe, bool yield_before_op )
/***********************************************************************
    Do a bit field assgnment, like a += b, or a = b or b++.  If
    "yield_before_op" is true, we want to yield the value of the bit
    field before the assignment.  Assignment is accomplished by
    extracting the RHS into an integer, shifting it about and then ORing
    it into the destination. "tipe" is the actual tipe which should be used
    for the operation.
*/
{
    an          left;
    an          rite;
    an          before_value;
    an          after_value;
    an          retv;
    an          free_retv;
    btn         lhs;
    uint        shift;
    unsigned_64 mask;
    unsigned_64 shiftmask;

    lhs = (btn)node->u.left;
    left = AddrGen( lhs->u.left );
    before_value = TNBitRVal( AddrCopy( left ), lhs );
    rite = NotAddrGen( node->rite );
    if( node->op != O_NOP ) {
        // convert everything - better safe than sorry
        after_value = BGBinary( node->op,
                BGConvert( BGDuplicate( before_value ), tipe ),
                BGConvert( rite, tipe ),
                tipe, TRUE );
    } else {
        after_value = rite;
    }
    after_value = BGConvert( after_value, node->tipe );
    mask = Mask64( lhs );
    shift = lhs->start;
    FreeTreeNode( (tn)lhs );
    U64ShiftR( &mask, shift, &shiftmask );  // shiftmask = mask >> shift;
    if( after_value->format == NF_CONS && after_value->class == CL_CONS2 ) {
        retv = Int( shiftmask.u._32[I64LO32] & after_value->u.name->c.int_value );
        if( retv->u.name->c.int_value != shiftmask.u._32[I64LO32] ) {
            DoAnd( left, mask, node );
        }
        if( retv->u.name->c.int_value != 0 ) {
            free_retv = retv;
            retv = Int( retv->u.name->c.int_value << shift );
            BGDone( free_retv );
            retv = BGOpGets( O_OR, left, retv, node->tipe, node->tipe );
        } else {
            BGDone( left );
        }
        BGDone( retv );
    } else {
        retv = BGBinary( O_AND, AddrCopy( after_value ), Int64( shiftmask ),
                  node->tipe, TRUE );
        DoAnd64( left, mask, node );
        retv = BGBinary( O_LSHIFT, retv, Int( shift ), node->tipe, TRUE );
        retv = BGOpGets( O_OR, left, retv, node->tipe, node->tipe );
        BGDone( retv );
    }
    if( yield_before_op ) {
        BGDone( after_value );
        retv = before_value;
    } else {
        BGDone( before_value );
        retv = BGBinary( O_AND, after_value, Int64( shiftmask ), node->tipe, TRUE );
    }
    return( retv );
}


an  TNPostGets( tn node )
/************************
    generate blocks for an x++ type node
*/
{
    an      retv;
    an      left;
    an      rite;
    an      leftp;

    if( node->u.left->class == TN_BIT_LVALUE ) {
        retv = TNBitOpGets( node, node->tipe, TRUE );
    } else {
        left = AddrGen( node->u.left );
        rite = NotAddrGen( node->rite );
        retv = MakeTempAddr( BGNewTemp( node->tipe ), node->tipe );
        leftp = BGUnary( O_POINTS, AddrCopy( left ), node->tipe );
        retv = BGAssign( retv, leftp, node->tipe );
        BGDone( BGOpGets( node->op, left, rite, node->tipe, node->tipe ) );
    }
    return( retv );
}


an  TNPreGets( tn node )
/***********************
    generate a += *= type node
*/
{
    an      retv;
    an      left;
    an      rite;

    if( node->u.left->class == TN_BIT_LVALUE ) {
        retv = TNBitOpGets( node, node->optipe, FALSE );
    } else {
        left = AddrGen( node->u.left );
        rite = NotAddrGen( node->rite );
        if( node->class == TN_LV_PRE_GETS ) {
            retv = BGCopy( left );
        }
        left = BGOpGets( node->op, left, rite, node->tipe, node->optipe );
        if( node->class == TN_LV_PRE_GETS ) {
            BGDone( left );
        } else {
            retv = left;
        }
    }
    retv->flags |= STACKABLE;
    return( retv );
}


static  an  TNBitAssign( tn node )
/*********************************
    do an assignment to a bit field.  If the RHS of the assignment is a
    similar bit field, zero the destination bit field, and OR in the
    appropriate bits.  If not, Convert the RHS to an integer, then back
    to the new bit field location of the left hand side.
*/
{
    an          left;
    an          rite;
    an          retv;
    btn         lhs;
    btn         rhs;
    unsigned_32 mask;

    lhs = (btn)node->u.left;
    rhs = (btn)node->rite;
    if( rhs->class == TN_BIT_RVALUE
     && rhs->start == lhs->start
     && rhs->len == lhs->len
     && rhs->tipe->length == lhs->tipe->length ) {
        mask = Mask( lhs );
        left = AddrGen( lhs->u.left );
        rite = AddrGen( rhs->u.left );
        rite = BGUnary( O_POINTS, rite, node->tipe );
        retv = BGBinary( O_AND, rite, Int( mask ), node->tipe, TRUE );
        BGDone( BGOpGets( O_AND, AddrCopy(left), Int( ~mask ),
                  node->tipe, node->tipe ) );
        BGDone( BGOpGets( O_OR, left, AddrCopy( retv ),
                  node->tipe, node->tipe ) );
        retv = TNBitShift( retv, lhs, TRUE );
        FreeTreeNode( (tn)rhs );
        FreeTreeNode( (tn)lhs );
    } else {
        retv = TNBitOpGets( node, node->tipe, FALSE );
    }
    return( retv );
}


an  TNAssign( tn node )
/**********************
    generate an assignment
*/
{
    an      retv;
    an      left;
    an      rite;

    if( node->u.left->class == TN_BIT_LVALUE ) {
        retv = TNBitAssign( node );
    } else {
        rite = NotAddrGen( node->rite );
        left = AddrGen( node->u.left );
        if( node->class == TN_LV_ASSIGN ) {
            retv = BGCopy( left );
        }
        left = BGAssign( left, rite, node->optipe );
        if( node->class == TN_ASSIGN ) {
            retv = BGDuplicate( left );
        }
        BGDone( left );
    }
    retv->flags |= STACKABLE;
    return( retv );
}


an  TNCompare( tn node )
/***********************
    generate a comparison.
*/
{
    an              retv;
    an              left;
    an              rite;
    label_handle    entry;

    entry = BGGetEntry();
    retv = FoldConsCompare( node->op, node->u.left,
                 node->rite, node->u.left->tipe );
    if( retv == NULL ) {
        if( node->u.left->kids >= node->rite->kids ) {
            left = NotAddrGen( node->u.left );
            rite = NotAddrGen( node->rite );
        } else {
            rite = NotAddrGen( node->rite );
            left = NotAddrGen( node->u.left );
        }
        retv = (an)BGCompare( node->op, left, rite, entry, left->tipe );
    } else {
        retv = (an)Boolean( Arithmetic( retv, TypeInteger ), entry );
    }
    return( retv );
}


an  TNUnary( tn node )
/*********************
    generate a unary node
*/
{
    an      left;
    an      retv;

    retv = NULL;
    if( node->op == O_POINTS ) {
       left = AddrGen( node->u.left );
    } else if( node->op == O_CONVERT ) {
        left = TreeGen( node->u.left );
        retv = BGConvert( left, node->tipe );
    } else {
        left = TreeGen( node->u.left );
    }
    if( retv == NULL ) {
        retv = BGUnary( node->op, left, node->tipe );
    }
    retv->flags |= STACKABLE;
    if( node->flags & TF_DEMOTED ) {
        AddrDemote( retv );
    }
    return( retv );
}

#if _TARGET & ( _TARG_80386 | _TARG_IAPX86 )
static an   MakeBased( an left, an rite, type_def *tipe )
/********************************************************
    Create a far pointer from the ashes of a near pointer on the left
    and a segment value on the right
*/
{
    an          temp;
    an          seg_dest;
    name        *temp_var;
    type_def    *near_type;
    type_def    *short_type;
    an          seg;

    temp_var = BGNewTemp( tipe );
    temp = MakeTempAddr( temp_var, tipe );
    near_type = TypeAddress( TY_NEAR_POINTER );
    short_type = TypeAddress( TY_UINT_2 );
    if( rite->format == NF_ADDR &&
    ( rite->class == CL_ADDR_GLOBAL || rite->class == CL_ADDR_TEMP ) ) {
        BGDone( BGAssign( AddrCopy( temp ), left, near_type ) );
        seg = AddrName( SegName( rite->u.name ), short_type );
        BGDone( rite );
        seg_dest = BGBinary( O_PLUS, AddrCopy(temp), Int(near_type->length),
                    TypePtr, TRUE );
        BGDone( BGAssign( seg_dest, seg, short_type ) );
    } else {
        switch( rite->tipe->refno ) {
        case TY_LONG_POINTER:
        case TY_HUGE_POINTER:
        case TY_LONG_CODE_PTR:
            BGDone( BGAssign( AddrCopy( temp ), rite, rite->tipe ) );
            BGDone( BGAssign( AddrCopy( temp ), left, near_type ) );
            break;
        default:
            BGDone( BGAssign( AddrCopy( temp ), left, near_type ) );
            seg_dest = BGBinary( O_PLUS, AddrCopy(temp), Int(near_type->length),
                    TypePtr, TRUE );
            BGDone( BGAssign( seg_dest, rite, short_type ) );
            break;
        }
    }
    temp = BGUnary( O_POINTS, temp, tipe );
    temp->flags |= STACKABLE;
    return( temp );
}
#endif


an  TNBinary( tn node )
/**********************
    generate a binary node
*/
{
    an      left;
    an      rite;
    an      retv;
    tn      l;
    tn      r;
    tn      folded;
    bool    was_address;

    if( node->u.left->kids >= node->rite->kids ) {
        left = TreeGen( node->u.left );
        rite = TreeGen( node->rite );
    } else {
        rite = TreeGen( node->rite );
        left = TreeGen( node->u.left );
    }

    // Ugly hack to make sure constants get folded
    l = TGLeaf( AddrCopy( left ) );
    r = TGLeaf( AddrCopy( rite ) );
    folded = BinFold( node->op, l, r, node->tipe );
    if( folded != NULL ) {
        BGDone( left );
        BGDone( rite );
        return( TreeGen( folded ) );
    }
    BurnTree( l );
    BurnTree( r );
    // end of ugly hack

#if _TARGET & (_TARG_80386 | _TARG_IAPX86)
    /* based pointer junk */
    if( node->op == O_CONVERT ) {
        retv = MakeBased( left, rite, node->tipe );
    } else /* Note missing brace */
#endif
    if( node->flags & TF_DEMOTED ) {
        was_address = SetAddress( FALSE ); /* force it to be generated! */
        retv = BGBinary( node->op, left, rite, node->tipe, FALSE );
        AddrDemote( retv );
        SetAddress( was_address );
    } else {
        retv = BGBinary( node->op, left, rite, node->tipe, TRUE );
    }
    retv->flags |= STACKABLE;
    return( retv );
}

static  an  TNWarp( tn node )
/****************************
    generate a TN_WARP node
*/
{
    an      dst;
    an      src;

    if( node->u.left->rite != NULL ) {
        BGDone( NotAddrGen( node->u.left->rite ) );
    }
    BGControl( O_INVOKE_LABEL, NULL, node->u.left->u.left );
    FreeTreeNode( node->u.left );
    dst = MakeTempAddr( BGNewTemp( node->tipe ), node->tipe );
    src = NotAddrGen( node->rite );
    return( BGAssign( dst, src, node->tipe ) );
}


static  an  TNQuestion( tn node )
/********************************
    Generate blocks for a ?  b : c.  Note that we call BGControl, rather
    than BGGenControl.  BGGenControl can start to flush out blocks if it
    is low on memory, and we can't allow this until the ?  operation is
    done since there may be an instruction hanging about that hasn't had
    its result field filled in yet.
*/
{
    label_handle    false;
    label_handle    around;
    an              retv;
    an              temp;
    name            *temp_var;

    false = AskForNewLabel();
    around = AskForNewLabel();
    Control( O_IF_FALSE, node->u.left, false, FALSE );
    temp_var = BGNewTemp( node->tipe );
    temp_var->v.usage |= USE_IN_ANOTHER_BLOCK;
    temp = MakeTempAddr( temp_var, node->tipe );
    retv = TreeGen( node->rite->u.left );
    BGDone( BGAssign( AddrCopy( temp ), retv, node->tipe ) );
    BGControl( O_GOTO, NULL, around );
    BGControl( O_LABEL, NULL, false );
    BGFiniLabel( false );
    retv = TreeGen( node->rite->rite );
    BGDone( BGAssign( AddrCopy( temp ), retv, node->tipe ) );
    BGControl( O_LABEL, NULL, around );
    BGFiniLabel( around );
    temp = BGUnary( O_POINTS, temp, node->tipe );
    FreeTreeNode( node->rite );
    temp->flags |= STACKABLE;
    return( temp );
}


static  tn  NodesToZap;

static  bool    FunctionModifiesSP( tn call_node )
/*************************************************
    does the given call modify sp?
*/
{
    tn          addr;
    pointer     sym;
    hw_reg_set  *pregs;

    addr = call_node->u.left;
    sym = addr->rite;
    pregs = FEAuxInfo( sym, SAVE_REGS );
    if( !HW_Ovlap( *pregs, StackReg() ) ) {
        return( TRUE );
    }
    return( FALSE );
}

static  bool    ModifiesSP( tn node )
/************************************
    see if the given tree can modify SP in any way - for now,
    just see if we make a call to a routine which modifies SP
    or use an OP_STK_ALLOC unary expression (might want to check
    results of assignments for SP in the future).
*/
{
    switch( node->class ) {
    case TN_BIT_LVALUE:
    case TN_BIT_RVALUE:
    case TN_LEAF:
    case TN_CALLBACK:
    case TN_HANDLE:
    case TN_PATCH:
        break;
    case TN_CALL:
        // need to check if routine modifies SP
        if( FunctionModifiesSP( node ) ) return( TRUE );
        // or if any of it's parm nodes modifies SP
        if( node->rite != NULL && ModifiesSP( node->rite ) ) return( TRUE );
        break;
    case TN_UNARY:
        if( node->op == OP_STK_ALLOC ) return( TRUE );
    /* fall through */
    default:
        if( node->rite != NULL ) {
            if( ModifiesSP( node->rite ) ) return( TRUE );
        }
        if( node->flags & TF_HAS_LEFT ) {
            assert( node->u.left != NULL );
            if( ModifiesSP( node->u.left ) ) return( TRUE );
        }
    }
    return( FALSE );
}

static  void    MakeSPSafe( tn scan )
/************************************
    run through the parm list and yank any trees which could
    modify the stack pointer out - this allows us to handle an
    alloca in a parm list and other such nonsense.
*/
{
    tn          parmtn;
    an          temp;
    an          parman;
    type_def    *tipe;

    while( scan != NULL ) {
        parmtn = scan->u.left;
        if( ModifiesSP( parmtn ) ) {
            tipe = parmtn->tipe;
            temp = MakeTempAddr( BGNewTemp( tipe ), tipe );
            parman = NotAddrGen( parmtn );
            BGDone( BGAssign( BGDuplicate( temp ), parman, parman->tipe ) );
            scan->u.left = TGUnary( O_POINTS, TGLeaf( temp ), tipe );
        }
        scan = scan->rite;
    }
}

static  an  TNCall( tn what, bool ignore_return )
/************************************************
    generate block for a TN_CALL node
*/
{
    tn          scan;
    tn          addr;
    an          temp;
    tn          parmtn;
    cn          call;
    an          retv;
    an          parman;
    type_def    *tipe;
    bool        in_line;
    name        *base;
    call_class  rtn_class;

    ignore_return=ignore_return;
    addr = what->u.left; /* address to call*/
    in_line = ( FEAuxInfo( (pointer *)addr->rite, CALL_BYTES ) != NULL );
    rtn_class = *(call_class *)FEAuxInfo( (pointer *)addr->rite, CALL_CLASS );
    if( rtn_class & MAKE_CALL_INLINE ) {
        BGDone( TreeGen( addr->u.left ) );
        BGStartInline( (sym_handle)addr->rite );
    } else {
        call = BGInitCall( TreeGen( addr->u.left ), what->tipe, addr->rite );
    }
    scan = what->rite;
    MakeSPSafe( scan );
    while( scan != NULL ) {
        base = TNFindBase( scan->u.left );
        parmtn = scan->u.left;
        scan->u.name = base;
        if( rtn_class & PARMS_BY_ADDRESS ) {
            if( parmtn->class == TN_UNARY && parmtn->op == O_POINTS ) {
                parman = AddrGen( parmtn->u.left );
                parmtn->u.left = NULL;
                parmtn->rite = NULL;
                BurnTree( parmtn );
            } else {
                temp = MakeTempAddr( BGNewTemp( parmtn->tipe ), parmtn->tipe );
                parman = AddrGen( parmtn );
                BGDone( BGAssign( BGDuplicate( temp ), parman, parman->tipe ) );
                parman = temp;
            }
            tipe = TypePtr;
        } else {
            parman = NotAddrGen( parmtn );
            tipe = scan->tipe;
        }
        if( tipe->refno == TY_DEFAULT ) {
            tipe = parman->tipe;
        }
        if( tipe == TypeProcParm ) {
            if( parman->tipe != TypeProcParm ) {
                retv = PassProcParm( parman );
                BGDone( parman );
                parman = retv;
            }
        } else {
            if( in_line || ( rtn_class & MAKE_CALL_INLINE ) ) {
                parman = BGConvert( parman, tipe );
            } else {
#if _TARGET & _TARG_AXP
                // kludge coming - Microsoft wants U4's sign extended to 8-byte
                // quantities when passed as parms to routines.
                if( parman->tipe->length == 4 &&
                    !(parman->tipe->attr & TYPE_SIGNED) ) {
                    parman = BGConvert( parman, TypeAddress( TY_INT_4 ) );
                }
#endif
                parman = BGConvert( parman, PassParmType( (pointer *)addr->rite, tipe, rtn_class ) );
            }
        }
        parman->flags |= STACKABLE;
        if( rtn_class & MAKE_CALL_INLINE ) {
            BGAddInlineParm( parman );
        } else {
            BGAddParm( call, parman );
        }
        scan = scan->rite;
    }
    FreeTreeNode( addr );
    if( rtn_class & MAKE_CALL_INLINE ) {
        retv = BGStopInline( what, what->tipe );
        NodesToZap = what->rite;
        TNZapParms();
    } else {
        NodesToZap = what->rite;
        retv = BGCall( call, ( what->flags & TF_USED ) != 0, in_line );
        retv->flags |= STACKABLE;
    }
    return( retv );
}


extern  void    TNZapParms( void )
/********************************/
{
    tn      junk;
    tn      scan;

    scan = NodesToZap;
    while( scan != NULL ) {
        BGZapBase( scan->u.name, scan->tipe );
        junk = scan;
        scan = scan->rite;
        FreeTreeNode( junk );
    }
}


extern  bool    TGIsAddress( void )
/**********************************
    Are we processing an address expression?
*/
{
    return( IsAddress );
}

extern  tn  TGQuestion( tn sel, tn left, tn rite, type_def *tipe )
/*****************************************************************
    sel ? left : rite
*/
{
    tn  result;

    sel = TGConvert( sel, TypeBoolean );
    left = TGConvert( left, tipe );
    rite = TGConvert( rite, tipe );
    result = TGNode( TN_COLON, O_NOP, left, rite, tipe );
    result = TGNode( TN_QUESTION, O_NOP, sel, result, tipe );
    return( result );
}


extern  void    BurnTree( tn node )
/**********************************
    figure it out
*/
{
    switch( node->class ) {
    case TN_LEAF:
        BGDone( node->u.addr );
        break;
    case TN_CONS:
        break;
    case TN_BIT_RVALUE:
    case TN_BIT_LVALUE:
        if( ((btn)node)->u.left != NULL ) {
            BurnTree( ((btn)node)->u.left );
        }
        break;
    case TN_CALL:
        BurnTree( node->u.left->u.left );
        FreeTreeNode( node->u.left );
        if( node->rite != NULL ) {
            BurnTree( node->rite );
        }
        break;
    default:
        if( node->rite != NULL ) {
            BurnTree( node->rite );
        }
        if( node->u.left != NULL ) {
            BurnTree( node->u.left );
        }
        break;
    }
    FreeTreeNode( node );
}


static  void    FreeTreeNode( tn node )
/**************************************
    free one tree node.
*/
{
    FrlFreeSize( &TreeFrl, (pointer *)node, sizeof( either_tree_node ) );
}


extern  void    TInit( void )
/****************************
    Initialize for tree processing
*/
{
    SetAddress( FALSE );
    SubMat[CP+XX*CP] = TypeClass( TypeLongInteger );
    SubMat[PT+XX*PT] = TypeClass( TypeHugeInteger );
    InitFrl( &TreeFrl );
}


extern  bool    TreeFrlFree( void )
/*********************************/
{
    return( FrlFreeAll( &TreeFrl, sizeof( either_tree_node ) ) );
}


extern  void    TFini( void )
/***************************/
{
    TreeFrlFree();
}


static  pointer  DoTreeGen( pointer nod )
/***************************************/
{
    an          retv;
    tn          node = nod;

    switch( node->class ) {
    case TN_LEAF:
        retv = node->u.addr;
        break;
    case TN_CONS:
        retv = AddrName( node->u.name, node->tipe );
        break;
    case TN_COMMA:
        BGDone( NotAddrGen( node->u.left ) );
        retv = TreeGen( node->rite );
        break;
    case TN_SIDE_EFFECT:
        retv = TreeGen( node->u.left );
        BGDone( NotAddrGen( node->rite ) );
        break;
    case TN_BINARY:
        retv = TNBinary( node );
        break;
    case TN_UNARY:
        retv = TNUnary( node );
        break;
    case TN_COMPARE:
        retv = TNCompare( node );
        break;
    case TN_LV_ASSIGN:
    case TN_ASSIGN:
        retv = TNAssign( node );
        break;
    case TN_LV_PRE_GETS:
    case TN_PRE_GETS:
        retv = TNPreGets( node );
        break;
    case TN_POST_GETS:
        retv = TNPostGets( node );
        break;
    case TN_FLOW:
        retv = TNFlow( node );
        break;
    case TN_CALL:
        retv = TNCall( node, FALSE );
        break;
    case TN_FLOW_OUT:
        retv = Arithmetic( NotAddrGen( node->u.left ), node->tipe );
        break;
    case TN_QUESTION:
        retv = TNQuestion( node );
        break;
    case TN_BIT_RVALUE:
        retv = TNBitRVal( AddrGen( node->u.left ), (btn)node );
        break;
    case TN_WARP:
        retv = TNWarp( node );
        break;
    case TN_CALLBACK:
        retv = TNCallback( node );
        break;
    case TN_PATCH:
        retv = TNPatch( node );
        break;
    default:
        _Zoiks( ZOIKS_057 );
    }
    FreeTreeNode( node );
    return( retv );
}


extern  an  TreeGen( tn node )
/*****************************
    generate tree node "node"
*/
{
    return( SafeRecurse( DoTreeGen, node ) );
}
