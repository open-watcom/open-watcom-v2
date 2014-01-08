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
* Description:  Type signatures.
*
****************************************************************************/

#include "plusplus.h"
#include "cgfront.h"
#include "cgback.h"
#include "ctexcept.h"
#include "ring.h"
#include "carve.h"
#include "cgsegid.h"
#include "pcheader.h"
#include "initdefs.h"


//************* temporary ****************

SYMBOL DefaultCtorFind(         // GET SYMBOL FOR DEFAULT CTOR
    TYPE type,                  // - class type
    TOKEN_LOCN* err_locn,       // - error location
    bool optional )             // - TRUE ==> is optional
{
    unsigned retn;              // - return from attempt to find
    SYMBOL ctor;                // - constructor

    retn = ClassDefaultCtorFind( type, &ctor, err_locn );
    switch( retn ) {
      case CNV_OK :
        break;
      case CNV_IMPOSSIBLE :
        if( optional ) break;
        SetErrLoc( err_locn );
        CErr2p( ERR_NO_DEFAULT_INIT_CTOR, type );
        ctor = NULL;
        break;
      case CNV_AMBIGUOUS :
        if( optional ) break;
        SetErrLoc( err_locn );
        CErr2p( ERR_NO_UNIQUE_DEFAULT_CTOR, type );
        ctor = NULL;
        break;
      case CNV_ERR :
        ctor = NULL;
        break;
#ifndef NDEBUG
      default :
        CFatal( "DefaultCtorFind -- impossible return" );
#endif
    }
    return ctor;
}

//************* temporary ****************

                                // Static Data
static carve_t carveTYPE_SIG;   // - allocations for TYPE_SIGs
static TYPE_SIG *type_sigs;     // - TYPE_SIGs so far

typedef struct                  // ACCINFO -- access information
{   TYPE type;                  // - class type
    TOKEN_LOCN *err_locn;       // - error location
    bool *err_occurred;         // - TRUE ==> error occurred
    SCOPE access_scope;         // - scope for access
    SCOPE class_scope;          // - scope for class
    TYPE_SIG_ACCESS acc;        // - access type (type signature)
} ACCINFO;


static void typeSigAccessVar(   // ACCESS A DTOR, DEFAULT-CTOR, COPY-CTOR
    TYPE_SIG_ACCESS acc_var,    // - access type (variable)
    SYMBOL *a_sym,              // - addr[ symbol for item accessed ]
    ACCINFO *info )             // - access information
{
    SYMBOL sym;                 // - symbol for routine
    TYPE type;                  // - class type
    TOKEN_LOCN *err_locn;       // - error location
    bool fill_out;              // - TRUE ==> fill out the entry

    sym = *a_sym;
    if( acc_var & info->acc ) {
        fill_out = ( (info->acc & TSA_FILL_OUT) != 0 );
        if( fill_out ) {
            SetCurrScope(info->class_scope);
        } else {
            SetCurrScope (info->access_scope);
        }
    } else {
        fill_out = TRUE;
        SetCurrScope(info->class_scope);
    }
    if( ( GetCurrScope() != info->class_scope ) || ( sym == NULL ) ) {
        type = info->type;
        err_locn = info->err_locn;
        switch( acc_var ) {
          case TSA_DTOR :
            sym = DtorFindLocn( type, err_locn );
            break;
          case TSA_DEFAULT_CTOR :
            sym = DefaultCtorFind( type, err_locn, fill_out );
            break;
          case TSA_COPY_CTOR :
            sym = CopyCtorFind( type, err_locn );
            break;
          default :
            DbgStmt( CFatal( "typeSigAccessVar -- impossible type" ) );
            sym = NULL;
        }
        if( sym == NULL ) {
            if( ! fill_out ) {
                *info->err_occurred = TRUE;
            }
        } else {
            sym = ClassFunMakeAddressable( sym );
            sym->flag |= SF_ADDR_TAKEN;
            if( (info->acc & TSA_NO_REF) == 0 ) {
                if( acc_var & info->acc ) {
                    sym = SymMarkRefed( sym );
                }
            }
            *a_sym = sym;
            CDtorScheduleArgRemap( sym );
        }
    }
}


