/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Callback functions invoked from cg - communicate
*               auxiliary information to the backend.
*
****************************************************************************/


#include "cvars.h"
#include <ctype.h>
#include "cgdefs.h"
#include "cg.h"
#include "cgswitch.h"
#include "pdefn2.h"
#include "compcfg.h"
#include <sys/stat.h>
#include "autodept.h"
#include "langenv.h"
#include "feprotos.h"
#include "cfeinfo.h"
#include "caux.h"
#include "dwarfid.h"


#define TRUNC_SYMBOL_HASH_LEN        4
#define TRUNC_SYMBOL_LEN_WARN        120

#define _HAS_EXE_MAIN   (CompFlags.has_main || CompFlags.has_winmain)
#define _HAS_DLL_MAIN   (CompFlags.bd_switch_used || CompFlags.has_libmain)
#define _HAS_ANY_MAIN   (_HAS_EXE_MAIN || _HAS_DLL_MAIN)

static unsigned char VarFuncWeights[] = {
/* a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z */
   0, 0,13, 0, 2, 1, 0, 0, 0, 0, 0,12, 0,14, 4,10, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0
};

static const char *VarParmFuncs[] = {
    /*
     * functions with var parm lists
     */
    "",             /* 0 */
    "",             /* 1 */
    "",             /* 2 */
    "",             /* 3 */
    "",             /* 4 */
    "",             /* 5 */
    "",             /* 6 */
    "",             /* 7 */
    "fscanf",       /* 8 */
    "fprintf",      /* 9 */
    "execle",       /* 10 */
    "execlpe",      /* 11 */
    "scanf",        /* 12 */
    "sscanf",       /* 13 */
    "sprintf",      /* 14 */
    "spawnle",      /* 15 */
    "spawnlpe",     /* 16 */
    "printf",       /* 17 */
    "execlp",       /* 18 */
    "execl",        /* 19 */
    "cscanf",       /* 20 */
    "cprintf",      /* 21 */
    "open",         /* 22 */
    "spawnlp",      /* 23 */
    "spawnl",       /* 24 */
    "sopen",        /* 25 */
    "",             /* 26 */
    "",             /* 27 */
    "",             /* 28 */
    "",             /* 29 */
    "",             /* 30 */
    "",             /* 31 */
};

/*
 * bitmap for which of the above are Standard
 */
#define VAR_PARM_FUNCS_ANSI \
    ((1<<8) | (1<<9) | (1<<12) | (1<<13) | (1<<14) | (1<<17))

#ifdef __SEH__
  #if _CPU == 386
    hw_reg_set TryParms[] = {
        HW_D( HW_EAX ),
        HW_D( HW_EMPTY )
    };
  #else
    hw_reg_set TryParms[] = {
        HW_D( HW_EMPTY )
    };
  #endif
#endif

#if _CPU == 386

extern const alt_inline_funcs FlatAlternates[];

static struct STRUCT_BYTE_SEQ( 1 ) FinallyCode = {
    1, false, { 0xc3 }   /* ret */
};

static hw_reg_set TryFiniParms[] = {
    HW_D( HW_EAX ),
    HW_D( HW_EMPTY )
};

static struct STRUCT_BYTE_SEQ( 6 ) TryFiniCode = {
    6, false, { 0x64, 0xA3, 0, 0, 0, 0 }  /* mov fs:[0],eax */
};

#endif


bool VarParm( SYMPTR sym )
/*************************
 * does the specified symbol take variable parameters? manual search.
 */
{
    TYPEPTR     *parm_types;
    TYPEPTR     typ;
    TYPEPTR     fn_typ;

    if( sym == NULL )
        return( false );

    if( sym->flags & SYM_FUNCTION ) {
        fn_typ = sym->sym_type;
        SKIP_TYPEDEFS( fn_typ );
        if( fn_typ->u.fn.parms != NULL ) {
            for( parm_types = fn_typ->u.fn.parms; (typ = *parm_types) != NULL; ++parm_types ) {
                if( typ->decl_type == TYP_DOT_DOT_DOT ) {
                    return( true );
                }
            }
        }
    }
    return( false );
}

bool VarFunc( SYMPTR sym )
/*************************
 * does the specified symbol take variable args? hash calc'ed
 */
{
    unsigned    hash;
    size_t      len;
    char        *p;

    if( sym == NULL )
        return( false );

    if( sym->flags & SYM_FUNCTION ) {
        p = sym->name;
        len = strlen( p );
        hash = (len + VarFuncWeights[p[0] - 'a'] + VarFuncWeights[p[len - 1] -'a']) & 31;

        if( strcmp( p, VarParmFuncs[hash] ) == 0
          && ( CompFlags.extensions_enabled || (( 1 << hash ) & VAR_PARM_FUNCS_ANSI) ) )
            return( true );

        return( VarParm( sym ) );
    }
    return( false );
}

#if _INTEL_CPU

