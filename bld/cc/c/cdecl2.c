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
* Description:  C declarator processing.
*
****************************************************************************/


#include "cvars.h"
#include "cgswitch.h"
#include "i64.h"


static  TYPEPTR     DeclPart2( TYPEPTR typ, type_modifiers mod );
static  TYPEPTR     DeclPart3( TYPEPTR typ, type_modifiers mod );
static  void        AbsDecl( SYMPTR sym, type_modifiers mod, TYPEPTR typ );
static  void        FreeParmList( void );
static   void        GetFuncParmList( void );

static segment_id   ThreadSeg;


void Chk_Struct_Union_Enum( TYPEPTR typ )
{
    SKIP_DUMMY_TYPEDEFS( typ );
    switch( typ->decl_type ) {
    case TYPE_STRUCT:
    case TYPE_UNION:
        if( typ->u.tag->name[0] == '\0' ) {
            InvDecl();
        }
        break;
    case TYPE_ENUM:
        break;
    default:
        InvDecl();
    }
}


static void FlushBadCode( void )
{
    int         count;

    CErr1( ERR_STMT_MUST_BE_INSIDE_FUNCTION );
    count = 0;
    for( ;; ) {
        NextToken();
        if( CurToken == T_EOF )
            return;
        if( CurToken == T_LEFT_BRACE ) {
            ++count;
        }
        if( CurToken == T_RIGHT_BRACE ) {
            if( count == 0 )
                break;
            --count;
        }
    }
    NextToken();
}


static stg_classes SCSpecifier( void )
{
    stg_classes     stg_class;

    stg_class = SC_NONE;        /* assume no storage class specified */
    if( TokenClass[CurToken] == TC_STG_CLASS ) {
        switch( CurToken ) {
        case T_EXTERN:  stg_class = SC_EXTERN;  break;
        case T_STATIC:  stg_class = SC_STATIC;  break;
        case T_TYPEDEF: stg_class = SC_TYPEDEF; break;
        case T_AUTO:    stg_class = SC_AUTO;    break;
        case T_REGISTER:stg_class = SC_REGISTER;break;
        default:
            break;
        }
        NextToken();
    }
    return( stg_class );
}


void InvDecl( void )
{
    CErr1( ERR_INVALID_DECLARATOR );
}

static void CmpFuncDecls( SYMPTR new_sym, SYMPTR old_sym )
{
    TYPEPTR     type_new, type_old;

    if( (new_sym->mods & ~MASK_FUNC) != (old_sym->mods & ~MASK_FUNC) ) {
        CErr2p( ERR_MODIFIERS_DISAGREE, new_sym->name );
    }

    /* check for conflicting information */
    /* skip over TYPEDEF's  */
    type_new = new_sym->sym_type;
    SKIP_TYPEDEFS( type_new );
    type_old = old_sym->sym_type;
    SKIP_TYPEDEFS( type_old );

    // diagnostics for function, target=old and source=new
    SetDiagType2( type_old->object, type_new->object );
    if( !IdenticalType( type_new->object, type_old->object ) ) {
        TYPEPTR     ret_new, ret_old;

        /* save return types */
        ret_new = type_new->object;
        ret_old = type_old->object;
        // skip over typedef's
        SKIP_TYPEDEFS( ret_new );
        SKIP_TYPEDEFS( ret_old );
        /* don't reorder this expression */
        //return value used in forward
        if( old_sym->attribs.stg_class != SC_FORWARD ) {
            CErr2p( ERR_INCONSISTENT_TYPE, new_sym->name );
        } else if( ret_new->decl_type != TYPE_VOID || (old_sym->flags & SYM_TYPE_GIVEN) ) {
            CErr2p( ERR_INCONSISTENT_TYPE, new_sym->name );
        }
    }
    SetDiagPop();

    /* check types of parms, including promotion */
    ChkCompatibleFunction( type_old, type_new, TRUE );
}


static SYM_HANDLE FuncDecl( SYMPTR sym, stg_classes stg_class, decl_state *state )
{
    SYM_HANDLE          sym_handle;
    SYM_HANDLE          old_sym_handle;
    SYM_ENTRY           old_sym;
    SYM_ENTRY           sym_typedef;
    TYPEPTR             old_typ;
    SYM_NAMEPTR         sym_name;
    char                *name;
    size_t              sym_len;
    ENUMPTR             ep;

    PrevProtoType = NULL;
    // Warn if assuming 'int' return type - should be an error in strict C99 mode
    if( *state & DECL_STATE_NOTYPE ) {
        CWarn2p( WARN_NO_RET_TYPE_GIVEN, ERR_NO_RET_TYPE_GIVEN, sym->name );
    }
    sym->attribs.rent = FALSE;   // Assume not override aka re-entrant
    if( CompFlags.rent && (sym->attribs.declspec == DECLSPEC_DLLIMPORT) ) {
        sym->attribs.rent = TRUE;
    }
    if( stg_class == SC_REGISTER ||
        stg_class == SC_AUTO ||
        stg_class == SC_TYPEDEF ) {
            CErr1( ERR_INVALID_STG_CLASS_FOR_FUNC );
            stg_class = SC_NONE;
    }
    old_sym_handle = SymLook( sym->info.hash, sym->name );
    if( old_sym_handle == SYM_NULL ) {
        ep = EnumLookup( sym->info.hash, sym->name );
        if( ep != NULL ) {
            SetDiagEnum( ep );
            CErr2p( ERR_SYM_ALREADY_DEFINED, sym->name );
            SetDiagPop();
        }
        sym_handle = SymAddL0( sym->info.hash, sym );
    } else {
        SymGet( &old_sym, old_sym_handle );
        SetDiagSymbol( &old_sym, old_sym_handle );
        if( (old_sym.flags & SYM_FUNCTION) == 0 ) {
            CErr2p( ERR_SYM_ALREADY_DEFINED_AS_VAR, sym->name );
            // sym_handle = old_sym_handle;
            sym_handle = SymAddL0( sym->info.hash, sym );
        } else {
            CmpFuncDecls( sym, &old_sym );
            PrevProtoType = old_sym.sym_type;
            if( (old_sym.flags & SYM_DEFINED) == 0 ) {
                if( sym->sym_type->u.fn.parms != NULL
                  || ( CurToken != T_COMMA && CurToken != T_SEMI_COLON ) ) {
                    old_typ = old_sym.sym_type;
                    if( old_typ->decl_type == TYPE_TYPEDEF &&
                       old_typ->object->decl_type == TYPE_FUNCTION ) {
                        SymGet( &sym_typedef, old_typ->u.typedefn );
                        sym_name = SymName( &sym_typedef, old_typ->u.typedefn );
                        sym_len = strlen( sym_name ) + 1;
                        name = CMemAlloc( sym_len );
                        memcpy( name, sym_name, sym_len );
                        XferPragInfo( name, sym->name );
                        CMemFree( name );
                    }
                    old_sym.sym_type = sym->sym_type;
                    old_sym.src_loc = sym->src_loc;
                }
            }
            // check lang flags to make sure no one saw an incompatible prototype; if
            // previous prototype specified calling convention and later definition does
            // not, propagate the convention from the prototype
            if( (sym->mods & MASK_LANGUAGES) && !ChkCompatibleLanguage( sym->mods, old_sym.mods ) ) {
                CErr2p( ERR_MODIFIERS_DISAGREE, sym->name );
            }
            if( (sym->mods & FLAG_INLINE) != (old_sym.mods & FLAG_INLINE) ) {
                old_sym.mods |= FLAG_INLINE; //either is inline
            }
            if( sym->attribs.declspec != old_sym.attribs.declspec ) {
                switch( sym->attribs.declspec ) {
                case DECLSPEC_DLLIMPORT:
                case DECLSPEC_THREAD:
                    CErr2p( ERR_MODIFIERS_DISAGREE, sym->name );
                    break;
                case DECLSPEC_DLLEXPORT:
                    /* Allow the following:     void foo( void ); void _Export foo( void );
                     * IBM's compiler allows this, so does our C++ compiler; and it's real useful!
                     */
                    if( old_sym.attribs.declspec == DECLSPEC_DLLIMPORT || old_sym.attribs.declspec == DECLSPEC_NONE ) {
                        old_sym.attribs.declspec = DECLSPEC_DLLEXPORT;
                    } else {
                        CErr2p( ERR_MODIFIERS_DISAGREE, sym->name );
                    }
                    break;
                }
            }
            old_sym.attribs.naked |= sym->attribs.naked;
            if( stg_class == SC_STATIC && old_sym.attribs.stg_class == SC_EXTERN ) {
                /* can't redeclare extern function as static */
                /* NB: We may want to handle SC_FORWARD functions too! */
                CWarn2p( WARN_FUNCTION_STG_CLASS_REDECLARED, ERR_FUNCTION_STG_CLASS_REDECLARED, sym->name );
            }
            CMemFree( sym->name );
            if( stg_class == SC_NONE && old_sym.attribs.stg_class != SC_FORWARD ) {
                stg_class = old_sym.attribs.stg_class;
            }
            if( old_sym.sym_type->decl_type == TYPE_FUNCTION ) {
                old_sym.sym_type = FuncNode( old_sym.sym_type->object,
                    old_sym.mods, old_sym.sym_type->u.fn.parms );
            }
            memcpy( sym, &old_sym, sizeof( SYM_ENTRY ) );
            sym_handle = old_sym_handle;
        }
        SetDiagPop();
    }
    sym->flags |= SYM_FUNCTION;
    if( (sym->flags & SYM_DEFINED) == 0 ) {
        if( sym->mods & FLAG_INLINE ) {
            sym->flags |= SYM_IGNORE_UNREFERENCE;
            stg_class = SC_STATIC;
        } else if( stg_class == SC_NONE ) {
            stg_class = SC_EXTERN;  /* SC_FORWARD; */
        }
        sym->attribs.stg_class = stg_class;
    }
    return( sym_handle );
}

