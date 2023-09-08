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


#include "plusplus.h"

#include "compcfg.h"
#include "tgtenv.h"

#include <limits.h>

#include "preproc.h"
#include "cgdata.h"
#include "codegen.h"
#include "memmgr.h"
#include "pragdefn.h"
#include "pdefn2.h"
#include "iosupp.h"
#include "segment.h"
#include "cgback.h"
#include "rtfuns.h"
#include "module.h"
#include "srcfile.h"
#include "symdbg.h"
#include "dwarfdbg.h"
#include "extrf.h"
#include "ctexcept.h"
#include "name.h"
#include "cginlibs.h"
#include "cginimps.h"
#include "initdefs.h"
#include "iosupp.h"
#include "cppexit.h"
#include "cgbackut.h"
#include "cginmisc.h"
#include "specfuns.h"
#include "autodept.h"
#include "dwarfid.h"
#include "cgfront.h"
#include "feprotos.h"
#ifndef NDEBUG
    #include "togglesd.h"
#endif


#if _CPU == 386
    extern const inline_funcs Fs_Functions[];   // FS PRAGMAS
    extern const alt_inline_funcs FlatAlternates[];
#endif

static SYMBOL lastFunctionOutOfMem;

#define _HAS_EXE_MAIN   (  CompFlags.has_main \
                        || CompFlags.has_winmain \
                        )
#define _HAS_DLL_MAIN   ( CompFlags.bd_switch_used \
                        || CompFlags.has_libmain \
                        || CompFlags.has_dllmain \
                        )
#define _HAS_ANY_MAIN   (  _HAS_EXE_MAIN \
                        || _HAS_DLL_MAIN \
                        )

static AUX_INFO *getLangInfo(   // GET LANGUAGE INFO. FOR SYMBOL
    SYMBOL sym )                // - the symbol
;

static void init(               // MODULE INITIALIZATION
    INITFINI* def )             // - definition
{
    /* unused parameters */ (void)def;

    CompFlags.low_on_memory_printed = false;
}

INITDEFN( cg_info, init, InitFiniStub )

const char *FEName(             // RETURN THE SYMBOL'S NAME
    cg_sym_handle _sym )
{
    const char  *sym_name;      // - symbol's name
    SYMBOL sym = _sym;

    if( sym == NULL || sym->name == NULL ) {
        sym_name = "!NULL!";
    } else {
        sym_name = CppNameDebug( sym );
    }
    return( sym_name );
}


void FEMessage(                 // MESSAGES FROM CODE-GENERATOR
    int class,                  // - message class
    pointer parm )              // - parameter
{
    switch( (msg_class)class ) {
    case MSG_SYMBOL_TOO_LONG:
        CErr2p( WARN_MANGLED_NAME_TOO_LONG, (SYMBOL)parm );
        break;
    case MSG_BLIP:
        if( CompFlags.ide_console_output ) {
            if( !CompFlags.quiet_mode ) {
                putchar( '.' );
                fflush( stdout );
            }
        }
        break;
    case MSG_INFO:
    case MSG_INFO_FILE:
    case MSG_INFO_PROC:
        if( CompFlags.ide_console_output ) {
            if( !CompFlags.quiet_mode ) {
                MsgDisplayLine( parm );
            }
        }
        break;
    case MSG_CODE_SIZE:
        if( CompFlags.ide_console_output ) {
            if( !CompFlags.quiet_mode ) {
                char buffer[30];
                sprintf( buffer, "\rCode size: %u", (unsigned)(pointer_uint)parm );
                MsgDisplayLine( buffer );
            }
        }
        break;
    case MSG_DATA_SIZE:
        break;
    case MSG_ERROR:
        CErr2p( ERR_INTERNAL_ERROR_MSG, parm );
        break;
    case MSG_FATAL:
        CErr2p( ERR_FATAL_ERROR, parm );
        CppExit( 1 );         /* exit to DOS do not pass GO */
        break;
    case MSG_BAD_PARM_REGISTER:
        CErr2( ERR_BAD_PARM_REGISTER, (int)(pointer_uint)parm );
        break;
    case MSG_BAD_RETURN_REGISTER:
        CErr2p( ERR_BAD_RETURN_REGISTER, FEName( (SYMBOL)parm ) );
        break;
    case MSG_SCHEDULER_DIED:
    case MSG_REGALLOC_DIED:
    case MSG_SCOREBOARD_DIED:
        if( (GenSwitches & CGSW_GEN_NO_OPTIMIZATION) == 0 ) {
            if( lastFunctionOutOfMem != parm ) {
                lastFunctionOutOfMem = parm;
                CErr2p( WARN_CG_MEM_PROC, FEName( (SYMBOL)parm ) );
            }
        }
        break;
    case MSG_PEEPHOLE_FLUSHED:
        if( (GenSwitches & CGSW_GEN_NO_OPTIMIZATION) == 0 ) {
            if( !CompFlags.low_on_memory_printed ) {
                CompFlags.low_on_memory_printed = true;
                CErr1( WARN_CG_MEM_PEEPHOLE );
            }
        }
        break;
    case MSG_BACK_END_ERROR:
        CErr2( ERR_BACK_END_ERROR, (int)(pointer_uint)parm );
        break;
    case MSG_BAD_SAVE:
        CErr2p( ERR_BAD_SAVE, FEName( (SYMBOL)parm ) );
        break;
    case MSG_NO_SEG_REGS:
        CErr2p( ERR_NO_SEG_REGS, FEName( (SYMBOL)parm ) );
        break;
    }
}


const char *FEModuleName(       // RETURN MODULE NAME
    void )
{
    return( ModuleName );
}


int FETrue(                     // RETURN true
    void )
{
    return( 1 );
}

#if 0
int FESymIndex(                 // STUB EXCEPT FOR JAVA
    SYMBOL sym )
{
    sym = sym;
    return( 0 );
}

int FECodeBytes(                // STUB EXCEPT FOR JAVA
    const char *buffer,
    int len )
{
    buffer = buffer; len = len;
    return( false );
}
#endif

static fe_attr basic_attributes(// GET BASIC ATTRIBUTES
    SYMBOL sym )                // - symbol
{
    fe_attr attr;               // - attribute

    switch( sym->id ) {
    case SYMC_EXTERN:
    case SYMC_EXTERN_FUNCTION_TEMPLATE:
        attr = FE_STATIC | FE_GLOBAL | FE_IMPORT ;
        break;
    case SYMC_PUBLIC:
    case SYMC_FUNCTION_TEMPLATE:
        attr = FE_STATIC | FE_GLOBAL;
        break;
    case SYMC_STATIC:
    case SYMC_STATIC_FUNCTION_TEMPLATE:
        attr = FE_STATIC | FE_VISIBLE;
        break;
    default :
        attr = 0;
        break;
    }
    return( attr );
}

