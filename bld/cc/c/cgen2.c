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


// Interface to Optimizing code generator

#include "cvars.h"
#include "cg.h"
#include "cgdefs.h"
#include "cgswitch.h"
#include "pragdefn.h"
#include "standard.h"
#include "cgprotos.h"
#include "cgdllcli.h"
#include <stdarg.h>
#include <malloc.h>

extern  void    InitExpressCode(int,int);
    extern      void    GenNewProc(cg_sym_handle);
extern  TREEPTR GenExpressCode(TREEPTR,int,TREEPTR);
extern  void    EmptyQueue(void);
extern  int     StartWCG();


static  struct  local_vars {
        struct local_vars       *next;
        SYM_HANDLE              sym_list;
} *LocalVarList;

extern  SYM_LISTS       *SymListHeads;

#ifdef __SEH__
#include "tryblock.h"

static int  TryRefno;

struct  try_table_back_handles {
        struct try_table_back_handles   *next;
        back_handle                     try_table_back_handle;
} *TryTableBackHandles;
#endif

#define PushCGName(name)        cgnames[index++] = name
#define PopCGName()             cgnames[--index]

static label_handle    *CGLabelHandles;
static TREEPTR         FirstNode;
static TREEPTR         LastNode;
static OPNODE         *FuncNodePtr;
static int             Refno;
static temp_handle     SSVar;
SYM_HANDLE      Saved_CurFunc;

extern  int     LabelIndex;
extern  TREEPTR FirstStmt;
static  int     InLineDepth;

struct func_save {
        SYMPTR          func;
        SYM_HANDLE      func_handle;
        OPNODE         *funcnode;
        label_handle   *cglabel_handles;
        int             labelindex;
};

static  char    CGDataType[] = {
        T_INT_1,        /* TYPE_CHAR */
        T_UINT_1,       /* TYPE_UCHAR */
        T_INT_2,        /* TYPE_SHORT */
        T_UINT_2,       /* TYPE_USHORT */
        T_INTEGER,      /* TYPE_INT */
        TY_UNSIGNED,    /* TYPE_UINT */
        T_INT_4,        /* TYPE_LONG */
        T_UINT_4,       /* TYPE_ULONG */
        T_INT_8,        /* TYPE_LONG64*/
        T_UINT_8,       /* TYPE_ULONG64 */
        T_SINGLE,       /* TYPE_FLOAT */
        TY_DOUBLE,      /* TYPE_DOUBLE */
        T_POINTER,      /* TYPE_POINTER */
        T_POINTER,      /* TYPE_ARRAY */
        T_POINTER,      /* TYPE_STRUCT */
        T_POINTER,      /* TYPE_UNION */
        TY_DEFAULT,     /* TYPE_FUNCTION */
        TY_DEFAULT,     /* TYPE_FIELD */
        T_INTEGER,      /* TYPE_VOID */
        T_INTEGER,      /* TYPE_ENUM */
        T_INTEGER,      /* TYPE_TYPEDEF */
        T_INTEGER,      /* TYPE_UFIELD unsigned bit field */
        T_INTEGER,      /* TYPE_DOT_DOT_DOT  for the ... in prototypes */
        T_INTEGER,      /* TYPE_PLAIN_CHAR */
        T_INTEGER,      /* TYPE_UNUSED (a unref'd function) */
        T_INTEGER,      /* TYPE_WCHAR L'c' - a wide character constant */
        T_POINTER,      /* TYPE_REF C++ reference */
        T_INTEGER       /* TYPE_CLASS  C++ class */
  };

static  char    CGOperator[] = {
        O_PLUS,         //      OPR_ADD,        // +
        O_MINUS,        //      OPR_SUB,        // -
        O_TIMES,        //      OPR_MUL,        // *
        O_DIV,          //      OPR_DIV,        // /
        O_UMINUS,       //      OPR_NEG,        // negate
        0,              //      OPR_CMP,        // compare
        O_MOD,          //      OPR_MOD,        // %
        O_COMPLEMENT,   //      OPR_COM,        // ~
        O_FLOW_NOT,     //      OPR_NOT,        // !
        O_OR,           //      OPR_OR,         // |
        O_AND,          //      OPR_AND,        // &
        O_XOR,          //      OPR_XOR,        // ^
        O_RSHIFT,       //      OPR_RSHIFT,     // >>
        O_LSHIFT,       //      OPR_LSHIFT,     // <<
        O_GETS,         //      OPR_EQUALS,     // lvalue = rvalue
        O_OR,           //      OPR_OR_EQUAL,   // |=
        O_AND,          //      OPR_AND_EQUAL,  // &=
        O_XOR,          //      OPR_XOR_EQUAL,  // ^=
        O_RSHIFT,       //      OPR_RSHIFT_EQUAL,// >>=
        O_LSHIFT,       //      OPR_LSHIFT_EQUAL,// <<=
        O_PLUS,         //      OPR_PLUS_EQUAL, // +=
        O_MINUS,        //      OPR_MINUS_EQUAL,// -=
        O_TIMES,        //      OPR_TIMES_EQUAL,// *=
        O_DIV,          //      OPR_DIV_EQUAL,  // /=
        O_MOD,          //      OPR_MOD_EQUAL,  // %=
        0,              //      OPR_QUESTION,   // ?
        0,              //      OPR_COLON,      // :
        O_FLOW_OR,      //      OPR_OR_OR,      // ||
        O_FLOW_AND,     //      OPR_AND_AND,    // &&
        O_POINTS,       //      OPR_POINTS,     // *ptr
        0,              //      OPR_UNUSED1,    // spare
        0,              //      OPR_UNUSED2,    // spare
        O_PLUS,         //      OPR_POSTINC,    // lvalue++
        O_MINUS,        //      OPR_POSTDEC,    // lvalue--
        O_CONVERT,      //      OPR_CONVERT,    // do conversion
};

static  char    CC2CGOp[] = { O_EQ, O_NE, O_LT, O_LE, O_GT, O_GE };
#ifdef HEAP_SIZE_STAT
struct heap_stat {
    int free;
    int used;
};

static int heap_size( struct heap_stat *stat ){
    struct _heapinfo h_info;
    int heap_status;
    h_info._pentry = NULL;
    stat->free = 0;
    stat->used = 0;
    for(;;){
        heap_status = _heapwalk( &h_info );
        if( heap_status != _HEAPOK )break;
        if( h_info._useflag ){
            stat->used += h_info._size;
        }else{
            stat->free += h_info._size;
        }
    }
    return( heap_status );
}
#endif

static void StartFunction( OPNODE *node )
{
    FuncNodePtr = node;
    if( InLineDepth == 0 ){
        LocalVarList = NULL;
    }

    FuncNodePtr->func.flags |=  FUNC_INUSE;
    DoFuncDefn( node->func.sym_handle );
}

static void DefineLabels( OPNODE *node )
{
    int         i;

    LabelIndex = node->label_count;
    if( LabelIndex != 0 ) {
        CGLabelHandles = (label_handle *)CMemAlloc( (LabelIndex + 1) *
                                                sizeof(label_handle) );
        for( i = 1; i <= LabelIndex; i++ ) {
            CGLabelHandles[i] = BENewLabel();
        }
    }
}

static struct local_vars *ReleaseVars( SYM_HANDLE sym_list,
                                struct local_vars *local_var_list )
{
    struct local_vars *local_entry;