static SYM_HANDLE VarDecl( SYMPTR sym, stg_classes stg_class, decl_state *state )
{
    bool                old_def;
    TYPEPTR             typ;
    SYM_HANDLE          sym_handle;
    SYM_HANDLE          old_sym_handle;
    SYM_ENTRY           old_sym;
    SYM_ENTRY           sym2;
    type_modifiers      old_attrs;
    type_modifiers      new_attrs;

    // Warn if neither type nor storage class were given; this should probably be
    // an error in strict C89 (and naturally C99) mode
    if( (stg_class == SC_NONE) && (*state & DECL_STATE_NOTYPE) && (*state & DECL_STATE_NOSTWRN) == 0 ) {
        CWarn1( WARN_NO_STG_OR_TYPE, ERR_NO_STG_OR_TYPE );
        *state |= DECL_STATE_NOSTWRN;   // Only warn once for each declarator list
    }

    // Additionally warn if assuming 'int' type - should be an error in strict C99 mode
    if( *state & DECL_STATE_NOTYPE ) {
        CWarn2p( WARN_NO_DATA_TYPE_GIVEN, ERR_NO_DATA_TYPE_GIVEN, sym->name );
    }
    if( CompFlags.rent ) {
        sym->attribs.rent = TRUE; //Assume instance data
    } else {
        sym->attribs.rent = FALSE;//Assume non instance data
    }
    if( sym->attribs.naked ) {
        CErr1( ERR_INVALID_DECLSPEC );
    }

    if( SymLevel == 0 ) {
        /*
        //  SymLevel == 0 is global scope (SymLevel is the count of nested {'s)
        */
        if( (stg_class == SC_AUTO) || (stg_class == SC_REGISTER) ) {
            CErr1( ERR_INV_STG_CLASS_FOR_GLOBAL );
            stg_class = SC_STATIC;
        } else if( stg_class == SC_NONE ) {
            CompFlags.external_defn_found = 1;
        }
        if( sym->attribs.declspec == DECLSPEC_THREAD ) {
            if( !CompFlags.thread_data_present ) {
                ThreadSeg = DefThreadSeg();
                CompFlags.thread_data_present = 1;
            }
            sym->u.var.segid = ThreadSeg;
        }
    } else {
        /*
        //  SymLevel != 0 is function scoped (SymLevel is the count of nested {'s)
        */
        if( stg_class == SC_NONE ) {
            stg_class = SC_AUTO;
        }
        if( stg_class == SC_AUTO || stg_class == SC_REGISTER ) {
            if( sym->mods & MASK_LANGUAGES ) {
                CErr1( ERR_INVALID_DECLARATOR );
            }
            if( sym->attribs.declspec != DECLSPEC_NONE ) {
                CErr1( ERR_INVALID_DECLSPEC );
            }
            /*
            // Local variables in stack will be far when SS != DS (/zu)
            // (applies only to auto vars, functions params are handled
            // NOT here but in "cexpr.c" [OPR_PUSHADDR])
            */
            if( TargetSwitches & FLOATING_SS ) {
                sym->mods |= FLAG_FAR;
            }
        }
        /*
        // static class variables can be thread local also
        */
        if( (stg_class == SC_STATIC) && (sym->attribs.declspec == DECLSPEC_THREAD) ) {
            if( !CompFlags.thread_data_present ) {
                ThreadSeg = DefThreadSeg();
                CompFlags.thread_data_present = 1;
            }
            sym->u.var.segid = ThreadSeg;
        }
    }
    if( (Toggles & TOGGLE_UNREFERENCED) == 0 ) {
        sym->flags |= SYM_IGNORE_UNREFERENCE;
    }
    old_sym_handle = SymLook( sym->info.hash, sym->name );
    if( old_sym_handle != SYM_NULL ) {
        SymGet( &old_sym, old_sym_handle );
        if( ChkEqSymLevel( &old_sym ) ) {
            SetDiagSymbol( &old_sym, old_sym_handle );
            if( old_sym.attribs.stg_class == SC_EXTERN && stg_class == SC_EXTERN ) {
                if( ! IdenticalType( old_sym.sym_type, sym->sym_type ) ) {
                    CErr2p( ERR_TYPE_DOES_NOT_AGREE, sym->name );
                }
            } else if( old_sym.attribs.stg_class == SC_TYPEDEF ) {
                CErr2p( ERR_SYM_ALREADY_DEFINED, sym->name );
            }
            SetDiagPop();
        }
    }
    if( stg_class == SC_EXTERN ) {
        old_sym_handle = Sym0Look( sym->info.hash, sym->name );
    }
    if( old_sym_handle != SYM_NULL ) {
        SymGet( &old_sym, old_sym_handle );
        SetDiagSymbol( &old_sym, old_sym_handle );
        if( ChkEqSymLevel( &old_sym ) || stg_class == SC_EXTERN ) {
            old_attrs = old_sym.mods;
            new_attrs = sym->mods;
            /* add default far/near flags depending on data model */
            if( TargetSwitches & BIG_DATA ) {
                old_attrs |= FLAG_FAR;
                new_attrs |= FLAG_FAR;
            } else {
                old_attrs |= FLAG_NEAR;
                new_attrs |= FLAG_NEAR;
            }
            if( (new_attrs & ~MASK_FUNC) != (old_attrs & ~MASK_FUNC) ) {
                 CErr2p( ERR_MODIFIERS_DISAGREE, sym->name );
            }
            if( (sym->mods & MASK_LANGUAGES) != (old_sym.mods & MASK_LANGUAGES) ) {
                // just inherit old lang flags
                // if new != 0 then it's possible someone saw a different prototype
                if( (sym->mods & MASK_LANGUAGES) != 0 ) {
                     CErr2p( ERR_MODIFIERS_DISAGREE, sym->name );
                }
            }
            if( sym->attribs.declspec != old_sym.attribs.declspec ) {
                switch( sym->attribs.declspec ) {
                case DECLSPEC_DLLIMPORT:
                case DECLSPEC_THREAD:
                    CErr2p( ERR_MODIFIERS_DISAGREE, sym->name );
                    break;
                case DECLSPEC_DLLEXPORT:
                    if( old_sym.attribs.declspec == DECLSPEC_DLLIMPORT ) {
                        old_sym.attribs.declspec = DECLSPEC_DLLEXPORT;
                    } else {
                        CErr2p( ERR_MODIFIERS_DISAGREE, sym->name );
                    }
                    break;
                }
            }
        }
        SetDiagPop();
    }
    if( ( old_sym_handle != SYM_NULL )
      && (stg_class == SC_NONE || stg_class == SC_EXTERN
      || (stg_class == SC_STATIC && SymLevel == 0)) ) {

        /* make sure sym->sym_type same type as old_sym->sym_type */

        SetDiagSymbol( &old_sym, old_sym_handle );
        old_def = VerifyType( sym->sym_type, old_sym.sym_type, sym );
        SetDiagPop();
        if( !old_def && ChkEqSymLevel( &old_sym ) ) {
            /* new symbol's type supersedes old type */
            old_sym.sym_type = sym->sym_type;
            if( (old_sym.flags & SYM_FUNCTION) ) {
                old_sym = *sym;  // ditch old sym
            }
            SymReplace( &old_sym, old_sym_handle );
        }
        if( stg_class == SC_EXTERN && SymLevel != 0 )
            goto new_var;
        CMemFree( sym->name );
        memcpy( sym, &old_sym, sizeof( SYM_ENTRY ) );
        sym_handle = old_sym_handle;
        SetDiagSymbol( &old_sym, old_sym_handle );
        /* verify that newly specified storage class doesn't conflict */
        if( (stg_class == SC_NONE) || (stg_class == SC_STATIC) ) {
            if( sym->attribs.stg_class == SC_EXTERN ) {
                /* was extern, OK to change to none */
                if( stg_class == SC_NONE ) {
                    sym->attribs.stg_class = stg_class;
                } else {
                    /* was extern, not OK to make static */
                    CErrSymName( ERR_STG_CLASS_DISAGREES, sym, sym_handle );
                }
            } else if( sym->attribs.stg_class == SC_STATIC && stg_class == SC_NONE ) {
                /* was static, not OK to redefine */
                CErrSymName( ERR_STG_CLASS_DISAGREES, sym, sym_handle );
            } else if( sym->attribs.stg_class == SC_NONE && stg_class == SC_STATIC ) {
                /* was extern linkage, not OK to to make static */
                CErrSymName( ERR_STG_CLASS_DISAGREES, sym, sym_handle );
            }
        }
        SetDiagPop();
    } else {
        if( stg_class == SC_EXTERN && SymLevel != 0 ) {
            ; /* do nothing */
        } else {
            VfyNewSym( sym->info.hash, sym->name );
        }
new_var:
        old_sym_handle = SYM_NULL;
        sym->flags |= SYM_DEFINED;
        typ = sym->sym_type;
        SKIP_DUMMY_TYPEDEFS( typ );
        if( typ->decl_type == TYPE_TYPEDEF ) {
            SymGet( &sym2, typ->u.typedefn );
            if( sym->u.var.segid == SEG_UNKNOWN && sym2.u.var.segid != SEG_UNKNOWN ) {
                sym->u.var.segid = sym2.u.var.segid;
            }
            SKIP_TYPEDEFS( typ );
        }
        if( typ->decl_type == TYPE_VOID ) {
            CErr2p( ERR_VAR_CANT_BE_VOID, sym->name );
            sym->sym_type = TypeDefault();
        }
        sym->attribs.stg_class = stg_class;
        sym_handle = SymAdd( sym->info.hash, sym );
    }
    if( sym->u.var.segid == SEG_UNKNOWN
      && ( stg_class == SC_STATIC
      || stg_class == SC_NONE
      || stg_class == SC_EXTERN ) ) {
        if( DefDataSegment != SEG_UNKNOWN ) {
            sym->u.var.segid = DefDataSegment;
            SymReplace( sym, sym_handle );
        }
    }
    if( CurToken == T_EQUAL ) {
        if( stg_class == SC_EXTERN ) {
            stg_class = SC_STATIC;
            if( SymLevel == 0 ) {
                CompFlags.external_defn_found = 1;
                stg_class = SC_NONE;
            } else {
                CErr1( ERR_CANT_INITIALIZE_EXTERN_VAR );
            }
        }
        sym->attribs.stg_class = stg_class;
        NextToken();
        VarDeclEquals( sym, sym_handle );
        sym->flags |=  SYM_ASSIGNED;
    }
    SymReplace( sym, sym_handle );
    if( old_sym_handle != SYM_NULL ) {
        sym_handle = SYM_NULL;
    }
    return( sym_handle );
}

