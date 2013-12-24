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

#include <assert.h>

#include "cgfront.h"
#include "cgback.h"
#include "codegen.h"
#include "pragdefn.h"
#include "carve.h"
#include "ppops.h"
#include "ring.h"
#include "vfun.h"
#include "symdbg.h"
#include "dbgsupp.h"
#include "stacksr.h"

typedef struct fwd_info FWD_INFO;
struct fwd_info {
    FWD_INFO    *next;
    TYPE        type;
    dbg_name    dn;
    dbg_type    dt;
    dbg_name    comp_dn;
};

// carve storage for fwd_info structures
#define BLOCK_FWD_INFO      16
static carve_t carveFWD_INFO;

static FWD_INFO *prevFwdInfo;
static dbg_type scopeEnum;
static dbg_type scopeUnion;
static dbg_type scopeStruct;
static dbg_type scopeClass;
static TYPE     vf_FieldType;
static TYPE     pvf_FieldType;
static unsigned vf_FieldTypeSize;
static TYPE     vb_FieldType;
static TYPE     pvb_FieldType;
static unsigned vb_FieldTypeSize;

// prototypes
static dbg_loc symbolicDebugSetSegment( dbg_loc dl, SYMBOL sym );
static dbg_loc symbolicDebugSetCodeSegment( dbg_loc dl );
static void doSymbolicDebugFundamentalType( TYPE type, void *data );
static void symbolicDebugFundamentalType( void );
static void doSymbolicDebugNamedType( TYPE type, void *data );
static void symbolicDebugNamedType( void );
static void symbolicDebugSymbol( void );

void SymbolicDebugInit( void )
/****************************/
{
    assert( sizeof( vf_FieldType->dbg.handle ) == sizeof( dbg_type ) );

    if( GenSwitches & DBG_TYPES ) {
        scopeEnum = DBScope( "enum" );
        scopeUnion = DBScope( "union" );
        scopeStruct = DBScope( "struct" );
        scopeClass = DBScope( "class" );

        DbgSuppInit( DSI_NULL );
    }

    if( GenSwitches & DBG_LOCALS ) {
        pvf_FieldType = MakeVFTableFieldType( TRUE );
        vf_FieldType = PointerTypeEquivalent( pvf_FieldType )->of;
        vf_FieldTypeSize = CgTypeSize( vf_FieldType );

        pvb_FieldType = MakeVBTableFieldType( TRUE );
        vb_FieldType = PointerTypeEquivalent( pvb_FieldType )->of;
        vb_FieldTypeSize = CgTypeSize( vb_FieldType );
    }
    carveFWD_INFO  = CarveCreate( sizeof( FWD_INFO ), BLOCK_FWD_INFO );

}

void SymbolicDebugFini( void )
/****************************/
{
    DbgStmt( CarveVerifyAllGone( carveFWD_INFO, "FWD_INFO" ) );
    CarveDestroy( carveFWD_INFO );
}

static cg_sym_handle symbolicDebugSymAlias( SYMBOL sym )
/******************************************************/
{
    SYMBOL check;

    sym = SymDeAlias( sym );
    check = SymIsAnonymous( sym );
    if( check != NULL ) {
        sym = check;
    }
    return( (cg_sym_handle)sym );
}

static dbg_loc symbolicDebugSetSegment( dbg_loc dl, SYMBOL sym )
/**************************************************************/
{
    dl = DBLocSym( dl, symbolicDebugSymAlias( sym ) );
    dl = DBLocOp( dl, DB_OP_XCHG, 1 );
    dl = DBLocOp( dl, DB_OP_MK_FP, 0 );
    return( dl );
}

static dbg_loc symbolicDebugSetCodeSegment( dbg_loc dl )
/******************************************************/
{
    if( !IsBigCode() ) {
        dl = symbolicDebugSetSegment( dl, DefaultCodeSymbol );
        DbgAddrTaken( DefaultCodeSymbol );
    }
    return( dl );
}

dbg_loc SymbolicDebugSetDataSegment( dbg_loc dl )
/***********************************************/
{
    if( !IsBigData() ) {
        dl = symbolicDebugSetSegment( dl, DefaultDataSymbol );
        DefaultDataSymbol->flag |= SF_REFERENCED;
    }
    return( dl );
}

static dbg_loc symbolicDebugSymAddr( dbg_loc dl, SYMBOL sym )
/***********************************************************/
{
#ifdef DISABLE_CGRF
    if( SymIsReferenced( sym ) ) {
        DbgAddrTaken( sym );
        dl = DBLocSym( dl, symbolicDebugSymAlias( sym ) );
        if( SymIsInitialized( sym ) ) {
            cgfile = CgioLocateFile( sym );
            if( cgfile != NULL ) {
                cgfile->refed = TRUE;
            }
        }
    }
#else
    DbgAddrTaken( sym );
    dl = DBLocSym( dl, symbolicDebugSymAlias( sym ) );
#endif
    return( dl );
}


