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
* Description:  C expression parsing.
*
****************************************************************************/


#include "cvars.h"
#include "cg.h"
#include "cgdefs.h"
#include "cgswitch.h"
#include "pragdefn.h"
#include "i64.h"
#include "caux.h"
#include "cfeinfo.h"

struct mathfuncs {
    char            *name;
    unsigned char   parm_count;
    unsigned char   mathop;
} MathFuncs[] = {                       /* 15-dec-90 */
    #define mathfunc(name,num,op) {name,num,op}
    #include "cmathfun.h"
};

local   TREEPTR GenNextParm(TREEPTR,TYPEPTR **);
local   TREEPTR StartFunc(TREEPTR,TYPEPTR **);
local   TREEPTR GetExpr(void);
local   TREEPTR ExprId(void);
local   TREEPTR ExprOpnd(void);
local   TREEPTR SizeofOp(TYPEPTR);
local   TREEPTR ScalarExpr(TREEPTR);
local   TREEPTR UnaryPlus(TREEPTR);
local   TREEPTR TernOp(TREEPTR,TREEPTR,TREEPTR);
local   TREEPTR ColonOp(TREEPTR);
local   TREEPTR StartTernary(TREEPTR);
local   TREEPTR NotOp(TREEPTR);
local   TREEPTR AndAnd(TREEPTR);
local   TREEPTR OrOr(TREEPTR);
local   TREEPTR GenFuncCall(TREEPTR);
local   TREEPTR IndexOp(TREEPTR,TREEPTR);
local   TREEPTR SegOp(TREEPTR,TREEPTR);
local      void PopNestedParms( TYPEPTR **plistptr );
local      void IncSymWeight( SYMPTR sym );
local      void AddCallNode( TREEPTR tree );
extern    int64 LongValue64( TREEPTR leaf );

        call_list   *CallNodeList;
static  call_list   **LastCallLnk;
static  call_list   **FirstCallLnk;

void ExprInit( void )
{
    Level = 0;
    NestedParms = NULL;
    CallNodeList = NULL;
    LastCallLnk = &CallNodeList;
    FirstCallLnk = LastCallLnk;
    InitExprTree();
}

//-----------------------------CNODE-----------------------------------
TREEPTR CallNode( TREEPTR func, TREEPTR parms, TYPEPTR func_result )
{
    TREEPTR     tree;
    TYPEPTR     typ;

    tree = ExprNode( func, OPR_CALL, parms );
    tree->u.expr_type = func_result;
    tree->op.u2.result_type = func_result;
    typ = func_result;
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type != TYPE_VOID && TypeSize( typ ) == 0 ) {
        CErr1( ERR_INCOMPLETE_EXPR_TYPE );
    }
    return( tree );
}

TYPEPTR Far16Type( TYPEPTR typ )
{ // TODO all this stuff should be part of the func defn.
  // not some hack on for later
    TYPEPTR     typ2;

    typ2 = typ;
    SKIP_TYPEDEFS( typ2 );
    if( typ2->decl_type == TYPE_ENUM ) {
        typ2 = typ2->object;
    }
    switch( typ2->decl_type ) {
    case TYPE_INT:
        typ = GetType( TYPE_SHORT );
        break;
    case TYPE_UINT:
        typ = GetType( TYPE_USHORT );
        break;
    default:
        break;
    }
    return( typ );
}

TREEPTR FarPtr16Cvt( TREEPTR newparm  )
{
    TYPEPTR         parmtyp;
    pointer_class   op1_class;
    pointer_class   op2_class;

    parmtyp = newparm->u.expr_type;
    SKIP_TYPEDEFS( parmtyp );
    if( parmtyp->decl_type == TYPE_POINTER ) {
        op1_class = PTR_FAR16;
        op2_class = ExprTypeClass( newparm->u.expr_type );
        newparm = ExprNode( NULL, OPR_CONVERT_PTR, newparm );
        newparm->u.expr_type = parmtyp;
        newparm->op.u2.sp.oldptr_class = op2_class;
        newparm->op.u2.sp.newptr_class = op1_class;
    }
    return( newparm );
}

TREEPTR ParmNode( TREEPTR parmlist, TREEPTR newparm, int far16_func )
{
    TREEPTR     tree;
    TYPEPTR     parmtyp;

    if( far16_func ) {
        newparm = FarPtr16Cvt( newparm );
    }
    tree = ExprNode( parmlist, OPR_PARM, newparm );
    if( far16_func ) {
        parmtyp = Far16Type( newparm->u.expr_type );
    } else {
        parmtyp = newparm->u.expr_type;
    }
    tree->u.expr_type = parmtyp;
    tree->op.u2.result_type = parmtyp;
    return( tree );
}

TREEPTR ErrorNode( TREEPTR tree )
{
    FreeExprTree( tree );
    tree =  LeafNode( OPR_ERROR );
    tree->u.expr_type = GetType( TYPE_INT );
    return( tree );
}

extern op_flags OpFlags( type_modifiers flags )
{
    op_flags      ops;

    ops = OPFLAG_NONE;
    if( flags & FLAG_CONST )    ops |= OPFLAG_CONST;
    if( flags & FLAG_VOLATILE ) ops |= OPFLAG_VOLATILE;
    if( flags & FLAG_UNALIGNED )ops |= OPFLAG_UNALIGNED;
#if ( _CPU == 8086 ) || ( _CPU == 386 )
    if( flags & FLAG_NEAR )     ops |= OPFLAG_NEARPTR;
    if( flags & FLAG_FAR )      ops |= OPFLAG_FARPTR;
    if( flags & FLAG_HUGE )     ops |= OPFLAG_HUGEPTR;
    if( flags & FLAG_FAR16 )    ops |= OPFLAG_FAR16PTR;
#endif
    return( ops );
}

extern type_modifiers FlagOps( op_flags ops )
{
    type_modifiers      flags;

    flags = FLAG_NONE;
    if( ops & OPFLAG_CONST )    flags |= FLAG_CONST;
    if( ops & OPFLAG_VOLATILE ) flags |= FLAG_VOLATILE;
    if( ops & OPFLAG_UNALIGNED )    flags|= FLAG_UNALIGNED;
#if ( _CPU == 8086 ) || ( _CPU == 386 )
    if( ops & OPFLAG_NEARPTR )     flags |= FLAG_NEAR;
    if( ops & OPFLAG_FARPTR )      flags |= FLAG_FAR;
    if( ops & OPFLAG_HUGEPTR )     flags |= FLAG_HUGE;
    if( Far16Pointer(ops)  )       flags |= FLAG_FAR16;
#endif
    return( flags );
}

TREEPTR ConstLeaf( void )
{
    TREEPTR     leaf;
    FLOATVAL    *flt;

    leaf = LeafNode( OPR_PUSHINT );     // assume integer value
    leaf->op.u1.const_type = ConstType;
    switch( ConstType ) {
    case TYPE_WCHAR:
        if( sizeof( wchar_t ) == sizeof( unsigned short ) ) {
            leaf->op.u1.const_type = TYPE_USHORT;
        } else {
            leaf->op.u1.const_type = TYPE_CHAR;
        }
        leaf->op.u2.long_value = Constant;
        break;
    case TYPE_CHAR:
        leaf->op.u1.const_type = TYPE_INT;
    case TYPE_INT:
        leaf->op.u2.long_value = Constant;
        break;
    case TYPE_UINT:
        leaf->op.u2.ulong_value = Constant;
        break;
    case TYPE_LONG:
        leaf->op.u2.long_value = Constant;
        break;
    case TYPE_ULONG:
        leaf->op.u2.ulong_value = Constant;
        break;
    case TYPE_LONG64:
        leaf->op.u2.long64_value = Const64;
        break;
    case TYPE_ULONG64:
        leaf->op.u2.ulong64_value = Const64;
        break;
    case TYPE_FLOAT:
    case TYPE_DOUBLE:
    case TYPE_LONG_DOUBLE:
    case TYPE_FIMAGINARY:
    case TYPE_DIMAGINARY:
    case TYPE_LDIMAGINARY:
        flt = CMemAlloc( sizeof( FLOATVAL ) + TokenLen );
        flt->string[0] = '+';
        strcpy( &flt->string[1], Buffer );
        flt->len = TokenLen + 1;
        flt->type = ConstType;
        flt->next = NULL;
        leaf->op.u2.float_value = flt;
        leaf->op.opr = OPR_PUSHFLOAT;
        /* Convert constant to binary; that ensures it'll be cast properly. */
        MakeBinaryFloat( leaf );
        break;
    }
    leaf->u.expr_type = GetType( leaf->op.u1.const_type );
    return( leaf );
}

TREEPTR IntLeaf( target_int value )
{
    TREEPTR     leaf;

    leaf = LeafNode( OPR_PUSHINT );
    leaf->op.u1.const_type = TYPE_INT;
    leaf->op.u2.long_value = value;
    leaf->u.expr_type = GetType( TYPE_INT );
    return( leaf );
}

TREEPTR UIntLeaf( target_uint value )
{
    TREEPTR     leaf;

    leaf = LeafNode( OPR_PUSHINT );
    leaf->op.u1.const_type = TYPE_UINT;
    leaf->op.u2.ulong_value = value;
    leaf->u.expr_type = GetType( TYPE_UINT );
    return( leaf );
}

TREEPTR LongLeaf( target_long value )
{
    TREEPTR     leaf;

    leaf = LeafNode( OPR_PUSHINT );
    leaf->op.u1.const_type = TYPE_LONG;
    leaf->op.u2.long_value = value;
    leaf->u.expr_type = GetType( TYPE_LONG );
    return( leaf );
}

local TREEPTR EnumLeaf( ENUMPTR ep )
{
    DATA_TYPE   decl_type;
    TREEPTR     leaf;

    leaf = LeafNode( OPR_PUSHINT );
    decl_type = ep->parent->sym_type->object->decl_type;
    switch( decl_type ) {
    case TYPE_CHAR:
    case TYPE_SHORT:
        decl_type = TYPE_INT;
        // fall through
    case TYPE_INT:
    case TYPE_LONG:
        leaf->op.u2.long_value = (signed_32)ep->value.u._32[L];
        break;
    case TYPE_UCHAR:
    case TYPE_USHORT:
        decl_type = TYPE_INT;
        // fall through
    case TYPE_UINT:
    case TYPE_ULONG:
        leaf->op.u2.long_value = ep->value.u._32[L];
        break;
    case TYPE_LONG64:
    case TYPE_ULONG64:
        leaf->op.u2.long64_value = ep->value;
        break;
    }
    leaf->op.u1.const_type = decl_type;
    leaf->u.expr_type = GetType( decl_type );
    return( leaf );
}

TREEPTR VarLeaf( SYMPTR sym, SYM_HANDLE sym_handle )
{
    TREEPTR         leaf;
    TYPEPTR         typ;
    type_modifiers  flags;

    leaf = LeafNode( OPR_PUSHADDR );
    leaf->op.u2.sym_handle = sym_handle;
    typ = sym->sym_type;
    leaf->u.expr_type = typ;
    flags = sym->mods & MASK_PTR;
    leaf->op.flags = OpFlags( flags );
    return( leaf );
}