static void AdjSymTypeNode( SYMPTR sym, type_modifiers decl_mod )
{
    TYPEPTR     typ;

    if( decl_mod ) {
        typ = sym->sym_type;
        if( typ->decl_type == TYPE_FUNCTION ) {
            if( (sym->mods & MASK_LANGUAGES) != decl_mod ) {
                if( sym->mods & MASK_LANGUAGES ) {
                    CErr1( ERR_INVALID_DECLSPEC );
                } else {
                    sym->mods |= decl_mod;
                    if( (typ->u.fn.decl_flags & MASK_LANGUAGES) != decl_mod ) {
                        if( typ->u.fn.decl_flags & MASK_LANGUAGES ) {
                            CErr1( ERR_INVALID_DECLSPEC );
                        } else {
                            sym->sym_type = FuncNode( typ->object, typ->u.fn.decl_flags | decl_mod, typ->u.fn.parms );
                        }
                    }
                }
            }
        } else {
            TYPEPTR     *xtyp;

            xtyp = &sym->sym_type;
            while( ( typ->object != NULL ) && ( typ->decl_type == TYPE_POINTER ) ) {
                xtyp = &typ->object;
                typ = typ->object;
            }
            if( typ->decl_type == TYPE_FUNCTION ) {
                if( (typ->u.fn.decl_flags & MASK_LANGUAGES) != decl_mod ) {
                    if( typ->u.fn.decl_flags & MASK_LANGUAGES ) {
                        CErr1( ERR_INVALID_DECLSPEC );
                    } else {
                        *xtyp = FuncNode( typ->object, typ->u.fn.decl_flags | decl_mod, typ->u.fn.parms );
                    }
                }
            } else {
                if( sym->mods & MASK_LANGUAGES ) {
                    CErr1( ERR_INVALID_DECLSPEC );
                } else {
                    sym->mods |= decl_mod;
                }
            }
        }
    }
}

