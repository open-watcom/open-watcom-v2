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
local   void        FreeParmList( void );
local   void        GetFuncParmList( void );

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


local void FlushBadCode( void )
{
    int         count;

    CErr1( ERR_STMT_MUST_BE_INSIDE_FUNCTION );
    count = 0;
    for( ;; ) {
        NextToken();
        if( CurToken == T_EOF ) return;
        if( CurToken == T_LEFT_BRACE ) {
            ++count;
        }
        if( CurToken == T_RIGHT_BRACE ) {
            if( count == 0 ) break;
            --count;
        }
    }
    NextToken();
}


static stg_classes SCSpecifier( void )
{
    stg_classes     stg_class;

    stg_class = SC_NULL;        /* assume no storage class specified */
    if( TokenClass[ CurToken ] == TC_STG_CLASS ) {
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

local void CmpFuncDecls( SYMPTR new_sym, SYMPTR old_sym )
{
    TYPEPTR     type_new, type_old;

    if( (new_sym->mods & ~MASK_FUNC) != (old_sym->mods & ~MASK_FUNC) ) {
        CErr2p( ERR_MODIFIERS_DISAGREE, new_sym->name );
    }

/*      check for conflicting information */
/*      skip over TYPEDEF's   29-aug-89   */
    type_new = new_sym->sym_type;
    SKIP_TYPEDEFS( type_new );
    type_old = old_sym->sym_type;
    SKIP_TYPEDEFS( type_old );

    // diagnostics for function, target=old and source=new
    SetDiagType2( type_old->object, type_new->object );
    if( !IdenticalType( type_new->object, type_old->object ) ) {
        TYPEPTR     ret_new, ret_old;

        ret_new = type_new->object;                    /* save return types */
        ret_old = type_old->object;
        // skip over typedef's 18-may-95
        SKIP_TYPEDEFS( ret_new );
        SKIP_TYPEDEFS( ret_old );
        /* don't reorder this expression */
        if( old_sym->attribs.stg_class != SC_FORWARD ) {
            CErr2p( ERR_INCONSISTENT_TYPE, new_sym->name );
        } else if( ret_new->decl_type != TYPE_VOID
               || (old_sym->flags & SYM_TYPE_GIVEN) ) { //return value used in forward
            CErr2p( ERR_INCONSISTENT_TYPE, new_sym->name );
        }
    }
    SetDiagPop();

    /* check types of parms, including promotion */
    ChkCompatibleFunction( type_old, type_new, 1 );
}


local SYM_HANDLE FuncDecl( SYMPTR sym, stg_classes stg_class, decl_state *state )
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

    PrevProtoType = NULL;                               /* 12-may-91 */
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
            stg_class = SC_NULL;
    }
    old_sym_handle = SymLook( sym->info.hash_value, sym->name );
    if( old_sym_handle == 0 ) {
        ep = EnumLookup( sym->info.hash_value, sym->name );
        if( ep != NULL ) {
            SetDiagEnum( ep );
            CErr2p( ERR_SYM_ALREADY_DEFINED, sym->name );
            SetDiagPop();
        }
        sym_handle = SymAddL0( sym->info.hash_value, sym );
    } else {
        SymGet( &old_sym, old_sym_handle );
        SetDiagSymbol( &old_sym, old_sym_handle );
        if( (old_sym.flags & SYM_FUNCTION) == 0 ) {
            CErr2p( ERR_SYM_ALREADY_DEFINED_AS_VAR, sym->name );
            //02-jun-89 sym_handle = old_sym_handle;                /* 05-apr-89 */
            sym_handle = SymAddL0( sym->info.hash_value, sym );     /* 02-jun-89 */
        } else {
            CmpFuncDecls( sym, &old_sym );
            PrevProtoType = old_sym.sym_type;               /* 12-may-91 */
            if( (old_sym.flags & SYM_DEFINED) == 0 ) {
                if( sym->sym_type->u.fn.parms != NULL ||    /* 11-jul-89 */
                   ( CurToken != T_COMMA &&                 /* 18-jul-89 */
                    CurToken != T_SEMI_COLON ) ) {
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
            if( stg_class == SC_NULL && old_sym.attribs.stg_class != SC_FORWARD ) {     /* 05-jul-89 */
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
        } else if( stg_class == SC_NULL ) {
            stg_class = SC_EXTERN;  /* SC_FORWARD; */
        }
        sym->attribs.stg_class = stg_class;
    }
    return( sym_handle );
}

local SYM_HANDLE VarDecl( SYMPTR sym, stg_classes stg_class, decl_state *state )
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
    if( (stg_class == SC_NULL) && (*state & DECL_STATE_NOTYPE) && !(*state & DECL_STATE_NOSTWRN) ) {
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
    if( sym->attribs.naked ) {         /* 25-jul-95 */
        CErr1( ERR_INVALID_DECLSPEC );
    }

    if( SymLevel == 0 ) {
        /*
        //  SymLevel == 0 is global scope (SymLevel is the count of nested {'s)
        */
        if( (stg_class == SC_AUTO) || (stg_class == SC_REGISTER) ) {
            CErr1( ERR_INV_STG_CLASS_FOR_GLOBAL );
            stg_class = SC_STATIC;
        } else if( stg_class == SC_NULL ) {
            CompFlags.external_defn_found = 1;
        }
        if( sym->attribs.declspec == DECLSPEC_THREAD ) {          /* 25-jul-95 */
            if( !CompFlags.thread_data_present ) {
                ThreadSeg = DefThreadSeg();
                CompFlags.thread_data_present = 1;
            }
            sym->u.var.segment = ThreadSeg;
        }
    } else {
        /*
        //  SymLevel != 0 is function scoped (SymLevel is the count of nested {'s)
        */
        if( stg_class == SC_NULL ) {
            stg_class = SC_AUTO;
        }
        if( stg_class == SC_AUTO  ||  stg_class == SC_REGISTER ) {
            if( sym->mods & MASK_LANGUAGES ) {
                CErr1( ERR_INVALID_DECLARATOR );
            }
            if( sym->attribs.declspec != DECLSPEC_NONE ) {          /* 25-jul-95 */
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
        if( (stg_class == SC_STATIC) && (sym->attribs.declspec == DECLSPEC_THREAD) ) {          /* 06-JAN-03 */
            if( !CompFlags.thread_data_present ) {
                ThreadSeg = DefThreadSeg();
                CompFlags.thread_data_present = 1;
            }
            sym->u.var.segment = ThreadSeg;
        }
    }
    if( (Toggles & TOGGLE_UNREFERENCED) == 0 ) {
        sym->flags |= SYM_IGNORE_UNREFERENCE;   /* 25-apr-91 */
    }
    old_sym_handle = SymLook( sym->info.hash_value, sym->name );
    if( old_sym_handle != 0 ) {                         /* 28-feb-94 */
        SymGet( &old_sym, old_sym_handle );
        if( old_sym.level == SymLevel ) {
            SetDiagSymbol( &old_sym, old_sym_handle );
            if( old_sym.attribs.stg_class == SC_EXTERN  &&  stg_class == SC_EXTERN ) {
                if( ! IdenticalType( old_sym.sym_type, sym->sym_type ) ) {
                    CErr2p( ERR_TYPE_DOES_NOT_AGREE, sym->name );
                }
            } else if( old_sym.attribs.stg_class == SC_TYPEDEF ) {
                CErr2p( ERR_SYM_ALREADY_DEFINED, sym->name );
            }
            SetDiagPop();
        }
    }
    if( stg_class == SC_EXTERN ) {              /* 27-oct-88 */
        old_sym_handle = Sym0Look( sym->info.hash_value, sym->name );
    }
    if( old_sym_handle != 0 ) {
        SymGet( &old_sym, old_sym_handle );
        SetDiagSymbol( &old_sym, old_sym_handle );
        if( old_sym.level == SymLevel           /* 28-mar-88 */
        ||      stg_class == SC_EXTERN ) {              /* 12-dec-88 */
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
    if( (old_sym_handle != 0)  &&
        (stg_class == SC_NULL || stg_class == SC_EXTERN ||
          (stg_class == SC_STATIC && SymLevel == 0)) ) {

        /* make sure sym->sym_type same type as old_sym->sym_type */

        SetDiagSymbol( &old_sym, old_sym_handle );
        old_def = VerifyType( sym->sym_type, old_sym.sym_type, sym );
        SetDiagPop();
        if( !old_def && old_sym.level == SymLevel ) { /* 06-jul-88 AFS */
            /* new symbol's type supersedes old type */
            old_sym.sym_type = sym->sym_type;
            if( (old_sym.flags & SYM_FUNCTION) ) {
                old_sym = *sym;  // ditch old sym
            }
            SymReplace( &old_sym, old_sym_handle );
        }
        if( stg_class == SC_EXTERN  &&  SymLevel != 0 ) goto new_var;
        CMemFree( sym->name );
        memcpy( sym, &old_sym, sizeof( SYM_ENTRY ) );
        sym_handle = old_sym_handle;
        SetDiagSymbol( &old_sym, old_sym_handle );
        /* verify that newly specified storage class doesn't conflict */
        if( (stg_class == SC_NULL) || (stg_class == SC_STATIC) ) {
            if( sym->attribs.stg_class == SC_EXTERN ) {
                /* was extern, OK to change to none */
                if( stg_class == SC_NULL ) {
                    sym->attribs.stg_class = stg_class;     /* 03-oct-88 */
                } else {
                    /* was extern, not OK to make static */
                    CErrSymName( ERR_STG_CLASS_DISAGREES, sym, sym_handle );
                }
            } else if( sym->attribs.stg_class == SC_STATIC && stg_class == SC_NULL ) {
                /* was static, not OK to redefine */
                CErrSymName( ERR_STG_CLASS_DISAGREES, sym, sym_handle );
            } else if( sym->attribs.stg_class == SC_NULL && stg_class == SC_STATIC ) {
                /* was extern linkage, not OK to to make static */
                CErrSymName( ERR_STG_CLASS_DISAGREES, sym, sym_handle );
            }
        }
        SetDiagPop();
    } else {
        if( stg_class == SC_EXTERN  &&  SymLevel != 0 ) {
            ; /* do nothing  29-jan-93 */
        } else {
            VfyNewSym( sym->info.hash_value, sym->name );
        }
new_var:
        old_sym_handle = 0;
        sym->flags |= SYM_DEFINED;
        typ = sym->sym_type;
        SKIP_DUMMY_TYPEDEFS( typ );
        if( typ->decl_type == TYPE_TYPEDEF ) {          /* 12-mar-92 */
            SymGet( &sym2, typ->u.typedefn );
            if( sym->u.var.segment == 0  &&  sym2.u.var.segment != 0 ) {
                sym->u.var.segment = sym2.u.var.segment;
            }
            SKIP_TYPEDEFS( typ );
        }
        if( typ->decl_type == TYPE_VOID ) {
            CErr2p( ERR_VAR_CANT_BE_VOID, sym->name );
            sym->sym_type = TypeDefault();
        }
        sym->attribs.stg_class = stg_class;
        sym_handle = SymAdd( sym->info.hash_value, sym );
    }
    if( sym->u.var.segment == 0  &&     /* 22-oct-92 */
     (stg_class == SC_STATIC ||
      stg_class == SC_NULL   ||
      stg_class == SC_EXTERN) ) {
        if( DefDataSegment != 0 ) {
            sym->u.var.segment = DefDataSegment;
            SymReplace( sym, sym_handle );
        }
    }
    if( CurToken == T_EQUAL ) {
        if( stg_class == SC_EXTERN ) {
            stg_class = SC_STATIC;
            if( SymLevel == 0 ) {
                CompFlags.external_defn_found = 1;
                stg_class = SC_NULL;
            } else {
                CErr1( ERR_CANT_INITIALIZE_EXTERN_VAR );
            }
        }
        sym->attribs.stg_class = stg_class;
        NextToken();
        VarDeclEquals( sym, sym_handle );
        sym->flags |=  SYM_ASSIGNED;
    }
    SymReplace( sym, sym_handle );                      /* 06-jul-88 */
    if( old_sym_handle != 0 ) {
        sym_handle = 0;
    }
    return( sym_handle );
}

local void AdjSymTypeNode( SYMPTR sym, type_modifiers decl_mod )
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
    memset( sym, 0, sizeof( SYM_ENTRY ) );              /* 02-apr-91 */
    sym->name = "";
    flags = TypeQualifier();                            /* 08-nov-94 */
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
        return( 0 );
    }
    typ = sym->sym_type;
    /* skip over typedef's  29-aug-89 */
    SKIP_TYPEDEFS( typ );
    if( info->stg == SC_TYPEDEF ) {
        if( CompFlags.extensions_enabled ) {            /* 24-mar-91 */
            old_sym_handle = SymLook( sym->info.hash_value, sym->name );
            if( old_sym_handle != 0 ) {
                SymGet( &old_sym, old_sym_handle );
                otyp = old_sym.sym_type;        /* skip typedefs 25-sep-92 */
                SKIP_TYPEDEFS( otyp );
                if( old_sym.attribs.stg_class == SC_TYPEDEF &&
                    old_sym.level == SymLevel &&
                    IdenticalType( typ, otyp ) ) {
                    return( 0 );        /* indicate already in symbol tab */
                }
            }
        }
        VfyNewSym( sym->info.hash_value, sym->name );
        sym->attribs.stg_class = info->stg;
        AdjSymTypeNode( sym, info->decl_mod );
        sym_handle = SymAdd( sym->info.hash_value, sym );
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

int DeclList( SYM_HANDLE *sym_head )
{
    decl_state          state;
    SYM_HANDLE          sym_handle;
    SYM_HANDLE          prevsym_handle;
    auto SYM_ENTRY      sym;
    auto SYM_ENTRY      prevsym;
    decl_info           info;

    ParmList = NULL;
    prevsym_handle = 0;
    *sym_head = 0;
    for( ;; ) {
        for( ;; ) {
            for( ;; ) {
                while( CurToken == T_SEMI_COLON ) {
                    if( SymLevel == 0 ) {
                        if( !CompFlags.extensions_enabled ) {  /* 28-nov-94 */
                            CErr2p( ERR_EXPECTING_DECL_BUT_FOUND, ";" );
                        }
                    }
                    NextToken();
                }
                if( CurToken == T_EOF ) return( 0 );
                FullDeclSpecifier( &info );
                if( info.stg != SC_NULL  ||  info.typ != NULL ) break;
                if( SymLevel != 0 ) return( 0 );
                break;
            }
            state = DECL_STATE_NONE;
            if( info.typ == NULL ) {
                state |= DECL_STATE_NOTYPE;
                info.typ = TypeDefault();
            }
            if( info.stg == SC_NULL  && (state & DECL_STATE_NOTYPE) ) {
                if( TokenClass[ CurToken ] == TC_MODIFIER ) {
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
                    if( info.stg == SC_NULL ) {
                        info.stg = SC_EXTERN;
                    }
                }
            }
            for( ;; ) {
                sym_handle = InitDeclarator( &sym, &info, &state );
                /* NULL is returned if sym already exists in symbol table */
                if( sym_handle != 0 ) {
                    sym.handle = 0;
                    if( sym.flags & SYM_FUNCTION ) {
                        if( !(state & DECL_STATE_NOTYPE ) ) {
                            sym.flags |= SYM_TYPE_GIVEN;
                        }
                    } else if( SymLevel > 0 ) { /* variable */
                        if( prevsym_handle != 0 ) {
                            SymGet( &prevsym, prevsym_handle );
                            prevsym.handle = sym_handle;
                            SymReplace( &prevsym, prevsym_handle );
                        }
                        if( *sym_head == 0 ) {
                            *sym_head = sym_handle;
                        }
                        prevsym_handle = sym_handle;
                    }
                    SymReplace( &sym, sym_handle );
                }
                /* case "int x *p" ==> missing ',' msg already given */
                if( CurToken != T_TIMES ) {
                    if( CurToken != T_COMMA ) break;
                    NextToken();
                }
            }
/*              the following is illegal:
                    typedef double math(double);
                    math sin { ; }
            That's the reason for the check
                    "typ->decl_type != TYPE_FUNCTION"
*/
            if( SymLevel == 0  &&  CurToken != T_SEMI_COLON
                               &&  sym_handle != 0 ) {
                if( sym.sym_type->decl_type == TYPE_FUNCTION
                    &&  sym.sym_type != info.typ ) { /* 21-mar-89 */
                    CurFuncHandle = sym_handle;
                    CurFunc = &CurFuncSym;
                    memcpy( CurFunc, &sym, sizeof( SYM_ENTRY ) );
                    return( 1 );        /* indicate this is a function defn */
                }
            }
            MustRecog( T_SEMI_COLON );
        } else {
            Chk_Struct_Union_Enum( info.typ );
            NextToken();                /* skip over ';' */
        }
    }
// can't get here!      return( 0 );
}


int LoopDecl( SYM_HANDLE *sym_head )
{
    decl_state          state;
    SYM_HANDLE          sym_handle;
    SYM_HANDLE          prevsym_handle;
    SYM_ENTRY           sym;
    SYM_ENTRY           prevsym;
    decl_info           info;

    ParmList = NULL;
    prevsym_handle = 0;
    *sym_head = 0;
    if( CurToken == T_EOF ) return( 0 );
    FullDeclSpecifier( &info );
    if( info.stg == SC_NULL  &&  info.typ == NULL ) {
        return( 0 );    /* No declaration-specifiers, get outta here */
    }
    if( info.stg != SC_NULL && info.stg != SC_AUTO && info.stg != SC_REGISTER ) {
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
            if( sym_handle != 0 ) {
                sym.handle = 0;
                if( sym.flags & SYM_FUNCTION ) {
                    if( !(state & DECL_STATE_NOTYPE ) ) {
                        sym.flags |= SYM_TYPE_GIVEN;
                    }
                } else {    /* variable */
                    if( prevsym_handle != 0 ) {
                        SymGet( &prevsym, prevsym_handle );
                        prevsym.handle = sym_handle;
                        SymReplace( &prevsym, prevsym_handle );
                    }
                    if( *sym_head == 0 ) {
                        *sym_head = sym_handle;
                    }
                    prevsym_handle = sym_handle;
                }
                SymReplace( &sym, sym_handle );
            }
            /* case "int x *p" ==> missing ',' msg already given */
            if( CurToken != T_TIMES ) {
                if( CurToken != T_COMMA ) break;
                NextToken();
            }
        }
        MustRecog( T_SEMI_COLON );
    } else {
        //  Chk_Struct_Union_Enum( info.typ );
        NextToken();                /* skip over ';' */
    }
    return( 1 );    /* We found a declaration */
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


local type_modifiers GetModifiers( void )
{
    type_modifiers      modifier;

    modifier = 0;
    for( ; TokenClass[ CurToken ] == TC_MODIFIER; ) {
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
    segment_id       segment;
    type_modifiers   modifier;  // const, vol flags
    BASED_KIND       based_kind;
    SYM_HANDLE       based_sym;
};

local TYPEPTR Pointer( TYPEPTR typ, struct mod_info *info )
{
    type_modifiers  flags;
    SYM_HANDLE      sym_handle;
    SYM_ENTRY       sym;

    sym_handle = 0;
    if( (typ != NULL) && (typ->decl_type == TYPE_TYPEDEF) ) {
        // get segment from typedef TODO should be done sooner
        TYPEPTR     ptr_typ;
        SYMPTR      symp;

        ptr_typ = typ;
        SKIP_DUMMY_TYPEDEFS( ptr_typ );
        if( ptr_typ->decl_type == TYPE_TYPEDEF ) {
            symp = SymGetPtr( ptr_typ->u.typedefn );
            if( info->modifier & FLAG_BASED ) {
                info->segment = symp->u.var.segment;
                sym_handle = SegSymHandle( info->segment );
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
                if( sym_handle == 0 ) {
                    SymCreate( &sym, Buffer );
                    sym.attribs.stg_class = SC_EXTERN;  /* indicate extern decl */
                    CErr2p( ERR_UNDECLARED_SYM, Buffer );
                    sym.sym_type = GetType( TYPE_INT );
                    sym_handle = SymAdd( HashValue, &sym );
                } else {
                    TYPEPTR     ptr_typ;

                    SymGet( &sym, sym_handle );
                    ptr_typ = sym.sym_type;
                    SKIP_TYPEDEFS( ptr_typ );
                    if( use_seg ) {
                        info->based_kind = BASED_VARSEG;
                        if( ptr_typ->decl_type != TYPE_POINTER ) {
                           CErr1( ERR_SYM_MUST_BE_TYPE_SEGMENT );
                           info->based_kind = BASED_NONE;
                       }
                    } else if( ptr_typ->decl_type == TYPE_POINTER ) {
                        info->based_kind = BASED_VAR;
                    } else if( sym.mods & FLAG_SEGMENT ) {
                        info->based_kind = BASED_SEGVAR;
                    } else {
                        CErr1( ERR_SYM_MUST_BE_TYPE_SEGMENT );
                        info->based_kind = BASED_NONE;
                    }
                }
                if( !(sym.flags & SYM_REFERENCED) ) {
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
                    info->segment = AddSegName( Buffer, "", SEGTYPE_BASED);
                    sym_handle = SegSymHandle( info->segment );
                    SymGet( &sym, sym_handle );
                    sym.u.var.segment = info->segment;
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
                    if( sym_handle == 0 ) {
                        SymCreate( &sym, Buffer );
                        sym.attribs.stg_class = SC_EXTERN;  /* indicate extern decl */
                        CErr2p( ERR_UNDECLARED_SYM, Buffer );
                        sym.sym_type = GetType( TYPE_INT );
                        sym_handle = SymAdd( HashValue, &sym );
                    } else {
                        SymGet( &sym, sym_handle );
                    }
                    if( !(sym.flags & SYM_REFERENCED) ) {
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
            typ = BPtrNode( typ, flags, info->segment, sym_handle, info->based_kind );
            sym_handle = 0;
            info->segment = 0;  // start over
            info->modifier = (flags & FLAG_INLINE) | TypeQualifier();  // .. * const
            info->based_kind = BASED_NONE;
        } else {
            break;
        }
    }
    return( typ );
}


local void ParseDeclPart2( TYPEPTR *typep, TYPEPTR typ, type_modifiers mod )
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

    info.segment = 0;
    info.modifier = mod;
    info.based_kind = BASED_NONE;
    info.based_sym = 0;
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
        sym->u.var.segment = info.segment;              /* 01-dec-91 */
        typ = DeclPart2( typ, info.modifier );
        sym->sym_type = typ;
    }
}

void Declarator( SYMPTR sym, type_modifiers mod, TYPEPTR typ, decl_state state )
{
    TYPEPTR             *type_list;
    TYPEPTR             parm_type;
    struct mod_info     info;

    info.segment = 0;
    info.modifier = mod;
    info.based_kind = BASED_NONE;
    info.based_sym = 0;
    typ = Pointer( typ, &info );
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        if( state & DECL_STATE_ISPARM ) {
            parm_type = TypeName();                     /* 14-mar-91 */
        } else {
            parm_type = NULL;
        }
        if( parm_type != NULL  ||  CurToken == T_RIGHT_PAREN ) {
            type_list = NULL;
            if( parm_type != NULL ) {
                type_list = MakeParmList( NewParm( parm_type, NULL ), 1, 0 );
            }
            typ = FuncNode( typ, info.modifier, type_list );
            typ = PtrNode( typ, FLAG_NONE, SEG_DATA );
            MustRecog( T_RIGHT_PAREN );
        } else {
            if( (state & DECL_STATE_ISPARM) && TokenClass[ CurToken ] == TC_STG_CLASS ) {
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
                    sym->info.hash_value = HashValue;
                    NextToken();
                } else {
                    SymCreate( sym, SavedId );
                    sym->info.hash_value = SavedHash;
                    CurToken = LAToken;
                }
                if( CurToken != T_ID && CurToken != T_TIMES ) break;
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
                if( CurToken != T_ID ) break;
                CMemFree( sym->name );
            }
        } else {
            SymCreate( sym, "" );
        }
        sym->mods = info.modifier;
        sym->u.var.segment = info.segment;              /* 01-dec-91 */
#if 0
        if( modifier & FLAG_INTERRUPT )  sym->flags |= SYM_INTERRUPT_FN;
#endif
        typ = DeclPart2( typ, info.modifier );
        sym->sym_type = typ;
    }
    if( typ != NULL ) {
        if( typ->decl_type == TYPE_FUNCTION ) {         /* 07-jun-94 */
            if( state & DECL_STATE_FORLOOP ) {
                CErr2p( ERR_DECL_IN_LOOP_NOT_OBJECT, sym->name );
            } else if( info.segment != 0 ) {            // __based( __segname("X"))
                SetFuncSegment( sym, info.segment );
            }
        }
    }
}


FIELDPTR FieldCreate( const char *name )
{
    FIELDPTR    field;
    size_t      len;

    len = strlen( name );
    field = (FIELDPTR)CPermAlloc( sizeof( FIELD_ENTRY ) + len );
    memcpy( field->name, name, len + 1 );
    if( CompFlags.emit_browser_info ) {
        field->xref = NewXref( NULL );
    }
    return( field );
}


FIELDPTR FieldDecl( TYPEPTR typ, type_modifiers mod, decl_state state )
{
    FIELDPTR            field;
    struct mod_info     info;

    info.segment = 0;
    info.modifier = mod;
    info.based_kind = BASED_NONE;
    info.based_sym = 0;
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
                if( CurToken != T_ID && CurToken != T_TIMES ) break;
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
                if( CurToken != T_ID ) break;
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


local TYPEPTR ArrayDecl( TYPEPTR typ )
{
    int         dimension;
    TYPEPTR     first_node, next_node, prev_node;

    if( typ != NULL ) {                                 /* 16-mar-90 */
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
                if( (val.type == TYPE_ULONG64) && !U64IsI32( val.value ) ) {
                    CErr1( ERR_CONSTANT_TOO_BIG );
                } else if( (val.type == TYPE_LONG64) && !I64IsI32( val.value ) ) {
                    CErr1( ERR_CONSTANT_TOO_BIG );
                }
                dimension = I32FetchTrunc( val.value );
            } else {
                dimension = 1;
            }
            if( dimension <= 0 ) {
                CErr1( ERR_INVALID_DIMENSION );
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


local TYPEPTR  *FuncProtoType( void );

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
        if( parms_list != NULL )  {
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
        if( !(mod & FLAG_INTERRUPT) ) {
            CWarn1( WARN_OBSOLETE_FUNC_DECL, ERR_OBSOLETE_FUNC_DECL );
        }
    }
    if( typ != NULL ) {                                 /* 09-apr-90 */
        TYPEPTR     typ2;

        typ2 = typ;                                     /* 08-dec-93 */
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
        if( CurToken != T_LEFT_PAREN ) break;
        NextToken();
        typ = DeclPart3( typ, mod );
    }
    return( typ );
}


local void CheckUniqueName( PARMPTR parm, char *name )  /* 13-apr-89 */
{
    if( name != NULL ) {                                /* 29-oct-91 */
        if( *name != '\0' ) {
            for( ; parm ; parm = parm->next_parm ) {
                if( parm->sym.name != NULL ) {          /* 16-oct-92 */
                    if( strcmp( parm->sym.name, name ) == 0 ) {
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


local TYPEPTR *GetProtoType( decl_info *first )
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
//        declspec  = info.decl;
        typ = info.typ;
        mod = info.mod;
        if( stg_class != SC_NULL  &&  stg_class != SC_REGISTER ) {
            CErr1( ERR_INVALID_STG_CLASS_FOR_PARM_PROTO );
        }
        state = DECL_STATE_ISPARM;
        if( typ == NULL ) {
            state |= DECL_STATE_NOTYPE;
            if( stg_class == SC_NULL ) {                /* 30-nov-94 */
                CErr1( ERR_TYPE_REQUIRED_IN_PARM_LIST );
            }
            typ = TypeDefault();
        }
        parm = (PARMPTR) CMemAlloc( sizeof( PARM_ENTRY ) );
        parm->next_parm = NULL;
        sym = &parm->sym;
        memset( sym, 0, sizeof( SYM_ENTRY ) );              /* 02-apr-91 */
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
        if( stg_class == SC_NULL ) {
            stg_class = SCSpecifier();                  /* 17-mar-91 */
            if( stg_class == SC_NULL ) {
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
        if( CurToken == T_RIGHT_PAREN ) break;
        if( (CurToken == T_EOF) || (CurToken == T_LEFT_BRACE) )  break;
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
    return( MakeParmList( parmlist, parm_count, 0 ) );
}


TYPEPTR *MakeParmList( parm_list *parm, int parm_count, int reversed )
{
    TYPEPTR             *type_list;
    parm_list           *next_parm;
    parm_list           *prev_parm;
    TYPEPTR             typ;
    int                 index;

    type_list = NULL;
    if( parm != NULL ) {
        if( ! reversed ) {
            prev_parm = NULL;
            for( ;; ) {
                next_parm = parm->next_parm;
                parm->next_parm = prev_parm;
                if( next_parm == NULL ) break;
                prev_parm = parm;
                parm = next_parm;
            }
        }
        parm_count = 0;
        next_parm = parm;
        while( next_parm != NULL ) {
            ++parm_count;
            next_parm = next_parm->next_parm;
        }

        /* try to find an existing parm list that matches, 29-dec-88 */
        index = parm_count;
        if( index > MAX_PARM_LIST_HASH_SIZE ) {
            index = MAX_PARM_LIST_HASH_SIZE;
        }
        for( typ = FuncTypeHead[ index ]; typ; typ = typ->next_type ) {
            type_list = typ->u.fn.parms;
            next_parm = parm;
            for( ;; ) {
                if( next_parm == NULL ) {
                    if( *type_list != NULL ) break;
                    while( parm != NULL ) {
                        next_parm = parm->next_parm;
                        CMemFree( parm );
                        parm = next_parm;
                    }
                    return( typ->u.fn.parms );
                }
                if( next_parm->parm_type != *type_list ) break;
                next_parm = next_parm->next_parm;
                ++type_list;
            }
        }

        type_list = (TYPEPTR *)CPermAlloc( (parm_count + 1) * sizeof( TYPEPTR ) );
        if( type_list != NULL ) {
            type_list[ parm_count ] = NULL;
            parm_count = 0;
            while( parm != NULL ) {
                type_list[ parm_count ] = parm->parm_type;
                next_parm = parm->next_parm;
                CMemFree( parm );
                parm = next_parm;
                ++parm_count;
            }
        }
    }
    return( type_list );
}

local int VoidType( TYPEPTR typ )                       /* 03-oct-91 */
{
    if( typ != NULL ) {
        SKIP_TYPEDEFS( typ );
        if( typ->decl_type == TYPE_VOID ) {
            return( 1 );
        }
    }
    return( 0 );
}

local TYPEPTR *FuncProtoType( void )
{
    TYPEPTR             *type_list;
    TAGPTR              old_taghead;
    decl_info           info;

    if( !CompFlags.extensions_enabled ) {
        ++SymLevel; /* 03-may-89 */
    }
    old_taghead = TagHead;
    FullDeclSpecifier( &info );
    if( (info.stg == SC_NULL) && (info.typ == NULL) ) {
        GetFuncParmList();
        if( ExpectingToken( T_RIGHT_PAREN ) ) {
            NextToken();
        }
        type_list = NULL;
    } else {    /* function prototype present */
        if( VoidType( info.typ )  &&  /*27-dec-88*/
            info.stg== SC_NULL  &&  CurToken == T_RIGHT_PAREN ) {
            type_list = VoidParmList;
        } else {
            type_list = GetProtoType( &info );
        }
        if( ExpectingToken( T_RIGHT_PAREN ) ) {
            NextToken();
        }
        if( CurToken != T_LEFT_BRACE ) {                /* 18-jan-89 */
            if( SymLevel > 1  ||                        /* 03-dec-90 */
            ! CompFlags.extensions_enabled ) {  /* 25-jul-91 */
                /* get rid of any new tags regardless of SymLevel;  23-jul-90 */
                TagHead = old_taghead;  /* get rid of new tags from proto */
                FreeEnums();                    /* 03-may-89 */
            }
        }
    }
    if( !CompFlags.extensions_enabled ) {
        --SymLevel; /* 03-may-89 */
    }
    return( type_list );
}


local void GetFuncParmList( void )
{
    PARMPTR     parm = NULL;
    PARMPTR     newparm;
    PARMPTR     parm_namelist;

    parm_namelist = NULL;
    while( CurToken == T_ID ) { /* scan off func parm list */
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
        parm->sym.info.hash_value = HashValue;
        if( (Toggles & TOGGLE_UNREFERENCED) == 0 ) {
            parm->sym.flags |= SYM_REFERENCED;
        }
        NextToken();
        if( CurToken == T_RIGHT_PAREN ) break;
        if( CurToken == T_EOF ) break;
        if( CurToken != T_COMMA ) {     /* 04-jan-89 */
            MustRecog( T_COMMA );               /* forces error msg */
            for( ;; ) {     /* skip until ')' to avoid cascading errors */
                if( CurToken == T_RIGHT_PAREN ) break;
                if( CurToken == T_EOF ) break;
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


local void FreeParmList( void )
{
    PARMPTR     parm;

    for( ; (parm = ParmList) != NULL; ) {
        ParmList = parm->next_parm;
        CMemFree( parm->sym.name );
        CMemFree( parm );
    }
}

#if _CPU == 370
local bool IsIntComp( TYPEPTR ret1 )
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
    case  TYPE_LONG:
        ret = TRUE;
        break;
    default:
       ret = FALSE;
    }
    return( ret );
}
#endif
