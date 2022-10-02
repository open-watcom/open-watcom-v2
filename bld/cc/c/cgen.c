/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Interface to Optimizing code generator.
*
****************************************************************************/


#include "cvars.h"
#include "cg.h"
#include "cgdefs.h"
#include "cgswitch.h"
#include "cgcli.h"
#include "cgprotos.h"
#include "feprotos.h"
#include "cgen.h"
#include "cfeinfo.h"
#ifdef __SEH__
#include "tryblock.h"
#endif
#ifndef NDEBUG
#include "togglesd.h"
#endif

#define PushCGName(name)        ValueStack[index++] = (TREEPTR)name
#define PopCGName()             (cg_name)ValueStack[--index]

typedef struct local_vars {
    struct local_vars   *next;
    SYM_HANDLE          sym_list;
} local_vars;

#ifdef __SEH__
typedef struct  try_table_back_handles {
    struct try_table_back_handles   *next;
    BACK_HANDLE                     back_handle;
} try_table_back_handles;
#endif

extern  SYM_LIST_HEADS  *SymListHeads;
extern  LABEL_INDEX     LabelIndex;

static void      FreeExtVars( void );
static void      FreeGblVars( SYM_HANDLE sym_handle );
static void      FreeLocalVars( SYM_HANDLE sym_list );
static cg_type   CodePtrType( type_modifiers flags );
static bool      DoFuncDefn( SYM_HANDLE funcsym_handle );
static void      EmitSyms( void );
static void      Emit1String( STR_HANDLE str_handle );
static void      EmitLiteral( STR_HANDLE strlit );
static void      EmitCS_Strings( void );
static void      FreeStrings( void );
static void      CDoAutoDecl( SYM_HANDLE sym_handle );
static void      CDoParmDecl( SYMPTR sym, SYM_HANDLE sym_handle );
static void      ParmReverse( SYM_HANDLE sym_handle );
#ifdef __SEH__
static void     FreeTrySymBackInfo( void );
static void     FreeTryTableBackHandles( void );
static void     CallTryFini( void );
static void     GenerateTryBlock( TREEPTR tree );
#endif

static local_vars       *LocalVarList;
static label_handle     *CGLabelHandles;
static TREEPTR          FirstNode;
static TREEPTR          LastNode;
static OPNODE           *FuncNodePtr;
static int              Refno;
static temp_handle      SSVar;
static SYM_HANDLE       Saved_CurFunc;
static int              InLineDepth;

#ifdef __SEH__
static int                      TryRefno;
static try_table_back_handles   *TryTableBackHandles;
#endif

struct func_save {
    SYMPTR          func;
    SYM_HANDLE      func_handle;
    OPNODE          *funcnode;
    label_handle    *cglabel_handles;
    LABEL_INDEX     labelindex;
};

/* matches table of type in ctypes.h */
static  char    CGDataType[] = {
    #define pick1(enum,cgtype,x86asmtype,name,size) cgtype,
    #include "cdatatyp.h"
    #undef  pick1
};

static  cg_op   CGOperator[] = {
    #define pick1(enum,dump,cgenum) cgenum,
    #include "copcodes.h"
    #undef pick1
};

static  cg_op   CC2CGOp[] = {
    #define pick1(enum,dump,cgenum) cgenum,
    #include "copcond.h"
    #undef pick1
};

#ifdef HEAP_SIZE_STAT

#include <malloc.h>

struct heap_stat {
    int     free;
    int     used;
};

static int heap_size( struct heap_stat *stat )
{
    struct _heapinfo    h_info;
    int                 heap_status;

    h_info._pentry = NULL;
    stat->free = 0;
    stat->used = 0;
    for( ; (heap_status = _heapwalk( &h_info )) == _HEAPOK; ) {
        if( h_info._useflag ) {
            stat->used += h_info._size;
        } else {
            stat->free += h_info._size;
        }
    }
    return( heap_status );
}
#endif

static void StartFunction( OPNODE *node )
{
    FuncNodePtr = node;
    if( InLineDepth == 0 ) {
        LocalVarList = NULL;
    }

    FuncNodePtr->u2.func.flags |= FUNC_INUSE;
    DoFuncDefn( node->u2.func.sym_handle );
}

static void DefineLabels( OPNODE *node )
{
    LABEL_INDEX i;

    LabelIndex = node->u2.label_count;
    if( LabelIndex != 0 ) {
        CGLabelHandles = (label_handle *)CMemAlloc( ( LabelIndex + 1 ) * sizeof( label_handle ) );
        for( i = 1; i <= LabelIndex; i++ ) {
            CGLabelHandles[i] = BENewLabel();
        }
    }
}

static local_vars *ReleaseVars( SYM_HANDLE sym_list, local_vars *local_var_list )
{
    local_vars  *local_entry;

    local_entry = (local_vars *)CMemAlloc( sizeof( local_vars ) );
    local_entry->next = local_var_list;
    local_entry->sym_list = sym_list;
    return( local_entry );
}

static void RelLocalVars( local_vars *local_var_list )
{
    local_vars  *local_entry;

    while( (local_entry = local_var_list) != NULL ) {
        local_var_list = local_entry->next;
        FreeLocalVars( local_entry->sym_list );
        CMemFree( local_entry );
    }
}

static cg_type ReturnType( cg_type type )
{
    if( CompFlags.returns_promoted && (InLineDepth == 0) ) {
        type = FEParmType( NULL, NULL, type );
    }
    return( type );
}

static void EndFunction( OPNODE *node )
{
    LABEL_INDEX i;
    cg_name     name;
    cg_type     dtype;
    SYM_ENTRY   sym;

    if( LabelIndex != 0 ) {
        for( i = 1; i <= LabelIndex; i++ ) {
            BEFiniLabel( CGLabelHandles[i] );
        }
        CMemFree( CGLabelHandles );     // free labels for the function
        CGLabelHandles = NULL;
    }
#ifdef __SEH__
    if( FuncNodePtr->u2.func.flags & FUNC_USES_SEH ) {
        CallTryFini();          // generate call to __TryFini
        FreeTrySymBackInfo();
    }
#endif
    if( node->u2.sym_handle == SYM_NULL ) {
        dtype = CGenType( CurFunc->sym_type->object );
        CGReturn( NULL, dtype );
    } else {                            // return value
        SymGet( &sym, node->u2.sym_handle );
        dtype = CGenType( sym.sym_type );
        name = CGTempName( sym.info.return_var, dtype );
        name = CGUnary( O_POINTS, name, dtype );
        CGReturn( name, ReturnType( dtype ) );
    }
    FreeLocalVars( CurFunc->u.func.parms );
    FreeLocalVars( CurFunc->u.func.locals );
    if( InLineDepth == 0 ) {
        RelLocalVars( LocalVarList );
    }
    if( GenSwitches & DBG_LOCALS ) {
        if( InLineDepth != 0 ) {
            DBEndBlock();
        }
    }
    FuncNodePtr->u2.func.flags &= ~FUNC_INUSE;
}

static void ReturnExpression( OPNODE *node, cg_name expr )
{
    cg_name     name;
    cg_type     dtype;
    SYM_ENTRY   sym;

    SymGet( &sym, node->u2.sym_handle );
    dtype = CGenType( sym.sym_type );
    name = CGTempName( sym.info.return_var, dtype );
    CGDone( CGAssign( name, expr, dtype ) );
}

static cg_type DataPointerType( OPNODE *node )
{
#if ( _CPU == 8086 ) || ( _CPU == 386 )
    cg_type     dtype;

    if( Far16Pointer( node->flags ) ) {
        dtype = TY_POINTER;
    } else if( node->flags & OPFLAG_NEARPTR ) {
        dtype = TY_NEAR_POINTER;
    } else if( node->flags & OPFLAG_FARPTR ) {
        dtype = TY_LONG_POINTER;
    } else if( node->flags & OPFLAG_HUGEPTR ) {
        dtype = TY_HUGE_POINTER;
    } else {
        dtype = TY_POINTER;
    }
    return( dtype );
#else
    /* unused parameters */ (void)node;

    return( TY_POINTER );
#endif
}

static cg_name ForceVolatileFloat( cg_name name, TYPEPTR typ )
{
    if( CompFlags.op_switch_used ) {
        if( typ->decl_type == TYP_FLOAT || typ->decl_type == TYP_DOUBLE || typ->decl_type == TYP_LONG_DOUBLE ) {
            name = CGVolatile( name );
        }
    }
    return( name );
}