static SYM_HANDLE InitDeclarator( SYMPTR sym, decl_info const * const info, decl_state *state )
{
    SYM_HANDLE      sym_handle;
    SYM_HANDLE      old_sym_handle;
    type_modifiers  flags;
    TYPEPTR         typ;
    TYPEPTR         otyp;
    SYM_ENTRY       old_sym;

    if( ParmList != NULL ) {
        FreeParmList();
    }
    memset( sym, 0, sizeof( SYM_ENTRY ) );
    sym->name = "";
    flags = TypeQualifier();
    if( flags & info->mod ) {
       CErr1( ERR_INV_TYPE );
    }
    flags |= info->mod;
    if( info->decl == DECLSPEC_DLLEXPORT ) {
        flags |= FLAG_EXPORT; //need to get rid of this
    }
    Declarator( sym, flags, info->typ, *state );
    if( sym->name[0] == '\0' ) {
        InvDecl();
        return( SYM_NULL );
    }
    typ = sym->sym_type;
    /* skip over typedef's */
    SKIP_TYPEDEFS( typ );
    if( info->stg == SC_TYPEDEF ) {
        if( CompFlags.extensions_enabled ) {
            old_sym_handle = SymLook( sym->info.hash, sym->name );
            if( old_sym_handle != SYM_NULL ) {
                SymGet( &old_sym, old_sym_handle );
                otyp = old_sym.sym_type;        /* skip typedefs */
                SKIP_TYPEDEFS( otyp );
                if( old_sym.attribs.stg_class == SC_TYPEDEF &&
                    ChkEqSymLevel( &old_sym ) && IdenticalType( typ, otyp ) ) {
                    return( SYM_NULL );        /* indicate already in symbol tab */
                }
            }
        }
        VfyNewSym( sym->info.hash, sym->name );
        sym->attribs.stg_class = info->stg;
        AdjSymTypeNode( sym, info->decl_mod );
        sym_handle = SymAdd( sym->info.hash, sym );
    } else {
        sym->attribs.declspec = info->decl;
        sym->attribs.naked = info->naked;
        if( sym->attribs.declspec == DECLSPEC_DLLEXPORT ) { //sync up flags
            sym->mods |= FLAG_EXPORT; //need to get rid of this
        } else if( sym->mods & FLAG_EXPORT ) {
            if( sym->attribs.declspec == DECLSPEC_NONE ) {
                sym->attribs.declspec = DECLSPEC_DLLEXPORT;
            } else if( sym->attribs.declspec  != DECLSPEC_DLLEXPORT ) {
                 CErr1( ERR_INVALID_DECLSPEC );
            }
        }
        AdjSymTypeNode( sym, info->decl_mod );
        if( typ->decl_type == TYPE_FUNCTION ) {
            sym_handle = FuncDecl( sym, info->stg, state );
        } else {
            sym_handle = VarDecl( sym, info->stg, state );
        }
    }
    return( sym_handle );
}

bool DeclList( SYM_HANDLE *sym_head )
{
    decl_state          state;
    SYM_HANDLE          sym_handle;
    SYM_HANDLE          prevsym_handle;
    auto SYM_ENTRY      sym;
    auto SYM_ENTRY      prevsym;
    decl_info           info;

    ParmList = NULL;
    prevsym_handle = SYM_NULL;
    *sym_head = SYM_NULL;
    for( ;; ) {
        for( ;; ) {
            for( ;; ) {
                while( CurToken == T_SEMI_COLON ) {
                    if( SymLevel == 0 ) {
                        if( !CompFlags.extensions_enabled ) {
                            CErr2p( ERR_EXPECTING_DECL_BUT_FOUND, ";" );
                        }
                    }
                    NextToken();
                }
                if( CurToken == T_EOF )
                    return( FALSE );
                FullDeclSpecifier( &info );
                if( info.stg != SC_NONE || info.typ != NULL )
                    break;
                if( SymLevel != 0 )
                    return( FALSE );
                break;
            }
            state = DECL_STATE_NONE;
            if( info.typ == NULL ) {
                state |= DECL_STATE_NOTYPE;
                info.typ = TypeDefault();
            }
            if( info.stg == SC_NONE && (state & DECL_STATE_NOTYPE) ) {
                if( TokenClass[CurToken] == TC_MODIFIER ) {
                } else {
                    switch( CurToken ) {
                    case T_ID:
                    case T_LEFT_PAREN:
                    case T_TIMES:
                        break;
                    case T_IF:
                    case T_FOR:
                    case T_WHILE:
                    case T_DO:
                    case T_SWITCH:
                    case T_BREAK:
                    case T_CONTINUE:
                    case T_CASE:
                    case T_DEFAULT:
                    case T_ELSE:
                    case T_GOTO:
                    case T_RETURN:
                        FlushBadCode();
                        continue;
                    default:
                        CErr2p( ERR_EXPECTING_DECL_BUT_FOUND, Tokens[CurToken] );
                        NextToken();
                        break;
                    }
                }
            }
            break;
        }
        if( CurToken != T_SEMI_COLON ) {
            if( info.decl == DECLSPEC_DLLIMPORT ) {
                if( !CompFlags.rent ) {
                    if( info.stg == SC_NONE ) {
                        info.stg = SC_EXTERN;
                    }
                }
            }
            for( ;; ) {
                sym_handle = InitDeclarator( &sym, &info, &state );
                /* NULL is returned if sym already exists in symbol table */
                if( sym_handle != SYM_NULL ) {
                    sym.handle = SYM_NULL;
                    if( sym.flags & SYM_FUNCTION ) {
                        if( (state & DECL_STATE_NOTYPE ) == 0 ) {
                            sym.flags |= SYM_TYPE_GIVEN;
                        }
                    } else if( SymLevel > 0 ) { /* variable */
                        if( prevsym_handle != SYM_NULL ) {
                            SymGet( &prevsym, prevsym_handle );
                            prevsym.handle = sym_handle;
                            SymReplace( &prevsym, prevsym_handle );
                        }
                        if( *sym_head == SYM_NULL ) {
                            *sym_head = sym_handle;
                        }
                        prevsym_handle = sym_handle;
                    }
                    SymReplace( &sym, sym_handle );
                }
                /* case "int x *p" ==> missing ',' msg already given */
                if( CurToken != T_TIMES ) {
                    if( CurToken != T_COMMA )
                        break;
                    NextToken();
                }
            }
/*              the following is illegal:
                    typedef double math(double);
                    math sin { ; }
            That's the reason for the check
                    "typ->decl_type != TYPE_FUNCTION"
*/
            if( SymLevel == 0 && CurToken != T_SEMI_COLON && sym_handle != SYM_NULL ) {
                if( sym.sym_type->decl_type == TYPE_FUNCTION && sym.sym_type != info.typ ) {
                    CurFuncHandle = sym_handle;
                    CurFunc = &CurFuncSym;
                    memcpy( CurFunc, &sym, sizeof( SYM_ENTRY ) );
                    return( TRUE );     /* indicate this is a function defn */
                }
            }
            MustRecog( T_SEMI_COLON );
        } else {
            Chk_Struct_Union_Enum( info.typ );
            NextToken();                /* skip over ';' */
        }
    }
// can't get here!      return( FALSE );
}


bool LoopDecl( SYM_HANDLE *sym_head )
{
    decl_state          state;
    SYM_HANDLE          sym_handle;
    SYM_HANDLE          prevsym_handle;
    SYM_ENTRY           sym;
    SYM_ENTRY           prevsym;
    decl_info           info;

    ParmList = NULL;
    prevsym_handle = SYM_NULL;
    *sym_head = SYM_NULL;
    if( CurToken == T_EOF )
        return( FALSE );
    FullDeclSpecifier( &info );
    if( info.stg == SC_NONE && info.typ == NULL ) {
        return( FALSE );    /* No declaration-specifiers, get outta here */
    }
    if( info.stg != SC_NONE && info.stg != SC_AUTO && info.stg != SC_REGISTER ) {
        CErr1( ERR_INVALID_STG_CLASS_FOR_LOOP_DECL );
    }
    state = DECL_STATE_FORLOOP;
    if( info.typ == NULL ) {
        /* C99 requires a type specifier; we could get away with defaulting
         * to int but what would be the point?
         */
        CErr1( ERR_NO_TYPE_IN_DECL );
        info.typ = TypeDefault();
    }
    if( CurToken != T_SEMI_COLON ) {
        for( ;; ) {
            sym_handle = InitDeclarator( &sym, &info, &state );
            /* NULL is returned if sym already exists in symbol table */
            if( sym_handle != SYM_NULL ) {
                sym.handle = SYM_NULL;
                if( sym.flags & SYM_FUNCTION ) {
                    if( (state & DECL_STATE_NOTYPE ) == 0 ) {
                        sym.flags |= SYM_TYPE_GIVEN;
                    }
                } else {    /* variable */
                    if( prevsym_handle != SYM_NULL ) {
                        SymGet( &prevsym, prevsym_handle );
                        prevsym.handle = sym_handle;
                        SymReplace( &prevsym, prevsym_handle );
                    }
                    if( *sym_head == SYM_NULL ) {
                        *sym_head = sym_handle;
                    }
                    prevsym_handle = sym_handle;
                }
                SymReplace( &sym, sym_handle );
            }
            /* case "int x *p" ==> missing ',' msg already given */
            if( CurToken != T_TIMES ) {
                if( CurToken != T_COMMA )
                    break;
                NextToken();
            }
        }
        MustRecog( T_SEMI_COLON );
    } else {
        //  Chk_Struct_Union_Enum( info.typ );
        NextToken();                /* skip over ';' */
    }
    return( TRUE );    /* We found a declaration */
}