TREEPTR SymLeaf( void )
{
    SYM_HANDLE  sym_handle;
    SYM_HANDLE  sym0_handle;
    int         hash;
    TREEPTR     tree;
    SYM_ENTRY   sym;
    ENUMPTR     ep;

    if( CurToken == T_SAVED_ID ) {
        CurToken = LAToken;
        hash = SavedHash;
        ep = EnumLookup( hash, SavedId );       /* 12-sep-88 */
        sym_handle = SymLook( hash, SavedId );
        if( sym_handle == 0 ) {
            if( ep != NULL ) {               /* if enum was found */
                return( EnumLeaf( ep ) );
            }
            SymCreate( &sym, SavedId );
        } else {
            SymGet( &sym, sym_handle );
            if( ep != NULL && ep->parent->level > sym.level )
                return( EnumLeaf( ep ) );
            /* 12-dec-88 */
            if( sym.attribs.stg_class == SC_EXTERN  &&  sym.level > 0 ) {
                sym0_handle = Sym0Look( hash, SavedId );
                if( sym0_handle != 0 ) {
                    sym_handle = sym0_handle;
                    SymGet( &sym, sym_handle );
                }
            }
            if( sym.attribs.stg_class == SC_TYPEDEF ) {
                CErr2p( ERR_CANT_USE_TYPEDEF_AS_VAR, SavedId );
                return( IntLeaf( 0 ) );
            }
        }
    } else {
        hash = HashValue;
        ep = EnumLookup( hash, Buffer );        /* 12-sep-88 */
        sym_handle = SymLook( hash, Buffer );
        if( sym_handle == 0 ) {
            if( ep != NULL ) {               /* if enum was found */
                NextToken();
                return( EnumLeaf( ep ) );
            }
            SymCreate( &sym, Buffer );
        } else {
            SymGet( &sym, sym_handle );
            if( ep != NULL && ep->parent->level > sym.level ) {
                NextToken();
                return( EnumLeaf( ep ) );
            }
            /* 12-dec-88 */
            if( sym.attribs.stg_class == SC_EXTERN  &&  sym.level > 0 ) {
                sym0_handle = Sym0Look( hash, Buffer );
                if( sym0_handle != 0 ) {
                    sym_handle = sym0_handle;
                    SymGet( &sym, sym_handle );
                }
            }
            if( sym.attribs.stg_class == SC_TYPEDEF ) {
                CErr2p( ERR_CANT_USE_TYPEDEF_AS_VAR, Buffer );
                return( IntLeaf( 0 ) );
            }
        }
        NextToken();
    }
    /* if( SizeOfCount == 0 ) */ /* causes defined but not referenced */
    /* always turning it on can cause referenced but not assigned */
    /* for the case:  int i;  j = sizeof(i);  */
    sym.flags |= SYM_REFERENCED;
    if( sym_handle == 0 ) {
        if( CurToken == T_LEFT_PAREN ) {
            sym.attribs.stg_class = SC_FORWARD;     /* indicate forward decl */
            /* Warn about unprototyped function */
            CWarn2p( WARN_ASSUMED_IMPORT, ERR_ASSUMED_IMPORT, sym.name );
            sym_handle = SymAddL0( hash, &sym ); /* add symbol to level 0 */
            sym.flags |= SYM_FUNCTION;
            sym.sym_type = FuncNode( GetType( TYPE_INT ), 0, NULL );
        } else {
            sym.attribs.stg_class = SC_EXTERN;      /* indicate extern decl */
            CErr2p( ERR_UNDECLARED_SYM, sym.name );
            sym_handle = SymAdd( hash, &sym ); /* add sym to current level*/
            sym.sym_type = GetType( TYPE_INT );
        }
    }
    IncSymWeight( &sym );
    tree = VarLeaf( &sym, sym_handle );
    SymReplace( &sym, sym_handle );
    return( tree );
}


local void IncSymWeight( SYMPTR sym )
{
    static int  LoopWeights[] = { 1, 0x10, 0x100, 0x1000 };

    if( sym->level != 0 ) {
        if( !(sym->flags & SYM_FUNCTION) ) {
            if( LoopDepth > 3 ) {
                sym->u.var.offset = ~0u >> 1;
            } else {
                sym->u.var.offset += LoopWeights[ LoopDepth ];
            }
        }
    }
}

//-----------------------------CMATH------------------------------------

static bool IsStruct( TYPEPTR typ )
/*********************************/
{
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type == TYPE_STRUCT  ||
        typ->decl_type == TYPE_UNION ) {
            return( TRUE );
    }
    return( FALSE );
}

bool IsLValue( TREEPTR tree )
{
    switch( tree->op.opr ) {
    case OPR_ERROR:
    case OPR_ARROW:
    case OPR_INDEX:
    case OPR_PUSHADDR:
    case OPR_POINTS:
        return( TRUE );
    case OPR_DOT:
        while( tree->op.opr == OPR_DOT )
            tree = tree->left;
        switch( tree->op.opr ) {
        case OPR_CALL:
        case OPR_QUESTION:
        case OPR_COMMA:
            // These are not lvalues
            break;
        default:
            return( TRUE );
            break;
        }
        break;
    }
    return( FALSE );
}

static bool IsCallValue( TREEPTR tree )
{
    bool ret;

    while( tree->op.opr == OPR_DOT || tree->op.opr == OPR_INDEX ) {
        tree = tree->left;
    }
    if( tree->op.opr == OPR_CALL ) {
        ret = TRUE;
    } else {
        ret = FALSE;
    }
    return( ret );
}

// This RVALUE thing is backwards -mjc
local TREEPTR TakeRValue( TREEPTR tree, int void_ok )
{
    TYPEPTR             typ;
    sym_flags           symb_flags;
    type_modifiers      decl_flags;
    target_uint         value;
    SYM_ENTRY           sym;

    if( tree->op.opr == OPR_ERROR )
        return( tree );
    if( CompFlags.pre_processing ) {
        if( tree->op.opr == OPR_PUSHFLOAT ) {
            CErr1( ERR_EXPR_MUST_BE_INTEGRAL );
            return( ErrorNode( tree ) );
        }
    }
    typ = tree->u.expr_type;
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type == TYPE_VOID ) {
        if( ! void_ok ) {
            CErr1( ERR_EXPR_HAS_VOID_TYPE );
            return( ErrorNode( tree ) );
        }
    } else if( typ->decl_type == TYPE_ARRAY ) {

        if( tree->op.opr == OPR_PUSHSTRING ) {
            if( typ->object->decl_type == TYPE_USHORT ) { /* 02-aug-91 */
                typ = PtrNode( typ->object, FLAG_NONE, SEG_DATA );
            } else {
                StringArrayType = typ;
                typ = StringType;
            }
            tree->u.expr_type = typ;
        } else {

            decl_flags = FlagOps( tree->op.flags );
            if( tree->op.opr == OPR_PUSHADDR ) {
                SymGet( &sym, tree->op.u2.sym_handle );
                symb_flags = sym.flags;
                sym.flags |= SYM_REFERENCED | SYM_ASSIGNED;
                if( symb_flags != sym.flags ) {
                    SymReplace( &sym, tree->op.u2.sym_handle );
                }
            }
            if( IsCallValue( tree ) ) {
                CErr1( ERR_CANT_TAKE_ADDR_OF_RVALUE );
            }
            tree = ExprNode( NULL, OPR_ADDROF, tree );
            tree->u.expr_type = PtrNode( typ->object, decl_flags, SEG_DATA );
        }
    } else if( typ->decl_type == TYPE_FUNCTION ) {

        symb_flags = FLAG_NONE;
        if( tree->op.opr == OPR_PUSHADDR ) {
            SymGet( &sym, tree->op.u2.sym_handle );
            decl_flags = sym.mods;
            symb_flags = sym.flags;
            sym.flags |= SYM_REFERENCED | SYM_ADDR_TAKEN;
            if( symb_flags != sym.flags ) {
                    SymReplace( &sym, tree->op.u2.sym_handle );
            }
        } else if( tree->op.opr == OPR_POINTS ) {
            decl_flags = tree->op.u2.result_type->u.p.decl_flags;
        } else {
            decl_flags = FLAG_NONE;
        }
        tree = ExprNode( NULL, OPR_ADDROF, tree );
        tree->u.expr_type = PtrNode( typ, decl_flags, 0 );
    } else if( TypeSize( typ ) == 0 ) {
        SetDiagType1( typ );
        CErr1( ERR_INCOMPLETE_EXPR_TYPE );
        SetDiagPop();
        return( ErrorNode( tree ) );
    } else {
        if( SizeOfCount == 0 ) {                        /* 05-jan-89 */
            if( tree->op.flags & OPFLAG_VOLATILE ) {    /* 02-nov-91 */
                CompFlags.useful_side_effect = 1;
            }
        }
        if( tree->op.opr == OPR_PUSHSYM || tree->op.opr == OPR_PUSHADDR ) {
            SymGet( &sym, tree->op.u2.sym_handle );
            symb_flags = sym.flags;
            sym.flags |= SYM_REFERENCED;                /* 07-jun-89 */
            if( CompFlags.label_dropped == 0  &&  SizeOfCount == 0 ) {
                if( sym.level != 0      &&
                    sym.attribs.stg_class != SC_STATIC &&       /* 15-feb-88 */
                    sym.attribs.stg_class != SC_EXTERN ) {
                    if( !(sym.flags & SYM_ASSIGNED) ) {
                      /* turn on flag so msg only comes out once per sym */
                        sym.flags |= SYM_ASSIGNED;
                        CWarn2p( WARN_SYM_NOT_ASSIGNED, ERR_SYM_NOT_ASSIGNED, SymName( &sym, tree->op.u2.sym_handle ) );
                    }
                }
            }
            if( symb_flags != sym.flags ) {
                SymReplace( &sym, tree->op.u2.sym_handle );
            }
        }
    }
    switch( tree->op.opr ) {
    case OPR_PUSHADDR:
        tree->op.opr = OPR_PUSHSYM;
        break;
    case OPR_QUESTION:
    case OPR_COMMA:
    case OPR_EQUALS:
        if( IsStruct( tree->u.expr_type ) ) {
            tree->op.flags |= OPFLAG_RVALUE;
        }
        break;
    case OPR_DOT:                       // sym.field
    case OPR_ARROW:                     // ptr->field
    case OPR_INDEX:                     // array[index]
        tree->op.flags |= OPFLAG_RVALUE;
        break;
    case OPR_CALL:                      // func()
        tree->op.flags |= OPFLAG_RVALUE;
        if( tree->left->op.opr == OPR_FUNCNAME ) {
            SymGet( &sym, tree->left->op.u2.sym_handle );
            if( sym.attribs.stg_class == SC_FORWARD && !(sym.flags & SYM_TYPE_GIVEN) ) {
                 sym.flags |= SYM_TYPE_GIVEN;
                 SymReplace( &sym, tree->left->op.u2.sym_handle );
            }
        }
        break;
    case OPR_POINTS:
        if( tree->left->op.opr == OPR_PUSHSTRING ) {    // *"abc"
            STRING_LITERAL      *string;

            string = tree->left->op.u2.string_handle;
            value = string->literal[0];
            typ = tree->left->u.expr_type;
            if( typ->object->decl_type == TYPE_USHORT ) {  /* 24-jul-92 */
                value = (string->literal[1] << 8) | value;
            }
            FreeExprTree( tree );
            tree = IntLeaf( value );
        } else {
            tree->op.flags |= OPFLAG_RVALUE;
        }
        break;
    }
    return( tree );
}


TREEPTR VoidRValue( TREEPTR tree )
{
    tree =  TakeRValue( tree, 1 );
    return( tree );
}

TREEPTR RValue( TREEPTR tree )
{
    tree =  TakeRValue( tree, 0 );
    return( tree );
}

