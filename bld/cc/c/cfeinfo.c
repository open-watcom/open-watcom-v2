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
* Description:  Callback functions invoked from cg - communicate
*               auxiliary information to the backend.
*
****************************************************************************/


#include "cvars.h"
#include <ctype.h>
#include "cgdefs.h"
#include "cg.h"
#include "cgswitch.h"
#include "pragdefn.h"
#include "pdefn2.h"
#include "compcfg.h"
#include <sys/stat.h>
#include "autodept.h"
#include "langenv.h"
#include "feprotos.h"
#include "cfeinfo.h"
#include "caux.h"

#define TRUNC_SYMBOL_HASH_LEN        4
#define TRUNC_SYMBOL_LEN_WARN        120

#define _HAS_EXE_MAIN   (CompFlags.has_main || CompFlags.has_winmain)
#define _HAS_DLL_MAIN   (CompFlags.bd_switch_used || CompFlags.has_libmain)
#define _HAS_ANY_MAIN   (_HAS_EXE_MAIN || _HAS_DLL_MAIN)

static unsigned char VarFuncWeights[] = {
//a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y,z
  0, 0,13, 0, 2, 1, 0, 0, 0, 0, 0,12, 0,14, 4,10, 0, 0, 6, 0, 0, 0, 0, 0, 0,0
};

static char *VarParmFuncs[] = {
    /* functions with var parm lists */
    "",             // 0
    "",             // 1
    "",             // 2
    "",             // 3
    "",             // 4
    "",             // 5
    "",             // 6
    "",             // 7
    "fscanf",       // 8
    "fprintf",      // 9
    "execle",       // 10
    "execlpe",      // 11
    "scanf",        // 12
    "sscanf",       // 13
    "sprintf",      // 14
    "spawnle",      // 15
    "spawnlpe",     // 16
    "printf",       // 17
    "execlp",       // 18
    "execl",        // 19
    "cscanf",       // 20
    "cprintf",      // 21
    "open",         // 22
    "spawnlp",      // 23
    "spawnl",       // 24
    "sopen",        // 25
    "",             // 26
    "",             // 27
    "",             // 28
    "",             // 29
    "",             // 30
    "",             // 31
};

/* bitmap for which of the above are Standard */
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

extern byte_seq     *FlatAlternates[];

static struct STRUCT_byte_seq( 1 ) FinallyCode = { 
    1, FALSE, { 0xc3 }   /* ret */
};

static hw_reg_set TryFiniParms[] = {
    HW_D( HW_EAX ),
    HW_D( HW_EMPTY )
};

static struct STRUCT_byte_seq( 6 ) TryFiniCode = { 
    6, FALSE, { 0x64, 0xA3, 0, 0, 0, 0 }  /* mov fs:[0],eax */
};

#endif


/*
//    does the specified symbol take variable parameters? manual search.
*/
int VarParm( SYMPTR sym )
{
    TYPEPTR     *parm;
    TYPEPTR     typ;
    TYPEPTR     fn_typ;

    if( sym == NULL )
        return( 0 );

    if( sym->flags & SYM_FUNCTION ) {
        fn_typ = sym->sym_type;
        SKIP_TYPEDEFS( fn_typ );
        parm = fn_typ->u.fn.parms;
        if( parm != NULL ) {
            for( ; (typ = *parm) != NULL; ++parm ) {
                if( typ->decl_type == TYPE_DOT_DOT_DOT ) {
                    return( 1 );
                }
            }
        }
    }
    return( 0 );
}

/*
//    does the specified symbol take variable args? hash calc'ed
//
*/
int VarFunc( SYMPTR sym )
{
    int         hash;
    size_t      len;
    char        *p;

    if( sym == NULL )
        return( 0 );

    if( sym->flags & SYM_FUNCTION ) {
        p = sym->name;
        len = strlen( p );
        hash = (len + VarFuncWeights[ p[0] - 'a' ] + VarFuncWeights[ p[len - 1] -'a' ]) & 31;

        if( strcmp( p, VarParmFuncs[ hash ] ) == 0 
            && ( CompFlags.extensions_enabled || ( ( 1 << hash ) & VAR_PARM_FUNCS_ANSI ) ) )
            return( 1 );

        return( VarParm( sym ) );
    }
    return( 0 );
}