TYPEPTR TypeName( void )
{
    TYPEPTR     typ;
    decl_info   info;
    SYM_ENTRY   abs_sym;

    TypeSpecifier( &info );
    typ = info.typ;
    if( typ != NULL ) {
         memset( &abs_sym, 0, sizeof( SYM_ENTRY ) );
         abs_sym.name = "";
         AbsDecl( &abs_sym, info.mod, info.typ  );
         typ = abs_sym.sym_type;
    }
    return( typ );
}


static type_modifiers GetModifiers( void )
{
    type_modifiers      modifier;

    modifier = 0;
    for( ; TokenClass[CurToken] == TC_MODIFIER; ) {
        switch( CurToken ) {
        case T___NEAR:      modifier |= FLAG_NEAR;      break;
        case T___FAR:       modifier |= FLAG_FAR;       break;
#if _CPU == 8086
        case T__FAR16:
        case T___FAR16:     modifier |= FLAG_FAR;       break;
        case T___HUGE:      modifier |= FLAG_HUGE;      break;
#else
        case T__FAR16:
        case T___FAR16:     modifier |= FLAG_FAR16;     break;
        case T___HUGE:      modifier |= FLAG_FAR;       break;
#endif
        case T___INTERRUPT: modifier |= FLAG_INTERRUPT; break;
        case T__CDECL:
        case T___CDECL:     modifier |= LANG_CDECL;     break;
        case T___FASTCALL:  modifier |= LANG_FASTCALL;  break;
        case T___FORTRAN:   modifier |= LANG_FORTRAN;   break;
        case T__OPTLINK:    modifier |= LANG_OPTLINK;   break;
        case T__PASCAL:
        case T___PASCAL:    modifier |= LANG_PASCAL;    break;
        case T___STDCALL:   modifier |= LANG_STDCALL;   break;
        case T__SYSCALL:
        case T___SYSCALL:
        case T__SYSTEM:     modifier |= LANG_SYSCALL;   break;
        case T___WATCALL:   modifier |= LANG_WATCALL;   break;
        case T__EXPORT:
        case T___EXPORT:    modifier |= FLAG_EXPORT;    break;
        case T___LOADDS:    modifier |= FLAG_LOADDS;    break;
        case T___SAVEREGS:  modifier |= FLAG_SAVEREGS;  break;
        default:
            break;
        }
        NextToken();
    }
    return( modifier );
}

struct mod_info {
    segment_id       segid;
    type_modifiers   modifier;  // const, vol flags
    BASED_KIND       based_kind;
    SYM_HANDLE       based_sym;
};

static TYPEPTR Pointer( TYPEPTR typ, struct mod_info *info )
{
    type_modifiers  flags;
    SYM_HANDLE      sym_handle;
    SYM_ENTRY       sym;

    sym_handle = SYM_NULL;
    if( (typ != NULL) && (typ->decl_type == TYPE_TYPEDEF) ) {
        // get segment from typedef TODO should be done sooner
        TYPEPTR     ptr_typ;
        SYMPTR      symp;

        ptr_typ = typ;
        SKIP_DUMMY_TYPEDEFS( ptr_typ );
        if( ptr_typ->decl_type == TYPE_TYPEDEF ) {
            symp = SymGetPtr( ptr_typ->u.typedefn );
            if( info->modifier & FLAG_BASED ) {
                info->segid = symp->u.var.segid;
                sym_handle = SegSymHandle( info->segid );
            }
        }
    }
    for( ;; ) {
        flags = GetModifiers();   // NEAR FAR CDECL stuff
        if( flags & info->modifier ) {
            CWarn1( WARN_REPEATED_MODIFIER, ERR_REPEATED_MODIFIER );
        }
        info->modifier |= flags;
        if( CurToken == T___BASED ) {
            bool    use_seg;

            use_seg = FALSE;
            NextToken();
            MustRecog( T_LEFT_PAREN );
            info->based_kind = BASED_NONE;
            if( CurToken == T_LEFT_PAREN ) {
                NextToken();
                MustRecog( T___SEGMENT );
                MustRecog( T_RIGHT_PAREN );
                use_seg = TRUE;
            }
            switch( CurToken ) {
            case T_ID:                                  /* __based(variable) */
                sym_handle = SymLook( HashValue, Buffer );
                if( sym_handle == SYM_NULL ) {
                    SymCreate( &sym, Buffer );
                    sym.attribs.stg_class = SC_EXTERN;  /* indicate extern decl */
                    CErr2p( ERR_UNDECLARED_SYM, Buffer );
                    sym.sym_type = GetType( TYPE_INT );
                    sym_handle = SymAdd( HashValue, &sym );
                } else {
                    TYPEPTR     sym_typ;

                    SymGet( &sym, sym_handle );
                    sym_typ = sym.sym_type;
                    SKIP_TYPEDEFS( sym_typ );
                    if( use_seg ) {
                        info->based_kind = BASED_VARSEG;
                        if( sym_typ->decl_type != TYPE_POINTER ) {
                           CErr1( ERR_SYM_MUST_BE_TYPE_SEGMENT );
                           info->based_kind = BASED_NONE;
                        }
                    } else if( sym_typ->decl_type == TYPE_POINTER ) {
                        info->based_kind = BASED_VAR;
                    } else if( sym_typ->decl_type < TYPE_FLOAT ) {
                        info->based_kind = BASED_SEGVAR;
                    } else {
                        CErr1( ERR_SYM_MUST_BE_TYPE_SEGMENT );
                        info->based_kind = BASED_NONE;
                    }
                }
                if( (sym.flags & SYM_REFERENCED) == 0 ) {
                    sym.flags |= SYM_REFERENCED;
                    SymReplace( &sym, sym_handle );
                }
                NextToken();
                break;
            case T_VOID:            /* __based(void)  */
                if( use_seg ) {
                    CErr1( ERR_INVALID_BASED_DECLARATOR );
                }
                info->based_kind = BASED_VOID;
                NextToken();
                break;
            case T___SEGNAME:       /* __based(__segname("string")) */
                if( use_seg ) {
                    CErr1( ERR_INVALID_BASED_DECLARATOR );
                }
                info->based_kind = BASED_SEGNAME;
                NextToken();
                MustRecog( T_LEFT_PAREN );
                if( CurToken == T_STRING ) {
                    info->segid = AddSegName( Buffer, "", SEGTYPE_BASED);
                    sym_handle = SegSymHandle( info->segid );
                    SymGet( &sym, sym_handle );
                    sym.u.var.segid = info->segid;
                    SymReplace( &sym, sym_handle );
                    NextToken();
                } else {
                    CErr1( ERR_SEGMENT_NAME_REQUIRED );
                    info->based_kind = BASED_NONE;
                }
                MustRecog( T_RIGHT_PAREN );
                break;
            case T___SELF:          /* __based(__self) */
                info->based_kind = BASED_SELFSEG;
                NextToken();
                break;
            case T_AND:             /* __based((__segment) &var ) */
                if( !use_seg ) {
                    CErr1( ERR_INVALID_BASED_DECLARATOR );
                }
                NextToken();
                if( CurToken == T_ID ) {
                    sym_handle = SymLook( HashValue, Buffer );
                    if( sym_handle == SYM_NULL ) {
                        SymCreate( &sym, Buffer );
                        sym.attribs.stg_class = SC_EXTERN;  /* indicate extern decl */
                        CErr2p( ERR_UNDECLARED_SYM, Buffer );
                        sym.sym_type = GetType( TYPE_INT );
                        sym_handle = SymAdd( HashValue, &sym );
                    } else {
                        SymGet( &sym, sym_handle );
                    }
                    if( (sym.flags & SYM_REFERENCED) == 0 ) {
                        sym.flags |= SYM_REFERENCED;
                        SymReplace( &sym, sym_handle );
                    }
                    info->based_kind = BASED_SEGNAME;
                    NextToken();
                } else {
                    CErr1( ERR_INVALID_BASED_DECLARATOR );
                }
                break;
            default:
                CErr1( ERR_INVALID_BASED_DECLARATOR );
                break;
            }
            MustRecog( T_RIGHT_PAREN );
            info->modifier &= ~MASK_ALL_MEM_MODELS;
            info->modifier = FLAG_NEAR | FLAG_BASED;
        }
        if( CurToken == T_TIMES ) {
            NextToken();
#if ( _CPU == 8086 ) || ( _CPU == 386 )
            // * seg16 binds with * cause of IBM dorks, and so does far16
            if( (CurToken == T__SEG16) || (CurToken == T__FAR16) || (CurToken == T___FAR16) ) {
#if _CPU == 386
                info->modifier |= FLAG_FAR16;
#else
                info->modifier |= FLAG_FAR;
#endif
                NextToken();
            }
#endif
            flags = info->modifier & ~FLAG_EXPORT;
            typ = BPtrNode( typ, flags, info->segid, sym_handle, info->based_kind );
            sym_handle = SYM_NULL;
            info->segid = SEG_UNKNOWN;  // start over
            info->modifier = (flags & FLAG_INLINE) | TypeQualifier();  // .. * const
            info->based_kind = BASED_NONE;
        } else {
            break;
        }
    }
    return( typ );
}


