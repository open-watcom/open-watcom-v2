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
#include "cgdata.h"
#include "cgfront.h"
#include "codegen.h"
#include "initdefs.h"
#include "objmodel.h"
#include "cgback.h"

// get name consistency
//
#define TY_NR_CODE_PTR      TY_NEAR_CODE_PTR
#define TY_FR_CODE_PTR      TY_LONG_CODE_PTR
#define TY_HG_CODE_PTR      TY_LONG_CODE_PTR
#define TY_NR_POINTER       TY_NEAR_POINTER
#define TY_FR_POINTER       TY_LONG_POINTER
#define TY_HG_POINTER       TY_HUGE_POINTER
#define TY_FAR16_POINTER    TY_NEAR_POINTER
#define TY_FAR16_CODE_PTR   TY_NEAR_POINTER
#define TARGET_HG_POINTER   TARGET_FAR_POINTER
#define TARGET_FR_POINTER   TARGET_FAR_POINTER
#define TARGET_NR_POINTER   TARGET_NEAR_POINTER

#define PTR_NEAR            PTR_NR
#define PTR_FAR             PTR_FR
#define PTR_HUGE            PTR_HG
#define PTR_LONG            PTR_FR

#define PTR_TYPE(name,diff) __PASTE( PTR, name )           // - pointer types
typedef enum
#include "ptrtypes.h"
PTR_CLASS;

#define PTR_TYPE(name,diff) __PASTE3( TY, name, _POINTER )  // - CG data types
static cg_type ptr_dg_type[] =
#include "ptrtypes.h"
;

#define PTR_TYPE(name,diff) __PASTE3( TY, name, _CODE_PTR ) // - CG code types
static cg_type ptr_cg_type[] =
#include "ptrtypes.h"
;

#define PTR_TYPE(name,diff) __PASTE3( TARGET, name, _POINTER ) // - sizes
static target_size_t ptr_size[] =
#include "ptrtypes.h"
;

#define PTR_TYPE(name,diff) diff            // - TYPE for difference
static TYPE *ptr_diff_type[] =
#include "ptrtypes.h"
;

static CGREFNO defined_type     // next refno for defined types
            = TY_FIRST_FREE;
static CGREFNO cg_member_ptr    // CG type for member pointers
            = NULL_CGREFNO;
static PTR_CLASS defaultDataPtrClass;// default data pointer type
static PTR_CLASS defaultCodePtrClass;// default code pointer type


static PTR_CLASS ptr_type(      // CLASSIFY POINTER TYPE
    type_flag mod_flags,        // - modifier flags
    PTR_CLASS ptr_class )       // - default classification
{
    if( mod_flags & TF1_MEM_MODEL ) {
        if( mod_flags & ( TF1_NEAR | TF1_BASED | TF1_FAR16 ) ) {
            ptr_class = PTR_NEAR;
        } else if( mod_flags & TF1_FAR ) {
            ptr_class = PTR_LONG;
        } else if( mod_flags & TF1_HUGE ) {
            ptr_class = PTR_HUGE;
        }
    }
    return( ptr_class );
}

#define data_ptr_type( mf ) ( ptr_type( (mf), defaultDataPtrClass ) )
#define code_ptr_type( mf ) ( ptr_type( (mf), defaultCodePtrClass ) )


TYPE CgStripType(               // STRIP ONE LEVEL OF TYPE INFORMATION
    TYPE type )                 // - type
{
    return( TypedefModifierRemove( type )->of );
}


static unsigned cg_new_type(    // GET NEW TYPE FOR CODE GENERATOR
    unsigned size,              // - size of type
    unsigned alignment )        // - alignment of type
{
    unsigned retn;              // - returned type

    // should be fatal error on overflow!
    retn = defined_type++;
    DbgAssert( defined_type != 0 );
    BEDefType( retn, alignment, size );
    return( retn );
}


static unsigned cg_defined_type( // GET DEFINED TYPE
    TYPE type,                  // - C++ type
    CGREFNO *refno )            // - addr( reference # for type )
{
    CGREFNO retn;               // - returned type

    retn = *refno;
    if( retn == NULL_CGREFNO ) {
        retn = cg_new_type( CgMemorySize( type ), CgMemorySize( AlignmentType( type ) ) );
        *refno = retn;
    }
    return( retn );
}


