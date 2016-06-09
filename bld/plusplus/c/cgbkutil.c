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
#include "cgfront.h"
#include "cgback.h"
#include "codegen.h"
#include "cgaux.h"
#include "feprotos.h"
#include "rtfuns.h"
#include "cgbackut.h"
#include "objmodel.h"
#include "cgio.h"

#ifndef NDEBUG
    #include "pragdefn.h"
#endif


//***********************************************************************
// Data Generation Support
//***********************************************************************


static void alignInSegment(     // PUT OUT ALIGNMENT WITHIN A SEGMENT
    target_size_t adjust,       // - adjustment
    fe_seg_id segid )           // - segment id
{
    if( adjust > 0 ) {
        if( segid == SEG_BSS ) {
            DgUninitBytes( adjust );
        } else {
            DgInitBytes( adjust, 0 );
        }
    }
}


void DgAlignSegment(            // ALIGN SEGMENT TO CORRECT BOUNDARY
    fe_seg_id segid,            // - aligned segment
    unsigned amount )           // - amount to align
{
    target_size_t adjust;

    adjust = SegmentAdjust( segid, DGTell(), amount );
    alignInSegment( adjust, segid );
}


void DgUninitBytes(             // DATA GENERATE UNINIT BYTES
    target_size_t size )        // - number of bytes
{
#if ( _CPU == 8086 )
    if( size == TARGET_UINT_MAX + 1 ) {
        size /= 2;
        DGUBytes( size );
        DGUBytes( size );
    } else {
        DGUBytes( size );
    }
#else
    DGUBytes( size );
#endif
}


void DgInitBytes(               // DATA GENERATE INIT BYTES
    target_size_t   size,       // - number of bytes
    uint_8          val )       // - byte to initialize with
{
#if ( _CPU == 8086 )
    if( size == TARGET_UINT_MAX + 1 ) {
        size /= 2;
        DGIBytes( size, val );
        DGIBytes( size, val );
    } else {
        DGIBytes( size, val );
    }
#else
    DGIBytes( size, val );
#endif
}


void DgByte(                    // DATA GENERATE A BYTE
    uint_8 byte )               // - to be generated
{
#if 0
    DGBytes( 1, &byte );
#else
    DGInteger( byte, TY_UINT_1 );
#endif
}


void DgSymbolDefInit(           // DATA GENERATE SYMBOL (DEFAULT DATA)
    SYMBOL sym )                // - the symbol
{
    segment_id      old_seg;    // - old segment
    segment_id      seg_id;     // - symbol segment
    target_size_t   size;       // - size of symbol

    seg_id = FESegID( sym );
    old_seg = BESetSeg( seg_id );
    DgAlignSegment( seg_id, SegmentAlignment( sym->sym_type ) );
    CgBackGenLabel( sym );
    size = CgMemorySize( sym->sym_type );
    if( sym->segid == SEG_BSS ) {
        DgUninitBytes( size );
    } else {
        DgInitBytes( size, 0 );
    }
    BESetSeg( old_seg );
}


static void dataGenPtrSym(      // GENERATE POINTER FOR A SYMBOL + OFFSET
    SYMBOL sym,                 // - the symbol
    target_size_t offset,       // - the offset
    cg_type type )              // - codegen type of pointer
{
    if( sym == NULL ) {
        DbgVerify( offset == 0, "dataGenPtrSym -- NULL symbol with offset <> 0" );
        DGInteger( 0, type );
    } else {
        DGFEPtr( (cg_sym_handle)sym, type, offset );
        if( type == TY_CODE_PTR && SymIsThunk( sym ) ) {
            CgioThunkAddrTaken( sym );
        }
    }
}


void DgPtrSymOff(               // GENERATE POINTER FOR A SYMBOL + OFFSET
    SYMBOL sym,                 // - the symbol
    target_size_t offset )      // - the offset
{
    if( sym == NULL || SymIsFunction( sym ) ) {
        DbgVerify( offset == 0, "DgPtrSymOffset -- function with offset <> 0" );
        dataGenPtrSym( NULL, offset, TY_CODE_PTR );
    } else {
        dataGenPtrSym( sym, offset, TY_POINTER );
    }
}


void DgOffset(                  // GENERATE AN OFFSET VALUE
    unsigned offset )           // - offset value
{
    DGInteger( offset, CgTypeOffset() );
}


