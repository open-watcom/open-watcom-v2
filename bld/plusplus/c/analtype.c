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
#include "codegen.h"


static TYPE makePointerToModType( // MAKE POINTER TO [OPTIONALLY MODIFIED] TYPE
    TYPE type,                  // - base type
    type_flag flags )           // - flags
{
    if( flags != 0 ) {
        type = MakeModifiedType( type, flags );
    }
    return MakePointerTo( type );
}


TYPE TypeReferenced(            // GET TYPE OR TYPE REFERENCED
    TYPE type )                 // - original type
{
    TYPE refed;                 // - referenced type

    refed = TypeReference( type );
    if( refed == NULL ) {
        refed = type;
    }
    return refed;
}


static TYPE typeTested(         // TEST FOR TYPE PAST MODIFIERS, REFERENCE
    TYPE type,                  // - input type
    type_id id )                // - requested id
{
    if( type != NULL ) {
        type = TypeReferenced( type );
        type = TypedefModifierRemoveOnly( type );
        if( id != type->id ) {
            type = NULL;
        }
    }
    return type;
}


TYPE ClassTypeForType(          // GET CLASS TYPE FOR TYPE OR REFERENCE TO IT
    TYPE type )                 // - input type
{
    return typeTested( type, TYP_CLASS );
}


TYPE EnumTypeForType(           // GET ENUM TYPE FOR TYPE OR REFERENCE TO IT
    TYPE type )                 // - input type
{
    return typeTested( type, TYP_ENUM );
}


TYPE UserDefTypeForType(        // GET ENUM or CLASS TYPE FOR TYPE OR REFERENCE TO IT
    TYPE type )                 // - input type
{
    TYPE retn;

    retn = ClassTypeForType( type );
    if( retn == NULL ) {
        retn = EnumTypeForType( type );
    }
    return retn;
}


CLASSINFO *TypeClassInfo(       // GET CLASSINFO PTR FOR A TYPE
    TYPE type )                 // - input type
{
    CLASSINFO *info;            // - return: NULL or CLASSINFO PTR.

    type = ClassTypeForType( type );
    if( type == NULL ) {
        info = NULL;
    } else {
        info = type->u.c.info;
    }
    return info;
}

static TYPE getThisBaseType( SYMBOL sym )
{
    SCOPE scope;                // - scope for symbol
    TYPE class_type;            // - type from function's class scope
    TYPE base;                  // - return: NULL or TYPE of "this" for symbol

    base = NULL;
    if( sym != NULL ) {
        scope = ScopeNearestNonTemplate( SymScope( sym ) );
        if( scope != NULL ) {
            class_type = ScopeClass( scope );
            if( class_type != NULL ) {
                if( sym->id != SC_STATIC ) {
                    base = class_type;
                }
            }
        }
    }
    return base;
}


TYPE TypeThisSymbol(            // GET TYPE OF THIS FOR SYMBOL MEMBER
    SYMBOL sym,                 // - symbol
    boolean reference )         // - use reference?
{
    TYPE base;                  // - return: NULL or TYPE of "this" for symbol
    type_flag flags;            // - flags for "this" pointer

    base = getThisBaseType( sym );
    if( base != NULL ) {
        flags = FunctionThisFlags( sym );
        base = MakeModifiedType( base, flags );
        if( reference ) {
            base = MakeReferenceTo( base );
        } else {
            base = MakePointerTo( base );
        }
        base = MakeModifiedType( base, TF1_CONST );
    }
    return( base );
}


TYPE TypeThis(                  // GET TYPE OF THIS FOR MEMBER BEING DEFINED
    void )
{
    return TypeThisSymbol( ScopeFunctionInProgress(), FALSE );
}


TYPE TypeThisExists(            // GET BASE TYPE OF THIS FOR MEMBER BEING DEFINED
    void )
{
    return getThisBaseType( ScopeFunctionInProgress() );
}


TYPE TypeMergeForMember(        // CREATE MERGED TYPE FOR A MEMBER
    TYPE owner,                 // - type on left (.), type pointed at (->)
    TYPE member )               // - type for member
{
    type_flag owner_flags;      // - modifier flags for owner
    type_flag member_flags;     // - modifier flags for member
    type_flag result_flags;     // - modifier flags for result
    void *owner_base;           // - base for owner

    TypeModExtract( owner
                  , &owner_flags
                  , &owner_base
                  , TC1_NOT_ENUM_CHAR );
    member = TypeGetActualFlags( member, &member_flags );
    if( owner_flags & TF1_CONST ) {
        if( member_flags & TF1_MUTABLE ) {
            owner_flags &= ~TF1_CONST;
        }
    }
    result_flags = owner_flags | member_flags;
    if( result_flags != TF1_NULL ) {
        member = MakeBasedModifierOf( member, result_flags, owner_base );
    }
    return( member );
}


