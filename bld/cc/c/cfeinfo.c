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
#include "iopath.h"
#include "compcfg.h"
#include <sys/stat.h>
#include "autodept.h"
#include "langenv.h"
#include "feprotos.h"

#define TRUNC_SYMBOL_HASH_LEN        4
#define TRUNC_SYMBOL_LEN_WARN        120

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
            for( ; (typ = *parm); ++parm ) {
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
    int         len;
    char        *p;

    if( sym == NULL )
        return( 0 );

    if( sym->flags & SYM_FUNCTION ) {
        p = sym->name;
        len = strlen( p );
        hash = (len + VarFuncWeights[ p[0] - 'a' ]
                 + VarFuncWeights[ p[len-1] -'a' ]) & 31;

        if( strcmp( p, VarParmFuncs[ hash ] ) == 0 
            && ( CompFlags.extensions_enabled
                 || ( ( 1 << hash ) & VAR_PARM_FUNCS_ANSI ) ) )
            return( 1 );

        return( VarParm( sym ) );
    }
    return( 0 );
}

#if ( _CPU == 8086 ) || ( _CPU == 386 )

static struct inline_funcs *Flat( struct inline_funcs *ifunc )
{
  #if _CPU == 386
    extern byte_seq *    FlatAlternates[];
    byte_seq **            p;

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

struct inline_funcs *IF_Lookup( char *name )
{
    struct inline_funcs     *ifunc;

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

struct aux_info *GetLangInfo( type_modifiers flags )
{
    switch( flags & FLAG_LANGUAGES ) {
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
static struct aux_info *LangInfo( type_modifiers flags, struct aux_info *inf )
{
    if( inf != &DefaultInfo )
        return( inf );

    return( GetLangInfo( flags ) );
}

int ParmsToBeReversed( int flags, struct aux_info *inf )
{
#ifdef REVERSE
    inf = LangInfo( flags, inf );
    if( inf != NULL ) {
        if( inf->cclass & REVERSE_PARMS ) {
            return( 1 );
        }
    }
#else
    flags = flags;
    inf = inf;
#endif
    return( 0 );
}

struct aux_info *InfoLookup( SYMPTR sym )
{
    char                  *name;
    struct aux_info       *inf;
    struct aux_entry      *ent;

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
            struct inline_funcs     *ifunc;

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
            inf->cclass = (WatcallInfo.cclass & FAR) | MODIFY_EXACT;
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

struct aux_info *FindInfo( SYM_ENTRY *sym, SYM_HANDLE sym_handle )
{
    SYM_ENTRY           sym_typedef;
    struct aux_entry    *ent;
    TYPEPTR             typ;
    struct aux_info     *inf;

    inf = &DefaultInfo;         /* assume default */
    if( sym_handle == 0 )
        return( inf );

    SymGet( sym, sym_handle );
#if _CPU == 386
    if( (sym_handle == SymSTOSB) || (sym_handle == SymSTOSD) ) {
        return( &STOSBInfo );
    } else if( sym_handle == SymFinally ) {
        static byte_seq FinallyCode = { 1, { 0xc3 } };  /* ret */

        InlineInfo = WatcallInfo;
        InlineInfo.code = &FinallyCode;
        return( &InlineInfo );
    } else if( sym_handle == SymTryFini ) {
        static hw_reg_set TryFiniParms[] = {
            HW_D( HW_EAX ),
            HW_D( HW_EMPTY )
        };
        static byte_seq TryFiniCode = {
            6, { 0x64, 0xA3, 0, 0, 0, 0 }
        };  /* mov fs:[0],eax */

        InlineInfo = WatcallInfo;
        InlineInfo.parms = TryFiniParms;
        InlineInfo.code = &TryFiniCode;
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
    if( ( inf->flags & AUX_FLAG_FAR16 )
      || ( sym->attrib & FLAG_FAR16 ) ) {
        if( ( (sym->attrib & FLAG_LANGUAGES) == LANG_PASCAL )
          || ( inf->cclass & REVERSE_PARMS ) ) {
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
    struct aux_entry    *ent;

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

void GetCallClass( SYM_HANDLE sym_handle )
{
    struct aux_info     *inf;
    SYM_ENTRY           sym;

    CallClass = DefaultInfo.cclass;
    if( sym_handle != 0 ) {
        inf = FindInfo( &sym, sym_handle );
        if( sym.flags & SYM_FUNCTION ) {
            if( inf != &DefaultInfo ) {
                CallClass = inf->cclass;
            } else {
                CallClass = GetLangInfo( sym.attrib )->cclass;
#if _CPU == 8086
                if( TargSys == TS_WINDOWS ) {
                    if( sym.attrib & (LANG_CDECL | LANG_PASCAL) ) {
                        CallClass |= FAT_WINDOWS_PROLOG;
                    }
                }
#endif
            }
#if ( _CPU == 8086 ) || ( _CPU == 386 )
            if( CompFlags.emit_names ) {
                CallClass |= EMIT_FUNCTION_NAME;
            }
            if( sym.attrib & FLAG_FAR ) {
                CallClass |= FAR;
                if( sym.attrib & FLAG_NEAR ) {
                    CallClass |= INTERRUPT;
                }
            } else if( sym.attrib & FLAG_NEAR ) {
                CallClass &= ~ FAR;
            }
#endif
#ifdef DLL_EXPORT
            if( sym.attrib & FLAG_EXPORT ) {  /* 12-mar-90 */
                CallClass |= DLL_EXPORT;
            }
#endif
#ifdef LOAD_DS_ON_ENTRY
            if( sym.attrib & FLAG_LOADDS ) {  /* 26-apr-90 */
  #if 0 /* John - 11-mar-93 */          /* 21-feb-93 */
                if( TargSys == TS_WINDOWS ) {
                    CallClass |= FAT_WINDOWS_PROLOG;
                } else {
                    CallClass |= LOAD_DS_ON_ENTRY;
                }
  #else
                CallClass |= LOAD_DS_ON_ENTRY;
  #endif
            }
#endif
#ifdef MAKE_CALL_INLINE
            if( IsInLineFunc( sym_handle ) ) {
                CallClass |= MAKE_CALL_INLINE;
            }
#endif
            if( VarFunc( &sym ) ) {
                CallClass |= CALLER_POPS | HAS_VARARGS;
            }
        }
    }
#ifdef REVERSE
    CallClass &= ~ REVERSE_PARMS;               /* 28-may-89 */
#endif
#if ( _CPU == 8086 ) || ( _CPU == 386 )
    if( sym_handle != 0 && sym.flags & SYM_FUNC_NEEDS_THUNK ) {
        CallClass |= THUNK_PROLOG;
    }
#endif
#ifdef PROLOG_HOOKS
    if( CompFlags.ep_switch_used != 0 ) {
        CallClass |= PROLOG_HOOKS;
    }
#endif
#ifdef EPILOG_HOOKS
    if( CompFlags.ee_switch_used != 0 ) {
        CallClass |= EPILOG_HOOKS;
    }
#endif
#ifdef GROW_STACK
    if( CompFlags.sg_switch_used ) {
        CallClass |= GROW_STACK;
    }
#endif
#ifdef TOUCH_STACK
    if( CompFlags.st_switch_used ) {
        CallClass |= TOUCH_STACK;
    }
#endif
}

static time_t *getFileDepTimeStamp( FNAMEPTR flist )
{
    static time_t            stamp;

#if ( ( _CPU == 8086 ) || ( _CPU == 386 ) ) && ( COMP_CFG_COFF == 0 )
    stamp = _timet2dos( flist->mtime );
#else
    stamp = flist->mtime;
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
static VOIDPTR NextLibrary( int index, aux_class request )
{
    struct library_list *liblist;
    char                *name = NULL;
    int                 i;

    i = 0;
    if( request == NEXT_LIBRARY )
        ++index;

    for( liblist = HeadLibs; liblist; liblist = liblist->next ) {
        name = &liblist->prio;
        ++i;
        if( i == index ) {
            break;
        }
    }
    if( liblist == NULL ) {
        switch( index - i ) {
        case 1: /* return 1 for CLIB */
            name = CLIB_Name;
            if( CompFlags.emit_library_any )
                break;
            if( CompFlags.emit_library_with_main ) {
                if( CompFlags.has_main )
                    break;
                if( CompFlags.has_winmain )
                    break;
                if( CompFlags.bd_switch_used )
                    break;
                if( CompFlags.has_libmain )
                    break;
                if( CompFlags.bm_switch_used )
                    break;  /* JBS */
                ++index;
            } else {
                name = NULL;
                index = 0;              // indicate all done
            }
            break;
        /*
        //    MATHLIB is always referenced as a default library because
        //    the linker wont include anything without a 'real' referenced
        //    symbol
        */
        case 2: /* return 2 for MATHLIB */
            name = MATHLIB_Name;
            break;
        case 3: /* return 3 for EMULIB */
            name = EmuLib_Name;
            if( EmuLib_Name != NULL )
                break;
            // fall through
        case 4: /* used to be PCODE */
            name = NULL;
            index = 0;                  // indicate all done
            break;
        default:
            break;
        }
    }
    /*
    //    return library name, or
    */
    if( request == LIBRARY_NAME )
        return( name );
    /*
    //    library index
    */
    return( (char *)index );
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
//            ALIAS_SUBSTITUTE
//                returns the name to be substituted for the alias.
//
// Note: One of ALIAS_NAME and ALIAS_SYMBOL will always be 0/NULL and the other
// will be valid, depending on which form of the pragma was used.
static VOIDPTR NextAlias( int index, aux_class request )
{
    struct alias_list   *aliaslist;
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
        return( (VOIDPTR)index );
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
            for( ; (typ = *parm); ++parm ) {
                if( typ->decl_type == TYPE_DOT_DOT_DOT ) {
                    total_parm_size = -1;
                    break;
                }

                SKIP_TYPEDEFS( typ );
                if( typ->decl_type == TYPE_VOID )
                    break;

                parm_size = TypeSize( typ );
                parm_size = (parm_size + sizeof( target_int ) - 1)
                            & -sizeof( target_int );
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
    struct aux_info      *inf;

    inf = FindInfo( &sym, sym_handle );
#ifdef __SEH__
    if(( sym_handle == SymTryInit )
      || ( sym_handle == SymTryFini )
      || ( sym_handle == SymTryUnwind )
      || ( sym_handle == SymExcept )) {
        pattern = "*";
    } else {
#endif
        inf = LangInfo( sym.attrib, inf );
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

extern char *FEExtName( CGSYM_HANDLE sym_handle, int request )
/************************************************************/
{
    switch( request ) {
    case EXTN_BASENAME:
        return( GetBaseName( sym_handle ) );
    case EXTN_PATTERN:
        return( GetNamePattern( sym_handle ) );
    case EXTN_PRMSIZE:
        return( (char *)GetParmsSize( sym_handle ) );
    default:
        return( NULL );
    }
}

#if ( _CPU == 8086 ) || ( _CPU == 386 )
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
{
    char        *name;

    if(!CompFlags.emit_targimp_symbols)
        return (NULL);

    if( request == NEXT_IMPORT )
        ++index;

    switch( index ) {
    /* handle entry points */
    case 1:
        /* wide char or MBCS entry */
        if( CompFlags.has_wchar_entry ) {
            name = "__DLLstartw_";
        } else {
            name = "__DLLstart_";
        }
        if( CompFlags.bd_switch_used ) /* build target == DLL ? */
            break;
        if( CompFlags.has_libmain ) {  /* object has defined symbol (w)LibMain/(w)DllMain */
            /* build target == console or gui application ? */
            if( !(CompFlags.bc_switch_used || CompFlags.bg_switch_used ) ) {
                break;
            }
        }
        /* wide char or MBCS entry */
        if( CompFlags.has_wchar_entry ) {
            name = "_wstartw_";
        } else {
            name = "_wstart_";
        }
        /* symbol (w)WinMain defined */
        if( CompFlags.has_winmain ) {
            /* gui application */
            if( CompFlags.bg_switch_used )
                break;
            /* target == DLL or target == console*/
            if( !(CompFlags.bd_switch_used || CompFlags.bc_switch_used ) ) {
                break;
            }
        }
  #if _CPU == 8086
        /* is target windows AND symbol (w)main is defined */
        if(( TargetSwitches & WINDOWS ) && CompFlags.has_main )
            break;
  #endif
        /* wide char or MBCS entry */
        if( CompFlags.has_wchar_entry ) {
            name = "_cstartw_";
        } else {
            name = "_cstart_";
        }
        /* is symbol (w)main is defined ? */
        if( CompFlags.has_main ) {
            /* build target == console ? */
            if( CompFlags.bc_switch_used  )
                break;
            /* target == DLL or target = GUI app ? */
            if( !(CompFlags.bd_switch_used || CompFlags.bg_switch_used ) ) {
                break;
            }
        }
        ++index;

    /* handle floating point support */
    case 2:
        /* floating point used */
        name = "_fltused_";
        if( CompFlags.use_long_double ) {
            name = "_fltused_80bit_";
        }
        if( CompFlags.emit_library_with_main    /* emit default library info? */
          || CompFlags.emit_library_any ) {     /* -zlf emit all library info? */
                    /* 12-mar-90 */
            if( CompFlags.float_used ) {        /* has this object used any floating point code? */
                break;
            }
        }
        ++index;

    /* handle code model library support */
    case 3:
  #if _CPU == 8086
        name = "_small_code_";
        if( TargetSwitches & BIG_CODE ) {       /* big code model ? */
            name = "_big_code_";
        }
        if( CompFlags.emit_library_with_main    /* emit default library info? */
          || CompFlags.emit_library_any ) {     /* -zlf emit all library info? */
                    /* 12-mar-90 */
            if( FirstStmt != 0 ) {
                break;
            }
        }
  #endif
        ++index;

    /* handle floating point emulator */
    case 4:
        /* generating FPU instructions OR this object used floats ?*/
        if( CompFlags.pgm_used_8087  || CompFlags.float_used ) {
  #if _CPU == 386
            name = "__init_387_emulator";
  #else
            name = "__init_87_emulator";
  #endif
            if( GET_FPU( ProcRevision ) & FPU_EMU ) {   /* using emulated FPU code? */
                break;
            }
        }
        ++index;

    /*-----------------------------------------------------------------------
    //    handle FPU requirement
    -----------------------------------------------------------------------*/
    case 5:
        /* (emit default library info OR -zlf emit all library info) AND use backward compatible FPU code? */
        if( (CompFlags.emit_library_with_main || CompFlags.emit_library_any )
          &&  Stack87 == 4 ) {
            name = "__old_8087";
        } else {
            name = "__8087";
        }
        /* generating FPU instructions OR this object used floats? */
        if( CompFlags.pgm_used_8087 || CompFlags.float_used ) {
            if( GET_FPU(ProcRevision) > FPU_NONE ) {
                break;
            }
        }
        ++index;

    /* handle entry point arg passing */
    case 6:
        /* wide char or MBCS entry */
        if( CompFlags.has_wchar_entry ) {
            name = "__wargc";
        } else {
            name = "__argc";
        }
  #if _CPU == 386
        /* NOT using register convention arg passing? */
        if( ! CompFlags.register_conventions ) {
            ++name;  // change from __wargc , __argc to _wargc..
        }
  #endif
        /* does (w)main have any arguments (NOT int main(void)) */
        if( CompFlags.main_has_parms )
            break;
        ++index;

    /* handle default windowing app */
    case 7:
        /* is target default windowing application? */
        name = "__init_default_win";
        if( CompFlags.bw_switch_used )
            break;
        ++index;

    /* handle NetWare */
    case 8:
        /* is target NETWARE or NETWARE5? */
        name = "__WATCOM_Prelude";
        if( TargSys == TS_NETWARE )
            break;
        if( TargSys == TS_NETWARE5 )
            break;
        ++index;

    /* handle 'old' profiling */
    case 9:
        /* is profiling enabled (-et)? */
        name = "__p5_profile";
        if( TargetSwitches & P5_PROFILING ) {
            break;
        }
        ++index;

    /* handle 'new' profiling */
    case 10:
        /* is profiling enabled (-etp)? */
        name = "__new_p5_profile";
        if( TargetSwitches & NEW_P5_PROFILING ) {
            break;
        }

    /* unknown / fallthrough */
    default:
        index = 0;                              // indicate no more
        name = NULL;
        break;
    }

    /* return the import name, or */
    if( request == IMPORT_NAME )
        return( name );

    /* return the index */
    return( (char *)index );
}

static VOIDPTR NextImportS( int index, aux_class request )
{
    void                *symbol;
    int                 i;
    struct extref_info  *extref;

    if(!CompFlags.emit_targimp_symbols)
        return (NULL);

    if( request == NEXT_IMPORT_S )
        ++index;

    symbol = NULL;
    for( i = 1, extref = ExtrefInfo; extref != NULL; extref = extref->next, ++i ) {
        if( i == index ) {
            symbol = extref->symbol;
            break;
        }
    }
    if( request == IMPORT_NAME_S || symbol == NULL ) {
        return( symbol );
    } else {
        return( (char *)index );
    }
}

/*
//    pass auxiliary information to back end
*/
VOIDPTR FEAuxInfo( CGSYM_HANDLE cgsym_handle, int request )
{
    SYM_HANDLE           sym_handle = cgsym_handle;
    struct aux_info *    inf;
    auto SYM_ENTRY       sym;
    static hw_reg_set    save_set;

    switch( request ) {
    case SOURCE_LANGUAGE:
        return( "C" );
    case STACK_SIZE_8087:
        return( (VOIDPTR)(unsigned)Stack87 );
    case CODE_GROUP:
        return( (VOIDPTR)GenCodeGroup );
    case DATA_GROUP:
        return( (VOIDPTR)DataSegName );
    case OBJECT_FILE_NAME:
        return( (VOIDPTR)ObjFileName( OBJ_EXT ) );
    case REVISION_NUMBER:
        return( (VOIDPTR)II_REVISION );
    case AUX_LOOKUP:
        return( (VOIDPTR)sym_handle );
    case PROEPI_DATA_SIZE:
        return( (VOIDPTR)ProEpiDataSize );
    case DBG_PREDEF_SYM:
        return( (VOIDPTR)SymDFAbbr );
    case P5_CHIP_BUG_SYM:
        return( (VOIDPTR)SymChipBug ); /* 09-dec-94 */
    case CODE_LABEL_ALIGNMENT:
        {
            static unsigned char    Alignment[] = { 2, 1, 1 };

            if( OptSize == 0 )
                Alignment[1] = TARGET_INT;

            return( Alignment );
        }
    case CLASS_NAME:
        return( SegClassName( (unsigned)sym_handle ) );
    case USED_8087:
        CompFlags.pgm_used_8087 = 1;
        return( NULL );
  #if _CPU == 386
    case P5_PROF_DATA:
        return( (VOIDPTR)FunctionProfileBlock );
    case P5_PROF_SEG:
        return( (VOIDPTR)FunctionProfileSegment );
  #endif
    case SOURCE_NAME:
        if( SrcFName == ModuleName ) {
            return( FNameFullPath( FNames ) );
        } else {
            return( ModuleName );
        }
    case CALL_CLASS:
        GetCallClass( sym_handle );
        return( &CallClass );
    case FREE_SEGMENT:
        return( NULL );
    case NEXT_LIBRARY:
    case LIBRARY_NAME:
        return( NextLibrary( (int)sym_handle, request ) );
    case NEXT_IMPORT:
    case IMPORT_NAME:
        return( NextImport( (int)sym_handle, request ) );
    case NEXT_IMPORT_S:
    case IMPORT_NAME_S:
        return( NextImportS( (int)sym_handle, request ) );
    case NEXT_ALIAS:
    case ALIAS_NAME:
    case ALIAS_SYMBOL:
    case ALIAS_SUBST_NAME:
    case ALIAS_SUBST_SYMBOL:
        return( NextAlias( (int)sym_handle, request ) );
    case TEMP_LOC_NAME:
        return( (char *)TEMP_LOC_QUIT );
    case TEMP_LOC_TELL:
        return( NULL );
    case NEXT_DEPENDENCY:                               /* 03-dec-92 */
        if( CompFlags.emit_dependencies )
            return( NextDependency( (FNAMEPTR) cgsym_handle ) );
        return( NULL );
    case DEPENDENCY_TIMESTAMP:
        return( getFileDepTimeStamp( (FNAMEPTR) cgsym_handle ) );
    case DEPENDENCY_NAME:
        return( FNameFullPath( (FNAMEPTR) cgsym_handle ) );
    case PEGGED_REGISTER:
        return( SegPeggedReg( (unsigned)cgsym_handle ) );
    default:
        break;
    }

    inf = FindInfo( &sym, sym_handle );
    switch( request ) {
    case SAVE_REGS:
        if( sym_handle != 0 ) {
            inf = LangInfo( sym.attrib, inf );
        } else {
            sym.attrib = 0;
        }
        save_set = inf->save;
        if( sym.attrib & FLAG_SAVEREGS ) {
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
            inf = LangInfo( sym.attrib, inf );
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
            inf = LangInfo( sym.attrib, inf );
            if( inf->code == NULL && VarFunc( &sym ) ) {
                return( DefaultVarParms );
            }
        }
        return( inf->parms );
    case STRETURN_REG:
        if( sym_handle != 0 ) {
            inf = LangInfo( sym.attrib, inf );
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
{
    char        *name;

    if( request == NEXT_IMPORT )
        ++index;
    /*-----------------------------------------------------------------------
    //    handle entry points
    -----------------------------------------------------------------------*/
    switch( index ) {
    case 1:
        /* wide char or MBCS entry */
        if( CompFlags.has_wchar_entry ) {
            name = "__DLLstartw_";
        } else {
            name = "__DLLstart_";
        }
        /* object has defined symbol (w)LibMain/(w)DllMain  OR buildtarget == DLL*/
        if( CompFlags.has_libmain || CompFlags.bd_switch_used )
            break;

        /* wide char or MBCS entry */
        if( CompFlags.has_wchar_entry ) {
            name = "_wstartw_";
        } else {
            name = "_wstart_";
        }
        /* symbol (w)WinMain defined */
        if( CompFlags.has_winmain )
            break;
        /* wide char or MBCS entry */
        if( CompFlags.has_wchar_entry ) {
            name = "_cstartw_";
        } else {
            name = "_cstart_";
        }
        /* symbol (w)main defined */
        if( CompFlags.has_main )
            break;
        ++index;

    /* handle floating-point support */
    case 2:
        name = "_fltused_";
        /* emit default library info OR -zlf emit all library info */
        if( CompFlags.emit_library_with_main
          || CompFlags.emit_library_any ) {     /* 12-mar-90 */
            if( CompFlags.float_used ) {
                break;
            }
        }
        ++index;

    /* handle entry point arg passing */
    case 3:
        name = "_argc";
        /* does (w)main have any arguments (NOT int main(void)) */
        if( CompFlags.main_has_parms )
            break;
        ++index;

    /* handle default windowing app */
    case 4:
        /* is target default windowing application? */
        name = "__init_default_win";
        if( CompFlags.bw_switch_used )
            break;
        ++index;

    /* unknown / fallthrough */
    default:
        index = 0;                              // indicate no more
        name = NULL;
        break;
    }
    /* return the import name, or */
    if( request == IMPORT_NAME )
        return( name );

    /* return the index */
    return( (char *)index );
}

static VOIDPTR NextImportS( int index, aux_class request )
{
    void                *symbol;
    int                 i;
    struct extref_info  *extref;

    symbol = NULL;
    if( request == NEXT_IMPORT_S )
        ++index;

    for( i = 1, extref = ExtrefInfo; extref != NULL; extref = extref->next, ++i ) {
        if( i == index ) {
            symbol = extref->symbol;
            break;
        }
    }
    if( request == IMPORT_NAME_S || symbol == NULL ) {
        return( symbol );
    } else {
        return( (char *)index );
    }
}

/*
//    pass auxiliary information to back end
*/
VOIDPTR FEAuxInfo( CGSYM_HANDLE cgsym_handle, int request )
{
    SYM_HANDLE              sym_handle = cgsym_handle;
    struct aux_info         *inf;
    auto SYM_ENTRY          sym;
    static hw_reg_set       save_set;

    switch( request ) {
    case SOURCE_LANGUAGE:
        return( "C" );
    case OBJECT_FILE_NAME:
        return( (VOIDPTR)ObjFileName( OBJ_EXT ) );
    case REVISION_NUMBER:
        return( (VOIDPTR)II_REVISION );
    case AUX_LOOKUP:
        return( (VOIDPTR)sym_handle );
    case SOURCE_NAME:
        if( SrcFName == ModuleName ) {
            return( FNameFullPath( FNames ) );
        } else {
            return( ModuleName );
        }
    case CALL_CLASS:
        GetCallClass( sym_handle );
        return( &CallClass );
    case NEXT_LIBRARY:
    case LIBRARY_NAME:
        return( NextLibrary( (int)sym_handle, request ) );
    case NEXT_IMPORT:
    case IMPORT_NAME:
        return( NextImport( (int)sym_handle, request ) );
    case NEXT_IMPORT_S:
    case IMPORT_NAME_S:
        return( NextImportS( (int)sym_handle, request ) );
    case NEXT_ALIAS:
    case ALIAS_NAME:
    case ALIAS_SYMBOL:
    case ALIAS_SUBST_NAME:
    case ALIAS_SUBST_SYMBOL:
        return( NextAlias( (int)sym_handle, request ) );
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
            inf = LangInfo( sym.attrib, inf );
        } else {
            sym.attrib = 0;
        }
        save_set = inf->save;
        return( &save_set );
    case RETURN_REG:
        if( sym_handle != 0 ) {
            inf = LangInfo( sym.attrib, inf );
        }
        return( &inf->returns );
    case CALL_BYTES:
        return( inf->code );
    case PARM_REGS:
        if( sym_handle != 0 ) {
            inf = LangInfo( sym.attrib, inf );
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

extern char *SrcFullPath( char *buff, char const *name, unsigned max )
{
    return( _getFilenameFullPath( buff, name, max ) );
}