static cg_name PushSymSeg( OPNODE *node )
{
    cg_name     segname;
//    SYMPTR      sym;
    SYM_HANDLE  sym_handle;

    sym_handle = node->u2.sym_handle;
    if( sym_handle == SYM_NULL ) {
        segname = CGInteger( 0, TY_UNSIGNED );
    } else {
        if( sym_handle == Sym_CS ) {
            segname = CGFEName( (CGSYM_HANDLE)CurFuncHandle, TY_LONG_CODE_PTR );
        } else if( sym_handle == Sym_SS ) {
            if( SSVar == NULL ) {
                SSVar = CGTemp( TY_UINT_2 );
            }
            segname = CGTempName( SSVar, TY_UINT_2 );
        } else {
//            sym = SymGetPtr( sym_handle );
            segname = CGFEName( (CGSYM_HANDLE)sym_handle, TY_UINT_2 );
        }
    }
    return( segname );
}

#ifdef __SEH__
static cg_name TryFieldAddr( unsigned offset )
{
    cg_name     name;

    name = CGFEName( (CGSYM_HANDLE)TrySymHandle, TryRefno );
    name = CGBinary( O_PLUS, name, CGInteger( offset, TY_UNSIGNED ), TY_POINTER );
    name = CGVolatile( name );
    return( name );
}

static cg_name TryExceptionInfoAddr( void )
{
    cg_name     name;

    name = TryFieldAddr( FLD_exception_info );
//    name = CGUnary( O_POINTS, name, TY_POINTER );
//    name = CGUnary( O_POINTS, name, TY_POINTER );
    return( name );
}

static void SetTryTable( BACK_HANDLE except_table )
{
    cg_name     name;
    cg_name     table;

    name = TryFieldAddr( FLD_scope_table );
    table = CGBackName( except_table, TY_POINTER );
    CGDone( CGAssign( name, table, TY_POINTER ) );
}

static void SetTryScope( tryindex_t scope )
{
    cg_name     name;

    name = TryFieldAddr( FLD_scope_index );
    CGDone( CGAssign( name, CGInteger( scope, TY_UINT_1 ), TY_UINT_1 ) );
}

static void EndFinally( void )
{
    cg_name      name;
    cg_name      func;
    LABEL_HANDLE label_handle;
    call_handle  call;

    label_handle = BENewLabel();
    name = TryFieldAddr( FLD_unwindflag );
    name = CGUnary( O_POINTS, name, TY_UINT_1 );
    name = CGCompare( O_EQ, name, CGInteger( 0, TY_UINT_1 ), TY_UINT_1 );
    CGControl( O_IF_TRUE, name, label_handle );
    func = CGFEName( (CGSYM_HANDLE)SymFinally, TY_CODE_PTR );
    call = CGInitCall( func, TY_INTEGER, (CGSYM_HANDLE)SymFinally );
    CGDone( CGCall( call ) );
    CGControl( O_LABEL, NULL, label_handle );
    BEFiniLabel( label_handle );
}

static cg_name TryAbnormalTermination( void )
{
    cg_name      name;

    name = TryFieldAddr( FLD_unwindflag );
    name = CGUnary( O_POINTS, name, TY_UINT_1 );
    return( name );
}

static void CallTryRtn( SYM_HANDLE try_rtn, cg_name parm )
{
    call_handle call;

    call = CGInitCall( CGFEName( (CGSYM_HANDLE)try_rtn, TY_POINTER ), TY_INTEGER, (CGSYM_HANDLE)try_rtn );
    CGAddParm( call, parm, TY_POINTER );
    CGDone( CGCall( call ) );
}

static void CallTryInit( void )
{
    CallTryRtn( SymTryInit, CGFEName( (CGSYM_HANDLE)TrySymHandle, TY_POINTER ) );
}

static void CallTryFini( void )
{
    cg_name     name;

    name = TryFieldAddr( FLD_next );
    name = CGUnary( O_POINTS, name, TY_POINTER );
    CallTryRtn( SymTryFini, name );
}

static void CallTryUnwind( tryindex_t scope_index )
{
    call_handle call;
    cg_name     parm;

    call = CGInitCall( CGFEName( (CGSYM_HANDLE)SymTryUnwind, TY_POINTER ), TY_INTEGER, (CGSYM_HANDLE)SymTryUnwind );
    parm = CGInteger( scope_index, TY_UINT_1 );
    CGAddParm( call, parm, TY_INTEGER );
    CGDone( CGCall( call ) );
}
#endif

#if _CPU == _AXP
static void GenVaStart( cg_name op1, cg_name offset )
{
    cg_name     name;
    cg_name     baseptr;

    baseptr = CGVarargsBasePtr( TY_POINTER );
    name = CGLVAssign( op1, baseptr, TY_POINTER );
    name = CGBinary( O_PLUS, name, CGInteger( TARGET_POINTER, TY_INTEGER ), TY_POINTER );
    name = CGAssign( name, offset, TY_INTEGER );
    CGDone( name );
}
#elif _CPU == _PPC
static void GenVaStart( cg_name op1, cg_name offset )
{
    cg_name     name;

    /* unused parameters */ (void)offset;

    name = CGUnary( O_VA_START, op1, TY_POINTER );
    CGDone( name );
}
#elif _CPU == _MIPS
/* Similar to Alpha, except we point va_list.__base to the first
 * vararg and va_list.__offset initially to zero. Strictly speaking
 * we don't need va_list.__offset.
 */
static void GenVaStart( cg_name op1, cg_name offset )
{
    cg_name     name;
    cg_name     baseptr;

    baseptr = CGVarargsBasePtr( TY_POINTER );
    name = CGBinary( O_PLUS, baseptr, offset, TY_POINTER );
    name = CGLVAssign( op1, name, TY_POINTER );
    name = CGBinary( O_PLUS, name, CGInteger( TARGET_POINTER, TY_INTEGER ), TY_POINTER );
    name = CGAssign( name, CGInteger( 0, TY_INTEGER ), TY_INTEGER );
    CGDone( name );
}
#endif

static cg_name PushSym( OPNODE *node )
{
    cg_name     name;
    cg_type     dtype;
    TYPEPTR     typ;
    SYM_ENTRY   sym;

    SymGet( &sym, node->u2.sym_handle );
    typ = sym.sym_type;
    if( sym.flags & SYM_FUNCTION ) {
        dtype = CodePtrType( sym.mods );
    } else {
        dtype = CGenType( typ );
    }
    if( sym.flags & SYM_FUNC_RETURN_VAR ) {
        name = CGTempName( sym.info.return_var, dtype );
    } else {
        name = CGFEName( (CGSYM_HANDLE)node->u2.sym_handle, dtype );
    }
    if( node->flags & OPFLAG_UNALIGNED ) {
        name = CGAttr( name, CG_SYM_UNALIGNED );
    }
    if( node->flags & OPFLAG_VOLATILE ) {
        name = CGVolatile( name );
    } else {
        name = ForceVolatileFloat( name, typ );
    }
    name = CGUnary( O_POINTS, name, dtype );
    return( name );
}

static cg_name PushSymAddr( OPNODE *node )
{
    cg_name     name;
    cg_type     dtype;
    SYM_ENTRY   sym;
    TYPEPTR     typ;

    SymGet( &sym, node->u2.sym_handle );
    typ = sym.sym_type;
    if( sym.flags & SYM_FUNCTION ) {
        dtype = CodePtrType( sym.mods );
    } else {
        dtype = CGenType( typ );
    }
    if( sym.flags & SYM_FUNC_RETURN_VAR ) {
        name = CGTempName( sym.info.return_var, dtype );
    } else {
        name = CGFEName( (CGSYM_HANDLE)node->u2.sym_handle, dtype );
     // if( (sym.mods & FLAG_VOLATILE) ||
     //     (sym.flags & SYM_USED_IN_PRAGMA) ) {
     //     name = CGVolatile( name );
     // }
        if( sym.flags & SYM_USED_IN_PRAGMA ) {
            name = CGVolatile( name );
        }
    }
    return( name );
}

static cg_name PushRValue( OPNODE *node, cg_name name )
{
    TYPEPTR     typ;

    if( node->flags & OPFLAG_RVALUE ) {
        typ = node->u2.result_type;
        if( node->flags & OPFLAG_VOLATILE ) {
            name = CGVolatile( name );
        } else {
            name = ForceVolatileFloat( name, typ );
        }
        name = CGUnary( O_POINTS, name, CGenType( typ ) );
    } else {
//      if( node->flags & OPFLAG_VOLATILE ) {
//          name = CGVolatile( name );
//      }
    }
    return( name );
}