#if ( _CPU == 8086 ) || ( _CPU == 386 )

static inline_funcs *Flat( inline_funcs *ifunc )
{
  #if _CPU == 386
    byte_seq            **p;

    if( TargetSwitches & FLAT_MODEL ) {
        for( p = FlatAlternates; p[0] != NULL; p += 2 ) {
            if( p[0] == ifunc->code ) {
                ifunc->code = p[1];
                return( ifunc );
            }
        }
    }
  #endif
    return( ifunc );
}

inline_funcs *IF_Lookup( char *name )
{
    inline_funcs     *ifunc;

    if( GET_FPU( ProcRevision ) > FPU_NONE ) {
        ifunc = _8087_Functions;
        while( ifunc->name ) {
            if( strcmp( ifunc->name, name ) == 0 )
                return( Flat( ifunc ) );
            ++ifunc;
        }
    }
    if( OptSize == 100 ) {              /* if /os specified */
        ifunc = SInline_Functions;
        if( TargetSwitches & BIG_DATA ) {
  #if _CPU == 8086
            if( TargetSwitches & FLOATING_DS ) {
                ifunc = ZF_Data_Functions;
            } else {
                ifunc = ZP_Data_Functions;
            }
  #else
            if( TargetSwitches & FLOATING_DS ) {
                ifunc = SBigData_Functions;
            } else {
                ifunc = SBigDataNoDs_Functions;
            }
  #endif
        }
        while( ifunc->name ) {
            if( strcmp( ifunc->name, name ) == 0 )
                return( Flat( ifunc ) );
            ++ifunc;
        }
    }
  #if _CPU == 386
    if( TargetSwitches & FLAT_MODEL ) {
        ifunc = Flat_Functions;
        while( ifunc->name ) {
            if( strcmp( ifunc->name, name ) == 0 )
                return( ifunc );
            ++ifunc;
        }
    }
  #endif
    ifunc = Inline_Functions;
    if( TargetSwitches & BIG_DATA ) {
  #if _CPU == 8086
        if( TargetSwitches & FLOATING_DS ) {
            ifunc = DF_Data_Functions;
        } else {
            ifunc = DP_Data_Functions;
        }
  #else
        if( TargetSwitches & FLOATING_DS ) {
            ifunc = BigData_Functions;
        } else {
            ifunc = BigDataNoDs_Functions;
        }
  #endif
    }
    while( ifunc->name ) {
        if( strcmp( ifunc->name, name ) == 0 )
            return( Flat( ifunc ) );
        ++ifunc;
    }
    ifunc = Common_Functions;
    while( ifunc->name ) {
        if( strcmp( ifunc->name, name ) == 0 )
            return( Flat( ifunc ) );
        ++ifunc;
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

/*
//    return language specific info
*/
static aux_info *LangInfo( type_modifiers flags, aux_info *inf )
{
    if( inf != &DefaultInfo )
        return( inf );

    return( GetLangInfo( flags ) );
}

bool ParmsToBeReversed( int flags, aux_info *inf )
{
#ifdef REVERSE
    inf = LangInfo( flags, inf );
    if( inf != NULL ) {
        if( inf->cclass & REVERSE_PARMS ) {
            return( TRUE );
        }
    }
#else
    flags = flags;
    inf = inf;
#endif
    return( FALSE );
}

aux_info *InfoLookup( SYMPTR sym )
{
    char            *name;
    aux_info        *inf;
    aux_entry       *ent;

    name = sym->name;
    inf = &DefaultInfo;         /* assume default */
    if( name == NULL )
        return( inf );                   /* 01-jun-90 */
    ent = AuxLookup( name );
    if( ent != NULL )
        inf = ent->info;
    if( ( ent == NULL ) || (sym->flags & SYM_INTRINSIC) ) {
        if( sym->flags & SYM_DEFINED )
            return( inf );
        if( !(sym->flags & SYM_INTRINSIC) ) {
            if( memcmp( name, "_inline_", 8 ) != 0 )
                return( inf );
            name += 8;
        }
#if ( _CPU == 8086 ) || ( _CPU == 386 )
        {
            inline_funcs     *ifunc;

            ifunc = IF_Lookup( name );
            if( ifunc == NULL )
                return( inf );
  #if ( _CPU == 8086 )
            if( HW_CEqual( ifunc->returns, HW_DX_AX )
              || HW_CEqual( ifunc->returns, HW_DS_SI )
              || HW_CEqual( ifunc->returns, HW_ES_DI )
              || HW_CEqual( ifunc->returns, HW_CX_DI ) ) {
                if( SizeOfArg( sym->sym_type->object ) != 4 ) {
  #else
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
            inf->cclass = (WatcallInfo.cclass & FAR_CALL) | MODIFY_EXACT;
            if( (sym->flags & SYM_INTRINSIC) && ( ent != NULL ) )
                inf->cclass |= ent->info->cclass;
            inf->code = ifunc->code;
            inf->parms = ifunc->parms;
            inf->returns = ifunc->returns;
  #if ( _CPU == 8086 )
            if( !HW_CEqual( inf->returns, HW_AX )
              && !HW_CEqual( inf->returns, HW_EMPTY ) ) {
  #else
            if( !HW_CEqual( inf->returns, HW_EAX )
              && !HW_CEqual( inf->returns, HW_EMPTY ) ) {
  #endif
                inf->cclass |= SPECIAL_RETURN;
            }
            HW_CAsgn( inf->streturn, HW_EMPTY );
            inf->save = ifunc->save;
            inf->objname = WatcallInfo.objname; /* 26-jan-93 */
            inf->use = 1;
        }
#endif
    }
    return( inf );
}

aux_info *FindInfo( SYM_ENTRY *sym, SYM_HANDLE sym_handle )
{
    SYM_ENTRY       sym_typedef;
    aux_entry       *ent;
    TYPEPTR         typ;
    aux_info        *inf;

    inf = &DefaultInfo;         /* assume default */
    if( sym_handle == 0 )
        return( inf );

    SymGet( sym, sym_handle );
#if _CPU == 386
    if( (sym_handle == SymSTOSB) || (sym_handle == SymSTOSD) ) {
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
    if( !(sym->flags & SYM_TEMP) ) {
        /* not an indirect func call*/
        inf = InfoLookup( sym );
    }
    if( inf == &DefaultInfo ) {
        typ = SkipDummyTypedef( sym->sym_type );
        if( typ->decl_type == TYPE_TYPEDEF ) {
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
    if( (inf->flags & AUX_FLAG_FAR16) || (sym->mods & FLAG_FAR16) ) {
        if( (sym->mods & MASK_LANGUAGES) == LANG_PASCAL || (inf->cclass & REVERSE_PARMS) ) {
            return( &Far16PascalInfo );
        } else {
            return( &Far16CdeclInfo );
        }
    }
#endif
    return( inf );
}

int FunctionAborts( SYM_ENTRY *sym, SYM_HANDLE sym_handle )  /* 09-apr-93 */
{
    aux_entry    *ent;

    if( sym_handle != 0 ) {              /* 19-apr-93 */
        SymGet( sym, sym_handle );
        ent = AuxLookup( SymName( sym, sym_handle ) );
        if( ent != NULL ) {
            if( ent->info->cclass & SUICIDAL ) {
                return( 1 );
            }
        }
    }
    return( 0 );
}

call_class GetCallClass( SYM_HANDLE sym_handle )
{
    aux_info            *inf;
    SYM_ENTRY           sym;
    call_class          cclass;

    cclass = DefaultInfo.cclass;
    if( sym_handle != 0 ) {
        inf = FindInfo( &sym, sym_handle );
        if( sym.flags & SYM_FUNCTION ) {
            if( inf != &DefaultInfo ) {
                cclass = inf->cclass;
            } else {
                cclass = GetLangInfo( sym.mods )->cclass;
#if _CPU == 8086
                if( TargSys == TS_WINDOWS ) {
                    if( sym.mods & (LANG_CDECL | LANG_PASCAL) ) {
                        cclass |= FAT_WINDOWS_PROLOG;
                    }
                }
#endif
            }
#if ( _CPU == 8086 ) || ( _CPU == 386 )
            if( CompFlags.emit_names ) {
                cclass |= EMIT_FUNCTION_NAME;
            }
            if( sym.mods & FLAG_FAR ) {
                cclass |= FAR_CALL;
                if( sym.mods & FLAG_NEAR ) {
                    cclass |= INTERRUPT;
                }
            } else if( sym.mods & FLAG_NEAR ) {
                cclass &= ~ FAR_CALL;
            }
#endif
#ifdef DLL_EXPORT
            if( sym.mods & FLAG_EXPORT ) {  /* 12-mar-90 */
                cclass |= DLL_EXPORT;
            }
#endif
#ifdef LOAD_DS_ON_ENTRY
            if( sym.mods & FLAG_LOADDS ) {  /* 26-apr-90 */
  #if 0 /* John - 11-mar-93 */          /* 21-feb-93 */
                if( TargSys == TS_WINDOWS ) {
                    cclass |= FAT_WINDOWS_PROLOG;
                } else {
                    cclass |= LOAD_DS_ON_ENTRY;
                }
  #else
                cclass |= LOAD_DS_ON_ENTRY;
  #endif
            }
#endif
#ifdef MAKE_CALL_INLINE
            if( IsInLineFunc( sym_handle ) ) {
                cclass |= MAKE_CALL_INLINE;
            }
#endif
            if( VarFunc( &sym ) ) {
                cclass |= CALLER_POPS | HAS_VARARGS;
            }
        }
#if ( _CPU == 8086 ) || ( _CPU == 386 )
        if( sym.flags & SYM_FUNC_NEEDS_THUNK ) {
            cclass |= THUNK_PROLOG;
        }
#endif
    }
#ifdef REVERSE
    cclass &= ~ REVERSE_PARMS;               /* 28-may-89 */
#endif
#ifdef PROLOG_HOOKS
    if( CompFlags.ep_switch_used != 0 ) {
        cclass |= PROLOG_HOOKS;
    }
#endif
#ifdef EPILOG_HOOKS
    if( CompFlags.ee_switch_used != 0 ) {
        cclass |= EPILOG_HOOKS;
    }
#endif
#ifdef GROW_STACK
    if( CompFlags.sg_switch_used ) {
        cclass |= GROW_STACK;
    }
#endif
#ifdef TOUCH_STACK
    if( CompFlags.st_switch_used ) {
        cclass |= TOUCH_STACK;
    }
#endif
    return( cclass );
}

static time_t *getFileDepTimeStamp( FNAMEPTR flist )
{
    static time_t            stamp;

#if _RISC_CPU || COMP_CFG_COFF
    stamp = flist->mtime;
#else
    /* OMF format */
    stamp = _timet2dos( flist->mtime );
#endif
    return( &stamp );
}

/*
//    NextLibrary
//        Called (indirectly) from the code generator to inject automagically defined symbols.
//    Inputs:
//        index    (n-1)
//            Usually called from a loop until we return 0/NULL to show no more libraries
//        request
//            NEXT_LIBRARY
//                examines the current flags to see if any libraries should be
//                automagically referenced and returns the relevant index if so.
//            LIBRARY_NAME
//                returns the requested name.
//
*/
static void addDefaultLibs( void )
{
    if( CompFlags.emit_library_names ) {
        if( _HAS_ANY_MAIN || CompFlags.pragma_library || CompFlags.emit_all_default_libs ) {
            AddLibraryName( CLIB_Name + 1, CLIB_Name[0] );
        }
        AddLibraryName( MATHLIB_Name + 1, MATHLIB_Name[0] );
        if( EmuLib_Name != NULL ) {
            AddLibraryName( EmuLib_Name + 1, EmuLib_Name[0] );
        }
    }
}

static VOIDPTR NextLibrary( int index, aux_class request )
{
    library_list    *lib;
    char            *name;
    int             i;

    name = NULL;
    if( index == 0 ) {
        addDefaultLibs();
    }
    if( request == NEXT_LIBRARY )
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
    /* return library name, or */
    if( request == LIBRARY_NAME || name == NULL )
        return( name );
    /* library index */
    return( (VOIDPTR)(pointer_int)index );
}

//    NextAlias
//        Called (indirectly) from the code generator to go through the list of
//        linker aliases.
//    Inputs:
//        index    (n-1)
//            Called from a loop until we return 0/NULL to show no more aliases
//        request
//            NEXT_ALIAS
//                returns the index of next alias in the list, or zero if none.
//            ALIAS_NAME
//                returns the alias name, or NULL if alias refers to a symbol.
//            ALIAS_SYMBOL
//                returns the alias symbol, or NULL if alias refers to a name.
//            ALIAS_SUBST_NAME
//                returns the name to be substituted for the alias, or NULL.
//            ALIAS_SUBST_SYMBOL
//                returns the symbol to be substituted for the alias, or NULL.
//
// Note: One of ALIAS..._NAME and ALIAS..._SYMBOL will always be 0/NULL and the other
// will be valid, depending on which form of the pragma was used.
static VOIDPTR NextAlias( int index, aux_class request )
{
    alias_list          *aliaslist;
    SYM_HANDLE          alias_sym = NULL;
    SYM_HANDLE          subst_sym = NULL;
    const char          *alias_name = NULL;
    const char          *subst_name = NULL;
    int                 i;

    if( request == NEXT_ALIAS )
        ++index;

    for( i = 1, aliaslist = AliasHead; aliaslist; aliaslist = aliaslist->next, ++i ) {
        alias_name = aliaslist->name;
        alias_sym  = aliaslist->a_sym;
        subst_name = aliaslist->subst;
        subst_sym  = aliaslist->s_sym;
        if( i == index ) {
            break;
        }
    }
    if( aliaslist == NULL )
        index = 0;          /* no (more) aliases */

    if( request == ALIAS_NAME ) {
        return( (VOIDPTR)alias_name );
    } else if( request == ALIAS_SYMBOL ) {
        return( (VOIDPTR)alias_sym );
    } else if( request == ALIAS_SUBST_NAME ) {
        return( (VOIDPTR)subst_name );
    } else if( request == ALIAS_SUBST_SYMBOL ) {
        return( (VOIDPTR)subst_sym );
    } else {    // this had better be a NEXT_ALIAS request
        return( (VOIDPTR)(pointer_int)index );
    }
}

/* Return the size of function parameters or -1 if size could
 * not be determined (symbol isn't a function or is variadic)
 */
static int GetParmsSize( CGSYM_HANDLE sym_handle )
{
    int         total_parm_size = 0;
    int         parm_size;
    TYPEPTR     fn_typ;
    TYPEPTR     *parm;
    TYPEPTR     typ;
    SYM_ENTRY   sym;

    SymGet( &sym, sym_handle );
    fn_typ = sym.sym_type;
    SKIP_TYPEDEFS( fn_typ );
    if( fn_typ->decl_type == TYPE_FUNCTION ) {
        parm = fn_typ->u.fn.parms;
        if( parm != NULL ) {
            for( ; (typ = *parm) != NULL; ++parm ) {
                if( typ->decl_type == TYPE_DOT_DOT_DOT ) {
                    total_parm_size = -1;
                    break;
                }

                SKIP_TYPEDEFS( typ );
                if( typ->decl_type == TYPE_VOID )
                    break;

                parm_size = _RoundUp( TypeSize( typ ), TARGET_INT );
                total_parm_size += parm_size;
            }
        }
    } else {
        total_parm_size = -1;
    }
    return( total_parm_size );
}

/*
//    Return name pattern manipulator string
*/
static char *GetNamePattern( CGSYM_HANDLE sym_handle )
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
        inf = LangInfo( sym.mods, inf );
        if( sym.flags & SYM_FUNCTION ) {
            pattern = inf->objname;
#if ( _CPU == 386 ) || ( _CPU == 8086 )
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
    }       // close that else
#endif
    return( pattern );
}

static char *GetBaseName( CGSYM_HANDLE sym_handle )
{
    SYM_ENTRY            sym;

    SymGet( &sym, sym_handle );
    return( sym.name );
}

char *FEExtName( CGSYM_HANDLE sym_handle, int request )
/*****************************************************/
{
    switch( request ) {
    case EXTN_BASENAME:
        return( GetBaseName( sym_handle ) );
    case EXTN_PATTERN:
        return( GetNamePattern( sym_handle ) );
    case EXTN_PRMSIZE:
        return( (char *)(pointer_int)GetParmsSize( sym_handle ) );
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
        if( (control & CM_WINMAIN) && CompFlags.has_winmain || (TargetSwitches & WINDOWS) && CompFlags.has_main ) {
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
#if ( _CPU == 8086 ) || ( _CPU == 386 )
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
            if( TargetSwitches & BIG_CODE ) {
                AddExtRefN( "_big_code_" );
            } else {
                AddExtRefN( "_small_code_" );
            }
        }
  #endif
        if( CompFlags.pgm_used_8087 || CompFlags.float_used ) {
            if( GET_FPU( ProcRevision ) & FPU_EMU ) {
  #if _CPU == 8086
                AddExtRefN( "__init_87_emulator" );
  #else
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
#else
    if( CompFlags.emit_library_names ) {
        /* handle floating-point support */
        if( CompFlags.float_used ) {
            AddExtRefN( "_fltused_" );
        }
    }
#endif
#if ( _CPU == 8086 ) || ( _CPU == 386 )
    if( CompFlags.main_has_parms ) {
  #if _CPU == 8086
        if( CompFlags.has_wchar_entry ) {
            AddExtRefN( "__wargc" );
        } else {
            AddExtRefN( "__argc" );
        }
  #else
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
#else
    if( CompFlags.main_has_parms ) {
        AddExtRefN( "_argc" );
    }
#endif
    /* handle default windowing app */
    if( CompFlags.bw_switch_used ) {
        AddExtRefN( "__init_default_win" );
    }
#if ( _CPU == 8086 ) || ( _CPU == 386 )
    /* handle NetWare */
    if( TargSys == TS_NETWARE || TargSys == TS_NETWARE5 ) {
        /* is target NETWARE or NETWARE5? */
        AddExtRefN( "__WATCOM_Prelude" );
    }

    /* handle 'old' profiling */
    if( TargetSwitches & P5_PROFILING ) {
        /* is profiling enabled (-et)? */
        AddExtRefN( "__p5_profile" );
    }

    /* handle 'new' profiling */
    if( TargetSwitches & NEW_P5_PROFILING ) {
        /* is profiling enabled (-etp)? */
        AddExtRefN( "__new_p5_profile" );
    }
#endif
}


/*
//    NextImport
//        Called (indirectly) from the code generator to inject automagically defined symbols.
//    Inputs:
//        index    (n-1)
//            Usually called from a loop until we return 0/NULL to show no more symbols
//            are required.
//        request
//            NEXT_IMPORT
//                examines the current flags to see if any symbols should be
//                automagically inserted and returns the relevant index if so.
//            IMPORT_NAME
//                returns the requested name. if we have returned an index for
//                the current compiler settings we should be called with a valid
//                index but we still perform exactly the same checks as this is
//                good practise.
//
*/

static VOIDPTR NextImport( int index, aux_class request )
/*******************************************************/
{
    char        *name;
    int         i;
    extref_info *e;

    if( !CompFlags.emit_targimp_symbols )
        return (NULL);

    name = NULL;
    if( index == 0 ) {
        addDefaultImports();
    }
    if( request == NEXT_IMPORT )
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
    /* return the import name, or */
    if( request == IMPORT_NAME || name == NULL )
        return( name );
    /* return the index */
    return( (char *)(pointer_int)index );
}

static VOIDPTR NextImportS( int index, aux_class request )
/********************************************************/
{
    void        *symbol;
    int         i;
    extref_info *e;

    if(!CompFlags.emit_targimp_symbols)
        return (NULL);

    symbol = NULL;
    if( request == NEXT_IMPORT_S )
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
    /* return the import symbol, or */
    if( request == IMPORT_NAME_S || symbol == NULL )
        return( symbol );
    /* return the index */
    return( (char *)(pointer_int)index );
}

#if ( _CPU == 8086 ) || ( _CPU == 386 )

/*
//    This section is for
//        8086
//        386
//
//    pass auxiliary information to back end
*/
VOIDPTR FEAuxInfo( CGSYM_HANDLE cgsym_handle, int request )
{
    SYM_HANDLE           sym_handle = cgsym_handle;
    aux_info             *inf;
    auto SYM_ENTRY       sym;
    static hw_reg_set    save_set;

    switch( request ) {
    case SOURCE_LANGUAGE:
        return( "C" );
    case STACK_SIZE_8087:
        return( (VOIDPTR)(pointer_int)Stack87 );
    case CODE_GROUP:
        return( (VOIDPTR)GenCodeGroup );
    case DATA_GROUP:
        return( (VOIDPTR)DataSegName );
    case OBJECT_FILE_NAME:
        return( (VOIDPTR)ObjFileName() );
    case REVISION_NUMBER:
        return( (VOIDPTR)(pointer_int)II_REVISION );
    case AUX_LOOKUP:
        return( (VOIDPTR)sym_handle );
    case PROEPI_DATA_SIZE:
        return( (VOIDPTR)(pointer_int)ProEpiDataSize );
    case DBG_PREDEF_SYM:
        return( (VOIDPTR)SymDFAbbr );
    case P5_CHIP_BUG_SYM:
        return( (VOIDPTR)SymChipBug ); /* 09-dec-94 */
    case CODE_LABEL_ALIGNMENT:
        {
            static  unsigned char   Alignment[] = { 2, 1, 1 };

            if( OptSize == 0 )
                Alignment[1] = TARGET_INT;

            return( Alignment );
        }
    case CLASS_NAME:
        return( SegClassName( (segment_id)(pointer_int)sym_handle ) );
    case USED_8087:
        CompFlags.pgm_used_8087 = 1;
        return( NULL );
  #if _CPU == 386
    case P5_PROF_DATA:
        return( (VOIDPTR)FunctionProfileBlock );
    case P5_PROF_SEG:
        return( (VOIDPTR)(pointer_int)FunctionProfileSegment );
  #endif
    case SOURCE_NAME:
        if( SrcFName == ModuleName ) {
            return( FNameFullPath( FNames ) );
        } else {
            return( ModuleName );
        }
    case CALL_CLASS:
        {
            static call_class cclass;

            cclass = GetCallClass( sym_handle );
            return( &cclass );
        }
    case FREE_SEGMENT:
        return( NULL );
    case NEXT_LIBRARY:
    case LIBRARY_NAME:
        return( NextLibrary( (int)(pointer_int)sym_handle, request ) );
    case NEXT_IMPORT:
    case IMPORT_NAME:
        return( NextImport( (int)(pointer_int)sym_handle, request ) );
    case NEXT_IMPORT_S:
    case IMPORT_NAME_S:
        return( NextImportS( (int)(pointer_int)sym_handle, request ) );
    case NEXT_ALIAS:
    case ALIAS_NAME:
    case ALIAS_SYMBOL:
    case ALIAS_SUBST_NAME:
    case ALIAS_SUBST_SYMBOL:
        return( NextAlias( (int)(pointer_int)sym_handle, request ) );
    case TEMP_LOC_NAME:
        return( (char *)TEMP_LOC_QUIT );
    case TEMP_LOC_TELL:
        return( NULL );
    case NEXT_DEPENDENCY:                               /* 03-dec-92 */
        if( CompFlags.emit_dependencies )
            return( NextDependency( (FNAMEPTR)cgsym_handle ) );
        return( NULL );
    case DEPENDENCY_TIMESTAMP:
        return( getFileDepTimeStamp( (FNAMEPTR)cgsym_handle ) );
    case DEPENDENCY_NAME:
        return( FNameFullPath( (FNAMEPTR)cgsym_handle ) );
    case PEGGED_REGISTER:
        return( SegPeggedReg( (segment_id)(pointer_int)cgsym_handle ) );
    default:
        break;
    }

    inf = FindInfo( &sym, sym_handle );
    switch( request ) {
    case SAVE_REGS:
        if( sym_handle != 0 ) {
            inf = LangInfo( sym.mods, inf );
        } else {
            sym.mods = 0;
        }
        save_set = inf->save;
        if( sym.mods & FLAG_SAVEREGS ) {
            HW_CTurnOn( save_set, HW_SEGS );
        }

  #ifdef __SEH__
        if( sym_handle == SymTryInit ) {
            HW_CTurnOff( save_set, HW_SP );
        }
  #endif
        return( &save_set );
    case RETURN_REG:
        if( sym_handle != 0 ) {
            inf = LangInfo( sym.mods, inf );
        }
        return( &inf->returns );
    case CALL_BYTES:
        return( inf->code );
    case PARM_REGS:
  #ifdef __SEH__
        if(( sym_handle == SymTryInit )
          || ( sym_handle == SymTryFini )
          || ( sym_handle == SymTryUnwind )
          || ( sym_handle == SymExcept )) {
            return( TryParms );
        }
  #endif
        if( sym_handle != 0 ) {
            inf = LangInfo( sym.mods, inf );
            if( inf->code == NULL && VarFunc( &sym ) ) {
                return( DefaultVarParms );
            }
        }
        return( inf->parms );
    case STRETURN_REG:
        if( sym_handle != 0 ) {
            inf = LangInfo( sym.mods, inf );
        }
        return( &inf->streturn );
    default:
        break;
    }
    return( NULL );
}

#else

/*
//    This section is NOT 8086 and 386 , i.e.,
//        _AXP
//        _PPC
//        _MIPS
//
//    pass auxiliary information to back end
*/
VOIDPTR FEAuxInfo( CGSYM_HANDLE cgsym_handle, int request )
{
    SYM_HANDLE              sym_handle = cgsym_handle;
    aux_info                *inf;
    auto SYM_ENTRY          sym;
    static hw_reg_set       save_set;

    switch( request ) {
    case SOURCE_LANGUAGE:
        return( "C" );
    case OBJECT_FILE_NAME:
        return( (VOIDPTR)ObjFileName() );
    case REVISION_NUMBER:
        return( (VOIDPTR)(pointer_int)II_REVISION );
    case AUX_LOOKUP:
        return( (VOIDPTR)(pointer_int)sym_handle );
    case SOURCE_NAME:
        if( SrcFName == ModuleName ) {
            return( FNameFullPath( FNames ) );
        } else {
            return( ModuleName );
        }
    case CALL_CLASS:
        {
            static call_class cclass;

            cclass = GetCallClass( sym_handle );
            return( &cclass );
        }
    case NEXT_LIBRARY:
    case LIBRARY_NAME:
        return( NextLibrary( (int)(pointer_int)sym_handle, request ) );
    case NEXT_IMPORT:
    case IMPORT_NAME:
        return( NextImport( (int)(pointer_int)sym_handle, request ) );
    case NEXT_IMPORT_S:
    case IMPORT_NAME_S:
        return( NextImportS( (int)(pointer_int)sym_handle, request ) );
    case NEXT_ALIAS:
    case ALIAS_NAME:
    case ALIAS_SYMBOL:
    case ALIAS_SUBST_NAME:
    case ALIAS_SUBST_SYMBOL:
        return( NextAlias( (int)(pointer_int)sym_handle, request ) );
    case FREE_SEGMENT:
        return( NULL );
    case TEMP_LOC_NAME:
        return( (char *)TEMP_LOC_QUIT );
    case TEMP_LOC_TELL:
        return( NULL );
    case NEXT_DEPENDENCY:                               /* 03-dec-92 */
        if( CompFlags.emit_dependencies )
            return( NextDependency( (FNAMEPTR) cgsym_handle ) );
        return( NULL );
    case DEPENDENCY_TIMESTAMP:
        return( getFileDepTimeStamp( (FNAMEPTR)cgsym_handle ) );
    case DEPENDENCY_NAME:
        return( FNameFullPath( (FNAMEPTR)cgsym_handle ) );
    default:
        break;
    }

    inf = FindInfo( &sym, sym_handle );
    switch( request ) {
    case SAVE_REGS:
        if( sym_handle != 0 ) {
            inf = LangInfo( sym.mods, inf );
        } else {
            sym.mods = 0;
        }
        save_set = inf->save;
        return( &save_set );
    case RETURN_REG:
        if( sym_handle != 0 ) {
            inf = LangInfo( sym.mods, inf );
        }
        return( &inf->returns );
    case CALL_BYTES:
        return( inf->code );
    case PARM_REGS:
        if( sym_handle != 0 ) {
            inf = LangInfo( sym.mods, inf );
            if( inf->code == NULL && VarFunc( &sym ) ) {
                return( DefaultVarParms );
            }
        }
        return( inf->parms );
    default:
        break;
    }
    return( NULL );
}
#endif

char *SrcFullPath( char const *name, char *buff, unsigned max )
{
    return( _getFilenameFullPath( buff, name, max ) );
}