fe_attr FEAttr(                 // GET SYMBOL ATTRIBUTES
    cg_sym_handle _sym )        // - symbol
{
    fe_attr attr;               // - attribute
    fe_attr mask;               // - remove these attributes
    SYMBOL_NAME sym_name;       // - symbol name of symbol
    SCOPE scope;                // - scope of symbol
    NAME name;                  // - name of symbol
    TYPE unmod_type;            // - type for symbol
    type_flag mod_flags;        // - modifier flags
    scf_mask scf_info;          // - sym comdat function info
    SYMBOL sym = _sym;

    attr = 0;
    mask = 0;
    name = NULL;
    sym_name = sym->name;
    if( sym_name != NULL ) {
        scope = sym_name->containing;
        if( ScopeEnclosedInUnnamedNameSpace( scope ) ) {
            mask = FE_GLOBAL;
        }
        name = sym_name->name;
        if( name != NULL ) {
            if( IsNameDummy( name ) ) {
                attr |= FE_INTERNAL;
            }
        }
    }
    unmod_type = TypeModFlags( sym->sym_type, &mod_flags );
    if( unmod_type->id == TYP_FUNCTION ) {
        attr |= FE_PROC;
        if( !IsCppNameInterestingDebug( sym ) ) {
            attr |= FE_COMPILER;
        }
        if( SymIsInitialized( sym ) ) {
            scf_info = SymComdatFunInfo( sym );
            if( scf_info & SCF_COMDAT ) {
                if( scf_info & SCF_STATIC ) {
                    mask = FE_COMMON | FE_GLOBAL;
                } else {
                    attr |= FE_COMMON;
                }
            }
            if( unmod_type->flag & TF1_NAKED ) {
                attr |= FE_NAKED;
            }
            if( CompFlags.unique_functions ) {
                attr |= FE_UNIQUE;
            }
        } else {
            attr |= FE_IMPORT;
        }
        if( TypeHasEllipsisArg( unmod_type ) ) {
            attr |= FE_VARARGS;
        }
    } else {
        if( SymIsComdatData( sym ) ) {
            attr |= FE_COMMON;
        }
        if( mod_flags & TF1_STAY_MEMORY ) {
            attr |= FE_VOLATILE;
        }
        if( mod_flags & TF1_CONST ) {
            attr |= FE_CONSTANT;
        }
        if( mod_flags & TF1_THREAD ) {
            attr |= FE_THREAD_DATA;
        }
        if( (mod_flags & TF1_HUGE) == 0 ) {
            attr |= FE_ONESEG;
        }
    }
    // don't export addressability thunks
    if( (sym->flag & SYMF_ADDR_THUNK) == 0 ) {
        if( mod_flags & (TF1_DLLEXPORT|TF1_DLLIMPORT) ) {
            if( SymIsInitialized( sym ) ) {
                if( mod_flags & TF1_DLLEXPORT ) {
                    attr |= FE_DLLEXPORT;
                }
            } else {
                if( mod_flags & TF1_DLLIMPORT ) {
                    // fill vftables with 'S::f' not '__imp_S::f'
                    if( ! SymIsVirtual( sym ) ) {
                        attr |= FE_DLLIMPORT;
                    }
                }
            }
        }
    }
    // change to this: if( sym->flag & SYMF_CG_ADDR_TAKEN ) {
    if( sym->flag & SYMF_CG_ADDR_TAKEN ) {
        attr |= FE_ADDR_TAKEN;
    }
    if( SymIsClassMember( sym ) ) {
        if( SymIsStatic( sym ) && IsCppMembPtrOffsetName( name ) ) {
            attr |= FE_UNIQUE;
        }
        if( SymIsInitialized( sym ) ) {
            attr |= FE_STATIC;
            /* only set FE_GLOBAL if it's not an in-class
         * initialization of a const static member */
            if( (sym->flag & SYMF_IN_CLASS_INIT) == 0 ) {
                attr |= FE_GLOBAL;
            }
        } else {
            attr |= FE_STATIC | FE_GLOBAL | FE_IMPORT ;
        }
    } else {
        attr |= basic_attributes( sym );
    }
    if( (mod_flags & TF1_COMMON) && 0 == mask ) {
        /* should never execute this but just in case ... */
        attr |= FE_COMMON | FE_GLOBAL;
    } else if( attr & FE_COMMON ) {
        attr |= FE_GLOBAL;
    }
    DbgAssert( mask == 0 || (attr & FE_COMMON) == 0 );
    attr &= ~mask;
#ifndef NDEBUG
    if( TOGGLEDBG( auxinfo ) ) {
        printf( "FeAttr( %p = %s ) -> %x\n"
              , (void *)sym
              , GetMangledName( sym )
              , attr );
    }
#endif
    return( attr );
}


segment_id FESegID(             // GET SEGMENT ID FOR SYMBOL
    cg_sym_handle sym )         // - symbol
{
    return( CgSegId( sym ) );
}


int FELexLevel(                 // GET LEXICAL LEVEL OF SYMBOL
    cg_sym_handle sym )         // - the symbol
{
    /* unused parameters */ (void)sym;

    return( 0 );
}


cg_type FEParmType(             // ARGUMENT PROMOTION ?
    cg_sym_handle func,         // function being called
    cg_sym_handle parm,         // parameter being passed
    cg_type type )              // - original type
{
    /* unused parameters */ (void)parm; (void)func;

    switch( type ) {
#if _INTEL_CPU
  #if _CPU == 8086
    case TY_INT_1:
    case TY_UINT_1:
        type = TY_INTEGER;
        break;
  #else
    case TY_UINT_2:
    case TY_INT_2:
    case TY_INT_1:
    case TY_UINT_1:
        if( func != NULL ) {
            type_flag fn_flags;
            TypeModFlags( ((SYMBOL)func)->sym_type, &fn_flags );
            if( fn_flags & TF1_FAR16 ) {
                return( TY_INT_2 );
            }
        }
        type = TY_INTEGER;
        break;
#elif _CPU == _AXP
    case TY_INT_1:
    case TY_INT_2:
    case TY_INT_4:
    case TY_INTEGER:
    case TY_POINTER:
    case TY_CODE_PTR:
    case TY_NEAR_POINTER:
    case TY_NEAR_CODE_PTR:
        return( TY_INT_8 );
    case TY_UINT_1:
    case TY_UINT_2:
    case TY_UINT_4:
        return( TY_UINT_8 );
#else
    case TY_UINT_2:
    case TY_INT_2:
    case TY_INT_1:
    case TY_UINT_1:
        type = TY_INTEGER;
        break;
#endif
    }
    return( type );
}