static void typeSigAccess(      // HANDLE ACCESS FOR TYPE-SIGNATURE
    TYPE_SIG_ACCESS acc,        // - access type
    TYPE_SIG *sig,              // - current signature
    TOKEN_LOCN* err_locn,       // - error location for access errors
    bool *error_occurred )      // - to set error indication
{
    ACCINFO info;               // - access information

    if( (TSA_GEN & acc) == 0 ) {
        info.acc = acc;
        info.type = StructType( sig->type );
        if( info.type != NULL ) {
            info.err_occurred = error_occurred;
            info.err_locn = err_locn;
            info.access_scope = GetCurrScope();
            info.class_scope = TypeScope( info.type );
            typeSigAccessVar( TSA_DTOR,         &sig->dtor,         &info );
            typeSigAccessVar( TSA_DEFAULT_CTOR, &sig->default_ctor, &info );
            typeSigAccessVar( TSA_COPY_CTOR,    &sig->copy_ctor,    &info );
            SetCurrScope(info.access_scope);
        }
    }
}


#define typeSigHdrSize() ( sizeof( TS_HDR ) )

static unsigned typeSigNameSize(// COMPUTE SIZE OF NAME IN TYPE SIGNATURE
    THROBJ thr )                // - category of object
{
    unsigned size;              // - size of mangled name in type signature

    switch( thr ) {
      case THROBJ_SCALAR :
      case THROBJ_PTR_FUN :
      case THROBJ_CLASS :
      case THROBJ_CLASS_VIRT :
        size = CgDataPtrSize();
        break;
      case THROBJ_PTR_SCALAR :
      case THROBJ_PTR_CLASS :
      case THROBJ_REFERENCE :
      case THROBJ_VOID_STAR :
      case THROBJ_ANYTHING :
        size = 0;
        break;
      DbgDefault( "typeSigNameSize -- bad THROBJ_..." );
    }
    return size;
}


