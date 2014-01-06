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

#include <setjmp.h>
#include <assert.h>

#include "preproc.h"
#include "cgfront.h"
#include "template.h"
#include "codegen.h"
#include "pragdefn.h"
#include "cgback.h"
#include "ring.h"
#include "dbg.h"
#include "dw.h"
#include "cppdwarf.h"
#include "dwarfdbg.h"
#include "browsio.h"
#include "vfun.h"
#include "dbgsupp.h"
#include "pcheader.h"
#include "name.h"
#include "icopmask.h"
#include "fmttype.h"

#define _typeHasForwardDwarfHandle( type ) \
( ((type)->dbgflag & (TF2_SYMDBG|TF2_DWARF_FWD)) == (TF2_SYMDBG|TF2_DWARF_FWD) )
#define _typeHasDefinedDwarfHandle( type ) \
( ((type)->dbgflag & (TF2_SYMDBG|TF2_DWARF_DEF)) == (TF2_SYMDBG|TF2_DWARF_DEF) )

#define _typeHasPCHDwarfHandle( type ) \
( CompFlags.pch_debug_info_read && ((type)->dbgflag & TF2_DBG_IN_PCH ) != 0 )

typedef enum
{   DC_RETURN           = 0x01,         // this is a return type
    DC_DEFINE           = 0x02,         // generate definition
    DC_FAKE             = 0x04,         // flag as artificial
    DC_DEFAULT          = 0x00          // default behaviour
} DC_CONTROL;


static uint      dwarfMarkInternalName( SYMBOL, uint );
static boolean   dwarfClassInfoFriend( TYPE, boolean );
static boolean   dwarfClassInfo( TYPE );
static dw_handle dwarfClass( TYPE, DC_CONTROL );
static dw_handle dwarfEnum( TYPE, DC_CONTROL );
static dw_handle dwarfTypedef( TYPE, DC_CONTROL );
static dw_handle dwarfTypeArray( TYPE );
static dw_handle dwarfTypeFunction( TYPE );
static dw_handle dwarfTypeModifier( TYPE );
static dw_handle dwarfTypeMemberPointer( TYPE );
static dw_handle dwarfType( TYPE, DC_CONTROL );
static void      dwarfLocation( SYMBOL );
static dw_handle dwarfData( SYMBOL );
static dw_handle dwarfFunctionDefine( SYMBOL, CGFILE * );
static dw_handle dwarfFunction( SYMBOL, DC_CONTROL );
static dw_handle dwarfSymbol( SYMBOL, DC_CONTROL );
static void      dwarfEmitSymbolScope( SCOPE );

static TYPE             vf_FieldType;
static unsigned         vf_FieldTypeSize;
static TYPE             pvf_FieldType;
static TYPE             vb_FieldType;
static unsigned         vb_FieldTypeSize;
static TYPE             pvb_FieldType;

static bool             InDebug;
static dw_client        Client;
static dw_loc_handle    dummyLoc;
static SRCFILE          currentFile;

static void virtTblTypes( void ) {
    pvf_FieldType = MakeVFTableFieldType( TRUE );
    vf_FieldType = PointerTypeEquivalent( pvf_FieldType )->of;
    vf_FieldTypeSize = CgTypeSize( vf_FieldType );

    pvb_FieldType = MakeVBTableFieldType( TRUE );
    vb_FieldType = PointerTypeEquivalent( pvb_FieldType )->of;
    vb_FieldTypeSize = CgTypeSize( vb_FieldType );
}

static void initDwarf( bool debug, dsi_control control ) {
    virtTblTypes();
    InDebug = debug;
    currentFile = NULL;
    DbgSuppInit( control );
}

static void type_reset( TYPE type )
/*********************************/
{
    if( InDebug ) {
        if( !(type->dbgflag & TF2_SYMDBG ) ) {
            if(( type->dbgflag & TF2_PCH_DBG_EXTERN ) && CompFlags.pch_debug_info_read ) {
                type->dbgflag |= TF2_SYMDBG | TF2_DWARF_DEF;
                type->dbg.handle = DWRefPCH( Client, type->dbg.pch_handle );
            } else {
                type->dbgflag = (type->dbgflag & ~TF2_DWARF) | TF2_SYMDBG;
            }
        }
    }
}

static void type_update( TYPE type, type_dbgflag mask, dw_handle dh )
/*******************************************************************/
{
    type->dbgflag = (type->dbgflag & ~TF2_DWARF ) | mask;
    type->dbg.handle = dh;
}

static void sym_reset( SYMBOL sym )
/*********************************/
{
    if( InDebug ) {
        if( !(sym->flag2 & SF2_SYMDBG) ) {
            sym->flag2 = (sym->flag2 & ~SF2_DW_HANDLE) | SF2_SYMDBG;
        }
    }
}

static void sym_update( SYMBOL sym, symbol_flag2 mask, dw_handle dh )
/*******************************************************************/
{
    #ifndef NDEBUG
    if( sym->flag2 & SF2_CG_HANDLE ) {
        DumpSymbol( sym );
        CFatal( "dwarf: handle for sym busy" );
    }
    #endif
    sym->flag2 |= mask;
    if( sym->flag2 & SF2_TOKEN_LOCN ) {
        sym->locn->u.dwh = dh;
    }
}

static void dwarfTokenLocation( TOKEN_LOCN *locn )
/************************************************/
{

    if( currentFile != locn->src_file ) {
        DWDeclFile( Client, SrcFileFullName( locn->src_file ) );
        currentFile = locn->src_file;
    }
    DWDeclPos( Client, locn->line, locn->column );
}

static void dwarfLocation( SYMBOL sym )
/*************************************/
{
    if( sym->flag2 & SF2_TOKEN_LOCN ) {
        dwarfTokenLocation( &sym->locn->tl );
    }
}

static uint dwarfMarkInternalName( SYMBOL sym, uint flags )
/*********************************************************/
{
    if( !IsCppNameInterestingDebug( sym ) ) {
        flags |= DW_FLAG_ARTIFICIAL;
    }
    return( flags );
}

static SYMBOL dwarfDebugSymAlias( SYMBOL sym )
/********************************************/
{
    SYMBOL check;

    sym = SymDeAlias( sym );
    check = SymIsAnonymous( sym );
    if( check != NULL ) {
        sym = check;
    }
    return( sym );
}

static dw_loc_handle dwarfDebugStaticLoc( SYMBOL sym )
/***********************************************/
{
    TYPE        pt;
    dw_loc_id       locid;
    dw_loc_handle   dl;


    locid = DWLocInit( Client );
    sym = dwarfDebugSymAlias( sym );
    DWLocStatic( Client, locid, (dw_sym_handle) sym );
    pt = PointerTypeEquivalent( sym->sym_type );
    if( pt != NULL && pt->id == TYP_POINTER && (pt->flag & TF1_REFERENCE) ) {
        DWLocOp0( Client, locid, DW_LOC_deref );
    }
    dl = DWLocFini( Client, locid );
    DbgAddrTaken( sym );
    return( dl );
}

#if _INTEL_CPU
static dw_loc_handle dwarfDebugStaticSeg( SYMBOL sym )
/***********************************************/
{
    dw_loc_id       locid;
    dw_loc_handle   dl;


    locid = DWLocInit( Client );
    sym = dwarfDebugSymAlias( sym );
    DWLocSegment( Client, locid, (dw_sym_handle) sym );
    dl = DWLocFini( Client, locid );
    DbgAddrTaken( sym );
    return( dl );
}
#endif

static uint  dwarfAddressClassFlags( TYPE type ) {
/**********************************/
    uint    flags;
    cg_type ptr_type;
    cg_type offset_type;

    ptr_type = CgTypeOutput( type );
    switch( ptr_type ) {
    case TY_HUGE_POINTER:
        flags = DW_PTR_TYPE_HUGE16;
        break;
    case TY_LONG_POINTER:
    case TY_LONG_CODE_PTR:
        offset_type = CgTypeOffset();
        if( offset_type == TY_UINT_4 ) {
            flags = DW_PTR_TYPE_FAR32;
        } else {
            flags = DW_PTR_TYPE_FAR16;
        }
        break;
    case TY_NEAR_POINTER:
    case TY_NEAR_CODE_PTR:
    case TY_POINTER:
#if _CPU == _AXP
        flags = DW_PTR_TYPE_DEFAULT;
#else
        if( IsFlat() ) {
            flags = DW_PTR_TYPE_DEFAULT;
        } else {
            offset_type = CgTypeOffset();
            if( offset_type == TY_UINT_4 ) {
                flags = DW_PTR_TYPE_NEAR32;
            } else {
                flags = DW_PTR_TYPE_NEAR16;
            }
        }
#endif
        break;
    default:
        CFatal( "dwarf: unknown pointer type" );
    }
    return( flags );
}

