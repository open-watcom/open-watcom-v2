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


#include <ctype.h>
#include "cvars.h"
#include "cg.h"
#include "cgswitch.h"
#include "pragdefn.h"
#include "pdefn2.h"
#include "iopath.h"
#include "compcfg.h"
#include <sys/stat.h>

#ifdef NEWCFE
extern  TREEPTR FirstStmt;
#endif

static unsigned char VarFuncWeights[] = {
//a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y,z
  0, 0,13, 0, 2, 1, 0, 0, 0, 0, 0,12, 0,14, 4,10, 0, 0, 6, 0, 0, 0, 0, 0, 0,0
};
static char *VarParmFuncs[] = {         /* functions with var parm lists */
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

#ifdef __SEH__
    #if _CPU == 386
        hw_reg_set TryParms[] = { HW_D_1( HW_EAX ), 0 };
    #else
        hw_reg_set TryParms[] = { HW_D( HW_EMPTY ), 0 };
    #endif
#endif

int VarParm( SYMPTR sym )
{
    TYPEPTR     *parm;
    TYPEPTR     typ;
    TYPEPTR     fn_typ;

    if( sym == NULL ) return( 0 );
    if( sym->flags & SYM_FUNCTION ) {
        fn_typ = sym->sym_type;
        while( fn_typ->decl_type == TYPE_TYPEDEF ) fn_typ = fn_typ->object;
        parm = fn_typ->u.parms;
        if( parm != NULL ) {
            for(; typ = *parm; ++parm ) {
                if( typ->decl_type == TYPE_DOT_DOT_DOT ) return( 1 );
            }
        }
    }
    return( 0 );
}


int VarFunc( SYMPTR sym )
{
    int         hash;
    int         len;
    char        *p;

    if( sym == NULL ) return( 0 );
    if( sym->flags & SYM_FUNCTION ) {
        p = sym->name;
        len = strlen( p );
        hash = (len + VarFuncWeights[ p[0] - 'a' ]
                 + VarFuncWeights[ p[len-1] -'a' ]) & 31;
        if( strcmp( p, VarParmFuncs[ hash ] ) == 0 ) return( 1 );
        return( VarParm( sym ) );
    }
    return( 0 );
}

#if _MACHINE == _PC

static struct inline_funcs __FAR *Flat( struct inline_funcs __FAR *ifunc )
{
    #if _CPU == 386
        extern byte_seq *FlatAlternates[];
        byte_seq **p;

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
#endif


#if _MACHINE == _PC
struct inline_funcs __FAR *IF_Lookup( char *name )
{
    struct inline_funcs __FAR *ifunc;

    if( GET_FPU( ProcRevision ) > FPU_NONE ) {
        ifunc = _8087_Functions;
        while( ifunc->name ) {
            if( strcmp( ifunc->name, name ) == 0 ) return( Flat( ifunc ) );
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
                ifunc = SBigData_Functions;
            #endif
        }
        while( ifunc->name ) {
            if( strcmp( ifunc->name, name ) == 0 ) return( Flat( ifunc ) );
            ++ifunc;
        }
    }
    #if _CPU == 386
        if( TargetSwitches & FLAT_MODEL ) {
            ifunc = Flat_Functions;
            while( ifunc->name ) {
                if( strcmp( ifunc->name, name ) == 0 ) return( ifunc );
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
            ifunc = BigData_Functions;
        #endif
    }
    while( ifunc->name ) {
        if( strcmp( ifunc->name, name ) == 0 ) return( Flat( ifunc ) );
        ++ifunc;
    }
    ifunc = Common_Functions;
    while( ifunc->name ) {
        if( strcmp( ifunc->name, name ) == 0 ) return( Flat( ifunc ) );
        ++ifunc;
    }
    return( NULL );
}
#endif


struct aux_info *LangInfo( int flags, struct aux_info *inf )
{
    if( inf != &DefaultInfo ) return( inf );
    switch( flags & FLAG_LANGUAGES ) {
    case LANG_CDECL:
        inf = &CdeclInfo;
        break;
    case LANG_PASCAL:
        inf = &PascalInfo;
        break;
    case LANG_FORTRAN:
        inf = &FortranInfo;
        break;
    case LANG_SYSCALL:                          /* 04-jul-91 */
        inf = &SyscallInfo;
        break;
    case LANG_STDCALL:                          /* 08-jan-92 */
        inf = &StdcallInfo;
        break;
    case LANG_OPTLINK:                          /* 08-jan-92 */
        inf = &OptlinkInfo;
        break;
    case LANG_FASTCALL:                         /* 08-jan-92 */
        inf = &FastCallInfo;
        break;
    }
    return( inf );
}


int ParmsToBeReversed( int flags, struct aux_info *inf )
{
    #ifdef REVERSE
        inf = LangInfo( flags, inf );
        if( inf != NULL ) {
            if( inf->class & REVERSE_PARMS )  return( 1 );
        }
    #else
        flags = flags;
        inf = inf;
    #endif
    return( 0 );
}

struct aux_info *ModifyLookup( SYMPTR sym )
{
#if _CPU == 386 || _CPU == 8086
    char        *p;
    struct aux_info *inf;
    int         len;
    int         hash;

static unsigned char NoModifyWeights[] = {
//a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y,z
  1, 4, 5, 0, 0,11, 0, 0, 8, 0, 0,15, 0,14,10, 0, 0, 9, 3, 7, 0, 0, 0, 0, 0,0
};
static char *NoModifyFuncs[] = {
        "sqrt",
        "ceil",
        "cos",
        "fabs",
        "atan2",
        "sinh",
        "atan",
        "labs",
        "abs",
        "tanh",
        "floor",
        "tan",
        "cosh",
        "asin",
        "sin",
        "acos",
};

    p = sym->name;
    if( p != NULL ) {                           /* 01-jun-90 */
        len = strlen( p );
        hash = (len + NoModifyWeights[ p[0] - 'a' ]
                    + NoModifyWeights[ p[len-2] - 'a' ]) & 15;
        if( strcmp( NoModifyFuncs[ hash ], p ) == 0 ) {
            inf = &InlineInfo;
            inf->class = DefaultInfo.class
                           | NO_MEMORY_READ | NO_MEMORY_CHANGED;
            inf->code = NULL;
            inf->parms = DefaultInfo.parms;
            inf->returns = DefaultInfo.returns;
            HW_CAsgn( inf->streturn, HW_EMPTY );
            inf->save = DefaultInfo.save;
            inf->objname = DefaultInfo.objname;
            inf->use = 1;
            return( inf );
        }
    }
#else
    sym = sym;
#endif
    return( &DefaultInfo );
}

struct aux_info *InfoLookup( SYMPTR sym )
{
    char        *name;
    struct aux_info *inf;
    struct aux_entry *ent;

    name = sym->name;
    inf = &DefaultInfo;         /* assume default */
    if( name == NULL ) return( inf );                   /* 01-jun-90 */
    ent = AuxLookup( name );
    if( ent != NULL ) {
        inf = ent->info;
    } else {
        if( sym->flags & SYM_DEFINED )    return( inf );
        if( ! (sym->flags & SYM_INTRINSIC) ) {  /* 12-oct-92 */
            if( memcmp( name, "_inline_", 8 ) != 0 )  return( inf );
            name += 8;
        }
        #if _MACHINE == _PC
        {
            struct inline_funcs __FAR *ifunc;

            ifunc = IF_Lookup( name );
            if( ifunc == NULL )  return( inf );
            if( HW_CEqual( ifunc->returns, HW_DX_AX ) ||
                HW_CEqual( ifunc->returns, HW_DS_SI ) ||
                HW_CEqual( ifunc->returns, HW_ES_DI ) ||
                HW_CEqual( ifunc->returns, HW_CX_DI ) ) {
                if( SizeOfArg( sym->sym_type->object ) != 4 )  return( inf );
            }
            inf = &InlineInfo;
            inf->class = (DefaultInfo.class & FAR) | MODIFY_EXACT;
            inf->code = ifunc->code;
            inf->parms = ifunc->parms;
            inf->returns = ifunc->returns;
            if( !HW_CEqual( inf->returns, HW_AX )
             && !HW_CEqual( inf->returns, HW_EMPTY ) ) {
                inf->class |= SPECIAL_RETURN;
            }
            HW_CAsgn( inf->streturn, HW_EMPTY );
            inf->save = ifunc->save;
            inf->objname = DefaultInfo.objname; /* 26-jan-93 */
            inf->use = 1;
        }
        #endif
    }
    return( inf );
}


struct aux_info *FindInfo( SYM_ENTRY *sym, SYM_HANDLE sym_handle )
{
    auto SYM_ENTRY      sym_typedef;
    struct aux_entry    *ent;
    TYPEPTR             typ;
    struct aux_info     *inf;

    inf = &DefaultInfo;         /* assume default */
    if( sym_handle == 0 ) return( inf );
    SymGet( sym, sym_handle );
    #if _CPU == 386
        if( sym_handle == SymSTOSB || sym_handle == SymSTOSD ) {
            return( &STOSBInfo );
        }
        if( sym_handle == SymFinally ) {                /* 28-mar-94 */
            static byte_seq FinallyCode = { 1, 0xc3 };

            InlineInfo = DefaultInfo;
            InlineInfo.code = &FinallyCode;
            return( &InlineInfo );
        }
        if( sym_handle == SymTryFini ) {
            static hw_reg_set TryFiniParms[] = { HW_D( HW_EAX ),
                                                 HW_D( HW_EMPTY ) };
            static byte_seq TryFiniCode = { 6,
                        0x64, 0xA3, 0,0,0,0 };  /* mov fs:0,eax */

            InlineInfo = DefaultInfo;
            InlineInfo.parms = TryFiniParms;
            InlineInfo.code = &TryFiniCode;
            return( &InlineInfo );
        }
    #endif
    if( !( sym->flags & SYM_TEMP ) ) {  /* not an indirect func call*/
        inf = InfoLookup( sym );
        if( inf == &DefaultInfo )  inf = ModifyLookup( sym );
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
        if( ( inf->flags & AUX_FLAG_FAR16 ) ||
            ( sym->attrib & FLAG_FAR16 ) ) {
            if( ( (sym->attrib & FLAG_LANGUAGES) == LANG_PASCAL ) ||
                ( inf->class & REVERSE_PARMS ) ) {
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

    if( sym_handle != 0 ) {                             /* 19-apr-93 */
        SymGet( sym, sym_handle );
        ent = AuxLookup( SymName( sym, sym_handle ) );
        if( ent != NULL ) {
            if( ent->info->class & SUICIDAL )  return( 1 );
        }
    }
    return( 0 );
}

void GetCallClass( SYM_HANDLE sym_handle )
{
    struct aux_info *inf;
    auto SYM_ENTRY sym;

    CallClass = DefaultInfo.class;
    if( sym_handle != 0 ) {
        inf = FindInfo( &sym, sym_handle );
        if( sym.flags & SYM_FUNCTION ) {
            switch( sym.attrib & FLAG_LANGUAGES ) {
            case LANG_CDECL:
                if( inf != &DefaultInfo ) {
                    CallClass = inf->class;
                } else {
                    CallClass = CdeclInfo.class;
                }
                #if _CPU == 8086                        /* 18-nov-94 */
                    if( TargSys == TS_WINDOWS ) {
                        CallClass |= FAT_WINDOWS_PROLOG;
                    }
                #endif
                break;
            case LANG_PASCAL:
                if( inf != &DefaultInfo ) {
                    CallClass = inf->class;
                } else {
                    CallClass = PascalInfo.class;
                }
                #if _CPU == 8086                        /* 21-jan-93 */
                    if( TargSys == TS_WINDOWS ) {       /* 01-mar-91 */
                        CallClass |= FAT_WINDOWS_PROLOG;
                    }
                #endif
                break;
            case LANG_FORTRAN:
                CallClass = FortranInfo.class;
                break;
            case LANG_SYSCALL:
                CallClass = SyscallInfo.class;
                break;
            case LANG_STDCALL:
                CallClass = StdcallInfo.class;
                break;
            case LANG_OPTLINK:
                CallClass = OptlinkInfo.class;
                break;
            default:
                CallClass = inf->class;
            }
        #if _MACHINE == _PC
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
            if( sym.attrib & FLAG_EXPORT ) {                /* 12-mar-90 */
                CallClass |= DLL_EXPORT;
            }
        #endif
        #ifdef LOAD_DS_ON_ENTRY
            if( sym.attrib & FLAG_LOADDS ) {                /* 26-apr-90 */
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
            if( IsInLineFunc( sym_handle ) ){
                CallClass |= MAKE_CALL_INLINE;
            }
        #endif
        }
        if( /* inf == &DefaultInfo && */  VarFunc( &sym ) ) {/* 19-dec-88*/
            CallClass |= CALLER_POPS | HAS_VARARGS;
        }
    }
    #ifdef REVERSE
        CallClass &= ~ REVERSE_PARMS;               /* 28-may-89 */
    #endif
    #if _MACHINE == _PC
        if( sym.flags & SYM_FUNC_NEEDS_THUNK ) {
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

enum {
    TIME_SEC_B  = 0,
    TIME_MIN_B  = 5,
    TIME_HOUR_B = 11,
};

enum {
    DATE_DAY_B  = 0,
    DATE_MON_B  = 5,
    DATE_YEAR_B = 9,
};

static unsigned long *GetTimeStamp( FNAMEPTR flist )
{
#if _MACHINE == _PC && COMP_CFG_COFF == 0
    struct tm *ltime;
    unsigned short dos_date;
    unsigned short dos_time;
    static unsigned long dos_stamp;

    ltime = localtime( &(flist->mtime) );
    dos_date = (( ltime->tm_year - 80 ) << DATE_YEAR_B )
             | (( ltime->tm_mon + 1 ) << DATE_MON_B )
             | (( ltime->tm_mday ) << DATE_DAY_B );
    dos_time = (( ltime->tm_hour ) << TIME_HOUR_B )
             | (( ltime->tm_min ) << TIME_MIN_B )
             | (( ltime->tm_sec / 2 ) << TIME_SEC_B );
    dos_stamp = dos_time | ( dos_date << 16 );
    return( &dos_stamp );
#else
    static time_t stamp;

    stamp = flist->mtime;
    return( &stamp );
#endif
}


#ifdef __LARGE__
 #define PTR(x) (VOIDPTR)MK_FP((x),0)
#else
 #define PTR(x) (VOIDPTR)(x)
#endif
static FNAMEPTR NextDependency( FNAMEPTR curr ){
    if( curr == NULL ){
        curr = FNames;
    }else{
        curr = curr->next;
    }
    while( curr != NULL ){
        if( curr->rwflag ){
            if( !SrcFileInRDir( curr ) )break;
        }
        curr = curr->next;
    }
    return( curr );
}

static VOIDPTR NextLibrary( int index, aux_class request )
{
    struct library_list *liblist;
    char                *name;
    int                 i;

    i = 0;
    if( request == NEXT_LIBRARY ) ++index;
    for( liblist = HeadLibs; liblist; liblist = liblist->next ) {
        name = &liblist->prio;
        ++i;
        if( i == index ) break;
    }
    if( liblist == NULL ) {
        switch( index - i ) {
        case 1: /* return 1 for CLIB */
            name = CLIB_Name;
            if( CompFlags.emit_library_any )break;
            if( CompFlags.emit_library_with_main ) {
                if( CompFlags.has_main )        break;
                if( CompFlags.has_winmain )     break;
                if( CompFlags.bd_switch_used )  break;
                if( CompFlags.has_libmain )     break;
                if( CompFlags.bm_switch_used )  break;  /* JBS */
                ++index;
            } else {
                name = NULL;
                index = 0;              // indicate all done
            }
            break;
        case 2: /* return 2 for MATHLIB */
            name = MATHLIB_Name;
            break;
        case 3: /* return 3 for EMULIB */
            name = EmuLib_Name;
            if( EmuLib_Name != NULL )   break;
            ++index;
        case 4: /* return 4 for PCODE */
            #ifdef __PCODE__
                name = PcodeLib_Name;
                if( CompFlags.pcode_generated ) break;
            #endif
            name = NULL;
            index = 0;                  // indicate all done
        }
    }
    if( request == LIBRARY_NAME ) return( name );
    return( (char *)index );
}
#if _MACHINE == _PC


static VOIDPTR NextImport( int index, aux_class request )
{
    char        *name;

    if( request == NEXT_IMPORT ) ++index;
    switch( index ) {
    case 1:
        if( CompFlags.has_wchar_entry ){
            name = "__DLLstartw_";
        }else{
            name = "__DLLstart_";
        }
        if( CompFlags.has_libmain ){
            if( CompFlags.bd_switch_used )break;
            if( !(CompFlags.bc_switch_used || CompFlags.bg_switch_used ) )break;
        }
        if( CompFlags.has_wchar_entry ){
            name = "_wstartw_";
        }else{
            name = "_wstart_";
        }
        if( CompFlags.has_winmain ){
            if( CompFlags.bg_switch_used  )break;
            if( !(CompFlags.bd_switch_used || CompFlags.bc_switch_used ) )break;
        }
    #if _CPU == 8086
        if( TargetSwitches & WINDOWS && CompFlags.has_main )break;
    #endif
        if( CompFlags.has_wchar_entry ){
            name = "_cstartw_";
        }else{
            name = "_cstart_";
        }
        if( CompFlags.has_main ){
            if( CompFlags.bc_switch_used  )break;
            if( !(CompFlags.bd_switch_used || CompFlags.bg_switch_used ) )break;
        }
        ++index;
    case 2:
        name = "_fltused_";
        if( CompFlags.emit_library_with_main
         || CompFlags.emit_library_any ){            /* 12-mar-90 */
            if( CompFlags.float_used ) break;
        }
        ++index;
    case 3:
        #if _CPU == 8086
            name = "_small_code_";
            if( TargetSwitches & BIG_CODE ) {
                name = "_big_code_";
            }
            if( CompFlags.emit_library_with_main
             || CompFlags.emit_library_any ){            /* 12-mar-90 */
                #ifdef NEWCFE
                    if( FirstStmt != 0 )        break;
                #else
                    if( SymHeaders != 0 )       break;
                #endif
            }
        #endif
        ++index;
    case 4:
        if( CompFlags.pgm_used_8087  || CompFlags.float_used ) {
            #if _CPU == 386
                name = "__init_387_emulator";
            #else
                name = "__init_87_emulator";
            #endif
            if( GET_FPU( ProcRevision ) & FPU_EMU )     break;
        }
        ++index;
    case 5:
        if( (CompFlags.emit_library_with_main || CompFlags.emit_library_any )
          &&  Stack87 == 4 ) {
            name = "__old_8087";
        } else {
            name = "__8087";
        }
        if( CompFlags.pgm_used_8087 || CompFlags.float_used ) {
            if( GET_FPU(ProcRevision) > FPU_NONE )      break;
        }
        ++index;
    case 6:
        if( CompFlags.has_wchar_entry ){
            name = "__wargc";
        }else{
            name = "__argc";
        }
    #if _CPU == 386
        if( ! CompFlags.register_conventions ){
          ++name;  // change from __wargc , __argc to _wargc..
        }
    #endif
        if( CompFlags.main_has_parms )          break;
        ++index;
    case 7:
        name = "__init_default_win";
        if( CompFlags.bw_switch_used )          break;
        ++index;
    case 8:
        name = "__WATCOM_Prelude";
        if( TargSys == TS_NETWARE )             break;
        if( TargSys == TS_NETWARE5 )            break;
        ++index;
    case 9:
        name = "__p5_profile";
        if( TargetSwitches & P5_PROFILING )     break;
    case 10:
        name = "__new_p5_profile";
        if( TargetSwitches & NEW_P5_PROFILING ) break;
    default:
        index = 0;                              // indicate no more
        name = NULL;
    }
    if( request == IMPORT_NAME ) return( name );
    return( (char *)index );
}

extern char *FEExtName( CGSYM_HANDLE sym_handle, char **pat_ret ){
// Return external name of symbol plus a pattern manipulator string
    char *pattern;
    SYM_ENTRY sym;
    struct  aux_info *inf;
    char   *name;

    inf = FindInfo( &sym, sym_handle );
#ifdef __SEH__
    if( sym_handle == SymTryInit ||
        sym_handle == SymTryFini ||
        sym_handle == SymTryUnwind ||
        sym_handle == SymExcept ) {
        pattern = "*";
    }else{
#endif
    inf = LangInfo( sym.attrib, inf );
    if( inf->objname != NULL ){
        pattern = inf->objname;
    }else if( ! (sym.flags & SYM_FUNCTION) ){
        pattern =  "_*";
    }else{
      pattern =  "*_";     /* for function names */
    }
#ifdef __SEH__
    }       // close that else
#endif
    name = sym.name;
    if( ((sym.flags & SYM_FUNCTION)&&(sym.attrib & FLAG_LANGUAGES) == LANG_STDCALL)
        && CompFlags.use_stdcall_at_number
     ) {
        int         total_parm_size = 0;
        int         parm_size;
        TYPEPTR     fn_typ;
        TYPEPTR     *parm;
        TYPEPTR     typ;
        char        suffix[6];

        fn_typ = sym.sym_type;
        while( fn_typ->decl_type == TYPE_TYPEDEF ) fn_typ = fn_typ->object;
        parm = fn_typ->u.parms;
        if( parm != NULL ) {
            for(; typ = *parm; ++parm ) {
                if( typ->decl_type == TYPE_DOT_DOT_DOT ) {
                    total_parm_size = -1;
                    break;
                }
                parm_size = TypeSize( typ );
                parm_size = (parm_size + sizeof(target_int) - 1)  &
                                - sizeof(target_int);
                total_parm_size += parm_size;
            }
        }
        if( total_parm_size != -1 ) {
            suffix[0] = '@';
            itoa( total_parm_size, &suffix[1], 10 );
            strcpy( Buffer, name );
            strcat( Buffer, suffix );
            name = Buffer;
        }
    }
    *pat_ret = pattern; // return pattern string
    return( name );
}

VOIDPTR FEAuxInfo( CGSYM_HANDLE cgsym_handle, aux_class request )
{
    SYM_HANDLE sym_handle = cgsym_handle;
    struct aux_info *inf;
    FNAMEPTR flist;
    auto SYM_ENTRY sym;
    static      hw_reg_set      save_set;

    switch( request ) {
    case SOURCE_LANGUAGE:       return( "C" );
    case STACK_SIZE_8087:       return( (VOIDPTR)Stack87 );
    case CODE_GROUP:            return( (VOIDPTR)GenCodeGroup );
    case DATA_GROUP:            return( (VOIDPTR)DataSegName );
    case OBJECT_FILE_NAME:      return( (VOIDPTR)ObjFileName( OBJ_EXT ) );
    case REVISION_NUMBER:       return( (VOIDPTR)II_REVISION );
    case AUX_LOOKUP:            return( (VOIDPTR)sym_handle );
    case PROEPI_DATA_SIZE:      return( (VOIDPTR)ProEpiDataSize );
    case DBG_PREDEF_SYM:        return( (VOIDPTR)SymDFAbbr );
    case P5_CHIP_BUG_SYM:       return( (VOIDPTR)SymChipBug ); /* 09-dec-94 */
    case CODE_LABEL_ALIGNMENT:
        {
            static unsigned char Alignment[] = { 2, 1, 1 };

            if( OptSize == 0 ) Alignment[1] = TARGET_INT;

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
        }else{
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
    case TEMP_LOC_NAME:
        return( (char *)TEMP_LOC_QUIT );
    case TEMP_LOC_TELL:
        return( NULL );
    case NEXT_DEPENDENCY:                               /* 03-dec-92 */
        if( !CompFlags.emit_dependencies ) {
            return( NULL );
        } else {
            return( NextDependency( (FNAMEPTR) cgsym_handle ) );
        }
        break;
    case DEPENDENCY_TIMESTAMP:
        flist = (FNAMEPTR) cgsym_handle;
        return( GetTimeStamp( flist ) );
    case DEPENDENCY_NAME:
        flist = (FNAMEPTR) cgsym_handle;
        return( FNameFullPath( flist ) );
    case PEGGED_REGISTER:
        return( SegPeggedReg( (unsigned)cgsym_handle ) );
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
    case CALL_BYTES:    return( inf->code );
    case PARM_REGS:
        #ifdef __SEH__
            if( sym_handle == SymTryInit ||
                sym_handle == SymTryFini ||
                sym_handle == SymTryUnwind ||
                sym_handle == SymExcept ) {
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
    }
    return( NULL );
}
#else

static VOIDPTR NextImport( int index, aux_class request )
{
    char        *name;

    if( request == NEXT_IMPORT ) ++index;
    switch( index ) {
    case 1:
        if( CompFlags.has_wchar_entry ){
            name = "__DLLstartw_";
        }else{
            name = "__DLLstart_";
        }
        if( CompFlags.bd_switch_used ){
            if( CompFlags.has_libmain )     break;
        }
        if( CompFlags.has_wchar_entry ){
            name = "_wstartw_";
        }else{
            name = "_wstart_";
        }
        if( CompFlags.has_winmain )     break;
        if( CompFlags.has_wchar_entry ){
            name = "_cstartw_";
        }else{
            name = "_cstart_";
        }
        if( CompFlags.has_main )        break;
        ++index;
    case 2:
        name = "_fltused_";
        if( CompFlags.emit_library_with_main
         || CompFlags.emit_library_any ){            /* 12-mar-90 */
            if( CompFlags.float_used ) break;
        }
        ++index;
    case 3:
        name = "_argc";
        if( CompFlags.main_has_parms )          break;
        ++index;
    case 4:
        name = "__init_default_win";
        if( CompFlags.bw_switch_used )          break;
        ++index;
    default:
        index = 0;                              // indicate no more
        name = NULL;
    }
    if( request == IMPORT_NAME ) return( name );
    return( (char *)index );
}

extern char *FEExtName( CGSYM_HANDLE sym_handle, char **pat_ret ){
// Return external name of symbol plus a pattern manipulator string
    char *pattern;
    SYM_ENTRY sym;
    struct  aux_info *inf;
    char   *name;

    inf = FindInfo( &sym, sym_handle );
#ifdef __SEH__
    if( sym_handle == SymTryInit ||
        sym_handle == SymTryFini ||
        sym_handle == SymTryUnwind ||
        sym_handle == SymExcept ) {
        pattern = "*";
    }else{
#endif
    inf = LangInfo( sym.attrib, inf );
    if( inf->objname != NULL ){
        pattern = inf->objname;
    }else{
      pattern =  "*";
    }
#ifdef __SEH__
    }       // close that else
#endif
    name = sym.name;
    *pat_ret = pattern; // return pattern string
    return( name );
}

VOIDPTR FEAuxInfo( CGSYM_HANDLE cgsym_handle, aux_class request )
{
    SYM_HANDLE sym_handle = cgsym_handle;
    struct aux_info *inf;
    FNAMEPTR flist;
    auto SYM_ENTRY sym;
    static      hw_reg_set      save_set;

    switch( request ) {
    case SOURCE_LANGUAGE:       return( "C" );
    case OBJECT_FILE_NAME:      return( (VOIDPTR)ObjFileName( OBJ_EXT ) );
    case REVISION_NUMBER:       return( (VOIDPTR)II_REVISION );
    case AUX_LOOKUP:            return( (VOIDPTR)sym_handle );
    case SOURCE_NAME:
        if( SrcFName == ModuleName ) {
            return( FNameFullPath( FNames ) );
        }else{
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
    case FREE_SEGMENT:
        return( NULL );
    case TEMP_LOC_NAME:
        return( (char *)TEMP_LOC_QUIT );
    case TEMP_LOC_TELL:
        return( NULL );
    case NEXT_DEPENDENCY:                               /* 03-dec-92 */
        if( !CompFlags.emit_dependencies ) {
            return( NULL );
        } else {
            return( NextDependency( (FNAMEPTR) cgsym_handle ) );
        }
        break;
    case DEPENDENCY_TIMESTAMP:
        flist = (FNAMEPTR) cgsym_handle;
        return( GetTimeStamp( flist ) );
    case DEPENDENCY_NAME:
        flist = (FNAMEPTR) cgsym_handle;
        return( SrcFullPath( Buffer, flist->name, 512 ) );
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
    case CALL_BYTES:    return( inf->code );
    case PARM_REGS:
        if( sym_handle != 0 ) {
            inf = LangInfo( sym.attrib, inf );
            if( inf->code == NULL && VarFunc( &sym ) ) {
                return( DefaultVarParms );
            }
        }
        return( inf->parms );
    }
    return( NULL );
}
#endif
extern char *SrcFullPath( char *buff, char const *name, unsigned max )
{
    char        *p;

    p = _fullpath( buff, name, max );
    if( p == NULL ) p = (char *)name;
    #if _OS == _QNX
        if( (p[0] == '/' && p[1] == '/') && (name[0] != '/' || name[1] != '/') ) {
            /*
               if the _fullpath result has a node number and
               the user didn't specify one, strip the node number
               off before returning
            */
            p += 2;
            while( *p != '/' ) ++p;
        }
    #endif
    return( p );
}