cg_type CgTypeOutput(           // COMPUTE TYPE FOR CODE GENERATOR
    TYPE type )                 // - C++ type
{
    cg_type     retn;           // - return type (code generator)
    type_flag   mod_flags;      // - modifier flags

    type = TypeModFlags( type, &mod_flags );
    switch( type->id ) {
      case TYP_SCHAR :
        retn = TY_INT_1;
        break;
      case TYP_BOOL :
        retn = TY_BOOL;
        break;
      case TYP_UCHAR :
        retn = TY_UINT_1;
        break;
      case TYP_UINT :
        retn = TY_UNSIGNED;
        break;
      case TYP_USHORT :
      case TYP_WCHAR :
        retn = TY_UINT_2;
        break;
      case TYP_SINT :
        retn = TY_INTEGER;
        break;
      case TYP_SSHORT :
        retn = TY_INT_2;
        break;
      case TYP_ULONG :
        retn = TY_UINT_4;
        break;
      case TYP_SLONG :
        retn = TY_INT_4;
        break;
      case TYP_ULONG64 :
        retn = TY_UINT_8;
        break;
      case TYP_SLONG64 :
        retn = TY_INT_8;
        break;
      case TYP_FLOAT :
        retn = TY_SINGLE;
        break;
      case TYP_LONG_DOUBLE :
        retn = TY_DOUBLE;           // change later when long-double support
        break;
      case TYP_DOUBLE :
        retn = TY_DOUBLE;
        break;
      case TYP_POINTER :
        type = TypeModFlags( type->of, &mod_flags );
        if( type->id == TYP_FUNCTION ) {
            retn = ptr_cg_type[ code_ptr_type( mod_flags ) ];
        } else {
            retn = ptr_dg_type[ data_ptr_type( mod_flags ) ];
        }
        break;
      case TYP_ARRAY :
        retn = cg_defined_type( type, &type->u.a.refno );
        break;
      case TYP_CLASS :
        retn = cg_defined_type( type, &type->u.c.info->refno );
        break;
      case TYP_FUNCTION :
        retn = ptr_cg_type[ code_ptr_type( mod_flags ) ];
        break;
      case TYP_MEMBER_POINTER :
        retn = cg_defined_type( type, &cg_member_ptr );
        break;
      default:
        retn = TY_INTEGER;
        break;
    }
    return( retn );
}


bool IsCgTypeAggregate(         // CAN TYPE CAN BE INITIALIZED AS AGGREGATE?
    TYPE type,                  // - C++ type
    bool string )               // - array of string not aggregate
{
    bool retb = false;          // - true if aggregate
    CLASSINFO *info;            // - info part of class type

    type = TypedefModifierRemove( type );
    switch( type->id ) {
      case TYP_ARRAY :
        if( string && TypeIsCharString( type ) ) break;
        retb = true;
        break;
      case TYP_BITFIELD :
        retb = true;
        break;
      case TYP_CLASS :
        info = type->u.c.info;
        if( info->corrupted ) break;
        if( info->bases != NULL ) break;
        if( info->size != type->u.c.info->vsize ) break;
        if( info->last_vfn != 0 ) break;
        if( info->last_vbase != 0 ) break;
        if( info->has_data == 0 ) break;
        if( info->has_ctor != 0 ) break;
        retb = true;
        break;
    }
    return( retb );
}


#define code_ptr_size( mf ) ( ptr_size[ code_ptr_type( (mf) ) ] )
#define data_ptr_size( mf ) ( ptr_size[ data_ptr_type( (mf) ) ] )


target_size_t CgDataPtrSize(    // SIZE OF DEFAULT DATA POINTER
    void )
{
    return( data_ptr_size( TF1_NULL ) );
}


target_size_t CgCodePtrSize(    // SIZE OF DEFAULT CODE POINTER
    void )
{
    return( code_ptr_size( TF1_NULL ) );
}