void DgPtrSymDataOffset(        // GENERATE POINTER FOR A DATA SYMBOL, OFFSET
    SYMBOL sym,                 // - the symbol
    target_size_t offset )      // - the offset
{
    dataGenPtrSym( sym, offset, TY_POINTER );
}


void DgPtrSymData(              // GENERATE POINTER FOR A DATA SYMBOL
    SYMBOL sym )                // - the symbol
{
    DgPtrSymDataOffset( sym, 0 );
}


void DgPtrSymCode(              // GENERATE POINTER FOR A CODE SYMBOL
    SYMBOL sym )                // - the symbol
{
    dataGenPtrSym( sym, 0, TY_CODE_PTR );
}


void CgBackGenLabel(            // GENERATE A LABEL
    SYMBOL sym )                // - symbol defining label
{
    DGLabel( FEBack( sym ) );
}


void CgBackGenLabelInternal(    // GENERATE A LABEL FOR INTERNAL STRUCTURE
    SYMBOL sym )                // - symbol defining label
{
#if _CPU == _AXP
    DgAlignSegment( SEG_CONST, TARGET_POINTER );
#endif
    DGLabel( FEBack( sym ) );
}

segment_id DgCurrSeg( void )
{
    segment_id curr_seg;

    curr_seg = BESetSeg( UNDEFSEG );
    BESetSeg( curr_seg );
    return( curr_seg );
}

segment_id DgSetSegSym( SYMBOL sym )
{
    return( BESetSeg( FESegID( sym ) ) );
}

back_handle DgStringConst(          // STORE STRING CONSTANT WITH NULL
    STRING_CONSTANT str,            // - string to store
    uint_16         *psegid,        // - addr(string segid)
    unsigned        control )       // - control mask (DSC_*)
{
    back_handle     handle;         // - back handle for literal
    target_offset_t str_align;      // - string's alignment
    target_size_t   str_len;        // - string's length (in bytes)
    segment_id      str_seg;        // - string's segment
    segment_id      old_seg;        // - old segment

    str_seg = str->segid;
    handle = str->cg_handle;
    if( control & DSC_CONST ) {
        if( handle == 0 ) {
            handle = BENewBack( 0 );
            str->cg_handle = handle;
            str_len = str->len + TARGET_CHAR;
            if( str->wide_string ) {
                str_align = TARGET_WIDE_CHAR;
            } else {
                str_align = TARGET_CHAR;
            }
#if _CPU == _AXP
            str_seg = SEG_CONST;
#else
            if( CompFlags.strings_in_code_segment && ( control & DSC_CODE_OK ) != 0 ) {
                if( IsBigData() ) {
                    str_seg = SegmentAddStringCodeFar( str_len, str_align );
                } else {
                    if( IsFlat() ) {
                        str_seg = SegmentAddStringCodeFar( str_len, str_align );
                    } else {
                        str_seg = SEG_CONST;
                    }
                }
            } else {
                if( IsBigData() ) {
                    str_seg = SegmentAddStringConstFar( str_len, str_align );
                } else {
                    str_seg = SEG_CONST;
                }
            }
#endif
            str->segid = str_seg;
            old_seg = BESetSeg( str_seg );
#if _CPU == _AXP
            DGAlign( TARGET_INT );
#else
            DGAlign( str_align );   // NT requires word aligned wide strings
#endif
            DGLabel( handle );
            DGString( str->string, str->len );
            DgByte( 0 );
#if _CPU == _AXP
            DGAlign( TARGET_INT );
#endif
            BESetSeg( old_seg );
        }
    } else {
        // char a[] = "asdf"; initialization (use current segment)
        str_seg = DgCurrSeg();
        str->segid = str_seg;
        DGString( str->string, str->len );
        DgByte( 0 );
    }
    if( psegid != NULL ) {
        *psegid = str_seg;
    }
    return( handle );
}



//***********************************************************************
// Destruction Methods
//***********************************************************************


DT_METHOD DtmDirect(            // CONVERT DTOR METHOD TO DIRECT COUNTERPART
    DT_METHOD dtm )             // - default method
{
    switch( dtm ) {
      DbgDefault( "DtmDirect -- bad method" );
      case DTM_DIRECT :
      case DTM_DIRECT_SMALL :
        break;
      case DTM_TABLE_SMALL :
        dtm = DTM_DIRECT_SMALL;
        break;
      case DTM_DIRECT_TABLE :
      case DTM_TABLE :
        dtm = DTM_DIRECT;
        break;
    }
    return dtm;
}