static dbg_type symWVDebugClassType( TYPE type )
/*************************************************/
{ // Dump WV class
    dbg_type    dt;
    dbg_struct  ds;
    dbg_loc     dl;
    SYMBOL      stop, curr;
    BASE_CLASS  *base;
    CLASSINFO   *info;
    CLASSINFO   *base_info;
    TYPE        root = TypedefModifierRemoveOnly( type );

    ds = DBBegStruct( CgTypeOutput( type ), !(root->flag & TF1_UNION) );
    dt = DBStructForward( ds );
    info = root->u.c.info;
    if( dt != DBG_NIL_TYPE  ){
        info->dbg_no_vbases = dt;
        type->dbg.handle = dt;
    }

    // define all the members
    // first data members
    stop = ScopeOrderedStart( root->u.c.scope );
    curr = ScopeOrderedNext( stop, NULL );
    while( curr != NULL ) {
        if( SymIsClassMember( curr ) &&
            SymIsData( curr ) &&
            !SymIsEnumeration( curr ) &&
            IsCppNameInterestingDebug( curr ) ) {
            uint    attribute;
            byte    offset = 0;
            byte    length = 0;
            if( curr->flag & SF_PRIVATE ) {
                attribute = FIELD_ATTR_PRIVATE;
            } else if( curr->flag & SF_PROTECTED ) {
                attribute = FIELD_ATTR_PROTECTED;
            } else {
                attribute = FIELD_ATTR_PUBLIC;
            }
            if( curr->sym_type->id == TYP_BITFIELD ) {
                offset = curr->sym_type->u.b.field_start;
                length = curr->sym_type->u.b.field_width;
            }
            dl = DBLocInit();
            if( SymIsStaticDataMember( curr ) ) {
                DbgAddrTaken( curr );
                dl = DBLocSym( dl, symbolicDebugSymAlias( curr ) );
            } else if( SymIsThisDataMember( curr ) ){
                TYPE pt;
                if( curr->u.member_offset != 0 ) {
                    dl = DBLocConst( dl, curr->u.member_offset );
                    dl = DBLocOp( dl, DB_OP_ADD, 0 );
                }
                pt = PointerTypeEquivalent( curr->sym_type );
                if( pt
                 && pt->id == TYP_POINTER
                 && (pt->flag & TF1_REFERENCE) ) {
                    dl = DBLocOp( dl, DB_OP_POINTS,
                                  CgTypeOutput( curr->sym_type ) );
                    dl = SymbolicDebugSetDataSegment( dl );
                }
            } else {
                CFatal( "symdbg: illegal data member symbol" );
            }
            DBAddLocField( ds,
                           dl,
                           attribute,
                           offset,
                           length,
                           CppNameDebug( curr ),
                           SymbolicDebugType( curr->sym_type, SD_DEFAULT ) );
            DBLocFini( dl );
        }
        curr = ScopeOrderedNext( stop, curr );
    }
    if( info->has_vbptr ) {
        dl = DBLocInit();
        if( info->vb_offset != 0 ) {
            dl = DBLocConst( dl, info->vb_offset );
            dl = DBLocOp( dl, DB_OP_ADD, 0 );
        }
        DBAddLocField( ds, dl, FIELD_ATTR_INTERNAL, 0, 0, "__vbptr",
                       SymbolicDebugType( pvb_FieldType, SD_DEFAULT ) );
        DBLocFini( dl );
    }
    if( info->has_vfptr ) {
        dl = DBLocInit();
        if( info->vf_offset != 0 ) {
            dl = DBLocConst( dl, info->vf_offset );
            dl = DBLocOp( dl, DB_OP_ADD, 0 );
        }
        DBAddLocField( ds, dl, FIELD_ATTR_INTERNAL, 0, 0, "__vfptr",
                       SymbolicDebugType( pvf_FieldType, SD_DEFAULT ) );
        DBLocFini( dl );
    }

    // now function members
    stop = ScopeOrderedStart( root->u.c.scope );
    curr = ScopeOrderedNext( stop, NULL );
    while( curr != NULL ) {
        if( SymIsClassMember( curr ) &&
            !SymIsData( curr ) &&
            IsCppNameInterestingDebug( curr ) ) {
            if( SymIsEnumeration( curr ) ) {
                SymbolicDebugType( curr->sym_type, SD_DEFAULT );
            } else {
                uint    attribute;
                if( curr->flag & SF_PRIVATE ) {
                    attribute = FIELD_ATTR_PRIVATE;
                } else if( curr->flag & SF_PROTECTED ) {
                    attribute = FIELD_ATTR_PROTECTED;
                } else {
                    attribute = FIELD_ATTR_PUBLIC;
                }
                dl = DBLocInit();
                if( SymIsStaticFuncMember( curr ) ) {
                    dl = symbolicDebugSymAddr( dl, curr );
                } else if( SymIsThisFuncMember( curr ) ) {
                    if( SymIsVirtual( curr ) ) {
                        if( info->vf_offset != 0 ) {
                            dl = DBLocConst( dl, info->vf_offset );
                            dl = DBLocOp( dl, DB_OP_ADD, 0 );
                        }
                        dl = DBLocOp( dl, DB_OP_POINTS, CgTypeOutput( pvf_FieldType ) );
                        dl = DBLocConst( dl, ( curr->u.member_vf_index - 1 ) * vf_FieldTypeSize );
                        dl = DBLocOp( dl, DB_OP_ADD, 0 );
                        dl = DBLocOp( dl, DB_OP_POINTS, CgTypeOutput( vf_FieldType ) );
                        dl = symbolicDebugSetCodeSegment( dl );
                    } else {
                        dl = symbolicDebugSymAddr( dl, curr );
                    }
                } else {
                    CFatal( "symdbg: illegal function member symbol" );
                }
                DBAddLocField( ds,
                               dl,
                               attribute,
                               0,
                               0,
                               CppNameDebug( curr ),
                               SymbolicDebugType( curr->sym_type, SD_DEFAULT ) );
                DBLocFini( dl );
            }
        }
        curr = ScopeOrderedNext( stop, curr );
    }

    // define all the direct non-virtual bases
    RingIterBeg( ScopeInherits( root->u.c.scope ), base ) {
        if( _IsDirectNonVirtualBase( base ) ) {
            dl = DBLocInit();
            dl = DBLocConst( dl, base->delta );
            SymbolicDebugType( base->type, SD_DEFAULT );
            base_info = StructType( base->type )->u.c.info;
            DBAddInheritance( ds, base_info->dbg_no_vbases,
                   FIELD_ATTR_PUBLIC, INHERIT_DBASE,  dl );
            DBLocFini( dl );
        }
    } RingIterEnd( base )
    dt = DBEndStruct( ds );
    info->dbg_no_vbases= dt;

    if( ScopeHasVirtualBases( root->u.c.scope ) ) {
        ds = DBBegStruct( CgTypeOutput( type ), !(root->flag & TF1_UNION) );

        // define own component
        dl = DBLocInit();
        dl = DBLocConst( dl, 0 );
        DBAddInheritance( ds, info->dbg_no_vbases,
                   FIELD_ATTR_PUBLIC, INHERIT_DBASE,  dl );
        DBLocFini( dl );

        // define all the direct+indirect virtual bases
        RingIterBeg( ScopeInherits( root->u.c.scope ), base ) {
            if( _IsVirtualBase( base ) ) {
                dl = DBLocInit();
                if( info->vb_offset != 0 ) {
                    dl = DBLocConst( dl, info->vb_offset );
                    dl = DBLocOp( dl, DB_OP_ADD, 0 );
                }
                dl = DBLocOp( dl, DB_OP_POINTS, CgTypeOutput( pvb_FieldType ) );
                dl = DBLocConst( dl, base->vb_index * vb_FieldTypeSize );
                dl = DBLocOp( dl, DB_OP_ADD, 0 );
                dl = DBLocOp( dl, DB_OP_POINTS, CgTypeOutput( vb_FieldType ) );
                if( info->vb_offset != 0 ) {
                    dl = DBLocConst( dl, info->vb_offset );
                    dl = DBLocOp( dl, DB_OP_ADD, 0 );
                }
                SymbolicDebugType( base->type, SD_DEFAULT );
                base_info = StructType( base->type )->u.c.info;
                DBAddInheritance( ds, base_info->dbg_no_vbases,
                            FIELD_ATTR_PUBLIC, INHERIT_IVBASE,  dl );
                DBLocFini( dl );
            }
        } RingIterEnd( base )
        dt = DBEndStruct( ds );
    }
    root->dbgflag |= TF2_SYMDBG;
    root->dbg.handle = dt;
    type->dbg.handle = dt;

    return( dt );
}