static dw_handle dwarfDebugMemberFuncDef( CLASSINFO *info, SYMBOL sym )
/********************************************************************/
{
    TYPE        base;
    type_flag   tf;
    dw_handle   return_dh;
    dw_handle   dh;
    uint        call_type;
    uint        flags;
    dw_loc_id       locid;
    dw_loc_handle   dl;
    dw_loc_handle   dl_virt;
    dw_loc_handle   dl_seg;
    char           *name;

    call_type = 0;
    base = TypeModFlags( sym->sym_type, &tf );
    flags = dwarfAddressClassFlags( sym->sym_type );
    if( base->flag & TF1_INLINE ) {
        flags |= DW_FLAG_DECLARED_INLINE;
    }
    if( tf & TF1_NEAR ) {
        call_type |= DW_SB_NEAR_CALL;
    }
    if( tf & TF1_FAR ) {
        call_type |= DW_SB_FAR_CALL;
    }
    if( tf & TF1_FAR16 ) {
        call_type |= DW_SB_FAR16_CALL;
    }
    flags |= DW_FLAG_PROTOTYPED;
    if( SymIsStaticMember( sym ) ) {
        flags |= DW_SUB_STATIC;
    }
    if( sym->flag & SF_PRIVATE ) {
        flags |= DW_FLAG_PRIVATE;
    } else if( sym->flag & SF_PROTECTED ) {
        flags |= DW_FLAG_PROTECTED;
    } else {
        flags |= DW_FLAG_PUBLIC;
    }
    flags = dwarfMarkInternalName( sym, flags );
    flags |= DW_FLAG_DECLARATION;
    return_dh = dwarfType( base->of, DC_DEFAULT );
    name = CppNameDebug( sym );
    if( SymIsVirtual( sym ) ) {
        flags |= DW_FLAG_VIRTUAL;
        locid = DWLocInit( Client );
        DWLocConstS( Client, locid, info->vf_offset );
        DWLocOp0( Client, locid, DW_LOC_plus );
        if( DefaultMemoryFlag( pvf_FieldType ) != TF1_NEAR ) {
            DWLocOp0( Client, locid, DW_LOC_xderef );
        } else {
            DWLocOp0( Client, locid, DW_LOC_deref );
        }
        DWLocConstS( Client, locid, ( sym->u.member_vf_index - 1 ) * vf_FieldTypeSize );
        DWLocOp0( Client, locid, DW_LOC_plus );
        if( DefaultMemoryFlag( vf_FieldType ) != TF1_NEAR ) {
            DWLocOp0( Client, locid, DW_LOC_xderef );
        } else {
            DWLocOp0( Client, locid, DW_LOC_deref );
        }
        dl_virt = DWLocFini( Client, locid );
        dh = DWBeginVirtMemFuncDecl( Client,
                   return_dh,
                   dl_virt,
                   name,
                   flags );
        DWLocTrash( Client, dl_virt );
    } else {
        dl = dwarfDebugStaticLoc( sym );
    #if _CPU == _AXP
        dl_seg = NULL;
    #else
        if( IsFlat() ) {
            dl_seg = NULL;
        } else {
            dl_seg =  dwarfDebugStaticSeg( sym );
        }
    #endif
        dh = DWBeginMemFuncDecl( Client,
                   return_dh,
                   dl_seg,
                   dl,
                   name,
                   flags );
        DWLocTrash( Client, dl );
        if( dl_seg != NULL ) {
            DWLocTrash( Client, dl_seg );
        }
    }
    return( dh );
}
#if 0
static void dwarfPumpArgTypes( TYPE type )
/*************************************/
{ // force out types for args
    arg_list    *alist;
    int         i;
    TYPE        p;

    alist = TypeArgList( type );
    for( i = 0 ; i < alist->num_args ; i++ ) {
        if( alist->type_list[i]->id != TYP_DOT_DOT_DOT ) {
            for( p = alist->type_list[i]; p != NULL; p = p->of ) {
                if( p->id == TYP_TYPEDEF ) {
                    SCOPE sc;

                    sc = p->u.t.scope;
                    if( sc->id == SCOPE_TEMPLATE_PARM ) {
                        dwarfType( p, DC_DEFINE | DC_FAKE );
                    }
                }
            }
        }
    }
}
#endif

static boolean dwarfClassInfoFriend( TYPE type, boolean addfriend )
/*****************************************************************/
{
    dw_handle   dh;
    FRIEND      *friend;
    boolean     check_friends;

    check_friends = FALSE;
    if( !InDebug ) {    /* TODO: if debug need handle */
        RingIterBeg( ScopeFriends( type->u.c.scope ), friend ) {
            if( FriendIsSymbol( friend ) ) {
                check_friends = TRUE;
                if( addfriend ) {
                    dh = dwarfSymbol( FriendGetSymbol( friend ), DC_DEFAULT );
                    if( dh ) {
                        DWAddFriend( Client, dh );
                    }
                } else {
                    dh = dwarfSymbol( FriendGetSymbol( friend ), DC_DEFINE );
                }
            }
        } RingIterEnd( friend )
    }
    return( check_friends );
}

static boolean dwarfClassInfo( TYPE type )
/****************************************/
{
    dw_handle       dh;
    SYMBOL          stop, curr;
    BASE_CLASS     *base;
    dw_loc_id       locid;
    dw_loc_handle   dl;
    CLASSINFO      *info;
    boolean         check_friends;

    // define any template typedefs
    for( ;; ) {
        SCOPE scope;
        if( (type->flag & TF1_INSTANTIATION) == 0 ) break;
        scope = type->u.c.scope->enclosing;
        if( !ScopeType( scope, SCOPE_TEMPLATE_INST ) ) break;
        scope = scope->enclosing;
        if( !ScopeType( scope, SCOPE_TEMPLATE_PARM ) ) break;
        stop = ScopeOrderedStart( scope );
        curr = ScopeOrderedNext( stop, NULL );
        while( curr != NULL ) {
            if( SymIsTypedef( curr ) ) {
                dwarfTypedef( curr->sym_type, DC_DEFINE | DC_FAKE );
            }
            curr = ScopeOrderedNext( stop, curr );
        }
        break;
    }

    // define all the bases
    info = type->u.c.info;
    RingIterBeg( ScopeInherits( type->u.c.scope ), base ) {
        uint flags = 0;
        switch( base->flag & IN_ACCESS_SPECIFIED ) {
        case IN_PRIVATE:
            flags |= DW_FLAG_PRIVATE;
            break;
        case IN_PROTECTED:
            flags |= DW_FLAG_PROTECTED;
            break;
        case IN_PUBLIC:
            flags |= DW_FLAG_PUBLIC;
            break;
        default:
            if( type->flag & TF1_UNION || type->flag & TF1_STRUCT ) {
                flags |= DW_FLAG_PUBLIC;
            } else {
                flags |= DW_FLAG_PRIVATE;
            }
            break;
        }
        if( _IsDirectVirtualBase( base ) ) {
            flags |= DW_FLAG_VIRTUAL;
            locid = DWLocInit( Client );
            DWLocConstS( Client, locid, info->vb_offset );
            DWLocOp0( Client, locid, DW_LOC_plus );
            DWLocOp0( Client, locid, DW_LOC_dup );
            DWLocOp0( Client, locid, DW_LOC_deref );
            DWLocConstS( Client, locid, base->vb_index * vb_FieldTypeSize );
            DWLocOp0( Client, locid, DW_LOC_plus );
            DWLocOp0( Client, locid, DW_LOC_deref );
            if( info->vb_offset != 0 ) {
                DWLocConstS( Client, locid, info->vb_offset );
                DWLocOp0( Client, locid, DW_LOC_plus );
            }
            DWLocOp0( Client, locid, DW_LOC_plus );
            dl = DWLocFini( Client, locid );
            DWAddInheritance( Client,
                              dwarfType( base->type, DC_DEFAULT ),
                              dl,
                              flags );
            DWLocTrash( Client, dl );
        } else if( _IsDirectNonVirtualBase( base ) ) {
            locid = DWLocInit( Client );
            DWLocConstS( Client, locid, base->delta );
            DWLocOp0( Client, locid, DW_LOC_plus );
            dl = DWLocFini( Client, locid );
            DWAddInheritance( Client,
                              dwarfType( base->type, DC_DEFAULT ),
                              dl,
                              flags );
            DWLocTrash( Client, dl );
        }
    } RingIterEnd( base )

    // forward reference all the friends
    check_friends = dwarfClassInfoFriend( type, TRUE );

    // hidden data members
    if( info->has_vbptr ) {
        locid = DWLocInit( Client );
        DWLocConstS( Client, locid, info->vb_offset );
        DWLocOp0( Client, locid, DW_LOC_plus );
        dl = DWLocFini( Client, locid );
        dh = dwarfType( pvb_FieldType, DC_DEFAULT );/* CppName no reentrant */
        dh = DWAddField( Client,
                         dh,
                         dl,
                         "__vbptr",
                         DW_FLAG_ARTIFICIAL );
        DWLocTrash( Client, dl );
    }
    if( info->has_vfptr ) {
        locid = DWLocInit( Client );
        DWLocConstS( Client, locid, info->vf_offset );
        DWLocOp0( Client, locid, DW_LOC_plus );
        dl = DWLocFini( Client, locid );
        dh = dwarfType( pvf_FieldType, DC_DEFAULT );/* CppName no reentrant */
        dh = DWAddField( Client,
                         dh,
                         dl,
                         "__vfptr",
                         DW_FLAG_ARTIFICIAL );
        DWLocTrash( Client, dl );
    }

    // define all the fields
    stop = ScopeOrderedStart( type->u.c.scope );
    curr = ScopeOrderedNext( stop, NULL );
    while( curr != NULL ) {
        sym_reset( curr );
        if( !IsCppNameInterestingDebug( curr ) ) {
             dh = 0;
        } else if( !InDebug && (curr->flag2 & SF2_TOKEN_LOCN)==0 )  {
             dh = 0;
        } else if( SymIsClassDefinition( curr ) ) {
            dh = dwarfSymbol( curr, DC_DEFINE );
        } else if( SymIsEnumDefinition( curr ) ) {
            dh = dwarfSymbol( curr, DC_DEFINE );
        } else if( SymIsEnumeration( curr ) ) {
            // do nothing, handled by SymIsEnumDefinition
            dh = 0;
        } else if( SymIsFunction( curr ) ) {
//          dwarfPumpArgTypes( curr->sym_type );
            if( InDebug ) { /* gen a short defn */
                dh = dwarfDebugMemberFuncDef( info, curr );
                dh = 0;   // if debug we don't want to clash with cg_handle
            } else {
                dh = dwarfSymbol( curr, DC_DEFINE );
            }
        } else if( SymIsTypedef( curr ) ) {
            dh = dwarfSymbol( curr, DC_DEFINE );
        } else if( SymIsData( curr ) ) {
            uint            flags;
            TYPE            pt;

            if( !InDebug ) {
                dwarfLocation( curr );
            }
            if( curr->flag & SF_PRIVATE ) {
                flags = DW_FLAG_PRIVATE;
            } else if( curr->flag & SF_PROTECTED ) {
                flags = DW_FLAG_PROTECTED;
            } else {
                flags = DW_FLAG_PUBLIC;
            }
            if( SymIsStaticMember( curr ) ) {
                dw_loc_handle   dl_seg;

                flags |= DW_FLAG_STATIC;
                dl_seg = NULL;
                if( InDebug ) {
                    dl = dwarfDebugStaticLoc( curr );
#if _INTEL_CPU
                    if( !IsFlat() ) {
                        dl_seg =  dwarfDebugStaticSeg( curr );
                    }
#endif
                } else {    /* fake up loc for browser */
                    locid = DWLocInit( Client );
                    dl = DWLocFini( Client, locid );
                }
                dh = dwarfType( curr->sym_type, DC_DEFAULT );/* CppName no reentrant */
                dh = DWVariable( Client,
                         dh,
                         dl,
                         0,
                         dl_seg,
                         CppNameDebug( curr ),
                         0,
                         flags );
                if( dl_seg != NULL ) {
                    DWLocTrash( Client, dl_seg );
                }
                if( InDebug ) {
                    dh = 0;   // if debug we don't want to clash with cg_handle
                }
            } else if( SymIsThisDataMember( curr ) ) {
                TYPE sym_type = curr->sym_type;
                TYPE btf = TypedefModifierRemoveOnly( sym_type );

                locid = DWLocInit( Client );
                DWLocOp( Client, locid, DW_LOC_plus_uconst, curr->u.member_offset );
                pt = PointerTypeEquivalent( sym_type );
                if( pt && pt->id == TYP_POINTER && (pt->flag & TF1_REFERENCE) ) {
                     DWLocOp0( Client, locid, DW_LOC_deref );
                }
                dl = DWLocFini( Client, locid );
                if( btf->id == TYP_BITFIELD ) {
                    int  strt;
                    int  tsize;

                    tsize = CgTypeSize( btf->of );
                    strt = 8*tsize-(btf->u.b.field_start+btf->u.b.field_width);
                    dh = dwarfType( sym_type, DC_DEFAULT );/* CppName no reentrant */
                    dh = DWAddBitField( Client,
                                   dh,
                                   dl,
                                   tsize,
                                   strt,
                                   btf->u.b.field_width,
                                   CppNameDebug( curr ),
                                   flags );
                } else {
                    dh = dwarfType( sym_type, DC_DEFAULT );/* CppName no reentrant */
                    dh = DWAddField( Client,
                                dh,
                                dl,
                                CppNameDebug( curr ),
                                flags );
                }
            }
            DWLocTrash( Client, dl );
        } else if( curr->id == SC_ACCESS ) {
            // fixme: access modifiers ignored for now
            dh = 0;
        } else {
            dh = 0;
            DbgStmt( DumpSymbol( curr ) );
            DbgStmt( CFatal( "dwarf: illegal member" ) );
        }
        if( dh != 0 ) {
            sym_reset( curr );
            sym_update( curr, SF2_DW_HANDLE_DEF, dh );
        }
        curr = ScopeOrderedNext( stop, curr );
    }
    return( check_friends );
}