static bool dtmTabular(         // DETERMINE IF METHOD IS TABULAR
    DT_METHOD method )          // - the method
{
    bool retb;                  // - true ==> is tabular

    retb = false;
    switch( method ) {
    DbgDefault( "dtmTabular -- bad method" );
    case DTM_DIRECT :
        break;
    case DTM_DIRECT_SMALL :
    case DTM_TABLE_SMALL :
    case DTM_DIRECT_TABLE :
    case DTM_TABLE :
        retb = true;
        break;
    }
    return( retb );
}


bool DtmTabular(                // DETERMINE IF SCOPE TABULAR DESTRUCTION METHOD, SCOPE
    FN_CTL* fctl )              // - function control
{
    return dtmTabular( fctl->dtor_method );
}


bool DtmTabularFunc(            // DETERMINE IF SCOPE TABULAR DESTRUCTION METHOD, FUNCTION
    FN_CTL* fctl )              // - function control
{
    return dtmTabular( fctl->func_dtor_method );
}



//***********************************************************************
// Code Generation Support -- general
//***********************************************************************


cg_name CgFetchType(            // PERFORM A FETCH
    cg_name operand,            // - operand to be fetched
    cg_type type )              // - type of fetch
{
    return CGUnary( O_POINTS, operand, type );
}


cg_name CgFetchPtr(             // FETCH A POINTER
    cg_name operand )           // - operand to be fetched
{
    return CgFetchType( operand, TY_POINTER );
}


cg_name CgFetchTemp(            // FETCH A TEMPORARY
    temp_handle handle,         // - handle for temporary
    cg_type type )              // - type of temp
{
    return CgFetchType( CGTempName( handle, type ), type );
}


cg_name CgSaveAsTemp(           // SAVE INTO A TEMPORARY
    temp_handle* a_hand,        // - addr[ temp handle ]
    cg_name expr,               // - expression to be saved
    cg_type type )              // - and its type
{
    temp_handle handle;         // - allocated temporary handle

    handle = CGTemp( type );
    *a_hand = handle;
    return CGLVAssign( CGTempName( handle, type ), expr, type );
}


static void addArgument(        // ADD AN ARGUMENT
    call_handle handle,         // - handle for call
    cg_name expr,               // - expression for argument
    cg_type type )              // - argument type
{
    CGAddParm( handle, expr, type );
}


cg_name CgSymbol(               // PASS SYMBOL TO CODE GENERATOR
    SYMBOL sym )                // - symbol
{
    return CGFEName( (cg_sym_handle)sym, CgTypeSym( sym ) );
}


cg_name CgOffsetExpr(           // MAKE OFFSET EXPRESSION
    cg_name expr,               // - lhs expression
    target_offset_t offset,     // - offset
    cg_type type )              // - resultant type
{
    if( offset == 0 ) {
        expr = CGUnary( O_CONVERT, expr, type );
    } else {
        expr = CGBinary( O_PLUS, expr, CgOffset( offset ), type );
    }
    return expr;
}


cg_name CgAddrSymbol(           // PASS ADDR OF SYMBOL TO CODE GENERATOR
    SYMBOL sym )                // - symbol
{
#if 0
    return CGFEName( (cg_sym_handle)sym, CgTypePtrSym( sym ) );
#else
    return CGFEName( (cg_sym_handle)sym, CgTypeSym( sym ) );
#endif
}


cg_name CgOffset(               // PASS ABSOLUTE OFFSET TO CODE GENERATOR
    unsigned offset )           // - offset value
{
    return CGInteger( offset, CgTypeOffset() );
}


cg_name CgSymbolPlusOffset(     // GENERATE SYMBOL + OFFSET
    SYMBOL sym,                 // - symbol
    target_offset_t offset )    // - offset
{
    return CgOffsetExpr( CgAddrSymbol( sym ), offset, CgTypePtrSym( sym ) );
}


cg_name CgFetchSymbolAddOffset( // GENERATE RVALUE( SYMBOL ) + OFFSET
    SYMBOL sym,                 // - symbol
    unsigned offset )           // - offset
{
    return CgOffsetExpr( CgFetchSym( sym ), offset, CgTypeSym( sym ) );
}


cg_type CgReturnType(           // GET CG-TYPE FOR RETURN
    cg_type type )              // - code-gen type
{
    if( CompFlags.returns_promoted && 0 == CgBackInlinedDepth() ) {
        type = FEParmType( NULL, NULL, type );
    }
    return type;
}