static dbg_type  symbolicMethodType( SYMBOL curr, TYPE cls  ){
/*************************************************************/
    dbg_proc    dp;
    arg_list    *alist;
    int         i;
    TYPE        base;
    TYPE        type;
    TYPE        this_type;
    dbg_type    this_dbg;
    dbg_type    ret;
    dbg_type    dc;

    type = curr->sym_type;
    base = TypedefModifierRemoveOnly( type );
    if( type->dbgflag & TF2_NON_SYMDBG ) {
        type->dbgflag &= ~TF2_NON_SYMDBG;
        type->dbg.handle = DBG_NIL_TYPE;
    }
    dp = DBBegProc( CgTypeOutput( type ),
                    SymbolicDebugType( base->of,
                                       SD_DEFAULT ^SD_DEREF ) );//XOR
    dc = SymbolicDebugType( cls, SD_DEFAULT );
    this_type = TypeThisSymbol( curr, FALSE );
    if( this_type != NULL ){
        this_dbg = SymbolicDebugType( this_type,
                                           SD_DEFAULT ^SD_DEREF );//XOR
    }else{
        this_dbg = DBG_NIL_TYPE;
    }
    DBAddMethParms( dp, dc, this_dbg );
    alist = TypeArgList( base );
    for( i = 0 ; i < alist->num_args ; i++ ) {
        if( alist->type_list[i]->id == TYP_DOT_DOT_DOT ) break;
        DBAddParm( dp,
                   SymbolicDebugType( alist->type_list[i],
                                      SD_DEFAULT^SD_DEREF ) );//XOR
    }
    ret = DBEndProc( dp );
    type->dbgflag |= TF2_SYMDBG;
    type->dbg.handle = ret;
    return( ret );
}


