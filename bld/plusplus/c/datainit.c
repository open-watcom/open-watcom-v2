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


#include "plusplus.h"

#include <limits.h>

#include "fnbody.h"
#include "cgfront.h"
#include "cgsegid.h"
#include "stringl.h"
#include "ptree.h"
#include "memmgr.h"
#include "ring.h"
#include "stack.h"
#include "carve.h"
#include "toggle.h"
#include "decl.h"
#include "dbg.h"
#include "datainit.h"
#include "rtfuns.h"
#include "label.h"
#include "typesig.h"
#include "datadtor.h"
#include "initdefs.h"
#ifndef NDEBUG
#include "pragdefn.h"
#endif

#define BLOCK_NEST      16
#define BLOCK_QUEUE     16
#define OPEN_BRACE      "{"
#define CLOSE_BRACE     "}"

static carve_t carveQUEUE;
static carve_t carveNEST;
static INITIALIZE_DATA *currInit;
static unsigned runTimeThreshold = 1;
static unsigned bitMask[] = {       // for bitfields
    0x00000001, 0x00000003, 0x00000007, 0x0000000F,
    0x0000001F, 0x0000003F, 0x0000007F, 0x000000FF,
    0x000001FF, 0x000003FF, 0x000007FF, 0x00000FFF,
    0x00001FFF, 0x00003FFF, 0x00007FFF, 0x0000FFFF,
    0x0001FFFF, 0x0003FFFF, 0x0007FFFF, 0x000FFFFF,
    0x001FFFFF, 0x003FFFFF, 0x007FFFFF, 0x00FFFFFF,
    0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF, 0x0FFFFFFF,
    0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF
};


#ifdef NDEBUG
#define _fatal( x )
#define _dump( x )
#define _dumpPTree( x )
#define _dumpSymbol( x )
#define _dumpFullType( x )
#define _dumpInt( x, y )
#define _dumpPtr( x, y )
#define _dumpInitInfo( x )
#define _dumpDtorPtr( x, y )
#define _dumpDtorInt( x, y )
#define _dumpDtor( x )
#define _dumpDtorSymbol( x )
#else
#define __DUMP_INIT ( PragDbgToggle.dump_init )
#define __DUMP_DTOR ( __DUMP_INIT || ( PragDbgToggle.dump_data_dtor ) )
#define _fatal( x )             CFatal( x )
#define _dump( x )              if( __DUMP_INIT ) puts( x )
#define _dumpPTree( x )         if( __DUMP_INIT ) DumpPTree( x )
#define _dumpSymbol( x )        if( __DUMP_INIT ) DumpSymbol( x )
#define _dumpFullType( x )      if( __DUMP_INIT ) DumpFullType( x )
#define _dumpInt( x, y )        if( __DUMP_INIT ) printf( x, y )
#define _dumpInitInfo( x )      if( __DUMP_INIT ) DumpInitInfo( (void *) x )
#define _dumpDtorPtr( x, y )    if( __DUMP_DTOR ) printf( x, y )
#define _dumpDtorInt( x, y )    if( __DUMP_DTOR ) printf( x, y )
#define _dumpDtor( x )          if( __DUMP_DTOR ) puts( x )
#define _dumpDtorSymbol( x )    if( __DUMP_DTOR ) DumpSymbol( x )
#endif


#if 0

static void *dataInitCodeFileOpen( bool modified, SCOPE *save )
/*************************************************************/
{
    void    *handle;

    *save = NULL;
    switch( currInit->location ) {
    case DL_INTERNAL_STATIC:
        handle = NULL;
        if( modified && currInit->dtor_reqd ) {
            CgFrontModInitUsed();
        }
        break;
    case DL_EXTERNAL_STATIC:
    case DL_EXTERNAL_PUBLIC:
        handle = CgFrontModInitOpen( save );
        if( modified ) {
            CgFrontModInitUsed();
        }
        break;
    }
    return( handle );
}


static void dataInitCodeFileClose( void *handle, SCOPE save_scope )
/*****************************************************************/
{
    switch( currInit->location ) {
    case DL_EXTERNAL_STATIC:
    case DL_EXTERNAL_PUBLIC:
        CgFrontModInitClose( save_scope, handle );
        break;
    }
}

#else

// data for code file processing

static SCOPE code_file_save_scope;      // saved scope
static void* code_file_save_handle;     // handle for saved file
static int code_file_save_depth;        // depth of open's
#define CODE_FILE_NOT_OPEN -1           // indicates code file not opened
static int code_file_open_depth;        // depth of actual open (-1 ==> none)


static void dataInitCodeFileOpen( bool modified )
/***********************************************/
{
    switch( currInit->location ) {
#if 0
//JWW -- I think this is hysterically here from the time when we used
//       the bit implementation of destruction
    case DL_INTERNAL_STATIC:
        if( modified && currInit->dtor_reqd ) {
            CgFrontModInitUsed();
        }
        break;
#endif
    case DL_EXTERNAL_STATIC:
    case DL_EXTERNAL_PUBLIC:
        if( CODE_FILE_NOT_OPEN == code_file_open_depth ) {
            code_file_save_handle
                = CgFrontModInitOpen( &code_file_save_scope );
            code_file_open_depth = code_file_save_depth;
        }
        if( modified ) {
            CgFrontModInitUsed();
        }
        break;
    }
    ++code_file_save_depth;
}


static void dataInitCodeFileClose( void )
/***************************************/
{
    DbgVerify( 0 != code_file_save_depth, "dataInitCodeFileClose -- bad depth" );
    --code_file_save_depth;
    if( code_file_save_depth == code_file_open_depth ) {
        code_file_open_depth = CODE_FILE_NOT_OPEN;
        CgFrontModInitClose( code_file_save_scope, code_file_save_handle );
    }
}

#endif


static bool isDataInitConstant( PTREE tree
                                 , PTREE *ctree
                                 , target_size_t *offset )
/********************************************************/
// return TRUE when it is a constant
{
    PTREE       right_tree;

    // skip CO_EXPR_DONE
    tree = PTreeOpLeft( tree );
    if( tree->cgop != CO_INIT ) {
        return( FALSE );
    }
    right_tree = tree->u.subtree[1];

    // handle "int a = ( b=1, 2 );"
    if( right_tree->flags & PTF_SIDE_EFF ) {
        return( FALSE );
    }
    return( IsLinkerConstant( right_tree, ctree, offset ) );
}


static SYMBOL dtorableObjectInitSymbol( // SYMBOL FOR CURRENT DTORABLE OBJECT
    void )
{
    return currInit->auto_static ? currInit->auto_sym : currInit->sym;
}

static PTREE makeSafeSymbol( SYMBOL sym, TOKEN_LOCN *locn, SEARCH_RESULT *result )
{
    PTREE tree;

    tree = PTreeIdSym( sym );
    tree = PTreeSetLocn( tree, locn );
    tree = NodeSymbol( tree, sym, result );
    return( tree );
}


static PTREE dataInitLeftSide( INITIALIZE_INFO *entry, TOKEN_LOCN* locn )
/***********************************************************************/
// recursive function to build left side of init expression
{
    SYMBOL sym;
    PTREE tree;
    PTREE subtree;

    tree = NULL;
    if( entry->previous != NULL ) {
        tree = dataInitLeftSide( entry->previous, locn );
    }
    if( entry->entry == DE_ROOT_TYPE ) {
        tree = makeSafeSymbol( dtorableObjectInitSymbol(), locn, NULL );
    } else if( currInit->nest != entry ) {
        switch( entry->target ) {
        case DT_ARRAY:
            subtree = PTreeIntConstant( entry->u.a.index, TYP_UINT );
            subtree = PTreeSetLocn( subtree, locn );
            tree = PTreeBinary( CO_INDEX, tree, subtree );
            tree = PTreeSetLocn( tree, locn );
            break;
        case DT_CLASS:
            sym = entry->u.c.curr;
            subtree = makeSafeSymbol( sym, locn, ScopeFindSymbol( sym ) );
            tree = PTreeBinary( CO_DOT, tree, subtree );
            tree = PTreeSetLocn( tree, locn );
            break;
        case DT_BITFIELD:
        case DT_SCALAR:
        case DT_ERROR:
            break;
        }
    }
    return( tree );
}