cg_name CgFetchSym(             // FETCH A SYMBOL
    SYMBOL sym )                // - symbol
{

    return CgFetchType( CgSymbol( sym ), CgTypeSym( sym ) );
}


cg_name CgMakeDup(              // MAKE A DUPLICATE
    cg_name *orig,              // - original
    cg_type cgtype )            // - and its type
{
    temp_handle handle;         // - dup. handle
    cg_name dup;                // - duplicated node

    dup = CgSaveAsTemp( &handle, *orig, cgtype );
    dup = CgFetchType( dup, cgtype );
    *orig = CgFetchTemp( handle, cgtype );
    // returned cg_name must be emitted before
    // any sequence points that use *orig
    // (i.e., returned cg_name sets up the duplicate value)
    return dup;
}


cg_name CgMakeTwoDups(          // MAKE TWO DUPLICATES
    cg_name *orig,              // - original and destination for first dup
    cg_name *second,            // - destination for second dup
    cg_type cgtype )            // - original type
{
    temp_handle handle;         // - dup. handle
    cg_name dup;                // - duplicated node

    dup = CgSaveAsTemp( &handle, *orig, cgtype );
    dup = CgFetchType( dup, cgtype );
    *orig = CgFetchTemp( handle, cgtype );
    *second = CgFetchTemp( handle, cgtype );
    // returned cg_name must be emitted before
    // any sequence points that use *orig or *second
    // (i.e., returned cg_name sets up the duplicate value)
    return dup;
}


static cg_type prcCgType(       // PROCESS A NEW CODE-GEN TYPE
    cg_type type )              // - code generation type
{
    if( ( type == TY_SINGLE )
      ||( type == TY_DOUBLE ) ) {         //***** LATER UPGRADE FOR LONG DBL
        CompFlags.float_used = true;
    }
    return type;
}


cg_type CgGetCgType(            // GET CODEGEN TYPE
    TYPE type )                 // - type
{
    TYPE basic;                 // - basic type
    cg_type cgtype;             // - codegen type

    basic = TypedefModifierRemove( type );
    if( basic->id == TYP_CLASS ) {
        if( OMR_CLASS_VAL == ObjModelArgument( basic ) ) {
            cgtype = prcCgType( CgTypeOutput( type ) );
        } else {
            cgtype = TY_POINTER;
        }
    } else {
        cgtype = prcCgType( CgTypeOutput( type ) );
    }
    return cgtype;
}


cg_type CgFuncRetnType(         // GET CG RETURN TYPE FOR A FUNCTION
    SYMBOL func )               // - function
{
    TYPE ftype;                 // - type for function
    cg_type cgtype;             // - codegen type

    ftype = FunctionDeclarationType( func->sym_type );
    if( OMR_CLASS_REF == ObjModelFunctionReturn( ftype ) ) {
        cgtype = TY_POINTER;
    } else {
        cgtype = prcCgType( CgTypeOutput( ftype->of ) );
    }
    return cgtype;
}


cg_type CgExprType(             // GET EXPRESSION TYPE
    TYPE type )                 // - C++ type
{
    return prcCgType( CgTypeOutput( type ) );
}


void CgAssign(                  // EMIT AN ASSIGNMENT
    cg_name lhs,                // - lhs argument
    cg_name rhs,                // - rhs argument
    cg_type type )              // - type for assignment
{
    CgDone( CGLVAssign( lhs, rhs, type ), TY_POINTER );
}


void CgAssignPtr(               // EMIT A POINTER ASSIGNMENT
    cg_name lhs,                // - lhs argument
    cg_name rhs )               // - rhs argument
{
    CgAssign( lhs, rhs, TY_POINTER );
}


static call_handle initDtorCall( // INITIALIZE DTOR CALL
    SYMBOL dtor )                // - DTOR to be called
{
    cg_name dtor_name;
    call_handle h;

    dtor_name = CgSymbol( dtor );
    h = CGInitCall( dtor_name, CgFuncRetnType( dtor ), (cg_sym_handle)dtor );
    return( h );
}