static dbg_type symCVDebugClassType( TYPE type )
/*************************************************/
{ // Dump Codeview class
    dbg_type    dt;
    dbg_type    dmt;
    dbg_struct  ds;
    dbg_loc     dl;
    SYMBOL      stop, curr;
    BASE_CLASS  *base;
    CLASSINFO   *info;
    CLASSINFO   *base_info;
    TYPE        root = TypedefModifierRemoveOnly( type );

    info = root->u.c.info;
    if( info->unnamed ){
        ds = DBBegStruct( CgTypeOutput( type ), !(root->flag & TF1_UNION) );
    }else{
        ds = DBBegNameStruct( NameStr( info->name ), CgTypeOutput( type ), !(root->flag & TF1_UNION) );
    }
    DBNested( FALSE );
    dt = DBStructForward( ds );
    if( dt != DBG_NIL_TYPE  ){
        info->dbg_no_vbases = dt;
        type->dbg.handle = dt;
    }

    // define all the direct non-virtual bases
    RingIterBeg( ScopeInherits( root->u.c.scope ), base ) {
        if( _IsDirectNonVirtualBase( base ) ) {
            dl = DBLocInit();
            dl = DBLocConst( dl, base->delta );
            SymbolicDebugType( base->type, SD_DEFAULT );
            base_info = StructType( base->type )->u.c.info;
            DBAddInheritance( ds, base_info->dbg_no_vbases,
                   FIELD_ATTR_PUBLIC, INHERIT_DBASE,  dl );
            DBLocFini( dl );
        }
    } RingIterEnd( base )
    if( ScopeHasVirtualBases( root->u.c.scope ) ) {
        // Dump virtual bases
        dbg_type    dvbt;

        // define all the direct+indirect virtual bases
        dvbt  = SymbolicDebugType( vb_FieldType, SD_DEFAULT );
        DBAddBaseInfo( ds, info->vb_offset,vb_FieldTypeSize, dvbt, CgTypeOutput( pvb_FieldType  ) );
        RingIterBeg( ScopeInherits( root->u.c.scope ), base ) {
            if( _IsDirectVirtualBase( base ) ) { /* cv specific could be adapted */
                dl = DBLocInit();
                dl = DBLocConst( dl, base->vb_index );
                SymbolicDebugType( base->type, SD_DEFAULT );
                base_info = StructType( base->type )->u.c.info;
                DBAddInheritance( ds, base_info->dbg_no_vbases,
                            FIELD_ATTR_PUBLIC, INHERIT_VBASE,  dl );
                DBLocFini( dl );
            }
        } RingIterEnd( base )
        // Dump Indirect virtual bases
        RingIterBeg( ScopeInherits( root->u.c.scope ), base ) {
            if( _IsIndirectVirtualBase( base ) ) {
                dl = DBLocInit();
                dl = DBLocConst( dl, base->vb_index );
                SymbolicDebugType( base->type, SD_DEFAULT );
                base_info = StructType( base->type )->u.c.info;
                DBAddInheritance( ds, base_info->dbg_no_vbases,
                            FIELD_ATTR_PUBLIC, INHERIT_IVBASE,  dl );
                DBLocFini( dl );
            }
        } RingIterEnd( base )
    }
    // first data members
    stop = ScopeOrderedStart( root->u.c.scope );
    curr = ScopeOrderedNext( stop, NULL );
    while( curr != NULL ) {
        if( SymIsClassDefinition( curr )||SymIsEnumDefinition( curr ) ) {
            DBNested( TRUE );
            DBAddNestedType( ds,
                          CppNameDebug( curr ),
                           SymbolicDebugType( curr->sym_type, SD_DEFAULT ) );
            DBNested( FALSE );
        } else if( SymIsEnumeration( curr ) ) {
        } else if( SymIsInjectedTypedef( curr ) ) {
        } else if( SymIsTypedef( curr ) ) {
        } else if( SymIsStaticDataMember( curr ) ) {
            uint    attribute;

            if( curr->flag & SF_PRIVATE ) {
                attribute = FIELD_ATTR_PRIVATE;
            } else if( curr->flag & SF_PROTECTED ) {
                attribute = FIELD_ATTR_PROTECTED;
            } else {
                attribute = FIELD_ATTR_PUBLIC;
            }
                DbgAddrTaken( curr );
                dl = DBLocInit();
                dl = DBLocSym( dl, symbolicDebugSymAlias( curr ) );
                DBAddStField( ds, dl,
                           CppNameDebug( curr ),
                           attribute,
                           SymbolicDebugType( curr->sym_type, SD_DEFAULT ) );
                if( SymIsInitialized(  curr ) ){
                    if( GenSwitches & DBG_LOCALS ) {
                        DBGenStMem( (cg_sym_handle)curr, dl );
                   }
               }
               DBLocFini( dl );
        } else if( SymIsClassMember( curr ) &&
            SymIsData( curr ) &&
            !SymIsEnumeration( curr ) &&
            IsCppNameInterestingDebug( curr ) ) {
            uint    attribute;
            byte    offset = 0;
            byte    length = 0;

            if( curr->flag & SF_PRIVATE ) {
                attribute = FIELD_ATTR_PRIVATE;
            } else if( curr->flag & SF_PROTECTED ) {
                attribute = FIELD_ATTR_PROTECTED;
            } else {
                attribute = FIELD_ATTR_PUBLIC;
            }
            if( curr->sym_type->id == TYP_BITFIELD ) {
                offset = curr->sym_type->u.b.field_start;
                length = curr->sym_type->u.b.field_width;
            }
            dl = DBLocInit();
            if( SymIsThisDataMember( curr ) ){
                if( curr->u.member_offset != 0 ) {
                    dl = DBLocConst( dl, curr->u.member_offset );
                    dl = DBLocOp( dl, DB_OP_ADD, 0 );
                }
            } else {
                CFatal( "symdbg: illegal data member symbol" );
            }
            DBAddLocField( ds,
                           dl,
                           attribute,
                           offset,
                           length,
                           CppNameDebug( curr ),
                           SymbolicDebugType( curr->sym_type, SD_DEFAULT ) );
            DBLocFini( dl );
        }
        curr = ScopeOrderedNext( stop, curr );
    }
    if( info->has_vfptr ) {
        DBAddVFuncInfo(ds, info->vf_offset, info->last_vfn, CgTypeOutput( pvf_FieldType  ) );
    }
    // now function members
    stop = ScopeOrderedStart( root->u.c.scope );
    curr = ScopeOrderedNext( stop, NULL );
    while( curr != NULL ) {
        if( SymIsClassMember( curr ) &&
            !SymIsData( curr ) &&
            IsCppNameInterestingDebug( curr ) ) {
            if( SymIsEnumeration( curr ) ) {
                SymbolicDebugType( curr->sym_type, SD_DEFAULT );
            } else {
                uint    attribute;
                uint    kind;

                kind = METHOD_VANILLA;
                if( curr->flag & SF_PRIVATE ) {
                    attribute = FIELD_ATTR_PRIVATE;
                } else if( curr->flag & SF_PROTECTED ) {
                    attribute = FIELD_ATTR_PROTECTED;
                } else {
                    attribute = FIELD_ATTR_PUBLIC;
                }
                dl = DBLocInit();
                if( SymIsStaticFuncMember( curr ) ) {
                    kind = METHOD_STATIC;
                    dl = symbolicDebugSymAddr( dl, curr );
                } else if( SymIsThisFuncMember( curr ) ) {
                    if( SymIsVirtual( curr ) ) {
                        kind = METHOD_VIRTUAL;
                        dl = DBLocConst( dl, ( curr->u.member_vf_index - 1 ) * vf_FieldTypeSize );
                    } else {
                        dl = symbolicDebugSymAddr( dl, curr );
                    }
                } else {
                    CFatal( "symdbg: illegal function member symbol" );
                }
                dmt = symbolicMethodType( curr, type  );
                DBAddMethod( ds,
                             dl,
                             attribute,
                             kind,
                             CppNameDebug( curr ),
                             dmt );
                DBLocFini( dl );
            }
        }
        curr = ScopeOrderedNext( stop, curr );
    }
    dt = DBEndStruct( ds );
    info->dbg_no_vbases = dt;
    root->dbgflag |= TF2_SYMDBG;
    root->dbg.handle = dt;
    type->dbg.handle = dt;

    return( dt );
}

