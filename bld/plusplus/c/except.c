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
#include "ctexcept.h"
#include "errdefns.h"
#include "ring.h"
#include "rtfuncod.h"
#include "typesig.h"

#define TSA_THROW (TSA_DTOR | TSA_COPY_CTOR )
#define TSA_NULL  0


static TYPE throwCnvTypeRetn(   // RETURN NEXT THROW TYPE
    THROW_CNV_CTL *ctl,         // - control area
    target_size_t *offset,      // - addr[ offset ]
    THROW_CNV *cnv )            // - conversion entry
{
    ctl->cur = cnv;
    *offset = cnv->offset;
    return cnv->sig->type;
}


TYPE ThrowCnvType(              // THROW CONVERSIONS: GET NEXT TYPE
    THROW_CNV_CTL *ctl,         // - control area
    target_size_t *offset )     // - addr[ offset ]
{
    THROW_CNV *cnv;             // - conversion entry
    TYPE type;                  // - type of conversion entry

    cnv = ctl->cur;
    if( cnv == NULL ) {
        cnv = ctl->hdr;
        if( cnv == NULL ) {
            type = NULL;
        } else {
            type = throwCnvTypeRetn( ctl, offset, cnv->next );
        }
    } else if( cnv == ctl->hdr ) {
        type = NULL;
    } else {
        type = throwCnvTypeRetn( ctl, offset, cnv->next );
    }
    return type;
}


THROBJ ThrowCategory(           // GET THROW-OBJECT CATEGORY FOR A TYPE
    TYPE type )                 // - type
{
    TYPE cltype;                // - class type or NULL
    THROBJ retn;                // - category of object

    if( type == NULL ) {
        retn = THROBJ_ANYTHING;
    } else if( NULL != FunctionDeclarationType( type ) ) {
        retn = THROBJ_PTR_FUN;
    } else if( NULL != TypeReference( type ) ) {
        retn = THROBJ_REFERENCE;
    } else if( NULL != (cltype = StructType( type ) ) ) {
        if( cltype->u.c.info->last_vbase == 0 ) {
            retn = THROBJ_CLASS;
        } else {
            retn = THROBJ_CLASS_VIRT;
        }
    } else {
        type = PointerTypeEquivalent( type );
        if( type == NULL ) {
            retn = THROBJ_SCALAR;
        } else if( StructType( type->of ) ) {
            retn = THROBJ_PTR_CLASS;
        } else {
            type = TypedefModifierRemove( type->of );
            if( type->id == TYP_VOID ) {
                retn = THROBJ_VOID_STAR;
            } else if( type->id == TYP_FUNCTION ) {
                retn = THROBJ_PTR_FUN;
            } else {
                retn = THROBJ_PTR_SCALAR;
            }
        }
    }
    return retn;
}


static void makeThrowCnvAccess( // MAKE A THROW CONVERSION, WITH ACCESS
    THROW_CNV_CTL *ctl,         // - control area
    TYPE type,                  // - conversion type
    target_size_t offset,       // - conversion offset
    TYPE_SIG_ACCESS access )    // - type of access
{
    THROW_CNV *cnv;             // - conversion entry
    boolean err_occurred;       // - set to TRUE if error occurred

    cnv = RingCarveAlloc( ctl->carver, &ctl->hdr );
    cnv->offset = offset;
    cnv->sig = TypeSigFind( access, type, &ctl->err_locn, &err_occurred );
    if( err_occurred ) {
        ctl->error_occurred = TRUE;
    }
}


static void makeThrowCnv(       // MAKE A THROW CONVERSION, DEFAULT ACCESS
    THROW_CNV_CTL *ctl,         // - control area
    TYPE type,                  // - conversion type
    target_size_t offset )      // - conversion offset
{
    if( CompFlags.codegen_active ) {
        makeThrowCnvAccess( ctl, type, offset, TSA_NULL );
    } else {
        makeThrowCnvAccess( ctl, type, offset, TSA_THROW );
    }
}