static PTREE dataInitPadLeftSide( target_size_t start )
/*****************************************************/
{
    TYPE  type;
    PTREE tree;

    tree = MakeNodeSymbol( dtorableObjectInitSymbol() );
    type = tree->type;
    tree = PTreeBinary( CO_DOT, tree, PTreeIntConstant( start, TYP_UINT ) );
    type = TypeMergeForMember( type, currInit->base_type );
    tree->type = type;
    tree->flags |= PTF_LVALUE;
    tree = NodeUnary( CO_INDIRECT, tree );
    tree->type = type;
    tree->flags |= PTF_LVALUE;
    return( tree );
}

#if 0
static PTREE refOfSym( TYPE type, SYMBOL var )
/********************************************/
{
    return NodeConvert( MakeReferenceTo( type ), MakeNodeSymbol( var ) );
}
#endif

static void dataInitEmitAutoAssign( SYMBOL dst, SYMBOL src )
/**********************************************************/
{
#if 0
    TYPE            type;
    PTREE           node;
    target_size_t   size;
#endif

    currInit->emit_code = 1;
#if 0
    size = currInit->nest->padded_size;
    type = MakeArrayType( size );
    if( size == 0 ) {
        currInit->auto_type = type;
    }
#endif
    CgFrontCodePtr( IC_PARM_SYM, src );
    CgFrontCodePtr( IC_INIT_AUTO, dst );
#if 0
    type = MakeTypeOf( type, GetBasicType( TYP_UCHAR ) );
    node = NodeFetch( refOfSym( type, src ) );
    node = NodeAssign( refOfSym( type, dst ), node );
    node = NodeDone( node );

    _dump( "- Auto Assign Expression ------------------------------" );
    _dumpPTree( node );
    _dump( "-------------------------------------------------------" );

    IcEmitExpr( node );
#endif
}

static void dataInitLabel( void )
/*******************************/
{
    if( !currInit->label_done ) {
        currInit->label_done = 1;
        DgSymbolLabel( currInit->sym );
    }
}

static void dataInitCheckHugeSegment( target_size_t position )
/************************************************************/
{
    // The following is meaningless when TARGET_UINT_MAX == UINT_MAX
    // and will cause division by zero
#if _CPU == 8086
    target_size_t check;

    check = position % (TARGET_UINT_MAX + 1);
    if( position > 0 && check == 0 ) {
        DgSegmentIncrement();
    }
#else
    position = position;
#endif
}

static bool dataInitCheckHugeAlign( TYPE type )
/*********************************************/
// returns TRUE if cannot align to segment
{
    target_size_t   total_size;
    target_size_t   base_size;
    TYPE            base_type;

    total_size = CgMemorySize( type );
    // The following is meaningless when TARGET_UINT_MAX == UINT_MAX
    // (but this code will only ever get called for i86 target)
#if _CPU == 8086
    if( total_size <= (TARGET_UINT_MAX + 1) ) {
        return( FALSE );
    }
#endif
    base_type = ArrayType( type );
    while( base_type != NULL ) {
        type = base_type->of;
        base_type = ArrayType( type );
    }
    base_size = CgMemorySize( type );
#if _CPU == 8086
    return( ( base_size % (TARGET_UINT_MAX + 1) ) != 0 );
#else
    return( base_size != 0 );
#endif
}

static TYPE dtorableObjectType( // GET TYPE OF DTORABLE OBJECT (AT ROOT) ELEMENT
    INITIALIZE_INFO* info )     // - info on current entry
{
    TYPE type;                  // - NULL or type of dtorable element

    if( info == NULL ) {
        type = NULL;
    } else if( info->entry == DE_ROOT_TYPE || info->entry == DE_TYPE ) {
        if( TypeRequiresDtoring( info->type ) ) {
            type = info->type;
        } else {
            type = NULL;
        }
    } else {
        type = NULL;
    }
    return type;
}

static void emitDtorInitSymbol( // EMIT DTOR MARKING FOR A SYMBOL
    void )
{
    SYMBOL sym;                 // - symbol to be initialized

    if( ! currInit->dtor_done ) {
        _dumpDtor( "emitDtorInitSymbol -- dtor for symbol\n" );
        sym = dtorableObjectInitSymbol();
        currInit->dtor_done = 1;
        _dumpDtorSymbol( sym );
        PtdInitSymEnd( NULL, sym );
        if( SymIsAutomatic( sym ) ) {
            IcEmitDtorAutoSym( sym );
        } else {
            IcEmitDtorStaticSym( sym );
        }
    }
}

static void dtorableObjectEnd(  // EMIT COMPLETION FOR DTORABLE OBJECT IF REQ'D
    INITIALIZE_INFO* info )     // - info on current entry
{
    TYPE type;                  // - NULL or type of dtorable element

    _dumpDtorPtr( "dtorableObjectEnd( %p )\n", info );
    type = dtorableObjectType( info );
    if( type != NULL ) {
        dataInitCodeFileOpen( TRUE );
        DataDtorObjPop( NULL );
        if( info->entry == DE_ROOT_TYPE ) {
            emitDtorInitSymbol();
        }
        dataInitCodeFileClose();
    }
}

static void dataInitPopStack( void )
/**********************************/
{
    INITIALIZE_INFO *nest;

    dtorableObjectEnd( currInit->nest );
    nest = StackPop( &currInit->nest );
    _dump( "Popped " );
    _dumpInitInfo( nest );
    CarveFree( carveNEST, nest );
}

static void dataInitAbandon( void )
/*********************************/
// called when initialization is being abandoned
{
    _dump( "dataInitAbandon" );
    if( currInit->bracketed ) {
        DgInitDone();
    }
    while( currInit->nest != NULL ) {
        dataInitPopStack();
    }
    if( currInit->once_only ) {
        StaticInitFuncEnd( currInit->once_only_label );
    }

    currInit->state = DS_ABANDONED;
    currInit = currInit->prev;
}

static SYMBOL dataInitScopeOrderedNext( SYMBOL stop, SYMBOL curr )
/****************************************************************/
{
    SYMBOL prev;

    prev = curr;
    for(;;) {
        curr = ScopeOrderedNext( stop, curr );
        if( curr == NULL ) break;
        if( SymIsNextInitializableMember( &prev, curr ) ) {
            break;
        }
    }
    return( curr );
}

static SEGID_CONTROL dataInitSegIdControl( void )
/***********************************************/
{
    SEGID_CONTROL control;

    control = SI_DEFAULT;
    if( currInit->all_zero ) {
        control |= SI_ALL_ZERO;
    }
    if( currInit->emit_code ) {
        control |= SI_NEEDS_CODE;
    }
    return( control);
}

static target_size_t dataInitFieldSize( INITIALIZE_INFO *entry )
/**************************************************************/
{
    type_flag       flags;
    target_size_t   size;
    target_offset_t curr_off;
    target_offset_t next_off;
    SYMBOL          curr;
    SYMBOL          next;

    curr = entry->u.c.curr;
    curr_off = curr->u.member_offset;
    next = curr;
    flags = StructType( entry->type )->flag;
    if( flags & TF1_UNION ) {
        for(;;) {
            DbgAssert( next != NULL );
            next = dataInitScopeOrderedNext( entry->u.c.stop, next );
            if( next == NULL ) break;
            next_off = next->u.member_offset;
            if( next_off == 0 ) {
                // member of the union so quit
                next = NULL;
                break;
            }
            if( next_off != curr_off ) {
                // member of an anonymous struct
                break;
            }
        }
    } else {
        for(;;) {
            DbgAssert( next != NULL );
            next = dataInitScopeOrderedNext( entry->u.c.stop, next );
            if( next == NULL ) break;
            next_off = next->u.member_offset;
            if( next_off != curr_off ) {
                // next field
                break;
            }
        }
    }
    if( next == NULL ) {
        size = entry->padded_size - curr_off;
    } else {
        size = next_off - curr_off;
    }
    return( size );
}