    local_entry = (struct local_vars *)
                    CMemAlloc( sizeof( struct local_vars ) );
    local_entry->next = local_var_list;
    local_entry->sym_list = sym_list;
    return( local_entry );
}

static void RelLocalVars( struct local_vars *local_var_list ) /* 12-mar-92 */
{
    struct local_vars   *local_entry;

    while( local_var_list != NULL ) {
        FreeLocalVars( local_var_list->sym_list );
        local_entry = local_var_list;
        local_var_list = local_var_list->next;
        CMemFree( local_entry );
    }
}

static void EndFunction( OPNODE *node )
{
    int         i;
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
    if( FuncNodePtr->func.flags & FUNC_USES_SEH ) {
        CallTryFini();          // generate call to __TryFini
        FreeTrySymBackInfo();
    }
#endif
    if( node->sym_handle == 0 ) {
        dtype = CGenType( CurFunc->sym_type->object );
        CGReturn( NULL, dtype );
    } else {                            // return value
        SymGet( &sym, node->sym_handle );
        dtype = CGenType( sym.sym_type );
        name = CGTempName( sym.info.return_var, dtype );
        name = CGUnary( O_POINTS, name, dtype );
        if( CompFlags. returns_promoted ) {
            dtype = FEParmType( NULL, NULL, dtype );
        }
        CGReturn( name, dtype );
    }
    FreeLocalVars( CurFunc->u.func.parms );
    FreeLocalVars( CurFunc->u.func.locals );
    if( InLineDepth == 0 ){
        RelLocalVars( LocalVarList );
    }
    if( GenSwitches & DBG_LOCALS ) {
        if( InLineDepth != 0 ){
            DBEndBlock();
        }
    }
    FuncNodePtr->func.flags &= ~FUNC_INUSE;
}

static void ReturnExpression( OPNODE *node, cg_name expr )
{
    cg_name     name;
    cg_type     dtype;
    SYM_ENTRY   sym;

    SymGet( &sym, node->sym_handle );
    dtype = CGenType( sym.sym_type );
    name = CGTempName( sym.info.return_var, dtype );
    CGDone( CGAssign( name, expr, dtype ) );
}

static cg_type DataPointerType( OPNODE *node )
{
#if _MACHINE == _PC
    cg_type     dtype;

    if( Far16Pointer( node->flags ) ) {
        dtype = T_POINTER;
    } else if( node->flags & OPFLAG_NEARPTR ) {
        dtype = T_NEAR_POINTER;
    } else if( node->flags & OPFLAG_FARPTR ) {
        dtype = T_LONG_POINTER;
    } else if( node->flags & OPFLAG_HUGEPTR ) {
        dtype = T_HUGE_POINTER;
    } else {
        dtype = T_POINTER;
    }
    return( dtype );
#else
    node;
    return( T_POINTER );
#endif
}


local cg_name ForceVolatileFloat( cg_name name, TYPEPTR typ ) /* 05-sep-92 */
{
    if( CompFlags.op_switch_used ) {
        if( typ->decl_type == TYPE_FLOAT  ||
            typ->decl_type == TYPE_DOUBLE ) {
            name = CGVolatile( name );
        }
    }
    return( name );
}

local cg_name LoadSegment( cg_name seg_label )/* 18-jan-92 */
{
    temp_handle temp_name;
    cg_name     name;
    cg_name     left;

    temp_name = CGTemp( T_LONG_POINTER );
    name = CGTempName( temp_name, T_LONG_POINTER );
    left = CGAssign( name, seg_label, T_LONG_POINTER );
//  name = CGTempName( temp_name, T_LONG_POINTER );
    name = CGBinary( O_PLUS, left, CGInteger( TARGET_INT, TY_UNSIGNED ),
                                T_LONG_POINTER );
//  name = CGBinary( O_COMMA, left, name, T_LONG_POINTER );
    return( name );
}

static cg_name PushSymSeg( OPNODE *node )
{
    cg_name     segname;
    SYMPTR      sym;
    SYM_HANDLE  sym_handle;

    sym_handle = node->sym_handle;
    if( sym_handle == 0 ) {         /* 30-nov-91 */
        segname = CGInteger( 0, TY_UNSIGNED );
    } else {
        if( sym_handle == Sym_CS ) { /* 23-jan-92 */
            segname = CGFEName(  CurFuncHandle, T_LONG_CODE_PTR );
        } else if( sym_handle == Sym_SS ) { /* 13-dec-92 */
            if( SSVar == NULL ) {
                SSVar = CGTemp( T_UINT_2 );
            }
            segname = CGTempName( SSVar, T_UINT_2 );
        } else {
            sym = SymGetPtr( sym_handle );
    //      if( sym->name[0] == '.' ) {  /* if segment label 15-mar-92 */
    //          segname = LoadSegment(
     //                 CGBackName( sym->info.backinfo, T_LONG_POINTER ) );
      //    } else {
                segname = CGFEName( sym_handle, T_UINT_2 );
       //   }
        }
    }
    return( segname );
}

cg_name MakeFarPtr( SYM_HANDLE sym_handle, cg_name offset_part )
{
    cg_name     segname;
    SYMPTR      sym;

    if( sym_handle == 0 ) {         /* 30-nov-91 */
        segname = CGInteger( 0, TY_UNSIGNED );
    } else {
        if( sym_handle == Sym_CS ) { /* 23-jan-92 */
            segname = CGBackName( CurFunc->info.backinfo, T_LONG_POINTER );
        } else if( sym_handle == Sym_SS ) { /* 13-dec-92 */
            if( SSVar == NULL ) {
                SSVar = CGTemp( T_UINT_2 );
            }
            segname = CGTempName( SSVar, T_UINT_2 );
        } else {
            sym = SymGetPtr( sym_handle );
            if( sym->name[0] == '.' ) {  /* if segment label 15-mar-92 */
                segname = LoadSegment(
                        CGBackName( sym->info.backinfo, T_LONG_POINTER ) );
            } else {
                segname = CGFEName( sym_handle, T_UINT_2 );
            }
            segname = CGUnary( O_POINTS, segname, T_UINT_2 );
        }
    }
    return( CGBinary( O_CONVERT, offset_part, segname, T_LONG_POINTER ) );
}

#ifdef __SEH__
cg_name TryFieldAddr( unsigned offset )
{
    cg_name     name;

    name = CGFEName( TrySymHandle, TryRefno );
    name = CGBinary( O_PLUS,
                     name,
                     CGInteger( offset, TY_UNSIGNED ),
                     T_POINTER );
    name = CGVolatile( name );
    return( name );
}

cg_name TryExceptionInfoAddr()
{
    cg_name     name;

    name = TryFieldAddr( offsetof( struct try_block, exception_info ) );
//    name = CGUnary( O_POINTS, name, T_POINTER );
//    name = CGUnary( O_POINTS, name, T_POINTER );
    return( name );
}

void SetTryTable( back_handle except_table )
{
    cg_name     name;
    cg_name     table;

    name = TryFieldAddr( offsetof( struct try_block, scope_table ) );
    table = CGBackName( except_table, T_POINTER );
    CGDone( CGAssign( name, table, T_POINTER ) );
}

