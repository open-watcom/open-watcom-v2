/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Parse functions, prototypes, handle argument promotion.
*
****************************************************************************/


#include "cvars.h"
#include "cgswitch.h"
#include "langenv.h"
#include "cfeinfo.h"
#include "toggles.h"


static   void    ParmDeclList( void );
static   void    AddParms( void );
static   void    ChkParms( void );
static   void    FuncDefn( SYMPTR );

static   void    BeginFunc( void );
static   void    ReverseParms( void );       /* reverse order of parms */


void ParsePgm( void )
{
    SYM_HANDLE      dummysym;

    CompFlags.external_defn_found = false;
    CompFlags.initializing_data = false;
    dummysym = SYM_NULL;
    GlobalSym = SYM_NULL;

    do {
        if( DeclList( &dummysym ) ) {  /* if this is a function defn */
            FuncDefn( CurFunc );
            SrcLoc = CurFunc->src_loc;
            GenFunctionNode( CurFuncHandle );

            SymLevel = 1;
            ParmDeclList();
            SymLevel = 0;
            if( CurToken == T_LEFT_BRACE ) {
                BeginFunc();
                Statement();
                CMemFree( CurFunc->name );
                CurFunc->name = NULL;
                SymReplace( CurFunc, CurFuncHandle );
                CurFunc = NULL;
                CurFuncNode = NULL;
                CurFuncHandle = SYM_NULL;
            } else {
                MustRecog( T_LEFT_BRACE );
            }
        }
    } while( CurToken != T_EOF );

    if( !CompFlags.external_defn_found ) {
        if( !CompFlags.extensions_enabled ) {
            CErr1( ERR_NO_EXTERNAL_DEFNS_FOUND );
        }
    }
}


static void FuncDefn( SYMPTR sym )
{
    SYM_NAMEPTR sym_name;
    TYPEPTR     typ;

    /* duplicate name in near space */
    sym_name = SymName( sym, CurFuncHandle );
    sym->name = CStrSave( sym_name );
    if( sym->flags & SYM_DEFINED ) {
        CErr2p( ERR_SYM_ALREADY_DEFINED, sym->name );
    }
    typ = sym->sym_type->object;                /* get return type */
    SKIP_TYPEDEFS( typ );

    if( typ->decl_type != TYP_VOID ) {
        if( TypeSize( typ ) == 0 ) {
            CErr2p( ERR_INCOMPLETE_TYPE, sym_name );
        }
    }
    sym->flags |= SYM_DEFINED /* | SYM_REFERENCED */;

    if( (GenSwitches & NO_OPTIMIZATION) == 0 ) {
        sym->flags |= SYM_OK_TO_RECURSE;
    }

    if( sym->attribs.stg_class == SC_EXTERN || sym->attribs.stg_class == SC_FORWARD ) {
        sym->attribs.stg_class = SC_NONE;       /* indicate exported function */
    }

    CompFlags.external_defn_found = true;
    if( TOGGLE( check_stack ) )
        sym->flags |= SYM_CHECK_STACK;

    if( !CompFlags.zu_switch_used ) {
        if( sym->mods & FLAG_FARSS ) {          /* function use far stack */
            TargetSwitches |= FLOATING_SS;
        } else {
            TargetSwitches &= ~FLOATING_SS;
        }
    }
    if( strcmp( CurFunc->name, "main" ) == 0
      || strcmp( CurFunc->name, "wmain" ) == 0 ) {
        sym->mods &= ~MASK_LANGUAGES;  // Turn off any language flags
        sym->mods |= LANG_WATCALL;     // Turn on __watcall calling convention for main
    }
    SymReplace( sym, CurFuncHandle );
}

#define NAMELIT(c) c, sizeof( c )

#define MAIN_NAMES \
    pick( MAIN_WMAIN,    "wmain" ), \
    pick( MAIN_MAIN,     "main" ), \
    pick( MAIN_WWINMAIN, "wWinMain" ), \
    pick( MAIN_WINMAIN,  "WinMain" ), \
    pick( MAIN_WLIBMAIN, "wLibMain" ), \
    pick( MAIN_LIBMAIN,  "LibMain" ), \
    pick( MAIN_WDLLMAIN, "wDllMain" ), \
    pick( MAIN_DLLMAIN,  "DllMain" ),

enum main_names {
    #define pick(e,n) e
    MAIN_NAMES
    #undef pick
    MAIN_NUM,
};

static struct {
    char const      *name;
    unsigned char   len;
} MainNames[] = {
    #define pick(e,n) { NAMELIT( n ) }
    MAIN_NAMES
    #undef pick
};