static void dataInitComputeTarget( INITIALIZE_INFO *top )
/*******************************************************/
{
    TYPE type;

    if( top->entry != DE_BRACE ) {
        type = TypedefModifierRemoveOnly( top->type );
        if( type->id == TYP_CLASS && type->u.c.info->corrupted ) {
            top->target = DT_ERROR;
            currInit->state = DS_ERROR;
        } else if( IsCgTypeAggregate( type, FALSE ) ) {
            if( type->id == TYP_CLASS ) {
                top->target = DT_CLASS;
                top->u.c.stop = ScopeOrderedStart( type->u.c.scope );
                top->u.c.curr = dataInitScopeOrderedNext( top->u.c.stop
                                                        , NULL );
            } else if( type->id == TYP_ARRAY ) {
                top->target = DT_ARRAY;
                top->u.a.index = 0;
            } else if( type->id == TYP_BITFIELD ) {
                top->target = DT_BITFIELD;
                top->u.b.type = type->of;
                top->u.b.mask = 0;
            }
        } else {
            top->target = DT_SCALAR;
            if( ( top->previous != NULL )
              &&( top->previous->target == DT_BITFIELD ) ) {
                top->u.s.bitf = TRUE;
            } else {
                top->u.s.bitf = FALSE;
            }
        }
    }
}

static TYPE_SIG *dataInitTypeSigFind( TYPE base_type, TYPE_SIG_ACCESS access )
/****************************************************************************/
{
    TYPE_SIG    *sig;          // - type signature
    bool        errors;        // - TRUE ==> errors occurred

    dataInitCodeFileOpen( FALSE );
    if( SymIsStaticMember( currInit->sym ) ) {
        ModuleInitResumeScoped( SymScope( currInit->sym ) );
    }
    sig = TypeSigFind( access
                     , base_type
                     , NULL
                     , &errors );
    TypeSigReferenced( sig );
    dataInitCodeFileClose();
    return( sig );
}

static TYPE arrayBaseStructType( // GET STRUCT TYPE OF ARRAY BASE TYPE
    TYPE type )                 // - array type
{
    type = ArrayBaseType( type );
    type = StructType( type );
    return type;
}

static void dtorableObjectBeg(  // EMIT START FOR DTORABLE OBJECT IF REQ'D
    INITIALIZE_INFO* info )     // - info on current entry
{
    TYPE type;                  // - NULL or type of dtorable element
    SYMBOL sym;                 // - symbol to be initialized

    _dumpDtorPtr( "dtorableObjectBeg( %p )\n", info );
    type = dtorableObjectType( info );
    if( type != NULL ) {
        dataInitCodeFileOpen( TRUE );
        sym = dtorableObjectInitSymbol();
#if 0
        if( info->entry == DE_ROOT_TYPE ) {
            CgFrontCode( IC_INIT_SYM_BEG );
        }
#endif
        dataInitTypeSigFind( type, TSA_DTOR );
        DataDtorObjPush( NULL, type, sym, info->base );
        dataInitCodeFileClose();
    }
}

static SYMBOL getCtorCalled(    // GET SCOPE-CALL TYPE FOR A CTOR
    PTREE expr )                // - expression
{
    SYMBOL ctor;                // - ctor called

    if( NodeIsBinaryOp( expr, CO_CALL_EXEC ) ) {
        expr = PTreeOpLeft( expr );
        DbgVerify( NodeIsUnaryOp( expr, CO_CALL_SETUP )
                 , "getCtorCallState -- not CO_CALL_SETUP" );
        expr = PTreeOpLeft( expr );
        DbgVerify( expr->op == PT_SYMBOL
                 , "getCtorCallState -- not PT_SYMBOL" );
        ctor = expr->u.symcg.symbol;
    } else {
        ctor = NULL;
    }
    return ctor;
}


static PTREE emitDtorInitExpr(  // EMIT DTOR MARKING FOR AN EXPRESSION
    PTREE expr )                // - initialization expression
{
    SYMBOL sym;                 // - symbol to be initialized
    PTREE done;                 // - NULL or CO_DONE node

    if( ! currInit->dtor_done ) {
        _dumpDtor( "emitDtorInitExpr -- dtor for symbol\n" );
        sym = dtorableObjectInitSymbol();
        currInit->dtor_done = 1;
        _dumpDtorSymbol( sym );
        if( NodeIsUnaryOp( expr, CO_EXPR_DONE ) ) {
            done = expr;
            expr = expr->u.subtree[0];
        } else {
            done = NULL;
        }
        if( currInit->nest->entry == DE_ROOT_TYPE ) {
            // the following code comes out ahead of the expression; it should
            // be made a side effect if we attempt to optimize statement scopes
            SYMBOL ctor = getCtorCalled( expr );
            if( TypeReallyDtorable( sym->sym_type ) ) {
                if( SymIsAutomatic( sym ) ) {
                    expr = PtdCtoredScopeType( expr, ctor, sym->sym_type );
                } else {
                    expr = PtdScopeCall( expr, ctor );
                }
            } else {
                expr = PtdScopeCall( expr, ctor );
            }
            // end of scope-call optimization
            expr = PtdInitSymEnd( expr, sym );
        }
        expr = NodeDtorExpr( expr, sym );
        if( done != NULL ) {
            done->u.subtree[0] = expr;
            expr = done;
        }
    }
    return expr;
}

static PTREE dtorableObjectCtored(// EMIT INDEX OF DTORABLE OBJECT, IF REQ'D
    INITIALIZE_INFO* curr,      // - info on current entry
    PTREE expr,                 // - expression to be emitted
    bool index_updated )        // - TRUE ==> index has been updated
{
    TYPE type;                  // - NULL or type of dtorable element
    INITIALIZE_INFO* info;      // - info on previous entry
    INITIALIZE_INFO* prev;      // - used in searching previous entries

    _dumpDtorPtr( "dtorableObjectCtored( %p )\n", curr );
    info = curr->previous;
    if( info == NULL ) {
        type = dtorableObjectType( curr );
        if( type != NULL ) {
            dataInitCodeFileOpen( TRUE );
            if( expr == NULL ) {
                emitDtorInitSymbol();
            } else {
                expr = emitDtorInitExpr( expr );
            }
            dataInitCodeFileClose();
        }
    } else {
        type = dtorableObjectType( info );
        if( type != NULL ) {
            if( info->target == DT_ARRAY ) {
                unsigned index;
                TYPE artype;
                TYPE eltype;
                eltype = arrayBaseStructType( info->type );
                index = info->u.a.index;
                for( prev = info->previous; prev != NULL; prev = prev->previous ) {
                    if( prev->target != DT_ARRAY ) break;
                    artype = ArrayType( prev->type );
                    if( eltype != arrayBaseStructType( artype ) ) break;
                    index += prev->u.a.index * artype->u.a.array_size;
                }
                if( index_updated ) {
                    -- index;
                }
                dataInitCodeFileOpen( TRUE );
                _dumpDtorInt( "dtorableObjectCtored -- index %x\n", index );
                expr = DataDtorCompArrEl( expr, index );
                dataInitCodeFileClose();
            } else {
                for( prev = curr; prev != NULL; prev = prev->previous ) {
                    if( prev->type != curr->type ) {
                        dataInitCodeFileOpen( TRUE );
                        _dumpDtorInt( "dtorableObjectCtored -- class offset %x\n", info->offset );
                        expr = DataDtorCompClass( expr, info->offset, DTC_COMP_MEMB );
                        dataInitCodeFileClose();
                        break;
                    }
                }
            }
        }
    }
    return expr;
}