int FEStackChk(                 // STACK CHECKING ?
    cg_sym_handle _sym )        // - the symbol
{
    TYPE fn_type;
    SYMBOL sym = _sym;

    fn_type = FunctionDeclarationType( sym->sym_type );
    return( (fn_type->flag & TF1_STACK_CHECK) != 0 );
}


#if _INTEL_CPU
static const inline_funcs *Flat( const inline_funcs *ifunc )
{
  #if _CPU != 8086
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

static const inline_funcs *InlineLookup( NAME name )
{
    const inline_funcs  *ifunc;

    if( GET_FPU( CpuSwitches ) > FPU_NONE ) {
        ifunc = _8087_Functions;
        while( ifunc->name ) {
            if( strcmp( ifunc->name, NameStr( name ) ) == 0 )
                return( Flat( ifunc ) );
            ++ifunc;
        }
    }
    if( OptSize == 100 ) {              /* if /os specified */
        ifunc = SInline_Functions;
        if( IsBigData() ) {
  #if _CPU == 8086
            if( TargetSwitches & CGSW_X86_FLOATING_DS ) {
                ifunc = ZF_Data_Functions;
            } else {
                ifunc = ZP_Data_Functions;
            }
  #else
            ifunc = SBigData_Functions;
  #endif
        }
        while( ifunc->name ) {
            if( strcmp( ifunc->name, NameStr( name ) ) == 0 )
                return( Flat( ifunc ) );
            ++ifunc;
        }
    }
  #if _CPU != 8086
    ifunc = Fs_Functions;
    while( ifunc->name ) {
        if( strcmp( ifunc->name, NameStr( name ) ) == 0 )
            return( ifunc );
        ++ifunc;
    }
    if( IsFlat() ) {
        ifunc = Flat_Functions;
        while( ifunc->name ) {
            if( strcmp( ifunc->name, NameStr( name ) ) == 0 )
                return( ifunc );
            ++ifunc;
        }
    }
  #endif
    ifunc = Inline_Functions;
    if( IsBigData() ) {
  #if _CPU == 8086
        if( TargetSwitches & CGSW_X86_FLOATING_DS ) {
            ifunc = DF_Data_Functions;
        } else {
            ifunc = DP_Data_Functions;
        }
  #else
        ifunc = BigData_Functions;
  #endif
    }
    while( ifunc->name ) {
        if( strcmp( ifunc->name, NameStr( name ) ) == 0 )
            return( Flat( ifunc ) );
        ++ifunc;
    }
    ifunc = Common_Functions;
    while( ifunc->name ) {
        if( strcmp( ifunc->name, NameStr( name ) ) == 0 )
            return( Flat( ifunc ) );
        ++ifunc;
    }
    return( NULL );
}
#endif

static AUX_INFO *IntrinsicAuxLookup(
    SYMBOL sym )
{
#if _INTEL_CPU
    const inline_funcs *ifunc;
    AUX_INFO *inf;

    ifunc = InlineLookup( sym->name->name );
    if( ifunc == NULL )
        return( NULL );
    if( HW_CEqual( ifunc->returns, HW_DX_AX ) ||
        HW_CEqual( ifunc->returns, HW_DS_SI ) ||
        HW_CEqual( ifunc->returns, HW_ES_DI ) ||
        HW_CEqual( ifunc->returns, HW_CX_DI ) ) {
        TYPE type;
        type = FunctionDeclarationType( sym->sym_type )->of;
        if( CgTypeSize( type ) != 4 ) {
            return( NULL );
        }
    }
    inf = &InlineInfo;
    inf->cclass         = FECALL_GEN_NONE;
    inf->cclass_target  = (DefaultInfo.cclass_target & FECALL_X86_FAR_CALL) | FECALL_X86_MODIFY_EXACT;

    inf->code = ifunc->code;
    inf->parms = ifunc->parms;
    inf->returns = ifunc->returns;
    if( !HW_CEqual( inf->returns, HW_xAX ) && !HW_CEqual( inf->returns, HW_EMPTY ) ) {
        inf->cclass_target |= FECALL_X86_SPECIAL_RETURN;
    }
    HW_CAsgn( inf->streturn, HW_EMPTY );
    inf->save = ifunc->save;
    inf->objname = NULL;
    inf->use = 1;
#else /* _RISC_CPU */
    AUX_INFO *inf;

    /* unused parameters */ (void)sym;

    inf = NULL;
#endif
    return( inf );
}

static AUX_INFO *getLangInfo(   // GET LANGUAGE INFO. FOR SYMBOL
    SYMBOL sym )                // - the symbol
{
    AUX_INFO *inf;              // - information ptr.
    TYPE unmod_type;            // - unmodified type of symbol
    type_flag mod_flags;        // - modifier flags

    if( sym == NULL ) {
        inf = &DefaultInfo;
    } else {
        unmod_type = TypeModFlags( sym->sym_type, &mod_flags );
        if( unmod_type->id == TYP_FUNCTION ) {
            inf = NULL;
            if( (unmod_type->flag & TF1_INTRINSIC) && ( sym->name != NULL ) ) {
                inf = IntrinsicAuxLookup( sym );
            }
            if( inf == NULL ) {
                if( unmod_type->u.f.pragma != NULL ) {
                    inf = unmod_type->u.f.pragma;
                } else {
                    inf = &DefaultInfo;
                }
            }
#if _CPU == 386
            if(( mod_flags & TF1_FAR16 ) || ( inf->flags & AUX_FLAG_FAR16 )) {
                if( inf->cclass & FECALL_GEN_REVERSE_PARMS ) {
                    inf = &Far16PascalInfo;
                } else {
                    inf = &Far16CdeclInfo;
                }
            }
#endif
        } else {
            inf = TypeHasPragma( sym->sym_type );
            if( inf == NULL ) {
                inf = &DefaultInfo;
            }
        }
    }
    return( inf );
}

static unsigned GetParmsSize( SYMBOL sym )
{
    TYPE        fn_type;
    unsigned    size;

    size = 0;
    fn_type = FunctionDeclarationType( sym->sym_type );
    if( fn_type == NULL ) {
        size = (unsigned)-1;
    } else {
        TypeParmSize( fn_type, &size );
    }
    return( size );
}

static const char *allowStrictReplacement( const char *patbuff )
{
    const char *p;
    char       prev;

    /*
     * mangled C++ name will be injected as the name so
     * we only allow 'patbuff' to be a pure replacement
     * rather than like "_*" "*_" "^" "__!"
     */
    if( patbuff == NULL ) {
        return( patbuff );
    }
    prev = '\0';
    for( p = patbuff; *p != '\0'; ++p ) {
        if( prev != '\\' ) {
            switch( *p ) {
            case '*':
            case '^':
            case '!':
            case '#':
                return( NULL );
            }
        }
        prev = *p;
    }
    return( patbuff );
}


static const char *GetNamePattern( // MANGLE SYMBOL NAME
    SYMBOL sym )                // - symbol to mangle
{
    SCOPE       scope;          // - scope for function
    TYPE        fn_type;        // - symbol's function type
    const char  *patbuff;       // - control of result
    AUX_INFO    *inf;           // - auxilary info

    if( sym == NULL || sym->name == NULL ) {
        patbuff = NULL;
    } else {
        inf = getLangInfo( sym );
        patbuff = inf->objname;
        scope = SymScope( sym );
        fn_type = FunctionDeclarationType( sym->sym_type );
        if( fn_type != NULL ) {
            if( ( scope->id != SCOPE_FILE )
              || LinkageIsCpp( sym ) && ( fn_type->flag & TF1_PLUSPLUS ) ) {
                patbuff = allowStrictReplacement( patbuff );
            } else {
                if( patbuff == NULL )
                    patbuff = TS_CODE_MANGLE ;
                return( patbuff );
            }
        } else {
            if( ( scope->id != SCOPE_FILE )
              || LinkageIsCpp( sym ) ) {
                patbuff = allowStrictReplacement( patbuff );
            } else {
                patbuff = VarNamePattern( inf );
                if( patbuff == NULL )
                    patbuff = TS_DATA_MANGLE ;
                return( patbuff );
            }
        }
    }
    if( patbuff == NULL )
        patbuff = "*";
    return( patbuff );
}

const char *FEExtName( cg_sym_handle sym, int request )
/*******************************************************
 * Return symbol name related info for object file.
 */
{
    switch( request ) {
    case EXTN_BASENAME:
        return( GetMangledName( sym ) );
    case EXTN_PATTERN:
        return( GetNamePattern( sym ) );
    case EXTN_PRMSIZE:
        return( (const char *)(pointer_uint)GetParmsSize( sym ) );
    case EXTN_IMPPREFIX:
        return( ( TargetSystem == TS_NT ) ? "__imp_" : NULL );
    case EXTN_CALLBACKNAME:
        return( CallbackName( sym ) );
    default:
        return( NULL );
    }
}

#if _INTEL_CPU
static bool makeFileScopeStaticNear( SYMBOL sym )
{
    /*
     * make a file-scope static function near in big code models if:
     *   - address has not been taken
     *   - no debugging info is required
     *     (debug info would be wrong because type says far function)
     *   - multiple code segments are not used
     *   - function will not end up as FE_COMMON
     */
    if( sym->id != SYMC_STATIC ) {
        return( false );
    }
    if( ScopeId( SymScope( sym ) ) != SCOPE_FILE ) {
        return( false );
    }
    if( (sym->flag & SYMF_ADDR_TAKEN) != 0 ) {
        /*
         * function may be called as a FAR function through a pointer
         */
        return( false );
    }
    if( (GenSwitches & (CGSW_GEN_DBG_TYPES | CGSW_GEN_DBG_LOCALS)) != 0 ) {
        /*
         * function's debugging info will be FAR
         */
        return( false );
    }
    if( CompFlags.zm_switch_used ) {
        /*
         * caller may not be in the same segment
         */
        return( false );
    }
    if( SymIsComdatFun( sym ) ) {
        /*
         * another module may not satisfy previous conditions so depending
         * on what copy the linker uses, one of the calls will not match
         */
        return( false );
    }
    return( true );
}
#endif

static call_class getCallClass( SYMBOL sym )
/************************************************
 * handle only generic attributes for call class
 */
{
    AUX_INFO *inf;              // - aux info. for symbol
    TYPE fn_type;               // - function type
    type_flag flags;            // - flags for the function TYPE
    type_flag fn_flags;         // - flags in the function TYPE
    call_class cclass;          // - call class

    inf = getLangInfo( sym );
    cclass = inf->cclass;
    if( sym != NULL ) {
        if( SymIsFunction( sym ) ) {
#if _CPU == _AXP
            {
                #define SETJMP_MASK  ( \
                        ( 1 << SPFN_SETJMP ) | \
                        ( 1 << SPFN_SETJMP1 ) | \
                        ( 1 << SPFN_SETJMP2 ) | \
                        ( 1 << SPFN_SETJMP3 ) )
                SPFN fn_index;
                fn_index = SpecialFunction( sym );
                DbgAssert( fn_index < ( CHAR_BIT * sizeof( unsigned ) ));
                if( ( 1 << fn_index ) & SETJMP_MASK ) {
                    cclass |= FECALL_GEN_SETJMP_KLUGE;
                }
            }
#endif
            if( SymIsEllipsisFunc( sym ) ) {
                cclass |= FECALL_GEN_CALLER_POPS | FECALL_GEN_HAS_VARARGS;
            }
            if( CgBackFuncInlined( sym ) ) {
                cclass |= FECALL_GEN_MAKE_CALL_INLINE;
            }
            /*
             * only want the explicit memory model flags
             * default near/far is in the aux info already
             */
            fn_type = TypeGetActualFlags( sym->sym_type, &flags );
            fn_flags = fn_type->flag;
            if( fn_flags & TF1_ABORTS ) {
                cclass |= FECALL_GEN_ABORTS;
            }
            if( fn_flags & TF1_NORETURN ) {
                cclass |= FECALL_GEN_NORETURN;
            }
#if _INTEL_CPU
            /*
             * don't export addressability thunks
             */
            if( (sym->flag & SYMF_ADDR_THUNK) == 0 ) {
                if( flags & TF1_DLLEXPORT ) {
                    if( (fn_flags & TF1_INLINE) == 0 ) {
                        cclass |= FECALL_GEN_DLL_EXPORT;
                    }
                }
            }
#endif
        }
#ifdef REVERSE
        cclass &= ~ FECALL_GEN_REVERSE_PARMS;
#endif
    }
    return( cclass );
}