static dbg_type symbolicDebugClassType( TYPE type )
/*************************************************/
{
    dbg_type ret;

    if( GenSwitches & DBG_CV ) {
        ret = symCVDebugClassType( type );
    } else {
        ret = symWVDebugClassType( type );
    }
    return( ret );
}

static dbg_type basedPointerType( TYPE type, TYPE base, SD_CONTROL control )
/**************************************************************************/
{
    TYPE btype;
    dbg_loc dl;
    dbg_type dt;

    btype = BasedType( base->of );
    dl = DBLocInit();
    switch(  btype->flag & TF1_BASED  ) {
    //a caution if you change these  expressions
    //codeview and dwarf might not be able to translate them
    case TF1_BASED_STRING:
        dl = DBLocConst( dl, 0 );
        if( SegmentFindBased( btype ) == SEG_CODE ) {
            dl = symbolicDebugSetCodeSegment( dl );
        } else {
            // fixme: this should handle segments that aren't
            // in DGROUP by defining a symbol in that segment and
            // using it.  For now all such pointers just go to
            // DGROUP.  Note that defining a symbol in that segment
            // may require defining that segment.
            dl = SymbolicDebugSetDataSegment( dl );
        }
        break;
    case TF1_BASED_SELF:
        dl = DBLocConst( dl, 0 );
        dl = DBLocOp( dl, DB_OP_MK_FP, 0 );
        break;
    case TF1_BASED_VOID:
        dl = DBLocConst( dl, 0 );
        dl = DBLocConst( dl, 0 );
        dl = DBLocOp( dl, DB_OP_MK_FP, 0 );
        break;
    case TF1_BASED_FETCH:
        dl = DBLocSym( dl, symbolicDebugSymAlias( btype->u.m.base ) );
        dl = DBLocOp( dl, DB_OP_POINTS,
                      CgTypeOutput( GetBasicType( TYP_USHORT ) ) );
        dl = DBLocConst( dl, 0 );
        dl = DBLocOp( dl, DB_OP_MK_FP, 0 );
        break;
    case TF1_BASED_ADD:
    {   SYMBOL      sym;
        TYPE        bptr;
        TYPE        ptr;
        type_flag   flags;
        sym = (SYMBOL)btype->u.m.base;
        dl = DBLocSym( dl, symbolicDebugSymAlias( sym ) );
        bptr = TypedefModifierRemove( sym->sym_type );
        bptr = TypeModFlagsEC( bptr->of, &flags );
        ptr = GetBasicType( TYP_VOID );
        if( flags & TF1_NEAR ) {
            ptr = MakePointerTo( ptr );
            dl = DBLocOp( dl, DB_OP_POINTS, CgTypeOutput( ptr ) );
            if( bptr->id == TYP_FUNCTION ) {
                dl = symbolicDebugSetCodeSegment( dl );
            } else {
                dl = SymbolicDebugSetDataSegment( dl );
            }
        } else {
            ptr = MakeModifiedType( ptr, TF1_FAR );
            ptr = MakePointerTo( ptr );
            dl = DBLocOp( dl, DB_OP_POINTS, CgTypeOutput( ptr ) );
        }
    }   break;
    }
    dt = DBBasedPtr( CgTypeOutput( type ),
                     SymbolicDebugType( base->of, control ),
                     dl );
    DBLocFini( dl );
    return( dt );
}