static cg_name DotOperator( cg_name op1, OPNODE *node, cg_name op2 )
{
    TYPEPTR     typ;
    cg_name     name;

    // node->u2.result_type is the type of the data
    // for the O_PLUS we want a pointer type
    name = CGBinary( O_PLUS, op1, op2, DataPointerType( node ) );
    typ = node->u2.result_type;
    if( typ->decl_type == TYP_FIELD || typ->decl_type == TYP_UFIELD ) {
        name = CGBitMask( name, typ->u.f.field_start,
                    typ->u.f.field_width, CGenType( typ ) );
    }
    if( node->flags & OPFLAG_UNALIGNED ) {
        name = CGAttr( name, CG_SYM_UNALIGNED );
    }
    return( PushRValue( node, name ) );
}

static cg_name ArrowOperator( cg_name op1, OPNODE *node, cg_name op2 )
{
    // node->u2.result_type is the type of the data
    // for the O_PLUS we want a pointer type
#if _CPU == 386
    if( Far16Pointer( node->flags ) ) {
        op1 = CGUnary( O_PTR_TO_NATIVE, op1, TY_POINTER );
    }
#endif
//  rvalue has already been done on left side of tree
//    op1 = CGUnary( O_POINTS, op1, DataPointerType( node ) );
    return( DotOperator( op1, node, op2 ) );
}

static cg_name IndexOperator( cg_name op1, OPNODE *node, cg_name op2 )
{
    int         element_size;
    cg_type     index_type;

    // node->u2.result_type is the type of the data
    // for the O_PLUS we want a pointer type
    // op2 needs to be multiplied by the element size of the array

#if _CPU == 386
    if( Far16Pointer( node->flags ) ) {
        op1 = CGUnary( O_PTR_TO_NATIVE, op1, TY_POINTER );
    }
#endif
    element_size = SizeOfArg( node->u2.result_type );
    if( element_size != 1 ) {
        index_type = TY_INTEGER;
#if _CPU == 8086
        if( (node->flags & OPFLAG_HUGEPTR) ||
           ((TargetSwitches & (BIG_DATA | CHEAP_POINTER)) == BIG_DATA &&
            (node->flags & (OPFLAG_NEARPTR | OPFLAG_FARPTR)) == 0) ) {
            index_type = TY_INT_4;
        }
#endif
        op2 = CGBinary( O_TIMES, op2,
                CGInteger( element_size, TY_INTEGER ), index_type );
    }
    op1 = CGBinary( O_PLUS, op1, op2, DataPointerType( node ) );
    if( node->flags & OPFLAG_UNALIGNED ) {
        op1 = CGAttr( op1, CG_SYM_UNALIGNED );
    }
    return( PushRValue( node, op1 ) );
}

static cg_name DoAddSub( cg_name op1, OPNODE *node, cg_name op2 )
{
    cg_name     name;
    TYPEPTR     typ;

    typ = node->u2.result_type;
    SKIP_TYPEDEFS( typ );
    name = CGBinary( CGOperator[node->opr], op1, op2, CGenType( typ ) );
//  if( typ->decl_type == TYP_POINTER ) {
//      if( typ->u.p.decl_flags & FLAG_VOLATILE ) {
//          name = CGVolatile( name );
//      }
//  }
    return( name );
}
static cg_name PushConstant( OPNODE *node )
{
    cg_name     name;
    cg_type     dtype;
    FLOATVAL    *flt;
    char        *flt_string;

    dtype = CGDataType[node->u1.const_type];
    switch( node->u1.const_type ) {
    case TYP_CHAR:
    case TYP_UCHAR:
    case TYP_SHORT:
    case TYP_USHORT:
    case TYP_INT:
    case TYP_UINT:
    case TYP_LONG:
    case TYP_ULONG:
        name = CGInteger( node->u2.ulong_value, dtype );
        break;
    case TYP_POINTER:
        name = CGInteger( node->u2.ulong_value, DataPointerType( node ) );
        break;
    case TYP_LONG64:
    case TYP_ULONG64:
        name = CGInt64( node->u2.ulong64_value, dtype );
        break;
    case TYP_FLOAT:
    case TYP_DOUBLE:
    case TYP_LONG_DOUBLE:
    case TYP_FIMAGINARY:
    case TYP_DIMAGINARY:
    case TYP_LDIMAGINARY:
        flt = node->u2.float_value;
        if( flt->len != 0 ) {                   // if still in string form
            flt_string = flt->string;
        } else {                                // else in binary form
            flt_string = ftoa( flt );
        }
        name = CGFloat( flt_string, dtype );
        break;
    default:
        assert( 0 );
        name = NULL;
        break;
    }
    return( name );
}

static cg_name PushString( OPNODE *node )
{
    STR_HANDLE      string;

    string = node->u2.string_handle;
    Emit1String( string );
    return( CGBackName( string->back_handle, TY_UINT_1 ) );
}

static cg_name DoIndirection( OPNODE *node, cg_name name )
{
    TYPEPTR     typ;

    // check for special kinds of pointers, eg. call __Far16ToFlat
    typ = node->u2.result_type;
#if _CPU == 386
    if( Far16Pointer( node->flags ) ) {
        // Do NOT convert __far16 function pointers to flat because the
        // thunk routine expects 16:16 pointers!
        if( ( typ->object != NULL ) && ( typ->object->decl_type != TYP_FUNCTION ) ) {
            name = CGUnary( O_PTR_TO_NATIVE, name, TY_POINTER );
        }
    }
#endif
    if( node->flags & OPFLAG_UNALIGNED ) {
        name = CGAttr( name, CG_SYM_UNALIGNED );
    }
    if( node->flags & OPFLAG_RVALUE ) {
        if( node->flags & OPFLAG_VOLATILE ) {
            name = CGVolatile( name );
        }
        name = CGUnary( O_POINTS, name, CGenType( typ->object ) );
    }
    return( name );
}

static cg_name ConvertPointer( OPNODE *node, cg_name name )
{
#if _CPU == 386
    if( FAR16_PTRCLASS( node->u2.sp.old_ptrclass ) ) {
        name = CGUnary( O_PTR_TO_NATIVE, name, TY_POINTER );
    } else if( FAR16_PTRCLASS( node->u2.sp.new_ptrclass ) ) {
        name = CGUnary( O_PTR_TO_FOREIGN, name, TY_POINTER );
    }
#else
    /* unused parameters */ (void)node;
#endif
    return( name );
}

static call_handle InitFuncCall( OPNODE *node )
{
    cg_name     name;
    cg_type     dtype;
    TYPEPTR     typ;
    SYMPTR      sym;

    sym = SymGetPtr( node->u2.sym_handle );
    typ = sym->sym_type;
    dtype = CGenType( typ );
    name = CGFEName( (CGSYM_HANDLE)node->u2.sym_handle, dtype );
//    dtype = FESymType( sym );
    SKIP_TYPEDEFS( typ );
    return( CGInitCall( name, CGenType( typ->object ), (CGSYM_HANDLE)node->u2.sym_handle ) );
}

static call_handle InitIndFuncCall( OPNODE *node, cg_name name )
{
    TYPEPTR     typ;
    SYMPTR      sym;

    sym = SymGetPtr( node->u2.sym_handle );
    typ = sym->sym_type;
    SKIP_TYPEDEFS( typ );
    return( CGInitCall( name, CGenType( typ->object ), (CGSYM_HANDLE)node->u2.sym_handle ) );
}

static void DoSwitch( OPNODE *node, cg_name name )
{
    sel_handle  table;
    SWITCHPTR   sw;
    CASEPTR     ce;

    table = CGSelInit();
    sw = node->u2.switch_info;
    for( ce = sw->case_list; ce != NULL; ce = ce->next_case ) {
        CGSelCase( table, CGLabelHandles[ce->label], ce->value );
    }
    CGSelOther( table, CGLabelHandles[sw->default_label] );
    CGSelect( table, name );
}

static bool IsStruct( TYPEPTR typ )
/*********************************/
{
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type == TYP_STRUCT
      || typ->decl_type == TYP_FCOMPLEX
      || typ->decl_type == TYP_DCOMPLEX
      || typ->decl_type == TYP_LDCOMPLEX
      || typ->decl_type == TYP_UNION ) {
        return( true );
    }
    return( false );
}