static void dataInitPushStack( INITIALIZE_ENTRY entry, INITIALIZE_INFO *prev )
/****************************************************************************/
// allocate and push a stack item
// initialize the stack item based on the entry
{
    target_size_t size;
    INITIALIZE_INFO *newtop;

    newtop = StackCarveAlloc( carveNEST, &currInit->nest );
    newtop->entry = entry;
    newtop->previous = prev;
    newtop->offset = 0;
    switch( entry ) {
    case DE_BRACE:
        newtop->type = NULL;
        newtop->base = 0;
        newtop->mem_size = 0;
        newtop->padded_size = 0;
        break;
    case DE_ROOT_TYPE:
        newtop->type = currInit->sym_type;
        dataInitComputeTarget( newtop );
        size = CgMemorySize( currInit->sym_type );
        newtop->mem_size = size;
        newtop->padded_size = size;
        newtop->base = 0;
        break;
    case DE_TYPE:
        if( prev->entry == DE_ROOT_TYPE ) {
            // just propagate root info for first push of aggregate
            newtop->type = prev->type;
            newtop->target = prev->target;
            newtop->u = prev->u;
            newtop->mem_size = prev->mem_size;
            newtop->padded_size = prev->padded_size;
        } else {
            if( prev->target == DT_CLASS ) {
                newtop->type = prev->u.c.curr->sym_type;
                dataInitComputeTarget( newtop );
                size = CgMemorySize( newtop->type );
                newtop->mem_size = size;
                newtop->padded_size = dataInitFieldSize( prev );
            } else {
                newtop->type = TypedefModifierRemoveOnly( prev->type )->of;
                dataInitComputeTarget( newtop );
                size = CgMemorySize( newtop->type );
                newtop->mem_size = size;
                newtop->padded_size = size;
            }
        }
        newtop->base = prev->base + prev->offset;
        break;
    }
    dtorableObjectBeg( newtop );
    _dump( "Pushed " );
    _dumpInitInfo( newtop );
}

static void dataInitInit( INITFINI* defn )
/****************************************/
{
    defn = defn;
    _dump( "DataInitInit" );
    code_file_save_scope = NULL;
    code_file_save_handle = NULL;
    code_file_save_depth = 0;
    code_file_open_depth = CODE_FILE_NOT_OPEN;
    carveNEST  = CarveCreate( sizeof( INITIALIZE_INFO ), BLOCK_NEST );
    carveQUEUE = CarveCreate( sizeof( INITIALIZE_QUEUE ), BLOCK_QUEUE );
    currInit = NULL;
}

static void dataInitFini( INITFINI* defn )
/****************************************/
{
    defn = defn;
    _dump( "DataInitFini" );
    while( currInit != NULL ) {
        // last initialization didn't get finished, just abandon it
        dataInitAbandon();
    }
    DbgStmt( CarveVerifyAllGone( carveNEST, "NEST" ) );
    DbgStmt( CarveVerifyAllGone( carveQUEUE, "QUEUE" ) );
    CarveDestroy( carveNEST );
    CarveDestroy( carveQUEUE );
}

INITDEFN( data_init, dataInitInit, dataInitFini )


static void dataInitStartLocal( INITIALIZE_DATA *init, DECL_INFO *dinfo, bool initializer )
/*****************************************************************************************/
// called internally for both initializer and no-initializer case
// initializer=TRUE indicates initializer to follow
{
    SYMBOL      sym;
    type_flag   sym_mods;
    INITIALIZE_DATA *prev_init;

    _dump( "dataInitStartLocal" );
#if 0   /* not correct for nested initializations */
    if( currInit != NULL ) {
        // most recent initialization didn't get finished, just abandon it
        dataInitAbandon();
    }
#endif
    sym = dinfo->sym;
    prev_init = currInit;
    currInit = init;
    currInit->prev = prev_init;
    currInit->dinfo = dinfo;
    currInit->sym = sym;
    currInit->auto_sym = NULL;
    currInit->ctor_sym = NULL;
    if( sym == NULL ) {
        currInit->sym_type = dinfo->type;
    } else {
        currInit->sym_type = sym->sym_type;
    }
    currInit->base_type = NULL;
#if 0
    currInit->auto_type = NULL;
#endif
    currInit->size_type = 0;
    currInit->nest = NULL;
    currInit->queue = NULL;
    currInit->state = DS_IGNORE;
    currInit->location = DL_IGNORE;
    currInit->once_only = 0;
    currInit->label_done = 0;
    currInit->auto_static = 0;
    currInit->need_storage = initializer;
    currInit->ctor_reqd = 0;
    currInit->dtor_reqd = 0;
    currInit->dtor_done = 0;
    currInit->const_int = 0;
    currInit->emit_code = 0;
    currInit->bracketed = 0;
    currInit->all_zero = 1;
    currInit->huge_sym = 0;
    currInit->initted = initializer;
    currInit->no_size = 0;
    currInit->const_object = 0;
    currInit->simple_set = 0;
    currInit->use_simple = 0;

    if( initializer && ! DeclWithInit( dinfo ) ) {
        return;                                 // <<<--- early return
    }

    if( SymIsHugeData( sym ) ) {
        currInit->huge_sym = 1;
        currInit->need_storage = 1;
        if( dataInitCheckHugeAlign( sym->sym_type ) ) {
            CErr( ERR_HUGE_SYM_ALIGN, sym );
            return;                             // <<<--- early return
        }
    }
    if( SymRequiresDtoring( sym ) ) {
        currInit->dtor_reqd = 1;
        currInit->emit_code = 1;
        if( CgTypeSize( sym->sym_type ) == 0 ) {
            currInit->no_size = 1;
        }
    }
    currInit->base_type = TypeNeedsCtor( sym->sym_type );
    if( currInit->base_type != NULL ) {
        currInit->ctor_reqd = 1;
        currInit->size_type = CgMemorySize( currInit->base_type );
    }
    if( SymIsConstant( sym ) ) {
        currInit->const_int = 1;
    }
    TypeModFlags( sym->sym_type, &sym_mods );
    if( sym_mods & TF1_CONST ) {
        currInit->const_object = 1;
    }
    if( ScopeType( SymScope( sym ), SCOPE_BLOCK ) ) {
        currInit->bracketed = 1;
        DgInitBegin( sym );
        if( SymIsAutomatic( sym ) ) {
            currInit->location = DL_INTERNAL_AUTO;
        } else {
            currInit->location = DL_INTERNAL_STATIC;
        }
    } else {
        if( SymIsStaticData( sym ) ) {
            currInit->bracketed = 1;
            DgInitBegin( sym );
            currInit->location = DL_EXTERNAL_STATIC;
        } else {
            currInit->location = DL_EXTERNAL_PUBLIC;
        }
    }

    dataInitPushStack( DE_ROOT_TYPE, NULL );
    if( currInit->state != DS_ERROR ) {
        currInit->state = DS_EXPRESSION;

        if( initializer && currInit->nest->target != DT_SCALAR ) {
            currInit->state = DS_OPEN_BRACE;
            if( currInit->location != DL_INTERNAL_AUTO ) {
                dataInitLabel();
            }
        }
    }
}

void DataInitStart( INITIALIZE_DATA *init, DECL_INFO *dinfo )
/***********************************************************/
// called when explicit data initialization is parsed
{
    _dump( "DataInitStart:" );
    dataInitStartLocal( init, dinfo, TRUE );
}