dbg_type SymbolicDebugType( TYPE type, SD_CONTROL control )
/*********************************************************/
{
    FWD_INFO    *fwd_info;
    TYPE        base;
    dbg_type    dt = DBG_NIL_TYPE;

    if( type->dbgflag & TF2_NON_SYMDBG ) {
        type->dbg.handle = DBG_NIL_TYPE;
    }
    type->dbgflag = (type->dbgflag & ~TF2_NON_SYMDBG);
    // mark current location to allow breaking of cycles in type structure
    if( type->dbg.handle == DBG_FWD_TYPE ) {
        FWD_INFO *fip;
        fip = prevFwdInfo;
        while( fip->type != type ) {
            fip = fip->next;
        }
        if( fip->dn == NULL ) {
            fip->dn = DBBegName( "", fip->dt );
        }
        type->dbgflag |= TF2_SYMDBG;
        type->dbg.handle = DBForward( fip->dn );
        if( type->id == TYP_CLASS ) {
            if( ScopeHasVirtualBases( type->u.c.scope ) ) {
                fip->comp_dn = DBBegName( "", DBG_NIL_TYPE );
                type->u.c.info->dbg_no_vbases = DBForward( fip->comp_dn );
            } else {
                fip->comp_dn = fip->dn;
                type->u.c.info->dbg_no_vbases = type->dbg.handle;
            }
        }
    }

    // if we have already done this one, just return it
    if( type->dbg.handle != DBG_NIL_TYPE ) {
        return( type->dbg.handle );
    }

    // chain the cycle breaking list
    fwd_info = CarveAlloc( carveFWD_INFO );
    fwd_info->next = prevFwdInfo;
    fwd_info->type = type;
    fwd_info->dn = NULL;
    fwd_info->dt = DBG_NIL_TYPE;
    fwd_info->comp_dn = NULL;

    // typedefs require special handling
    // normally we ignore typedefs, but we want the names to come out
    if( type->id == TYP_TYPEDEF ) {
        if( ScopeType( type->u.t.scope, SCOPE_TEMPLATE_DECL ) ) return( dt );
        if( !ScopeType( type->u.t.scope, SCOPE_TEMPLATE_PARM )
         && !ScopeType( type->u.t.scope, SCOPE_TEMPLATE_PARM ) ) {
            if( !CompFlags.no_debug_type_names ) {
                if( !ScopeType( type->u.t.scope, SCOPE_TEMPLATE_INST ) ) {
                    fwd_info->dn = DBBegName( NameStr( SimpleTypeName( type ) ), DBG_NIL_TYPE );
                }
            }
            type->dbgflag |= TF2_SYMDBG;
            type->dbg.handle = DBG_FWD_TYPE;
            prevFwdInfo = fwd_info;
            dt = SymbolicDebugType( type->of, control);
            if( fwd_info->dn != NULL ) {
                dt = DBEndName( fwd_info->dn, dt );
            }
            prevFwdInfo = fwd_info->next;
            CarveFree( carveFWD_INFO, fwd_info );
            type->dbg.handle = dt;
            return( dt );
        }
    }

    // modifiers require special handling
    // debugger doesn't really care about modifiers, except for pointers
    // they are handled later on
    if( type->id == TYP_MODIFIER ) {
        dt = SymbolicDebugType( type->of, control );
        type->dbgflag |= TF2_SYMDBG;
        type->dbg.handle = dt;
        CarveFree( carveFWD_INFO, fwd_info );
        return( dt );
    }

    // handle rest of types
    base = TypedefModifierRemoveOnly( type );
    switch( base->id ) {
    case TYP_BOOL:
        dt = DBScalar( "bool", CgTypeOutput( type ) );
        break;
    case TYP_CHAR:
        dt = DBScalar( "char", CgTypeOutput( type ) );
        break;
    case TYP_SINT:
        dt = DBScalar( "int", CgTypeOutput( type ) );
        break;
    case TYP_UINT:
        dt = DBScalar( "unsigned int", CgTypeOutput( type ) );
        break;
    case TYP_USHORT:
    case TYP_SCHAR:
    case TYP_UCHAR:
    case TYP_WCHAR:
    case TYP_SSHORT:
    case TYP_SLONG:
    case TYP_ULONG:
    case TYP_SLONG64:
    case TYP_ULONG64:
    case TYP_FLOAT:
    case TYP_DOUBLE:
    case TYP_LONG_DOUBLE:
        dt = DBScalar( "", CgTypeOutput( type ) );
        break;
    case TYP_ENUM:
    {   dbg_enum    de;
        NAME        name;
        SYMBOL      sym;

        de = DBBegEnum( CgTypeOutput( type ) );
        sym = base->u.t.sym->thread;
        while( SymIsEnumeration( sym ) ) {
            DBAddConst( de, NameStr( sym->name->name ), sym->u.sval );
            sym = sym->thread;
        }
        dt = DBEndEnum( de );
        name = SimpleTypeName( base );
        if( name != NULL ) {
            DBEndName( DBBegName( NameStr( name ), scopeEnum ), dt );
        }
    }   break;
    case TYP_POINTER:
    {   type_flag bflag;
        TypeModFlags( base->of, &bflag );
        if( bflag & TF1_BASED ) {
            dt = basedPointerType( type, base, control );
        } else if( base->flag & TF1_REFERENCE ) {
            if( GenSwitches & DBG_CV ){
                dt = DBDereference( CgTypeOutput( type ),
                                    SymbolicDebugType( base->of,
                                                       control&~SD_DEREF ) );
            }else{
                if( control & SD_DEREF ) {
                    dt = DBDereference( CgTypeOutput( type ),
                                        SymbolicDebugType( base->of,
                                                           control&~SD_DEREF ) );
                    control |= SD_NO_UPDATE;
                } else {
                    dt = SymbolicDebugType( base->of, control );
                }
            }
        } else {
            dt = DBPtr( CgTypeOutput( type ), SymbolicDebugType( base->of, control ) );
        }
    }   break;
    case TYP_CLASS:
    {   NAME name;

        switch( base->flag & (TF1_UNION|TF1_STRUCT) ) {
        case TF1_STRUCT:
            fwd_info->dt = scopeStruct;
            break;
        case TF1_UNION:
            fwd_info->dt = scopeUnion;
            break;
        default:
            fwd_info->dt = scopeClass;
            break;
        }
        if( base->u.c.info->defined ) {
            type->dbgflag |= TF2_SYMDBG;
            type->dbg.handle = DBG_FWD_TYPE;
            if( !(base->flag & TF1_INSTANTIATION) ) {
                name = SimpleTypeName( base );
                if( name != NULL ) {
                    fwd_info->dn = DBBegName( NameStr( name ), fwd_info->dt );
                }
            }
            prevFwdInfo = fwd_info;
            {
                // from code generator
                dt = (dbg_type)(pointer_int)SafeRecurseCpp( (func_sr)symbolicDebugClassType, type );
            }
            if( fwd_info->dn != NULL ) {
                dt = DBEndName( fwd_info->dn, dt );
            }
            if( fwd_info->comp_dn != NULL ) {
                if( fwd_info->comp_dn == fwd_info->dn ) {
                    base->u.c.info->dbg_no_vbases = dt;
                } else {
                    base->u.c.info->dbg_no_vbases =
                        DBEndName( fwd_info->comp_dn,
                                   base->u.c.info->dbg_no_vbases );
                }
            }
            prevFwdInfo = fwd_info->next;
        } else {
            dt = SymbolicDebugType( GetBasicType( TYP_VOID ), SD_DEFAULT );
        }
    }   break;
    case TYP_BITFIELD:
        dt = SymbolicDebugType( type->of, control );
        break;
    case TYP_FUNCTION:
    {   dbg_proc    dp;
        arg_list    *alist;
        int         i;
        dp = DBBegProc( CgTypeOutput( type ),
                        SymbolicDebugType( base->of,
                                           control^SD_DEREF ) );//XOR
        alist = TypeArgList( base );
        for( i = 0 ; i < alist->num_args ; i++ ) {
            if( alist->type_list[i]->id == TYP_DOT_DOT_DOT ) break;
            DBAddParm( dp,
                       SymbolicDebugType( alist->type_list[i],
                                          control^SD_DEREF ) );//XOR
        }
        dt = DBEndProc( dp );
    }   break;
    case TYP_ARRAY:
        dt = DBIntArrayCG( CgTypeOutput( type ), base->u.a.array_size-1,
                         SymbolicDebugType( base->of, control ) );
        break;
    case TYP_VOID:
        dt = DBScalar( "", TY_DEFAULT );
        break;
    case TYP_MEMBER_POINTER:
    {   dbg_struct  ds;
        dbg_loc     dl;
        TYPE        typ;
        unsigned    offset;
        ds = DBBegStruct( CgTypeOutput( type ), 1 );
        typ = MembPtrDerefFnPtr();
        dl = DBLocInit();
        DBAddLocField( ds, dl, FIELD_ATTR_INTERNAL, 0, 0, "__fn",
                       SymbolicDebugType( typ, control ) );
        DBLocFini( dl );
        offset = CgTypeSize( typ );
        dl = DBLocInit();
        dl = DBLocConst( dl, offset );
        dl = DBLocOp( dl, DB_OP_ADD, 0 );
        typ = TypeTargetSizeT();
        DBAddLocField( ds, dl, FIELD_ATTR_INTERNAL, 0, 0, "__delta",
                       SymbolicDebugType( typ, control ) );
        DBLocFini( dl );
        offset += CgTypeSize( typ );
        dl = DBLocInit();
        dl = DBLocConst( dl, offset );
        dl = DBLocOp( dl, DB_OP_ADD, 0 );
        DBAddLocField( ds, dl, FIELD_ATTR_INTERNAL, 0, 0, "__index",
                       SymbolicDebugType( typ, control ) );
        DBLocFini( dl );
        dt = DBEndStruct( ds );
    }   break;
    default:
        CFatal( "symdbg: illegal type" );
        break;
    }
    if( !(control & SD_NO_UPDATE) ) {
        type->dbgflag |= TF2_SYMDBG;
        type->dbg.handle = dt;
    }
    CarveFree( carveFWD_INFO, fwd_info );
    return( dt );
}