static void ParseDeclPart2( TYPEPTR *typep, TYPEPTR typ, type_modifiers mod )
{
    TYPEPTR         decl1;
    TYPEPTR         decl2;

    decl1 = *typep;
    if( decl1 != NULL ) {
        while( decl1->object != NULL ) {
            decl1 = decl1->object;
        }
    }
    // Pass on pointer flags
    if( (decl1 != NULL) && (decl1->decl_type == TYPE_POINTER) )
        mod = decl1->u.p.decl_flags;
    decl2 = DeclPart2( typ, mod );
    if( decl1 == NULL ) {
        *typep = decl2;
    } else {
        decl1->object = decl2;
        if( decl1->decl_type == TYPE_POINTER && decl2 != NULL ) {
            AddPtrTypeHash( decl1 );
        }
    }
}


static void AbsDecl( SYMPTR sym, type_modifiers mod, TYPEPTR typ )
{
    struct mod_info     info;

    info.segid = SEG_UNKNOWN;
    info.modifier = mod;
    info.based_kind = BASED_NONE;
    info.based_sym = SYM_NULL;
    typ = Pointer( typ, &info );
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        if( CurToken == T_RIGHT_PAREN ) {
            typ = FuncNode( typ, info.modifier, NULL );
        } else {
            AbsDecl( sym, info.modifier, (TYPEPTR)NULL );
            info.modifier = FLAG_NONE;
            MustRecog( T_RIGHT_PAREN );
            ParseDeclPart2( &sym->sym_type, typ, FLAG_NONE );
        }
    } else {
        sym->mods = info.modifier;
        sym->u.var.segid = info.segid;
        typ = DeclPart2( typ, info.modifier );
        sym->sym_type = typ;
    }
}

void Declarator( SYMPTR sym, type_modifiers mod, TYPEPTR typ, decl_state state )
{
    TYPEPTR             *type_list;
    TYPEPTR             parm_type;
    struct mod_info     info;

    info.segid = SEG_UNKNOWN;
    info.modifier = mod;
    info.based_kind = BASED_NONE;
    info.based_sym = SYM_NULL;
    typ = Pointer( typ, &info );
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        if( state & DECL_STATE_ISPARM ) {
            parm_type = TypeName();
        } else {
            parm_type = NULL;
        }
        if( parm_type != NULL || CurToken == T_RIGHT_PAREN ) {
            type_list = NULL;
            if( parm_type != NULL ) {
                type_list = MakeParmList( NewParm( parm_type, NULL ), FALSE );
            }
            typ = FuncNode( typ, info.modifier, type_list );
            typ = PtrNode( typ, FLAG_NONE, SEG_DATA );
            MustRecog( T_RIGHT_PAREN );
        } else {
            if( (state & DECL_STATE_ISPARM) && TokenClass[CurToken] == TC_STG_CLASS ) {
                typ = DeclPart3( typ, info.modifier );
            } else {
                Declarator( sym, info.modifier, (TYPEPTR)NULL, state );
                info.modifier = FLAG_NONE;
                MustRecog( T_RIGHT_PAREN );
            }
        }
        ParseDeclPart2( &sym->sym_type, typ, sym->mods );
        typ = sym->sym_type;
    } else {
        if( (CurToken == T_ID) || (CurToken == T_SAVED_ID) ) {
            for( ;; ) {
                if( CurToken == T_ID ) {
                    SymCreate( sym, Buffer );
                    sym->info.hash = HashValue;
                    NextToken();
                } else {
                    SymCreate( sym, SavedId );
                    sym->info.hash = SavedHash;
                    CurToken = LAToken;
                }
                if( CurToken != T_ID && CurToken != T_TIMES )
                    break;
                if( state & DECL_STATE_NOTYPE ) {
                    CErr2p( ERR_MISSING_DATA_TYPE, sym->name );
                    if( CurToken == T_TIMES ) { /* "garbage *p" */
                        typ = Pointer( typ, &info );
                    }
                } else if( CurToken == T_TIMES ) {
                    Expecting( "," );
                } else {
                    Expecting( ",' or ';" );
                }
                if( CurToken != T_ID )
                    break;
                CMemFree( sym->name );
            }
        } else {
            SymCreate( sym, "" );
        }
        sym->mods = info.modifier;
        sym->u.var.segid = info.segid;
#if 0
        if( modifier & FLAG_INTERRUPT )
            sym->flags |= SYM_INTERRUPT_FN;
#endif
        typ = DeclPart2( typ, info.modifier );
        sym->sym_type = typ;
    }
    if( typ != NULL ) {
        if( typ->decl_type == TYPE_FUNCTION ) {
            if( state & DECL_STATE_FORLOOP ) {
                CErr2p( ERR_DECL_IN_LOOP_NOT_OBJECT, sym->name );
            } else if( info.segid != SEG_UNKNOWN ) {            // __based( __segname("X"))
                SetFuncSegment( sym, info.segid );
            }
        }
    }
}


FIELDPTR FieldCreate( const char *name )
{
    FIELDPTR    field;
    size_t      len;

    len = strlen( name ) + 1;
    field = (FIELDPTR)CPermAlloc( sizeof( FIELD_ENTRY ) - 1 + len );
    memcpy( field->name, name, len );
    if( CompFlags.emit_browser_info ) {
        field->xref = NewXref( NULL );
    }
    return( field );
}


FIELDPTR FieldDecl( TYPEPTR typ, type_modifiers mod, decl_state state )
{
    FIELDPTR            field;
    struct mod_info     info;

    info.segid = SEG_UNKNOWN;
    info.modifier = mod;
    info.based_kind = BASED_NONE;
    info.based_sym = SYM_NULL;
    typ = Pointer( typ, &info );
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        field = FieldDecl( (TYPEPTR)NULL, info.modifier, DECL_STATE_NONE );
        info.modifier = FLAG_NONE;
        MustRecog( T_RIGHT_PAREN );
        ParseDeclPart2( &field->field_type, typ, FLAG_NONE );
    } else {
        if( CurToken == T_ID ) {
            for( ;; ) {
                field = FieldCreate( Buffer );
                NextToken();
                if( CurToken != T_ID && CurToken != T_TIMES )
                    break;
                if( state & DECL_STATE_NOTYPE ) {
                    CErr2p( ERR_MISSING_DATA_TYPE, field->name );
                    if( CurToken == T_TIMES ) { /* "garbage *p" */
                        typ = Pointer( typ, &info );
                    }
                } else if( CurToken == T_TIMES ) {
                    Expecting( "," );
                } else {
                    Expecting( ",' or ';" );
                }
                if( CurToken != T_ID ) {
                    break;
                }
            }
        } else {
            field = FieldCreate( "" );
        }
        field->attrib = info.modifier;
        typ = DeclPart2( typ, info.modifier );
        field->field_type = typ;
    }
    return( field );
}