static dw_handle dwarfClass( TYPE type, DC_CONTROL control )
/**********************************************************/
{
    dw_handle       dh;
    const char      *name;
    boolean         defined;
    boolean         check_friends;

    type_reset( type );
    if( type->dbgflag & TF2_DWARF ) {
        dh = type->dbg.handle;
    } else {
        uint    kind;
        if( type->flag & TF1_UNION ) {
            kind = DW_ST_UNION;
        } else if( type->flag & TF1_STRUCT ) {
            kind = DW_ST_STRUCT;
        } else {
            kind = DW_ST_CLASS;
        }
        dh = DWStruct( Client, kind );
        type_update( type, TF2_DWARF_FWD, dh );
    }
    if( type->dbgflag & TF2_DWARF_DEF ) {
        return( dh );
    }
    defined = (type->u.c.info->defined != 0);
    if( !defined || type->u.c.info->unnamed || (control & DC_DEFINE) ) {
        type_update( type, TF2_DWARF_DEF, dh );
        if( (type->u.c.info->anonymous == 0) && (type->u.c.info->unnamed == 0) ) {
            name = NameStr( SimpleTypeName( type ) );
        } else {
            name = NULL;
        }
        DWBeginStruct( Client,
                       dh,
                       CgTypeSize( type ),
                       name,
                       0,
                       (defined ? 0 : DW_FLAG_DECLARATION ) );
        check_friends = FALSE;
        if( defined ) {
            check_friends = dwarfClassInfo( type );
        }
        DWEndStruct( Client );
        if( check_friends ) {
            dwarfClassInfoFriend( type, FALSE );
        }
    }
    return( dh );
}

static dw_handle dwarfEnum( TYPE type, DC_CONTROL control )
/*********************************************************/
{
    dw_handle       dh;

    type_reset( type );
    if( type->dbgflag & TF2_DWARF ) {
        dh = type->dbg.handle;
    } else {
        dh = DWHandle( Client, DW_ST_NONE );
        type_update( type, TF2_DWARF_FWD, dh );
    }
    if( type->dbgflag & TF2_DWARF_DEF ) {
        return( dh );
    }

    if( (type->flag & TF1_UNNAMED) || (control & DC_DEFINE) ) {     //check for enum{ }foo
        SYMBOL      sym;
        type_update( type, TF2_DWARF_DEF, dh );
        DWHandleSet( Client, dh );
        dh = DWBeginEnumeration( Client, CgTypeSize( type->of ), NameStr( SimpleTypeName( type ) ), 0, 0 );
        sym = type->u.t.sym->thread;
        while( SymIsEnumeration( sym ) ) {
            // fixme: enums need to be in reverse order
            DWAddConstant( Client, sym->u.sval, NameStr( sym->name->name ) );
            sym = sym->thread;
        }
        DWEndEnumeration( Client );
    }
    return( dh );
}

static dw_handle dwarfTypedef( TYPE type, DC_CONTROL control )
/************************************************************/
{
    dw_handle       dh;

    type_reset( type );
    if( type->dbgflag & TF2_DWARF ) {
        dh = type->dbg.handle;
    } else {
        dh = DWHandle( Client, DW_ST_NONE );
        type_update( type, TF2_DWARF_FWD, dh );
    }
    if( type->dbgflag & TF2_DWARF_DEF ) {
        return( dh );
    }

    if( control & DC_DEFINE ) {
        dw_handle of_hdl;
        TYPE      of_type;
        SYMBOL    sym;
        uint      flags;
        type_update( type, TF2_DWARF_DEF, dh );
        sym = type->u.t.sym;
        of_type = StructType( type->of );
        if( (of_type == type->of)
         && (of_type->u.c.info->unnamed)
         && (sym->name->containing == of_type->u.c.scope->enclosing) ) {
         // typedef struct { ... } foo
            of_hdl = dwarfClass( of_type, DC_DEFINE );
        } else {
            of_type = EnumType( type->of );
            if( (of_type == type->of)
             && (of_type->flag & TF1_UNNAMED)
             && (sym->name->containing == of_type->u.t.scope) ) {
             // typedef enum { ... } foo
                of_hdl = dwarfEnum( of_type, DC_DEFINE );
            } else {
                of_hdl = dwarfType( type->of, DC_DEFAULT );
            }
        }
        flags = 0;
        if( control & DC_FAKE ) {
            flags |= DW_FLAG_ARTIFICIAL;
        }
        DWHandleSet( Client, dh );
        dh = DWTypedef( Client,
                        of_hdl,
                        NameStr( SimpleTypeName( type ) ),
                        0,
                        flags );
    }
    return( dh );
}

static dw_handle dwarfTypeArray( TYPE type )
/******************************************/
{
    dw_handle           dh;

    type_reset( type );
    if( type->dbgflag & TF2_DWARF )
        return( type->dbg.handle );
    dh = DWSimpleArray( Client, dwarfType( type->of, DC_DEFAULT ), type->u.a.array_size );
    type_update( type, TF2_DWARF_DEF, dh );
    return( dh );
}

static dw_handle dwarfTypeFunction( TYPE type )
/*********************************************/
{
    dw_handle   dh;
    arg_list    *alist;
    int         i;
    uint        flags;

    type_reset( type );
    if( type->dbgflag & TF2_DWARF )
        return( type->dbg.handle );
    flags = dwarfAddressClassFlags( type );
    flags |= DW_FLAG_DECLARATION | DW_FLAG_PROTOTYPED;
    dh = DWBeginSubroutineType( Client,
                                dwarfType( type->of, DC_RETURN ),
                                NULL,
                                0,
                                flags );
    type_update( type, TF2_DWARF_DEF, dh );
    alist = TypeArgList( type );
    for( i = 0 ; i < alist->num_args ; i++ ) {
        if( alist->type_list[i]->id == TYP_DOT_DOT_DOT ) {
            DWAddEllipsisToSubroutineType( Client );
        } else {
            DWAddParmToSubroutineType( Client,
                                       dwarfType( alist->type_list[i], DC_DEFAULT ),
                                       dummyLoc,
                                       dummyLoc,
                                       NULL );
        }
    }
    DWEndSubroutineType( Client );
    return( dh );
}