static void doSymbolicDebugFundamentalType( TYPE type, void *data )
/*****************************************************************/
{
    dbg_type *dt;
    dt = data;
    if( *dt == 0 ) {
        *dt = SymbolicDebugType( type, SD_DEFAULT );
    } else {
        if( type->dbgflag & TF2_NON_SYMDBG ) {
            type->dbg.handle = DBG_NIL_TYPE;
        }
        // if we have already done this one, just return it
        if( type->dbg.handle == DBG_NIL_TYPE ) {
            type->dbgflag = (type->dbgflag & ~TF2_NON_SYMDBG) | TF2_SYMDBG;
            type->dbg.handle = *dt;
        }
    }
}

static void symbolicDebugSegmentType( void )
/******************************************/
{
    TYPE        type;
    dbg_type    dt;

    type = TypeSegmentShort();
    dt = DBScalar( "__segment", CgTypeOutput( type ) );
    type->dbgflag |= TF2_SYMDBG;
    type->dbg.handle = dt;
}

static void symbolicDebugFundamentalType( void )
/**********************************************/
{
    type_id     id;
    dbg_type    data;

    symbolicDebugSegmentType();
    for( id = TYP_FIRST_VALID ; id < TYP_LONG_DOUBLE ; ++id ) {
        data = 0;
        TypeTraverse( id, &doSymbolicDebugFundamentalType, (void *)&data );
    }
    data = 0;
    TypeTraverse( TYP_MEMBER_POINTER, &doSymbolicDebugFundamentalType, (void *)&data );
}