static void EmitNodes( TREEPTR tree )
{
    cg_name     op1;
    cg_name     op2;
    cg_name     expr;
    call_handle call;
    int         index;
    OPNODE      *node;

    index = 0;
    for( ; tree != NULL; tree = tree->u.thread ) {
        node = &tree->op;
        switch( node->opr ) {
        case OPR_FUNCTION:              // start of function
            StartFunction( node );
            break;
        case OPR_NEWBLOCK:              // start of new block { vars; }
            DBBegBlock();
            CDoAutoDecl( node->u2.sym_handle );
            break;
        case OPR_ENDBLOCK:              // end of new block { vars; }
            DBEndBlock();
            LocalVarList = ReleaseVars( node->u2.sym_handle, LocalVarList );
            break;
        case OPR_LABELCOUNT:            // number of labels used in function
            DefineLabels( node );
            break;
        case OPR_RETURN:                // return( expression );
            op1 = PopCGName();          // - get expression
            ReturnExpression( node, op1 );
            break;
        case OPR_FUNCEND:               // end of function
            EndFunction( node );
            break;
        case OPR_EQUALS:                // =
            op2 = PopCGName();          // get right opnd
            op1 = PopCGName();          // get lvalue
            if( node->flags & OPFLAG_VOLATILE ) { // is lvalue volatile
                 op1 = CGVolatile( op1 );
            }
            if( IsStruct( node->u2.result_type ) ) {
                op1 = CGLVAssign( op1, op2, CGenType( node->u2.result_type ) );
                op1 = PushRValue( node, op1 );
            } else {
                op1 = CGAssign( op1, op2, CGenType( node->u2.result_type ) );
            }
            PushCGName( op1 );
            break;
        case OPR_ADD:                   // +
        case OPR_SUB:                   // -
            op2 = PopCGName();
            op1 = PopCGName();
            PushCGName( DoAddSub( op1, node, op2 ) );
            break;
        case OPR_MUL:                   // *
        case OPR_DIV:                   // /
        case OPR_MOD:                   // %
        case OPR_OR:                    // |
        case OPR_AND:                   // &
        case OPR_XOR:                   // ^
        case OPR_RSHIFT:                // >>
        case OPR_LSHIFT:                // <<
            op2 = PopCGName();
            op1 = PopCGName();
            PushCGName( CGBinary( CGOperator[node->opr], op1, op2,
                                    CGenType( node->u2.result_type ) ) );
            break;
        case OPR_COMMA:                 // expr , expr
          {
            cg_type     dtype;
            op2 = PopCGName();
            op1 = PopCGName();
            dtype =  IsStruct( node->u2.result_type ) ?
                   DataPointerType( node ) : CGenType( node->u2.result_type );
            op1 = CGBinary( O_COMMA, op1, op2, dtype );
            PushCGName( PushRValue( node, op1 ) );
          } break;
        case OPR_PLUS_EQUAL:
        case OPR_MINUS_EQUAL:
        case OPR_TIMES_EQUAL:
        case OPR_DIV_EQUAL:
        case OPR_MOD_EQUAL:
        case OPR_XOR_EQUAL:
        case OPR_AND_EQUAL:
        case OPR_OR_EQUAL:
        case OPR_RSHIFT_EQUAL:
        case OPR_LSHIFT_EQUAL:
            op2 = PopCGName();          // get right opnd
            op1 = PopCGName();          // get lvalue
            if( node->flags & OPFLAG_VOLATILE ) { // is lvalue volatile
                 op1 = CGVolatile( op1 );
            }
            PushCGName( CGPreGets( CGOperator[node->opr], op1, op2,
                                    CGenType( node->u2.result_type ) ) );
            break;
        case OPR_NEG:                   // negate
            op1 = PopCGName();
            PushCGName( CGUnary( O_UMINUS, op1,
                                    CGenType( node->u2.result_type ) ) );
            break;
        case OPR_CMP:                   // compare
            op2 = PopCGName();
            op1 = PopCGName();
            PushCGName( CGCompare( CC2CGOp[node->u1.cc], op1, op2,
                                    CGenType( node->u2.compare_type ) ) );
            break;
        case OPR_COM:                   // ~
            op1 = PopCGName();
            PushCGName( CGUnary( O_COMPLEMENT, op1,
                                    CGenType( node->u2.result_type ) ) );
            break;
        case OPR_NOT:                   // !
            op1 = PopCGName();
            PushCGName( CGFlow( O_FLOW_NOT, op1, NULL ) );
            break;
        case OPR_QUESTION:             // ?
          {
            cg_type     dtype;

            op2 = PopCGName();          // false_part
            op1 = PopCGName();          // true_part
            expr = PopCGName();         // test expr
            dtype =  IsStruct( node->u2.result_type ) ?
                   DataPointerType( node ) : CGenType( node->u2.result_type );
            op1 = CGChoose( expr, op1, op2, dtype );

            PushCGName( PushRValue( node, op1 ) );
          } break;
        case OPR_COLON:                 // :
            // do nothing, wait for OPR_QUESTION to come along
            break;
        case OPR_OR_OR:                 // ||
        case OPR_AND_AND:               // &&
            op2 = PopCGName();
            op1 = PopCGName();
            PushCGName( CGFlow( CGOperator[node->opr], op1, op2 ) );
            break;
        case OPR_POINTS:                // *ptr
            op1 = PopCGName();
            PushCGName( DoIndirection( node, op1 ) );
            break;
        case OPR_POSTINC:               // lvalue++
        case OPR_POSTDEC:               // lvalue--
            op2 = PopCGName();          // get amount to inc/dec by
            op1 = PopCGName();          // get lvalue
            if( node->flags & OPFLAG_VOLATILE ) { // is lvalue volatile
                 op1 = CGVolatile( op1 );
            }
            PushCGName( CGPostGets( CGOperator[node->opr], op1, op2,
                                    CGenType( node->u2.result_type ) ) );
            break;
        case OPR_PUSHSYM:               // push sym_handle
            PushCGName( PushSym( node ) );
            break;
        case OPR_PUSHADDR:              // push address of sym_handle
            PushCGName( PushSymAddr( node ) );
            break;
        case OPR_PUSHINT:               // push integer constant
        case OPR_PUSHFLOAT:             // push floating-point constant
            PushCGName( PushConstant( node ) );
            break;
        case OPR_PUSHSTRING:            // push address of string literal
            PushCGName( PushString( node ) );
            break;
        case OPR_PUSHSEG:               // push seg of sym
            PushCGName( PushSymSeg( node ) );
            break;
        case OPR_DUPE:               // push seg of sym
         {
            temp_handle temp_name;
            cg_name     name;
            cg_name     left;

            op1 = PopCGName();      // - get expression
            temp_name = CGTemp( TY_LONG_POINTER );
            name = CGTempName( temp_name, TY_LONG_POINTER );
            left = CGAssign( name, op1, TY_LONG_POINTER );
            name = CGTempName( temp_name, TY_LONG_POINTER );
            left = CGEval( left );  // force order - temp_name used on both sides
            PushCGName( left );
            name = CGUnary( O_POINTS, name, TY_LONG_POINTER );
            PushCGName( CGUnary( O_POINTS, name, TY_LONG_POINTER ) );
          } break;
        case OPR_CONVERT:
            if( node->u2.result_type->decl_type != TYP_VOID ) {
                op1 = PopCGName();      // - get expression
                PushCGName( CGUnary( O_CONVERT, op1, CGenType( node->u2.result_type )) );
            }
            break;
        case OPR_CONVERT_PTR:           // convert pointer
            op1 = PopCGName();          // - get expression
            PushCGName( ConvertPointer( node, op1 ) );
            break;
        case OPR_CONVERT_SEG:           // convert pointer to segment
          {
            cg_name     name;

            op1 = PopCGName();          // - get expression
            name = CGUnary( O_CONVERT, op1, TY_LONG_POINTER );
            name = CGUnary( O_CONVERT, name, TY_UINT_4 );
            name = CGBinary( O_RSHIFT, name,
                             CGInteger( 16, TY_UNSIGNED ), TY_UINT_4 );
            PushCGName( name );
          } break;
        case OPR_MATHFUNC:              // intrinsic math func with 1 parm
            op1 = PopCGName();          // - get expression
            PushCGName( CGUnary( node->u1.mathfunc, op1, TY_DOUBLE ) );
            break;
        case OPR_MATHFUNC2:             // intrinsic math func with 2 parms
            op2 = PopCGName();          // - get expression
            op1 = PopCGName();          // - get expression
            PushCGName( CGBinary( node->u1.mathfunc, op1, op2, TY_DOUBLE ) );
            break;
        case OPR_DOT:                   // sym.field
            op2 = PopCGName();          // - get offset of field
            op1 = PopCGName();          // - get address of sym
            PushCGName( DotOperator( op1, node, op2 ) );
            break;
        case OPR_ARROW:                 // ptr->field
            op2 = PopCGName();          // - get offset of field
            op1 = PopCGName();          // - get pointer
            PushCGName( ArrowOperator( op1, node, op2 ) );
            break;
        case OPR_INDEX:                 // array[index]
            op2 = PopCGName();          // - get index
            op1 = PopCGName();          // - get address of array
            PushCGName( IndexOperator( op1, node, op2 ) );
            break;
        case OPR_ADDROF:                // & expr
        case OPR_NOP:                   // nop
            break;
        case OPR_FARPTR:                // segment :> offset
            op2 = PopCGName();          // - get offset
            op1 = PopCGName();          // - get segment
            PushCGName( CGBinary( O_CONVERT, op2, op1, TY_LONG_POINTER ) );
            break;
        case OPR_FUNCNAME:              // function name
            PushCGName( InitFuncCall( node ) ); // - push call_handle
            break;
        case OPR_CALL_INDIRECT:         // indirect function call
            op1 = PopCGName();                          // - get name
            PushCGName( InitIndFuncCall( node, op1 ) ); // - push call_handle
            break;
        case OPR_CALL:                  // function call
            call = PopCGName();         // - get call_handle
            op1 = CGCall( call );
            if( node->flags & OPFLAG_RVALUE ) {
                op1 = CGUnary( O_POINTS, op1, CGenType( node->u2.result_type ) );
            }
            PushCGName( op1 );
            break;
        case OPR_PARM:                  // function parm
            op1 = PopCGName();          // - get parm
            call = PopCGName();         // - get call_handle
            CGAddParm( call, op1, CGenType( node->u2.result_type ) );
            PushCGName( call );
            break;
        case OPR_LABEL:                 // label
            CGControl( O_LABEL, NULL, CGLabelHandles[node->u2.label_index] );
            break;
        case OPR_CASE:                  // case label
            if( node->u2.case_info->gen_label ) {
                CGControl( O_LABEL, NULL, CGLabelHandles[node->u2.case_info->label] );
            }
            break;
        case OPR_JUMP:                  // jump
            CGControl( O_GOTO, NULL, CGLabelHandles[node->u2.label_index] );
            break;
        case OPR_JUMPTRUE:              // jump if true
            op1 = PopCGName();
            CGControl( O_IF_TRUE, op1, CGLabelHandles[node->u2.label_index] );
            break;
        case OPR_JUMPFALSE:             // jump if false
            op1 = PopCGName();
            CGControl( O_IF_FALSE, op1, CGLabelHandles[node->u2.label_index] );
            break;
        case OPR_SWITCH:                // switch
            op1 = PopCGName();
            DoSwitch( node, op1 );
            break;
#ifdef __SEH__
        case OPR_TRY:                   // start of try block
            SetTryScope( node->u2.st.parent_scope );
            break;
        case OPR_EXCEPT:
        case OPR_FINALLY:
            CGBigLabel( FEBack( (CGSYM_HANDLE)node->u2.sym_handle ) );
            break;
        case OPR_END_FINALLY:
            EndFinally();
            break;
        case OPR_UNWIND:
            CallTryUnwind( node->u2.st.u.try_index );
            break;
        case OPR_EXCEPT_CODE:
            op1 = TryExceptionInfoAddr();
            op1 = CGUnary( O_POINTS, op1, TY_POINTER );
            PushCGName( CGUnary( O_POINTS, op1, TY_INTEGER ) );
            break;
        case OPR_EXCEPT_INFO:
            PushCGName( TryExceptionInfoAddr() );
            break;
        case OPR_ABNORMAL_TERMINATION:
            PushCGName( TryAbnormalTermination() );
            break;
#endif
#if (_CPU == _AXP) || (_CPU == _PPC) || (_CPU == _MIPS)
        case OPR_VASTART:
            op2 = PopCGName();          // - get offset of parm
            op1 = PopCGName();          // - get address of va_list
            GenVaStart( op1, op2 );
            break;
        case OPR_ALLOCA:
            op1 = PopCGName();          // - get size
            PushCGName( CGUnary( O_STACK_ALLOC, op1, TY_POINTER ) );
            break;
#endif
        default:
           {
             char        msgbuf[MAX_MSG_LEN];

             sprintf( msgbuf, "opr=%d not handled\n", node->opr );
             DebugMsg( msgbuf );
           }
            break;
        }
    }
    if( index > 0 ) {
        CGDone( PopCGName() );
    }
}

