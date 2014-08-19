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
#include "ring.h"
#include "name.h"
#include "pragdefn.h"
#include "reposit.h"
#include "initdefs.h"
#include "pcheader.h"

typedef enum                    // SPECIFIES RUN-TIME SYMBOL'S TYPE
{   RTS_BASE_VOID = 0x0100      // - base type: void
,   RTS_BASE_SINT = 0x0200      // - base type: SINT
,   RTS_BASE_UINT = 0x0400      // - base type: UINT
,   RTS_INLINE    = 0x0800      // - inlined by pragma
,   RTS_HANDLER   = 0x1000      // - function has system convention
,   RTS_POINTER   = 0x2000      // - make pointer to void
,   RTS_FUNCTION  = 0x4000      // - symbol is function
,   RTS_IS_THROW  = 0x8000      // - function is a C++ throw
,   RTS_NO_THROW  = 0x0001      // - never does throw or equivalent
,   RTS_CAN_THROW = 0x0002      // - could throw
,   RTS_IG_THROW  = 0x0004      // - throwing to be ignored
} RTS_TYPE;

// function name array

static const char *runTimeCodeString[] = {
    #define QSTRING( name ) __STR( name )
    #define RTFUN( code, name ) QSTRING(CPPLIB(name))
    #define RTDAT( code, name ) QSTRING(CPPLIBDATA(name))
    #define RTFNC( code, name ) QSTRING(name)
    #define RTFNP( code, name ) #name

    #include "_rtfuns.h"

    #undef QSTRING
    #undef RTFUN
    #undef RTDAT
    #undef RTFNC
    #undef RTFNP
};

static NAME runTimeCodeName[ ARRAY_SIZE( runTimeCodeString ) ];

static SYMBOL rtSymbolLookup(   // LOOKUP RUN-TIME SYMBOL IN FILE SCOPE
    NAME name )                 // - name of run-time function
{
    SEARCH_RESULT *result;      // - lookup result
    SYMBOL sym;                 // - symbol for lookup

    result = ScopeFindNaked( GetInternalScope(), name );
    if( result == NULL ) {
        sym = NULL;
    } else {
        sym = result->sym_name->name_syms;
        ScopeFreeResult( result );
    }
    return sym;
}


// make sure to get a predefined type so that the type system is not
// exercised in the back end
//
static SYMBOL rtSymbolCreate(   // CREATE NEW RUN-TIME SYMBOL
    RTS_TYPE runtime_type,      // - run-time type definition
    NAME name )                 // - name of run-time function
{
    SYMBOL sym;                 // - new symbol
    TYPE sym_type;              // - symbol's type
    symbol_flag flags;          // - symbol's flags

    flags = SF_REFERENCED;
    if( runtime_type & RTS_FUNCTION ) {
        if( runtime_type & RTS_POINTER ) {
            sym_type = TypePtrVoidFunOfVoid();
        } else if( runtime_type & RTS_HANDLER ) {
            sym_type = TypeVoidHandlerFunOfVoid();
        } else {
            sym_type = TypeVoidFunOfVoid();
        }
        if( runtime_type & RTS_INLINE ) {
            sym_type = AddFunctionFlag( sym_type, TF1_INTRINSIC );
        }
        if( runtime_type & RTS_CAN_THROW ) {
            flags |= SF_LONGJUMP;
        } else if( runtime_type & RTS_NO_THROW ) {
            flags |= SF_NO_LONGJUMP;
        } else if( runtime_type & RTS_IG_THROW ) {
            RepoFunAdd( name, RFFLAG_IG_LONGJUMP );
        }
        if( runtime_type & RTS_IS_THROW ) {
            flags |= SF_IS_THROW;
        }
    } else if( runtime_type & RTS_BASE_VOID ) {
        if( runtime_type & RTS_POINTER ) {
            sym_type = TypePtrToVoid();
        } else {
            sym_type = GetBasicType( TYP_VOID );
        }
    } else {
        sym_type = GetBasicType( TYP_SINT );
    }
    sym = SymCreate( sym_type, SC_EXTERN, flags, name, GetInternalScope() );
    LinkageSet( sym, "C" );
    return sym;
}


bool RunTimeIsThrow(            // TEST IF FUNCTION IS A C++ THROW
    SYMBOL func )               // - function symbol
{
    return GetInternalScope() == SymScope( func ) && ( func->flag & SF_IS_THROW ) != 0;
}


static NAME getRTCName( RTF code )
{
    NAME name;

    name = runTimeCodeName[ code ];
    if( name == NULL ) {
        name = NameCreateNoLen( runTimeCodeString[ code ] );
        runTimeCodeName[ code ] = name;
    }
    return( name );
}