static bool dwarfRefSymLoc( dw_loc_id locid, SYMBOL sym ) {
/** make a loc for sym return true if segmented**/
    bool ret;

    ret = FALSE;
    if( SymIsAutomatic( sym ) ) {
        DFDwarfLocal( Client, locid, sym );
    } else {
#if _INTEL_CPU
        if( !IsFlat() ) {  /* should check Client */
            DWLocSegment( Client, locid, (dw_sym_handle)sym );
            ret = TRUE;
        }
#endif
        DWLocStatic( Client, locid, (dw_sym_handle)sym );
    }
    return( ret );
}

/* for based pointers I use the hokey convention of
   | offset | seg | on location stack sets offset and seg
   for the based pointer
*/
static dbg_type dwarfBasedPointerType( TYPE type, uint flags ) {
/**************************************************************/
    dw_loc_id       locid;
    dw_loc_handle   dl_seg;
    TYPE            btype;
    SYMBOL          sym;
    dw_handle       dh;
    int             dref;

    locid = DWLocInit( Client );
    btype = BasedType( type->of );
    switch( btype->flag & TF1_BASED ) {
    case TF1_BASED_STRING:
    //   __based(__segname("_name"))
    // on stack top->| offset 0 | seg sym |
        if( SegmentFindBased( btype ) == SEG_CODE ) {
#if _INTEL_CPU
            if( !IsFlat() ) {  /* should check Client */
                DWLocSegment( Client, locid, (dw_sym_handle)DefaultCodeSymbol );
                DbgAddrTaken( DefaultCodeSymbol );
            }
#endif
        } else {
            // fixme: this should handle segments that aren't
            // in DGROUP by defining a symbol in that segment and
            // using it.  For now all such pointers just go to
            // DGROUP.  Note that defining a symbol in that segment
            // may require defining that segment.
#if _INTEL_CPU
            if( !IsFlat() ) {  /* should check Client */
                DWLocSegment( Client, locid, (dw_sym_handle) DefaultDataSymbol );
                DefaultDataSymbol->flag |= SF_REFERENCED;
            }
#endif
        }
        DWLocConstS( Client, locid, 0 );
        break;
    case TF1_BASED_SELF:
    //  char __based((__segment)__self) *sp; -- inherits base from expression
    // on stack top->| offset 0 |
        DWLocConstS( Client, locid, 0 );
        break;
    case TF1_BASED_VOID:
    //   char __based(void) *vp;           -- just an offset (based on nothing)
    // on stack top->| offset 0 | seg 0 |
        DWLocConstS( Client, locid, 0 );
        DWLocConstS( Client, locid, 0 );
        break;
    case TF1_BASED_FETCH:
   // char __based(seg_var) *bp;        -- fetch segment from seg_var
   // on stack top->| offset 0 | seg (seg_var) |
        sym = dwarfDebugSymAlias( btype->u.m.base );
        DbgAddrTaken( sym );
        dref = DW_LOC_deref_size;
        if( dwarfRefSymLoc( locid, sym ) ) {
            dref = DW_LOC_xderef_size;
        } else {
            dref = DW_LOC_deref_size;
        }
        DWLocOp( Client, locid, dref, 2 );
        DWLocConstS( Client, locid, 0 );
        break;
    case TF1_BASED_ADD:
    // char __based(fp) *pp;             -- add offset to fp to produce pointer
    // on stack top->| offset (fp) | seg (fp) | if far
    // on stack top->| offset (fp) | if flat
    {
        TYPE        bptr;
        type_flag   bflags;

        sym = dwarfDebugSymAlias( btype->u.m.base );
        DbgAddrTaken( sym );
        bptr = TypedefModifierRemove( sym->sym_type );
        bptr = TypeModFlagsEC( bptr->of, &bflags );
        if( bflags & TF1_NEAR ) {
#if _INTEL_CPU
            if( !IsFlat() ) {  /* should check Client */
                if( bptr->id == TYP_FUNCTION ) {
                    DWLocSegment( Client, locid, (dw_sym_handle)DefaultCodeSymbol );
                    DbgAddrTaken( DefaultCodeSymbol );
                } else {
                    DWLocSegment( Client, locid, (dw_sym_handle) DefaultDataSymbol );
                    DefaultDataSymbol->flag |= SF_REFERENCED;
                }
            }
#endif
            if( dwarfRefSymLoc( locid, sym ) ) {
                dref = DW_LOC_xderef_size;
            } else {
                dref = DW_LOC_deref_size;
            }
            DWLocOp( Client, locid, dref, CgTypeOffset() );
        } else {
            if( dwarfRefSymLoc( locid, sym ) ) {
                dref = DW_LOC_xderef_size;
            } else {
                dref = DW_LOC_deref_size;
            }
            DWLocOp( Client, locid, DW_LOC_plus_uconst, 2 ); // segment
            DWLocOp( Client, locid, dref, 2 );
            dwarfRefSymLoc( locid, sym ); /*  offset on top  */
            DWLocOp( Client, locid, dref, CgTypeOffset() );
        }
    }   break;
    }
    dl_seg = DWLocFini( Client, locid );
    dh = DWBasedPointer( Client, dwarfType( type->of, DC_DEFAULT ), dl_seg, flags );
    DWLocTrash( Client, dl_seg );
    type_update( type, TF2_DWARF_DEF, dh );
    return( dh );
}

static dw_handle dwarfTypeModifier( TYPE type )
/*********************************************/
{
    dw_handle   dh;
    uint        modtype = 0;

    if( type->flag & TF1_CONST ) {
        modtype |= DW_MOD_CONSTANT;
    }
    if( type->flag & TF1_VOLATILE ) {
        modtype |= DW_MOD_VOLATILE;
    }
    if( modtype != 0 ) {
        dh = DWModifier( Client, dwarfType( type->of, DC_DEFAULT ), modtype );
    } else {
        dh = dwarfType( type->of, DC_DEFAULT );
    }
    type_update( type, TF2_DWARF_DEF, dh );
    return( dh );
}

static dw_handle dwarfTypeMemberPointer( TYPE type )
/**************************************************/
{
    dw_handle   dh;
    dw_handle   host_dh;
    TYPE        host;

    host = type->u.mp.host;
    if( host == NULL ) {
        host_dh = dwarfType( GetBasicType( TYP_VOID ), DC_DEFAULT );
    } else {
        host_dh = dwarfType( host, DC_DEFAULT );
    }
    dh = DWMemberPointer( Client,
                          host_dh,
                          dummyLoc,
                          dwarfType( type->of, DC_DEFAULT ),
                          NULL,
                          DW_FLAG_DECLARATION );
    type_update( type, TF2_DWARF_DEF, dh );
    return( dh );
}