static target_size_t cgSize(    // COMPUTE SIZE OF A TYPE
    TYPE type,                  // - type
    bool ref_as_ptr )           // - true ==> treat reference as pointer
{
    type_flag mod_flags;        // - modifier flags
    target_size_t size;         // - size of type

    type = TypeModFlags( type, &mod_flags );
    switch( type->id ) {
      case TYP_BOOL :
        size = TARGET_BOOL;
        break;
      case TYP_ERROR :
      case TYP_UCHAR :
      case TYP_SCHAR :
        size = TARGET_CHAR;
        break;
      case TYP_WCHAR :
      case TYP_USHORT :
      case TYP_SSHORT :
        size = TARGET_SHORT;
        break;
      case TYP_SINT :
        size = TARGET_INT;
        break;
      case TYP_UINT :
        size = TARGET_UINT;
        break;
      case TYP_ULONG :
        size = TARGET_ULONG;
        break;
      case TYP_SLONG :
        size = TARGET_LONG;
        break;
      case TYP_ULONG64 :
        size = TARGET_ULONG64;
        break;
      case TYP_SLONG64 :
        size = TARGET_LONG64;
        break;
      case TYP_FLOAT :
        size = TARGET_FLOAT;
        break;
      case TYP_DOUBLE :
        size = TARGET_DOUBLE;
        break;
      case TYP_LONG_DOUBLE :
        size = TARGET_LONG_DOUBLE;
        break;
      case TYP_POINTER :
        if( ( ! ref_as_ptr ) && ( type->flag & TF1_REFERENCE ) ) {
            size = cgSize( type->of, false );
        } else {
            type = TypeModFlags( type->of, &mod_flags );
            if( type->id == TYP_FUNCTION ) {
                size = code_ptr_size( mod_flags );
            } else {
                size = data_ptr_size( mod_flags );
            }
        }
        break;
      case TYP_BITFIELD :
        size = cgSize( type->of, true );
        break;
      case TYP_ARRAY :
        size = type->u.a.array_size;
        if( type->flag & TF1_ZERO_SIZE ) {
            DbgAssert( type->u.a.array_size == 1 );
            size = 0;
        } else {
            size *= cgSize( type->of, false );
        }
        break;
      case TYP_CLASS :
        if( TypeDefined( type ) ) {
            size = type->u.c.info->size;
        } else {
            size = 0;
        }
        break;
      case TYP_FUNCTION :
        size = code_ptr_size( mod_flags );
        break;
      case TYP_MEMBER_POINTER :
        size = CgCodePtrSize() + 2 * TARGET_UINT;
        break;
      default :
        size = 0;
        break;
    }
    return( size );
}


target_size_t CgTypeSize(       // COMPUTE SIZE OF A TYPE IN USE
    TYPE type )                 // - type
{
    return cgSize( type, false );
}


target_size_t CgMemorySize(     // COMPUTE SIZE OF A TYPE IN MEMORY
    TYPE type )                 // - type
{
    return cgSize( type, true );
}


cg_type CgTypeSym(             // COMPUTE OUTPUT TYPE FOR SYMBOL
    SYMBOL sym )                // - the symbol
{
    TYPE type = sym->sym_type;
    if( SymIsArgument( sym )
     && NULL != StructType( type )
     && OMR_CLASS_REF == ObjModelArgument( type ) ) {
        type = MakeReferenceTo( type );
    }
    return CgTypeOutput( type );
}


cg_type CgTypePtrSym(          // COMPUTE OUTPUT TYPE OF POINTER TO SYMBOL
    SYMBOL sym )                // - symbol
{
    type_flag mod_flags;        // - modifier flags
    TYPE type;                  // - unmodified type
    cg_type codegen_type;      // - code-gen type

    type = TypeModFlags( sym->sym_type, &mod_flags );
    if( type->id == TYP_FUNCTION ) {
        if( mod_flags & TF1_NEAR ) {
            codegen_type = TY_NEAR_CODE_PTR;
        } else if( mod_flags & TF1_FAR ) {
            codegen_type = TY_LONG_CODE_PTR;
        } else {
            codegen_type = TY_CODE_PTR;
        }
    } else {
        if( mod_flags & TF1_NEAR ) {
            codegen_type = TY_NEAR_POINTER;
        } else if( mod_flags & TF1_FAR ) {
            codegen_type = TY_LONG_POINTER;
        } else if( mod_flags & TF1_HUGE ) {
            codegen_type = TY_HUGE_POINTER;
        } else {
            codegen_type = TY_POINTER;
        }
    }
    return codegen_type;
}


static
target_size_t cgPtrSize(        // COMPUTE SIZE OF A POINTER
    TYPE type )                 // - type
{
    type_flag mod_flags;        // - modifier flags
    unsigned size;              // - size of the type

    type = TypeModFlags( type, &mod_flags );
    switch( type->id ) {
      case TYP_ARRAY :
      case TYP_POINTER :
        TypeModFlags( type->of, &mod_flags );
        size = data_ptr_size( mod_flags );
        break;
      case TYP_FUNCTION :
        size = code_ptr_size( mod_flags );
        break;
      default :
        size = 0;
        break;
    }
    return( size );
}