void DataInitFinish( INITIALIZE_DATA *init )
/******************************************/
// called when initialization is complete (syntax-wise)
{
    SYMBOL      sym;
    SCOPE       save_scope;

    if( init != currInit ) {
        _dump( "datainit: currInit already popped in DataInitFinish" );
        return;
    }
    _dumpInt( "DataInitFinish: state(%d)\n", currInit->state );
    switch( currInit->state ) {
    case DS_EXPRESSION:
    case DS_OPEN_BRACE:
    case DS_CLOSE_BRACE:
    case DS_ERROR:
        dataInitAbandon();
        break;
    case DS_ABANDONED:
        if( CompFlags.compile_failed ) {
            while( code_file_open_depth != 0 ) {
                dataInitCodeFileClose();
            }
        }
    case DS_COMPLETED:
    case DS_IGNORE:
        currInit = currInit->prev;
        break;
    case DS_FINISH:
        sym = currInit->sym;
        if( currInit->auto_static ) {
            CgSegIdData( sym, dataInitSegIdControl() );
            #if 0
            if( currInit->auto_type != NULL ) {
                currInit->auto_type->u.a.array_size =
                    CgTypeSize( sym->sym_type );
            }
            #endif
        }
        sym = currInit->dinfo->sym;
        dataInitCodeFileOpen( FALSE );
        if( currInit->bracketed ) {
            DgInitDone();
        }
        if( currInit->emit_code ) {
            sym->flag |= SF_REFERENCED;
            if( SymIsThreaded( sym ) ) {
                CErr2p( ERR_THREAD_CODE_REQD, sym );
            }
        } else {
            sym->flag &= ~SF_REFERENCED;
            if( currInit->const_object ) {
                sym->flag |= SF_CONST_NO_CODE;
            }
        }
        if( currInit->initted ) {
            sym->flag |= SF_INITIALIZED;
        } else {
            sym->flag &= ~SF_INITIALIZED;
        }
        switch( currInit->location ) {
        case DL_INTERNAL_AUTO:
        case DL_INTERNAL_STATIC:
            if( SymIsInitialized( sym )
             || SymIsStaticData( sym )
             || SymIsClass( sym )
             || SymIsClassArray( sym ) ) {
                LabelDeclInited( sym );
            }
            break;
        case DL_EXTERNAL_STATIC:
        case DL_EXTERNAL_PUBLIC:
            if( currInit->dtor_reqd ) {
                save_scope = ModuleInitResume();
                LabelDeclInitedFileScope( sym );
                ModuleInitRestore( save_scope );
            }
            break;
        }
        sym->flag |= SF_INITIALIZED;
        /* must be called after symbol is initialized */
        CgSegIdData( sym, dataInitSegIdControl() );
        dataInitCodeFileClose();
        dataInitPopStack();
        if( currInit->once_only ) {
            StaticInitFuncEnd( currInit->once_only_label );
        }
        currInit->state = DS_COMPLETED;
        currInit = currInit->prev;
        break;
    }
}

static bool dataInitCheckAnalyse( PTREE *pexpr )
/**********************************************/
{
    PTREE   expr = *pexpr;
    bool    retn = TRUE;        // return TRUE if no error

    _dump( "- Analysed Expression ---------------------------------" );
    _dumpPTree( expr );
    _dump( "-------------------------------------------------------" );

    // check for error in expression
    if( ( expr->op == PT_ERROR ) || ( expr->cgop != CO_EXPR_DONE ) ) {
        PTreeErrorNode( expr );
        currInit->state = DS_ERROR;
        retn = FALSE;
    }
    *pexpr = expr;
    return( retn );
}

static void setModuleInitFnScope( void )
{
    SYMBOL sym;
    SCOPE scope;

    sym = currInit->sym;
    scope = SymScope( sym );
    if( scope != ModuleFnScope() ) {
        switch( ScopeId( scope ) ) {
        case SCOPE_CLASS:
        case SCOPE_FILE:
            ModuleInitResumeScoped( scope );
            break;
        DbgDefault( "unexpected scope type" );
        }
    }
}

static bool dataInitAnalyseExpr( PTREE *pexpr )
/*********************************************/
// return FALSE if error during analysis
{
    PTREE       expr = *pexpr;
    PTREE       left;
    PTREE       right;
    bool        retn;
    TOKEN_LOCN  locn;       // - location of RHS

    right = PTreeExtractLocn( expr, &locn );
    left = dataInitLeftSide( currInit->nest, &locn );
    expr = PTreeBinary( CO_INIT, left, right );
    if( currInit->use_simple ) {
        expr = PTreeSetLocn( expr, &(currInit->simple_locn) );
    } else {
        expr = PTreeSetLocn( expr, &locn );
    }

    _dump( "- Pre Analysed Expression -----------------------------" );
    _dumpPTree( expr );
    _dump( "-------------------------------------------------------" );

    switch( currInit->location ) {
    case DL_INTERNAL_AUTO:
        expr = AnalyseInitExpr( expr, FALSE );
        break;
    case DL_INTERNAL_STATIC:
        expr->flags |= PTF_KEEP_MPTR_SIMPLE;
        expr = AnalyseInitExpr( expr, TRUE );
        break;
    case DL_EXTERNAL_STATIC:
    case DL_EXTERNAL_PUBLIC:
        expr->flags |= PTF_KEEP_MPTR_SIMPLE;
        dataInitCodeFileOpen( FALSE );
        setModuleInitFnScope();
        expr = AnalyseInitExpr( expr, TRUE );
        dataInitCodeFileClose();
        break;
    }
    retn = dataInitCheckAnalyse( &expr );

    *pexpr = expr;
    return( retn );
}

static bool dataInitAnalyseCtor( PTREE *pexpr )
/*********************************************/
// return FALSE if error in analysis
{
    bool     retn;

    currInit->use_simple = TRUE;
    retn = dataInitAnalyseExpr( pexpr );
    return( retn );
}

static void dataInitEmitExpr( PTREE node )
/****************************************/
{
    currInit->emit_code = 1;
    dataInitCodeFileOpen( TRUE );
    switch( currInit->location ) {
    case DL_INTERNAL_STATIC:
        if( !currInit->once_only ) {
            currInit->once_only = 1;
            currInit->once_only_label = StaticInitFuncBeg();
        }
        // deliberate fall through
    case DL_INTERNAL_AUTO:
    case DL_EXTERNAL_STATIC:
    case DL_EXTERNAL_PUBLIC:
        break;
    }
    IcEmitExpr( node );
    dataInitCodeFileClose();
}

static SYMBOL dataInitFindDefaultCtor( TYPE type )
/************************************************/
{
    unsigned    result;
    SYMBOL      ctor;

    dataInitCodeFileOpen( FALSE );
    if( SymIsStaticMember( currInit->sym ) ) {
        ModuleInitResumeScoped( SymScope( currInit->sym ) );
    }
    result = ClassDefaultCtorFind( type, &ctor, NULL );
    dataInitCodeFileClose();
    switch( result ) {
    case CNV_OK:
        break;
    case CNV_IMPOSSIBLE:
    case CNV_AMBIGUOUS:
        CErr2p( ERR_NO_DEFAULT_INIT_CTOR, type );
        // fall through
    case CNV_ERR:
        ctor = NULL;
        break;
    }
    return( ctor );
}

static void dataInitInvokeCtor( target_size_t start )
/***************************************************/
{
    PTREE   node;

    node = dataInitPadLeftSide( start );
    node = EffectCtor( NULL
                     , currInit->ctor_sym
                     , currInit->base_type
                     , node
                     , NULL
                     , EFFECT_EXACT );
    if( node->op == PT_ERROR ) {
        NodeFreeDupedExpr( node );
        currInit->state = DS_ERROR;
        return;
    }
    node = dtorableObjectCtored( currInit->nest, node, TRUE );
    node = NodeDone( node );

    _dump( "- Invoke Ctor Expression ------------------------------" );
    _dumpPTree( node );
    _dump( "-------------------------------------------------------" );

    dataInitEmitExpr( node );
}