static enum main_names checkMain( const char *name )
{
    enum main_names     main_entry;

    for( main_entry = MAIN_WMAIN; main_entry < MAIN_NUM; ++main_entry ) {
       if( strcmp( name, MainNames[main_entry].name ) == 0 ) {
           break;
       }
    }
    return( main_entry );
}

static void BeginFunc( void )
{
    char                *name;
    char                *segname;

    if( CurFunc->seginfo == NULL ) {
        CurFunc->seginfo = DefCodeSegment;
        if( CurFunc->seginfo == NULL ) {
            if( CompFlags.zm_switch_used ) {
                name = "";
                if( TargetSwitches & BIG_CODE )
                    name = CurFunc->name;
                segname = TS_SEG_CODE; /* "_TEXT" */
                if( TextSegName[0] != '\0' ) {
                    segname = TextSegName;
                }
                CurFunc->seginfo = NewTextSeg( name, segname, "" );
            }
        }
    }
    switch( checkMain( CurFunc->name ) ) {
    case MAIN_WMAIN:
        CompFlags.has_wchar_entry = true;
        // fall through!
    case MAIN_MAIN:
        if( CurFunc->u.func.parms != SYM_NULL ) {
            CompFlags.main_has_parms = true;
        } else {
            CompFlags.main_has_parms = false;
        }
        CompFlags.has_main = true;
        break;

    case MAIN_WWINMAIN:
        CompFlags.has_wchar_entry = true;
        // fall through!
    case MAIN_WINMAIN:
        if( TargSys == TS_WINDOWS || TargSys == TS_CHEAP_WINDOWS || TargSys == TS_NT ) {
            CompFlags.has_winmain = true;
        } else {
            CompFlags.has_wchar_entry = false;
        }
        break;

    case MAIN_WLIBMAIN:
    case MAIN_WDLLMAIN:
        CompFlags.has_wchar_entry = true;
        // fall through!
    case MAIN_LIBMAIN:
    case MAIN_DLLMAIN:
        CompFlags.has_libmain = true;
        break;

    case MAIN_NUM:
        break;
    }
}

bool CheckFuncMain( const char *name )
{
    switch( checkMain( name ) ) {
    case MAIN_MAIN:
        return( true );
    case MAIN_WMAIN:
    case MAIN_WWINMAIN:
    case MAIN_WINMAIN:
    case MAIN_WLIBMAIN:
    case MAIN_WDLLMAIN:
    case MAIN_LIBMAIN:
    case MAIN_DLLMAIN:
    case MAIN_NUM:
    default:
        return( false );
    }
}

static void  ArgPromotion( SYMPTR sym )
{
    TYPEPTR     typ;
    TYPEPTR     arg_typ;

    AdjParmType( sym );
    arg_typ = sym->sym_type;
    /* perform default argument promotions */
    typ = arg_typ;
    SKIP_TYPEDEFS( typ );

    switch( typ->decl_type ) {
#if 0
    case TYP_CHAR:
    case TYP_UCHAR:
    case TYP_SHORT:
#endif
    case TYP_ENUM:
        arg_typ = GetType( TYP_INT );
        break;

#if 0
    case TYP_USHORT:
        arg_typ = GetType( TYP_UINT );
        break;

    case TYP_FLOAT:
        arg_typ = GetType( TYP_DOUBLE );
        break;
#endif
    default:
        break;
    }
}