static void ThreadNode( TREEPTR node )
{
    if( FirstNode == NULL )
        FirstNode = node;
    if( LastNode != NULL )
        LastNode->u.thread = node;
    LastNode = node;
}

static TREEPTR LinearizeTree( TREEPTR tree )
{
    FirstNode = NULL;
    LastNode = NULL;
    WalkExprTree( tree, ThreadNode, NoOp, NoOp, ThreadNode );
    LastNode->u.thread = NULL;
    return( FirstNode );
}


void EmitInit( void )
{
    SegListHead = NULL;
    ImportSegIdInit();
    Refno = TY_FIRST_FREE;
}


static int NewRefno( void )
{
    return( Refno++ );
}


void EmitAbort( void )
{
}

static TREEPTR GenOptimizedCode( TREEPTR tree )
{
    unroll_type unroll_count;

    unroll_count = 0;
    for( ; tree != NULL; tree = tree->left ) {
        if( tree->op.u2.src_loc.line != SrcLoc.line || tree->op.u2.src_loc.fno != SrcLoc.fno ) {
            if( Saved_CurFunc == SYM_NULL ) {
                FNAMEPTR    flist;

                flist = FileIndexToFName( tree->op.u2.src_loc.fno );
                if( flist->index_db == DBFILE_INVALID ) {
                    char *fullpath;

                    fullpath = FNameFullPath( flist );
                    flist->index_db = DBSrcFile( fullpath );
                }
                DBSrcCue( flist->index_db, tree->op.u2.src_loc.line, 1 );
            }
            SrcLoc = tree->op.u2.src_loc;
        }
        if( tree->op.u1.unroll_count != unroll_count ) {
            unroll_count = tree->op.u1.unroll_count;
            BEUnrollCount( unroll_count );
        }
        /* eliminate functions that are always inlined or not used */
        if( tree->right->op.opr == OPR_FUNCTION ) {     // if start of func
            TREEPTR right;

            right = tree->right;
            if ( ( right->op.u2.func.flags & FUNC_USED ) == 0 ) {
                if( InLineDepth == 0 ) {
                    while( tree->right->op.opr != OPR_FUNCEND ) {
                        tree = tree->left;
                    }
                    break;
                }
            }
        }
        EmitNodes( LinearizeTree( tree->right ) );
#ifdef __SEH__
        if( tree->right->op.opr == OPR_FUNCTION ) {     // if start of func
            if( FuncNodePtr->u2.func.flags & FUNC_USES_SEH ) {
                GenerateTryBlock( tree->left );
            }
        }
#endif
        if( tree->right->op.opr == OPR_FUNCEND ) {
            break;
        }
    }
    return( tree );
}

static void DoInLineFunction( TREEPTR tree )
{   // Push some state info and use InLineDepth for turning some stuff off
    struct func_save    save;

    ++InLineDepth;
    save.func = CurFunc;
    save.func_handle = CurFuncHandle;
    save.funcnode = FuncNodePtr;
    save.cglabel_handles = CGLabelHandles;
    save.labelindex = LabelIndex;
    CurFuncHandle = SYM_NULL;
    FuncNodePtr = NULL;
    CGLabelHandles = NULL;
    LabelIndex = 0;
    GenOptimizedCode( tree );
    CurFunc = save.func;
    SymGet( CurFunc, save.func_handle );  // must be done before changing CurFuncHandle
    CurFuncHandle = save.func_handle;
    FuncNodePtr = save.funcnode;
    CGLabelHandles = save.cglabel_handles;
    LabelIndex = save.labelindex;
    --InLineDepth;
}

static TREEPTR FindFuncStmtTree( SYM_HANDLE sym_handle )
{
    SYMPTR      symptr;

    symptr = SymGetPtr( sym_handle );
    return( symptr->u.func.start_of_func );
}

void GenInLineFunc( SYM_HANDLE sym_handle )
{
    TREEPTR     tree;

    tree = FindFuncStmtTree(sym_handle);
    if( tree != NULL ) {
        DoInLineFunction( tree );
    }
}

bool IsInLineFunc( SYM_HANDLE sym_handle )
{
    bool        ret;
    TREEPTR     tree;
    TREEPTR     right;

    ret = false;
    if( InLineDepth < MAX_INLINE_DEPTH ) {
        tree = FindFuncStmtTree(sym_handle);
        if( tree != NULL ) {
           right = tree->right;
           if( (right->op.u2.func.flags & FUNC_INUSE) == 0 ) {
               ret = ( (right->op.u2.func.flags & FUNC_OK_TO_INLINE) != 0 );
           }
        }
    }
    return( ret );
}