static void dataInitRunTimeCall( target_size_t start, target_size_t size )
/************************************************************************/
{
    PTREE               node;
    unsigned            num_elem;
    TYPE                base_type;
    RTF                 fun_code;
    SYMBOL              ctor_sym;

    switch( currInit->state ) {
    case DS_ERROR:
    case DS_ABANDONED:
    case DS_COMPLETED:
    case DS_IGNORE:
        return;
        break;
    }
    ctor_sym = currInit->ctor_sym;
    if( ctor_sym == NULL ) {
        ctor_sym = dataInitFindDefaultCtor( currInit->base_type );
        if( ctor_sym == NULL ) {
            currInit->state = DS_ERROR;
            return;
        }
        currInit->ctor_sym = ctor_sym;
    }
    DbgAssert( ctor_sym != NULL );
    currInit->emit_code = 1;
    num_elem = size / currInit->size_type;
    if( num_elem <= runTimeThreshold ) {
        while( num_elem > 0 && currInit->state != DS_ERROR ) {
            dataInitInvokeCtor( start );
            start += currInit->size_type;
            num_elem--;
        }
    } else {
        TYPE_SIG *sig;          // - type signature
        TYPE refd;              // - reference type of base_type
        ctor_sym = ClassFunMakeAddressable( ctor_sym );
        base_type = currInit->base_type;
        refd = currInit->sym->sym_type;
        if( NULL == ArrayType( refd ) ) {
            refd = PointerTypeForArray( refd );
            refd = TypePointedAtModified( refd );
        }
        if( TypeTruncByMemModel( refd ) ) {
            CErr( ERR_CTOR_OBJ_MEM_MODEL );
            currInit->state = DS_ERROR;
        } else {
            sig = dataInitTypeSigFind( base_type, TSA_DEFAULT_CTOR | TSA_DTOR );
            node = NodeArguments( NodeTypeSig( sig )
                                , NodeOffset( num_elem )
                                , dataInitPadLeftSide( start )
                                , NULL );
            if( TypeHasVirtualBases( base_type ) ) {
                fun_code = RTF_CTOR_VARR;
            } else {
                fun_code = RTF_CTOR_ARR;
            }
            node = RunTimeCall( node, MakeReferenceTo( base_type ), fun_code );
            node = dtorableObjectCtored( currInit->nest, node, TRUE );
            node = NodeDone( node );

            _dump( "- RunTime Call Expression -----------------------------" );
            _dumpPTree( node );
            _dump( "-------------------------------------------------------" );

            dataInitEmitExpr( node );
        }
    }
    //DtorArrayIndex
}

static void dataInitRunTimeCallHuge( target_size_t position, target_size_t diff )
/*******************************************************************************/
{
    target_size_t     increment;

    while( diff != 0 && currInit->state != DS_ERROR ) {
        dataInitCheckHugeSegment( position );
#if _CPU == 8086
        position %= (TARGET_UINT_MAX + 1);
        increment = TARGET_UINT_MAX + 1 - position;
#else
        increment = - position;
#endif
        if( increment > diff ) {
            increment = diff;
        }
        increment -= (increment % currInit->size_type );
        dataInitRunTimeCall( position, increment );
        diff -= increment;
        position += increment;
    }
}

static void dataInitFlushQueue( void )
/************************************/
// flush out calls to default ctors
{
    target_size_t       start;
    target_size_t       size;
    INITIALIZE_QUEUE    *nextq;

    start = 0;
    size = 0;
    nextq = RingPop( &currInit->queue );
    while( nextq != NULL && currInit->state != DS_ERROR ) {
        start = nextq->start;
        size = nextq->size;
        CarveFree( carveQUEUE, nextq );
        nextq = RingPop( &currInit->queue );
        while( nextq != NULL && nextq->start == (start+size) ) {
            size += nextq->size;
            CarveFree( carveQUEUE, nextq );
            nextq = RingPop( &currInit->queue );
        }
        if( currInit->huge_sym ) {
            dataInitRunTimeCallHuge( start, size );
        } else {
            dataInitRunTimeCall( start, size );
        }
    }
}

static void dataInitStashExpr( PTREE expr )
/*****************************************/
{
    bool            constant;
    target_size_t   size;
    target_size_t   offset;
    PTREE           cexpr;
    TYPE            type;

    type = currInit->nest->type;
    size = CgMemorySize( type );
    constant = isDataInitConstant( expr, &cexpr, &offset );

    switch( currInit->location ) {
    case DL_INTERNAL_AUTO:
        // make sure PTF_KEEP_MPTR_SIMPLE hasn't been used
        DbgAssert( expr->u.subtree[0] != NULL );
        if( currInit->auto_static ) {
            if( constant ) {
                if( !DgStoreScalar( cexpr, offset, type ) )
                    currInit->all_zero = 0;
                NodeFreeSearchResult( cexpr );
                PTreeFreeSubtrees( expr );
            } else {
                DgPadBytes( size );
                dataInitFlushQueue();
                dtorableObjectCtored( currInit->nest, expr, FALSE );
                dataInitEmitExpr( expr );
            }
        } else {
            if( constant && currInit->const_int ) {
                DgStoreConstScalar( cexpr, type, currInit->sym );
            }
            dataInitFlushQueue();
            dtorableObjectCtored( currInit->nest, expr, FALSE );
            dataInitEmitExpr( expr );
        }
        break;
    case DL_INTERNAL_STATIC:
    case DL_EXTERNAL_STATIC:
    case DL_EXTERNAL_PUBLIC:
        dataInitLabel();
        if( currInit->huge_sym ) {
            dataInitCheckHugeSegment( currInit->nest->base + currInit->nest->offset );
        }
        if( constant ) {
            if( currInit->const_int ) {
                DbgAssert( expr->u.subtree[0] != NULL );
                DgStoreConstScalar( cexpr, type, currInit->sym );
            }
            if( !DgStoreScalar( cexpr, offset, type ) )
                currInit->all_zero = 0;
            NodeFreeSearchResult( cexpr );
            PTreeFreeSubtrees( expr );
            dtorableObjectCtored( currInit->nest, NULL, FALSE );
        } else {
            DbgAssert( expr->u.subtree[0] != NULL );
            DgPadBytes( size );
            dataInitFlushQueue();
            dtorableObjectCtored( currInit->nest, expr, FALSE );
            dataInitEmitExpr( expr );
        }
        break;
    }
    currInit->nest->offset += size;
}

static void dataInitUpdateState( void )
/*************************************/
{
    switch( currInit->state ) {
    case DS_ERROR:
    case DS_ABANDONED:
    case DS_IGNORE:
        break;
    default:
        if( currInit->nest->entry == DE_ROOT_TYPE ) {
            dataInitFlushQueue();
            currInit->state = DS_FINISH;
        } else if( currInit->nest->entry == DE_BRACE ) {
            currInit->state = DS_CLOSE_BRACE;
        } else {
            currInit->state = DS_EXPRESSION;
        }
    }
}

void DataInitConstructorParms( PTREE expr )
/*****************************************/
// called for "id(expr1, expr2, expr3, ...);" cases
{
    if( currInit == NULL ) {
        _fatal( "datainit: currInit null in DataInitConstructorParms" );
        return;
    }
    _dumpInt( "DataInitConstructorParms: state(%d)\n", currInit->state );
    switch( currInit->state ) {
    case DS_CLOSE_BRACE:
    case DS_FINISH:
        CErr( ERR_TOO_MANY_INITIALIZERS );
        currInit->state = DS_ERROR;
        PTreeFreeSubtrees( expr );
        break;
    case DS_ERROR:
    case DS_ABANDONED:
    case DS_COMPLETED:
    case DS_IGNORE:
        PTreeFreeSubtrees( expr );
        break;
    case DS_OPEN_BRACE:
        if( currInit->nest->target != DT_CLASS ) {
            CErr( ERR_EXPECTING_BUT_FOUND_EXPRESSION, OPEN_BRACE );
            currInit->state = DS_ERROR;
            PTreeFreeSubtrees( expr );
            break;
        }
        /* fall through */
    case DS_EXPRESSION:
        if( dataInitAnalyseCtor( &expr ) ) {
            dataInitStashExpr( expr );
            dataInitUpdateState();
        }
        break;
    }
}

static bool dataInitIsFull( INITIALIZE_INFO *nest )
/*************************************************/
// return TRUE if stack entry is full
// braces and root_type are never full
{
    bool retn = FALSE;

    if( ( nest->entry != DE_BRACE ) && ( nest->entry != DE_ROOT_TYPE ) ) {
        switch( nest->target ) {
        case DT_SCALAR:
            retn = TRUE;
            break;
        case DT_ARRAY:
            if( nest->offset == nest->mem_size ) {
                retn = TRUE;
            }
            break;
        case DT_CLASS:
            if( nest->u.c.curr == NULL ) {
                retn = TRUE;
            }
            break;
        case DT_BITFIELD:
            // type has already been advanced to next aggregate entry
            if( nest->type == NULL ) {
                retn = TRUE;
            } else if( nest->type->id != TYP_BITFIELD ) {
                retn = TRUE;
            } else if( nest->type->u.b.field_start == 0 ) {
                retn = TRUE;
            }
            break;
        }
    }
    return( retn );
}