static boolean typedef_is_of_basic_types( TYPE type )
/***************************************************/
{
    int         i;
    arg_list    *alist;

    for(;;) {
        type = TypedefModifierRemove( type );
        if( type == NULL ) break;
        if( type->id == TYP_CLASS ) return( FALSE );
        if( type->id == TYP_MEMBER_POINTER ) return( FALSE );
        if( type->id == TYP_FUNCTION ) {
            alist = TypeArgList( type );
            for( i = 0 ; i < alist->num_args ; i++ ) {
                if( !typedef_is_of_basic_types( alist->type_list[i] ) ) {
                    return( FALSE );
                }
            }
        }
        type = type->of;
    }
    return( TRUE );
}

static void doSymbolicDebugNamedType( TYPE type, void *data )
/***********************************************************/
{
    data = data;
    if( !ScopeType( type->u.t.scope, SCOPE_TEMPLATE_PARM ) &&
        !ScopeType( type->u.t.scope, SCOPE_TEMPLATE_SPEC_PARM ) &&
        !ScopeType( type->u.t.scope, SCOPE_TEMPLATE_DECL ) ) {
        if( CompFlags.all_debug_type_names ||
            typedef_is_of_basic_types( type ) ) {
            SymbolicDebugType( type, SD_DEFAULT );
        }
    }
}

static void symbolicDebugNamedType( void )
/****************************************/
{
    int    data;

    data = 0;
    TypeTraverse( TYP_TYPEDEF, &doSymbolicDebugNamedType, (void *)&data );
}

void SymbolicDebugGenSymbol( SYMBOL sym, boolean scoped, boolean by_ref )
/***********************************************************************/
{
    TYPE        pt;
    dbg_loc     dl;
    dl = DBLocInit();
    dl = DBLocSym( dl, symbolicDebugSymAlias( sym ) );
    if( !(GenSwitches & DBG_CV) ){
        if( by_ref ) {
            pt = MakePointerTo( sym->sym_type );
            dl = DBLocOp( dl, DB_OP_POINTS, CgTypeOutput( pt ) );
            dl = SymbolicDebugSetDataSegment( dl );
        } else {
            pt = PointerTypeEquivalent( sym->sym_type );
            if( pt != NULL
             && pt->id == TYP_POINTER
             && (pt->flag & TF1_REFERENCE) ) {
                dl = DBLocOp( dl, DB_OP_POINTS, CgTypeOutput( sym->sym_type ) );
                dl = SymbolicDebugSetDataSegment( dl );
            }
        }
    }
    DBGenSym( (cg_sym_handle)sym, dl, scoped );
    DBLocFini( dl );
}

static void symbolicDebugSymbol( void )
/*************************************/
{
    SYMBOL stop;
    SYMBOL curr;

    stop = ScopeOrderedStart( GetFileScope() );
    curr = ScopeOrderedNext( stop, NULL );
    while( curr != NULL ) {
        if( ! SymIsFunctionTemplateModel( curr ) &&
            ! SymIsClassTemplateModel( curr ) &&
            ( curr->flag & (SF_INITIALIZED | SF_REFERENCED) ) &&
            SymIsData( curr ) &&
            !SymIsEnumeration( curr) &&
            !SymIsTemporary( curr ) &&
            IsCppNameInterestingDebug( curr ) ) {
            SymbolicDebugGenSymbol( curr, FALSE, FALSE );
            DbgAddrTaken( curr );
        }else if( GenSwitches & DBG_CV ){
            if( SymIsTypedef( curr ) ){
                DBTypeDef( CppNameDebug( curr ) ,
                      SymbolicDebugType( curr->sym_type, SD_DEFAULT ) );
            }
        }
        curr = ScopeOrderedNext( stop, curr );
    }
}

void SymbolicDebugEmit( void )
/****************************/
{
    if( GenSwitches & DBG_TYPES ) {
        symbolicDebugFundamentalType();
        if( !CompFlags.no_debug_type_names ) {
            symbolicDebugNamedType();
        }
    }
    if( GenSwitches & DBG_LOCALS ) {
        symbolicDebugSymbol();
    }
}

void SymbolicDebugMemberFunc( SYMBOL func, SYMBOL this_sym )
/**********************************************************/
{
    TYPE    sym_type;
    dbg_loc dl;

    if( this_sym != NULL ) {
        sym_type = this_sym->sym_type;
        dl = DBLocInit();
        dl = DBLocSym( dl, (cg_sym_handle)this_sym );
        dl = DBLocOp( dl, DB_OP_POINTS, CgTypeOutput( sym_type ) );
        dl = SymbolicDebugSetDataSegment( dl );
        DBObject( SymbolicDebugType( SymClass( func ), SD_DEFAULT ),
                  dl,
                  CgTypeOutput( sym_type ) );
        DBLocFini( dl );
    } else {
        DBObject( SymbolicDebugType( SymClass( func ), SD_DEFAULT ),
                  NULL,
                  TY_DEFAULT );
    }
}