static dw_handle dwarfType( TYPE type, DC_CONTROL control )
/*********************************************************/
{
    dw_handle       dh = 0;

    type_reset( type );
    if( control & DC_DEFINE ) {
        if( _typeHasDefinedDwarfHandle( type ) ) {
            return( type->dbg.handle );
        }
        // need to define type (it probably is forward ref'd)
    } else {
        if( type->dbgflag & TF2_DWARF ) {
            return( type->dbg.handle );
        }
    }
    switch( type->id ) {
    case TYP_ERROR:
        dh = DWFundamental( Client, FormatErrorType( type ), DW_FT_UNSIGNED_CHAR, 1 );
        type_update( type, TF2_DWARF_DEF, dh );
        break;
    case TYP_BOOL:
        dh = DWFundamental( Client, "bool", DW_FT_BOOLEAN, CgTypeSize( type ) );
        type_update( type, TF2_DWARF_DEF, dh );
        break;
    case TYP_CHAR:
        if( type->of->id == TYP_SCHAR ) {
            dh = DWFundamental( Client, "char", DW_FT_SIGNED_CHAR, CgTypeSize( type ) );
        } else {
            dh = DWFundamental( Client, "char", DW_FT_UNSIGNED_CHAR, CgTypeSize( type ) );
        }
        type_update( type, TF2_DWARF_DEF, dh );
        break;
    case TYP_SCHAR:
        dh = DWFundamental( Client, "signed char", DW_FT_SIGNED_CHAR, CgTypeSize( type ) );
        type_update( type, TF2_DWARF_DEF, dh );
        break;
    case TYP_UCHAR:
        dh = DWFundamental( Client, "unsigned char", DW_FT_UNSIGNED_CHAR, CgTypeSize( type ) );
        type_update( type, TF2_DWARF_DEF, dh );
        break;
    case TYP_WCHAR:
        dh = DWFundamental( Client, "wchar_t", DW_FT_UNSIGNED_CHAR, CgTypeSize( type ) );
        type_update( type, TF2_DWARF_DEF, dh );
        break;
    case TYP_SSHORT:
        dh = DWFundamental( Client, "signed short", DW_FT_SIGNED, CgTypeSize( type ) );
        type_update( type, TF2_DWARF_DEF, dh );
        break;
    case TYP_USHORT:
        dh = DWFundamental( Client, "unsigned short", DW_FT_UNSIGNED, CgTypeSize( type ) );
        type_update( type, TF2_DWARF_DEF, dh );
        break;
    case TYP_SINT:
        dh = DWFundamental( Client, "signed int", DW_FT_SIGNED, CgTypeSize( type ) );
        type_update( type, TF2_DWARF_DEF, dh );
        break;
    case TYP_UINT:
        dh = DWFundamental( Client, "unsigned int", DW_FT_UNSIGNED, CgTypeSize( type ) );
        type_update( type, TF2_DWARF_DEF, dh );
        break;
    case TYP_SLONG:
        dh = DWFundamental( Client, "signed long", DW_FT_SIGNED, CgTypeSize( type ) );
        type_update( type, TF2_DWARF_DEF, dh );
        break;
    case TYP_ULONG:
        dh = DWFundamental( Client, "unsigned long", DW_FT_UNSIGNED, CgTypeSize( type ) );
        type_update( type, TF2_DWARF_DEF, dh );
        break;
    case TYP_SLONG64:
        dh = DWFundamental( Client, "__int64", DW_FT_SIGNED, CgTypeSize( type ) );
        type_update( type, TF2_DWARF_DEF, dh );
        break;
    case TYP_ULONG64:
        dh = DWFundamental( Client, "unsigned __int64", DW_FT_UNSIGNED, CgTypeSize( type ) );
        type_update( type, TF2_DWARF_DEF, dh );
        break;
    case TYP_FLOAT:
        dh = DWFundamental( Client, "float", DW_FT_FLOAT, CgTypeSize( type ) );
        type_update( type, TF2_DWARF_DEF, dh );
        break;
    case TYP_DOUBLE:
        dh = DWFundamental( Client, "double", DW_FT_FLOAT, CgTypeSize( type ) );
        type_update( type, TF2_DWARF_DEF, dh );
        break;
    case TYP_LONG_DOUBLE:
        dh = DWFundamental( Client, "long double", DW_FT_FLOAT, CgTypeSize( type ) );
        type_update( type, TF2_DWARF_DEF, dh );
        break;
    case TYP_ENUM:
        dh = dwarfEnum( type, control );
        break;
    case TYP_POINTER:
    {   uint        flags;
        type_flag   bflag;

        flags = dwarfAddressClassFlags( type );
        if( type->flag & TF1_REFERENCE ) {
            flags |= DW_FLAG_REFERENCE;
        }
        TypeModFlags( type->of, &bflag );
        if( bflag & TF1_BASED ) {
            dh = dwarfBasedPointerType( type, flags );
        } else {
            dh = DWPointer( Client, dwarfType( type->of, DC_DEFAULT ), flags );
        }
        type_update( type, TF2_DWARF_DEF, dh );
    }   break;
    case TYP_TYPEDEF:
        if( SymIsClassDefinition( type->u.t.sym ) ) {
            dh = dwarfType( type->of, control );
            type_update( type, TF2_DWARF_DEF, dh );
        } else if( SymIsEnumDefinition( type->u.t.sym ) ) {
            dh = dwarfType( type->of, control );
//          type_update( type, TF2_DWARF_DEF, dh );
        } else if( SymIsInjectedTypedef( type->u.t.sym ) ) {
            dh = dwarfType( type->of, control );
            type_update( type, TF2_DWARF_DEF, dh );
        } else {
            if( ScopeType( type->u.t.scope, SCOPE_TEMPLATE_PARM ) ) {
                dh = dwarfTypedef( type, DC_DEFINE | DC_FAKE );
            } else {
                dh = dwarfTypedef( type, control );
            }
        }
        break;
    case TYP_CLASS:
        dh = dwarfClass( type, DC_DEFAULT );
        break;
    case TYP_FUNCTION:
        dh = dwarfTypeFunction( type );
        break;
    case TYP_ARRAY:
        dh = dwarfTypeArray( type );
        break;
    case TYP_VOID:
//      if( control & DC_RETURN ) {
//          dh = 0;
//      } else {
            dh = DWFundamental( Client, "void", DW_FT_UNSIGNED, 0 );
            type_update( type, TF2_DWARF_DEF, dh );
//      }
        break;
    case TYP_MODIFIER:
        dh = dwarfTypeModifier( type );
        break;
    case TYP_MEMBER_POINTER:
        dh = dwarfTypeMemberPointer( type );
        break;
    case TYP_BITFIELD:
        dh = dwarfType( type->of, control );
        type_update( type, TF2_DWARF_DEF, dh );
        break;
    case TYP_GENERIC:
        dh = DWFundamental( Client, "?", DW_FT_UNSIGNED, 1 );
        type_update( type, TF2_DWARF_DEF, dh );
        break;
    default:
        DbgStmt( DumpFullType( type ) );
        DbgStmt( CFatal( "dwarf: illegal type" ) );
        break;
    }
#ifndef NDEBUG
    if( dh == 0 && !(control & DC_RETURN) ) {
        DumpFullType( type );
        CFatal( "dwarf: unable to define type" );
    }
#endif
    return( dh );
}

static void dwarfTemplateParm( TYPE p )
/*************************************/
{ // force out types for args

    while( p != NULL ) {
        if( p->id == TYP_TYPEDEF ) {
            SCOPE sc;

            sc = p->u.t.scope;
            if( sc->id == SCOPE_TEMPLATE_PARM ) {
                dwarfType( p, DC_DEFINE | DC_FAKE );
            }
        }
        p = p->of;
    }
}

static void dwarf_define_parm( SYMBOL sym )
/*****************************************/
{
    dw_handle   dh;
    char        *name;

    if( (sym->flag2 & SF2_TOKEN_LOCN)==0 ) return;
    if( IsCppNameInterestingDebug( sym ) ) {
        name = CppNameDebug( sym );
    } else {
        name = NULL;
    }
    dwarfTemplateParm( sym->sym_type );
    dh = DWFormalParameter( Client,
                            dwarfType( sym->sym_type, DC_DEFAULT ),
                            NULL,
                            NULL,
                            name,
                            DW_DEFAULT_NONE ); // fixme: default args
    sym_reset( sym );
    sym_update( sym, SF2_DW_HANDLE_DEF, dh );
}

static boolean dwarfValidateSymbol( SYMBOL sym )
/**********************************************/
{
    unsigned    junk;

    if( !SymIsAnonymous( sym ) ) {
        if( IsCppSpecialName( sym->name->name, &junk ) ) {
            return( FALSE );
        }
    }
    return( TRUE );
}

static void dwarf_block_open( SYMBOL sym )
/****************************************/
{
    dw_handle   dh;

    sym_reset( sym );
    if( (sym->flag2 & SF2_TOKEN_LOCN)==0 ) return;
    if( sym->flag2 & SF2_DW_HANDLE ) return;
    if( !dwarfValidateSymbol( sym ) ) return;

    if( SymIsClassDefinition( sym ) ) {
        dh = dwarfSymbol( sym, DC_DEFINE );
    } else if( SymIsEnumDefinition( sym ) ) {
        dh = dwarfSymbol( sym, DC_DEFINE );
    } else if( SymIsEnumeration( sym ) ) {
        // do nothing, handled by SymIsEnumDefinition
        dh = 0;
    } else if( SymIsFunction( sym ) ) {
        dh = dwarfSymbol( sym, DC_DEFINE );
    } else if( SymIsTypedef( sym ) ) {
        dh = dwarfSymbol( sym, DC_DEFINE );
    } else if( SymIsData( sym ) ) {
        char    *name;
        dwarfTemplateParm( sym->sym_type );
        dh = dwarfType( sym->sym_type, DC_DEFAULT ); //CppName no-reentrant
        if( IsCppNameInterestingDebug( sym ) ) {
            name = CppNameDebug( sym );
        } else {
            name = "<unnamed>";
        }
        if( !InDebug ) {
            dwarfLocation( sym );
        }
        dh = DWVariable( Client,
                         dh,
                         dummyLoc,
                         0,
                         dummyLoc,
                         name,
                         0,
                         0 );
        sym_update( sym, SF2_DW_HANDLE_DEF, dh );
    }
}