/* This function recursively checks if a function is really used and
   needs to be emitted.
   A function is not inlined and generated if it calls itself recursively,
   or if the inline depth goes over the limit.
   In that case, and for normal function calls, the function is marked
   as FUNC_USED and can't be skipped by the GenOptimizedCode().
*/
static int ScanFunction( TREEPTR tree, int inline_depth )
{
    TREEPTR             right;
    struct func_info   *f;
    int                 marked;

    if( tree == NULL || tree->right == NULL )
        return( 0 );
    f = &tree->right->op.u2.func;

    if( inline_depth == -1 ) {
        /* non-recursive call */
        inline_depth = 0;
    } else if( (f->flags & FUNC_OK_TO_INLINE) && (f->flags & FUNC_INUSE) == 0 &&
        inline_depth < MAX_INLINE_DEPTH ) {
        /* simulate inlining when appropriate */
        inline_depth++;
    } else {
        /* if already examined no need to do it again */
        if( f->flags & FUNC_USED )
            return 0;
        f->flags |= FUNC_USED | FUNC_MARKED;
        return( 1 );
    }

    f->flags |= FUNC_INUSE;
    marked = 0;
    for( ; tree != NULL; tree = tree->left ) {
        for( right = LinearizeTree( tree->right ); right != NULL; right = right->u.thread ) {
            if( right->op.opr == OPR_FUNCNAME ) {
                marked += ScanFunction( FindFuncStmtTree( right->op.u2.sym_handle ), inline_depth );
            }
        }
        if( tree->right->op.opr == OPR_FUNCEND ) {
            break;
        }
    }
    f->flags &= ~FUNC_INUSE;
    return( marked );
}

/* This function scans the source file tree for functions. Any non-static
   function or static function whose address is taken is scanned for
   any functions called, and not inlined.
   These functions are marked as used.
*/
static void PruneFunctions( void )
{
    TREEPTR     tree;
    SYM_ENTRY   sym;
    int         marked;

    marked = 0;
    for( tree = FirstStmt; tree != NULL; tree = tree->left ) {
        if( tree->right->op.opr == OPR_FUNCTION ) {
            SymGet( &sym, tree->right->op.u2.func.sym_handle );
            if( sym.attribs.stg_class != SC_STATIC || ( sym.flags & SYM_ADDR_TAKEN ) ) {
                tree->right->op.u2.func.flags |= FUNC_MARKED | FUNC_USED;
                marked++;
            }
        }
    }
    while( marked ) {
        marked = 0;
        for( tree = FirstStmt; tree != NULL; tree = tree->left ) {
            if( tree->right->op.opr == OPR_FUNCTION ) {
                if (tree->right->op.u2.func.flags & FUNC_MARKED) {
                    tree->right->op.u2.func.flags &= ~FUNC_MARKED;
                    marked += ScanFunction( tree, -1 );
                }
            }
        }
    }
}

static void GenModuleCode( void )
{
    TREEPTR     tree;

    InLineDepth = 0;
//  InLineFuncStack = NULL;
    for( tree = FirstStmt; tree != NULL; tree = tree->left ) {
        tree = GenOptimizedCode( tree );
    }
}

static void NoCodeGenDLL( void )
{
    FEMessage( MSG_FATAL, "Unable to load code generator DLL" );
}

void DoCompile( void )
{
    jmp_buf         *old_env;
    jmp_buf         env;
    cg_init_info    cgi_info;

    old_env = Environment;
    if( setjmp( env ) == 0 ) {
        Environment = &env;
        if( BELoad( NULL ) ) {
            if( ! CompFlags.zu_switch_used ) {
                TargetSwitches &= ~ FLOATING_SS;
            }
#ifndef NDEBUG
            if( TOGGLEDBG( dump_cg ) ) {
                GenSwitches |= ECHO_API_CALLS;
            }
#endif
#ifdef POSITION_INDEPENDANT
            if( CompFlags.rent ) {
                GenSwitches |= POSITION_INDEPENDANT;
            }
#endif
            cgi_info = BEInit( GenSwitches, TargetSwitches, OptSize, ProcRevision );
            if( cgi_info.success ) {
#if _CPU == 386
                if( TargetSwitches & (P5_PROFILING | NEW_P5_PROFILING) ) {
                    FunctionProfileSegId = AddSegName( "TI", "DATA", SEGTYPE_INITFINI );
                }
#endif
                SetSegs();
                BEStart();
                EmitSegLabels();
                if( GenSwitches & DBG_TYPES )
                    EmitDBType();
                EmitSyms();
                EmitCS_Strings();
                SrcLoc.line = 0;
                SrcLoc.column = 0;
                SrcLoc.fno = 0;
                EmitDataQuads();
                FreeDataQuads();
#ifdef __SEH__
                TryRefno = NewRefno();
                BEDefType( TryRefno, 1, TRY_BLOCK_SIZE );
                TryTableBackHandles = NULL;
#endif
                PruneFunctions();
                GenModuleCode();
                FreeStrings();
                FiniSegLabels();
                if( ErrCount != 0 ) {
                    BEAbort();
                }
                BEStop();
                FiniSegBacks();
                FreeGblVars( GlobalSym );
                FreeGblVars( SpecialSyms );
                FreeExtVars();
#ifdef __SEH__
                FreeTryTableBackHandles();
#endif
                BEFini();
                BEUnload();
                SegFini();
            }
        } else {
            NoCodeGenDLL();
        }
    }
    Environment = old_env;
}


static void EmitSym( SYMPTR sym, SYM_HANDLE sym_handle )
{
    TYPEPTR             typ;
    segment_id          segid;
    target_size         size;

    typ = sym->sym_type;
    if( (GenSwitches & DBG_TYPES) && (sym->attribs.stg_class == SC_TYPEDEF) ) {
        if( typ->decl_type != TYP_TYPEDEF ) {
            DBEndName( DBBegName( sym->name, DBG_NIL_TYPE ), DBType( typ ) );
        }
    }
    SKIP_TYPEDEFS( typ );
    CGenType( typ );    /* create refno for ARRAY type, etc */
    if( sym->attribs.stg_class != SC_EXTERN &&  /* if not imported */
        sym->attribs.stg_class != SC_TYPEDEF ) {
        if( ( sym->flags & SYM_FUNCTION ) == 0 ) {
            segid = sym->u.var.segid;
            if( (sym->flags & SYM_INITIALIZED) == 0 || segid == SEG_BSS) {
                BESetSeg( segid );
                AlignIt( typ );
                DGLabel( FEBack( (CGSYM_HANDLE)sym_handle ) );
                /* initialize all bytes to 0 */
                /* if size > 64k, have to break it into chunks of 64k */
                size = SizeOfArg( typ );
                if( segid == SEG_BSS ) {
                    DGUBytes( size );
                } else {
#if _CPU == 8086
                    while( size >= 0x10000 ) {
                        EmitZeros( 0x10000 );
                        size -= 0x10000;
                        if( size == 0 )
                            break;
                        if( segid != SEG_CONST && segid != SEG_DATA ) {
                            ++segid;
                            BESetSeg( segid );
                        }
                    }
#endif
                    if( size != 0 ) {
                        EmitZeros( size );
                    }
                }
            }
        }
    }
}


static void EmitSyms( void )
{
    SYM_HANDLE          sym_handle;
    SYM_ENTRY           sym;

    for( sym_handle = GlobalSym; sym_handle != SYM_NULL; sym_handle = sym.handle ) {
        SymGet( &sym, sym_handle );
        EmitSym( &sym, sym_handle );
        if( ( GenSwitches & DBG_LOCALS )
          && ( sym.sym_type->decl_type != TYP_FUNCTION )
          && ( (sym.flags & SYM_TEMP) == 0 )
          && ( sym.attribs.stg_class != SC_TYPEDEF ) ) {
#if _CPU == 370
            if( sym.attribs.stg_class != SC_EXTERN || (sym.flags & SYM_REFERENCED) ) {
                DBModSym( (CGSYM_HANDLE)sym_handle, TY_DEFAULT );
            }
#else
            DBModSym( (CGSYM_HANDLE)sym_handle, TY_DEFAULT );
#endif
        }
    }
}