SYMBOL RunTimeCallSymbol(       // GET SYMBOL FOR A RUN-TIME CALL
    RTF code )                  // - code for call
{
    SYMBOL sym;                 // - symbol for run-time call
    NAME name;                  // - name for function
    RTS_TYPE runtime;           // - definition for run-time symbol

    name = getRTCName( code );
    sym = rtSymbolLookup( name );
    if( sym == NULL ) {
        switch( code ) {
          case RTF_ASSIGN_ARR :
          case RTF_COPY_ARR :
          case RTF_COPY_VARR :
          case RTF_CTOR_ARR :
          case RTF_CTOR_VARR :
          case RTF_DEREGISTER :
          case RTF_DTOR_ARR :
          case RTF_CTAS_1S :
          case RTF_CTAS_1M :
          case RTF_CTAS_2S :
            runtime = RTS_BASE_VOID | RTS_POINTER | RTS_FUNCTION | RTS_IG_THROW;
            break;
          case RTF_DYN_CAST_PTR :
          case RTF_DYN_CAST_VOID :
            runtime = RTS_BASE_VOID | RTS_POINTER | RTS_FUNCTION | RTS_NO_THROW;
            break;
          case RTF_CTAS_GM :
          case RTF_CTAS_GS :
          case RTF_DTOR_AR_STORE :
          case RTF_DYN_CAST_REF :
          case RTF_GET_TYPEID :
            runtime = RTS_BASE_VOID | RTS_POINTER | RTS_FUNCTION | RTS_CAN_THROW;
            break;
          case RTF_STATIC_INIT :
            runtime = RTS_BASE_SINT | RTS_FUNCTION | RTS_NO_THROW;
            break;
#if _CPU == _AXP
          case RTF_PD_HANDLER :
          case RTF_PD_HANDLER_RTN :
#else
          case RTF_FS_HANDLER :
          case RTF_FS_HANDLER_RTN :
#endif
            runtime = RTS_BASE_SINT | RTS_FUNCTION | RTS_HANDLER | RTS_NO_THROW;
            break;
          case RTF_SETJMP :
            runtime = RTS_BASE_UINT | RTS_FUNCTION | RTS_CAN_THROW;
            break;
          case RTF_MOD_DTOR :
//        case RTF_INLINE_FREG :    // not req'd with new library
          case RTF_LONGJMP_REF :
          case RTF_UNDEF_DATA :
          case RTD_FS_ROOT :
          case RTD_TS_GENERIC :
          case RTD_TS_OS2 :
          case RTD_TS_NT :
            runtime = RTS_BASE_SINT;
            break;
          case RTF_THROW :
          case RTF_THROW_ZERO :
          case RTF_RETHROW :
            runtime = RTS_BASE_VOID | RTS_FUNCTION | RTS_IS_THROW | RTF_THROW;
            // it is absolutely critical that the function definition
            // in the runtime library be #pragma aborts because otherwise
            // -3s code in the runtime lib assumes a return address was
            // pushed (AFS 29-jul-93)
            break;
          case RTF_FS_PUSH :
          case RTF_FS_POP :
            runtime = RTS_BASE_VOID | RTS_FUNCTION | RTS_INLINE;
            break;
          default :
            runtime = RTS_BASE_VOID | RTS_FUNCTION | RTS_NO_THROW;
            break;
        }
        sym = rtSymbolCreate( runtime, name );
    }
    return sym;
}


PTREE RunTimeCall(              // GENERATE A RUN-TIME CALL PARSE SUBTREE
    PTREE expr,                 // - expression for operands
    TYPE type,                  // - type for function return
    RTF code )                  // - code for function
{
    SYMBOL func;

    func = RunTimeCallSymbol( code );
    DbgVerify( (PointerTypeEquivalent( type ) == NULL)
               == (PointerTypeEquivalent( SymFuncReturnType( func ) ) == NULL)
             , "RunTimeCall -- return type mismatch" );
    return NodeMakeCall( func, type, expr );
}


const char *RunTimeCodeString(  // GET IMPORT STRING FOR RUN-TIME FUNCTION FROM RTF CODE
    RTF code )                  // - code for function
{
    return( runTimeCodeString[code] );
}

static void rtfInit(            // INITIALIZE NAMES FOR NAMES PROCESSING
    INITFINI* defn )            // - definition
{
    defn = defn;
    if( CompFlags.dll_subsequent ) {
        memset( (void *)runTimeCodeName, 0, sizeof( runTimeCodeName ) );
    }
}

INITDEFN( rtf_names, rtfInit, InitFiniStub )

pch_status PCHReadRTFNames( void )
{
    NAME *name;

    for( name = runTimeCodeName; name < &runTimeCodeName[ RTF_LAST ]; ++name ) {
        *name = NamePCHRead();
    }
    return( PCHCB_OK );
}

pch_status PCHWriteRTFNames( void )
{
    NAME *name;

    for( name = runTimeCodeName; name < &runTimeCodeName[ RTF_LAST ]; ++name ) {
        NamePCHWrite( *name );
    }
    return( PCHCB_OK );
}

pch_status PCHInitRTFNames( bool writing )
{
    writing = writing;
    return( PCHCB_OK );
}

pch_status PCHFiniRTFNames( bool writing )
{
    writing = writing;
    return( PCHCB_OK );
}