static void addDtorArgs(        // ADD DTOR ARGUMENTS
    call_handle handle,         // - call handle
    SYMBOL dtor,                // - destructor
    cg_name var,                // - destruction address
    unsigned cdtor )            // - CDTOR to be used
{
    cg_name expr;               // - expression for CDTOR

    expr = CGInteger( cdtor, TY_UNSIGNED );
    switch( PcCallImpl( dtor->sym_type ) ) {
      case CALL_IMPL_REV_CPP :
      case CALL_IMPL_REV_C :
        addArgument( handle, var, TY_POINTER );
        addArgument( handle, expr, TY_UNSIGNED );
        break;
      default :
        addArgument( handle, expr, TY_UNSIGNED );
        addArgument( handle, var, TY_POINTER );
        break;
    }
}


static cg_name finiDtorCall(    // COMPLETE DTOR CALL
    call_handle handle,         // - call handle
    unsigned cdtor )            // - cdtor arg to use
{
    cg_name n;

    CgBackCallGened( handle );
    n = CgFetchPtr( CGCall( handle ) );
    CallStabCdArgSet( handle, cdtor );
    return( n );
}


cg_name CgDestructSymOffset(    // CONSTRUCT DTOR CALL FOR SYMBOL+OFFSET
    FN_CTL* fctl,               // - function control
    SYMBOL dtor,                // - destructor
    SYMBOL sym,                 // - SYMBOL to be DTOR'ed
    target_offset_t offset,     // - offset from "sym"
    unsigned cdtor )            // - CDTOR to be used
{
    call_handle handle;         // - call handle
    SYMBOL trans;               // - translated symbol
    SYMBOL bound;               // - bound reference
    target_offset_t bound_off;  // - bound offset
    bool inlined;               // - true ==> inlined dtor call

    handle = initDtorCall( dtor );
    inlined = CgBackFuncInlined( dtor );
    if( inlined ) {
        CallStackPush( dtor, handle, TY_POINTER );
        IbpAdd( sym, offset, fctl );
        IbpDefineIndex( 0 );
    }
    if( IbpReference( sym, &trans, &bound, &bound_off ) ) {
        trans = bound;
        offset += bound_off;
    }
    addDtorArgs( handle, dtor, CgSymbolPlusOffset( trans, offset ), cdtor );
    if( inlined ) {
        CallStackPop();
    }
    return finiDtorCall( handle, cdtor );
}


cg_name CgDestructExpr(         // CONSTRUCT DTOR CALL FOR EXPRESSION
    SYMBOL dtor,                // - destructor
    cg_name var,                // - expression to be DTOR'ed
    unsigned cdtor )            // - CDTOR to be used
{
    call_handle handle;         // - call handle

    handle = initDtorCall( dtor );
    addDtorArgs( handle, dtor, var, cdtor );
    return finiDtorCall( handle, cdtor );
}


static cg_name cgCommaSideEffect( // CONSTRUCT COMMA/SIDE-EFFECT EXPRESSION
    cg_name lhs,                // - expression on left
    cg_name rhs,                // - expression on right
    cg_type type,               // - type of right expression
    cg_op opcode )              // - type of opcode
{
    cg_name expr;               // - result
    if( NULL == lhs ) {
        expr = rhs;
    } else if( NULL == rhs ) {
        expr = lhs;
    } else {
        expr = CGBinary( opcode, lhs, rhs, type );
    }
    return expr;
}


cg_name CgComma(                // CONSTRUCT COMMA EXPRESSION
    cg_name lhs,                // - expression on left
    cg_name rhs,                // - expression on right
    cg_type type )              // - type of right expression
{
    return cgCommaSideEffect( lhs, rhs, type, O_COMMA );
}


void CgCommaWithTopExpr(        // PUSH COMMA'D EXPRESSION WITH TOP EXPR
    cg_name expr,               // - rhs expression
    cg_type type )              // - rhs type
{
    cg_name lhs;                // - lhs expression

    if( CgExprPopGarbage() ) {
        lhs = NULL;
    } else {
        lhs = CgExprPop();
    }
    CgExprPush( CgComma( lhs, expr, type ), type );
}


cg_name CgSideEffect(           // CONSTRUCT SIDE-EFFECT EXPRESSION
    cg_name lhs,                // - expression on left
    cg_name rhs,                // - expression on right
    cg_type type )              // - type of right expression
{
#if 0
    return cgCommaSideEffect( lhs, rhs, type, O_SIDE_EFFECT );
#else
    cg_name expr;               // - result
    if( NULL == lhs ) {
        expr = rhs;
    } else if( NULL == rhs ) {
        expr = lhs;
    } else {
        temp_handle handle;     // - handle
        expr = CgSaveAsTemp( &handle, lhs, type );
        expr = CgComma( expr, rhs, type );
        expr = CgComma( expr, CgFetchTemp( handle, type ), type );
    }
    return expr;
#endif
}