static bool DoFuncDefn( SYM_HANDLE funcsym_handle )
{
    bool        parms_reversed;
    SYM_HANDLE  sym_handle;
    cg_type     ret_type;

    SSVar = NULL;
    CurFunc = &CurFuncSym;
    SymGet( CurFunc, funcsym_handle );
    CurFuncHandle = funcsym_handle;
#if ( _CPU == 8086 ) || ( _CPU == 386 )
    if( ! CompFlags.zu_switch_used ) {
        if( CurFunc->mods & FLAG_FARSS ) {      /* function use far stack */
            TargetSwitches |= FLOATING_SS;
        } else {
            TargetSwitches &= ~ FLOATING_SS;
        }
    }
#endif
    ret_type = ReturnType( CGenType( CurFunc->sym_type->object ) );
    CGProcDecl( (CGSYM_HANDLE)funcsym_handle, ret_type );
#if _CPU == 386
    if( TargetSwitches & P5_PROFILING ) {
        const char *fn_name;
        size_t     len;
        segment_id old_segid;

        fn_name = FEName( (CGSYM_HANDLE)funcsym_handle );
        len = strlen( fn_name ) + 1;
        old_segid = BESetSeg( FunctionProfileSegId );
        FunctionProfileBlock = BENewBack( NULL );
        DGLabel( FunctionProfileBlock );
        DGInteger( 0,   TY_INTEGER );
        DGInteger( (unsigned)-1,  TY_INTEGER );
        DGInteger( 0,   TY_INTEGER );
        DGInteger( 0,   TY_INTEGER );
        DGBytes( len, fn_name );
        len &= 0x03;
        if( len ) {
            DGIBytes( 4 - len, 0 );
        }
        BESetSeg( old_segid );
    }
#endif
    if( GenSwitches & DBG_LOCALS ) {
        if( InLineDepth == 0 ) {
            DBModSym( (CGSYM_HANDLE)CurFuncHandle, TY_DEFAULT );
        } else {
            DBBegBlock();
        }
    }
    parms_reversed = false;
    if( CurFunc->u.func.parms != SYM_NULL ) {
        if( GetCallClass( CurFuncHandle ) & REVERSE_PARMS ) {
            ParmReverse( CurFunc->u.func.parms );
            parms_reversed = true;
        } else {
            SYMPTR      sym;

            for( sym_handle = CurFunc->u.func.parms; sym_handle != SYM_NULL; sym_handle = sym->handle ) {
                sym = SymGetPtr( sym_handle );
                if( sym->sym_type->decl_type == TYP_DOT_DOT_DOT )
                    break;
                CDoParmDecl( sym, sym_handle );
            }
        }
    }
    CGLastParm();
    CDoAutoDecl( CurFunc->u.func.locals );
#ifdef __SEH__
    if( FuncNodePtr->u2.func.flags & FUNC_USES_SEH ) {
        CGAutoDecl( (CGSYM_HANDLE)TrySymHandle, TryRefno );
        CallTryInit();                  // generate call to __TryInit
    }
#endif
    return( parms_reversed );
}