TYPE TypePointerDiff(           // GET TYPE FOR DIFFERENCE OF POINTERS
    TYPE type )                 // - node for a type
{
    type_flag flag;             // - flags, when modifier

    type = TypeModFlags( type, &flag );
    switch( type->id ) {
      case TYP_ARRAY :
      case TYP_POINTER :
        TypeModFlags( type->of, &flag );
        type = *ptr_diff_type[ data_ptr_type( flag ) ];
        break;
      case TYP_FUNCTION :
        type = *ptr_diff_type[ code_ptr_type( flag ) ];
        break;
      default :
        type = NULL;
        break;
    }
    return( type );
}


static target_size_t cgTypeTruncSize( // GET SIZE FOR TRUNCATION
    TYPE type )                 // - type to be sized
{
    target_size_t size;         // - size of element
    type_flag flags;            // - flags for a type

    switch( TypedefModifierRemove( type )->id ) {
      case TYP_ARRAY :
      case TYP_FUNCTION :
      case TYP_POINTER :
        TypePointedAt( type, &flags );
        if( flags & ( TF1_BASED | TF1_FAR16 ) ) {
            type = TypeConvertFromPcPtr( type );
        }
        size = cgPtrSize( type );
        break;
      default :
        size = CgTypeSize( type );
        break;
    }
    return( size );
}


CNV_RETN CgTypeTruncation(      // GET CNV_... FOR TRUNCATION
    TYPE tgt,                   // - target type
    TYPE src )                  // - source type
{
    CNV_RETN retn;              // - CNV_OK or CNV_OK_TRUNC
    type_flag src_flags;        // - flags for pointed source type
    type_flag tgt_flags;        // - flags for pointed target type
    TYPE umod;                  // - unmodified type for item
    TYPE ptr_type;              // - pointer type

    umod = TypePointedAt( tgt, &tgt_flags );
    if( ( umod != NULL ) && ( tgt_flags & ( TF1_BASED | TF1_FAR16 ) ) ) {
        retn = CNV_OK;
    } else {
        umod = TypePointedAt( src, &src_flags );
        if( ( umod != NULL ) && ( src_flags & ( TF1_BASED | TF1_FAR16 ) ) ) {
            src = TypeConvertFromPcPtr( src );
        }
        if( cgTypeTruncSize( tgt ) < cgTypeTruncSize( src ) ) {
            retn = CNV_OK_TRUNC;
            if( tgt_flags & TF1_NEAR ) {
                if( src_flags & (TF1_FAR|TF1_HUGE) ) {
                    ptr_type = PointerTypeEquivalent( src );
                    if( ptr_type->id == TYP_POINTER ) {
                        /* far to near cnv of a ptr/ref */
                        if( ( ptr_type->flag & TF1_FAR_BUT_NEAR ) != 0 ) {
                            /* but the far is really near */
                            retn = CNV_OK;
                        }
                    }
                }
            }
        } else {
            retn = CNV_OK;
        }
    }
    return( retn );
}


cg_type CgTypeOffset(          // GET CODEGEN TYPE FOR AN OFFSET
    void )
{
#if _CPU == 8086
    return TY_UINT_2;
#else
    return TY_UINT_4;
#endif
}


static void init(               // MODULE INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    defined_type = TY_FIRST_FREE;
    cg_member_ptr = NULL_CGREFNO;
#if _CPU == _AXP
    defaultDataPtrClass = PTR_NEAR;
    defaultCodePtrClass = PTR_NEAR;
#elif _INTEL_CPU
    defaultDataPtrClass = PTR_NEAR;
    defaultCodePtrClass = PTR_NEAR;
    if( !IsFlat() ) {
        if( IsHugeData() ) {
            defaultDataPtrClass = PTR_HUGE;
        } else if( IsBigData() ) {
            defaultDataPtrClass = PTR_LONG;
        }
        if( IsBigCode() ) {
            defaultCodePtrClass = PTR_LONG;
        }
    }
#else
    #error bad target
#endif
}


INITDEFN( cg_typing, init, InitFiniStub )