target_size_t ArrayTypeNumberItems( // GET ACTUAL NUMBER OF ITEMS FOR AN ARRAY
    TYPE artype )               // - an array type
{
    target_size_t count;        // - counts number of items

    for( count = 1; ; artype = artype->of ) {
        artype = TypedefModifierRemove( artype );
        if( artype->id != TYP_ARRAY ) break;
        count *= artype->u.a.array_size;
    }
    return count;
}


TYPE TypeTargetSizeT(           // GET TYPE OF TARGET'S size_t
    void )
{
    TYPE type;                  // - return type

    if( IsHugeData() ) {
        type = GetBasicType( TYP_ULONG );
    } else {
        type = GetBasicType( TYP_UINT );
    }
    return type;
}


unsigned SizeTargetSizeT(       // GET SIZE OF TARGET'S size_t
    void )
{
    unsigned size;              // - size of type

    if( IsHugeData() ) {
        size = TARGET_ULONG;
    } else {
        size = TARGET_UINT;
    }
    return size;
}


boolean TypeTruncByMemModel(    // TEST TYPE TRUNCATION FOR DEF. MEMORY MODEL
    TYPE type )                 // - the type
{
    boolean retn;               // - TRUE ==> type doesn't matches default mem. model
    type_flag flags;            // - flags for the type
    type_flag mflags;           // - memory-model flags for the type

    type = TypeGetActualFlags( type, &flags );
    mflags = flags & TF1_MEM_MODEL;
    if( 0 == mflags ) {
        retn = FALSE;
    } else {
        if( type->id == TYP_FUNCTION ) {
            retn = ( !IsBigCode() && (TF1_NEAR & mflags) == 0 );
        } else {
            retn = ( !IsBigData() && (TF1_NEAR & mflags) == 0 );
        }
    }
    return retn;
}


TYPE TypeRebuildPcPtr(          // REBUILD PC-PTR TYPE
    TYPE type,                  // - type of element pointed at (unmodified)
    type_flag old_flags,        // - old flags
    type_flag new_flags )       // - new flags
{
    return makePointerToModType( type
                               , ( old_flags & ~TF1_MEM_MODEL ) | new_flags );
}


TYPE TypeSegOp(                 // GET TYPE FOR :> OPERATION
    TYPE type )                 // - type of RHS of :>
{
    type_flag flags;            // - flags for type

    type = TypePointedAt( type, &flags );
    return TypeRebuildPcPtr( type, flags, TF1_FAR );
}


TYPE TypeSegId(                 // GET TYPE OF SEGMENT ID
    void )
{
    return TypeSegmentShort();
}


TYPE TypeSegAddr(               // GET INTERNAL TYPE OF BASE :> ADDRESS
    void )
{
    TYPE type;                  // - resultant type

    type = GetBasicType( TYP_VOID );
    return makePointerToModType( type, TF1_NEAR );
}


boolean TypeIsBasedPtr(         // SEE IF A PTR TO BASED ITEM
    TYPE type )                 // - the type
{
    boolean retn;               // - FALSE ==> not based
    type_flag flags;            // - flags for item pointed at

    if( NULL == TypePointedAt( type, &flags ) ) {
        retn = FALSE;
    } else {
        retn = flags & TF1_BASED;
    }
    return retn;
}


PC_PTR TypePcPtr(               // CLASSIFY PTR FOR PC
    type_flag flag )            // - modifier flags for pointed item
{
    PC_PTR classification;      // - the classification

    if( flag & TF1_FAR16 ) {
        classification = PC_PTR_FAR16;
    } else {
        classification = ( flag & TF1_BASED ) >> 8;
    }
    return classification;
}


