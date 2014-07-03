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
* Description:  Parse functions, prototypes, handle argument promotion.
*
****************************************************************************/


#include "cvars.h"
#include "cgswitch.h"
#include "langenv.h"
#include "pragdefn.h"
#include "cfeinfo.h"

local   void    ParmDeclList( void );
local   void    AddParms( void );
local   void    ChkParms( void );
local   void    FuncDefn( SYMPTR );

local   void    BeginFunc( void );
local   void    ReverseParms( void );       /* reverse order of parms */


void ParsePgm( void )
{
    SYM_HANDLE      dummysym;

    CompFlags.external_defn_found = 0;
    CompFlags.initializing_data   = 0;
    dummysym = 0;
    GlobalSym = 0;

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
                CurFuncHandle = 0;
            } else {
                MustRecog( T_LEFT_BRACE );
            }
        }
    } while( CurToken != T_EOF );

    if( CompFlags.external_defn_found == 0 ) {
        if( !CompFlags.extensions_enabled ) {       /* 20-mar-90 */
            CErr1( ERR_NO_EXTERNAL_DEFNS_FOUND );
        }
    }
}


local void FuncDefn( SYMPTR sym )
{
    SYM_NAMEPTR sym_name;
    size_t      sym_len;
    TYPEPTR     typ;

    /* duplicate name in near space */
    sym_name = SymName( sym, CurFuncHandle );
    sym_len = strlen( sym_name ) + 1;
    sym->name = CMemAlloc( sym_len );
    memcpy( sym->name, sym_name, sym_len );
    if( sym->flags & SYM_DEFINED ) {
        CErr2p( ERR_SYM_ALREADY_DEFINED, sym->name );   /* 03-aug-88 */
    }
    typ = sym->sym_type->object;                /* get return type */
    SKIP_TYPEDEFS( typ );

    if( typ->decl_type != TYPE_VOID ) {         /* 26-mar-91 */
        if( TypeSize( typ ) == 0 ) {
            CErr2p( ERR_INCOMPLETE_TYPE, sym_name );
        }
    }
    sym->flags |= /*SYM_REFERENCED | 18-jan-89 */ SYM_DEFINED;

    if( (GenSwitches & NO_OPTIMIZATION) == 0 ) {
        sym->flags |= SYM_OK_TO_RECURSE;                /* 25-sep-91 */
    }

    if( sym->attribs.stg_class == SC_EXTERN  ||  sym->attribs.stg_class == SC_FORWARD ) {
        sym->attribs.stg_class = SC_NULL;       /* indicate exported function */
    }

    CompFlags.external_defn_found = 1;
    if( Toggles & TOGGLE_CHECK_STACK )
        sym->flags |= SYM_CHECK_STACK;

    if( !CompFlags.zu_switch_used ) {
        if( (sym->mods & FLAG_INTERRUPT) == FLAG_INTERRUPT ) {
            /* interrupt function */
            TargetSwitches |= FLOATING_SS;      /* force -zu switch on */
        } else {
            TargetSwitches &= ~FLOATING_SS;     /* turn it back off */
        }
    }
    if( CMPLIT( CurFunc->name, "main" ) == 0 || CMPLIT( CurFunc->name, "wmain" ) == 0 ) {
        sym->mods &= ~MASK_LANGUAGES;  // Turn off any language flags
        sym->mods |= LANG_WATCALL;     // Turn on __watcall calling convention for main
    }
    SymReplace( sym, CurFuncHandle );
}


enum main_names {
    MAIN_WMAIN,
    MAIN_MAIN,
    MAIN_WWINMAIN,
    MAIN_WINMAIN,
    MAIN_WLIBMAIN,
    MAIN_LIBMAIN,
    MAIN_WDLLMAIN,
    MAIN_DLLMAIN,
    MAIN_NUM,
};

static char const   *MainNames[MAIN_NUM] = {
    "wmain",          // MAIN_WMAIN,
    "main",           // MAIN_MAIN,
    "wWinMain",       // MAIN_WWINMAIN,
    "WinMain",        // MAIN_WINMAIN,
    "wLibMain",       // MAIN_WLIBMAIN,
    "LibMain",        // MAIN_LIBMAIN,
    "wDllMain",       // MAIN_WDLLMAIN,
    "DllMain",        // MAIN_DLLMAIN,
};