TYPE_SIG *TypeSigFind(          // FIND TYPE SIGNATURE
    TYPE_SIG_ACCESS acc,        // - access type
    TYPE type,                  // - type for signature
    TOKEN_LOCN* err_locn,       // - error location for access errors
    bool *error_occurred )      // - to set error indication
{
    TYPE_SIG *srch;             // - signature for searching
    TYPE_SIG *sig;              // - signature
    SYMBOL sym;                 // - symbol
    unsigned size;              // - size of R/O data
    NAME typesig_name;          // - name of type signature
    TYPE typesig_type;          // - type of type signature
    bool err_this_time;         // - TRUE ==> we have error
    TYPE_SIG_ACCESS acc_ind;    // - indirect access

    err_this_time = FALSE;
    type = TypeCanonicalThr( type );
    sig = NULL;
    RingIterBeg( type_sigs, srch ) {
        if( TypesSameExclude( srch->type, type, TC1_NOT_ENUM_CHAR ) ) {
            sig = srch;
            break;
        }
    } RingIterEnd( srch );
    if( sig == NULL ) {
        THROBJ thr;             // - category of object
        DbgVerify( 0 == ( acc & TSA_GEN )
                 , "TypeSigFind -- no type signature & TSA_GEN" );
        sig = RingCarveAlloc( carveTYPE_SIG, &type_sigs );
        sig->type = type;
        sig->default_ctor = NULL;
        sig->copy_ctor = NULL;
        sig->dtor = NULL;
        sig->sym = NULL;
        sig->base = NULL;
        sig->cgref = FALSE;
        sig->cggen = FALSE;
        sig->free = FALSE;
        thr = ThrowCategory( type );
        if( acc & TSA_INDIRECT ) {
            acc_ind = acc | TSA_INDIRECT_ACCESS;
        } else if( acc & TSA_INDIRECT_ACCESS ) {
            acc_ind = ( acc & ~ TSA_INDIRECT ) | TSA_INDIRECT_GEN;
        } else {
            acc_ind = acc;
        }
        switch( thr ) {
          case THROBJ_PTR_SCALAR :
          case THROBJ_PTR_CLASS :
            sig->base = TypeSigFind( acc_ind & TSA_INDIRECT
                                   , TypePointedAtModified( type )
                                   , err_locn
                                   , &err_this_time );
            size = typeSigHdrSize();
            break;
          case THROBJ_SCALAR :
          case THROBJ_PTR_FUN :
          case THROBJ_VOID_STAR :
            size = typeSigHdrSize() + SizeTargetSizeT();
            break;
          case THROBJ_REFERENCE :
            sig->base = TypeSigFind( acc_ind
                                   , TypeReference( type )
                                   , err_locn
                                   , &err_this_time );
            size = 0;
            break;
          case THROBJ_CLASS :
          case THROBJ_CLASS_VIRT :
            size = 3 * CgCodePtrSize() + CgDataPtrSize() + typeSigHdrSize();
            break;
          case THROBJ_ANYTHING :
            size = 0;
            break;
          default :
            DbgStmt( CFatal( "cgTypeSignature -- invalid throw category" ) );
            size = 0;
        }
        size += typeSigNameSize( thr );
        if( size == 0 ) {
            sym = NULL;
        } else {                // - type for TYPE SIGNATURE variable
            typesig_name = CppNameTypeSig( type );
            sym = ScopeAlreadyExists( GetFileScope(), typesig_name );
            if( sym == NULL ) {
                typesig_type = MakeInternalType( size );
                typesig_type = MakeCompilerConstCommonData( typesig_type );
                sym = SymCreateFileScope( typesig_type
                                        , SC_PUBLIC
                                        , SF_REFERENCED | SF_ADDR_TAKEN
                                        , typesig_name );
                LinkageSet( sym, "C++" );
                CgSegId( sym );
            }
        }
        sig->sym = sym;
    }
    if( err_this_time ) {
        *error_occurred = TRUE;
    } else {
        if( NULL != sig->sym ) {
            SegmentMarkUsed( sig->sym->segid );
        }
        *error_occurred = FALSE;
        typeSigAccess( acc, sig, err_locn, error_occurred );
    }
    return sig;
}


void TypeSigWalk(               // WALK THRU TYPE SIGNATURES
    void (*walker)              // - walking routine
        ( TYPE_SIG *sig ) )     // - - type signature
{
    TYPE_SIG *ts;

    RingIterBegSafe( type_sigs, ts ) {
        (*walker)( ts );
    } RingIterEndSafe( ts )
}


void TypeSigReferenced(         // EMIT CODE NOTING A TYPE-SIGNATURE REFERENCE
    TYPE_SIG *sig )             // - type signature
{
    CgFrontCodePtr( IC_EXPR_TS, sig->type );
}


void TypeSigSymOffset(          // GET SYMBOL,OFFSET FOR TYPE-SIG REFERENCE
    TYPE_SIG* sig,              // - type signature
    SYMBOL* a_sym,              // - addr[ symbol ]
    target_offset_t* a_offset ) // - addr[ offset ]
{
    target_offset_t offset;     // - offset

    if( NULL == sig->base ) {
        *a_sym = sig->sym;
        offset = offsetof( TS_HDR, hdr_actual );
    } else {
        *a_sym = sig->base->sym;
        if( NULL == TypeReference( sig->type ) ) {
            offset = offsetof( TS_HDR, hdr_ptr );
        } else {
            offset = offsetof( TS_HDR, hdr_ref );
        }
    }
    *a_offset = offset;
    DbgVerify( sig->type == NULL || *a_sym != NULL
             , "TypeSigSymOffset -- no symbol found" );
}


static void typeSigFini(        // COMPLETION OF TYPE SIGNATURING
    INITFINI* defn )            // - definition
{
    defn = defn;
    CarveDestroy( carveTYPE_SIG );
}