static TYPEPTR ArrayDecl( TYPEPTR typ )
{
    target_size dimension;
    TYPEPTR     first_node, next_node, prev_node;

    if( typ != NULL ) {
        if( typ->decl_type == TYPE_FUNCTION ) {
            CErr1( ERR_CANT_HAVE_AN_ARRAY_OF_FUNCTIONS );
        }
    }
    first_node = NULL;
    prev_node = NULL;
    while( CurToken == T_LEFT_BRACKET ) {
        NextToken();
        if( CurToken != T_RIGHT_BRACKET ) {
            const_val   val;

            if( ConstExprAndType( &val ) ) {
                dimension = U32FetchTrunc( val.value );
                switch( val.type ) {
                case TYPE_ULONG64:
                    if( !U64IsU32( val.value ) )
                        CErr1( ERR_CONSTANT_TOO_BIG );
                    break;
                case TYPE_LONG64:
                    if( !U64IsI32( val.value ) )
                        CErr1( ERR_CONSTANT_TOO_BIG );
                    break;
                case TYPE_ULONG:
                case TYPE_UINT:
                    break;
                default:
                    if( I32FetchTrunc( val.value ) <= 0 ) {
                        CErr1( ERR_INVALID_DIMENSION );
                        dimension = 1;
                    }
                    break;
                }
            } else {
                dimension = 1;
            }
        } else {
            if( first_node == NULL ) {
                dimension = 0;
            } else {
                CErr1( ERR_DIMENSION_REQUIRED );
                dimension = 1;
            }
        }
        MustRecog( T_RIGHT_BRACKET );
        next_node = ArrayNode( typ );
        next_node->u.array->dimension = dimension;
        next_node->u.array->unspecified_dim = ( dimension == 0 );
        if( first_node == NULL ) {
            first_node = next_node;
        } else {
            prev_node->object = next_node;
        }
        prev_node = next_node;
    }
    return( first_node );
}


static TYPEPTR  *FuncProtoType( void );

static TYPEPTR DeclPart3( TYPEPTR typ, type_modifiers mod )
{
    PARMPTR     parms_list;
    TYPEPTR     *parms;

    parms = NULL;
    if( CurToken != T_RIGHT_PAREN ) {
        parms_list = ParmList;
        ParmList = NULL;
        parms = FuncProtoType();
        if( (parms == NULL) && (ParmList != NULL) ) {
            if( CurToken == T_SEMI_COLON || CurToken == T_COMMA ) {
                /* int f16(i,j); */
                CErr1( ERR_ID_LIST_SHOULD_BE_EMPTY );
            }
            /* Old-style declarations are obsolescent (ever since ANSI C89!) */
            CWarn1( WARN_OBSOLETE_FUNC_DECL, ERR_OBSOLETE_FUNC_DECL );
        }
        if( parms_list != NULL ) {
            FreeParmList();
            ParmList = parms_list;
        }
    } else {
        NextToken();    /* skip over ')' */
        /* Non-prototype declarators are obsolescent too; however, __interrupt
         * functions have a special exemption due to messy historical usage,
         * with variants both with and without arguments in use. Note that
         * __interrupt functions are unlikely to be called directly.
         */
        if( (mod & FLAG_INTERRUPT) == 0 ) {
            CWarn1( WARN_OBSOLETE_FUNC_DECL, ERR_OBSOLETE_FUNC_DECL );
        }
    }
    if( typ != NULL ) {
        TYPEPTR     typ2;

        typ2 = typ;
        SKIP_TYPEDEFS( typ2 );
        if( typ2->decl_type == TYPE_ARRAY ) {
            CErr1( ERR_FUNCTION_CANT_RETURN_AN_ARRAY );
        } else if( typ2->decl_type == TYPE_FUNCTION ) {
            CErr1( ERR_FUNCTION_CANT_RETURN_A_FUNCTION );
        }
    }
    typ = FuncNode( typ, mod, parms );
    return( typ );
}


static TYPEPTR DeclPart2( TYPEPTR typ, type_modifiers mod )
{
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        typ = DeclPart3( typ, mod );
    }
    for( ;; ) {
        if( CurToken == T_LEFT_BRACKET ) {
            typ = ArrayDecl( typ );
        }
        if( CurToken != T_LEFT_PAREN )
            break;
        NextToken();
        typ = DeclPart3( typ, mod );
    }
    return( typ );
}


static void CheckUniqueName( PARMPTR parm, const char *name )
{
    size_t  len;

    if( name != NULL ) {
        if( *name != '\0' ) {
            len = strlen( name ) + 1;
            for( ; parm != NULL; parm = parm->next_parm ) {
                if( parm->sym.name != NULL ) {
                    if( memcmp( parm->sym.name, name, len ) == 0 ) {
                        CErr2p( ERR_SYM_ALREADY_DEFINED, name );
                        parm->sym.flags |= SYM_REFERENCED;
                    }
                }
            }
        }
    }
}


parm_list *NewParm( TYPEPTR typ, parm_list *prev_parm )
{
    parm_list    *parm;

    parm = (parm_list *)CMemAlloc( sizeof( parm_list ) );
    parm->parm_type = typ;
    parm->next_parm = prev_parm;
    return( parm );
}


void AdjParmType( SYMPTR sym )
{
    TYPEPTR     typ;

    typ = sym->sym_type;
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type == TYPE_FUNCTION ) {
        sym->sym_type = PtrNode( sym->sym_type, sym->mods, SEG_CODE );
        sym->mods = FLAG_NONE;
    } else if( typ->decl_type == TYPE_ARRAY ) {
        sym->sym_type = PtrNode( typ->object, FLAG_WAS_ARRAY | sym->mods, SEG_DATA );
        sym->mods = FLAG_NONE;
    }
}