static void dataInitEnQueue( target_size_t start, target_size_t size )
/********************************************************************/
// enqueue a call to default ctor
{
    INITIALIZE_QUEUE    *newq;

    newq = RingCarveAlloc( carveQUEUE, &currInit->queue );
    newq->start = start;
    newq->size = size;
}

static TYPE dataInitAdvanceField( INITIALIZE_INFO *entry )
/********************************************************/
{
    SYMBOL      curr;
    TYPE        type;
    type_flag   flags;

    curr = dataInitScopeOrderedNext( entry->u.c.stop, entry->u.c.curr );
    entry->u.c.curr = curr;
    if( curr != NULL ) {
        type = curr->sym_type;
        flags = StructType( entry->type )->flag;
        if(( flags & TF1_UNION ) == 0 ) {
            return( type );
        }
        if( curr->u.member_offset != 0 ) {
            // anonymous struct fields in a union
            return( type );
        }
        entry->u.c.curr = NULL;
    }
    return( NULL );
}

static void dataInitPadOutHuge( INITIALIZE_INFO *top )
/****************************************************/
{
    target_size_t     position;
    target_size_t     diff;
    target_size_t     increment;

    if( top->padded_size < top->offset ) {
        top->offset = top->padded_size;
    }
    diff = top->padded_size - top->offset;
    position = top->base + top->offset;
    while( diff != 0 ) {
        dataInitCheckHugeSegment( position );
#if _CPU == 8086
        position %= (TARGET_UINT_MAX + 1);
        increment = TARGET_UINT_MAX + 1 - position;
#else
        increment = - position;
#endif
        if( increment > diff ) {
            increment = diff;
        }
        DgPadBytes( increment );
        diff -= increment;
        position += increment;
    }
}

static void dataInitPadOut( INITIALIZE_INFO *top )
/************************************************/
{
    target_size_t     diff;

    _dump( "dataInitPadOut" );
    _dumpInitInfo( top );
    switch( top->target ) {
    case DT_ARRAY:
        if( top->padded_size != 0 ) {
            if( top->padded_size < top->offset ) {
                top->offset = top->padded_size;
            }
            diff = top->padded_size - top->offset;
            if( diff > 0 ) {
                if( currInit->ctor_reqd ) {
                    dataInitEnQueue( top->base + top->offset,
                                     top->padded_size - top->offset );
                }
                if( currInit->need_storage ) {
                    if( currInit->huge_sym ) {
                        dataInitPadOutHuge( top );
                    } else {
                        DgPadBytes( diff );
                    }
                }
            }
            top->offset = top->padded_size;
        } else {
            if( currInit->auto_static ) {
                currInit->auto_sym->sym_type =
                    DupArrayTypeForArray( currInit->auto_sym->sym_type
                                        , top->u.a.index );
            }
            currInit->sym->sym_type =
                DupArrayTypeForArray( currInit->sym->sym_type
                                    , top->u.a.index );
            top->mem_size = top->offset;
            top->padded_size = top->offset;
        }
        break;
    case DT_SCALAR:
    case DT_CLASS:
        if( top->padded_size < top->offset ) {
            top->offset = top->padded_size;
        }
        diff = top->padded_size - top->offset;
        if( diff > 0 ) {
            if( currInit->ctor_reqd ) {
                dataInitEnQueue( top->base + top->offset,
                                 top->padded_size - top->offset );
            }
            if( currInit->need_storage ) {
                if( currInit->huge_sym ) {
                    dataInitPadOutHuge( top );
                } else {
                    DgPadBytes( diff );
                }
            }
        }
        top->offset = top->padded_size;
        break;
    case DT_BITFIELD:
        if( currInit->huge_sym ) {
            dataInitCheckHugeSegment( top->base + top->offset );
        }
        if( !DgStoreBitfield( top->u.b.type, top->u.b.mask ) )
            currInit->all_zero = 0;
        while( !dataInitIsFull( top ) ) {
            top->type = dataInitAdvanceField( top->previous );
        }
#if 1
        diff = top->padded_size - CgMemorySize( top->u.b.type );
        if( diff > 0 ) {
            if( currInit->need_storage ) {
                DgPadBytes( diff );
            }
        }
#endif
        top->offset = top->padded_size;
        break;
    }
}

static void dataInitUpdatePrevious( INITIALIZE_INFO *top )
/********************************************************/
{
    INITIALIZE_INFO *prev;

    prev = top->previous;
    if( ( prev != NULL ) && ( prev->entry != DE_ROOT_TYPE ) ) {
        if( !(top->target == DT_SCALAR && top->u.s.bitf == TRUE) ) {
            prev->offset += top->padded_size;
        }
        switch( prev->target ) {
        case DT_ARRAY:
            prev->u.a.index++;
            break;
        case DT_CLASS:
            if( top->target != DT_BITFIELD ) {
                dataInitAdvanceField( prev );
            }
            break;
        case DT_BITFIELD:
            prev->type = dataInitAdvanceField( prev->previous );
            break;
        }
    }
}

static void dataInitPushNest( bool brace )
/****************************************/
// push the level of nesting
// if brace, then push a brace entry first
{
    INITIALIZE_INFO *oldtop;

    oldtop = currInit->nest;
    if( brace ) {
        dataInitPushStack( DE_BRACE, NULL );
    }
    dataInitPushStack( DE_TYPE, oldtop );
}

static void dataInitPopNest( void )
/*********************************/
{
    if( currInit->nest->entry != DE_BRACE ) {
        dataInitPadOut( currInit->nest );
        dataInitUpdatePrevious( currInit->nest );
    }
    dataInitPopStack();
}

static void mayNeedAutoStaticInitCopy( void )
/*******************************************/
{
    if( currInit->location == DL_INTERNAL_AUTO ) {
        if( !currInit->auto_static ) {
            currInit->auto_static = 1;
            currInit->auto_sym = currInit->sym;
            currInit->sym = StaticInitSymbol( currInit->auto_sym );
            dataInitEmitAutoAssign( currInit->auto_sym, currInit->sym );
            currInit->sym->flag |= SF_INITIALIZED;
            dataInitLabel();
        }
    }
}

static void dataInitStashString( PTREE expr )
/*******************************************/
{
    target_size_t size;
    target_size_t dim;

    size = StringByteLength( expr->u.string );
    dim = StringAWStrLen( expr->u.string );
    switch( currInit->location ) {
    case DL_INTERNAL_AUTO:
        mayNeedAutoStaticInitCopy();
        DgStoreString( expr );
        break;
    case DL_INTERNAL_STATIC:
    case DL_EXTERNAL_STATIC:
    case DL_EXTERNAL_PUBLIC:
        dataInitLabel();
        DgStoreString( expr );
        break;
    }
    PTreeFreeSubtrees( expr );
    currInit->all_zero = 0;
    currInit->nest->offset += size;
    currInit->nest->u.a.index = dim;
    if( currInit->nest->mem_size != 0 ) {
        if( currInit->nest->offset > currInit->nest->mem_size ) {
            CErr( ERR_TOO_MANY_STRING_INITIALIZERS );
            currInit->state = DS_ERROR;
        }
    }
}

static void dataInitStringExpr( PTREE expr )
/******************************************/
{
    dataInitStashString( expr );
    dataInitPopNest();
    while( dataInitIsFull( currInit->nest ) ) {
        dataInitPopNest();
    }
    dataInitUpdateState();
}

static void dataInitSaveBits( PTREE expr )
/****************************************/
{
    target_ulong        value;
    INITIALIZE_INFO     *prev;

    prev = currInit->nest->previous;
    value = expr->u.uint_constant;
    value &= bitMask[prev->type->u.b.field_width-1];
    value = value << prev->type->u.b.field_start;
    prev->u.b.mask |= value;
}