void SetTryScope( int scope )
{
    cg_name     name;

    name = TryFieldAddr( offsetof( struct try_block, scope_index ) );
    CGDone( CGAssign( name, CGInteger( scope, T_UINT_1 ), T_UINT_1 ) );
}

void EndFinally()
{
    cg_name      name;
    cg_name      func;
    LABEL_HANDLE label_handle;
    call_handle  call_list;

    label_handle = BENewLabel();
    name = TryFieldAddr( offsetof( struct try_block, unwindflag ) );
    name = CGUnary( O_POINTS, name, T_UINT_1 );
    name = CGCompare( O_EQ, name, CGInteger( 0, T_UINT_1 ), T_UINT_1 );
    CGControl( O_IF_TRUE, name, label_handle );
    func = CGFEName( SymFinally, T_CODE_PTR );
    call_list = CGInitCall( func, T_INTEGER, SymFinally );
    CGDone( CGCall( call_list ) );
    CGControl( O_LABEL, NULL, label_handle );
    BEFiniLabel( label_handle );
}

cg_name TryAbnormalTermination()
{
    cg_name      name;

    name = TryFieldAddr( offsetof( struct try_block, unwindflag ) );
    name = CGUnary( O_POINTS, name, T_UINT_1 );
    return( name );
}

void CallTryRtn( SYM_HANDLE try_rtn, cg_name parm )
{
    call_handle call_list;

    call_list = CGInitCall( CGFEName( try_rtn, T_POINTER ),
                            T_INTEGER, try_rtn );
    CGAddParm( call_list, parm, T_POINTER );
    CGDone( CGCall( call_list ) );
}

void CallTryInit()
{
    CallTryRtn( SymTryInit, CGFEName( TrySymHandle, T_POINTER ) );
}

void CallTryFini()
{
    cg_name     name;

    name = TryFieldAddr( offsetof( struct try_block, next ) );
    name = CGUnary( O_POINTS, name, T_POINTER );
    CallTryRtn( SymTryFini, name );
}

void TryUnwind( int scope_index )
{
    call_handle call_list;
    cg_name     parm;

    call_list = CGInitCall( CGFEName( SymTryUnwind, T_POINTER ),
                            T_INTEGER, SymTryUnwind );
    parm = CGInteger( scope_index, T_UINT_1 );
    CGAddParm( call_list, parm, T_INTEGER );
    CGDone( CGCall( call_list ) );
}
#endif

#if _MACHINE == _ALPHA
local void GenVaStart( cg_name op1, cg_name offset )
{
    cg_name     name;
    cg_name     baseptr;

    baseptr = CGVarargsBasePtr( T_POINTER );
    name = CGLVAssign( op1, baseptr, T_POINTER );
    name = CGBinary( O_PLUS, name, CGInteger( TARGET_POINTER, T_INTEGER ),
                                        T_POINTER );
    name = CGAssign( name, offset, T_INTEGER );
    CGDone( name );
}
#elif _MACHINE == _PPC
local void GenVaStart( cg_name op1, cg_name offset )
{
    cg_name     name;

    offset = offset;
    name = CGUnary( O_VA_START, op1, T_POINTER );
    CGDone( name );
}
#endif