#if _INTEL_CPU
static call_class_target getCallClassTarget( SYMBOL sym )                        // - symbol
/********************************************************
 * handle only target specific attributes for call class
 */
{
    AUX_INFO *inf;                      // - aux info. for symbol
    TYPE fn_type;                       // - function type
    type_flag flags;                    // - flags for the function TYPE
    type_flag fn_flags;                 // - flags in the function TYPE
    call_class_target cclass_target;    // - call class

    inf = getLangInfo( sym );
    cclass_target = inf->cclass_target;
    if( sym != NULL ) {
        if( SymIsFunction( sym ) ) {
            fn_type = TypeGetActualFlags( sym->sym_type, &flags );
            if( flags & TF1_FAR ) {
                /*
                 * function has an explicit FAR
                 */
                cclass_target |= FECALL_X86_FAR_CALL;
            } else if( flags & TF1_NEAR ) {
                /*
                 * function has an explicit NEAR
                 */
                cclass_target &= ~FECALL_X86_FAR_CALL;
            } else if( flags & TF1_FAR16 ) {
                cclass_target |= FECALL_X86_FAR16_CALL;
            } else {
                if( IsBigCode() ) {
                    if( makeFileScopeStaticNear( sym ) ) {
                        cclass_target &= ~FECALL_X86_FAR_CALL;
                    }
                }
            }
            fn_flags = fn_type->flag;
            /*
             * don't export addressability thunks
             */
            if( (sym->flag & SYMF_ADDR_THUNK) == 0 ) {
                if( flags & TF1_DLLEXPORT ) {
                    if( fn_flags & TF1_INLINE ) {
                        /*
                         * may be COMDATed so make sure the calling convention
                         * matches what it would be for an exported fn
                         */
                        if( TargetSwitches & CGSW_X86_WINDOWS ) {
                            cclass_target |= FECALL_X86_FAT_WINDOWS_PROLOG;
                        }
                    }
                }
            }
            if( fn_flags & TF1_INTERRUPT ) {
                cclass_target |= FECALL_X86_INTERRUPT;
            }
            if( fn_flags & TF1_FARSS ) {
                cclass_target |= FECALL_X86_FARSS;
            }
            if( fn_flags & TF1_LOADDS ) {
                cclass_target |= FECALL_X86_LOAD_DS_ON_ENTRY;
            }
            if( CompFlags.emit_names ) {
                cclass_target |= FECALL_X86_EMIT_FUNCTION_NAME;
            }
    #if _CPU == 8086
            if( inf == &PascalInfo || inf == &CdeclInfo ) {
                if( TargetSwitches & CGSW_X86_WINDOWS ) {
                    cclass_target |= FECALL_X86_FAT_WINDOWS_PROLOG;
                }
            }
    #endif
            if( sym->flag & SYMF_FAR16_CALLER ) {
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
    }
    return( cclass_target );
}
#endif

static sym_access getSymAccess( // GET access flag of symbol
    SYMBOL sym )                // - symbol
{
    sym_access access;

    if( sym->flag & SYMF_PRIVATE ) {
        access = SYM_ACC_PRIVATE;
    } else if( sym->flag & SYMF_PROTECTED ) {
        access = SYM_ACC_PROTECTED;
    } else {
        access = SYM_ACC_PUBLIC;
    }
    return( access );
}

static void addDefaultLibs( void )
{
    if( CompFlags.emit_library_names ) {
        if( _HAS_ANY_MAIN || CompFlags.extern_C_defn_found
            || CompFlags.pragma_library || CompFlags.emit_all_default_libs ) {
#if _CPU == 386
            if( CompFlags.br_switch_used ) {
                CgInfoAddCompLib( CDLL_Name );
            } else
#endif
            if( _HAS_DLL_MAIN ) {
                CgInfoAddCompLib( DLL_CLIB_Name );
            } else {
                CgInfoAddCompLib( CLIB_Name );
            }
#if _CPU == 386
            if( CompFlags.br_switch_used ) {
                CgInfoAddCompLib( WCPPDLL_Name );
            } else
#endif
            if( _HAS_DLL_MAIN ) {
                CgInfoAddCompLib( DLL_WCPPLIB_Name );
            } else {
                CgInfoAddCompLib( WCPPLIB_Name );
            }
        }
        CgInfoAddCompLib( MATHLIB_Name );
#if _INTEL_CPU
        if( EmuLib_Name != NULL ) {
            CgInfoAddCompLib( EmuLib_Name );
        }
#endif
    }
}

void CgInfoAddPragmaExtrefS(    // ADD EXTREF FOR PRAGMA'D NAME
    SYMBOL sym )
{
    DbgVerify( NULL != sym, "CgInfoAddPragmaExtrefS -- null symbol" );
    CgInfoAddImportS( sym );
}

void CgInfoAddPragmaExtrefN(    // ADD EXTREF FOR PRAGMA'D NAME
    char *name )
{
    DbgVerify( NULL != name, "CgInfoAddPragmaExtrefN -- null name" );
    CgInfoAddImport( name );
}

static void addDefaultImports( void )
{
    typedef enum {
        CM_DLLMAIN      = 0x01,
        CM_WINMAIN      = 0x02,
        CM_MAIN         = 0x04,
        CM_NULL         = 0x00
    } check_mask;
    PragmaExtrefsInject();
#if _INTEL_CPU || ( _CPU == _AXP )
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
        if( control & CM_DLLMAIN ) {
            if( _HAS_DLL_MAIN ) {
                if( CompFlags.has_wide_char_main ) {
                    CgInfoAddImport( "__DLLstartw_" );
                } else {
                    CgInfoAddImport( "__DLLstart_" );
                }
                control = CM_NULL;
            }
        }
        if( control & CM_WINMAIN ) {
            if( CompFlags.has_winmain ) {
                if( CompFlags.has_wide_char_main ) {
                    CgInfoAddImport( "_wstartw_" );
                } else {
                    CgInfoAddImport( "_wstart_" );
                }
                control = CM_NULL;
            }
        }
        if( control & CM_MAIN ) {
            DbgAssert( CompFlags.has_main );
            if( CompFlags.has_wide_char_main ) {
                CgInfoAddImport( "_cstartw_" );
            } else {
                CgInfoAddImport( "_cstart_" );
            }
            control = CM_NULL;
        }
    }
#else
    #error bad system
#endif
    if( CompFlags.emit_library_names ) {
        if( CompFlags.float_used ) {
            CgInfoAddImport( "_fltused_" );
            CgInfoAddImport( "__ppfltused_" );
        }
        if( CompFlags.bm_switch_used ) {
            CgInfoAddImport( "__imthread" );
        }
#if _INTEL_CPU
    #if _CPU == 8086
        if( CompFlags.external_defn_found ) {
            if( IsBigCode() ) {
                CgInfoAddImport( "_big_code_" );
            } else {
                CgInfoAddImport( "_small_code_" );
            }
        }
    #endif
        if( CompFlags.pgm_used_8087 || CompFlags.float_used ) {
            if( CpuSwitches & FPU_EMU ) {
    #if _CPU == 8086
                CgInfoAddImport( "__init_87_emulator" );
    #else
                CgInfoAddImport( "__init_387_emulator" );
    #endif
            }
            if( GET_FPU( CpuSwitches ) > FPU_NONE ) {
                if( Stack87 == 4 ) {
                    CgInfoAddImport( "__old_8087" );
                } else {
                    CgInfoAddImport( "__8087" );
                }
            }
        }
#endif
    }
    if( CompFlags.main_has_parms ) {
#if _INTEL_CPU
    #if _CPU == 8086
        if( CompFlags.has_wide_char_main ) {
            CgInfoAddImport( "__wargc" );
        } else {
            CgInfoAddImport( "__argc" );
        }
    #else
        if( CompFlags.register_conventions ) {
            if( CompFlags.has_wide_char_main ) {
                CgInfoAddImport( "__wargc" );
            } else {
                CgInfoAddImport( "__argc" );
            }
        } else {
            if( CompFlags.has_wide_char_main ) {
                CgInfoAddImport( "_wargc" );
            } else {
                CgInfoAddImport( "_argc" );
            }
        }
    #endif
#elif _CPU == _AXP
        if( CompFlags.has_wide_char_main ) {
            CgInfoAddImport( "_wargc" );
        } else {
            CgInfoAddImport( "_argc" );
        }
#else
    #error missing _CPU case
#endif
    }
    if( CompFlags.bw_switch_used ) {
        CgInfoAddImport( "__init_default_win" );
    }
#if 0
    if( CompFlags.file_scope_dtors || CompFlags.fun_scope_static_dtors ) {
        CgInfoAddImportS( RunTimeCallSymbol( RTF_MOD_DTOR ) );
    }
#else
    if( CompFlags.genned_static_dtor ) {
        CgInfoAddImportS( RunTimeCallSymbol( RTF_MOD_DTOR ) );
    }
#endif
#if 0
    /*
     * not req'd with new library
     */
    if( CompFlags.inline_fun_reg ) {
        CgInfoAddImportS( RunTimeCallSymbol( RTF_INLINE_FREG ) );
    }
#endif
#if _INTEL_CPU
    if( CompFlags.has_longjmp && CompFlags.rw_registration && !CompFlags.fs_registration ) {
        CgInfoAddImportS( RunTimeCallSymbol( RTF_LONGJMP_REF ) );
    }
    if( CompFlags.has_main && ( (TargetSystem == TS_NETWARE) || (TargetSystem == TS_NETWARE5) ) ) {
        CgInfoAddImport( "__WATCOM_Prelude" );
    }
#endif
    if( _HAS_EXE_MAIN && CompFlags.rw_registration ) {
        CgInfoAddImportS( RunTimeCallSymbol( RTD_FS_ROOT ) );
    }
    if( CompFlags.excs_enabled ) {
#if _CPU == 386
        if( CompFlags.fs_registration ) {
            switch( TargetSystem ) {
            case TS_OS2 :
                CgInfoAddImport( RunTimeCodeString( RTD_TS_OS2 ) );
                break;
            case TS_NT :
                CgInfoAddImport( RunTimeCodeString( RTD_TS_NT ) );
                break;
            DbgDefault( "CGINFO -- bad fs o/s" );
            }
        } else {
            CgInfoAddImport( RunTimeCodeString( RTD_TS_GENERIC ) );
        }
#else
        CgInfoAddImport( RunTimeCodeString( RTD_TS_GENERIC ) );
#endif
    }
#if _CPU == 386
    if( TargetSwitches & CGSW_X86_NEW_P5_PROFILING ) {
        CgInfoAddImport( "__new_p5_profile" );
    } else if( TargetSwitches & CGSW_X86_P5_PROFILING ) {
        CgInfoAddImport( "__p5_profile" );
    }
#endif
}

#ifndef NDEBUG
    #define DbgNotSym() isSym = false;
    #define DbgNotRetn() isRetn = false;
#else
    #define DbgNotSym()
    #define DbgNotRetn()
#endif


void *FEAuxInfo(                // REQUEST AUXILLIARY INFORMATION
    void *_sym,                 // - symbol
    aux_class request )         // - request
{
    AUX_INFO *inf;              // - auxilary info
    void *retn = NULL;          // - return value
    static char *buf = NULL;    // - temporary buffer
    static SYMBOL dtor_sym;     // - symbol to be DTOR'ed
    static EXTRF res_info;      // - external-symbol resolution information
    SYMBOL sym = _sym;
#ifndef NDEBUG
    bool isSym = true;          // DEBUGGING: true ==> "sym" is SYMBOL
    bool isRetn = true;         // DEBUGGING: true ==> "retn" is SYMBOL
#endif

    if( buf != NULL )
        CMemFreePtr( &buf );
    inf = &DefaultInfo;
    switch( request ) {
    case FEINF_SOURCE_LANGUAGE:
        DbgNotSym();
        DbgNotRetn();
        retn = "CPP";
        break;
#if _INTEL_CPU
    case FEINF_P5_CHIP_BUG_SYM:
        DbgNotSym();
        retn = ChipBugSym;
        break;
    case FEINF_STACK_SIZE_8087:
        DbgNotSym();
        DbgNotRetn();
        retn = (void *)(pointer_uint)Stack87;
        break;
    case FEINF_CODE_GROUP:
        DbgNotSym();
        DbgNotRetn();
        retn = GenCodeGroup;
        break;
    case FEINF_DATA_GROUP:
        DbgNotSym();
        DbgNotRetn();
        retn = DataSegName;
        break;
#endif
    case FEINF_OBJECT_FILE_NAME:
        DbgNotSym();
        DbgNotRetn();
        retn = IoSuppOutFileName( OFT_OBJ );
        break;
    case FEINF_REVISION_NUMBER:
        DbgNotSym();
        DbgNotRetn();
        retn = (char *)II_REVISION;
        break;
    case FEINF_AUX_LOOKUP:
        retn = sym;
        break;
    case FEINF_DBG_PCH_SYM:
        DbgNotSym();
        retn = PCHDebugSym;
        break;
    case FEINF_DBG_PREDEF_SYM:
        DbgNotSym();
        retn = DFAbbrevSym;
        break;
    case FEINF_DBG_SYM_ACCESS:
        DbgNotRetn();
      { static sym_access access;
        access= getSymAccess( sym );
        retn = &access;
      } break;
#if _INTEL_CPU
    case FEINF_PROEPI_DATA_SIZE:
        DbgNotSym();
        DbgNotRetn();
        retn = (void *)(pointer_uint)ProEpiDataSize;
        break;
  #if _CPU != 8086
    case FEINF_P5_PROF_DATA:
        DbgNotSym();
        DbgNotRetn();
        retn = CgProfData();
        break;
    case FEINF_P5_PROF_SEG:
        DbgNotSym();
        DbgNotRetn();
        retn = (void *)SEG_PROF_REF;
        break;
  #endif
    case FEINF_CODE_LABEL_ALIGNMENT:
      {
        DbgNotSym();
        DbgNotRetn();
        retn = CgInfoCodeAlignment();
      } break;
    case FEINF_CLASS_NAME:
        DbgNotSym();
        DbgNotRetn();
        if( ((fe_seg_id)(pointer_uint)sym) == SEG_CODE ) {
            retn = CodeClassName;
        } else {
            retn = SegmentClassName( (fe_seg_id)(pointer_uint)sym );
        }
        break;
    case FEINF_USED_8087:
        DbgNotSym();
        DbgNotRetn();
        CompFlags.pgm_used_8087 = true;
        retn = NULL;
        break;
#endif
    case FEINF_SOURCE_NAME:
        DbgNotSym();
        DbgNotRetn();
        if( strcmp( SrcFName, ModuleName ) == 0 ) {
            SRCFILE src_file = SrcFileGetPrimary();
            if( src_file != NULL ) {
                retn = SrcFileFullName( src_file );
            } else {
                retn = IoSuppFullPath( WholeFName, Buffer, sizeof( Buffer ) );
            }
        } else {
            retn = ModuleName;
        }
        break;
    case FEINF_CALL_CLASS:
        DbgNotRetn();
        retn = (void *)getCallClass( sym );
        break;
#if _INTEL_CPU
    case FEINF_CALL_CLASS_TARGET:
        DbgNotRetn();
        retn = (void *)getCallClassTarget( sym );
        break;
#endif
    case FEINF_FREE_SEGMENT:
        DbgNotSym();
        DbgNotRetn();
        retn = NULL;
        break;
    case FEINF_NEXT_LIBRARY:
        DbgNotSym();
        DbgNotRetn();
        if( sym == NULL ) {
            addDefaultLibs();
        }
        retn = CgInfoLibNext( sym );
        break;
    case FEINF_LIBRARY_NAME:
        DbgNotSym();
        DbgNotRetn();
        retn = CgInfoLibName( sym );
        break;
    case FEINF_NEXT_IMPORT:
        DbgNotSym();
        DbgNotRetn();
        if( sym == NULL ) {
            addDefaultImports();
        }
        retn = CgInfoImportNext( sym );
        break;
    case FEINF_NEXT_IMPORT_S:
        DbgNotSym();
        DbgNotRetn();
        retn = CgInfoImportNextS( sym );
        break;
    case FEINF_IMPORT_NAME:
        DbgNotSym();
        DbgNotRetn();
        retn = (void *)CgInfoImportName( sym );
        break;
    case FEINF_IMPORT_NAME_S:
        DbgNotSym();
        DbgNotRetn();
        retn = CgInfoImportNameS( sym );
        break;
    case FEINF_SAVE_REGS:
      { static hw_reg_set save_set;
        TYPE type;
        DbgNotRetn();
        inf = getLangInfo( sym );
        save_set = inf->save;
        if( sym != NULL ) {
            type = FunctionDeclarationType( sym->sym_type );
            if( type != NULL && (type->flag & TF1_SAVEREGS) != 0 ) {
                HW_CTurnOn( save_set, HW_SEGS );
            }
        }
        retn = &save_set;
      } break;
    case FEINF_RETURN_REG:
        DbgNotRetn();
        inf = getLangInfo( sym );
        retn = &inf->returns;
        break;
    case FEINF_CALL_BYTES:
        DbgNotRetn();
        inf = getLangInfo( sym );
        retn = inf->code;
        break;
#if _INTEL_CPU
    case FEINF_STRETURN_REG:
        DbgNotRetn();
        inf = getLangInfo( sym );
        retn = &inf->streturn;
        break;
#endif
    case FEINF_PARM_REGS:
        DbgNotRetn();
        inf = getLangInfo( sym );
        if( inf->code == NULL && SymIsEllipsisFunc( sym ) ) {
            /*
             * so <stdarg.h> will work properly; all parms must be on the stack
             */
            retn = DefaultVarParms;
        } else {
            /*
             * (1) non ... functions
             * (2) ... functions that are #pragma code bursts
             */
            retn = inf->parms;
        }
        break;
    case FEINF_NEXT_DEPENDENCY :
        DbgNotSym();
        DbgNotRetn();
        if( !CompFlags.emit_dependencies ) {
            retn = NULL;
        } else {
            if( sym == NULL ) {
                retn = SrcFileWalkInit();
            } else {
                retn = SrcFileWalkNext( (SRCFILE)sym );
            }
            retn = SrcFileNotReadOnly( retn );
        }
        break;
    case FEINF_DEPENDENCY_TIMESTAMP :
        DbgNotSym();
        DbgNotRetn();
        retn = &(((SRCFILE)sym)->time_stamp);
        break;
    case FEINF_DEPENDENCY_NAME :
        DbgNotSym();
        DbgNotRetn();
        retn = SrcFileFullName( (SRCFILE)sym );
        break;
    case FEINF_TEMP_LOC_NAME :
        DbgNotRetn();
        dtor_sym = sym;
        retn = (void *)TEMP_LOC_YES;
        break;
    case FEINF_TEMP_LOC_TELL :
        DbgNotSym();
        DbgNotRetn();
        CgBackDtorAutoOffset( dtor_sym, (unsigned)(pointer_uint)sym );
        break;
    case FEINF_DEFAULT_IMPORT_RESOLVE :
        retn = ExtrefResolve( sym, &res_info );
  #ifndef NDEBUG
        if( TOGGLEDBG( extref ) ) {
            printf( "DEFAULT_IMPORT_RESOLVE[%p]: %s ==> %s\n", sym
                  , GetMangledName( sym )
                  , retn == NULL ? "0" : GetMangledName( retn ) );
        }
  #endif
        break;
    case FEINF_IMPORT_TYPE :
        DbgNotRetn();
        retn = ExtrefImportType( &res_info );
  #ifndef NDEBUG
        if( TOGGLEDBG( extref ) ) {
            printf( "  IMPORT_TYPE[%p]: %s <%p>\n"
                  , sym, GetMangledName( sym ), retn );
        }
  #endif
        break;
    case FEINF_CONDITIONAL_IMPORT :
    case FEINF_NEXT_CONDITIONAL :
        DbgNotSym();
        retn = ExtrefVirtualSymbol( &res_info );
  #ifndef NDEBUG
        if( TOGGLEDBG( extref ) ) {
            printf( "  NEXT_/CONDITIONAL/_IMPORT: %s\n"
                  , GetMangledName( retn ) );
        }
  #endif
        break;
    case FEINF_CONDITIONAL_SYMBOL :
        retn = sym;
  #ifndef NDEBUG
        if( TOGGLEDBG( extref ) ) {
            printf( "  CONDITIONAL_SYMBOL: %s\n"
                  , GetMangledName( retn ) );
        }
  #endif
        break;
    case FEINF_VIRT_FUNC_REFERENCE :
  #ifndef NDEBUG
        DbgNotRetn();
        if( ( TOGGLEDBG( extref ) )
          &&( sym->id == SYMC_VIRTUAL_FUNCTION ) ) {
            SYMBOL vsym;
            vsym = sym->u.virt_fun;
            printf( "VIRTUAL_FUNC_REFERENCE[%p]: %s"
                      , vsym, GetMangledName( vsym ) );
            retn = ExtrefVfunInfo( sym );
            printf( " <%p>\n", retn );
        } else {
            retn = ExtrefVfunInfo( sym );
        }
  #else
        retn = ExtrefVfunInfo( sym );
  #endif
        break;
    case FEINF_VIRT_FUNC_NEXT_REFERENCE:
        DbgNotSym();
        DbgNotRetn();
        retn = ExtrefNextVfunSym( sym );
  #ifndef NDEBUG
        if( TOGGLEDBG( extref ) ) {
            printf( "  VIRT_FUNC_NEXT_REFERENCE[%p]: <%p>\n", sym, retn );
        }
  #endif
        break;
    case FEINF_VIRT_FUNC_SYM :
        DbgNotSym();
        retn = ExtrefVfunSym( sym );
  #ifndef NDEBUG
        if( TOGGLEDBG( extref ) ) {
            printf( "  VIRT_FUNC_SYM[%p]: %s\n"
                  , sym, GetMangledName( retn ) );
        }
  #endif
        break;
#if _INTEL_CPU
    case FEINF_PEGGED_REGISTER :
        DbgNotSym();
        DbgNotRetn();
        retn = SegmentBoundReg( (fe_seg_id)(pointer_uint)sym );
        break;
#endif
    case FEINF_CLASS_APPENDED_NAME :
        DbgNotRetn();
        retn = CppClassPathDebug( sym );
        break;
#if _CPU == _AXP
    case FEINF_EXCEPTION_HANDLER: //based on sym return sym of exception handler
        DbgNotSym();
        retn = FstabExcHandler();
        break;
    case FEINF_EXCEPTION_DATA://based on sym return sym of exception data
        DbgNotSym();
        retn = FstabExcData();
        break;
#endif
    case FEINF_DBG_DWARF_PRODUCER:
        retn = DWARF_PRODUCER_ID;
        break;
    default :
        DbgNotSym();
        DbgNotRetn();
        retn = NULL;
        break;
    }
#ifndef NDEBUG
    if( TOGGLEDBG( auxinfo ) ) {
        printf( "FeAuxInfo( %p, %x ) -> %p\n", sym, request, retn );
        if( isSym && ( NULL != sym )) {
            printf( "  sym = %s\n", GetMangledName( sym ) );
        }
        if( isRetn && NULL != retn ) {
            printf( "  retn = %s\n", GetMangledName( retn ) );
        }
    }
#endif
    return( retn );
}


bool IsPragmaAborts(            // TEST IF FUNCTION NEVER RETURNS
    SYMBOL sym )                // - function symbol
{
    return( (getLangInfo( sym )->cclass & FECALL_GEN_ABORTS) != 0 );
}

bool IsFuncAborts(              // TEST IF FUNCTION NEVER RETURNS
    SYMBOL func_sym )           // - function symbol
{
    TYPE    func_type;

    func_type = FunctionDeclarationType( func_sym->sym_type );
    if( func_type != NULL )
        return( (func_type->flag & (TF1_NORETURN | TF1_ABORTS)) != 0 );
    return( false );
}

dbg_type FEDbgType(             // GET DEBUG TYPE FOR SYMBOL
    cg_sym_handle _sym )        // - symbol
{
    dbg_type ret;
    SYMBOL sym = _sym;

    if( GenSwitches & CGSW_GEN_DBG_DF ) {
        ret = DwarfDebugSym( sym );
    } else {
        ret = SymbolicDebugType( sym->sym_type, SD_DEFAULT );
    }
    return( ret );
}

dbg_type FEDbgRetType(           // GET DEBUG RETURN TYPE FOR SYMBOL
    cg_sym_handle _sym )         // - symbol (stubbed in at monent)
{
    TYPE type;
    dbg_type ret;
    SYMBOL sym = _sym;

    type = FunctionDeclarationType( sym->sym_type );
    if( type == NULL ) {
        type = sym->sym_type;
    } else {
        type = type->of;
    }
    if( GenSwitches & CGSW_GEN_DBG_DF ) {
        ret = DwarfDebugType( type );
    } else {
        ret = SymbolicDebugType( type, SD_DEFAULT );
    }
    return( ret );
}


const char *FEGetEnv(           // GET VALUE FOR ENV-VAR
    char const *name )          // - name of environment var
{
    return( CppGetEnv( name ) );
}

int FEMoreMem( size_t size )
/**************************/
{
    /* unused parameters */ (void)size;

    return( 0 );
}