local void BeginFunc( void )
{
    char                *name;
    char                *segname;
    enum main_names     main_entry;

    if( CurFunc->seginfo == NULL ) {                /* 18-nov-92 */
        CurFunc->seginfo = DefCodeSegment;          /* 22-oct-92 */
        if( CurFunc->seginfo == NULL ) {            /* 08-dec-92 */
            if( CompFlags.zm_switch_used ) {
                name = "";                          /* 05-feb-93 */
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
    name = CurFunc->name;
    for( main_entry = MAIN_WMAIN; main_entry < MAIN_NUM; ++main_entry ) {
       if( strcmp( name, MainNames[main_entry] ) == 0 ) {
           break;
       }
    }

    switch( main_entry ) {
    case MAIN_WMAIN:
        CompFlags.has_wchar_entry =1;
        // fall through!
    case MAIN_MAIN:
        if( CurFunc->u.func.parms ) {               /* 07-dec-88 */
            CompFlags.main_has_parms = 1;
        } else {
            CompFlags.main_has_parms = 0;
        }
        CompFlags.has_main = 1;
        break;

    case MAIN_WWINMAIN:
        CompFlags.has_wchar_entry =1;
        // fall through!
    case MAIN_WINMAIN:
        if( TargSys == TS_WINDOWS || TargSys == TS_CHEAP_WINDOWS
            || TargSys == TS_NT )
        {
            CompFlags.has_winmain = 1;
        } else {
            CompFlags.has_wchar_entry =0;
        }
        break;

    case MAIN_WLIBMAIN:
    case MAIN_WDLLMAIN:
        CompFlags.has_wchar_entry =1;
        // fall through!
    case MAIN_LIBMAIN:
    case MAIN_DLLMAIN:
        CompFlags.has_libmain = 1;
        break;

    case MAIN_NUM:
        break;
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
    case TYPE_CHAR:
    case TYPE_UCHAR:
    case TYPE_SHORT:
#endif
    case TYPE_ENUM:
        arg_typ = GetType( TYPE_INT );
        break;

#if 0
    case TYPE_USHORT:
        arg_typ = GetType( TYPE_UINT );
        break;

    case TYPE_FLOAT:
        arg_typ = GetType( TYPE_DOUBLE );
        break;
#endif
    default:
        break;
    }
}


local void ParmDeclList( void )     /* process old style function definitions */
{
    TYPEPTR             typ;
    PARMPTR             parm;
    decl_state          state;
    SYM_ENTRY           sym;
    decl_info           info;

    while( CurToken != T_LEFT_BRACE ) {
        FullDeclSpecifier( &info );
        if( info.stg == SC_NULL  &&  info.typ == NULL ) {
            if( CurToken == T_ID ) {
                CErr2p( ERR_MISSING_DATA_TYPE, Buffer );
            }
        }
        if( info.stg != SC_NULL  &&  info.stg != SC_REGISTER ) {
            CErr1( ERR_INVALID_STG_CLASS_FOR_PARM );
            info.stg = SC_NULL;
        }
        state = DECL_STATE_NONE;
        typ = info.typ;
        if( typ == NULL ) {
            state |= DECL_STATE_NOTYPE;
            typ = TypeDefault();
        }
        if( info.stg == SC_NULL )
            info.stg = SC_AUTO;

        for( ;; ) {
            if( CurToken == T_SEMI_COLON ) {
                Chk_Struct_Union_Enum( typ );
            } else {
                sym.name = NULL;                        /* 04-oct-91 */
                Declarator( &sym, info.mod, typ, state );
                if( sym.name == NULL  ||  sym.name[0] == '\0' ) {
                    InvDecl();
                } else {
                    for( parm = ParmList; parm != NULL; ) {
                        if( parm->sym.name != NULL ) {  /* 03-may-93 */
                            if( strcmp( parm->sym.name, sym.name ) == 0 ) {
                                break;
                            }
                        }
                        parm = parm->next_parm;
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
        CurFunc->flags |= SYM_OLD_STYLE_FUNC;   /* 13-sep-89 */
        AddParms();
    } else {
        ChkParms();
    }
    ParmList = NULL;
    if( VarParm( CurFunc ) ) {
        CurFunc->flags &= ~ SYM_OK_TO_RECURSE;  /* 25-sep-91 */
    }
}


local void ReverseParms( void )       /* reverse order of parms */
{
    PARMPTR     prev_parm, parm, next_parm;

    if( ParmsToBeReversed( CurFunc->mods, NULL ) ) {
        prev_parm = NULL;
        parm = ParmList;
        while( parm != NULL ) {
            next_parm = parm->next_parm;
            parm->next_parm = prev_parm;
            prev_parm = parm;
            parm = next_parm;
        }
        ParmList = prev_parm;
    }
}


local void ChkProtoType( void )
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


local void AddParms( void )
{
    PARMPTR             parm;
    PARMPTR             prev_parm;
    SYM_HANDLE          sym_handle;
    SYM_HANDLE          prev_sym_handle = 0;
    SYM_HANDLE          new_sym_handle;
    TYPEPTR             typ = NULL;
    int                 parm_count;
    id_hash_idx         h;
    parm_list           *parmlist;
    SYM_ENTRY           new_sym;

    CurFunc->u.func.locals = 0;
    CurFunc->u.func.parms = 0;
    parm = ParmList;
    parmlist = NULL;
    parm_count = 0;
    prev_parm = NULL;
    while( parm != NULL ) {
        new_sym_handle = 0;
        parm->sym.flags |= SYM_DEFINED | SYM_ASSIGNED;
        parm->sym.attribs.is_parm = TRUE;
        h = parm->sym.info.hash;
        if( parm->sym.name[0] == '\0' ) {
            /* no name ==> ... */
            parm->sym.sym_type = GetType( TYPE_DOT_DOT_DOT );
            parm->sym.attribs.stg_class = SC_AUTO;
        } else if( parm->sym.sym_type == NULL ) {
            parm->sym.sym_type = TypeDefault();
            parm->sym.attribs.stg_class = SC_AUTO;
        } else {
/*
        go through ParmList again, looking for FLOAT parms
        change the name to ".P" and duplicate the symbol with type
        float and generate an assignment statement.
*/
            typ = parm->sym.sym_type;
            SKIP_TYPEDEFS( typ );

            switch( typ->decl_type ) {
            case TYPE_CHAR:
            case TYPE_UCHAR:
            case TYPE_SHORT:
                if( CompFlags.strict_ANSI ) {
                    parm->sym.sym_type = GetType( TYPE_INT );
                }
                break;

            case TYPE_USHORT:
                if( CompFlags.strict_ANSI ) {
#if TARGET_SHORT == TARGET_INT
                    parm->sym.sym_type = GetType( TYPE_UINT );
#else
                    parm->sym.sym_type = GetType( TYPE_INT );
#endif
                }
                break;

            case TYPE_FLOAT:
                memcpy( &new_sym, &parm->sym, sizeof(SYM_ENTRY) );
                new_sym.handle = CurFunc->u.func.locals;
                new_sym_handle = SymAdd( h, &new_sym );
                CurFunc->u.func.locals = new_sym_handle;
                SymReplace( &new_sym, new_sym_handle );
                parm->sym.name = ".P";
                parm->sym.flags |= SYM_REFERENCED;      /* 24-nov-89 */
                parm->sym.sym_type = GetType( TYPE_DOUBLE );
                break;

            default:
                break;
            }
        }
        sym_handle = SymAdd( h, &parm->sym );
        if( new_sym_handle != 0 ) {
            TREEPTR         tree;

            tree = ExprNode( VarLeaf( &new_sym, new_sym_handle ),
                 OPR_EQUALS, RValue( VarLeaf(&parm->sym, sym_handle) ) );
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
        parm = parm->next_parm;
    }
    if( prev_parm != NULL ) {
        prev_parm->sym.handle = 0;
        SymReplace( &prev_parm->sym, prev_sym_handle );
        CMemFree( prev_parm );
    }
    typ = CurFunc->sym_type;
    // TODO not following my scheme
    CurFunc->sym_type = FuncNode( typ->object, FLAG_NONE,
        MakeParmList( parmlist, ParmsToBeReversed( CurFunc->mods, NULL ) ) );

    if( PrevProtoType != NULL ) {                       /* 12-may-91 */
        ChkProtoType();
    }
}


local void ChkParms( void )
{
    PARMPTR             parm;
    PARMPTR             prev_parm;
    SYM_HANDLE          sym_handle;
    SYM_HANDLE          prev_sym_handle = 0;
    TYPEPTR             typ;

    CurFunc->u.func.locals = 0;
    CurFunc->u.func.parms  = 0;
    parm = ParmList;
    prev_parm = NULL;
    typ = *(CurFunc->sym_type->u.fn.parms);
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type != TYPE_VOID ) {
        while( parm != NULL ) {
            if( parm->sym.name == NULL ) {              /* 03-may-93 */
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
            parm->sym.attribs.is_parm = TRUE;
            parm = parm->next_parm;
        }
        if( prev_parm != NULL ) {
#if _CPU == 370                     /* 24-oct-91 */
            {
                SYM_ENTRY   var_parm;

                if( VarParm( CurFunc ) ) {
                    typ = ArrayNode( GetType( TYPE_CHAR ) );
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