static void makeThrowCnvNoAcc(  // MAKE A THROW CONVERSION, NO ACCESS
    THROW_CNV_CTL *ctl,         // - control area
    TYPE type,                  // - conversion type
    target_size_t offset )      // - conversion offset
{
    makeThrowCnvAccess( ctl, type, offset, TSA_NULL );
}


target_size_t ThrowBaseOffset(  // GET OFFSET OF BASE
    SCOPE thr_scope,            // - scope of throw
    SCOPE base_scope )          // - scope for base
{
    SEARCH_RESULT *result;      // - search result for valid type
    target_size_t offset;       // - exact offset of base

    result = ScopeBaseResult( thr_scope, base_scope );
    offset = result->exact_delta;
    ScopeFreeResult( result );
    return offset;
}


static boolean validateBase(    // VALIDATE BASE CLASS OK
    SCOPE base_scope,           // - scope for base class
    THROW_CNV_CTL *ctl )        // - control area
{
    boolean retn;               // - TRUE ==> generate conversion
    SCOPE thr_scope;            // - scope for throw

    thr_scope = TypeScope( ctl->src_type );
    switch( ScopeDerived( thr_scope, base_scope ) ) {
      DbgDefault( "validateBase -- impossible derived type" );
      case DERIVED_YES :
      case DERIVED_YES_BUT_VIRTUAL :
        ctl->offset = ThrowBaseOffset( thr_scope, base_scope );
        retn = TRUE;
        break;
      case DERIVED_YES_BUT_AMBIGUOUS :
      case DERIVED_YES_BUT_PRIVATE :
      case DERIVED_YES_BUT_PROTECTED :
        retn = FALSE;
        break;
    }
    return retn;
}


static void throwBaseCnv(       // CONVERSION TO A BASE CLASS
    SCOPE base,                 // - scope for base class
    void* dat )                 // - control area
{
    THROW_CNV_CTL *ctl;         // - control area

    ctl = dat;
    if( validateBase( base, ctl ) ) {
        SCOPE curr_scope = CurrScope;
        if( base->owner.type != ctl->src_type ) {
            CurrScope = base;
        }
        makeThrowCnv( ctl, ScopeClass( base ), ctl->offset );
        CurrScope = curr_scope;
    }
}


static void throwBasePtrCnv(    // CONVERSION TO A PTR. TO BASE CLASS
    SCOPE base,                 // - scope for base class
    void *dat )                 // - control area
{
    THROW_CNV_CTL *ctl;         // - control area

    ctl = dat;
    if( validateBase( base, ctl ) ) {
        makeThrowCnv( ctl, MakePointerTo( ScopeClass( base ) ), ctl->offset );
    }
}


static void throwClassCnvs(     // MAKE CONVERSIONS FOR A CLASS
    TYPE type,                  // - class type
    THROW_CNV_CTL *ctl )        // - control area
{
    ctl->src_type = type;
    ScopeWalkAncestry( type->u.c.scope, &throwBaseCnv, ctl );
}


static void makeCnvVoidStar(    // MAKE A CONVERSION TO void*, MODIFIED void*
    THROW_CNV_CTL *ctl )        // - control area
{
    makeThrowCnvNoAcc( ctl, TypePtrToVoid(), 0 );
}


static void throwClassPtrCnvs(  // MAKE CONVERSIONS FOR A PTR TO CLASS
    TYPE type,                  // - class type
    THROW_CNV_CTL *ctl )        // - control area
{
    ctl->src_type = type;
    ScopeWalkAncestry( type->u.c.scope, &throwBasePtrCnv, ctl );
}