TYPE TypeConvertFromPcPtr(      // TRANSFORM TYPE AFTER CONVERSION FROM PC PTR
    TYPE ptype )                // - pointer type
{
    type_flag flags;            // - flags for item pointed at
    TYPE pted;                  // - type pointed at (modified)
    TYPE type;                  // - type pointed at (unmodified)
    SYMBOL baser;               // - basing infomation

    pted = TypePointedAtModified( ptype );
    type = TypeModExtract( pted, &flags, &baser, TC1_NOT_ENUM_CHAR );
    switch( flags & TF1_BASED ) {
      case 0 :
        if( flags & TF1_FAR16 ) {
            ptype = TypeRebuildPcPtr( type, flags, DefaultMemoryFlag( type ) );
        }
        break;
      case TF1_BASED_VOID :
        ptype = TypeRebuildPcPtr( type, flags, TF1_NEAR );
        break;
      case TF1_BASED_ADD :
      { type_flag bflags;       // - flags for baser
        TypePointedAt( baser->sym_type, &bflags );
        ptype = TypeRebuildPcPtr( type, flags, bflags & TF1_MEM_MODEL );
      } break;
      case TF1_BASED_SELF :
        ptype = TypeRebuildPcPtr( type, flags, TF1_FAR );
        break;
      case TF1_BASED_FETCH :
        ptype = TypeRebuildPcPtr( type, flags, TF1_FAR );
        break;
      case TF1_BASED_STRING :
        ptype = TypeRebuildPcPtr( type, flags, TF1_FAR );
        break;
    }
    return ptype;
}

#if _CPU == 8086
    #define CNV( opI86, op386 ) opI86
#else
    #define CNV( opI86, op386 ) op386
#endif

#define ENTRY_ERROR         TYP_ERROR,
#define ENTRY_BOOL          TYP_SINT,
#define ENTRY_CHAR          TYP_SINT,
#define ENTRY_SCHAR         TYP_SINT,
#define ENTRY_UCHAR         TYP_SINT,
#define ENTRY_WCHAR         CNV( TYP_UINT, TYP_SINT ),
#define ENTRY_SSHORT        TYP_SINT,
#define ENTRY_USHORT        CNV( TYP_UINT, TYP_SINT ),
#define ENTRY_SINT          TYP_SINT,
#define ENTRY_UINT          TYP_UINT,
#define ENTRY_SLONG         TYP_SLONG,
#define ENTRY_ULONG         TYP_ULONG,
#define ENTRY_SLONG64       TYP_SLONG64,
#define ENTRY_ULONG64       TYP_ULONG64,
#define ENTRY_FLOAT         TYP_FLOAT,
#define ENTRY_DOUBLE        TYP_DOUBLE,
#define ENTRY_LONG_DOUBLE   TYP_LONG_DOUBLE,
#define ENTRY_ENUM          TYP_ERROR,
#define ENTRY_POINTER       TYP_ERROR,
#define ENTRY_TYPEDEF       TYP_ERROR,
#define ENTRY_CLASS         TYP_ERROR,
#define ENTRY_BITFIELD      TYP_ERROR,
#define ENTRY_FUNCTION      TYP_ERROR,
#define ENTRY_ARRAY         TYP_ERROR,
#define ENTRY_DOT_DOT_DOT   TYP_ERROR,
#define ENTRY_VOID          TYP_ERROR,
#define ENTRY_MODIFIER      TYP_ERROR,
#define ENTRY_MEMBER_POINTER TYP_ERROR,
#define ENTRY_GENERIC       TYP_ERROR,

static type_id intPromo[] = {   // Table of integral promotions
    #include "type_arr.h"
};

#undef CNV


static type_id integralPromote( // GET type_id AFTER INTEGRAL PROMOTION
    TYPE type )                 // - original type
{
    return intPromo[ TypedefModifierRemove( type ) -> id ];
}


TYPE TypeBinArithResult(        // TYPE OF BINARY ARITHMETIC RESULT
    TYPE op1,                   // - type[1]
    TYPE op2 )                  // - type[2]
{
    type_id id1;                // - id for type(1)
    type_id id2;                // - id for type(2)

    id1 = integralPromote( op1 );
    id2 = integralPromote( op2 );
    if( id1 > id2 ) {
        type_id tmp;
        tmp = id1;
        id1 = id2;
        id2 = tmp;
    }
#if _CPU != 8086
    if( ( id1 == TYP_UINT ) && ( id2 == TYP_SLONG ) ) {
        id2 = TYP_ULONG;
    }
#endif
    return GetBasicType( id2 );
}


TYPE TypeUnArithResult(         // TYPE OF UNARY ARITHMETIC RESULT
    TYPE op1 )                  // - type
{
    return GetBasicType( integralPromote( op1 ) );
}

TYPE PointerTypeEquivalent( TYPE type )
/*************************************/
{
    type = TypedefModifierRemove( type );
    switch( type->id ) {
    case TYP_POINTER:
        /* this allows references also */
    case TYP_ARRAY:
    case TYP_FUNCTION:
        return( type );
    }
    return( NULL );
}