static void ParmDeclList( void )     /* process old style function definitions */
{
    TYPEPTR             typ;
    PARMPTR             parm;
    decl_state          state;
    SYM_ENTRY           sym;
    decl_info           info;

    while( CurToken != T_LEFT_BRACE ) {
        FullDeclSpecifier( &info );
        if( info.stg == SC_NONE && info.typ == NULL ) {
            if( CurToken == T_ID ) {
                CErr2p( ERR_MISSING_DATA_TYPE, Buffer );
            }
        }
        if( info.stg != SC_NONE && info.stg != SC_REGISTER ) {
            CErr1( ERR_INVALID_STG_CLASS_FOR_PARM );
            info.stg = SC_NONE;
        }
        state = DECL_STATE_NONE;
        typ = info.typ;
        if( typ == NULL ) {
            state |= DECL_STATE_NOTYPE;
            typ = TypeDefault();
        }
        if( info.stg == SC_NONE )
            info.stg = SC_AUTO;

        for( ;; ) {
            if( CurToken == T_SEMI_COLON ) {
                Chk_Struct_Union_Enum( typ );
            } else {
                sym.name = NULL;
                Declarator( &sym, info.mod, typ, state );
                if( sym.name == NULL || sym.name[0] == '\0' ) {
                    InvDecl();
                } else {
                    for( parm = ParmList; parm != NULL; parm = parm->next_parm ) {
                        if( parm->sym.name != NULL ) {
                            if( strcmp( parm->sym.name, sym.name ) == 0 ) {
                                break;
                            }
                        }
                    }
                    if( parm == NULL ) {
                        CErr2p( ERR_SYM_NOT_IN_PARM_LIST, sym.name );
                    } else if( parm->sym.sym_type != NULL ) {
                        CErr2p( ERR_PARM_ALREADY_DECLARED, sym.name );
                    } else {
                        ArgPromotion( &sym );
                        parm->sym.sym_type = sym.sym_type;
                        parm->sym.attribs.stg_class = info.stg;
                    }
                }
                CMemFree( sym.name );
            }

            if( CurToken == T_SEMI_COLON ) {
                NextToken();
                break;
            }
            if( CurToken == T_LEFT_BRACE ) {
                CErr1( ERR_MISSING_SEMICOLON );
                break;
            }
            if( CurToken == T_EOF )
                return;
            MustRecog( T_COMMA );
        }
    }
    ReverseParms();
    if( CurFunc->sym_type->u.fn.parms == NULL ) {
        CurFunc->flags |= SYM_OLD_STYLE_FUNC;
        AddParms();
    } else {
        ChkParms();
    }
    ParmList = NULL;
    if( VarParm( CurFunc ) ) {
        CurFunc->flags &= ~ SYM_OK_TO_RECURSE;
    }
}


static void ReverseParms( void )       /* reverse order of parms */
{
    PARMPTR     prev_parm, parm, next_parm;

    if( ParmsToBeReversed( CurFunc->mods, NULL ) ) {
        prev_parm = NULL;
        for( parm = ParmList; parm != NULL; parm = next_parm ) {
            next_parm = parm->next_parm;
            parm->next_parm = prev_parm;
            prev_parm = parm;
        }
        ParmList = prev_parm;
    }
}


static void ChkProtoType( void )
{
    TYPEPTR     ret1;
    TYPEPTR     ret2;
    TYPEPTR     typ1;
    TYPEPTR     typ2;

    typ1 = CurFunc->sym_type;
    SKIP_TYPEDEFS( typ1 );
    ret1 = typ1->object;
    typ2 = PrevProtoType;
    SKIP_TYPEDEFS( typ2 );
    ret2 = typ2->object;
    typ1->object = NULL;
    typ2->object = NULL;
    VerifyType( CurFunc->sym_type, PrevProtoType, CurFunc );
    typ1->object = ret1;
    typ2->object = ret2;
}