//-----------------------------COPS------------------------------------
static TREEPTR AddrOp( TREEPTR tree )
{
    TYPEPTR         typ;
    TREEPTR         leaf;
    type_modifiers  modifiers;
    segment_id      segid;
//    SYM_HANDLE      based_sym;
    SYM_ENTRY       sym;

    if( tree->op.opr == OPR_ERROR )  return( tree );
    typ = tree->u.expr_type;
    if( typ->decl_type == TYPE_FIELD || typ->decl_type == TYPE_UFIELD ) {
        CErr1( ERR_CANT_TAKE_ADDR_OF_BIT_FIELD );
        return( ErrorNode( tree ) );
    } else if( typ->decl_type == TYPE_VOID ) {
        CErr1( ERR_CANT_TAKE_ADDR_OF_RVALUE );
        return( ErrorNode( tree ) );
    }
    if( tree->op.flags & OPFLAG_LVALUE_CAST ) {     /* 18-aug-95 */
        if( CompFlags.extensions_enabled ) {
            tree->op.flags &= ~(OPFLAG_LVALUE_CAST | OPFLAG_RVALUE);
            CWarn1( WARN_LVALUE_CAST, ERR_LVALUE_CAST );
        } else {
            CErr1( ERR_CANT_TAKE_ADDR_OF_RVALUE );
            return( ErrorNode( tree ) );
        }
    }
    if( (tree->op.opr == OPR_CONVERT || tree->op.opr == OPR_CONVERT_PTR)
      && CompFlags.extensions_enabled ) {
        tree = LCastAdj( tree );
    }
    leaf = tree;
    if( tree->op.opr == OPR_DOT ) {
        if( tree->left->op.opr == OPR_PUSHADDR ) {
            leaf = tree->left;
        }
    } else if( tree->op.opr == OPR_ARROW ) {
        // checking for offsetof macro construct
        // #define offsetof(typ,field) (size_t)&(((typ*)0)->field)
        if( tree->left->op.opr == OPR_PUSHINT ) {
            leaf = tree->left;
            leaf->op.u2.ulong_value += tree->right->op.u2.ulong_value;
            tree->left = NULL;
            FreeExprTree( tree );
            leaf->u.expr_type = PtrNode( typ, FLAG_NONE, SEG_DATA );
            return( leaf );
        }
    }
//    based_sym = 0;
    modifiers  = FLAG_NONE;
    segid = SEG_DATA;
    if( leaf->op.opr == OPR_PUSHADDR ) {
        SymGet( &sym, leaf->op.u2.sym_handle );
        if( sym.attribs.stg_class == SC_REGISTER ) {
            CErr1( ERR_CANT_TAKE_ADDR_OF_REGISTER );
        }
        if( sym.level != 0 ) {
            sym.flags |= SYM_ASSIGNED;
        }
        sym.flags |= SYM_ADDR_TAKEN | SYM_REFERENCED;
        SymReplace( &sym, leaf->op.u2.sym_handle );
    }
    if( tree->op.opr == OPR_PUSHADDR ) {
        SymGet( &sym, leaf->op.u2.sym_handle );
        modifiers = sym.mods;
        if( sym.attribs.stg_class == SC_AUTO ) {
            segid = SEG_STACK;
            CompFlags.addr_of_auto_taken = 1;           /* 23-oct-91 */
            if( TargetSwitches & FLOATING_SS ) {        /* 05-oct-88 */
                modifiers |= FLAG_FAR;
            }
        }
        typ = PtrNode( typ, modifiers & MASK_PTR, segid );
    } else if( tree->op.opr == OPR_POINTS ) {
        typ = tree->op.u2.result_type;
    } else {
        modifiers = FlagOps( tree->op.flags );
        typ = PtrNode( typ, modifiers, SEG_DATA );
    }
    if( IsLValue( tree ) ) {
        tree = ExprNode( NULL, OPR_ADDROF, tree );
    } else {
        CErr1( ERR_CANT_TAKE_ADDR_OF_RVALUE );
        return( ErrorNode( tree ) );
    }
    tree->u.expr_type = typ;
    return( tree );
}

static TREEPTR FarPtrCvt( SYMPTR sym, SYM_HANDLE sym_handle )
{
    TYPEPTR         typ;
    TREEPTR         tree;

    typ = sym->sym_type;
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type == TYPE_POINTER ) {
        if( typ->u.p.decl_flags & FLAG_FAR ) {
            tree = VarLeaf( sym, sym_handle );
            tree->op.opr = OPR_PUSHSYM;
        } else if( typ->u.p.decl_flags & FLAG_BASED ) {
            tree = VarLeaf( sym, sym_handle );
            tree->op.opr = OPR_PUSHSYM;
            tree = BasedPtrNode( typ, tree );
        } else {
            tree = VarLeaf( sym, sym_handle );
            tree->op.opr = OPR_PUSHSYM;
            tree = ExprNode( NULL, OPR_CONVERT, tree );
            tree->u.expr_type = typ;
            tree->op.u2.result_type = PtrNode( typ->object, FLAG_FAR, 0 );
        }
    } else {
        assert( 0 );
        tree = NULL;
    }
    return( tree );
}

static TREEPTR MakeFarOp( TREEPTR based_sym, TREEPTR tree )
{
    TYPEPTR     typ;

    typ = tree->u.expr_type;
    SKIP_TYPEDEFS( typ );
    typ = PtrNode( typ->object, FLAG_FAR, 0 );
    tree = ExprNode( based_sym, OPR_FARPTR, tree );
    tree->u.expr_type = typ;
    return( tree );
}

TREEPTR BasedPtrNode( TYPEPTR ptrtyp, TREEPTR tree )
{
    TREEPTR     based_sym;
    SYM_HANDLE  sym_handle;
    SYMPTR      sym;
    segment_id  segid;

    sym_handle = ptrtyp->u.p.based_sym;
    segid =  ptrtyp->u.p.segment;
    switch( ptrtyp->u.p.based_kind ) {
    case BASED_VOID:          //__based( void )       segment:>offset base op
        break;
    case BASED_NONE:
        break;
    case BASED_SELFSEG:       //__based( (__segment) __self ) use seg of self
        tree->op.flags &= ~OPFLAG_RVALUE;
        based_sym = ExprNode( tree, OPR_DUPE, NULL );
        based_sym->op.flags |= OPFLAG_RVALUE;
        based_sym->op.u2.result_type = tree->op.u2.result_type;
        tree = MakeFarOp( based_sym, tree );
        tree->right = NULL;
        break;
    case BASED_VAR:              //__based( <var> )             add offset to var pointer
      {
        TYPEPTR     typ;
        TYPEPTR     old;

        old = tree->u.expr_type;
        SKIP_TYPEDEFS( old );
        sym = SymGetPtr( sym_handle );
        based_sym = VarLeaf( sym, sym_handle );
        based_sym->op.opr = OPR_PUSHSYM;
        typ = sym->sym_type;
        SKIP_TYPEDEFS( typ );
        if( typ->u.p.decl_flags & FLAG_BASED ) {
            based_sym  = BasedPtrNode( typ, based_sym );
            typ = based_sym->u.expr_type;
            SKIP_TYPEDEFS( typ );
        }
        old = PtrNode( old->object, typ->u.p.decl_flags, 0 );
        tree = ExprNode( based_sym, OPR_ADD, tree );
        tree->u.expr_type = old;
        tree->op.u2.result_type = old;
      } break;
    case BASED_SEGVAR:           //__based( <var> )             add offset to seg var
        sym = SymGetPtr( sym_handle );
        based_sym = VarLeaf( sym, sym_handle );
        based_sym->op.opr = OPR_PUSHSYM;
        tree = MakeFarOp( based_sym, tree );
        break;
    case BASED_VARSEG:        //__based( (__segment) <var> ) ) use seg of var
        sym = SymGetPtr( sym_handle );
        based_sym = FarPtrCvt( sym, sym_handle );
        tree = MakeFarOp( based_sym, tree );
        break;
    case BASED_SEGNAME:       //__based( __segname( "name" )   use seg of segname
        if( sym_handle != 0 ) {
            sym = SymGetPtr( sym_handle );
            based_sym = VarLeaf( sym, sym_handle );
            based_sym->op.opr = OPR_PUSHSEG;
        } else { //error I guess
            based_sym = LeafNode( OPR_PUSHINT );
            based_sym->op.u1.const_type = TYPE_INT;
            based_sym->op.u2.long_value = segid;
            based_sym->u.expr_type =  GetType( TYPE_USHORT );
        }
        tree = MakeFarOp( based_sym, tree );
    }
    return( tree  );
}


static TREEPTR CheckBasedPtr( TREEPTR tree, TYPEPTR typ, type_modifiers *p_flags )
{
    type_modifiers  flags;

    flags = typ->u.p.decl_flags & MASK_PTR;
    if( flags & FLAG_BASED ) {
        tree = BasedPtrNode( typ, tree );
        flags &= ~(FLAG_NEAR | FLAG_BASED);
        if( typ->u.p.based_kind == BASED_VAR ) {
            SYM_HANDLE  base_sym_handle;
            SYMPTR      base_sym;
            TYPEPTR     base_typ;

            // For pointers based on another pointer, copy flags from the base ptr
            base_sym_handle = typ->u.p.based_sym;
            base_sym = SymGetPtr( base_sym_handle );
            base_typ = base_sym->sym_type;
            flags |= base_typ->u.p.decl_flags & MASK_PTR;
        } else {
            flags |= FLAG_FAR;
        }
    }
    *p_flags = flags;
    return( tree );
}


TREEPTR PtrOp( TREEPTR tree )
{
    TYPEPTR         ptrtyp;
    TYPEPTR         typ;
    type_modifiers  flags;

    if( tree->op.opr == OPR_ERROR )
        return( tree );
    typ = tree->u.expr_type;
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type != TYPE_POINTER ) {
        CErr1( ERR_EXPR_MUST_BE_POINTER_TO );
        return( ErrorNode( tree ) );
    }
    ptrtyp = typ;
    tree = CheckBasedPtr( tree, typ, &flags );
    tree = ExprNode( tree, OPR_POINTS, NULL );
    typ = typ->object;
//  SKIP_TYPEDEFS( typ );
    tree->u.expr_type = typ;
    tree->op.u2.result_type = ptrtyp;
    tree->op.flags = OpFlags( flags );
    if( SizeOfCount == 0 ) {
        if( tree->op.flags & OPFLAG_VOLATILE ) {
            CompFlags.useful_side_effect = 1;
        }
    }
    return( tree );
}



FIELDPTR SearchFields( TYPEPTR *class_typ, unsigned *field_offset, char *name )
{
    FIELDPTR    field;
    FIELDPTR    subfield;
    TYPEPTR     typ;
    TAGPTR      tag;

    tag = (*class_typ)->u.tag;
    field = tag->u.field_list;
    while( field != NULL ) {
        typ = field->field_type;
        SKIP_TYPEDEFS( typ );
        if( field->name[0] == '\0' ) {  /* if nameless field: 15-sep-90 */
            if( typ->decl_type == TYPE_STRUCT  ||
                typ->decl_type == TYPE_UNION ) {
                subfield = SearchFields( &typ, field_offset, name );
                if( subfield != NULL ) {
                    *field_offset += field->offset;
                    *class_typ = typ;
                    return( subfield );
                }
            }
        } else if( strcmp( name, field->name ) == 0 ) {
            if( typ->decl_type != TYPE_FUNCTION ) {
                *field_offset += field->offset;
            }
            return( field );
        }
        field = field->next_field;
    }
    return( NULL );
}


local TYPEPTR Ptr2Struct( TYPEPTR typ )
{
    if( typ->decl_type != TYPE_POINTER ) {
        return( NULL );
    }
    typ = typ->object;
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type == TYPE_STRUCT  ||
        typ->decl_type == TYPE_UNION ) {
            return( typ );
    }
    return( NULL );
}