static const inline_funcs *Flat( const inline_funcs *ifunc )
{
  #if _CPU == 386
    const alt_inline_funcs  *p;

    if( TargetSwitches & CGSW_X86_FLAT_MODEL ) {
        for( p = FlatAlternates; p->byteseq != NULL; p++ ) {
            if( p->byteseq == ifunc->code ) {
                return( &(p->alt_ifunc) );
            }
        }
    }
  #endif
    return( ifunc );
}

static const inline_funcs *IF_Lookup( const char *name )
{
    const inline_funcs  *ifunc;

    if( GET_FPU( ProcRevision ) > FPU_NONE ) {
        for( ifunc = _8087_Functions; ifunc->name != NULL; ++ifunc ) {
            if( strcmp( ifunc->name, name ) == 0 ) {
                return( Flat( ifunc ) );
            }
        }
    }
    if( OptSize == 100 ) {              /* if /os specified */
        ifunc = SInline_Functions;
        if( TargetSwitches & CGSW_X86_BIG_DATA ) {
  #if _CPU == 8086
            if( TargetSwitches & CGSW_X86_FLOATING_DS ) {
                ifunc = ZF_Data_Functions;
            } else {
                ifunc = ZP_Data_Functions;
            }
  #else /* _CPU == 386 */
            if( TargetSwitches & CGSW_X86_FLOATING_DS ) {
                ifunc = SBigData_Functions;
            } else {
                ifunc = SBigDataNoDs_Functions;
            }
  #endif
        }
        for( ; ifunc->name != NULL; ++ifunc ) {
            if( strcmp( ifunc->name, name ) == 0 ) {
                return( Flat( ifunc ) );
            }
        }
    }
  #if _CPU == 386
    if( TargetSwitches & CGSW_X86_FLAT_MODEL ) {
        for( ifunc = Flat_Functions; ifunc->name != NULL; ++ifunc ) {
            if( strcmp( ifunc->name, name ) == 0 ) {
                return( ifunc );
            }
        }
    }
  #endif
    ifunc = Inline_Functions;
    if( TargetSwitches & CGSW_X86_BIG_DATA ) {
  #if _CPU == 8086
        if( TargetSwitches & CGSW_X86_FLOATING_DS ) {
            ifunc = DF_Data_Functions;
        } else {
            ifunc = DP_Data_Functions;
        }
  #else /* _CPU == 386 */
        if( TargetSwitches & CGSW_X86_FLOATING_DS ) {
            ifunc = BigData_Functions;
        } else {
            ifunc = BigDataNoDs_Functions;
        }
  #endif
    }
    for( ; ifunc->name != NULL; ++ifunc ) {
        if( strcmp( ifunc->name, name ) == 0 ) {
            return( Flat( ifunc ) );
        }
    }
    for( ifunc = Common_Functions; ifunc->name != NULL; ++ifunc ) {
        if( strcmp( ifunc->name, name ) == 0 ) {
            return( Flat( ifunc ) );
        }
    }
    return( NULL );
}
#endif

aux_info *GetLangInfo( type_modifiers flags )
{
    switch( flags & MASK_LANGUAGES ) {
    case LANG_WATCALL:
        return( &WatcallInfo );
    case LANG_CDECL:
        return( &CdeclInfo );
    case LANG_PASCAL:
        return( &PascalInfo );
    case LANG_FORTRAN:
        return( &FortranInfo );
    case LANG_SYSCALL:
        return( &SyscallInfo );
    case LANG_STDCALL:
        return( &StdcallInfo );
    case LANG_FASTCALL:
        return( &FastcallInfo );
    case LANG_OPTLINK:
        return( &OptlinkInfo );
    default:
        return( &DefaultInfo );
    }
}

bool ParmsToBeReversed( type_modifiers flags, aux_info *inf )
{
#ifdef REVERSE
    if( inf == &DefaultInfo ) {
        inf = GetLangInfo( flags );
    }
    if( inf != NULL ) {
        if( inf->cclass & FECALL_GEN_REVERSE_PARMS ) {
            return( true );
        }
    }
#else
    /* unused parameters */ (void)flags; (void)inf;
#endif
    return( false );
}