static void dataInitStashBitfield( PTREE expr )
/*********************************************/
{
    bool            constant;
    target_size_t   size;
    target_size_t   offset;
    PTREE           cexpr;

    size = CgMemorySize( currInit->nest->type );
    constant = isDataInitConstant( expr, &cexpr, &offset );

    if( constant ) {
        dataInitSaveBits( cexpr );
        NodeFreeSearchResult( cexpr );
        PTreeFreeSubtrees( expr );
    } else {
        dataInitEmitExpr( expr );
    }
    currInit->nest->offset += size;
}

static void dataInitCheckStringConcat( bool string
                                     , bool multi_line_concat
                                     , PTREE expr )
/*************************************************/
{
    if( string && multi_line_concat ) {
        if( currInit->nest->previous->target == DT_ARRAY ) {
            expr = PTreeSetErrLoc( expr );
            CErr1( WARN_STRING_CONCAT_IN_ARRAY );
        }
    }
}

void DataInitExpr( PTREE expr )
/*****************************/
// called for an expression
{
    bool string, multi_line_concat;

    if( currInit == NULL ) {
        _fatal( "datainit: currInit null in DataInitExpr" );
        return;
    }
    _dumpInt( "DataInitExpr: state(%d)\n", currInit->state );
    string = IsStringConstant( expr, &multi_line_concat );
    switch( currInit->state ) {
    case DS_CLOSE_BRACE:
    case DS_FINISH:
        CErr( ERR_TOO_MANY_INITIALIZERS );
        currInit->state = DS_ERROR;
        PTreeFreeSubtrees( expr );
        break;
    case DS_ERROR:
    case DS_ABANDONED:
    case DS_COMPLETED:
    case DS_IGNORE:
        PTreeFreeSubtrees( expr );
        break;
    case DS_OPEN_BRACE:
        if( string && TypeIsCharString( currInit->nest->type ) ) {
            dataInitStringExpr( expr );
        } else if( currInit->nest->target == DT_CLASS ) {
            if( dataInitAnalyseExpr( &expr ) ) {
                dataInitStashExpr( expr );
                dataInitPopNest();
                dataInitUpdateState();
            } else {
                currInit->state = DS_ERROR;
                PTreeFreeSubtrees( expr );
            }
        } else {
            CErr( ERR_EXPECTING_BUT_FOUND_EXPRESSION, OPEN_BRACE );
            currInit->state = DS_ERROR;
            PTreeFreeSubtrees( expr );
        }
        break;
    case DS_EXPRESSION:
        while( IsCgTypeAggregate( currInit->nest->type, string ) ) {
            dataInitPushNest( FALSE );
        }
        if( string && TypeIsCharString( currInit->nest->type ) ) {
            dataInitStringExpr( expr );
        } else if( dataInitAnalyseExpr( &expr ) ) {
            dataInitCheckStringConcat( string, multi_line_concat, expr );
            if( currInit->nest->u.s.bitf ) {
                dataInitStashBitfield( expr );
            } else {
                dataInitStashExpr( expr );
            }
            while( dataInitIsFull( currInit->nest ) ) {
                dataInitPopNest();
            }
            dataInitUpdateState();
        }
        break;
    }
}

void DataInitSimpleLocn( TOKEN_LOCN *locn )
/*****************************************/
// set location for simple initializations
{
    DbgAssert( currInit != NULL );
    TokenLocnAssign( currInit->simple_locn, *locn );
    currInit->simple_set = TRUE;
}

void DataInitSimple( PTREE expr )
/*******************************/
// called for "= expr;" cases
{
    if( currInit == NULL ) {
        _fatal( "datainit: currInit null in DataInitSimple" );
        return;
    }
    switch( currInit->state ) {
    case DS_ERROR:
    case DS_ABANDONED:
    case DS_COMPLETED:
    case DS_IGNORE:
        PTreeFreeSubtrees( expr );
        break;
    default:
        currInit->use_simple = TRUE;
        dataInitPushNest( FALSE );
        DataInitExpr( expr );
        break;
    }
}

void DataInitPush( void )
/***********************/
// called when open brace is parsed
{
    if( currInit == NULL ) {
        _fatal( "datainit: currInit null in DataInitPush" );
        return;
    }
    _dumpInt( "DataInitPush: state(%d)\n", currInit->state );
    switch( currInit->state ) {
    case DS_CLOSE_BRACE:
        CErr( ERR_EXPECTING_BUT_FOUND, CLOSE_BRACE, OPEN_BRACE );
        currInit->state = DS_ERROR;
        break;
    case DS_FINISH:
        CErr( ERR_UNEXPECTED_DURING_INITIALIZATION, OPEN_BRACE );
        currInit->state = DS_ERROR;
        break;
    case DS_IGNORE:
    case DS_ABANDONED:
    case DS_COMPLETED:
    case DS_ERROR:
        break;
    case DS_OPEN_BRACE:
        mayNeedAutoStaticInitCopy();
        currInit->state = DS_EXPRESSION;
        // deliberate falling through
    case DS_EXPRESSION:
        if( IsCgTypeAggregate( currInit->nest->type, FALSE ) ) {
            dataInitPushNest( TRUE );
        } else if( currInit->nest->entry == DE_ROOT_TYPE ) {
            dataInitPushNest( TRUE );
        } else {
            CErr( ERR_UNEXPECTED_DURING_INITIALIZATION, OPEN_BRACE );
            currInit->state = DS_ERROR;
        }
        break;
    }
}

void DataInitPop( void )
/**********************/
// called when close brace is parsed
{
    if( currInit == NULL ) {
        _fatal( "datainit: currInit null in DataInitPop" );
        return;
    }
    _dumpInt( "DataInitPop: state(%d)\n", currInit->state );
    switch( currInit->state ) {
    case DS_OPEN_BRACE:
        CErr( ERR_EXPECTING_BUT_FOUND, OPEN_BRACE, CLOSE_BRACE );
        currInit->state = DS_ERROR;
        break;
    case DS_FINISH:
        CErr( ERR_UNEXPECTED_DURING_INITIALIZATION, CLOSE_BRACE );
        currInit->state = DS_ERROR;
        break;
    case DS_IGNORE:
    case DS_ABANDONED:
    case DS_COMPLETED:
    case DS_ERROR:
        break;
    case DS_CLOSE_BRACE:
    case DS_EXPRESSION:
        while( currInit->nest->entry != DE_BRACE ) {
            dataInitPopNest();
        }
        dataInitPopNest();
        while( dataInitIsFull( currInit->nest ) ) {
            dataInitPopNest();
        }
        dataInitUpdateState();
        break;
    }
}

DECL_INFO *DataInitNoInit( INITIALIZE_DATA *init, DECL_INFO *dinfo )
/******************************************************************/
// called for simple no initializer declarations (e.g., int a; )
{
    _dump( "DataInitNoInit:" );
    if( DeclNoInit( dinfo ) ) {
        dataInitStartLocal( init, dinfo, FALSE );
        if( currInit->state != DS_IGNORE ) {
            switch( currInit->location ) {
            case DL_INTERNAL_STATIC:
            case DL_EXTERNAL_STATIC:
            case DL_EXTERNAL_PUBLIC:
                if( currInit->huge_sym ) {
                    dataInitLabel();
                } else {
                    DgSymbol( currInit->sym );
                }
                break;
            }
            if( currInit->huge_sym || currInit->ctor_reqd ) {
                currInit->initted = currInit->ctor_reqd;
                dataInitPushNest( FALSE );
                while( IsCgTypeAggregate( currInit->nest->type, FALSE ) ) {
                    dataInitPushNest( FALSE );
                }
                while( currInit->nest->entry != DE_ROOT_TYPE ) {
                    dataInitPopNest();
                }
            }
            dataInitUpdateState();
        }
        DataInitFinish( currInit );
    }
    return( dinfo );
}