TREEPTR DotOp( TREEPTR tree )
{
    TYPEPTR             typ;
    TYPEPTR             get_typ;
    FIELDPTR            field;
    unsigned            offset;
    op_flags            opflag;
    SYM_ENTRY           sym;

    if( CurToken != T_ID ) {
        CErr1( ERR_EXPECTING_ID );
        return( ErrorNode( tree ) );
    }
    if( tree->op.opr == OPR_ERROR )
        return( tree );
    typ = tree->u.expr_type;
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type != TYPE_STRUCT  &&
        typ->decl_type != TYPE_UNION ) {
        CErr1( ERR_MUST_BE_STRUCT_OR_UNION );
        return( ErrorNode( tree ) );
    }
    get_typ = typ;
    offset = 0;
    field = SearchFields( &get_typ, &offset, Buffer );
    if( field == NULL ) {
        CErr3p( ERR_NAME_NOT_FOUND_IN_STRUCT, Buffer, typ->u.tag->name );
        return( ErrorNode( tree ) );
    }
    typ = field->field_type;
    opflag = tree->op.flags | OpFlags( field->attrib );
    if( CompFlags.emit_browser_info ) {                 /* 27-oct-94 */
        field->xref->next_xref = NewXref( field->xref->next_xref );
    }
    if( tree->op.opr == OPR_DOT || tree->op.opr == OPR_ARROW ) {
        tree->right->op.u2.ulong_value += offset;
    } else {
        if( tree->op.opr == OPR_PUSHADDR ) {
            SymGet( &sym, tree->op.u2.sym_handle );
            if( !(sym.flags & SYM_ASSIGNED) ) {
                sym.flags |= SYM_ASSIGNED;
                SymReplace( &sym, tree->op.u2.sym_handle );
            }
        }
        tree = ExprNode( tree, OPR_DOT, UIntLeaf( offset ) );
    }
    tree->u.expr_type = typ;
    tree->op.u2.result_type = typ;
    tree->op.flags = opflag;
    return( tree );
}


TREEPTR ArrowOp( TREEPTR tree )
{
    TYPEPTR             typ;
    TYPEPTR             get_typ;
    FIELDPTR            field;
    unsigned            offset;
    type_modifiers      flags;

    if( CurToken != T_ID ) {
        CErr1( ERR_EXPECTING_ID );
        return( ErrorNode( tree ) );
    }
    if( tree->op.opr == OPR_ERROR )
        return( tree );
    typ = tree->u.expr_type;
    SKIP_TYPEDEFS( typ );
    if( Ptr2Struct( typ ) == NULL ) {
        CErr1( ERR_MUST_BE_PTR_TO_STRUCT_OR_UNION );
        return( ErrorNode( tree ) );
    }
    if( tree->op.opr == OPR_PUSHADDR ) {
        tree->op.opr = OPR_PUSHSYM;
    }
    tree = CheckBasedPtr( tree, typ, &flags );
    typ = typ->object;
    SKIP_TYPEDEFS( typ );
    get_typ = typ;
    offset = 0;
    field = SearchFields( &get_typ, &offset, Buffer );
    if( field == NULL ) {
        CErr3p( ERR_NAME_NOT_FOUND_IN_STRUCT, Buffer, typ->u.tag->name );
        return( ErrorNode( tree ) );
    }
    if( CompFlags.emit_browser_info ) {                 /* 27-oct-94 */
        field->xref->next_xref = NewXref( field->xref->next_xref );
    }
    tree = ExprNode( tree, OPR_ARROW, UIntLeaf( offset ) );
    typ = field->field_type;
    tree->u.expr_type = typ;
    tree->op.u2.result_type = typ;
    flags |= field->attrib;
    tree->op.flags |= OpFlags( flags );
    return( tree );
}

TREEPTR IncDec( TREEPTR tree, TOKEN opr )
{
    op_flags    volatile_flag;

    volatile_flag = tree->op.flags & OPFLAG_VOLATILE;
    ChkConst( tree );
    tree = AddOp( tree, opr, IntLeaf( 1 ) );
    tree->op.flags |= volatile_flag;
    CompFlags.meaningless_stmt = 0;
    CompFlags.useful_side_effect = 1;
    return( tree );
}
//-----------------------------CEXPR-----------------------------------

bool ConstExprAndType( const_val *val )
{
    TREEPTR     tree;
    call_list   **save;
    bool        ret;

    save = FirstCallLnk;
    ++Level;                    /* allow for nested expressions */
    tree = SingleExpr();
    --Level;                    /* put Level back */
    FirstCallLnk = save;
    val->type = TYPE_INT;
    switch( tree->op.opr ) {
    case OPR_PUSHINT:
        val->type = tree->op.u1.const_type;
        switch( tree->op.u1.const_type ) {
        case TYPE_LONG64:
        case TYPE_ULONG64:
            val->value = tree->op.u2.long64_value;
            break;
        case TYPE_ULONG:
        case TYPE_UINT:
            U32ToU64( tree->op.u2.long_value, &val->value );
            break;
        default:
            I32ToI64( tree->op.u2.long_value, &val->value );
            break;
        }
        ret = TRUE;
        break;
    case OPR_PUSHFLOAT:
        CErr1( ERR_EXPR_MUST_BE_INTEGRAL );
        I32ToI64( (long)atof( tree->op.u2.float_value->string ), &val->value );
        ret = FALSE;
        break;
    default:
        if( tree->op.opr != OPR_ERROR ) {
            CErr1( ERR_NOT_A_CONSTANT_EXPR );
        }
        U64Clear( val->value );
        ret = FALSE;
        break;
    }
    FreeExprTree( tree );
    return( ret );
}

int ConstExpr( void )
{
    const_val   val;

    ConstExprAndType( &val );
    if( (val.type == TYPE_ULONG64) && !U64IsI32( val.value ) ) {
        CErr1( ERR_CONSTANT_TOO_BIG );
    } else if( (val.type == TYPE_LONG64) && !I64IsI32( val.value ) ) {
        CErr1( ERR_CONSTANT_TOO_BIG );
    }
    return( I32FetchTrunc( val.value ) );
}


TREEPTR Expr( void )
{
    Class[ Level ] = TC_START;
    return( GetExpr() );
}


TREEPTR CommaExpr( void )
{
    Class[ Level ] = TC_START1;
    return( GetExpr() );
}


TREEPTR AddrExpr( void )
{
    return( SingleExpr() );
}


TREEPTR SingleExpr( void )
{
    TREEPTR     tree;

    Class[ Level ] = TC_START2;
    tree = GetExpr();
    FoldExprTree( tree );
    return( tree );
}


local TREEPTR GetExpr( void )
{
    TREEPTR     tree, op1;
    TYPEPTR     typ;
    token_class curclass;
    TYPEPTR     *plist;

    CompFlags.useful_side_effect = 0;
    FirstCallLnk = LastCallLnk;
    tree = 0;
    plist = NULL;
    for( ;; ) {
        if( tree == 0 )
            tree = ExprOpnd();
        curclass = TokenClass[ CurToken ];
        while( curclass <= Class[ Level ] ) {
            op1 = ValueStack[ Level ];

            /* the following cases are listed from lowest to highest
               priority */
            switch( Class[ Level ] ) {
            case TC_START:
            case TC_START1:
            case TC_START2:
            case TC_START_UNARY:
                return( tree );
            case TC_LEFT_PAREN:         /* bracketed expression */
                MustRecog( T_RIGHT_PAREN );
                curclass = TokenClass[ CurToken ];
                break;
            case TC_COMMA:
                if( op1->op.opr == OPR_ERROR ) {
                    FreeExprTree( tree );
                    return( op1 );
                }
                if( tree->op.opr == OPR_ERROR ) {
                    FreeExprTree( op1 );
                    return( tree );
                }
                op1 = VoidRValue( op1 );
                typ = TypeOf( tree );
                if( !IsStruct( typ ) ) {
                    tree = VoidRValue( tree );
                }
                tree = ExprNode( op1, OPR_COMMA, tree );
                tree->u.expr_type = typ;
                tree->op.u2.result_type = typ;
                break;
            case TC_ASSIGNMENT:  /*  = += -= *= /= %= >>= <<= &= ^= |=  */
                ChkConst( op1 );
                tree = AsgnOp( op1, Token[ Level ], tree );
                CompFlags.meaningless_stmt = 0;
                CompFlags.useful_side_effect = 1;
                break;
            case TC_TERNARY:
                MustRecog( T_COLON );
                break;
            case TC_TERNARY_DONE:
                CompFlags.meaningless_stmt &= Token[ Level ];/*13-mar-93*/
                --Level;
                tree = TernOp( ValueStack[ Level ], op1, tree );
                CompFlags.pending_dead_code = 0;
                break;
            case TC_OR_OR:
                if( Token[ Level ] == 0 ) {
                    BoolExpr( tree );   /* checks type of op2 */
                    --SizeOfCount;
                    tree = op1;
                } else {
                    tree = FlowOp( op1, OPR_OR_OR, BoolExpr( tree ) );
                }
                CompFlags.meaningless_stmt = 1;
                CompFlags.pending_dead_code = 0;
                break;
            case TC_AND_AND:
                if( Token[ Level ] == 0 ) {
                    BoolExpr( tree );   /* checks type of op2 */
                    --SizeOfCount;
                    tree = op1;
                } else {
                    tree = FlowOp( op1, OPR_AND_AND, BoolExpr( tree ) );
                }
                CompFlags.meaningless_stmt = 1;
                CompFlags.pending_dead_code = 0;
                break;
            case TC_OR:
            case TC_XOR:
            case TC_AND:
                tree = IntOp( op1, Token[ Level ], tree );
                CompFlags.meaningless_stmt = 1;
                break;
            case TC_EQ_NE:
            case TC_REL_OP:
                tree = RelOp( op1, Token[ Level ], tree );
                CompFlags.meaningless_stmt = 1;
                break;
            case TC_SHIFT_OP:
                tree = ShiftOp( op1, Token[ Level ], tree );
                CompFlags.meaningless_stmt = 1;
                break;
            case TC_ADD_OP:
                tree = AddOp( op1, Token[ Level ], tree );
                CompFlags.meaningless_stmt = 1;
                break;
            case TC_MUL_OP:
                tree = BinOp( RValue(op1), Token[ Level ], tree );
                CompFlags.meaningless_stmt = 1;
                break;
            case TC_PREINC:
                tree = IncDec( tree, Token[ Level ] );
                break;
            case TC_ADDR:
                tree = AddrOp( tree );
                CompFlags.meaningless_stmt = 1;
                break;
            case TC_EXCLAMATION:
                tree = NotOp( RValue( tree ) );
                CompFlags.meaningless_stmt = 1;
                break;
            case TC_PLUS:                       /* unary plus */
                tree = UnaryPlus( tree );
                CompFlags.meaningless_stmt = 1;
                break;
            case TC_MINUS:                      /* unary minus */
                tree = UMinus( tree );
                CompFlags.meaningless_stmt = 1;
                break;
            case TC_TILDE:
                tree = UComplement( tree );
                CompFlags.meaningless_stmt = 1;
                break;
            case TC_SIZEOF:
                typ = tree->u.expr_type;
                FreeExprTree( tree );
                if( typ != NULL ) {
                    tree = SizeofOp( typ );
                } else {
                    tree = UIntLeaf( 1 ); //error use this as default
                }
                --SizeOfCount;
                CompFlags.meaningless_stmt = 1;
                break;
            case TC_INDIRECTION:
                tree = PtrOp( RValue( tree ) );
                CompFlags.meaningless_stmt = 1;
                break;
            case TC_CAST:
                tree = CnvOp( tree, TypeOf( op1 ), 1 );
                FreeExprTree( op1 );
                break;
            case TC_INDEX:
                tree = IndexOp( op1, tree );
                curclass = TokenClass[ CurToken ];
                CompFlags.meaningless_stmt = 1;
                break;
            case TC_SEG_OP:                             /* 23-oct-91 */
                tree = SegOp( op1, tree );
                CompFlags.meaningless_stmt = 1;
                break;
            case TC_PARM_LIST:          /* do func call */
                {
                SYMPTR      sym;
                int         n;
                TREEPTR     functree;

                // find the corresponding function symbol
                n = Level;
                while( Token[ n ] != T_LEFT_PAREN ) {
                    --n;
                }
                functree = ValueStack[ n ];
                sym = SymGetPtr( functree->op.u2.sym_handle );
                if( !(sym->flags & SYM_TEMP) )
                    SetDiagSymbol( sym, functree->op.u2.sym_handle );
                tree = GenNextParm( tree, &plist );
                tree = GenFuncCall( tree );
                if( plist != NULL ) {   // function has prototype
                    if( *plist != NULL && (*plist)->decl_type != TYPE_DOT_DOT_DOT ) {
                        CErr1( ERR_PARM_COUNT_MISMATCH );
                    }
                } else {
                    AddCallNode( tree );
                }
                if( !(sym->flags & SYM_TEMP) )
                    SetDiagPop();
                PopNestedParms( &plist );
                curclass = TokenClass[ CurToken ];
                CompFlags.meaningless_stmt = 0;
                CompFlags.useful_side_effect = 1;
                }
                break;
            }
            --Level;
        }
        switch( curclass ) {
        case TC_RIGHT_PAREN:                    /* 27-feb-94 */
            CErr1( ERR_UNEXPECTED_RIGHT_PAREN );
            NextToken();
            continue;
        case TC_QUESTION:
            tree = StartTernary( tree );
            curclass = TC_TERNARY;
            break;
        case TC_COLON:
            tree = ColonOp( tree );
            CurToken = CompFlags.meaningless_stmt;
            curclass = TC_TERNARY_DONE;
            break;
        case TC_OR_OR:
            tree = BoolExpr( tree );
            tree = OrOr( tree );
            break;
        case TC_AND_AND:
            tree = BoolExpr( tree );
            tree = AndAnd( tree );
            break;
        case TC_OR:
        case TC_XOR:
        case TC_AND:
        case TC_EQ_NE:
        case TC_REL_OP:
        case TC_SHIFT_OP:
        case TC_ADD_OP:
        case TC_MUL_OP:
/*              tree = RValue( tree );   17-jan-88 */
            break;
        case TC_ASSIGN_OP:
            curclass = TC_ASSIGNMENT;
            break;
        case TC_LEFT_BRACKET:
            curclass = TC_INDEX;
            break;
        case TC_COMMA:
            if( Class[ Level ] != TC_PARM_LIST ) {
                if( CompFlags.meaningless_stmt == 1 ) {
                    if( CompFlags.useful_side_effect ) {
                        CWarn1( WARN_USEFUL_SIDE_EFFECT, ERR_USEFUL_SIDE_EFFECT );
                    } else {
                        CWarn1( WARN_MEANINGLESS, ERR_MEANINGLESS );
                    }
                }
            } else {
                tree = GenNextParm( tree, &plist );
                curclass = TC_PARM_LIST;
            }
            break;
        case TC_FUNC_CALL:
            tree = StartFunc( tree, &plist );
            continue;
        case TC_DOT:
            NextToken();
            tree = DotOp( tree );
            if( CurToken != T_SEMI_COLON ) NextToken();
            CompFlags.meaningless_stmt = 1;
            continue;
        case TC_ARROW:
            NextToken();
            tree = ArrowOp( RValue( tree ) );
            if( CurToken != T_SEMI_COLON ) NextToken();
            CompFlags.meaningless_stmt = 1;
            continue;
        case TC_POSTINC:
            tree = IncDec( tree, CurToken );
            NextToken();
            continue;
        }
        if( Level >= (MAX_LEVEL - 1) ) {
            CErr1( ERR_EXPR_TOO_COMPLICATED );
            CSuicide();
        }
        if( tree != 0 ) {
            ++Level;
            ValueStack[ Level ] = tree;
            Class[ Level ] = curclass;
            Token[ Level ] = CurToken;
            tree = 0;               /* indicate need opnd */
        }
        NextToken();
    }
}