static aux_info *InfoLookup( SYMPTR sym )
{
    char            *name;
    aux_info        *inf;
    aux_entry       *ent;

    name = sym->name;
    /*
     * assume default
     */
    inf = &DefaultInfo;
    if( name == NULL )
        return( inf );
    ent = AuxLookup( name );
    if( ent != NULL )
        inf = ent->info;
    if( ( ent == NULL )
      || (sym->flags & SYM_INTRINSIC) ) {
        if( sym->flags & SYM_DEFINED )
            return( inf );
        if( (sym->flags & SYM_INTRINSIC) == 0 ) {
            if( strncmp( name, "_inline_", 8 ) != 0 )
                return( inf );
            name += 8;
        }
#if _INTEL_CPU
        {
            const inline_funcs  *ifunc;

            ifunc = IF_Lookup( name );
            if( ifunc == NULL )
                return( inf );
  #if ( _CPU == 8086 )
            if( HW_CEqual( ifunc->returns, HW_DX_AX )
              || HW_CEqual( ifunc->returns, HW_DS_SI )
              || HW_CEqual( ifunc->returns, HW_ES_DI )
              || HW_CEqual( ifunc->returns, HW_CX_DI ) ) {
                if( SizeOfArg( sym->sym_type->object ) != 4 ) {
  #else /* _CPU == 386 */
            if( HW_CEqual( ifunc->returns, HW_DX_AX )
              || HW_CEqual( ifunc->returns, HW_DS_ESI )
              || HW_CEqual( ifunc->returns, HW_ES_EDI )
              || HW_CEqual( ifunc->returns, HW_CX_DI ) ) {
                if( SizeOfArg( sym->sym_type->object ) != 6 ) {
  #endif
                    return( inf );
                }
            }
            inf = &InlineInfo;
            inf->cclass         = FECALL_GEN_NONE;
            inf->cclass_target  = (WatcallInfo.cclass_target & FECALL_X86_FAR_CALL) | FECALL_X86_MODIFY_EXACT;

            if( (sym->flags & SYM_INTRINSIC)
              && ( ent != NULL ) ) {
                inf->cclass         |= ent->info->cclass;
                inf->cclass_target  |= ent->info->cclass_target;
            }
            inf->code = ifunc->code;
            inf->parms = ifunc->parms;
            inf->returns = ifunc->returns;
            if( !HW_CEqual( inf->returns, HW_xAX )
              && !HW_CEqual( inf->returns, HW_EMPTY ) ) {
                inf->cclass_target |= FECALL_X86_SPECIAL_RETURN;
            }
            HW_CAsgn( inf->streturn, HW_EMPTY );
            inf->save = ifunc->save;
            inf->objname = WatcallInfo.objname;
            inf->use = 1;
        }
#endif
    }
    return( inf );
}

aux_info *FindInfo( SYMPTR sym, SYM_HANDLE sym_handle )
{
    SYM_ENTRY       sym_typedef;
    aux_entry       *ent;
    TYPEPTR         typ;
    aux_info        *inf;

    /*
     * assume default
     */
    inf = &DefaultInfo;
    if( sym_handle == SYM_NULL )
        return( inf );

    SymGet( sym, sym_handle );
#if _CPU == 386
    if( (sym_handle == SymSTOSB)
      || (sym_handle == SymSTOSD) ) {
        return( &STOSBInfo );
    } else if( sym_handle == SymFinally ) {
        InlineInfo = WatcallInfo;
        InlineInfo.code = (byte_seq *)&FinallyCode;
        return( &InlineInfo );
    } else if( sym_handle == SymTryFini ) {
        InlineInfo = WatcallInfo;
        InlineInfo.parms = TryFiniParms;
        InlineInfo.code = (byte_seq *)&TryFiniCode;
        return( &InlineInfo );
    }
#endif
    if( (sym->flags & SYM_TEMP) == 0 ) {
        /*
         * not an indirect func call
         */
        inf = InfoLookup( sym );
    }
    if( inf == &DefaultInfo ) {
        typ = sym->sym_type;
        SKIP_DUMMY_TYPEDEFS( typ );
        if( typ->decl_type == TYP_TYPEDEF ) {
            SymGet( &sym_typedef, typ->u.typedefn );
            if( sym_typedef.name != NULL ) {
                ent = AuxLookup( sym_typedef.name );
                if( ent != NULL ) {
                    inf = ent->info;
                }
            }
        }
    }
#if _CPU == 386
    if( (inf->flags & AUX_FLAG_FAR16)
      || (sym->mods & FLAG_FAR16) ) {
        if( (sym->mods & MASK_LANGUAGES) == LANG_PASCAL
          || (inf->cclass & FECALL_GEN_REVERSE_PARMS) ) {
            return( &Far16PascalInfo );
        } else {
            return( &Far16CdeclInfo );
        }
    }
#endif
    if( inf == &DefaultInfo ) {
        inf = GetLangInfo( sym->mods );
    }
    return( inf );
}

bool FunctionAborts( SYMPTR sym, SYM_HANDLE sym_handle )
{
    aux_entry    *ent;

    if( sym_handle != SYM_NULL ) {
        SymGet( sym, sym_handle );
        if( sym->mods & FLAG_ABORTS )
            return( true );
        if( sym->mods & FLAG_NORETURN )
            return( true );
        ent = AuxLookup( SymName( sym, sym_handle ) );
        if( ent != NULL ) {
            if( ent->info->cclass & FECALL_GEN_ABORTS ) {
                return( true );
            }
        }
    }
    return( false );
}

static call_class getCallClass( SYM_HANDLE sym_handle )
/******************************************************
 * handle only generic attributes for call class
 */
{
    aux_info            *inf;
    SYM_ENTRY           sym;
    call_class          cclass;

    sym.mods = 0;
    inf = FindInfo( &sym, sym_handle );
    cclass = inf->cclass;
    if( sym_handle != SYM_NULL ) {
        if( sym.flags & SYM_FUNCTION ) {
            if( sym.mods & FLAG_ABORTS ) {
                cclass |= FECALL_GEN_ABORTS;
            }
            if( sym.mods & FLAG_NORETURN ) {
                cclass |= FECALL_GEN_NORETURN;
            }
            if( sym.mods & FLAG_EXPORT ) {
                cclass |= FECALL_GEN_DLL_EXPORT;
            }
            if( IsInLineFunc( sym_handle ) ) {
                cclass |= FECALL_GEN_MAKE_CALL_INLINE;
            }
            if( VarFunc( &sym ) ) {
                cclass |= FECALL_GEN_CALLER_POPS | FECALL_GEN_HAS_VARARGS;
            }
        }
    }
#ifdef REVERSE
    cclass &= ~ FECALL_GEN_REVERSE_PARMS;
#endif
    return( cclass );
}

static call_class_target getCallClassTarget( SYM_HANDLE sym_handle )
/*******************************************************************
 * handle only target specific attributes for call class
 */
{
    call_class_target   cclass_target;
#if _INTEL_CPU
    aux_info            *inf;
    SYM_ENTRY           sym;

    sym.mods = 0;
    inf = FindInfo( &sym, sym_handle );
    cclass_target = inf->cclass_target;
    if( sym_handle != SYM_NULL ) {
        if( sym.flags & SYM_FUNCTION ) {
  #if _CPU == 8086
            if( TargetSystem == TS_WINDOWS ) {
                if( inf == &PascalInfo
                  || inf == &CdeclInfo ) {
                    cclass_target |= FECALL_X86_PROLOG_FAT_WINDOWS;
                }
            }
  #endif
            if( sym.mods & FLAG_FARSS ) {
                cclass_target |= FECALL_X86_FARSS;
            }
            if( CompFlags.emit_names ) {
                cclass_target |= FECALL_X86_EMIT_FUNCTION_NAME;
            }
            if( sym.mods & FLAG_FAR ) {
                cclass_target |= FECALL_X86_FAR_CALL;
                if( sym.mods & FLAG_NEAR ) {
                    cclass_target |= FECALL_X86_INTERRUPT;
                }
            } else if( sym.mods & FLAG_NEAR ) {
                cclass_target &= ~ FECALL_X86_FAR_CALL;
            }
            if( sym.mods & FLAG_LOADDS ) {
  #if 0
                if( TargetSystem == TS_WINDOWS ) {
                    cclass_target |= FECALL_X86_PROLOG_FAT_WINDOWS;
                } else {
                    cclass_target |= FECALL_X86_LOAD_DS_ON_ENTRY;
                }
  #else
                cclass_target |= FECALL_X86_LOAD_DS_ON_ENTRY;
  #endif
            }
        }
        if( sym.flags & SYM_FUNC_NEEDS_THUNK ) {
            cclass_target |= FECALL_X86_THUNK_PROLOG;
        }
    }
    if( CompFlags.ep_switch_used ) {
        cclass_target |= FECALL_X86_PROLOG_HOOKS;
    }
    if( CompFlags.ee_switch_used ) {
        cclass_target |= FECALL_X86_EPILOG_HOOKS;
    }
    if( CompFlags.sg_switch_used ) {
        cclass_target |= FECALL_X86_GROW_STACK;
    }
    if( CompFlags.st_switch_used ) {
        cclass_target |= FECALL_X86_TOUCH_STACK;
    }
#else
    /* unused parameters */ (void)sym_handle;

    cclass_target = 0;
#endif
    return( cclass_target );
}

static void addDefaultLibs( void )
/*********************************
 * NextLibrary
 *  Called (indirectly) from the code generator to inject automagically defined symbols.
 * Inputs:
 *  index   (n-1)
 *      Usually called from a loop until we return 0/NULL to show no more libraries
 *  request
 *      FEINF_NEXT_LIBRARY
 *          examines the current flags to see if any libraries should be
 *          automagically referenced and returns the relevant index if so.
 *      FEINF_LIBRARY_NAME
 *          returns the requested name.
 */
{
    if( CompFlags.emit_library_names ) {
        if( _HAS_ANY_MAIN
          || CompFlags.pragma_library
          || CompFlags.emit_all_default_libs ) {
            AddLibraryName( CLIB_Name + 1, CLIB_Name[0] );
        }
        AddLibraryName( MATHLIB_Name + 1, MATHLIB_Name[0] );
        if( EmuLib_Name != NULL ) {
            AddLibraryName( EmuLib_Name + 1, EmuLib_Name[0] );
        }
    }
}

static CGPOINTER NextLibrary( int index, aux_class request )
{
    library_list    *lib;
    char            *name;
    int             i;

    name = NULL;
    if( index == 0 ) {
        addDefaultLibs();
    }
    if( request == FEINF_NEXT_LIBRARY )
        ++index;
    if( index > 0 ) {
        for( i = 1, lib = HeadLibs; lib != NULL; lib = lib->next ) {
            if( i == index ) {
                name = lib->libname;
                break;
            }
            ++i;
        }
    }
    /*
     * return library name, or
     */
    if( request == FEINF_LIBRARY_NAME
      || name == NULL )
        return( (CGPOINTER)name );
    /*
     * library index
     */
    return( (CGPOINTER)(pointer_uint)index );
}

static CGPOINTER NextAlias( int index, aux_class request )
/*********************************************************
 * NextAlias
 *  Called (indirectly) from the code generator to go through the list of
 *  linker aliases.
 * Inputs:
 *  index    (n-1)
 *      Called from a loop until we return 0/NULL to show no more aliases
 *  request
 *      FEINF_NEXT_ALIAS
 *          returns the index of next alias in the list, or zero if none.
 *      FEINF_ALIAS_NAME
 *          returns the alias name, or NULL if alias refers to a symbol.
 *      FEINF_ALIAS_SYMBOL
 *          returns the alias symbol, or NULL if alias refers to a name.
 *      FEINF_ALIAS_SUBST_NAME
 *          returns the name to be substituted for the alias, or NULL.
 *      FEINF_ALIAS_SUBST_SYMBOL
 *          returns the symbol to be substituted for the alias, or NULL.
 *
 * Note: One of FEINF_ALIAS..._NAME and FEINF_ALIAS..._SYMBOL will always be 0/NULL
 * and the other will be valid, depending on which form of the pragma was used.
 */
{
    alias_list          *aliaslist;
    SYM_HANDLE          alias_sym = SYM_NULL;
    SYM_HANDLE          subst_sym = SYM_NULL;
    const char          *alias_name = NULL;
    const char          *subst_name = NULL;
    int                 i;

    if( request == FEINF_NEXT_ALIAS )
        ++index;

    for( i = 1, aliaslist = AliasHead; aliaslist != NULL; aliaslist = aliaslist->next, ++i ) {
        alias_name = aliaslist->name;
        alias_sym = aliaslist->a_sym;
        subst_name = aliaslist->subst;
        subst_sym = aliaslist->s_sym;
        if( i == index ) {
            break;
        }
    }
    if( aliaslist == NULL )
        index = 0;          /* no (more) aliases */

    if( request == FEINF_ALIAS_NAME ) {
        return( (CGPOINTER)alias_name );
    } else if( request == FEINF_ALIAS_SYMBOL ) {
        return( (CGPOINTER)alias_sym );
    } else if( request == FEINF_ALIAS_SUBST_NAME ) {
        return( (CGPOINTER)subst_name );
    } else if( request == FEINF_ALIAS_SUBST_SYMBOL ) {
        return( (CGPOINTER)subst_sym );
    } else {    /* this had better be a FEINF_NEXT_ALIAS request */
        return( (CGPOINTER)(pointer_uint)index );
    }
}

static unsigned GetParmsSize( SYM_HANDLE sym_handle )
/****************************************************
 * Return the size of function parameters or -1 if size could
 * not be determined (symbol isn't a function or is variadic)
 */
{
    unsigned    total_parm_size = 0;
    unsigned    parm_size;
    TYPEPTR     fn_typ;
    TYPEPTR     *parm_types;
    TYPEPTR     typ;
    SYM_ENTRY   sym;

    SymGet( &sym, sym_handle );
    fn_typ = sym.sym_type;
    SKIP_TYPEDEFS( fn_typ );
    if( fn_typ->decl_type == TYP_FUNCTION ) {
        if( fn_typ->u.fn.parms != NULL ) {
            for( parm_types = fn_typ->u.fn.parms; (typ = *parm_types) != NULL; ++parm_types ) {
                if( typ->decl_type == TYP_DOT_DOT_DOT ) {
                    total_parm_size = (unsigned)-1;
                    break;
                }

                SKIP_TYPEDEFS( typ );
                if( typ->decl_type == TYP_VOID )
                    break;

                parm_size = _RoundUp( TypeSize( typ ), TARGET_INT );
                total_parm_size += parm_size;
            }
        }
    } else {
        total_parm_size = (unsigned)-1;
    }
    return( total_parm_size );
}

static const char *GetNamePattern( SYM_HANDLE sym_handle )
/*********************************************************
 * Return name pattern manipulator string
 */
{
    char                 *pattern;
    SYM_ENTRY            sym;
    aux_info             *inf;

    inf = FindInfo( &sym, sym_handle );
#ifdef __SEH__
    if(( sym_handle == SymTryInit )
      || ( sym_handle == SymTryFini )
      || ( sym_handle == SymTryUnwind )
      || ( sym_handle == SymExcept )) {
        pattern = "*";
    } else {
#endif
        if( sym.flags & SYM_FUNCTION ) {
            pattern = inf->objname;
#if _INTEL_CPU
            if( VarFunc( &sym ) ) {
                if( inf == &DefaultInfo )
                    inf = DftCallConv;
                if( inf == &StdcallInfo ) {
                    pattern = CdeclInfo.objname;
                } else if( inf == &FastcallInfo ) {
                    pattern = CdeclInfo.objname;
                }
            }
#endif
            if( pattern == NULL ) {
                pattern =  TS_CODE_MANGLE;
            }
        } else {
            pattern = VarNamePattern( inf );
            if( pattern == NULL ) {
                pattern =  TS_DATA_MANGLE;
            }
        }
#ifdef __SEH__
    }   /* close that else */
#endif
    return( pattern );
}

static const char *GetBaseName( SYM_HANDLE sym_handle )
{
    SYM_ENTRY            sym;

    SymGet( &sym, sym_handle );
    return( sym.name );
}

const char *FEExtName( CGSYM_HANDLE sym_handle, int request )
/***********************************************************/
{
    switch( request ) {
    case EXTN_BASENAME:
        return( GetBaseName( (SYM_HANDLE)sym_handle ) );
    case EXTN_PATTERN:
        return( GetNamePattern( (SYM_HANDLE)sym_handle ) );
    case EXTN_PRMSIZE:
        return( (const char *)(pointer_uint)GetParmsSize( (SYM_HANDLE)sym_handle ) );
    case EXTN_IMPPREFIX:
        return( ( TargetSystem == TS_NT ) ? "__imp_" : NULL );
    case EXTN_CALLBACKNAME:
    default:
        return( NULL );
    }
}

static void addDefaultImports( void )
{
    typedef enum {
        CM_DLLMAIN      = 0x01,
        CM_WINMAIN      = 0x02,
        CM_MAIN         = 0x04,
        CM_NULL         = 0x00
    } check_mask;

    if( _HAS_ANY_MAIN ) {
        check_mask control;

        if( CompFlags.bd_switch_used ) {
            control = CM_DLLMAIN;
        } else if( CompFlags.bw_switch_used ) {
            control = CM_WINMAIN | CM_MAIN;
        } else if( CompFlags.bg_switch_used ) {
            control = CM_WINMAIN;
        } else if( CompFlags.bc_switch_used ) {
            control = CM_MAIN;
        } else {
            control = CM_DLLMAIN | CM_WINMAIN | CM_MAIN;
        }
        if( (control & CM_DLLMAIN) && _HAS_DLL_MAIN ) {
            if( CompFlags.has_wchar_entry ) {
                AddExtRefN( "__DLLstartw_" );
            } else {
                AddExtRefN( "__DLLstart_" );
            }
            control = CM_NULL;
        }
#if _CPU == 8086
        if( (control & CM_WINMAIN) && CompFlags.has_winmain
          || (TargetSwitches & CGSW_X86_WINDOWS) && CompFlags.has_main ) {
#else
        if( (control & CM_WINMAIN) && CompFlags.has_winmain ) {
#endif
            if( CompFlags.has_wchar_entry ) {
                AddExtRefN( "_wstartw_" );
            } else {
                AddExtRefN( "_wstart_" );
            }
            control = CM_NULL;
        }
        if( control & CM_MAIN ) {
            assert( CompFlags.has_main );
            if( CompFlags.has_wchar_entry ) {
                AddExtRefN( "_cstartw_" );
            } else {
                AddExtRefN( "_cstart_" );
            }
            control = CM_NULL;
        }
    }
#if _INTEL_CPU
    if( CompFlags.emit_library_names ) {
        if( CompFlags.float_used ) {
            if( CompFlags.use_long_double ) {
                AddExtRefN( "_fltused_80bit_" );
            } else {
                AddExtRefN( "_fltused_" );
            }
        }
  #if _CPU == 8086
        if( FirstStmt != 0 ) {
            if( TargetSwitches & CGSW_X86_BIG_CODE ) {
                AddExtRefN( "_big_code_" );
            } else {
                AddExtRefN( "_small_code_" );
            }
        }
  #endif
        if( CompFlags.pgm_used_8087
          || CompFlags.float_used ) {
            if( GET_FPU( ProcRevision ) & FPU_EMU ) {
  #if _CPU == 8086
                AddExtRefN( "__init_87_emulator" );
  #else /* _CPU == 386 */
                AddExtRefN( "__init_387_emulator" );
  #endif
            }
            if( GET_FPU( ProcRevision ) > FPU_NONE ) {
                if( Stack87 == 4 ) {
                    AddExtRefN( "__old_8087" );
                } else {
                    AddExtRefN( "__8087" );
                }
            }
        }
    }
#else /* _RISC_CPU */
    if( CompFlags.emit_library_names ) {
        /*
         * handle floating-point support
         */
        if( CompFlags.float_used ) {
            AddExtRefN( "_fltused_" );
        }
    }
#endif
#if _INTEL_CPU
    if( CompFlags.main_has_parms ) {
  #if _CPU == 8086
        if( CompFlags.has_wchar_entry ) {
            AddExtRefN( "__wargc" );
        } else {
            AddExtRefN( "__argc" );
        }
  #else /* _CPU == 386 */
        if( CompFlags.register_conventions ) {
            if( CompFlags.has_wchar_entry ) {
                AddExtRefN( "__wargc" );
            } else {
                AddExtRefN( "__argc" );
            }
        } else {
            if( CompFlags.has_wchar_entry ) {
                AddExtRefN( "_wargc" );
            } else {
                AddExtRefN( "_argc" );
            }
        }
  #endif
    }
#else /* _RISC_CPU */
    if( CompFlags.main_has_parms ) {
        AddExtRefN( "_argc" );
    }
#endif
    /*
     * handle default windowing app
     */
    if( CompFlags.bw_switch_used ) {
        AddExtRefN( "__init_default_win" );
    }
#if _INTEL_CPU
    /*
     * handle NetWare
     */
    if( TargetSystem == TS_NETWARE
      || TargetSystem == TS_NETWARE5 ) {
        /*
         * is target NETWARE or NETWARE5?
         */
        AddExtRefN( "__WATCOM_Prelude" );
    }
    /*
     * handle 'old' profiling
     */
    if( TargetSwitches & CGSW_X86_P5_PROFILING ) {
        /*
         * is profiling enabled (-et)?
         */
        AddExtRefN( "__p5_profile" );
    }
    /*
     * handle 'new' profiling
     */
    if( TargetSwitches & CGSW_X86_NEW_P5_PROFILING ) {
        /*
         * is profiling enabled (-etp)?
         */
        AddExtRefN( "__new_p5_profile" );
    }
#endif
}


static CGPOINTER NextImport( int index, aux_class request )
/**********************************************************
 * NextImport
 *  Called (indirectly) from the code generator to inject automagically defined symbols.
 * Inputs:
 *  index    (n-1)
 *      Usually called from a loop until we return 0/NULL to show no more symbols
 *      are required.
 *  request
 *      FEINF_NEXT_IMPORT
 *          examines the current flags to see if any symbols should be
 *          automagically inserted and returns the relevant index if so.
 *      FEINF_IMPORT_NAME
 *          returns the requested name. if we have returned an index for
 *          the current compiler settings we should be called with a valid
 *          index but we still perform exactly the same checks as this is
 *          good practise.
 */
{
    char        *name;
    int         i;
    extref_info *e;

    if( !CompFlags.emit_targimp_symbols )
        return( NULL );

    name = NULL;
    if( index == 0 ) {
        addDefaultImports();
    }
    if( request == FEINF_NEXT_IMPORT )
        ++index;
    if( index > 0 ) {
        for( i = 1, e = ExtrefInfo; e != NULL; e = e->next ) {
            if( e->symbol != NULL )
                continue;
            if( i == index ) {
                name = e->name;
                break;
            }
            ++i;
        }
    }
    /*
     * return the import name, or
     */
    if( request == FEINF_IMPORT_NAME
      || name == NULL )
        return( (CGPOINTER)name );
    /*
     * return the index
     */
    return( (CGPOINTER)(pointer_uint)index );
}

static CGPOINTER NextImportS( int index, aux_class request )
/**********************************************************/
{
    void        *symbol;
    int         i;
    extref_info *e;

    if(!CompFlags.emit_targimp_symbols)
        return( NULL );

    symbol = NULL;
    if( request == FEINF_NEXT_IMPORT_S )
        ++index;
    if( index > 0 ) {
        for( i = 1, e = ExtrefInfo; e != NULL; e = e->next ) {
            if( e->symbol == NULL )
                continue;
            if( i == index ) {
                symbol = e->symbol;
                break;
            }
            ++i;
        }
    }
    /*
     * return the import symbol, or
     */
    if( request == FEINF_IMPORT_NAME_S
      || symbol == NULL )
        return( (CGPOINTER)symbol );
    /*
     * return the index
     */
    return( (CGPOINTER)(pointer_uint)index );
}

CGPOINTER FEAuxInfo( CGPOINTER req_handle, aux_class request )
/*************************************************************
 * pass auxiliary information to back end
 */
{
    aux_info            *inf;
    SYM_ENTRY           sym;
    static hw_reg_set   save_set;

    switch( request ) {
    case FEINF_SOURCE_LANGUAGE:
        return( (CGPOINTER)"C" );
#if _INTEL_CPU
    case FEINF_STACK_SIZE_8087:
        return( (CGPOINTER)(pointer_uint)Stack87 );
    case FEINF_CODE_GROUP:
        return( (CGPOINTER)GenCodeGroup );
    case FEINF_DATA_GROUP:
        return( (CGPOINTER)DataSegName );
#endif
    case FEINF_OBJECT_FILE_NAME:
        return( (CGPOINTER)ObjFileName() );
    case FEINF_REVISION_NUMBER:
        return( (CGPOINTER)(pointer_uint)II_REVISION );
    case FEINF_AUX_LOOKUP:
        return( req_handle );
#if _INTEL_CPU
    case FEINF_PROEPI_DATA_SIZE:
        return( (CGPOINTER)(pointer_uint)ProEpiDataSize );
    case FEINF_DBG_PREDEF_SYM:
        return( (CGPOINTER)SymDFAbbr );
    case FEINF_P5_CHIP_BUG_SYM:
        return( (CGPOINTER)SymChipBug );
    case FEINF_CODE_LABEL_ALIGNMENT:
        {
            static  unsigned char   Alignment[] = { 2, 1, 1 };

            if( OptSize == 0 )
                Alignment[1] = TARGET_INT;

            return( (CGPOINTER)Alignment );
        }
    case FEINF_CLASS_NAME:
        return( (CGPOINTER)SegClassName( (segment_id)(pointer_uint)req_handle ) );
    case FEINF_USED_8087:
        CompFlags.pgm_used_8087 = true;
        return( NULL );
  #if _CPU == 386
    case FEINF_P5_PROF_DATA:
        return( (CGPOINTER)FunctionProfileBlock );
    case FEINF_P5_PROF_SEG:
        return( (CGPOINTER)(pointer_uint)FunctionProfileSegId );
  #endif
#endif
    case FEINF_SOURCE_NAME:
        if( SrcFName == ModuleName ) {
            return( (CGPOINTER)FNameFullPath( FNames ) );
        } else {
            return( (CGPOINTER)ModuleName );
        }
    case FEINF_CALL_CLASS:
        return( (CGPOINTER)getCallClass( req_handle ) );
    case FEINF_CALL_CLASS_TARGET:
        return( (CGPOINTER)getCallClassTarget( req_handle ) );
    case FEINF_FREE_SEGMENT:
        return( NULL );
    case FEINF_NEXT_LIBRARY:
    case FEINF_LIBRARY_NAME:
        return( NextLibrary( (int)(pointer_uint)req_handle, request ) );
    case FEINF_NEXT_IMPORT:
    case FEINF_IMPORT_NAME:
        return( NextImport( (int)(pointer_uint)req_handle, request ) );
    case FEINF_NEXT_IMPORT_S:
    case FEINF_IMPORT_NAME_S:
        return( NextImportS( (int)(pointer_uint)req_handle, request ) );
    case FEINF_NEXT_ALIAS:
    case FEINF_ALIAS_NAME:
    case FEINF_ALIAS_SYMBOL:
    case FEINF_ALIAS_SUBST_NAME:
    case FEINF_ALIAS_SUBST_SYMBOL:
        return( NextAlias( (int)(pointer_uint)req_handle, request ) );
    case FEINF_TEMP_LOC_NAME:
        return( (CGPOINTER)(pointer_uint)TEMP_LOC_QUIT );
    case FEINF_TEMP_LOC_TELL:
        return( NULL );
    case FEINF_NEXT_DEPENDENCY:
        if( CompFlags.emit_dependencies )
            return( (CGPOINTER)NextDependency( (FNAMEPTR)req_handle ) );
        return( NULL );
    case FEINF_DEPENDENCY_TIMESTAMP:
        return( (CGPOINTER)&(((FNAMEPTR)req_handle)->mtime) );
    case FEINF_DEPENDENCY_NAME:
        return( (CGPOINTER)FNameFullPath( (FNAMEPTR)req_handle ) );
#if _INTEL_CPU
    case FEINF_PEGGED_REGISTER:
        return( (CGPOINTER)SegPeggedReg( (segment_id)(pointer_uint)req_handle ) );
#endif
    case FEINF_DBG_DWARF_PRODUCER:
        return( (CGPOINTER)DWARF_PRODUCER_ID );
    case FEINF_SAVE_REGS:
        sym.mods = 0;
        inf = FindInfo( &sym, req_handle );
        save_set = inf->save;
#if _INTEL_CPU
        if( sym.mods & FLAG_SAVEREGS ) {
            HW_CTurnOn( save_set, HW_SEGS );
        }
  #ifdef __SEH__
        if( (SYM_HANDLE)req_handle == SymTryInit ) {
            HW_CTurnOff( save_set, HW_xSP );
        }
  #endif
#endif
        return( (CGPOINTER)&save_set );
    case FEINF_RETURN_REG:
        inf = FindInfo( &sym, req_handle );
        return( (CGPOINTER)&inf->returns );
    case FEINF_CALL_BYTES:
        inf = FindInfo( &sym, req_handle );
        return( (CGPOINTER)inf->code );
    case FEINF_PARM_REGS:
#if _INTEL_CPU
  #ifdef __SEH__
        if(( (SYM_HANDLE)req_handle == SymTryInit )
          || ( (SYM_HANDLE)req_handle == SymTryFini )
          || ( (SYM_HANDLE)req_handle == SymTryUnwind )
          || ( (SYM_HANDLE)req_handle == SymExcept )) {
            return( (CGPOINTER)TryParms );
        }
  #endif
#endif
        inf = FindInfo( &sym, req_handle );
        if( req_handle != NULL ) {
            if( inf->code == NULL
              && VarFunc( &sym ) ) {
                return( (CGPOINTER)DefaultVarParms );
            }
        }
        return( (CGPOINTER)inf->parms );
#if _INTEL_CPU
    case FEINF_STRETURN_REG:
        inf = FindInfo( &sym, req_handle );
        return( (CGPOINTER)&inf->streturn );
#endif
    default:
        break;
    }
    return( NULL );
}

char *SrcFullPath( char const *name, char *buff, size_t max )
{
    return( _getFilenameFullPath( buff, name, max ) );
}