static cg_name PushSym( OPNODE *node )
{
    cg_name     name;
    cg_type     dtype;
    TYPEPTR     typ;
    SYM_ENTRY   sym;

    SymGet( &sym, node->sym_handle );
    typ = sym.sym_type;
    if( sym.flags & SYM_FUNCTION ){
        dtype = CodePtrType( sym.attrib );
    }else{
        dtype = CGenType( typ );
    }
    if( sym.flags & SYM_FUNC_RETURN_VAR ) {
        name = CGTempName( sym.info.return_var, dtype );
    } else {
        name = CGFEName( node->sym_handle, dtype );
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

    SymGet( &sym, node->sym_handle );
    typ = sym.sym_type;
    if( sym.flags & SYM_FUNCTION ){
        dtype = CodePtrType( sym.attrib );
    }else{
         dtype = CGenType( typ );
    }
    if( sym.flags & SYM_FUNC_RETURN_VAR ) {
        name = CGTempName( sym.info.return_var, dtype );
    } else {
        name = CGFEName( node->sym_handle, dtype );
     // if( (sym.attrib & FLAG_VOLATILE) ||
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
        typ = node->result_type;
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

    // node->result_type is the type of the data
    // for the O_PLUS we want a pointer type
    name = CGBinary( O_PLUS, op1, op2, DataPointerType( node ) );
    typ = node->result_type;
    if( typ->decl_type == TYPE_FIELD || typ->decl_type == TYPE_UFIELD ) {
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
    // node->result_type is the type of the data
    // for the O_PLUS we want a pointer type
#if _CPU == 386
    if( Far16Pointer( node->flags ) ) {
        op1 = CGUnary( O_PTR_TO_NATIVE, op1, T_POINTER );
    }
#endif
//  rvalue has already been done on left side of tree
//    op1 = CGUnary( O_POINTS, op1, DataPointerType( node ) );
    return( DotOperator( op1, node, op2 ) );
}

static cg_name IndexOperator( cg_name op1, OPNODE *node, cg_name op2 )
{
    long        element_size;
    int         index_type;

    // node->result_type is the type of the data
    // for the O_PLUS we want a pointer type
    // op2 needs to be multiplied by the element size of the array

#if _CPU == 386
    if( Far16Pointer( node->flags ) ) {
        op1 = CGUnary( O_PTR_TO_NATIVE, op1, T_POINTER );
    }
#endif
    element_size = SizeOfArg( node->result_type );
    if( element_size != 1 ) {
        index_type = T_INTEGER;
        #if _CPU == 8086
            if( node->flags & OPFLAG_HUGEPTR )  index_type = T_INT_4;
        #endif
        op2 = CGBinary( O_TIMES, op2,
                CGInteger( element_size, T_INTEGER ), index_type );
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

    typ = node->result_type;
    while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
    name = CGBinary( CGOperator[node->opr], op1, op2, CGenType( typ ) );
//  if( typ->decl_type == TYPE_POINTER ) {
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
#ifdef _LONG_DOUBLE_
    double      doubleval;
    long_double ld;
#endif
    auto char   buffer[32];

    dtype = CGDataType[ node->const_type ];
    switch( node->const_type ) {
    case TYPE_CHAR:
    case TYPE_UCHAR:
    case TYPE_SHORT:
    case TYPE_USHORT:
    case TYPE_INT:
    case TYPE_UINT:
    case TYPE_LONG:
    case TYPE_ULONG:
    case TYPE_POINTER:
        name = CGInteger( node->ulong_value, dtype );
        break;
    case TYPE_LONG64:
    case TYPE_ULONG64:
        name = CGInt64( node->ulong64_value, dtype );
        break;
    case TYPE_FLOAT:
    case TYPE_DOUBLE:
        flt = node->float_value;
        if( flt->len != 0 ) {                   // if still in string form
            name = CGFloat( flt->string, dtype );
        } else {                                // else in binary form
#ifdef _LONG_DOUBLE_
            ld = flt->ld;
            __LDFD( (long_double near *)&ld, (double near *)&doubleval );
            ftoa( doubleval, buffer );
#else
            ftoa( flt->ld.value, buffer );
#endif
            name = CGFloat( buffer, dtype );
        }
        break;
    }
    return( name );
}

static cg_name PushString( OPNODE *node )
{
    STRING_LITERAL      *string;

    string = node->string_handle;
    Emit1String( string );
    return( CGBackName( string->cg_back_handle, T_UINT_1 ) );
}

static cg_name DoIndirection( OPNODE *node, cg_name name )
{
    TYPEPTR     typ;

    // check for special kinds of pointers, eg. call __Far16ToFlat
    typ = node->result_type;
#if _CPU == 386
    if( Far16Pointer( node->flags ) ) {
        name = CGUnary( O_PTR_TO_NATIVE, name, T_POINTER );
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
    if( node->oldptr_class == PTR_FAR16 ) {
        name = CGUnary( O_PTR_TO_NATIVE, name, T_POINTER );
    } else if( node->newptr_class == PTR_FAR16 ) {
        name = CGUnary( O_PTR_TO_FOREIGN, name, T_POINTER );
    }
#endif
    return( name );
}

static call_handle InitFuncCall( OPNODE *node )
{
    cg_name     name;
    cg_type     dtype;
    TYPEPTR     typ;
    SYMPTR      sym;

    sym = SymGetPtr( node->sym_handle );
    typ = sym->sym_type;
    dtype = CGenType( typ );
    name = CGFEName( node->sym_handle, dtype );
//    dtype = FESymType( sym );
    while( typ->decl_type == TYPE_TYPEDEF )  typ = typ->object;
    return( CGInitCall( name, CGenType( typ->object ), node->sym_handle ) );
}

static call_handle InitIndFuncCall( OPNODE *node, cg_name name )
{
    TYPEPTR     typ;
    SYMPTR      sym;

    sym = SymGetPtr( node->sym_handle );
    typ = sym->sym_type;
    while( typ->decl_type == TYPE_TYPEDEF )  typ = typ->object;
    return( CGInitCall( name, CGenType( typ->object ), node->sym_handle ) );
}

local void DoSwitch( OPNODE *node, cg_name name )
{
    sel_handle  table;
    SWITCHPTR   sw;
    CASEPTR     ce;

    table = CGSelInit();
    sw = node->switch_info;
    for( ce = sw->case_list; ce; ce = ce->next_case ) {
        CGSelCase( table, CGLabelHandles[ ce->label ], ce->value );
    }
    CGSelOther( table, CGLabelHandles[ sw->default_label ] );
    CGSelect( table, name );
}

static bool IsStruct( TYPEPTR typ ){
/*********************************/
    while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
    if( typ->decl_type == TYPE_STRUCT  ||
        typ->decl_type == TYPE_UNION ) {
            return( TRUE );
    }
    return( FALSE );
}

void EmitNodes( TREEPTR tree )
{
    cg_name     op1;
    cg_name     op2;
    cg_name     expr;
    call_handle call_list;
    cg_name     *cgnames;
    unsigned    index;
    OPNODE      *node;

    index = 0;
    cgnames = (cg_name *)&ValueStack[0];
    for( ; tree != NULL; tree = tree->thread ) {
        node = &tree->op;
        switch( node->opr ) {
        case OPR_FUNCTION:              // start of function
            StartFunction( node );
            break;
        case OPR_NEWBLOCK:              // start of new block { vars; }
            DBBegBlock();
            DoAutoDecl( node->sym_handle );
            break;
        case OPR_ENDBLOCK:              // end of new block { vars; }
            DBEndBlock();
            LocalVarList = ReleaseVars( node->sym_handle, LocalVarList );
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
          {
            cg_type     dtype;

            op2 = PopCGName();          // get right opnd
            op1 = PopCGName();          // get lvalue
            if( node->flags & OPFLAG_VOLATILE ) { // is lvalue volatile
                 op1 = CGVolatile( op1 );
            }
            if( IsStruct( node->result_type ) ){
                dtype = DataPointerType( node );
                op1 = CGLVAssign( op1, op2, CGenType( node->result_type ));
                op1 = PushRValue( node, op1 );
            }else{
                dtype =  CGenType( node->result_type );
                op1 = CGAssign( op1, op2, CGenType( node->result_type ));
            }
            PushCGName( op1 );
          } break;
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
                                    CGenType( node->result_type ) ) );
            break;
        case OPR_COMMA:                 // expr , expr
          {
            cg_type     dtype;
            op2 = PopCGName();
            op1 = PopCGName();
            dtype =  IsStruct( node->result_type ) ?
                   DataPointerType( node ) : CGenType( node->result_type );
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
                                    CGenType( node->result_type ) ) );
            break;
        case OPR_NEG:                   // negate
            op1 = PopCGName();
            PushCGName( CGUnary( O_UMINUS, op1,
                                    CGenType( node->result_type ) ) );
            break;
        case OPR_CMP:                   // compare
            op2 = PopCGName();
            op1 = PopCGName();
            PushCGName( CGCompare( CC2CGOp[ node->cc ], op1, op2,
                                    CGenType( node->compare_type ) ) );
            break;
        case OPR_COM:                   // ~
            op1 = PopCGName();
            PushCGName( CGUnary( O_COMPLEMENT, op1,
                                    CGenType( node->result_type ) ) );
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
            dtype =  IsStruct( node->result_type ) ?
                   DataPointerType( node ) : CGenType( node->result_type );
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
            PushCGName( CGFlow( CGOperator[ node->opr ], op1, op2 ) );
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
                                    CGenType( node->result_type ) ) );
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
            temp_name = CGTemp( T_LONG_POINTER );
            name = CGTempName( temp_name, T_LONG_POINTER );
            left = CGAssign( name, op1, T_LONG_POINTER );
            name = CGTempName( temp_name, T_LONG_POINTER );
            PushCGName( left );
            PushCGName( PushRValue( node, name ) );
          } break;
        case OPR_CONVERT:
            if( node->result_type->decl_type != TYPE_VOID ) {
                op1 = PopCGName();      // - get expression
                PushCGName( CGUnary( O_CONVERT, op1,
                                CGenType(node->result_type)) );
            }
            break;
        case OPR_CONVERT_PTR:           // convert pointer
            op1 = PopCGName();          // - get expression
            PushCGName( ConvertPointer( node, op1 ) );
            break;
        case OPR_MATHFUNC:              // intrinsic math func with 1 parm
            op1 = PopCGName();          // - get expression
            PushCGName( CGUnary( node->mathfunc, op1, TY_DOUBLE ) );
            break;
        case OPR_MATHFUNC2:             // intrinsic math func with 2 parms
            op2 = PopCGName();          // - get expression
            op1 = PopCGName();          // - get expression
            PushCGName( CGBinary( node->mathfunc, op1, op2, TY_DOUBLE ) );
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
            PushCGName( CGBinary( O_CONVERT, op2, op1, T_LONG_POINTER ) );
            break;
        case OPR_FUNCNAME:              // function name
            PushCGName( InitFuncCall( node ) ); // - push call_handle
            break;
        case OPR_CALL_INDIRECT:         // indirect function call
            op1 = PopCGName();                          // - get name
            PushCGName( InitIndFuncCall( node, op1 ) ); // - push call_handle
            break;
        case OPR_CALL:                  // function call
            call_list = PopCGName();    // - get call_handle
            op1 = CGCall( call_list );
            if( node->flags & OPFLAG_RVALUE ) {
                op1 = CGUnary( O_POINTS, op1, CGenType( node->result_type ) );
            }
            PushCGName( op1 );
            break;
        case OPR_PARM:                  // function parm
            op1 = PopCGName();          // - get parm
            call_list = PopCGName();    // - get call_handle
            CGAddParm( call_list, op1, CGenType( node->result_type ) );
            PushCGName( call_list );
            break;
        case OPR_LABEL:                 // label
        case OPR_CASE:                  // case label
            CGControl( O_LABEL, NULL, CGLabelHandles[ node->label_index ] );
            break;
        case OPR_JUMP:                  // jump
            CGControl( O_GOTO, NULL, CGLabelHandles[ node->label_index ] );
            break;
        case OPR_JUMPTRUE:              // jump if true
            op1 = PopCGName();
            CGControl( O_IF_TRUE, op1, CGLabelHandles[ node->label_index ] );
            break;
        case OPR_JUMPFALSE:             // jump if false
            op1 = PopCGName();
            CGControl( O_IF_FALSE, op1, CGLabelHandles[ node->label_index ] );
            break;
        case OPR_SWITCH:                // switch
            op1 = PopCGName();
            DoSwitch( node, op1 );
            break;
#ifdef __SEH__
        case OPR_TRY:                   // start of try block
            SetTryScope( node->parent_scope );
            break;
        case OPR_EXCEPT:
        case OPR_FINALLY:
            CGBigLabel( FEBack( node->sym_handle ) );
            break;
        case OPR_END_FINALLY:
            EndFinally();
            break;
        case OPR_UNWIND:
            TryUnwind( node->try_index );
            break;
        case OPR_EXCEPT_CODE:
            op1 = TryExceptionInfoAddr();
            op1 = CGUnary( O_POINTS, op1, T_POINTER );
            PushCGName( CGUnary( O_POINTS, op1, T_INTEGER ) );
            break;
        case OPR_EXCEPT_INFO:
            PushCGName( TryExceptionInfoAddr() );
            break;
        case OPR_ABNORMAL_TERMINATION:
            PushCGName( TryAbnormalTermination() );
            break;
#endif
#if _MACHINE == _ALPHA  || _MACHINE == _PPC
        case OPR_VASTART:
            op2 = PopCGName();          // - get offset of parm
            op1 = PopCGName();          // - get address of va_list
            GenVaStart( op1, op2 );
            break;
        case OPR_ALLOCA:
            op1 = PopCGName();          // - get size
            PushCGName( CGUnary( O_STACK_ALLOC, op1, T_POINTER ) );
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
    if( index != 0 ) {
        CGDone( PopCGName() );
    }
}

void ThreadNode( TREEPTR node )
{
    if( FirstNode == NULL )  FirstNode = node;
    if( LastNode != NULL )   LastNode->thread = node;
    LastNode = node;
}

TREEPTR LinearizeTree( TREEPTR tree )
{
    FirstNode = NULL;
    LastNode = NULL;
    WalkExprTree( tree, ThreadNode, NoOp, NoOp, ThreadNode );
    LastNode->thread = NULL;
    return( FirstNode );
}


void EmitInit()
{
    SegListHead = NULL;
    SegImport = SegData-1;
    Refno = T_FIRST_FREE;
}


local int NewRefno()
{
    return( Refno++ );
}


void EmitAbort()
{
}

TREEPTR GenOptimizedCode( TREEPTR tree )
{
    unsigned    unroll_count;

    unroll_count = 0;
    while( tree != NULL ) {
        SrcFno = tree->op.source_fno;
        SrcLineNum = tree->srclinenum;
        if( SrcLineNum != SrcLineCount ) {
            if( Saved_CurFunc == 0 ) {      /* 24-nov-91 */
                DBSrcCue( SrcFno, SrcLineNum, 1 );
            }
        }
        SrcLineCount = SrcLineNum;      /* for error msgs 14-jul-89 */
        if( tree->op.unroll_count != unroll_count ) {
            unroll_count = tree->op.unroll_count;
            BEUnrollCount( unroll_count );
        }
        if( tree->right->op.opr == OPR_FUNCTION ) {     // if start of func
            TREEPTR right;
            SYM_ENTRY sym;

             right = tree->right;
             SymGet( &sym,  right->op.func.sym_handle );
             if( ! (sym.flags & SYM_REFERENCED) ) {
                 if( (sym.attrib & FLAG_INLINE)
                  && (sym.stg_class != SC_NULL) ){
                    while( tree->right->op.opr != OPR_FUNCEND ){
                        tree = tree->left;
                    }
                    break;
                 }
            }
        }
        EmitNodes( LinearizeTree( tree->right ) );
#ifdef __SEH__
        if( tree->right->op.opr == OPR_FUNCTION ) {     // if start of func
           if( FuncNodePtr->func.flags & FUNC_USES_SEH ) {
                GenerateTryBlock( tree->left );
           }
        }
#endif
        if( tree->right->op.opr == OPR_FUNCEND ) break;
        tree = tree->left;
    }
    return( tree );
}

static void DoInLineFunction( TREEPTR tree )
{   // Push some state info and use InLineDepth for turning some stuff off
    struct func_save  save;
    ++InLineDepth;
    save.func  = CurFunc;
    save.func_handle = CurFuncHandle;
    save.funcnode    = FuncNodePtr;
    save.cglabel_handles = CGLabelHandles;
    save.labelindex = LabelIndex;
    CurFuncHandle = 0;
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

TREEPTR FindFuncStmtTree( SYM_HANDLE sym_handle )
{
    TREEPTR     tree;
    TREEPTR     right;
    tree = FirstStmt;
    while( tree != NULL ) {
        right = tree->right;
        if( right->op.opr == OPR_FUNCTION ) {     // if start of func
            if( right->op.func.sym_handle == sym_handle ){
                break;
            }
        }
        tree = tree->left;
    }
    return( tree );
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

    ret = FALSE;
    if( InLineDepth < MAX_INLINE_DEPTH ){
        tree = FindFuncStmtTree(sym_handle);
        if( tree != NULL ) {
           right = tree->right;
           if( !(right->op.func.flags & FUNC_INUSE) ){
               ret =  right->op.func.flags & FUNC_OK_TO_INLINE;
           }
        }
    }
    return( ret );
}

void GenModuleCode()
{
    TREEPTR     tree;

    tree = FirstStmt;
    InLineDepth = 0;
//  InLineFuncStack = NULL;
    while( tree != NULL ) {
        tree = GenOptimizedCode( tree );
        tree = tree->left;
    }
}

static void NoCodeGenDLL()
{
    extern void FEMessage( msg_class, void * );
    FEMessage( MSG_FATAL, "Unable to load code generator DLL" );
}

void DoCompile()
{
    unsigned int /*jmp_buf*/ *old_env;
    auto jmp_buf env;
    auto cg_init_info cgi_info;

    old_env = Environment;
    if( ! setjmp( env ) ) {
        Environment = &env;
        CStringList = 0;
        if( BEDLLLoad( NULL ) ) {
#if _MACHINE == _PC
            BEMemInit(); // cg has a strange static var that doesn't get reset
#endif
            if( ! CompFlags.zu_switch_used )  TargetSwitches &= ~ FLOATING_SS;
            if( Toggles & TOGGLE_DUMP_CG ){
                GenSwitches |= ECHO_API_CALLS;
            }
#ifdef POSITION_INDEPENDANT
            if( CompFlags.rent ){
                GenSwitches |= POSITION_INDEPENDANT;
            }
#endif
            cgi_info = BEInit( GenSwitches, TargetSwitches, OptSize, ProcRevision );
            if( cgi_info.success ) {
#if 0
                if( cgi_info.version.revision != II_REVISION ) WrongCodeGen();
#if _CPU == 386
                if( cgi_info.version.target != II_TARG_80386 ) WrongCodeGen();
#elif _CPU == 370
                if( cgi_info.version.target != II_TARG_370 ) WrongCodeGen();
#elif _CPU == 8086
                if( cgi_info.version.target != II_TARG_8086 ) WrongCodeGen();
#elif _CPU == 0000
                if( cgi_info.version.target != II_TARG_AXP ) WrongCodeGen();
#else
#error "Undefined _CPU type"
#endif
#endif
#if _CPU == 386
                if( TargetSwitches & (P5_PROFILING|NEW_P5_PROFILING) ) {
                    FunctionProfileSegment = AddSegName( "TI", "DATA", SEGTYPE_INITFINI );
                }
#endif
                SetSegs();
                BEStart();
                EmitSegLabels();                        /* 15-mar-92 */
                if( GenSwitches & DBG_TYPES ) EmitDBType();
                EmitSyms();
                EmitCS_Strings();
                SrcLineCount = 0;
                FListSrcQue();
                EmitDataQuads();
                FreeDataQuads();
                #ifdef __SEH__
                    TryRefno = NewRefno();
                    BEDefType( TryRefno, 1, sizeof( struct try_block ) );
                    TryTableBackHandles = NULL;
                #endif
                GenModuleCode();
                FreeStrings();
                FreeCS_Strings();
                FiniSegLabels();                        /* 15-mar-92 */
                if( ErrCount != 0 ) {
                    BEAbort();
                }
                BEStop();
                FiniSegBacks();                /* 15-mar-92 */
                FreeGblVars( GlobalSym );
                FreeGblVars( SpecialSyms );     /* 05-dec-89 */
                FreeExtVars();                          /* 02-apr-92 */
                #ifdef __SEH__
                    FreeTryTableBackHandles();
                #endif
                BEFini();
                BEDLLUnload();
            }
        } else {
            NoCodeGenDLL();
        }
    }
    Environment = old_env;
}


void EmitSyms()
{
    SYM_HANDLE          sym_handle;
    auto SYM_ENTRY      sym;

    for( sym_handle = GlobalSym; sym_handle; ) {
        SymGet( &sym, sym_handle );
        EmitSym( &sym, sym_handle );
        if( ( GenSwitches & DBG_LOCALS ) &&
            ( sym.sym_type->decl_type != TYPE_FUNCTION ) &&
            ( (sym.flags & SYM_TEMP) == 0 )  && /* 06-oct-93 */
            ( sym.stg_class != SC_TYPEDEF )) {
#if _CPU == 370
                if( sym.stg_class != SC_EXTERN || sym.flags & SYM_REFERENCED){
                    DBModSym( sym_handle, TY_DEFAULT );
                }
#else
                DBModSym( sym_handle, TY_DEFAULT );
#endif
        }
        sym_handle = sym.handle;
    }
}


void EmitSym( SYMPTR sym, SYM_HANDLE sym_handle )
{
    TYPEPTR             typ;
    int                 segment;
    auto unsigned long  size;

    typ = sym->sym_type;
    if( (GenSwitches & DBG_TYPES) && (sym->stg_class == SC_TYPEDEF) ) {
        if( typ->decl_type != TYPE_TYPEDEF ) {
            DBEndName( DBBegName( sym->name, DBG_NIL_TYPE ),
                                  DBType( typ ) );
        }
    }
    while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
    CGenType( typ );    /* create refno for ARRAY type, etc */
    if( sym->stg_class != SC_EXTERN     &&  /* if not imported */
        sym->stg_class != SC_TYPEDEF ) {
        if( ( sym->flags & SYM_FUNCTION ) == 0 ) {
            segment = sym->u.var.segment;
            if( (sym->flags & SYM_INITIALIZED) == 0 || segment == SEG_BSS){
                BESetSeg( segment );
                AlignIt( typ );
                DGLabel( FEBack( sym_handle ) );
                /* initialize all bytes to 0 */
                /* if size > 64k, have to break it into chunks of 64k */
                size = SizeOfArg( typ );
                if( segment == SEG_BSS ) {
                    DGUBytes( size );
                } else {
#if _CPU == 8086
                    while( size >= 0x10000 ) {
                        EmitZeros( 0x10000 );
                        size -= 0x10000;
                        if( size == 0 ) break;
                        if( segment != SEG_CONST && segment != SEG_DATA ) {
                            ++segment;
                            BESetSeg( segment );
                        }
                    }
#endif
                    if( size != 0 ) EmitZeros( size );
                }
            }
        }
    }
}

local int DoFuncDefn( SYM_HANDLE funcsym_handle )
{
    int         parms_reversed;
    SYM_HANDLE  sym_handle;
    int         ret_type;

    SSVar = NULL;
    CurFunc = &CurFuncSym;
    SymGet( CurFunc, funcsym_handle );
    CurFuncHandle = funcsym_handle;
#if _MACHINE == _PC
    if( ! CompFlags.zu_switch_used ) {
        if( (CurFunc->attrib & FLAG_INTERRUPT) == FLAG_INTERRUPT ){
            /* interrupt function */
            TargetSwitches |= FLOATING_SS;      /* force -zu switch on */
        } else {
            TargetSwitches &= ~ FLOATING_SS;    /* turn it back off */
        }
    }
#endif
    ret_type = CGenType( CurFunc->sym_type->object );
    if( CompFlags.returns_promoted ) {
        ret_type = FEParmType( NULL, NULL, ret_type );
    }
    CGProcDecl( funcsym_handle, ret_type );
#if _CPU == 386
    if( TargetSwitches & P5_PROFILING ) {
        char *fn_name = FEName( funcsym_handle );
        size_t len = strlen( fn_name )+1;
        segment_id old_segment;

        old_segment = BESetSeg( FunctionProfileSegment );
        FunctionProfileBlock = BENewBack( NULL );
        DGLabel( FunctionProfileBlock );
        DGInteger( 0,   T_INTEGER );
        DGInteger( -1,  T_INTEGER );
        DGInteger( 0,   T_INTEGER );
        DGInteger( 0,   T_INTEGER );
        DGBytes( len, fn_name );
        len &= 0x03;
        if( len ) {
            DGIBytes( 4 - len, 0 );
        }
        BESetSeg( old_segment );
    }
#endif
    if( GenSwitches & DBG_LOCALS ) {
        if( InLineDepth == 0 ){
            DBModSym( CurFuncHandle, TY_DEFAULT );
        }else{
            DBBegBlock();
        }
    }
    parms_reversed = 0;
    sym_handle = CurFunc->u.func.parms;
    if( sym_handle ) {
        GetCallClass( CurFuncHandle );
        if( CallClass & REVERSE_PARMS ) {                       /* 22-jan-90 */
            ParmReverse( sym_handle );
            parms_reversed = 1;
        } else {
            for( ; sym_handle; ) {
                SYMPTR      sym;

                sym = SymGetPtr( sym_handle );
                if( sym->sym_type->decl_type == TYPE_DOT_DOT_DOT ) break;
                DoParmDecl( sym, sym_handle );
                sym_handle = sym->handle;
            }
        }
    }
    CGLastParm();
    DoAutoDecl( CurFunc->u.func.locals );
#ifdef __SEH__
    if( FuncNodePtr->func.flags & FUNC_USES_SEH ) {
        CGAutoDecl( TrySymHandle, TryRefno );
        CallTryInit();                  // generate call to __TryInit
    }
#endif
    return( parms_reversed );
}

local void DoParmDecl( SYMPTR sym, SYM_HANDLE sym_handle )
{
    TYPEPTR typ;
    int     dtype;

    typ = sym->sym_type;
    while( typ->decl_type == TYPE_TYPEDEF )  typ = typ->object;
    dtype = CGenType( typ );
    CGParmDecl( sym_handle, dtype );
    if( (GenSwitches & NO_OPTIMIZATION)     /* 20-sep-88 */
#if _CPU == 386
        || ((! CompFlags.register_conventions) &&
        CompFlags.debug_info_some)
#endif
    ) {
        if( GenSwitches & DBG_LOCALS ) {
            DBLocalSym( sym_handle, TY_DEFAULT );
        }
    }
}
local void ParmReverse( SYM_HANDLE sym_handle ) /* 22-jan-90 */
{
    SYMPTR      sym;

    sym = SymGetPtr( sym_handle );
    if( sym->handle ) {
        ParmReverse( sym->handle );
        sym = SymGetPtr( sym_handle );
    }
    DoParmDecl( sym, sym_handle );
}

local void DoAutoDecl( SYM_HANDLE sym_handle )
{
    TYPEPTR             typ;
    cg_type             dtype;
    char                emit_debug_info;                /* 01-mar-91 */
    char                emit_extra_info;                /* 25-nov-91 */
    auto SYM_ENTRY      sym;

    while( sym_handle != 0 ) {
        SymGet( &sym, sym_handle );
        emit_debug_info = 0;
        emit_extra_info = 0;
        if( (GenSwitches & NO_OPTIMIZATION) )  emit_debug_info = 1;
        if( sym.stg_class == SC_STATIC ) {
            emit_debug_info = 0;
            if( (sym.flags & SYM_EMITTED) == 0 ) {
                if( sym.sym_type->decl_type != TYPE_VOID ) {
                    EmitSym( &sym, sym_handle );
                    emit_debug_info = 1;
                    SymGet( &sym, sym_handle );
                    sym.flags |= SYM_EMITTED;
                    SymReplace( &sym, sym_handle );
                }
            }
        } else if( sym.stg_class != SC_EXTERN &&
                   sym.stg_class != SC_TYPEDEF ) {      /* 25-nov-94 */
            if( sym.flags & SYM_ADDR_TAKEN ) {
                emit_extra_info = 1;
            }
            typ = sym.sym_type;
            while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
            switch( typ->decl_type ) {
            case TYPE_UNION:
            case TYPE_STRUCT:
            case TYPE_ARRAY:
                emit_extra_info = 1;
                break;
            }
            dtype = CGenType( typ );
            if( sym.flags & SYM_FUNC_RETURN_VAR ) {
                sym.info.return_var = CGTemp( dtype );
                SymReplace( &sym, sym_handle );
            } else {
                CGAutoDecl( sym_handle, dtype );
            }
        }
#if _CPU != 370
        if( ! CompFlags.debug_info_some ) emit_extra_info = 0;
#endif
        if( emit_debug_info != 0 || emit_extra_info != 0 ) {
            if( Saved_CurFunc == 0 ) {  /* if we are not inlining */
                if( GenSwitches & DBG_LOCALS ) {
                    if( !(sym.flags & SYM_TEMP) ) {
                        DBLocalSym( sym_handle, TY_DEFAULT );
                    }
                }
            }
        }
        sym_handle = sym.handle;
    }
}


void FreeSymBackInfo( SYM_ENTRY *sym, SYM_HANDLE sym_handle )
{
    if( sym->info.backinfo != NULL ) {
        BEFiniBack( sym->info.backinfo );
        BEFreeBack( sym->info.backinfo );
        sym->info.backinfo = NULL;
        SymReplace( sym, sym_handle );
    }
}

#ifdef __SEH__
void FreeTrySymBackInfo()
{
    SYM_ENTRY   sym;

    SymGet( &sym, TrySymHandle );
    FreeSymBackInfo( &sym, TrySymHandle );
}

void FreeTryTableBackHandles()
{
    struct try_table_back_handles       *try_backinfo;

    for(;;) {
        try_backinfo = TryTableBackHandles;
        if( try_backinfo == NULL )  break;
        TryTableBackHandles = try_backinfo->next;
        BEFreeBack( try_backinfo->try_table_back_handle );
        CMemFree( try_backinfo );
    }
}
#endif

void FreeLocalVars( SYM_HANDLE sym_list )
{
    SYM_HANDLE          sym_handle;
    auto SYM_ENTRY      sym;

    for( ; sym_handle = sym_list; ) {
        SymGet( &sym, sym_handle );
        sym_list = sym.handle;
        if( sym.stg_class != SC_EXTERN ) {
            if( ! (sym.flags & SYM_FUNC_RETURN_VAR) ) {
                if( sym.sym_type->decl_type != TYPE_VOID ) {
                    FreeSymBackInfo( &sym, sym_handle );
                }
            }
        }
    }
}


local void FreeGblVars( SYM_HANDLE sym_handle )
{
    SYMPTR      sym;

    for( ; sym_handle; ) {
        sym = SymGetPtr( sym_handle );
        sym_handle = sym->handle;
        if( sym->info.backinfo != NULL ) {
//              BEFiniBack( sym->info.backinfo );
            BEFreeBack( sym->info.backinfo );
        }
    }
}

local void RelExtVars( SYM_HANDLE sym_handle )
{
    SYMPTR      sym;

    for( ; sym_handle; ) {
        sym = SymGetPtr( sym_handle );
        sym_handle = sym->handle;
        if( !(sym->flags & SYM_FUNC_RETURN_VAR) ) {
            if( sym->stg_class == SC_EXTERN || sym->stg_class == SC_STATIC ||
                sym->sym_type->decl_type == TYPE_VOID ) {
                if( sym->info.backinfo != NULL ) {
                    BEFreeBack( sym->info.backinfo );
                }
            }
        }
    }
}

local void FreeExtVars()                                /* 02-apr-92 */
{
    SYM_LISTS   *sym_list;
    SYM_LISTS   *next_sym;

    for( sym_list = SymListHeads; sym_list; ) {
        RelExtVars( sym_list->sym_head );
        next_sym = sym_list->next;
        CMemFree( sym_list );
        sym_list = next_sym;
    }
}

int CGenType( TYPEPTR typ )
{
    int         dtype;
    int         flags;
    int         align;

    while( typ->decl_type == TYPE_TYPEDEF )  typ = typ->object;
    switch( typ->decl_type ) {
    case TYPE_STRUCT:
    case TYPE_UNION:
        if( typ->object != NULL ) { /* 15-jun-94 */
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
    case TYPE_ARRAY:
        if( typ->u.array->refno == 0 ) {
            dtype = NewRefno();
            align = GetTypeAlignment( typ );
            BEDefType( dtype, align, SizeOfArg( typ ) );
            typ->u.array->refno = dtype;
        }
        dtype = typ->u.array->refno;
        break;
    case TYPE_FIELD:
    case TYPE_UFIELD:
        dtype = CGDataType[ typ->u.f.field_type ];
        break;
    case TYPE_FUNCTION:
        dtype = CodePtrType( FLAG_NONE ); /* 20-nov-87 */
        break;
    case TYPE_POINTER:
        flags = typ->u.p.decl_flags;
        #if _MACHINE == _PC
            if( typ->u.p.segment == SEG_STACK ) {   /* 05-oct-88 */
                if( TargetSwitches & FLOATING_SS ) {
                    flags |= FLAG_FAR;
                }
            }
        #endif
        dtype = PtrType( typ->object, flags );
        break;
    case TYPE_ENUM:
        typ = typ->object;
    default:
        dtype = CGDataType[ typ->decl_type ];
    }
    return( dtype );
}


local int CodePtrType( int flags )
{
#if _MACHINE == _PC
    int         dtype;

    if( flags & FLAG_FAR ) {
        dtype = T_LONG_CODE_PTR;
    } else if( flags & FLAG_NEAR ) {
        dtype = T_NEAR_CODE_PTR;
    } else {
        dtype = T_CODE_PTR;
    }
    return( dtype );
#else
    return( T_CODE_PTR );
#endif
}


extern int PtrType( TYPEPTR typ, int flags )
{
    int         dtype;

    while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;/*03-dec-91*/
    if( typ->decl_type == TYPE_FUNCTION ) {
        dtype = CodePtrType( flags );
    } else {
#if _MACHINE == _PC
        if( flags & FLAG_FAR ) {
            dtype = T_LONG_POINTER;
        } else if( flags & FLAG_HUGE ) {
            dtype = T_HUGE_POINTER;
        } else if( flags & FLAG_NEAR ) {
            dtype = T_NEAR_POINTER;
        } else {
            dtype = T_POINTER;
        }
#else
        dtype = T_POINTER;
#endif
    }
    return( dtype );
}


int StringSegment( STR_HANDLE strlit )
{
#if _MACHINE == _PC
    if( strlit->flags & FLAG_FAR )   return( FarStringSegment );
#endif
    if( strlit->flags & FLAG_CONST ) return( SEG_CODE );/* 01-sep-89*/
    return( SEG_CONST );
}

void EmitStrPtr( STR_HANDLE str_handle, int pointer_type )
{
    str_handle->ref_count++;
    Emit1String( str_handle );
    DGBackPtr( str_handle->cg_back_handle, StringSegment( str_handle ),
                    0, pointer_type );
}


void Emit1String( STR_HANDLE str_handle )
{
    if( str_handle->cg_back_handle == 0 ) {
        str_handle->cg_back_handle = BENewBack( NULL );
        if( ! (str_handle->flags & FLAG_CONST) ) {
            EmitLiteral( str_handle );
        }
    }
}


int EmitBytes( STR_HANDLE strlit )
{
    DGBytes( strlit->length, &strlit->literal[0] );
    return( strlit->length );
}


local void EmitLiteral( STR_HANDLE strlit )
{
    segment_id  old_segment;

    old_segment = BESetSeg( StringSegment( strlit ) );
    if( OptSize == 0 ) {                /* if optimize for time */
        DGAlign( TARGET_INT );          /* align to word boundary */
    }
    DGLabel( strlit->cg_back_handle );
    EmitBytes( strlit );
    BESetSeg( old_segment );
}


void FreeStrings()
{
    STR_HANDLE  strlit;
    int         i;

    for( i = 0; i < STRING_HASH_SIZE; ++i ) {
        for( strlit = StringHash[i]; strlit; strlit = strlit->next_string ) {
            if( strlit->cg_back_handle != 0 ) {
                BEFiniBack( strlit->cg_back_handle );
                BEFreeBack( strlit->cg_back_handle );
                strlit->cg_back_handle = 0;
            }
        }
        StringHash[i] = 0;
    }
}


local void DumpCS_Strings( STR_HANDLE strlit )
{
    while( strlit != NULL ) {
        if( strlit->flags & FLAG_CONST  &&
            strlit->ref_count != 0 ) {          /* 17-aug-91 */
            strlit->cg_back_handle = BENewBack( NULL );
            EmitLiteral( strlit );
        }
        strlit = strlit->next_string;
    }
}


local void EmitCS_Strings()
{
    int         i;

    if( CompFlags.strings_in_code_segment ) {
        DumpCS_Strings( CS_StringList );
        for( i = STRING_HASH_SIZE - 1; i >= 0; --i ) {
            DumpCS_Strings( StringHash[i] );
        }
    }
}

local void FreeCS_Strings()
{
    STR_HANDLE  strlit;

    if( CompFlags.strings_in_code_segment ) {
        for( strlit = CS_StringList; strlit; strlit = strlit->next_string ) {
            if( strlit->cg_back_handle != 0 ) {
                BEFiniBack( strlit->cg_back_handle );
                BEFreeBack( strlit->cg_back_handle );
                strlit->cg_back_handle = 0;
            }
        }
    }
}

#ifdef __SEH__
void GenerateTryBlock( TREEPTR tree )
{
    TREEPTR     stmt;
    int         try_index;
    int         max_try_index;

    try_index = 0;
    max_try_index = -1;
    for(;;) {
        stmt = tree->right;
        if( stmt->op.opr == OPR_FUNCEND ) break;
        switch( stmt->op.opr ) {
        case OPR_TRY:
            try_index = stmt->op.try_index;
            if( try_index > max_try_index )  max_try_index = try_index;
            break;
        case OPR_EXCEPT:
        case OPR_FINALLY:
            ValueStack[ try_index ] = (TREEPTR)stmt->op.try_sym_handle;
            Class[ try_index ] = stmt->op.parent_scope;
            Token[ try_index ] = stmt->op.opr;
            break;
        }
        tree = tree->left;
        if( tree == NULL ) break;               // should never happen
    }
    if( max_try_index != -1 ) {
        segment_id      old_segment;
        back_handle     except_label;
        back_handle     except_table;
        struct try_table_back_handles *try_backinfo;

        old_segment = BESetSeg( SEG_DATA );
        except_table = BENewBack( NULL );
        try_backinfo = (struct try_table_back_handles *)
                        CMemAlloc( sizeof( struct try_table_back_handles ) );
        try_backinfo->try_table_back_handle = except_table;
        try_backinfo->next = TryTableBackHandles;
        TryTableBackHandles = try_backinfo;
        DGLabel( except_table );
        for( try_index = 0; try_index <= max_try_index; try_index++ ) {
            DGInteger( Class[ try_index ], T_UINT_1 );  // parent index
            if( Token[ try_index ] == OPR_EXCEPT ) {
                DGInteger( 0, T_UINT_1 );
            } else {
                DGInteger( 1, T_UINT_1 );
            }
            except_label = FEBack( (SYM_HANDLE)ValueStack[ try_index ] );
            DGBackPtr( except_label, FESegID( CurFuncHandle ), 0,
                            T_CODE_PTR );
        }
        BESetSeg( old_segment );
        SetTryTable( except_table );
        BEFiniBack( except_table );
    }
}
#endif