local TREEPTR ExprOpnd( void )
{
    TREEPTR     tree;
    TYPEPTR     typ;

    CompFlags.meaningless_stmt = 1;
    for( ;; ) {
        ++Level;
        switch( CurToken ) {
        case T_PLUS_PLUS:
        case T_MINUS_MINUS:
            Token[ Level ] = CurToken - 1;
            Class[ Level ] = TC_PREINC;
            NextToken();
            continue;
        case T_AND:
            Class[ Level ] = TC_ADDR;
            NextToken();
            continue;
        case T_EXCLAMATION:
            Class[ Level ] = TC_EXCLAMATION;
            NextToken();
            continue;
        case T_PLUS:
            Class[ Level ] = TC_PLUS;
            NextToken();
            continue;
        case T_MINUS:
            Class[ Level ] = TC_MINUS;
            NextToken();
            continue;
        case T_TILDE:
            Class[ Level ] = TC_TILDE;
            NextToken();
            continue;
        case T_SIZEOF:
            if( CompFlags.pre_processing ) {
                CErr1( ERR_NO_SIZEOF_DURING_PP );
            }
            Class[ Level ] = TC_SIZEOF;
            NextToken();
            Token[ Level ] = CurToken;
            if( CurToken == T_LEFT_PAREN ) {
                NextToken();
                typ = TypeName();
                if( typ != NULL ) {
                    tree = SizeofOp( typ );
                    MustRecog( T_RIGHT_PAREN );
                    break;
                }
                ++Level;
                Class[ Level ] = TC_LEFT_PAREN;
            }
            ++SizeOfCount;
            continue;
        case T___BUILTIN_ISFLOAT:
            NextToken();
            if( CurToken == T_LEFT_PAREN ) {
                NextToken();
                typ = TypeName();
                if( typ != NULL ) {
                    SKIP_TYPEDEFS( typ );
                    switch( typ->decl_type ) {
                    case TYPE_FLOAT:
                    case TYPE_DOUBLE:
                        tree = IntLeaf( 1 );
                        break;
                    default:
                        tree = IntLeaf( 0 );
                        break;
                    }
                } else {
                    CErr1( ERR_OPND_OF_BUILTIN_ISFLOAT_MUST_BE_TYPE );
                    NextToken();
                    tree = LeafNode( OPR_ERROR );
                }
                MustRecog( T_RIGHT_PAREN );
            } else {
                MustRecog( T_LEFT_PAREN );
                tree = LeafNode( OPR_ERROR );
            }
            break;
        case T_TIMES:
            Class[ Level ] = TC_INDIRECTION;
            NextToken();
            continue;
        case T_LEFT_PAREN:
            NextToken();
            typ = TypeName();
            if( typ != NULL ) {
                if( CompFlags.pre_processing ) {
                    CErr1( ERR_NO_CAST_DURING_PP );
                }
                MustRecog( T_RIGHT_PAREN );
                Class[ Level ] = TC_CAST;
                tree = LeafNode( OPR_CAST );
                tree->u.expr_type = typ;
                ValueStack[ Level ] = tree;
            } else {
                Class[ Level ] = TC_LEFT_PAREN;
            }
            continue;
        case T_ID:
            tree = ExprId();
            CompFlags.meaningless_stmt = 0;             /* 07-apr-93 */
            break;
        case T_SAVED_ID:
            tree = SymLeaf();
            CMemFree( SavedId );
            SavedId = NULL;
            CurToken = LAToken;
            CompFlags.meaningless_stmt = 0;             /* 07-apr-93 */
            break;
        case T_CONSTANT:
            tree = ConstLeaf();
            NextToken();
            break;
        case T_STRING:
            if( CompFlags.strings_in_code_segment ) {
                tree = StringLeaf( FLAG_CONST );        /* 01-sep-89 */
            } else {
                tree = StringLeaf( 0 );
            }
            break;
        case T_BAD_TOKEN:
            CErr1( BadTokenInfo );
            BadTokenInfo = 0;
            NextToken();
            tree = ErrorNode( NULL );
            break;
        case T___OW_IMAGINARY_UNIT:
            {
            FLOATVAL    *flt;

            tree = LeafNode( OPR_PUSHFLOAT );
            tree->op.u1.const_type = TYPE_DIMAGINARY;
            flt = CMemAlloc( sizeof( FLOATVAL ) + 3 );
            flt->string[0] = '+';
            strcpy( &flt->string[1], "1.0" );
            flt->len = 3 + 1;
            flt->type = TYPE_DIMAGINARY;
            flt->next = NULL;
            tree->op.u2.float_value = flt;
            tree->op.opr = OPR_PUSHFLOAT;
            tree->u.expr_type = GetType( tree->op.u1.const_type );
            }
            NextToken();
            break;
        default:
            if( CompFlags.pre_processing && IS_KEYWORD( CurToken ) ) {
                tree = ExprId();
            } else {
                CErr1( ERR_MISSING_OPERAND );
                tree = ErrorNode( NULL );
            }
        }
        break;
    }
    --Level;
    return( tree );
}


local int IsMacroDefined( void )
{
    MEPTR       mentry;

    if( CurToken != T_ID ) {
        ExpectIdentifier();
    } else {
        mentry = MacroLookup( Buffer );
        if( mentry != NULL ) {
            mentry->macro_flags |= MFLAG_REFERENCED;
            return( 1 );
        }
    }
    return( 0 );
}


local TREEPTR ExprId( void )
{
    TREEPTR     tree;
    int         value;
    int         count;

    if( CompFlags.pre_processing ) {
        if( strcmp( Buffer, "defined" ) == 0 ) {
            ppctl_t old_ppctl;

            old_ppctl = CompFlags.pre_processing;
            PPCTL_DISABLE_MACROS();    /* don't want macro expanded */
            NextToken();
            if( CurToken == T_LEFT_PAREN ) {
                NextToken();
                value = IsMacroDefined();
                NextToken();
                MustRecog( T_RIGHT_PAREN );
                CompFlags.pre_processing = old_ppctl;
            } else {
                value = IsMacroDefined();
                CompFlags.pre_processing = old_ppctl;
                NextToken();
            }
        } else {
//          if( SizeOfCount == 0 ) {                    /* 17-jul-94 */
//              CWarn2p( WARN_UNDECLARED_PP_SYM, ERR_UNDECLARED_PP_SYM, Buffer);
//          }
            NextToken();
            if( CurToken == T_LEFT_PAREN ) {    /* 26-may-89 */
                count = 0;
                for(;;) {               /* flush to ')' or end of line */
                    NextToken();
                    if( CurToken == T_NULL ) break;
                    if( CurToken == T_LEFT_PAREN )
                        ++count;
                    if( CurToken == T_RIGHT_PAREN ) {
                        if( count == 0 ) {
                            NextToken();
                            break;
                        }
                        --count;
                    }
                }
            }
            value = 0;
        }
        tree = IntLeaf( value );
    } else {
        tree = SymLeaf();
    }
    return( tree );
}