CTD TypeCommonDerivation(       // GET COMMON TYPE DERIVATION FOR TWO TYPES
    TYPE type1,                 // - type [1] (left)
    TYPE type2 )                // - type [2] (right)
{
    CTD retn;                   // - return: CTD_...
    SCOPE scope1;               // - scope for type[1]
    SCOPE scope2;               // - scope for type[2]

    retn = CTD_NO;
    scope1 = TypeScope( StructType( type1 ) );
    if( NULL != scope1 ) {
        scope2 = TypeScope( StructType( type2 ) );
        if( scope1 == scope2 ) {
            retn = CTD_LEFT;
        } else if( NULL != scope2 ) {
            switch( ScopeDerived( scope1, scope2 ) ) {
              case DERIVED_YES :
                retn = CTD_LEFT;
                break;
              case DERIVED_YES_BUT_VIRTUAL :
                retn = CTD_LEFT_VIRTUAL;
                break;
              case DERIVED_YES_BUT_AMBIGUOUS :
                retn = CTD_LEFT_AMBIGUOUS;
                break;
              case DERIVED_YES_BUT_PRIVATE :
                retn = CTD_LEFT_PRIVATE;
                break;
              case DERIVED_YES_BUT_PROTECTED :
                retn = CTD_LEFT_PROTECTED;
                break;
              case DERIVED_NO :
                switch( ScopeDerived( scope2, scope1 ) ) {
                  case DERIVED_YES :
                    retn = CTD_RIGHT;
                    break;
                  case DERIVED_YES_BUT_VIRTUAL :
                    retn = CTD_RIGHT_VIRTUAL;
                    break;
                  case DERIVED_YES_BUT_AMBIGUOUS :
                    retn = CTD_RIGHT_AMBIGUOUS;
                    break;
                  case DERIVED_YES_BUT_PRIVATE :
                    retn = CTD_RIGHT_PRIVATE;
                    break;
                  case DERIVED_YES_BUT_PROTECTED :
                    retn = CTD_RIGHT_PROTECTED;
                    break;
                  case DERIVED_NO :
                    retn = CTD_NO;
                    break;
                }
            }
        }
    }
    return retn;
}


boolean TypeRequiresCtorParm(   // TEST IF EXTRA CTOR PARM REQUIRED
    TYPE type )                 // - the type
{
    return TypeHasVirtualBases( type );
}


boolean PointerToFuncEquivalent( // TEST IF EQUIVALENT TO PTR(FUNCTION)
    TYPE type )
{
    boolean retn;               // - return: TRUE ==> equiv. to ptr to funct.
    type_flag not_used;

    type = TypedefModifierRemove( type );
    if( type->id == TYP_FUNCTION ) {
        retn = TRUE;
    } else if( PointerTypeEquivalent( type ) ) {
        type = TypePointedAt( type, &not_used );
        if( type->id == TYP_FUNCTION ) {
            retn = TRUE;
        } else {
            retn = FALSE;
        }
    }
    return retn;
}


#if 0 // not used
boolean TypeIsCppFunc(          // TEST IF C++ FUNCTION TYPE
    TYPE type )                 // - type to be tested
{
    boolean retn;               // - return: TRUE ==> C++ function

    type = FunctionDeclarationType( type );
    if( type == NULL ) {
        retn = FALSE;
    } else if( type->flag & TF1_PLUSPLUS ) {
        retn = TRUE;
    } else {
        retn = FALSE;
    }
    return retn;
}
#endif


TYPE TypeFunctionCalled(        // GET FUNCTION DECLARATION TYPE CALLED
    TYPE type )                 // - function type called
{
    TYPE pted;                  // - NULL or type pointed at

    type = TypeReferenced( type );
    pted = TypePointedAtModified( type );
    if( NULL != pted ) {
        type = pted;
    }
    return FunctionDeclarationType( type );
}


TYPE TypeThisForCall(           // GET "THIS" TYPE FOR A CALL
    PTREE this_node,            // - this node
    SYMBOL sym )                // - function being called
{
    TYPE this_type;             // - target type for "this"

    this_type = SymClass( sym );
    if( ExprIsLvalue( this_node ) ) {
        this_type = MakeReferenceTo( this_type );
    } else {
        this_type = MakePointerTo( this_type );
    }
    return this_type;
}


static CLASSINFO* getClassInfo( // GET CLASS INFO FOR GOOD ELEMENTAL TYPE
    TYPE type )                 // - type
{
    CLASSINFO* info;            // - information for class
    TYPE artype;                // - NULL or array type

    if( type == NULL ) {
        info = NULL;
    } else {
        artype = ArrayType( type );
        if( artype != NULL ) {
            type = ArrayBaseType( artype );
        }
        type = StructType( type );
        if( type == NULL ) {
            info = NULL;
        } else {
            info = TypeClassInfo( type );
            if( info->corrupted ) {
                info = NULL;
            }
        }
    }
    return info;
}