static void dwarfProcessFunction( CGFILE *file_ctl )
/**************************************************/
{
    register CGVALUE ins_value; // - value on intermediate-code instruction
    CGINTER *ins;               // - next intermediate-code instruction
    dw_linenum line;
    dw_column column;
    dw_handle dh;

    CgioOpenInput( file_ctl );
    line = 0;
    column = 0;
    for( ; ; ) {
        ins = CgioReadICMask( file_ctl, ICOPM_DWARF );
        if( ins->opcode == IC_EOF ) break;
        ins_value = ins->value;
        // The following comment is a trigger for the ICMASK program to start
        // scanning for case IC_* patterns.
        // ICMASK BEGIN DWARF (do not remove)
        switch( ins->opcode ) {

          case IC_EOF :                     // TERMINATING IC FOR ICMASK PROGRAM
            DbgNever();
            break;

//
//          SYMBOL REFERENCES
//
          case IC_LEAF_NAME_FRONT :         // LEAF: FRONT-END SYMBOL
          case IC_VIRT_FUNC :               // MARK INDIRECT AS VIRTUAL CALL
            if( dwarfValidateSymbol( ins_value.pvalue ) ) {
                dh = dwarfSymbol( ins_value.pvalue, DC_DEFAULT );
                if( dh != 0 ) {
                    DWReference( Client, line, column, dh );
                }
            }
            break;

//
//          PROCEDURE DECLARATIONS
//
          case IC_FUNCTION_ARGS :           // DEFINE FUNCTION ARG.S
            ScopeWalkOrderedSymbols( ins_value.pvalue, &dwarf_define_parm );
            break;

          case IC_BLOCK_OPEN :              // OPEN BLOCK SCOPE (LIVE CODE)
            if( ins_value.pvalue != NULL ) {
                DWBeginLexicalBlock( Client, NULL, NULL );
//              ScopeWalkSymbols( ins_value.pvalue, &dwarf_block_open );
                ScopeWalkOrderedSymbols( ins_value.pvalue, &dwarf_block_open );
            }
            break;

          case IC_BLOCK_DEAD :              // OPEN BLOCK SCOPE (DEAD CODE)
            if( ins_value.pvalue != NULL ) {
                DWBeginLexicalBlock( Client, NULL, NULL );
//              ScopeWalkSymbols( ins_value.pvalue, &dwarf_block_open );
                ScopeWalkOrderedSymbols( ins_value.pvalue, &dwarf_block_open );
            }
            break;

          case IC_BLOCK_END :               // CLOSE BLOCK SCOPE
            DWEndLexicalBlock( Client );
            break;

//
//          DEBUGGING -- for program
//
          case IC_DBG_LINE :                // SET LINE NUMBER
            line = ins_value.uvalue;
            break;

//
//          EXCEPTION HANDLING
//
          case IC_TRY :                     // START A TRY BLOCK
            dh = dwarfSymbol( ins_value.pvalue, DC_DEFAULT );
            if( dh != 0 ) {
                DWReference( Client, line, column, dh );
            }
            break;

          case IC_CATCH_VAR :               // SET TRY_VAR FOR CATCH
            dh = dwarfSymbol( ins_value.pvalue, DC_DEFAULT );
            if( dh != 0 ) {
                DWReference( Client, line, column, dh );
            }
            break;

          case IC_CATCH :                   // SET TYPE OF A CATCH
            if( ins_value.pvalue != 0 ) {
                dh = dwarfType( ins_value.pvalue, DC_DEFAULT );
                if( dh != 0 ) {
                    DWReference( Client, line, column, dh );
                }
            }
            break;

          case IC_EXCEPT_SPEC :             // FUNCTION EXCEPTION SPEC.
            if( ins_value.pvalue != NULL ) { // not throw()
                dh = dwarfType( ins_value.pvalue, DC_DEFAULT );
                if( dh != 0 ) {
                    DWReference( Client, line, column, dh );
                }
            }
            break;

          case IC_THROW_RO_BLK :            // SET THROW R/O BLOCK
            dh = dwarfType( ins_value.pvalue, DC_DEFAULT );
            if( dh != 0 ) {
                DWReference( Client, line, column, dh );
            }
            break;

          default:
            DbgNever();
        }
        // ICMASK END (do not remove)
    }
    CgioCloseInputFile( file_ctl );
}

static dw_handle dwarfFunctionDefine( SYMBOL sym, CGFILE *file_ctl )
/******************************************************************/
{
    TYPE        base;
    type_flag   tf;
    dw_handle   class_dh;
    dw_handle   return_dh;
    dw_handle   dh;
    uint        call_type;
    uint        flags;
    int         ctor_or_dtor;

    ctor_or_dtor = 0;
    call_type = 0;
    base = TypeModFlags( sym->sym_type, &tf );
    if( tf & TF1_NEAR ) {
        call_type |= DW_SB_NEAR_CALL;
    }
    if( tf & TF1_FAR ) {
        call_type |= DW_SB_FAR_CALL;
    }
    if( tf & TF1_FAR16 ) {
        call_type |= DW_SB_FAR16_CALL;
    }
    flags = dwarfAddressClassFlags( sym->sym_type );
    flags |= DW_FLAG_PROTOTYPED;
    if( SymIsClassMember( sym ) ) {
        class_dh = dwarfType( SymClass( sym ), DC_DEFAULT );
        if( SymIsStaticMember( sym ) ) {
            flags |= DW_SUB_STATIC;
        }
        if( SymIsVirtual( sym ) ) {
            flags |= DW_FLAG_VIRTUAL;
        }
        if( base->flag & TF1_INLINE ) {
            flags |= DW_FLAG_DECLARED_INLINE;
        }
        if( sym->flag & SF_PRIVATE ) {
            flags |= DW_FLAG_PRIVATE;
        } else if( sym->flag & SF_PROTECTED ) {
            flags |= DW_FLAG_PROTECTED;
        } else {
            flags |= DW_FLAG_PUBLIC;
        }
        if( SymIsCtor( sym ) || SymIsDtor( sym ) ) {
            ctor_or_dtor = 1;
        }
    } else {
        if( SC_STATIC == SymDefaultBase( sym )->id ) {
            flags |= DW_SUB_STATIC;
        }
        class_dh = 0;
    }
    flags = dwarfMarkInternalName( sym, flags );
    if( file_ctl != NULL ) {
        dwarfTokenLocation( &file_ctl->defined );
    } else {
        flags |= DW_FLAG_DECLARATION;
    }
    if( ctor_or_dtor ) {
        return_dh = 0;
    } else {
        return_dh = dwarfType( base->of, DC_RETURN );
    }
    DWHandleSet( Client, sym->locn->u.dwh );
    dh = DWBeginSubroutine( Client,
                   call_type,
                   return_dh,
                   dummyLoc,
                   dummyLoc,
                   dummyLoc,
                   class_dh,
                   dummyLoc,
                   GetMangledName( sym ),
                   0,
                   flags );
    if( file_ctl != NULL ) {
        dwarfProcessFunction( file_ctl );
    }
    DWEndSubroutine( Client );
    return( dh );
}

static dw_handle dwarfFunction( SYMBOL sym, DC_CONTROL control )
/**************************************************************/
{
    dw_handle   dh;
    CGFILE      *file_ctl;

    sym_reset( sym );
    if( sym->flag2 & SF2_DW_HANDLE ) {
        dh = sym->locn->u.dwh;
    } else {
        dh = DWHandle( Client, DW_ST_NONE );
        sym_update( sym, SF2_DW_HANDLE_FWD, dh );
    }
    if( (control & DC_DEFINE) && (sym->flag2 & SF2_DW_HANDLE_FWD) ) {
        file_ctl = CgioLocateFile( sym );
        if( (file_ctl != NULL) && (file_ctl->defined.src_file == NULL) ) {
            file_ctl = NULL;
        }
        dh = dwarfFunctionDefine( sym, file_ctl );
        sym_update( sym, SF2_DW_HANDLE_DEF, dh );
    }
    return( dh );
}

static dw_handle dwarfData( SYMBOL sym )
/**************************************/
{
    dw_handle dh;
    dw_handle class_dh;
    uint      flags;

    #ifndef NDEBUG
        if( sym->flag2 & SF2_DW_HANDLE_DEF ) {
            DumpSymbol( sym );
            CFatal( "dwarf: data symbol already defined" );
        }
    #endif
    flags = 0;
    class_dh = 0;
    if( SymIsClassMember( sym ) ) {
        if( SymIsStaticDataMember( sym ) ) {
            class_dh = dwarfType( SymClass( sym ), DC_DEFAULT );
        }
        if( sym->flag & SF_PRIVATE ) {
            flags |= DW_FLAG_PRIVATE;
        } else if( sym->flag & SF_PROTECTED ) {
            flags |= DW_FLAG_PROTECTED;
        } else {
            flags |= DW_FLAG_PUBLIC;
        }
    }
    if( !SymIsStaticData( sym ) && !SymIsAutomatic( sym ) ) {
        flags |= DW_FLAG_GLOBAL;
    }
    dh = DWVariable( Client,
                dwarfType( sym->sym_type, DC_DEFAULT ),
                dummyLoc,
                class_dh,
                dummyLoc,
                CppNameDebug( sym ),
                0,
                flags );
    sym_update( sym, SF2_DW_HANDLE_DEF, dh );
    return( dh );
}