#if 0
local int LValueArray( TREEPTR tree )
{
    TYPEPTR     typ;

    if( tree->op.opr == OPR_PUSHSTRING ) {
        return( 1 );
    } else if( IsLValue( tree ) ) {
        return( 1 );
    } else {
        typ = tree->u.expr_type;
        SKIP_TYPEDEFS( typ );
        if( typ->decl_type == TYPE_POINTER ) {
            return( 1 );
        }
    }
    return( 0 );
}
#endif

local TREEPTR GenIndex( TREEPTR tree, TREEPTR index_expr )
{
    TYPEPTR         typ;
    op_flags        tree_flags;

//  if( ! LValueArray( tree ) ) {
//      CErr1( ERR_CANT_TAKE_ADDR_OF_RVALUE );
//      FreeExprTree( index_expr );
//      return( ErrorNode( tree ) );
//  }
    index_expr = RValue( index_expr );
    if( DataTypeOf( TypeOf( index_expr ) ) > TYPE_ULONG64 ) {
        CErr1( ERR_EXPR_MUST_BE_INTEGRAL );
        FreeExprTree( tree );
        return( ErrorNode( index_expr ) );
    }
    if( TypeOf( index_expr )->type_flags & TF2_TYPE_PLAIN_CHAR ) {
        CWarn1( WARN_PLAIN_CHAR_SUBSCRIPT, ERR_PLAIN_CHAR_SUBSCRIPT );
    }
    typ = tree->u.expr_type;
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type == TYPE_ARRAY ) {
        tree_flags = tree->op.flags;  // get modifiers of array obj
    } else if( typ->decl_type == TYPE_POINTER ) {
        type_modifiers  flags;

        tree = CheckBasedPtr( tree, typ, &flags );
        // We will indirect so get modifiers of indirected obj
        tree_flags = OpFlags( flags );
    } else {
        CErr2p( ERR_FATAL_ERROR, "Bad array index tree" );
        tree_flags = OPFLAG_NONE;
    }
    typ = typ->object;
    SKIP_TYPEDEFS( typ );
    // Some crappy little optimization that probably does nothing
    if( index_expr->op.opr == OPR_PUSHINT  &&
        tree->u.expr_type->decl_type == TYPE_ARRAY ) {
        index_expr->op.u2.long_value *= SizeOfArg( typ );
        tree = ExprNode( tree, OPR_DOT, index_expr );
#if _CPU == 8086
        if( index_expr->op.u2.long_value > 0x7FFF ) {
            index_expr->op.u1.const_type = TYPE_LONG;
            index_expr->u.expr_type = GetType( TYPE_LONG );
        }
#endif
    } else {
#if _CPU == 8086
        if( DataTypeOf( TypeOf( index_expr ) ) == TYPE_UINT ) {
            if( (tree_flags & OPFLAG_HUGEPTR) ||
               ((TargetSwitches & (BIG_DATA | CHEAP_POINTER)) == BIG_DATA &&
                (tree_flags & (OPFLAG_NEARPTR | OPFLAG_FARPTR)) == 0) ) {
                index_expr = CnvOp( index_expr, GetType( TYPE_LONG ), 0 );
            }
        }
#endif
        tree = ExprNode( tree, OPR_INDEX, index_expr );
    }
    tree->u.expr_type = typ;
    tree->op.u2.result_type = typ;
    tree->op.flags = tree_flags;
    return( tree );
}

local TREEPTR ArrayIndex( TREEPTR tree, TREEPTR index_expr )
{
    tree = GenIndex( tree, index_expr );
    return( tree );
}


local TREEPTR IndexOp( TREEPTR tree, TREEPTR index_expr )
{
    TYPEPTR     typ;

    if( tree->op.opr == OPR_ERROR )
        return( tree );
    typ = tree->u.expr_type;
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type == TYPE_ARRAY ) {
        tree = ArrayIndex( tree, index_expr );
    } else if( typ->decl_type == TYPE_POINTER ) {
        tree = GenIndex( RValue( tree ), index_expr );
    } else {                    /* try index[array] */
        typ = index_expr->u.expr_type;
        SKIP_TYPEDEFS( typ );
        if( typ->decl_type == TYPE_ARRAY ) {
            tree = ArrayIndex( index_expr, tree );
        } else if( typ->decl_type == TYPE_POINTER ) {
            tree = GenIndex( RValue( index_expr ), tree );
        } else {
            CErr1( ERR_EXPR_MUST_BE_ARRAY );
            FreeExprTree( index_expr );
            tree = ErrorNode( tree );
        }
    }
    MustRecog( T_RIGHT_BRACKET );
    return( tree );
}

local void AddCallNode( TREEPTR tree )
// if a function call has no prototype wait till end
// to check it out
{
    call_list   *new;

    if( tree->left != NULL ) {  // could be errors
        new = CMemAlloc( sizeof( call_list ) );
        new->next = NULL;
        new->callnode = tree;
        new->src_loc = SrcLoc;
        *LastCallLnk = new;
        LastCallLnk =  &new->next;
    }
}

void ChkCallNode( TREEPTR tree )
{
    call_list   **lnk;
    call_list   *curr;

    lnk = FirstCallLnk;
    while( (curr = *lnk) != NULL ) {
        if( curr->callnode == tree ) {
            curr->callnode = NULL;
            break;
        }
        lnk = &curr->next;
    }
}

static int ParmNum( void )
// get current parm num
{
    int     parm_count, n;

    parm_count = 1;
    n = Level;
    while( Token[ n ] != T_LEFT_PAREN ) {
        --n;
        ++parm_count;
    }
    return( parm_count );
}

local TREEPTR GenNextParm( TREEPTR tree, TYPEPTR **plistptr )
{
    TYPEPTR     *plist;
    TYPEPTR     typ;
    TYPEPTR     typ2;
    TYPEPTR     parm_typ;

    tree = RValue( tree );
    if( tree->op.opr == OPR_ERROR )
        return( tree );
    typ = SkipTypeFluff( tree->u.expr_type );
    plist = *plistptr;
    if( plist != NULL ) {
        if( *plist == NULL ) {  // To many parm trees
#if _CPU == 386
            /* can allow wrong number of parms with -3s option; 06-dec-91 */
            if( !CompFlags.register_conventions ) {
                CWarn1( WARN_PARM_COUNT_MISMATCH, ERR_PARM_COUNT_WARNING );
            } else {
                CErr1( ERR_PARM_COUNT_MISMATCH );           /* 18-feb-90 */
            }
#else
            CErr1( ERR_PARM_COUNT_MISMATCH );               /* 18-feb-90 */
#endif
            *plist = GetType( TYPE_DOT_DOT_DOT );           //shut up message
            plist = NULL;
        } else if( (*plist)->decl_type == TYPE_DOT_DOT_DOT ) {
            plist = NULL;
        } else if( (*plist)->decl_type == TYPE_VOID ) {
            return( tree );
        }
    }
    if( plist != NULL ) {  //do conversion from tree to parm type
        parm_typ = *plist;
        ParmAsgnCheck( parm_typ, tree, ParmNum(), TRUE );
        if( parm_typ != NULL ) {
            parm_typ = SkipTypeFluff( parm_typ );
            if( parm_typ != typ ) {
                tree = FixupAss( tree, parm_typ );
            }
            ++*plistptr;
        }
    } else {
        if( typ->decl_type == TYPE_FLOAT ) { // default conversions
            typ = GetType( TYPE_DOUBLE );
            tree = FixupAss( tree, typ );
        } else if( typ->decl_type == TYPE_POINTER ) {   /* 17-nov-88 */
            typ2 = typ->object;
            SKIP_TYPEDEFS( typ );
            if( typ2->decl_type != TYPE_FUNCTION ) {
                if( typ->u.p.decl_flags & FLAG_NEAR ) {
                    if( DataPtrSize == TARGET_FAR_POINTER ) {
                        typ = PtrNode( typ2, FLAG_NONE, SEG_DATA );
                        tree = FixupAss( tree, typ );
                    }
                }
            }
        }
    }
    if( typ->decl_type == TYPE_POINTER )
        typ = typ->object;
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type == TYPE_DOUBLE ||
        typ->decl_type == TYPE_LONG_DOUBLE ||
        typ->decl_type == TYPE_FLOAT ) {
        CompFlags.float_used = 1;
    }
    return( tree );
}


local int IntrinsicMathFunc( SYM_NAMEPTR sym_name, int i, int n, SYMPTR sym )
{
    char    func_name[6];

    if( far_strcmp( sym_name, MathFuncs[i].name, n ) == 0 )
        return( 1 );
    if( sym->flags & SYM_INTRINSIC ) {
        strcpy( func_name, &MathFuncs[i].name[2] ); /* copy name without __ */
        strlwr( func_name );
        if( far_strcmp( sym_name, func_name, n ) == 0 )
            return( 1 );
    }
    return( 0 );        /* indicate not a math intrinsic function */
}

#if (_CPU == _AXP) || (_CPU == _PPC) || (_CPU == _MIPS)
// This really ought to be defined somewhere else...
#if (_CPU == _AXP)
    #define REG_SIZE    8
#else
    #define REG_SIZE    4
#endif
local TREEPTR GenVaStartNode( TREEPTR last_parm )
{
    // there should be 3 parms __builtin_va_start( list, parm_name, stdarg )
    // - first parm should be name of va_list
    // - second parm should be name of the parameter before the ...
    // - last_parm should be an integer 0 or 1 (0=>varargs, 1=>stdarg)

    SYMPTR      sym;
    SYM_HANDLE  parms_list;
    int         offset;
    TREEPTR     parmsym;
    TREEPTR     tree;

    tree = NULL;
    if( Level >= 2 &&
        Token[Level] != T_LEFT_PAREN &&
        Token[Level-1] != T_LEFT_PAREN &&
        Token[Level-2] == T_LEFT_PAREN ) {
        offset = 0;
        if( last_parm->op.opr == OPR_PUSHINT &&
            last_parm->op.u2.long_value == 0 ) {           // varargs.h
            offset = -REG_SIZE;
        }
        parmsym = ValueStack[Level];            // get name of parameter
        parms_list = 0;
        if( parmsym->op.opr == OPR_PUSHSYM ) {
            parms_list = CurFunc->u.func.parms;
            while( parms_list != 0 ) {
                sym = SymGetPtr( parms_list );
                offset += _RoundUp( SizeOfArg( sym->sym_type ), REG_SIZE );
                if( parms_list == parmsym->op.u2.sym_handle )
                    break;
                parms_list = sym->handle;
            }
        }
        if( offset == 0 || parms_list == 0 ) {
            // error: name not found in parm list
            sym = SymGetPtr( parmsym->op.u2.sym_handle );
            CErr2p( ERR_SYM_NOT_IN_PARM_LIST,
                        SymName( sym, parmsym->op.u2.sym_handle ) );
        }
        FreeExprNode( parmsym );
        tree = ValueStack[ Level - 1 ];
        if( tree->op.opr == OPR_PUSHSYM ) {
            tree->op.opr = OPR_PUSHADDR;        // want address of va_list
        }
        tree->op.flags &= ~OPFLAG_RVALUE;
        tree = ExprNode( tree, OPR_VASTART, IntLeaf(offset) );
        Level -= 2;
    } else {
        // error
    }
    return( tree );
}

local TREEPTR GenAllocaNode( TREEPTR size_parm )
{
    // there should be 1 parm __builtin_alloca( size )
    TREEPTR     tree;

    if( Token[Level] == T_LEFT_PAREN ) {
        tree = ExprNode( 0, OPR_ALLOCA, size_parm );
        tree->u.expr_type = PtrNode( GetType( TYPE_VOID ), FLAG_NONE, SEG_STACK );
    } else {
        // error
        tree = 0;
    }
    return( tree );
}
#endif