static TYPEPTR *GetProtoType( decl_info *first )
{
    PARMPTR             parm;
    PARMPTR             prev_parm = NULL;
    PARMPTR             parm_namelist;
    int                 parm_count;
    parm_list           *parmlist;
    decl_state          state;
//    declspec_class      declspec;
    stg_classes         stg_class;
    type_modifiers      mod;
    TYPEPTR             typ;
    decl_info           info;

    parm_count = 0;
    parmlist = NULL;
    parm_namelist = NULL;
    info = *first;
    for( ;; ) {
        SYMPTR           sym; // parm sym

        stg_class = info.stg;
//        declspec = info.decl;
        typ = info.typ;
        mod = info.mod;
        if( stg_class != SC_NONE && stg_class != SC_REGISTER ) {
            CErr1( ERR_INVALID_STG_CLASS_FOR_PARM_PROTO );
        }
        state = DECL_STATE_ISPARM;
        if( typ == NULL ) {
            state |= DECL_STATE_NOTYPE;
            if( stg_class == SC_NONE ) {
                CErr1( ERR_TYPE_REQUIRED_IN_PARM_LIST );
            }
            typ = TypeDefault();
        }
        parm = (PARMPTR) CMemAlloc( sizeof( PARM_ENTRY ) );
        parm->next_parm = NULL;
        sym = &parm->sym;
        memset( sym, 0, sizeof( SYM_ENTRY ) );
        sym->name = "";
        Declarator( sym, mod, typ, state );
        typ = sym->sym_type;
        SKIP_TYPEDEFS( typ );
        if( typ->decl_type == TYPE_VOID ) {
            char    buffer[20];
            char    *name;
            if( sym->name[0] == '\0' ) {
                sprintf( buffer, "Parm %d", parm_count );
                name = buffer;
            } else {
                name = sym->name;
            }
            CErr2p( ERR_VAR_CANT_BE_VOID, name );
            sym->sym_type = TypeDefault();
        }
        if( stg_class == SC_NONE ) {
            stg_class = SCSpecifier();
            if( stg_class == SC_NONE ) {
                stg_class = SC_AUTO;
            }
        }
        sym->attribs.stg_class = stg_class;
        AdjSymTypeNode( sym, info.decl_mod );
        AdjParmType( sym );
        parmlist = NewParm( sym->sym_type, parmlist );
        if( parm_count == 0 ) {
            parm_namelist = parm;
        } else {
            CheckUniqueName( parm_namelist, sym->name );
            prev_parm->next_parm = parm;
        }
        if( (Toggles & TOGGLE_UNREFERENCED) == 0 ) {
            sym->flags |= SYM_REFERENCED;
        }
        ++parm_count;
        if( CurToken == T_RIGHT_PAREN )
            break;
        if( (CurToken == T_EOF) || (CurToken == T_LEFT_BRACE) )
            break;
        MustRecog( T_COMMA );
        if( CurToken == T_DOT_DOT_DOT ) {
            typ = GetType( TYPE_DOT_DOT_DOT );
            parmlist = NewParm( typ, parmlist );
            NextToken();
            break;
        }
        prev_parm = parm;
        FullDeclSpecifier( &info );
    }
    ParmList = parm_namelist;
    /* if void is specified as a parm, it is the only parm allowed */
    return( MakeParmList( parmlist, FALSE ) );
}


TYPEPTR *MakeParmList( parm_list *parm, bool reversed )
{
    TYPEPTR             *parm_types;
    parm_list           *next_parm;
    parm_list           *prev_parm;
    TYPEPTR             typ;
    parm_hash_idx       h;
    int                 parm_count;

    parm_types = NULL;
    if( parm != NULL ) {
        if( !reversed ) {
            prev_parm = NULL;
            for( ;; ) {
                next_parm = parm->next_parm;
                parm->next_parm = prev_parm;
                if( next_parm == NULL )
                    break;
                prev_parm = parm;
                parm = next_parm;
            }
        }
        parm_count = 0;
        for( next_parm = parm; next_parm != NULL; next_parm = next_parm->next_parm ) {
            ++parm_count;
        }

        /* try to find an existing parm list that matches */
        h = MAX_PARM_LIST_HASH_SIZE;
        if( parm_count < MAX_PARM_LIST_HASH_SIZE ) {
            h = (parm_hash_idx)parm_count;
        }
        for( typ = FuncTypeHead[h]; typ != NULL; typ = typ->next_type ) {
            next_parm = parm;
            for( parm_types = typ->u.fn.parms; ; ++parm_types ) {
                if( next_parm == NULL ) {
                    if( *parm_types != NULL )
                        break;
                    while( parm != NULL ) {
                        next_parm = parm->next_parm;
                        CMemFree( parm );
                        parm = next_parm;
                    }
                    return( typ->u.fn.parms );
                }
                if( next_parm->parm_type != *parm_types )
                    break;
                next_parm = next_parm->next_parm;
            }
        }

        parm_types = (TYPEPTR *)CPermAlloc( ( parm_count + 1 ) * sizeof( TYPEPTR ) );
        if( parm_types != NULL ) {
            parm_types[parm_count] = NULL;
            parm_count = 0;
            for( ; parm != NULL; parm = next_parm ) {
                next_parm = parm->next_parm;
                parm_types[parm_count++] = parm->parm_type;
                CMemFree( parm );
            }
        }
    }
    return( parm_types );
}

static bool VoidType( TYPEPTR typ )
{
    if( typ != NULL ) {
        SKIP_TYPEDEFS( typ );
        if( typ->decl_type == TYPE_VOID ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

static TYPEPTR *FuncProtoType( void )
{
    TYPEPTR             *parm_types;
    TAGPTR              old_taghead;
    decl_info           info;

    if( !CompFlags.extensions_enabled ) {
        ++SymLevel;
    }
    old_taghead = TagHead;
    FullDeclSpecifier( &info );
    if( (info.stg == SC_NONE) && (info.typ == NULL) ) {
        GetFuncParmList();
        if( ExpectingToken( T_RIGHT_PAREN ) ) {
            NextToken();
        }
        parm_types = NULL;
    } else {    /* function prototype present */
        if( VoidType( info.typ ) && info.stg == SC_NONE && CurToken == T_RIGHT_PAREN ) {
            parm_types = VoidParmList;
        } else {
            parm_types = GetProtoType( &info );
        }
        if( ExpectingToken( T_RIGHT_PAREN ) ) {
            NextToken();
        }
        if( CurToken != T_LEFT_BRACE ) {
            if( SymLevel > 1 || !CompFlags.extensions_enabled ) {
                /* get rid of any new tags regardless of SymLevel */
                /* get rid of new tags from proto */
                TagHead = old_taghead;
                FreeEnums();
            }
        }
    }
    if( !CompFlags.extensions_enabled ) {
        --SymLevel;
    }
    return( parm_types );
}


static void GetFuncParmList( void )
{
    PARMPTR     parm = NULL;
    PARMPTR     newparm;
    PARMPTR     parm_namelist;

    parm_namelist = NULL;
    /* scan off func parm list */
    while( CurToken == T_ID ) {
        if( parm_namelist == NULL ) {
            parm = (PARMPTR)CMemAlloc( sizeof( PARM_ENTRY ) );
            SymCreate( &parm->sym, Buffer );
            parm_namelist = parm;
        } else {
            newparm = (PARMPTR)CMemAlloc( sizeof( PARM_ENTRY ) );
            SymCreate( &newparm->sym, Buffer );
            CheckUniqueName( parm_namelist, Buffer );
            parm->next_parm = newparm;
            parm = newparm;
        }
        parm->sym.info.hash = HashValue;
        if( (Toggles & TOGGLE_UNREFERENCED) == 0 ) {
            parm->sym.flags |= SYM_REFERENCED;
        }
        NextToken();
        if( CurToken == T_RIGHT_PAREN )
            break;
        if( CurToken == T_EOF )
            break;
        if( CurToken != T_COMMA ) {
            MustRecog( T_COMMA );               /* forces error msg */
            /* skip until ')' to avoid cascading errors */
            while( CurToken != T_RIGHT_PAREN && CurToken != T_EOF ) {
                NextToken();
            }
            break;
        }
        MustRecog( T_COMMA );
        if( CurToken == T_DOT_DOT_DOT ) {
            parm->next_parm = (PARMPTR)CMemAlloc( sizeof( PARM_ENTRY ) );
            parm = parm->next_parm;
            SymCreate( &parm->sym, "" );
            /* set flags so we don't give funny error messages */
            parm->sym.flags |= SYM_TEMP | SYM_ASSIGNED | SYM_REFERENCED;
            NextToken();
            break;
        }
    }
    ParmList = parm_namelist;
}


static void FreeParmList( void )
{
    PARMPTR     parm;

    for( ; (parm = ParmList) != NULL; ) {
        ParmList = parm->next_parm;
        CMemFree( parm->sym.name );
        CMemFree( parm );
    }
}

#if _CPU == 370
static bool IsIntComp( TYPEPTR ret1 )
    /*
     * what's target compatible between default int as ret type
     * and a later declaration
     */
{
    bool        ret;

    SKIP_TYPEDEFS( ret1 );
    switch( ret1->decl_type ) {
    case TYPE_CHAR:
    case TYPE_UCHAR:
    case TYPE_SHORT:
    case TYPE_USHORT:
    case TYPE_INT:
    case TYPE_LONG:
        ret = TRUE;
        break;
    default:
       ret = FALSE;
    }
    return( ret );
}
#endif