// when expr is non-null, top of stack is replaced by:
//
//                      COMMA
//                       | |
//             +---------+ +-------+
//             |                   |
//           COMMA               temp
//            | |
//       +----+ +----+
//       |           |
//    ASSIGN        expr
//      | |
//    +-+ +-+
//    |     |
//  temp   top
//
void CgCommaOptional(           // EMIT OPTIONAL COMMA'ED EXPRESSION
    cg_name expr,               // - expression or NULL
    cg_type type )              // - type of expression
{
    cg_name orig;               // - original expression
    cg_type orig_type;          // - original expression type

    if( expr != NULL ) {
        if( ! CgExprPopGarbage() ) {
            orig = CgExprPopType( &orig_type );
            expr = CgSideEffect( orig, expr, type );
        }
        CgExprPush( expr, type );
    }
}


void CgCommaBefore(             // EMIT COMMA'ED EXPRESSION BEFORE
    cg_name expr,               // - expression
    cg_type type )              // - type of above expression
{
    cg_name top_expr;           // - expression on top
    cg_type top_type;           // - type on top

    top_expr = CgExprPopType( &top_type );
    if( top_expr == NULL ) {
        if( expr != NULL ) {
            CgDone( expr, type );
        }
    } else {
        CgExprPush( CgComma( expr, top_expr, top_type ), top_type );
    }
}


SYMBOL CgBackOpDelete(          // GET ADDRESIBLE OPERATOR DELETE FOR A TYPE
    TYPE type )                 // - the type
{
    SEARCH_RESULT* result;      // - lookup result
    SYMBOL op_del;              // - operator delete

    op_del = NULL;
    result = ScopeFindNaked( TypeScope( type ), CppOperatorName( CO_DELETE ) );
    if( result != NULL ) {
        op_del = ClassFunMakeAddressable( result->sym_name->name_syms );
        ScopeFreeResult( result );
    }
    return( op_del );
}


cg_name CgAssignStateVar(       // ASSIGN STATE-VAR VALUE
    SYMBOL blk,                 // - R/W Block
    SE* se,                     // - state entry
    target_offset_t offset )    // - offset of state variable
{
    return CGLVAssign( CgSymbolPlusOffset( blk, offset )
                     , CgOffset( SeStateVar( se ) )
                     , CgTypeOffset() );
}



//***********************************************************************
// Code Generation Support -- run-time calls
//***********************************************************************

void CgRtCallInit(              // SET UP A R/T CALL
    RT_DEF *def,                // - definition for call
    RTF rt_code )               // - code for run/time call
{
    SYMBOL sym;                 // - symbol for run/time call

    sym = RunTimeCallSymbol( rt_code );
    def->type = CgTypeOutput( SymFuncReturnType( sym ) );
    def->handle = CGInitCall( CgSymbol( sym )
                            , def->type
                            , (cg_sym_handle)sym );
}


void CgRtParam(                 // SET UP A PARAMETER
    cg_name expr,               // - expression gen'ed
    RT_DEF *def,                // - definition for call
    cg_type type )              // - argument type
{
    addArgument( def->handle, expr, type );
}


void CgRtParamConstOffset(      // SET UP PARAMETER: CONSTANT OFFSET
    RT_DEF *def,                // - definition for call
    unsigned value )            // - parameter value
{
    CgRtParam( CgOffset( value ), def, CgTypeOffset() );
}


void CgRtParamAddrSym(          // SET UP PARAMETER: ADDR( SYMBOL )
    RT_DEF *def,                // - definition for call
    SYMBOL sym )                // - symbol
{
    CgRtParam( CgAddrSymbol( sym ), def, TY_POINTER );
}


cg_name CgRtCallExec(           // EXECUTE R/T CALL
    RT_DEF *def )               // - definition for call
{
    CgBackCallGened( def->handle );
    return CgFetchType( CGCall( def->handle ), def->type );
}


void CgRtCallExecDone(          // EXECUTE R/T CALL, THEN DONE
    RT_DEF *def )               // - definition for call
{
    CgDone( CgRtCallExec( def ), def->type );
}