#if _CPU == _PPC
local TREEPTR GenVaArgNode( TREEPTR last_parm )
{
    // there should be 2 parms __builtin_varg( list, type_arg )
    // - first parm should be name of va_list
    // - last_parm should be an integer 0 or 1 (0=>varargs, 1=>stdarg)

    TREEPTR     parmsym;
    TREEPTR     tree;

    tree = NULL;
    if( Level >= 2 &&
        Token[Level] != T_LEFT_PAREN &&
        Token[Level-1] != T_LEFT_PAREN &&
        Token[Level-2] == T_LEFT_PAREN ) {
        if( last_parm->op.opr == OPR_PUSHINT &&
            last_parm->op.u2.long_value == 0 ) {           // varargs.h
        }
        parmsym = ValueStack[Level];            // get name of parameter
        FreeExprNode( parmsym );
        tree = ValueStack[ Level - 1 ];
        if( tree->op.opr == OPR_PUSHSYM ) {
            tree->op.opr = OPR_PUSHADDR;        // want address of va_list
        }
        tree->op.flags &= ~OPFLAG_RVALUE;
        tree = ExprNode( tree, OPR_VASTART, IntLeaf( 0) );
        Level -= 2;
    } else {
        // error
    }
    return( tree );
}
#endif

local TREEPTR GenFuncCall( TREEPTR last_parm )
{
    int             i, n;
    TREEPTR         functree;
    TREEPTR         tree;
    TYPEPTR         func_result;
    TYPEPTR         typ;
    type_modifiers  flags;
    unsigned        string_len;
    int             optimized;
//    int             recursive = 0;
    int             far16_func = 0;
    unsigned char   parm_count;
    SYM_NAMEPTR     sym_name;
    SYM_ENTRY       sym;

    flags = 0;
    parm_count = 1;
    n = Level;
    while( Token[ n ] != T_LEFT_PAREN ) {
        --n;
        ++parm_count;
    }
    functree = ValueStack[ n ];
    typ = TypeOf( functree );
    if( typ->decl_type == TYPE_FUNCTION ) {
        SymGet( &sym, functree->op.u2.sym_handle );
        sym_name = SymName( &sym, functree->op.u2.sym_handle );
        flags = sym.mods;
        typ = typ->object;
        SKIP_TYPEDEFS( typ );
        if( typ->decl_type == TYPE_DOUBLE ||    /* 20-apr-89 */
            typ->decl_type == TYPE_LONG_DOUBLE ||
            typ->decl_type == TYPE_FLOAT ) {
            CompFlags.float_used = 1;
        }
#if _CPU == 386                                 /* 22-aug-94 */
        {
            aux_info    *inf;

            inf = FindInfo( &sym, functree->op.u2.sym_handle );
            if( (inf->flags & AUX_FLAG_FAR16) || (sym.mods & FLAG_FAR16) ) {
                far16_func = 1;
                typ = Far16Type( typ );
            }
        }
#endif
    } else {
        typ = TypeDefault();
    }
    func_result = typ;
    if( ParmsToBeReversed( flags, NULL ) ) {
        i = n;
        if( i == Level ) {
            tree = ParmNode( NULL, last_parm, far16_func );
        } else {
            ++i;
            tree = ParmNode( NULL, ValueStack[i], far16_func );
        }
        tree->op.u2.result_type = typ;
        while( i != Level ) {
            ++i;
            tree = ParmNode( tree, ValueStack[i], far16_func );
        }
        if( n != Level ) {
            tree = ParmNode( tree, last_parm, far16_func );     // this is 1'st parm
        }
        Level = n;
    } else {
        optimized = 0;
        if( last_parm->op.opr == OPR_PUSHSTRING ) {
            if( (GenSwitches & NO_OPTIMIZATION) == 0 ) {
                string_len = last_parm->op.u2.string_handle->length;
                if( functree->op.opr == OPR_PUSHADDR ) {
                    SymGet( &sym, functree->op.u2.sym_handle );
                    sym_name = SymName( &sym, functree->op.u2.sym_handle );
                    if( far_strcmp( sym_name, "_inline_strcmp", 15 ) == 0 ) {
                        SymGet( &sym, SymMEMCMP );
                        tree = VarLeaf( &sym, SymMEMCMP );
                        optimized = 1;
                    }
                }
            }
        } else {
            if( functree->op.opr == OPR_FUNCNAME ) {
                SymGet( &sym, functree->op.u2.sym_handle );
#if 0
                if( functree->op.u2.sym_handle == CurFuncHandle ) {
                    recursive = 1;                      /* 22-sep-91 */
                }
#endif
                sym_name = SymName( &sym, functree->op.u2.sym_handle );
                n = far_strlen_plus1( sym_name );
                if( (GenSwitches & NO_OPTIMIZATION) == 0  &&
                    CompFlags.extensions_enabled ) {
                    for( i = 0; MathFuncs[i].name; ++i ) {
                        if( parm_count == MathFuncs[i].parm_count  &&
                            IntrinsicMathFunc( sym_name, i, n, &sym ) ) {
                            FreeExprNode( functree );
                            if( parm_count == 1 ) {
                                tree = ExprNode( 0, OPR_MATHFUNC, last_parm );
                            } else {  /* parm_count == 2 */
                                tree = ValueStack[ Level ]; /* op1 */
                                --Level;
                                tree = ExprNode( tree, OPR_MATHFUNC2,
                                                        last_parm );
                            }
                            tree->u.expr_type = GetType( TYPE_DOUBLE );
                            tree->op.u1.mathfunc = MathFuncs[i].mathop;
                            goto done_call;
                        }
                    }
                }
#if (_CPU == _AXP) || (_CPU == _PPC) || (_CPU == _MIPS)
                if( far_strcmp( sym_name, "__builtin_va_start", n ) == 0 ) {
                    tree = GenVaStartNode( last_parm );
                    goto done_call;
                }
                if( far_strcmp( sym_name, "__builtin_alloca", n ) == 0 ) {
                    tree = GenAllocaNode( last_parm );
                    goto done_call;
                }
#endif
#if  _CPU == _PPC
                if( far_strcmp( sym_name, "__builtin_varg", n ) == 0 ) {
                    tree = GenVaArgNode( last_parm );
                    goto done_call;
                }
#endif
            }
        }
        if( ! optimized ) {
            tree = ParmNode( NULL, last_parm, far16_func );
        } else {
            tree = ParmNode( NULL, IntLeaf( string_len ), far16_func );
            tree = ParmNode( tree, last_parm, far16_func );
        }
        while( Token[ Level ] != T_LEFT_PAREN ) {
            tree = ParmNode( tree, ValueStack[ Level ], far16_func );
            --Level;
        }
    }
    tree = CallNode( functree, tree, func_result );
    if( ! DeadCode ) {                          /* 09-apr-93 */
        if( functree->op.opr == OPR_FUNCNAME ) {
            CompFlags.pending_dead_code |=
                        FunctionAborts( &sym, functree->op.u2.sym_handle );
        }
    }
done_call:
    MustRecog( T_RIGHT_PAREN );
    return( tree );
}

local void PopNestedParms( TYPEPTR **plistptr )
{
    struct nested_parm_lists    *npl;

    if( NestedParms != NULL ) {
        npl = NestedParms;
        *plistptr = npl->next_parm_type;
        NestedParms = npl->prev_list;
        CMemFree( npl );
    } else {
        *plistptr = NULL;
    }
}

static TREEPTR DummyFuncName( void )
{
    TREEPTR             tree;
    TYPEPTR             typ;
    SYM_HANDLE          sym_handle;
    SYM_ENTRY           sym;

    typ = FuncNode( GetType( TYPE_INT ), 0, NULL ); /* 15-jun-90 */
    sym_handle = MakeNewSym( &sym, 'F', typ, SC_AUTO );
    sym.flags |= SYM_FUNCTION;
    sym.mods = FLAG_NONE;
    SymReplace( &sym, sym_handle );
    tree = VarLeaf( &sym, sym_handle );
    return( tree );
}

local TREEPTR StartFunc( TREEPTR tree, TYPEPTR **plistptr )
{
    TYPEPTR             typ;
    TYPEPTR             orig_typ;
    TYPEPTR             *parms;
    type_modifiers      decl_flags;
//    char                recursive = 0;
#ifdef __SEH__
    opr_code            opr;
#endif
    SYM_HANDLE          sym_handle;
    SYM_ENTRY           sym;

    if( tree->op.opr == OPR_ERROR ) {   // hoke something up
        tree = DummyFuncName();
    }
    decl_flags = FLAG_NONE;
    typ = tree->u.expr_type;
    orig_typ = typ;
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type == TYPE_POINTER ) {
        tree = PtrOp( RValue( tree ) );
        typ = tree->u.expr_type;
        orig_typ = typ;
        SKIP_TYPEDEFS( typ );
    }
    if( tree->op.opr == OPR_PUSHADDR ) {        /* direct call */
        tree->op.opr = OPR_FUNCNAME;            // change to funcname
        sym_handle = tree->op.u2.sym_handle;
        SymGet( &sym, sym_handle );
#if 0
        if( tree->op.u2.sym_handle == CurFuncHandle ) {
            recursive = 1;
        }
#endif
        decl_flags = sym.mods;
        // is it OK to inline?
    } else {                                    /* indirect call */
        if( CompFlags.initializing_data ) {
            CErr1( ERR_NOT_A_CONSTANT_EXPR );
            sym_handle = 0;
        } else {
            if( tree->op.opr == OPR_POINTS ) {  //need to recover decl flags
                decl_flags = typ->u.fn.decl_flags;
            }
            sym_handle = MakeNewSym( &sym, 'F', orig_typ, SC_AUTO );
            sym.flags |= SYM_FUNCTION;
            sym.mods = decl_flags;
            SymReplace( &sym, sym_handle );
            tree = ExprNode( 0, OPR_CALL_INDIRECT, tree );
            tree->u.expr_type = typ;
            tree->op.u2.sym_handle = sym_handle;
        }
    }
    if( typ->decl_type != TYPE_FUNCTION ) {
        CErr1( ERR_NOT_A_FUNCTION );
        tree = DummyFuncName();
        typ = tree->u.expr_type;
    } else {
        if( decl_flags & FLAG_FAR16 ) {    /* 22-nov-91 */
            if( DeadCode == 0 ) {
                CurFuncSym.flags |= SYM_FUNC_NEEDS_THUNK;
            }
        }
    }
    parms = typ->u.fn.parms;   /* Parameters (prototype), if any */
    NextToken();                                /* skip over '(' */
    if( CurToken != T_RIGHT_PAREN ) {
        // push previous plist for nested calls
        struct nested_parm_lists    *npl;

        npl = CMemAlloc( sizeof( struct nested_parm_lists ) );
        npl->prev_list = NestedParms;           /* 17-dec-89 */
        npl->next_parm_type = *plistptr;
        NestedParms = npl;

        *plistptr = parms;
        ++Level;
        ValueStack[ Level ] = tree;
        Class[ Level ] = TC_PARM_LIST;
        Token[ Level ] = T_LEFT_PAREN;
        tree = NULL;               /* indicate need operand for parm */
    } else { // foo() build call
#ifdef __SEH__
        opr = OPR_CALL;
        if( CompFlags.exception_filter_expr ||
            CompFlags.in_finally_block      ||
            CompFlags.exception_handler ) {
            SYM_NAMEPTR         func_name;

            if( !(sym.flags & SYM_TEMP) ) {
                sym_handle = tree->op.u2.sym_handle;
                SymGet( &sym, sym_handle );
                func_name = SymName( &sym, sym_handle );
                if( strcmp( func_name, "_exception_code" ) == 0 ) {
                    if( CompFlags.exception_filter_expr ||
                        CompFlags.exception_handler ) {
                        opr = OPR_EXCEPT_CODE;
                    }
                } else if( strcmp( func_name, "_exception_info" ) == 0 ) {
                    if( CompFlags.exception_filter_expr ) {
                        opr = OPR_EXCEPT_INFO;
                    }
                } else if( strcmp( func_name, "_abnormal_termination" ) == 0 ) {
                    if( CompFlags.in_finally_block ) {
                        opr = OPR_ABNORMAL_TERMINATION;
                    }
                }
                if( opr != OPR_CALL ) {
                    FreeExprNode( tree );
                    tree = LeafNode( opr );
                    tree->u.expr_type = GetType( TYPE_INT );
                }
            }
        }
        if( opr == OPR_CALL ) {
#endif
            typ = typ->object; //get return type
            SKIP_TYPEDEFS( typ );
            if( !(sym.flags & SYM_TEMP) )
                SetDiagSymbol( &sym, sym_handle );
            tree = CallNode( tree, NULL, typ );
            if( parms != NULL ) {        /* function has prototype */
                if( *parms != NULL && (*parms)->decl_type != TYPE_VOID ) {
                    CErr1( ERR_PARM_COUNT_MISMATCH );
                }
            } else {                                /* check later */
                AddCallNode(tree);
            }
            if( !(sym.flags & SYM_TEMP) ) {
                SetDiagPop();
            }
#ifdef __SEH__
        }
#endif
        CompFlags.meaningless_stmt = 0;
        CompFlags.useful_side_effect = 1;
        NextToken();                    /* skip over ')' */
    }
    return( tree );
}