static void typeSigInit(        // INITIALIZATION OF TYPE SIGNATURING
    INITFINI* defn )            // - definition
{
    defn = defn;
    type_sigs = NULL;
    carveTYPE_SIG = CarveCreate( sizeof( TYPE_SIG ), 16 );
}


INITDEFN( type_signature, typeSigInit, typeSigFini )

pch_status PCHReadTypeSigs( void )
{
    TYPE_SIG *s;
    auto cvinit_t data;

    type_sigs = TypeSigMapIndex( (TYPE_SIG *)(pointer_int)PCHReadCVIndex() );
    CarveInitStart( carveTYPE_SIG, &data );
    for( ; (s = PCHReadCVIndexElement( &data )) != NULL; ) {
        PCHReadVar( *s );
        s->next = TypeSigMapIndex( s->next );
        s->base = TypeSigMapIndex( s->base );
        s->type = TypeMapIndex( s->type );
        s->sym = SymbolMapIndex( s->sym );
        s->dtor = SymbolMapIndex( s->dtor );
        s->default_ctor = SymbolMapIndex( s->default_ctor );
        s->copy_ctor = SymbolMapIndex( s->copy_ctor );
    }
    return( PCHCB_OK );
}

static void markFreeTypeSig( void *p )
{
    TYPE_SIG *s = p;

    s->free = TRUE;
}

static void saveTypeSig( void *e, carve_walk_base *d )
{
    TYPE_SIG *s = e;
    TYPE_SIG *save_next;
    TYPE_SIG *save_base;
    TYPE save_type;
    SYMBOL save_sym;
    SYMBOL save_dtor;
    SYMBOL save_default_ctor;
    SYMBOL save_copy_ctor;

    if( s->free ) {
        return;
    }
    save_next = s->next;
    s->next = TypeSigGetIndex( save_next );
    save_base = s->base;
    s->base = TypeSigGetIndex( save_base );
    save_type = s->type;
    s->type = TypeGetIndex( save_type );
    save_sym = s->sym;
    s->sym = SymbolGetIndex( save_sym );
    save_dtor = s->dtor;
    s->dtor = SymbolGetIndex( save_dtor );
    save_default_ctor = s->default_ctor;
    s->default_ctor = SymbolGetIndex( save_default_ctor );
    save_copy_ctor = s->copy_ctor;
    s->copy_ctor = SymbolGetIndex( save_copy_ctor );
    PCHWriteCVIndex( d->index );
    PCHWriteVar( *s );
    s->next = save_next;
    s->base = save_base;
    s->type = save_type;
    s->sym = save_sym;
    s->dtor = save_dtor;
    s->default_ctor = save_default_ctor;
    s->copy_ctor = save_copy_ctor;
}

pch_status PCHWriteTypeSigs( void )
{
    auto carve_walk_base data;

    PCHWriteCVIndex( (cv_index)(pointer_int)TypeSigGetIndex( type_sigs ) );
    CarveWalkAllFree( carveTYPE_SIG, markFreeTypeSig );
    CarveWalkAll( carveTYPE_SIG, saveTypeSig, &data );
    PCHWriteCVIndexTerm();
    return( PCHCB_OK );
}

TYPE_SIG *TypeSigGetIndex( TYPE_SIG *e )
{
    return( CarveGetIndex( carveTYPE_SIG, e ) );
}

TYPE_SIG *TypeSigMapIndex( TYPE_SIG *e )
{
    return( CarveMapIndex( carveTYPE_SIG, e ) );
}

pch_status PCHInitTypeSigs( bool writing )
{
    if( writing ) {
        PCHWriteCVIndex( CarveLastValidIndex( carveTYPE_SIG ) );
    } else {
        carveTYPE_SIG = CarveRestart( carveTYPE_SIG );
        CarveMapOptimize( carveTYPE_SIG, PCHReadCVIndex() );
    }
    return( PCHCB_OK );
}

pch_status PCHFiniTypeSigs( bool writing )
{
    if( ! writing ) {
        CarveMapUnoptimize( carveTYPE_SIG );
    }
    return( PCHCB_OK );
}