void CgRtCallExecNoArgs(        // EXECUTE R/T CALL, WITHOUT ARGUMENTS
    RTF rt_code )               // - code for run/time call
{
    RT_DEF def;                 // - call definition

    CgRtCallInit( &def, rt_code );
    CgRtCallExecDone( &def );
}



//***********************************************************************
// General support
//***********************************************************************


cg_name CgDtorStatic(           // DTOR STATIC OBJECT
    SYMBOL sym )                // - object symbol
{
    STAB_CTL sctl;              // - state-table instance
    STAB_DEFN dctl;             // - state-table definition
    RT_DEF def;                 // - control for run-time call
    SE* se;                     // - state entry
    segment_id old_seg;         // - old segment

    StabCtlInit( &sctl, &dctl );
    StabDefnInit( &dctl, DTRG_STATIC_INITLS );
#ifndef NDEBUG
    if( PragDbgToggle.dump_stab ) {
        printf( "State Table for static object: %p\n", &dctl.state_table );
    }
#endif
    sctl.rw = CgVarRw( CgbkInfo.size_rw_base + CgbkInfo.size_data_ptr, SC_STATIC );
    dctl.ro = CgVarRo( 1, SC_STATIC, NULL );
    se = SeAlloc( DTC_SYM_STATIC );
    se->base.gen = true;
    se->sym_static.sym = sym;
    se->sym_static.dtor = RoDtorFind( sym );
    se = StateTableAdd( se, &sctl );
    StabGenerate( &sctl );
    old_seg = DgSetSegSym( sctl.rw );
    CgBackGenLabelInternal( sctl.rw );
    DgInitBytes( CgbkInfo.size_data_ptr, 0 );
    DgPtrSymData( dctl.ro );
    DgOffset( 1 );
    DgPtrSymData( sym );
    BESetSeg( old_seg );
    CgRtCallInit( &def, RTF_REG_LCL );
    CgRtParamAddrSym( &def, sctl.rw );
    return( CgRtCallExec( &def ) );
}


void CgDtorAll(                 // DTOR ALL IN FUNCTION
    void )
{
    RT_DEF def;                 // - call definition

    CgRtCallInit( &def, RTF_DTOR_ALL );
#if _CPU == _AXP
    CgRtParamAddrSym( &def, FstabExcRw() );
#endif
    CgRtCallExecDone( &def );
}


void CgDtorSe(                  // DTOR UNTIL SE ENTRY
    SE* bound )                 // - bounding entry
{
    RT_DEF def;                 // - call definition

    CgRtCallInit( &def, RTF_DTOR );
    CgRtParamConstOffset( &def, SeStateOptimal( bound ) );
#if _CPU == _AXP
    CgRtParamAddrSym( &def, FstabExcRw() );
#endif
    CgRtCallExecDone( &def );
}


#if _CPU == 386

back_handle CgProfData( void )
/****************************/
{
    FN_CTL *top;

    top = FnCtlTop();
    return( top->prof_data );
}

#endif

// The following can be extended for more types, if required
//
TYPE TypeFromCgType(            // GET C++ TYPE FOR cg_type
    cg_type cgtype )            // - code-gen type
{
    TYPE type;                  // - C++ type

    switch( cgtype ) {
      case TY_UINT_1 :
        type = GetBasicType( TYP_UCHAR );
        break;
      case TY_INT_1 :
        type = GetBasicType( TYP_SCHAR );
        break;
      case TY_UINT_2 :
        type = GetBasicType( TYP_USHORT );
        break;
      case TY_INT_2 :
        type = GetBasicType( TYP_SSHORT );
        break;
      case TY_UINT_4 :
      #if( TARGET_INT == 4 )
        type = GetBasicType( TYP_UINT );
      #else
        type = GetBasicType( TYP_ULONG );
      #endif
        break;
      case TY_INT_4 :
      #if( TARGET_INT == 4 )
        type = GetBasicType( TYP_SINT );
      #else
        type = GetBasicType( TYP_SLONG );
      #endif
        break;
      case TY_INT_8 :
        type = GetBasicType( TYP_SLONG64 );
        break;
      case TY_UINT_8 :
        type = GetBasicType( TYP_ULONG64 );
        break;
      case TY_BOOLEAN :
      case TY_INTEGER :
        type = GetBasicType( TYP_SINT );
        break;
      case TY_UNSIGNED :
        type = GetBasicType( TYP_UINT );
        break;
      default :
        type = MakeInternalType( BETypeLength( cgtype ) );
        break;
    }
    return type;
}