static void CDoParmDecl( SYMPTR sym, SYM_HANDLE sym_handle )
{
    TYPEPTR typ;
    cg_type dtype;

    typ = sym->sym_type;
    SKIP_TYPEDEFS( typ );
    dtype = CGenType( typ );
    CGParmDecl( (CGSYM_HANDLE)sym_handle, dtype );
#if _CPU == 386
    if( (GenSwitches & NO_OPTIMIZATION)
      || (!CompFlags.register_conventions && CompFlags.debug_info_some) ) {
#else
    if( GenSwitches & NO_OPTIMIZATION ) {
#endif
        if( GenSwitches & DBG_LOCALS ) {
            DBLocalSym( (CGSYM_HANDLE)sym_handle, TY_DEFAULT );
        }
    }
}

static void ParmReverse( SYM_HANDLE sym_handle )
{
    SYMPTR      sym;

    sym = SymGetPtr( sym_handle );
    if( sym->handle ) {
        ParmReverse( sym->handle );
        sym = SymGetPtr( sym_handle );
    }
    CDoParmDecl( sym, sym_handle );
}

static void CDoAutoDecl( SYM_HANDLE sym_handle )
{
    TYPEPTR             typ;
    cg_type             dtype;
    bool                emit_debug_info;
    bool                emit_extra_info;
    SYM_ENTRY           sym;

    for( ; sym_handle != SYM_NULL; sym_handle = sym.handle ) {
        SymGet( &sym, sym_handle );
        emit_debug_info = false;
        emit_extra_info = false;
        if( (GenSwitches & NO_OPTIMIZATION) )
            emit_debug_info = true;
        if( sym.attribs.stg_class == SC_STATIC ) {
            emit_debug_info = false;
            if( (sym.flags & SYM_EMITTED) == 0 ) {
                if( sym.sym_type->decl_type != TYP_VOID ) {
                    EmitSym( &sym, sym_handle );
                    emit_debug_info = true;
                    SymGet( &sym, sym_handle );
                    sym.flags |= SYM_EMITTED;
                    SymReplace( &sym, sym_handle );
                }
            }
        } else if( sym.attribs.stg_class != SC_EXTERN &&
                   sym.attribs.stg_class != SC_TYPEDEF ) {
            if( sym.flags & SYM_ADDR_TAKEN ) {
                emit_extra_info = true;
            }
            typ = sym.sym_type;
            SKIP_TYPEDEFS( typ );
            switch( typ->decl_type ) {
            case TYP_UNION:
            case TYP_STRUCT:
            case TYP_ARRAY:
            case TYP_FCOMPLEX:
            case TYP_DCOMPLEX:
            case TYP_LDCOMPLEX:
                emit_extra_info = true;
                break;
            default:
                break;
            }
            dtype = CGenType( typ );
            if( sym.flags & SYM_FUNC_RETURN_VAR ) {
                sym.info.return_var = CGTemp( dtype );
                SymReplace( &sym, sym_handle );
            } else {
                CGAutoDecl( (CGSYM_HANDLE)sym_handle, dtype );
            }
        }
#if _CPU != 370
        if( ! CompFlags.debug_info_some )
            emit_extra_info = false;
#endif
        if( emit_debug_info || emit_extra_info ) {
            if( Saved_CurFunc == SYM_NULL ) {  /* if we are not inlining */
                if( GenSwitches & DBG_LOCALS ) {
                    if( (sym.flags & SYM_TEMP) == 0 ) {
                        DBLocalSym( (CGSYM_HANDLE)sym_handle, TY_DEFAULT );
                    }
                }
            }
        }
    }
}


static void FreeSymBackInfo( SYMPTR sym, SYM_HANDLE sym_handle )
{
    if( sym->info.backinfo != NULL ) {
        BEFiniBack( sym->info.backinfo );
        BEFreeBack( sym->info.backinfo );
        sym->info.backinfo = NULL;
        SymReplace( sym, sym_handle );
    }
}

#ifdef __SEH__
static void FreeTrySymBackInfo( void )
{
    SYM_ENTRY   sym;

    SymGet( &sym, TrySymHandle );
    FreeSymBackInfo( &sym, TrySymHandle );
}

static void FreeTryTableBackHandles( void )
{
    try_table_back_handles  *try_backinfo;

    for( ; (try_backinfo = TryTableBackHandles) != NULL; ) {
        TryTableBackHandles = try_backinfo->next;
        BEFreeBack( try_backinfo->back_handle );
        CMemFree( try_backinfo );
    }
}
#endif

static void FreeLocalVars( SYM_HANDLE sym_list )
{
    SYM_HANDLE          sym_handle;
    SYM_ENTRY           sym;

    for( sym_handle = sym_list; sym_handle != SYM_NULL; sym_handle = sym.handle) {
        SymGet( &sym, sym_handle );
        if( sym.attribs.stg_class != SC_EXTERN ) {
            if( (sym.flags & SYM_FUNC_RETURN_VAR) == 0 ) {
                if( sym.sym_type->decl_type != TYP_VOID ) {
                    FreeSymBackInfo( &sym, sym_handle );
                }
            }
        }
    }
}


static void FreeGblVars( SYM_HANDLE sym_handle )
{
    SYMPTR      sym;

    for( ; sym_handle != SYM_NULL; sym_handle = sym->handle ) {
        sym = SymGetPtr( sym_handle );
        if( sym->info.backinfo != NULL ) {
//              BEFiniBack( sym->info.backinfo );
            BEFreeBack( sym->info.backinfo );
        }
    }
}

static void RelExtVars( SYM_HANDLE sym_handle )
{
    SYMPTR      sym;

    for( ; sym_handle != SYM_NULL; sym_handle = sym->handle ) {
        sym = SymGetPtr( sym_handle );
        if( (sym->flags & SYM_FUNC_RETURN_VAR) == 0 ) {
            if( sym->attribs.stg_class == SC_EXTERN
              || sym->attribs.stg_class == SC_STATIC
              || sym->sym_type->decl_type == TYP_VOID ) {
                if( sym->info.backinfo != NULL ) {
                    BEFreeBack( sym->info.backinfo );
                }
            }
        }
    }
}

static void FreeExtVars( void )
{
    SYM_LIST_HEADS   *sym_list_head;
    SYM_LIST_HEADS   *next_sym_list_head;

    for( sym_list_head = SymListHeads; sym_list_head != NULL; sym_list_head = next_sym_list_head ) {
        RelExtVars( sym_list_head->sym_head );
        next_sym_list_head = sym_list_head->next;
        CMemFree( sym_list_head );
    }
}

cg_type CGenType( TYPEPTR typ )
{
    cg_type         dtype;
    type_modifiers  flags;
    align_type      align;

    SKIP_TYPEDEFS( typ );
    switch( typ->decl_type ) {

    case TYP_FCOMPLEX:
    case TYP_DCOMPLEX:
    case TYP_LDCOMPLEX:
    case TYP_STRUCT:
    case TYP_UNION:
        if( typ->object != NULL ) {
            /* structure has a zero length array as last field */
            dtype = NewRefno();
            align = GetTypeAlignment( typ );
            BEDefType( dtype, align, TypeSize(typ) );
        } else if( typ->u.tag->refno == 0 ) {
            dtype = NewRefno();
            align = GetTypeAlignment( typ );
            BEDefType( dtype, align, TypeSize(typ) );
            typ->u.tag->refno = dtype;
        } else {
            dtype = typ->u.tag->refno;
        }
        break;
    case TYP_ARRAY:
        if( typ->u.array->refno == 0 ) {
            dtype = NewRefno();
            align = GetTypeAlignment( typ );
            BEDefType( dtype, align, SizeOfArg( typ ) );
            typ->u.array->refno = dtype;
        }
        dtype = typ->u.array->refno;
        break;
    case TYP_FIELD:
    case TYP_UFIELD:
        dtype = CGDataType[typ->u.f.field_type];
        break;
    case TYP_FUNCTION:
        dtype = CodePtrType( FLAG_NONE );
        break;
    case TYP_POINTER:
        flags = typ->u.p.decl_flags;
        dtype = PtrType( typ->object, flags );
        break;
    case TYP_ENUM:
        typ = typ->object;
        /* fall through */
    default:
        dtype = CGDataType[typ->decl_type];
    }
    return( dtype );
}


static cg_type CodePtrType( type_modifiers flags )
{
#if ( _CPU == 8086 ) || ( _CPU == 386 )
    cg_type     dtype;

    if( flags & FLAG_FAR ) {
        dtype = TY_LONG_CODE_PTR;
    } else if( flags & FLAG_NEAR ) {
        dtype = TY_NEAR_CODE_PTR;
    } else {
        dtype = TY_CODE_PTR;
    }
    return( dtype );
#else
    /* unused parameters */ (void)flags;

    return( TY_CODE_PTR );
#endif
}


extern cg_type PtrType( TYPEPTR typ, type_modifiers flags )
{
    cg_type     dtype;

    SKIP_TYPEDEFS( typ );
    if( typ->decl_type == TYP_FUNCTION ) {
        dtype = CodePtrType( flags );
    } else {
#if ( _CPU == 8086 ) || ( _CPU == 386 )
        if( flags & FLAG_FAR ) {
            dtype = TY_LONG_POINTER;
        } else if( flags & FLAG_HUGE ) {
            dtype = TY_HUGE_POINTER;
        } else if( flags & FLAG_NEAR ) {
            dtype = TY_NEAR_POINTER;
        } else {
            dtype = TY_POINTER;
        }
#else
        dtype = TY_POINTER;
#endif
    }
    return( dtype );
}


static segment_id StringSegment( STR_HANDLE strlit )
{
#if ( _CPU == 8086 ) || ( _CPU == 386 )
    if( strlit->flags & STRLIT_FAR )
        return( FarStringSegId );
#endif
    if( strlit->flags & STRLIT_CONST )
        return( SEG_CODE );
    return( SEG_CONST );
}

void EmitStrPtr( STR_HANDLE str_handle, cg_type pointer_type )
{
    str_handle->ref_count++;
    Emit1String( str_handle );
    DGBackPtr( str_handle->back_handle, StringSegment( str_handle ), 0, pointer_type );
}


static void Emit1String( STR_HANDLE str_handle )
{
    if( str_handle->back_handle == NULL ) {
        str_handle->back_handle = BENewBack( NULL );
        if( (str_handle->flags & STRLIT_CONST) == 0 ) {
            EmitLiteral( str_handle );
        }
    }
}


target_size EmitBytes( STR_HANDLE strlit )
{
    DGBytes( strlit->length, strlit->literal );
    return( strlit->length );
}


static void EmitLiteral( STR_HANDLE strlit )
{
    segment_id  old_segid;

    old_segid = BESetSeg( StringSegment( strlit ) );
    if( strlit->flags & STRLIT_WIDE ) {
        DGAlign( TARGET_SHORT );    /* NT requires word aligned wide strings */
    }
    DGLabel( strlit->back_handle );
    EmitBytes( strlit );
    BESetSeg( old_segid );
}


static void FreeStrings( void )
{
    STR_HANDLE      strlit;
    STR_HANDLE      strlit_next;
    str_hash_idx    hash;

    for( hash = 0; hash < STRING_HASH_SIZE; hash++ ) {
        for( strlit = StringHash[hash]; strlit != NULL; strlit = strlit_next ) {
            strlit_next = strlit->next_string;
            if( strlit->back_handle != NULL ) {
                BEFiniBack( strlit->back_handle );
                BEFreeBack( strlit->back_handle );
                strlit->back_handle = NULL;
            }
            FreeLiteral( strlit );
        }
        StringHash[hash] = NULL;
    }
}


static void DumpCS_Strings( STR_HANDLE strlit )
{
    for( ; strlit != NULL; strlit = strlit->next_string ) {
        if( (strlit->flags & STRLIT_CONST) && strlit->ref_count != 0 ) {
            strlit->back_handle = BENewBack( NULL );
            EmitLiteral( strlit );
        }
    }
}


static void EmitCS_Strings( void )
{
    str_hash_idx    hash;

    if( CompFlags.strings_in_code_segment ) {
        for( hash = STRING_HASH_SIZE; hash-- > 0; ) {
            DumpCS_Strings( StringHash[hash] );
        }
    }
}

#ifdef __SEH__
static void GenerateTryBlock( TREEPTR tree )
{
    TREEPTR     stmt;
    tryindex_t  try_index;
    tryindex_t  max_try_index;

    try_index = 0;
    max_try_index = TRYSCOPE_NONE;
    for( ; tree != NULL; tree = tree->left ) {
        stmt = tree->right;
        if( stmt->op.opr == OPR_FUNCEND )
            break;
        switch( stmt->op.opr ) {
        case OPR_TRY:
            try_index = stmt->op.u2.st.u.try_index;
            if( max_try_index < try_index )
                max_try_index = try_index;
            break;
        case OPR_EXCEPT:
        case OPR_FINALLY:
            ValueStack[try_index] = stmt;
            break;
        default:
            break;
        }
    }
    if( max_try_index != TRYSCOPE_NONE ) {
        segment_id              old_segid;
        BACK_HANDLE             except_label;
        BACK_HANDLE             except_table;
        try_table_back_handles  *try_backinfo;

        old_segid = BESetSeg( SEG_DATA );
        except_table = BENewBack( NULL );
        try_backinfo = (try_table_back_handles *)CMemAlloc( sizeof( try_table_back_handles ) );
        try_backinfo->back_handle = except_table;
        try_backinfo->next = TryTableBackHandles;
        TryTableBackHandles = try_backinfo;
        DGLabel( except_table );
        for( try_index = 0; try_index <= max_try_index; try_index++ ) {
            stmt = ValueStack[try_index];
            DGInteger( stmt->op.u2.st.parent_scope, TY_UINT_1 );  // parent index
            if( stmt->op.opr == OPR_EXCEPT ) {
                DGInteger( 0, TY_UINT_1 );
            } else {
                DGInteger( 1, TY_UINT_1 );
            }
            except_label = FEBack( (CGSYM_HANDLE)stmt->op.u2.st.u.try_sym_handle );
            DGBackPtr( except_label, FESegID( (CGSYM_HANDLE)CurFuncHandle ), 0, TY_CODE_PTR );
        }
        BESetSeg( old_segid );
        SetTryTable( except_table );
        BEFiniBack( except_table );
    }
}
#endif