local TREEPTR OrOr( TREEPTR tree )
{
    // This routine is called when || token is found
    if( tree->op.opr == OPR_PUSHINT ) {
        if( tree->op.u2.long_value != 0 ) {
            ++SizeOfCount;      /* skip code gen */
            CurToken = T_NULL;
        }
    }
    return( tree );
}


local TREEPTR AndAnd( TREEPTR tree )
{
    // This routine is called when && token is found
    if( tree->op.opr == OPR_PUSHINT ) {
        if( tree->op.u2.long_value == 0 ) {
            ++SizeOfCount;      /* skip code gen */
            CurToken = T_NULL;
        }
    }
    return( tree );
}


TREEPTR BoolExpr( TREEPTR tree )
{
    switch( tree->op.opr ) {
    case OPR_NOT:
    case OPR_CMP:
    case OPR_OR_OR:
    case OPR_AND_AND:
    case OPR_ERROR:
        break;                          // tree is a boolean expression
    default:
        if( tree->op.opr == OPR_EQUALS ) {
            if( IsConstLeaf( tree ) ) {
                CWarn1( WARN_ASSIGNMENT_IN_BOOL_EXPR, ERR_ASSIGNMENT_IN_BOOL_EXPR );
            }
        }
        tree = RValue( tree );
        if( DataTypeOf( TypeOf( tree ) ) > TYPE_POINTER ) {
            CErr1( ERR_EXPR_MUST_BE_SCALAR );
        } else if( tree->op.opr == OPR_PUSHINT ) {
            uint64      val64;

            val64 = LongValue64( tree );
            if( U64Test( &val64 ) != 0 ) {
                tree->op.u2.ulong_value = 1;
            }
            tree->op.u1.const_type = TYPE_INT;
            tree->u.expr_type = GetType( TYPE_INT );
        } else {
            tree = RelOp( tree, T_NE, IntLeaf( 0 ) );
        }
    }
    return( tree );
}


local TREEPTR NotOp( TREEPTR tree )
{
    TREEPTR     opnd;
    FLOATVAL    *flt;

    if( IsConstLeaf( tree ) ) {
        switch( tree->op.u1.const_type ) {
        case TYPE_CHAR:
        case TYPE_UCHAR:
        case TYPE_SHORT:
        case TYPE_USHORT:
        case TYPE_INT:
        case TYPE_UINT:
        case TYPE_LONG:
        case TYPE_ULONG:
            if( tree->op.u2.long_value == 0 ) {
                tree->op.u2.long_value = 1;
            } else {
                tree->op.u2.long_value = 0;
            }
            tree->op.u1.const_type = TYPE_INT;
            break;
        case TYPE_FLOAT:
        case TYPE_DOUBLE:
        case TYPE_LONG_DOUBLE:
            flt = tree->op.u2.float_value;
            if( atof( flt->string ) == 0.0 ) {
                tree->op.u2.long_value = 1;
            } else {
                tree->op.u2.long_value = 0;
            }
            CMemFree( flt );
            tree->op.opr = OPR_PUSHINT;
            tree->op.u1.const_type = TYPE_INT;
            break;
        default:
            tree = RelOp( ScalarExpr( tree ), T_EQ, IntLeaf( 0 ) );
            break;
        }
    } else {
        if( tree->op.opr == OPR_EQUALS ) {
            opnd = tree->right;
            if( IsConstLeaf( opnd ) ) {
                CWarn1( WARN_ASSIGNMENT_IN_BOOL_EXPR, ERR_ASSIGNMENT_IN_BOOL_EXPR );
            }
        }
        tree = RelOp( ScalarExpr( tree ), T_EQ, IntLeaf( 0 ) );
    }
    tree->u.expr_type = GetType( TYPE_INT );
    return( tree );
}


local TREEPTR StartTernary( TREEPTR tree )
{
    tree = BoolExpr( tree );
    return( tree );
}


local TREEPTR ColonOp( TREEPTR true_part )
{
    if( Class[ Level ] != TC_TERNARY ) {
        CErr1( ERR_MISSING_QUESTION_OR_MISPLACED_COLON );
        true_part = ErrorNode( NULL );
    }
    return( true_part );
}



static TREEPTR TernOp( TREEPTR expr1, TREEPTR true_part, TREEPTR false_part )
{
    TYPEPTR     typ1, typ2, result_typ;
    TREEPTR     tree;
    op_flags    ops;

    if( true_part->op.opr == OPR_ERROR ) {              /* 14-sep-95 */
        FreeExprTree( false_part );
        return( ErrorNode( expr1 ) );
    }
    if( false_part->op.opr == OPR_ERROR ) {             /* 14-sep-95 */
        FreeExprTree( true_part );
        return( ErrorNode( expr1 ) );
    }
    typ1 = true_part->u.expr_type;
    typ2 = false_part->u.expr_type;
    if( !IsStruct( typ1 ) ) {
        true_part = VoidRValue( true_part );            /* 22-jul-91 */
        typ1 = true_part->u.expr_type;
    }
    if( !IsStruct( typ2 ) ) {
        false_part = VoidRValue( false_part );          /* 22-jul-91 */
        typ2 = false_part->u.expr_type;
    }
    if( typ1->decl_type == TYPE_VOID || typ2->decl_type == TYPE_VOID ) {
        if( typ1->decl_type != typ2->decl_type ) {
            CErr1( ERR_TYPE_MISMATCH );
        }
        result_typ = typ1;                              /* 10-mar-90 */
    } else {
        result_typ = TernType( true_part, false_part );
    }
    if( expr1->op.opr == OPR_PUSHINT ) {
        uint64      val64;

        val64 = LongValue64( expr1 );
        if( U64Test( &val64 ) == 0 ) {
            FreeExprTree( true_part );
            tree = false_part;
        } else {
            FreeExprTree( false_part );
            tree = true_part;
        }
        FreeExprNode( expr1 );
    } else {
        tree = ExprNode( true_part, OPR_COLON, false_part );
        tree->op.u2.label_index = NextLabel();
        NextLabel();                            // need 2 labels
        tree = ExprNode( expr1, OPR_QUESTION, tree );
        tree->op.u2.result_type = result_typ;
        ops = OPFLAG_NONE;
        if( !IsConstLeaf( true_part ) ) {
            ops |= (true_part->op.flags & OPFLAG_MEM_MODEL);
        }
        if( !IsConstLeaf( false_part ) ) {
            ops |= (true_part->op.flags & OPFLAG_MEM_MODEL);
        }
        tree->op.flags = ops;
    }
    tree->u.expr_type = result_typ;
    return( tree );
}


local TREEPTR UnaryPlus( TREEPTR tree )
{
    TYPEPTR     typ;

    tree = RValue( tree );
    typ = tree->u.expr_type;
    switch( typ->decl_type ) {
    case TYPE_CHAR:
    case TYPE_UCHAR:
    case TYPE_ENUM:
    case TYPE_SHORT:                            /* 26-mar-91 */
    case TYPE_USHORT:                           /* 26-mar-91 */
    case TYPE_FIELD:                            /* 26-mar-91 */
    case TYPE_UFIELD:                           /* 26-mar-91 */
        tree = ExprNode( NULL, OPR_CONVERT, tree );
        tree->u.expr_type = GetType( TYPE_INT );
        tree->op.u2.result_type = tree->u.expr_type;
        break;
    case TYPE_ARRAY:                            /* 26-mar-91 */
    case TYPE_POINTER:                          /* 26-mar-91 */
    case TYPE_STRUCT:
    case TYPE_UNION:
    case TYPE_FUNCTION:
    case TYPE_VOID:
        CErr1( ERR_EXPR_MUST_BE_SCALAR );
        tree = ErrorNode( tree );
        break;
    }
    return( tree );
}


TREEPTR ScalarExpr( TREEPTR tree )
{
    TYPEPTR     typ;

    if( tree->op.opr == OPR_ERROR ) return( tree );     /* 16-mar-88*/
    tree = RValue( tree );
    typ = tree->u.expr_type;
    switch( typ->decl_type ) {
    case TYPE_STRUCT:
    case TYPE_UNION:
    case TYPE_FUNCTION:
    case TYPE_VOID:
        CErr1( ERR_EXPR_MUST_BE_SCALAR );
        tree = ErrorNode( tree );
        break;
    }
    return( tree );
}


local TREEPTR SizeofOp( TYPEPTR typ )
{
    TREEPTR             tree;
    unsigned long       size;

    if( typ->decl_type == TYPE_FIELD || typ->decl_type == TYPE_UFIELD ) {
        CErr1( ERR_CANT_TAKE_SIZEOF_FIELD );
    }
    size = SizeOfArg( typ );
    if( size == 0 ) {
        SKIP_TYPEDEFS( typ );
        if( typ->decl_type == TYPE_VOID ) {
            CErr1( ERR_EXPR_HAS_VOID_TYPE );
        } else {
            SetDiagType1( typ );
            CErr1( ERR_INCOMPLETE_EXPR_TYPE );
            SetDiagPop();
        }
    }
#if TARGET_INT < TARGET_LONG
    if( size > TARGET_UINT_MAX ) {                      /* 30-jul-93 */
        tree = LongLeaf( size );
    } else {
        tree = UIntLeaf( size );
    }
#else
    tree = UIntLeaf( size );
#endif
    return( tree );
}

local TREEPTR SegOp( TREEPTR seg, TREEPTR offset )
{
    TREEPTR     tree;
    TYPEPTR     typ;

    typ = offset->u.expr_type->object;
    if( typ == NULL )
        typ = GetType( TYPE_VOID );      /* 14-jan-93 */
    tree = ExprNode( RValue(seg), OPR_FARPTR, RValue(offset) );
    tree->u.expr_type = PtrNode( typ, FLAG_FAR, 0 );
    return( tree );
}