static dw_handle dwarfDebugStatic( SYMBOL sym )
/**************************************/
{
    dw_handle dh;
    dw_handle class_dh;
    dw_loc_handle   dl;
    dw_loc_handle   dl_seg;
    uint      flags;
    char     *name;

    sym_reset( sym );
#ifndef NDEBUG
    if( sym->flag2 & SF2_DW_HANDLE_DEF ) {
        DumpSymbol( sym );
        CFatal( "dwarf: data symbol already defined" );
    }
#endif
    flags = 0;
    class_dh = 0;
    if( SymIsClassMember( sym ) ) {
        if( SymIsStaticDataMember( sym ) ) {
            class_dh = dwarfType( SymClass( sym ), DC_DEFAULT );
        }
        if( sym->flag & SF_PRIVATE ) {
            flags |= DW_FLAG_PRIVATE;
        } else if( sym->flag & SF_PROTECTED ) {
            flags |= DW_FLAG_PROTECTED;
        } else {
            flags |= DW_FLAG_PUBLIC;
        }
    }
    if( !SymIsStaticData( sym ) ) {
        flags |= DW_FLAG_GLOBAL;
    }

    dl_seg = NULL;
    if( InDebug ) {
        dl = dwarfDebugStaticLoc( sym );
#if _INTEL_CPU
        if( !IsFlat() ) {  /* should check Client */
            dl_seg =  dwarfDebugStaticSeg( sym );
        }
#endif
    } else {    /* fake up loc for browser */
        dw_loc_id       locid;

        locid = DWLocInit( Client );
        dl = DWLocFini( Client, locid );
    }
    dh = dwarfType( sym->sym_type, DC_DEFAULT );
    name = CppNameDebug( sym ); //non rent do after dwarftype
    dh = DWVariable( Client,
                dh,
                dl,
                class_dh,
                dl_seg,
                name,
                0,
                flags );
    DWLocTrash( Client, dl );
    if( dl_seg != NULL ) {
        DWLocTrash( Client, dl_seg );
    }
    if( flags & DW_FLAG_GLOBAL ){
        name = CppClassPathDebug( sym ); //non rent do after dwarftype
        DWPubname( Client, dh, name );
    }

    if( !InDebug ) {
        sym_update( sym, SF2_DW_HANDLE_DEF, dh );
    }
    return( dh );
}
static dw_handle dwarfSymbol( SYMBOL sym, DC_CONTROL control )
/************************************************************/
{
    dw_handle       dh = 0;

    if( !InDebug ) {
        if( (sym->flag2 & SF2_TOKEN_LOCN) == 0 ) return( 0 );
    };
    sym_reset( sym );
    if( sym->flag2 & SF2_DW_HANDLE_DEF ) return( sym->locn->u.dwh );

    if( !InDebug ) {
        dwarfLocation( sym );
    }
    if( SymIsClassDefinition( sym ) ) {
        dh = dwarfClass( StructType( sym->sym_type ), control );
    } else if( SymIsEnumDefinition( sym ) ) {
        dh = dwarfEnum( EnumType( sym->sym_type ), control );
    } else if( SymIsInjectedTypedef( sym ) ) {
        dh = dwarfType( sym->sym_type, DC_DEFAULT );
    } else if( SymIsTypedef( sym ) ) {
        dh = dwarfTypedef( sym->sym_type, control );
    } else if( !IsCppNameInterestingDebug( sym ) ) {
        // don't want any boring names
        dh = 0;
    } else if( SymIsFunction( sym ) ) {
        dh = dwarfFunction( sym, control );
    } else if( SymIsData( sym ) ) {
        dh = dwarfData( sym );
#ifndef NDEBUG
    } else {
        DumpSymbol( sym );
        CFatal( "dwarf: illegal symbol" );
#endif
    }
    return( dh );
}

static void doDwarfForwardFollowupClass( TYPE type, void *ignore )
/****************************************************************/
{
    boolean *keep_going = ignore;
    if( (type->dbgflag & TF2_DWARF) == TF2_DWARF_FWD ) {
#ifndef NDEBUG
        if( type->flag & TF1_UNBOUND ) {
            DumpFullType( type );
            CFatal( "dwarf: unbound template type in browser info" );
        }
#endif
        dwarfClass( type, DC_DEFINE );
        *keep_going = TRUE;
    }
}

static void doDwarfForwardFollowupTypedef( TYPE type, void *ignore )
/******************************************************************/
{
    boolean *keep_going = ignore;
    if( (type->dbgflag & TF2_DWARF) == TF2_DWARF_FWD ) {
        dwarfTypedef( type, DC_DEFINE );
        *keep_going = TRUE;
    }
}

static void doDwarfForwardFollowupEnum( TYPE type, void *ignore )
/***************************************************************/
{
    boolean *keep_going = ignore;
    if( (type->dbgflag & TF2_DWARF) == TF2_DWARF_FWD ) {
        dwarfEnum( type, DC_DEFINE );
        *keep_going = TRUE;
    }
}

static boolean dwarfForwardFollowup( void )
/*****************************************/
{
    boolean did_something = FALSE;

    for(;;) {
        boolean keep_going = FALSE;
        TypeTraverse( TYP_CLASS, &doDwarfForwardFollowupClass, &keep_going );
        TypeTraverse( TYP_TYPEDEF, &doDwarfForwardFollowupTypedef, &keep_going );
        TypeTraverse( TYP_ENUM, &doDwarfForwardFollowupEnum, &keep_going );
        if( ! keep_going ) break;
        did_something = TRUE;
    }
    return( did_something );
}

static void dwarfEmitSymbolScope( SCOPE scope )
/*********************************************/
// dump all the symbols defined within the specified scope
{
    SYMBOL stop;
    SYMBOL curr;

    stop = ScopeOrderedStart( scope );
    curr = ScopeOrderedNext( stop, NULL );
    while( curr != NULL ) {
        // skip over enum's because they are handled by
        // the typedef that precedes the enum symbols
        if( !SymIsEnumeration( curr ) ) {
            (void)dwarfSymbol( curr, DC_DEFINE );
        }
        curr = ScopeOrderedNext( stop, curr );
    }
    dwarfForwardFollowup();
}

static void doDwarfEmitFundamentalType( TYPE type, void *data )
/*************************************************************/
{
    dw_handle *pdh;

    pdh = data;
    if( *pdh == 0 ) {
        *pdh = dwarfType( type, DC_DEFAULT );
    } else {
        type_reset( type );
        type_update( type, TF2_DWARF_DEF, *pdh );
    }
}

static void dwarfEmitFundamentalType( void )
/******************************************/
{
    type_id     id;
    dw_handle   data;

    for( id = TYP_FIRST_VALID ; id < TYP_LONG_DOUBLE ; ++id ) {
        data = 0;
        TypeTraverse( id, &doDwarfEmitFundamentalType, (void *)&data );
    }
}

extern void DwarfBrowseEmit( void )
/*********************************/
{
    if( !CompFlags.emit_browser_info ) return;
    initDwarf( FALSE, DSI_ONLY_SYMS );
    Client = DwarfInit();
    dummyLoc = DWLocFini( Client, DWLocInit( Client ) );
    dwarfEmitFundamentalType();
    dwarfEmitSymbolScope( GetFileScope() );
    DWLocTrash( Client, dummyLoc );
    DwarfFini( Client );
}


extern void DwarfDebugInit( void )
/********************************/
{
    assert( sizeof( vf_FieldType->dbg.handle ) == sizeof( dbg_type ) );
    if( GenSwitches & DBG_TYPES ) {
        initDwarf( TRUE, DSI_NULL );
        Client = DFClient();
        dummyLoc = DWLocFini( Client, DWLocInit( Client ) );
        // quick wedge to test
    }
}

extern dbg_type DwarfDebugType( TYPE type )
{
    return( dwarfType( type, DC_DEFAULT ) );
}

extern dbg_type DwarfDebugSym( SYMBOL sym )
{
    dbg_type dh;

    if( SymIsTypedef( sym ) ) {
        if( SymIsClassDefinition( sym ) ) {
            dh = dwarfClass( StructType( sym->sym_type ), DC_DEFINE );
        } else if( SymIsEnumDefinition( sym ) ) {
            dh = dwarfEnum( EnumType( sym->sym_type ), DC_DEFINE );
        } else if( SymIsInjectedTypedef( sym ) ) {
            dh = dwarfType( sym->sym_type, DC_DEFAULT );
        } else if( SymIsTypedef( sym ) ) {
            dh = dwarfTypedef( sym->sym_type, DC_DEFINE );
        } else {
            dh = dwarfType( sym->sym_type, DC_DEFINE );
        }
    } else {
        dh = dwarfType(  sym->sym_type, DC_DEFAULT );
    }
    return( dh );
}

static bool dwarfUsedTypeSymbol( SCOPE scope );

extern void DwarfDebugFini( void )
/********************************/
{
    if( GenSwitches & DBG_TYPES ) {
        if( !CompFlags.all_debug_type_names ) { // generate what's used
            dwarfUsedTypeSymbol(GetFileScope());
            dwarfForwardFollowup();
        }
        if( CompFlags.pch_debug_info_write ) {
            PCHPerformReloc( PCHRELOC_TYPE );
        }
        DWLocTrash( Client, dummyLoc );
    }
}

static int dwarfTempScope(  SCOPE scope ) {
/*****************************************/
    SYMBOL stop;
    SYMBOL curr;

    stop = ScopeOrderedStart( scope );
    curr = ScopeOrderedNext( stop, NULL );
    while( curr != NULL ) {
        if( SymIsTypedef( curr ) ) {
            dwarfSymbol(  curr, DC_DEFINE );
        }
        curr = ScopeOrderedNext( stop, curr );
    }
    return( TRUE );
}

static bool ADirtyNameSpace( SYMBOL curr )
{
    bool ret;
    ret = FALSE;
    if( SymIsNameSpace( curr ) ) {
        ret = curr->u.ns->scope->u.s.dirty;
    }
    return( ret );
}
static void dwarfBegNameSpace( SYMBOL curr )
/*********************************************/
{
    const char  *name;
    TYPE        type;
    dw_handle   dh;

    type = curr->sym_type;
    type_reset( type );
    name = ScopeNameSpaceFormatName( curr->u.ns->scope );
    dh = DWBeginNameSpace( Client,name );
    type_update( type, TF2_DWARF_DEF, dh );
}

static void dwarfDebugSymbol( SCOPE scope );

static void dwarfNameSpace( SYMBOL curr )
/***************************************/
{
    SCOPE scope = curr->u.ns->scope;
    if( curr->u.ns->u.s.unnamed ){
        dwarfDebugSymbol( scope );
    } else {
        dwarfBegNameSpace( curr );
        dwarfDebugSymbol( scope );
        DWEndNameSpace( Client );
    }
}