unsigned ThrowCnvInit(          // THROW CONVERSIONS: INITIALIZE
    THROW_CNV_CTL *ctl,         // - control area
    TYPE type )                 // - type thrown
{
    type_flag not_used;         // - not used
    THROBJ thr_obj;             // - type of throw object

    ctl->carver = CarveCreate( sizeof( THROW_CNV ), 8 );
    ctl->hdr = NULL;
    ctl->cur = NULL;
    type = TypeCanonicalThr( type );
    ctl->error_occurred = FALSE;
    thr_obj = ThrowCategory( type );
    if( thr_obj == THROBJ_REFERENCE ) {
        makeThrowCnvNoAcc( ctl, type, 0 );
    } else {
        makeThrowCnv( ctl, type, 0 );
    }
    switch( thr_obj ) {
      case THROBJ_VOID_STAR :
      case THROBJ_ANYTHING :
      case THROBJ_SCALAR :
        break;
      case THROBJ_PTR_SCALAR :
        makeCnvVoidStar( ctl );
        break;
      case THROBJ_REFERENCE :
        type = TypeReference( type );
        if( NULL != StructType( type ) ) {
            throwClassCnvs( type, ctl );
        } else {
            type = TypePointedAt( type, &not_used );
            if( type != NULL ) {
                type = StructType( type );
                if( type != NULL ) {
                    throwClassPtrCnvs( type, ctl );
                }
                makeCnvVoidStar( ctl );
            }
        }
        break;
      case THROBJ_CLASS :
      case THROBJ_CLASS_VIRT :
        throwClassCnvs( type, ctl );
        break;
      case THROBJ_PTR_CLASS :
        throwClassPtrCnvs( type, ctl );
        makeCnvVoidStar( ctl );
        break;
      case THROBJ_PTR_FUN :
        if( CgCodePtrSize() <= CgDataPtrSize() ) {
            makeCnvVoidStar( ctl );
        }
        break;
      DbgDefault( "EXCEPT -- illegal throw object" );
    }
    return RingCount( ctl->hdr );
}


void ThrowCnvFini(              // THROW CONVERSIONS: COMPLETION
    THROW_CNV_CTL *ctl )        // - control area
{
    CarveDestroy( ctl->carver );
}


static boolean throwCnvFront(   // GET FRONT-END INFO. FOR THROW TYPE
    TYPE type,                  // - type to be thrown
    PTREE expr )                // - expression for errors
{
    boolean ret;                // - return value
    THROW_CNV_CTL ctl;          // - control information

    PTreeExtractLocn( expr, &ctl.err_locn );
    ThrowCnvInit( &ctl, type );
    ret = ! ctl.error_occurred;
    ThrowCnvFini( &ctl );
    return ret;
}


static TYPE canonicalBaseType(  // GET CANONICAL BASE TYPE
    TYPE type )                 // - type
{
    return TypedefModifierRemoveOnly( type );
}


static TYPE canonicalPtrType(   // GET CANONICAL PTR TYPE
    TYPE type )                 // - type
{
    TYPE test;                  // - used to test type

    test = PointerTypeEquivalent( type );
    if( test == NULL ) {
        type = canonicalBaseType( type );
    } else if( FunctionDeclarationType( test ) ) {
        type = MakePointerTo( canonicalBaseType( test ) );
    } else {
        type = MakePointerTo( canonicalBaseType( test->of ) );
    }
    return type;
}


TYPE TypeCanonicalThr(          // GET CANONICAL THROW TYPE
    TYPE type )
{
    TYPE test;                  // - used to test type

    if( type != NULL ) {
        test = TypeReference( type );
        if( test == NULL ) {
            type = canonicalPtrType( type );
        } else {
            type = MakeReferenceTo( canonicalPtrType( test ) );
        }
    }
    return type;
}


PTREE ThrowTypeSig(             // GET THROW ARGUMENT FOR TYPE SIGNATURE
    TYPE type,                  // - type to be thrown
    PTREE expr )                // - expression for errors
{
    PTREE node;                 // - node constructed

    type = TypeCanonicalThr( type );
    if( throwCnvFront( type, expr ) ) {
        node = NodeUnary( CO_TYPE_SIG, NULL );
        node->type = type;
        node = NodeArg( node );
        node->type = type;
    } else {
        node = NULL;
    }
    return node;
}


void ExceptionsCheck(           // DIAGNOSE EXCEPTIONAL USE, WHEN DISABLED
    void )
{
    if( ! CompFlags.excs_enabled ) {
        CErr1( ERR_EXCEPTIONS_DISABLED );
    }
}