boolean TypeRequiresRWMemory(   // TEST IF TYPE MUST BE IN NON-CONST STORAGE
    TYPE type )                 // - type
{
    CLASSINFO* info;            // - information for class
    boolean retn;               // - TRUE ==> requires CTOR'ING

    retn = FALSE;
    info = getClassInfo( type );
    if( info != NULL ) {
        if( info->needs_ctor || info->needs_dtor || info->has_mutable ) {
            retn = TRUE;
        }
    }
    return retn;
}


boolean TypeRequiresCtoring(    // TEST IF TYPE MUST BE CTOR'ED
    TYPE type )                 // - type
{
    CLASSINFO* info;            // - information for class
    boolean retn;               // - TRUE ==> requires CTOR'ING

    info = getClassInfo( type );
    if( info == NULL ) {
        retn = FALSE;
    } else if( info->needs_ctor ) {
        retn = TRUE;
    } else {
        retn = FALSE;
    }
    return retn;
}


boolean TypeRequiresDtoring(    // TEST IF TYPE MUST BE DTOR'ED
    TYPE type )                 // - type
{
    CLASSINFO* info;            // - information for class
    boolean retn;               // - TRUE ==> requires CTOR'ING

    info = getClassInfo( type );
    if( info == NULL ) {
        retn = FALSE;
    } else if( info->needs_dtor ) {
        retn = TRUE;
    } else {
        retn = FALSE;
    }
    return retn;
}

static TYPE augmentWithNear(    // AUGMENT TYPE WITH TF1_NEAR, IF REQ'D
    TYPE type )                 // - type to be augmented
{
    type_flag flags;            // - existing flags

    TypeGetActualFlags( type, &flags );
    if( 0 == ( flags & TF1_MEM_MODEL ) ) {
        type = MakeModifiedType( type, TF1_NEAR );
    }
    return type;
}


TYPE TypeAutoDefault(           // ADD NEAR QUALIFIER FOR AUTO SYMBOL
    TYPE type,                  // - a type
    PTREE expr )                // - possible PT_SYMBOL of SC_AUTO
{
#if 0
    if( PtreeOpFlags( expr ) & PTO_RVALUE ) {
        type = augmentWithNear( type );
    } else if( expr->op == PT_SYMBOL ) {
#else
    if( expr->op == PT_SYMBOL ) {
#endif
        SYMBOL sym = expr->u.symcg.symbol;
        if( sym->id == SC_AUTO ) {
            type_flag flags;
            TypeGetActualFlags( sym->sym_type, &flags );
            if( 0 == ( flags & TF1_MEM_MODEL ) ) {
                type = augmentWithNear( type );
            }
        }
    }
    return type;
}


TYPE TypeForLvalue              // GET TYPE FOR LVALUE
    ( PTREE expr )              // - lvalue expression
{
    TYPE type_expr;             // - expression type
    TYPE type_lv;               // - type of LVALUE

    type_expr = NodeType( expr );
    type_lv = TypeReference( type_expr );
    DbgVerify( type_lv != NULL, "TypeForLvalue -- not lvalue" );
    return type_lv;
}


boolean ExprIsLvalue            // TEST IF EXPRESSION IS LVALUE
    ( PTREE expr )              // - expression
{
    boolean retn;               // - return: TRUE ==> is lvalue

    if( expr->flags & PTF_LVALUE ) {
        retn = TRUE;
    } else {
#ifndef NDEBUG
        TYPE type_expr;
        TYPE type_lv;
        type_expr = NodeType( expr );
        type_lv = TypeReference( type_expr );
        retn = NULL != type_lv;
#else
        retn = NULL != TypeForLvalue( expr );
#endif
    }
    return retn;
}


boolean TypeDefedNonAbstract    // REQUIRE DEFINED, NON-ABSTRACT TYPE
    ( TYPE type                 // - the type
    , PTREE expr                // - NULL or expression for error
    , MSG_NUM msg_abstract      // - message when abstract
    , MSG_NUM msg_undefed )     // - message when undefined
{
    boolean retn;               // - return: TRUE ==> defined & non-abstract

    expr = expr;
    if( ! TypeDefined( type ) ) {
        CErr1( msg_undefed );
        InfClassDecl( type );
        return FALSE;
    } else if( AbstractClassType( type ) ) {
        CErr1( msg_abstract );
        InfClassDecl( type );
        ScopeNotePureFunctions( type );
        return FALSE;
    } else {
        retn = TRUE;
    }
    return retn;
}