static void dwarfDebugSymbol( SCOPE scope )
/*******************************/
{
    SYMBOL stop;
    SYMBOL curr;

    stop = ScopeOrderedStart( scope );
    curr = ScopeOrderedNext( stop, NULL );
    while( curr != NULL ) {
        if(  SymIsClassTemplateModel( curr ) ) {
            WalkTemplateInst( curr, &dwarfTempScope );
        } else if( ! SymIsFunctionTemplateModel( curr ) &&
            ( curr->flag & (SF_INITIALIZED | SF_REFERENCED) ) &&
            SymIsData( curr ) &&
            !SymIsEnumeration( curr) &&
            !SymIsTemporary( curr ) &&
            IsCppNameInterestingDebug( curr ) ) {
            dwarfDebugStatic( curr );
            DbgAddrTaken( curr );
        } else {
            if( SymIsTypedef( curr ) ) {
                dwarfSymbol(  curr, DC_DEFINE );
            } else if( ADirtyNameSpace( curr ) ) {
                dwarfNameSpace(  curr );
            }
        }
        curr = ScopeOrderedNext( stop, curr );
    }
}


static int dwarfUsedTempScope( SCOPE scope ) {
/*****************************************/
    SYMBOL stop;
    SYMBOL curr;
    TYPE   type;

    stop = ScopeOrderedStart( scope );
    curr = ScopeOrderedNext( stop, NULL );
    while( curr != NULL ) {
        if( SymIsTypedef( curr ) ) {
            type = curr->sym_type;
            if( ! _typeHasForwardDwarfHandle( type ) ) {
                type = type->of;
            }
            if( _typeHasForwardDwarfHandle( type ) ) {
                if( ! _typeHasPCHDwarfHandle( type ) ) {
                    dwarfSymbol( curr, DC_DEFINE );
                    DbgAssert( ! _typeHasForwardDwarfHandle( type ) );
                    return( FALSE );
                }
            }
        }
        curr = ScopeOrderedNext( stop, curr );
    }
    return( TRUE );
}


static bool dwarfUsedNameSpace( SYMBOL curr )
/***************************************/
{
    bool   has_changed;

    if( !curr->u.ns->u.s.unnamed ) {
        dwarfBegNameSpace( curr );
    }
    has_changed = dwarfUsedTypeSymbol( curr->u.ns->scope );
    if( !curr->u.ns->u.s.unnamed ) {
        DWEndNameSpace( Client );
    }
    return( has_changed );
}

static bool dwarfUsedTypeSymbol( SCOPE scope )
/********************************************/
{
    SYMBOL stop;
    SYMBOL curr;
    bool   change;
    bool   has_changed;

    has_changed = FALSE;
    for(;;) {
        change = FALSE;
        stop = ScopeOrderedStart( scope );
        curr = ScopeOrderedNext( stop, NULL );
        while( curr != NULL ) {
            if(  SymIsClassTemplateModel( curr ) ) {
                if( !WalkTemplateInst( curr, &dwarfUsedTempScope ) ) {
                    change = TRUE;
                }
#if 0
            } else if( SymIsTypedef( curr ) ) {
                TYPE type = curr->sym_type;
                // KLUDGE around Dwarf problem
                if( 1||!_typeHasDefinedDwarfHandle( type ) ) {  // either undefined or forward
                    if( ! _typeHasForwardDwarfHandle( type ) ) { // if undefined check one in
                        type = type->of;
                    }
                    if( _typeHasForwardDwarfHandle( type ) ) {  // define forward type
                        if( ! _typeHasPCHDwarfHandle( type ) ) {
                            dwarfType( type, DC_DEFINE );
                            dwarfSymbol( curr, DC_DEFINE );
//                            DbgAssert( ! _typeHasForwardDwarfHandle( type ) );
                            change = TRUE;
                        }
                    }
                }
                type = 0;
#endif
            } else if( ADirtyNameSpace( curr ) ) {
                if( dwarfUsedNameSpace(  curr ) ) {
                    change = TRUE;
                }
            }
            curr = ScopeOrderedNext( stop, curr );
        }
        if( !change )break;
        has_changed = TRUE;
    }
    return( has_changed );
}

static void dwarfPreUsedSymbol( SCOPE scope );

static void dwarfPreUsedNameSpace( SYMBOL curr )
/***************************************/
{

    if( !curr->u.ns->u.s.unnamed ){
        dwarfBegNameSpace( curr );
    }
    dwarfPreUsedSymbol( curr->u.ns->scope );
    if( !curr->u.ns->u.s.unnamed ){
        DWEndNameSpace( Client );
    }
}

static void dwarfPreUsedSymbol( SCOPE scope )
/************************************/
{
    TYPE type;
    SYMBOL stop;
    SYMBOL curr;

    stop = ScopeOrderedStart( scope );
    curr = ScopeOrderedNext( stop, NULL );
    while( curr != NULL ) {
        if( ! SymIsFunctionTemplateModel( curr ) &&
            ( curr->flag & (SF_INITIALIZED ) ) &&
            SymIsData( curr ) &&
            !SymIsEnumeration( curr) &&
            !SymIsTemporary( curr ) &&
            IsCppNameInterestingDebug( curr ) ) {
            dwarfDebugStatic( curr );
            DbgAddrTaken( curr );
        } else {
            if( SymIsTypedef( curr ) ) {
                type = curr->sym_type;
                if( ! _typeHasPCHDwarfHandle( type ) ) {
                    if( _typeHasForwardDwarfHandle( type ) ) {
                        dwarfSymbol( curr, DC_DEFINE );
                        DbgAssert( ! _typeHasForwardDwarfHandle( type ) );
                    }
                }
            } else if( ADirtyNameSpace( curr ) ) {
                dwarfPreUsedNameSpace(  curr );
            }
        }
        curr = ScopeOrderedNext( stop, curr );
    }
}

#if 0   // not sure if needed
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

static void doDwarfDebugNamedType( TYPE type, void *data )
/***********************************************************/
{
    data = data;
    if( !ScopeType( type->u.t.scope, SCOPE_TEMPLATE_PARM ) &&
        !ScopeType( type->u.t.scope, SCOPE_TEMPLATE_DECL ) ) {
        if( CompFlags.all_debug_type_names || typedef_is_of_basic_types( type ) ) {
            dwarfTypedef( type, DC_DEFINE );
        }
    }
}
static void dwarfDebugNamedType()
/**********************************/
{
    int    data;

    data = 0;
    TypeTraverse( TYP_TYPEDEF, &doDwarfDebugNamedType, (void *)&data );
}
#endif
extern void DwarfDebugEmit( void )
/****************************/
{
    if( GenSwitches & DBG_TYPES ) {
        dwarfEmitFundamentalType();
        if( CompFlags.all_debug_type_names ) {      // generate the works
            dwarfDebugSymbol( GetFileScope() );
        } else {
            dwarfPreUsedSymbol( GetFileScope() );   // generate whats used
        }
    }
}

static void dwarfDebugTemplateParm( TYPE p )
/*************************************/
{ // force out types for args

    while( p != NULL ) {
        if( p->id == TYP_TYPEDEF ) {
            SCOPE sc;

            sc = p->u.t.scope;
            if( sc->id == SCOPE_TEMPLATE_PARM ) {
                DBLocalType( (cg_sym_handle)p->u.t.sym, TRUE );
            }
        }
        p = p->of;
    }
}

extern void DwarfDebugGenSymbol( SYMBOL sym, boolean scoped )
/*******************************************************/
{
    if( scoped ) {  /* if scoped need to feed thu codegen */
        dwarfDebugTemplateParm( sym->sym_type );
        DBLocalSym( (cg_sym_handle)dwarfDebugSymAlias( sym ), TRUE );
    } else {    /* can handle static by ourselves */
        dwarfDebugStatic( sym );
    }
}

#if _INTEL_CPU
static dbg_loc symbolicDebugSetSegment( dbg_loc dl, SYMBOL sym )
/**************************************************************/
{
    dl = DBLocSym( dl, (cg_sym_handle)dwarfDebugSymAlias( sym ) );
    dl = DBLocOp( dl, DB_OP_XCHG, 1 );
    dl = DBLocOp( dl, DB_OP_MK_FP, 0 );
    return( dl );
}
#endif

extern void DwarfSymDebugGenSymbol( SYMBOL sym, boolean scoped, boolean by_ref )
/******************************************************************************/
//TODO :it would be nice to get rid of this WV dbg dep.
{
    TYPE        pt;
    dbg_loc     dl;
    dl = DBLocInit();
    dl = DBLocSym( dl, (cg_sym_handle)dwarfDebugSymAlias( sym ) );
    if( by_ref ) {
        pt = MakePointerTo( sym->sym_type );
        dl = DBLocOp( dl, DB_OP_POINTS, CgTypeOutput( pt ) );
#if _INTEL_CPU
        if( !IsFlat() ) {  /* should check Client */
            if( !IsBigData() ) {
                dl = symbolicDebugSetSegment( dl, DefaultDataSymbol );
                DefaultDataSymbol->flag |= SF_REFERENCED;
            }
        }
#endif
    }
    DBGenSym( (cg_sym_handle)sym, dl, scoped );
    DBLocFini( dl );
}

extern void DwarfDebugNameSpaceEnclosed( SYMBOL sym ) {
/*************************************************/

    SCOPE scope;
    NAME_SPACE *ns;

    scope = sym->name->containing;
    ns = scope->owner.ns;
    DBObject( DwarfDebugType( ns->sym->sym_type ), NULL, TY_DEFAULT );
}

extern void DwarfDebugMemberFunc( SYMBOL func, SYMBOL this_sym ) {
/*******************************************************************/
    this_sym = this_sym;
    DBObject( DwarfDebugType( SymClass( func ) ), NULL, TY_DEFAULT );
}

extern uint_32 DwarfDebugOffset( uint_32 handle ) {
/************************************************/
    return( DWDebugRefOffset( Client, handle ) );
}