static void AddParms( void )
{
    PARMPTR             parm;
    PARMPTR             prev_parm;
    SYM_HANDLE          sym_handle;
    SYM_HANDLE          prev_sym_handle;
    SYM_HANDLE          new_sym_handle;
    TYPEPTR             typ = NULL;
    int                 parm_count;
    id_hash_idx         hash;
    parm_list           *parmlist;
    SYM_ENTRY           new_sym;

    CurFunc->u.func.locals = SYM_NULL;
    CurFunc->u.func.parms = SYM_NULL;
    parmlist = NULL;
    prev_sym_handle = SYM_NULL;
    parm_count = 0;
    prev_parm = NULL;
    for( parm = ParmList; parm != NULL; parm = parm->next_parm ) {
        new_sym_handle = SYM_NULL;
        parm->sym.flags |= SYM_DEFINED | SYM_ASSIGNED;
        parm->sym.attribs.is_parm = true;
        hash = parm->sym.info.hash;
        if( parm->sym.name[0] == '\0' ) {
            /* no name ==> ... */
            parm->sym.sym_type = GetType( TYP_DOT_DOT_DOT );
            parm->sym.attribs.stg_class = SC_AUTO;
        } else if( parm->sym.sym_type == NULL ) {
            parm->sym.sym_type = TypeDefault();
            parm->sym.attribs.stg_class = SC_AUTO;
        } else {
            /*
             * go through ParmList again, looking for FLOAT parms
             * change the name to ".P" and duplicate the symbol with type
             * float and generate an assignment statement.
             */
            typ = parm->sym.sym_type;
            SKIP_TYPEDEFS( typ );

            switch( typ->decl_type ) {
            case TYP_CHAR:
            case TYP_UCHAR:
            case TYP_SHORT:
                if( CompFlags.strict_ANSI ) {
                    parm->sym.sym_type = GetType( TYP_INT );
                }
                break;

            case TYP_USHORT:
                if( CompFlags.strict_ANSI ) {
#if TARGET_SHORT == TARGET_INT
                    parm->sym.sym_type = GetType( TYP_UINT );
#else
                    parm->sym.sym_type = GetType( TYP_INT );
#endif
                }
                break;

            case TYP_FLOAT:
                memcpy( &new_sym, &parm->sym, sizeof( SYM_ENTRY ) );
                new_sym.handle = CurFunc->u.func.locals;
                new_sym_handle = SymAdd( hash, &new_sym );
                CurFunc->u.func.locals = new_sym_handle;
                SymReplace( &new_sym, new_sym_handle );
                parm->sym.name = ".P";
                parm->sym.flags |= SYM_REFERENCED;
                parm->sym.sym_type = GetType( TYP_DOUBLE );
                break;

            default:
                break;
            }
        }
        sym_handle = SymAdd( hash, &parm->sym );
        if( new_sym_handle != SYM_NULL ) {
            TREEPTR         tree;

            tree = ExprNode( VarLeaf( &new_sym, new_sym_handle ),
                 OPR_EQUALS, RValue( VarLeaf( &parm->sym, sym_handle ) ) );
            tree->op.u2.result_type = typ;
            tree->u.expr_type = typ;
            AddStmt( tree );
        }

        if( prev_parm == NULL ) {
            CurFunc->u.func.parms = sym_handle;
        } else {
            prev_parm->sym.handle = sym_handle;
            SymReplace( &prev_parm->sym, prev_sym_handle );
            CMemFree( prev_parm );
        }
        prev_parm = parm;
        prev_sym_handle = sym_handle;
        ++parm_count;
        parmlist = NewParm( parm->sym.sym_type, parmlist );
    }
    if( prev_parm != NULL ) {
        prev_parm->sym.handle = SYM_NULL;
        SymReplace( &prev_parm->sym, prev_sym_handle );
        CMemFree( prev_parm );
    }
    typ = CurFunc->sym_type;
    // TODO not following my scheme
    CurFunc->sym_type = FuncNode( typ->object, FLAG_NONE,
        MakeParmList( parmlist, ParmsToBeReversed( CurFunc->mods, NULL ) ) );

    if( PrevProtoType != NULL ) {
        ChkProtoType();
    }
}


static void ChkParms( void )
{
    PARMPTR             parm;
    PARMPTR             prev_parm;
    SYM_HANDLE          sym_handle;
    SYM_HANDLE          prev_sym_handle;
    TYPEPTR             typ;

    CurFunc->u.func.locals = SYM_NULL;
    CurFunc->u.func.parms = SYM_NULL;
    typ = *(CurFunc->sym_type->u.fn.parms);
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type != TYP_VOID ) {
        prev_sym_handle = SYM_NULL;
        prev_parm = NULL;
        for( parm = ParmList; parm != NULL; parm = parm->next_parm ) {
            if( parm->sym.name == NULL ) {
                parm->sym.name = ".J";
                parm->sym.flags |= SYM_REFERENCED;
            }
            if( parm->sym.name[0] == '\0' ) {
                parm->sym.name = ".I";
                InvDecl();
            }
            if( parm->sym.sym_type == NULL ) {
                parm->sym.sym_type = TypeDefault();
            }
            /* make sure name not already defined in this SymLevel */
            sym_handle = SymAdd( parm->sym.info.hash, &parm->sym );
            if( prev_parm == NULL ) {
                CurFunc->u.func.parms = sym_handle;
            } else {
                prev_parm->sym.handle = sym_handle;
                SymReplace( &prev_parm->sym, prev_sym_handle );
                CMemFree( prev_parm );
            }
            prev_parm = parm;
            prev_sym_handle = sym_handle;
            parm->sym.flags |= SYM_DEFINED | SYM_ASSIGNED;
            parm->sym.attribs.is_parm = true;
        }
        if( prev_parm != NULL ) {
#if _CPU == 370
            {
                SYM_ENTRY   var_parm;

                if( VarParm( CurFunc ) ) {
                    typ = ArrayNode( GetType( TYP_CHAR ) );
                    typ->u.array->dimension = 160;
                    sym_handle = GetNewSym( &var_parm, 'V', typ, SC_AUTO );
                    SymReplace( &var_parm, sym_handle );
                    prev_parm->sym.handle = sym_handle;
                }
            }
#endif
            SymReplace( &prev_parm->sym, prev_sym_handle );
            CMemFree( prev_parm );
        }
    }
}
