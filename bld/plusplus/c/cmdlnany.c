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
* Description:  Platform independent command line processing.
*
****************************************************************************/


#include "plusplus.h"
#include <ctype.h>
#include <errno.h>
#include "compcfg.h"
#include "wio.h"
#include "memmgr.h"
#include "preproc.h"
#include "macro.h"
#include "cmdline.h"
#include "context.h"
#include "scan.h"
#include "iosupp.h"
#include "fname.h"
#include "unicode.h"
#include "cgdata.h"
#include "pcheader.h"
#include "hfile.h"
#include "codegen.h"
#include "cgback.h"
#include "pragdefn.h"
#include "pdefn2.h"
#include "initdefs.h"
#include "brinfo.h"
#include "cbanner.h"
#include "cmdlnprs.gh"
#include "cmdlnprs.h"
#include "cmdlnsys.h"
#include "compinfo.h"
#include "toggles.h"
#ifndef NDEBUG
    #include "togglesd.h"
#endif

#include "clibext.h"


#define MAX_INDIRECTION                 5       // max. '@' indirections
static unsigned indirectionLevel;               // # '@' indirections

static void checkPacking( unsigned *p )
{
    *p = VerifyPackAmount( *p );
}

static void checkWarnLevel( unsigned *p )
{
    if( *p > WLEVEL_MAX ) {
        *p = WLEVEL_MAX;
    }
}

static void checkPPWidth( unsigned *p )
{
    *p = PpVerifyWidth( *p );
}

static void checkTabWidth( unsigned *p )
{
    switch( *p ) {
    case 2:
    case 4:
    case 8:
        break;
    default:
        *p = DEF_TAB_WIDTH;
    }
}

static void checkOENumber( unsigned *p )
{
    /* unused parameters */ (void)p;
}

static void checkPrologSize( unsigned *p )
{
    *p = _RoundUp( *p, TARGET_UINT );
}

static void checkErrorLimit( unsigned *p )
{
    /* unused parameters */ (void)p;
}

static bool scanDefine( OPT_STRING **p )
{
    MEPTR cmdln_mentry;

    /* unused parameters */ (void)p;

    cmdln_mentry = DefineCmdLineMacro( CompFlags.extended_defines );
    if( cmdln_mentry != NULL ) {
        cmdln_mentry->macro_flags |= MFLAG_USER_DEFINED;
    }
    return( true );
}

static bool scanDefinePlus( OPT_STRING **p )
{
    MEPTR cmdln_mentry;

    /* unused parameters */ (void)p;

    if( CmdScanSwEnd() ) {
        CompFlags.extended_defines = true;
    } else {
        cmdln_mentry = DefineCmdLineMacro( true );
        if( cmdln_mentry != NULL ) {
            cmdln_mentry->macro_flags |= MFLAG_USER_DEFINED;
        }
    }
    return( true );
}

static bool scanUndefine( OPT_STRING **p )
{
    /* unused parameters */ (void)p;

    AddUndefName();
    return( true );
}

#ifdef OPT_BR
typedef enum                    // BROWSE KINDS
{   FBI_VAR          = 0x01     // - variables
,   FBI_TYPE         = 0x02     // - types
,   FBI_MEMB_DATA    = 0x04     // - member data
,   FBI_FUN          = 0x08     // - functions
,   FBI_MACRO        = 0x10     // - macros
// derived
,   FBI_DEFAULT_ON
        = FBI_VAR | FBI_TYPE | FBI_MEMB_DATA | FBI_FUN | FBI_MACRO
,   FBI_DEFAULT_OFF
        = FBI_VAR | FBI_TYPE | FBI_MEMB_DATA | FBI_FUN | FBI_MACRO
} FBI_KIND;

static bool scanFBIopts         // SCAN FBI/FBX OPTIONS
    ( FBI_KIND* a_kind          // - addr[ option kinds ]
    , FBI_KIND def_kind )       // - default kind
{
    bool ok;                    // - return: true ==> ok
    FBI_KIND kind;              // - options scanned

    ok = false;
    kind = 0;
    CmdRecogEquals();
    for( ; ; ) {
        if( CmdScanSwEnd()
         || CmdPeekChar() == '-' ) {
            if( 0 == kind ) {
                kind = def_kind;
            }
            ok = true;
            break;
        }
        switch( CmdScanChar() ) {
        case 'v' :
            kind |= FBI_VAR;
            continue;
        case 't' :
            kind |= FBI_TYPE;
            continue;
        case 'f' :
            kind |= FBI_FUN;
            continue;
        case 'm' :
            kind |= FBI_MEMB_DATA;
            continue;
        case 'p' :
            kind |= FBI_MACRO;
            continue;
        default :
            BadCmdLine( ERR_INVALID_OPTION );
            ok = false;
            break;
        }
        break;
    }
    *a_kind = kind;
    return( ok );
}

#endif

static bool scanFBX( OPT_STRING **p )
{
#ifdef OPT_BR
    bool ok;                    // - return: true ==> ok
    FBI_KIND options;           // - options scanned

    /* unused parameters */ (void)p;

    if( scanFBIopts( &options, FBI_DEFAULT_OFF ) ) {
        if( options & FBI_VAR ) {
            CompFlags.optbr_v = false;
        }
        if( options & FBI_TYPE ) {
            CompFlags.optbr_t = false;
        }
        if( options & FBI_MEMB_DATA ) {
            CompFlags.optbr_m = false;
        }
        if( options & FBI_MACRO ) {
            CompFlags.optbr_p = false;
        }
        if( options & FBI_FUN ) {
            CompFlags.optbr_f = false;
        }
        ok = true;
    } else {
        ok = false;
    }
    return( ok );
#else
    /* unused parameters */ (void)p;

    BadCmdLine( ERR_INVALID_OPTION );
    return( false );
#endif
}

static bool scanFBI( OPT_STRING **p )
{
#ifdef OPT_BR
    bool ok;                    // - return: true ==> ok
    FBI_KIND options;           // - options scanned

    /* unused parameters */ (void)p;

    if( scanFBIopts( &options, FBI_DEFAULT_ON ) ) {
        if( options & FBI_VAR ) {
            CompFlags.optbr_v = true;
        }
        if( options & FBI_TYPE ) {
            CompFlags.optbr_t = true;
        }
        if( options & FBI_MEMB_DATA ) {
            CompFlags.optbr_m = true;
        }
        if( options & FBI_MACRO ) {
            CompFlags.optbr_p = true;
        }
        if( options & FBI_FUN ) {
            CompFlags.optbr_f = true;
        }
        ok = true;
    } else {
        ok = false;
    }
    return( ok );
#else
    /* unused parameters */ (void)p;

    BadCmdLine( ERR_INVALID_OPTION );
    return( false );
#endif
}

static void reverseList( OPT_STRING **h )
{
    OPT_STRING *s;
    OPT_STRING *p;
    OPT_STRING *n;

    s = *h;
    *h = NULL;
    for( p = s; p != NULL; p = n ) {
        n = p->next;
        p->next = *h;
        *h = p;
    }
}

static char *reduceToOneString( OPT_STRING **h )
{
    OPT_STRING *s;
    char *p;

    s = *h;
    if( s != NULL ) {
        if( s->data[0] != '\0' ) {
            *h = s->next;
            OPT_CLEAN_STRING( h );
            /* HACK: Whoever wrote this assumed that strcpy() did a byte-by-byte copy.
             *       GCC's version however may use DWORD-sized copies instead.
             *       If source and dest overlap there is NO guarantee of data integrity.
             *       To avoid corrupting the string, use memmove() instead.
             *
             *  NTS: So why can't we just go and return s->data instead of strcpy'ing
             *       over the struct with it's own string data? Why this bizarre code
             *       in the first place? --J.C. */
            {
                int l = strlen(s->data)+1; /* string + NUL */
                p = (char *)s;
                memmove(p,s->data,l);
            }
        } else {
            OPT_CLEAN_STRING( h );
            p = NULL;
        }
    } else {
        p = NULL;
    }
    return( p );
}

char *SetStringOption( char **o, OPT_STRING **h )
/***********************************************/
{
    char *p;

    p = reduceToOneString( h );
    if( o != NULL ) {
        CMemFree( *o );
        *o = p;
    }
    return( p );
}

bool MergeIncludeFromEnv( char *env )
/***********************************/
{
    const char  *env_value;

    if( CompFlags.cpp_ignore_env )
        return( false );

    env_value = CppGetEnv( env );
    if( NULL != env_value ) {
        HFileAppend( env_value );
        return( true );
    }
    return( false );
}

void DefSwitchMacro( char *n )
/****************************/
{
    char *p;
    char buff[64];

    p = stxpcpy( buff, "__SW_" );
    strcpy( p, n );
    PreDefineStringMacro( buff );
}

void ConcatBase10( char *buff, unsigned num )
/*******************************************/
{
    size_t len;
    char *dest;

    len = strlen( buff );
    dest = &buff[len];
    sprintf( dest, "%u", num );
}

static void setTarget( char **n, char *t )
{
    if( *n ) {
        CMemFree( *n );
    }
    *n = strupr( t );
}

void SetTargetLiteral( char **n, char *t )
/****************************************/
{
    if( t != NULL ) {
        setTarget( n, strsave( t ) );
    } else {
        *n = t;
    }
}

static void procOptions(        // PROCESS AN OPTIONS LINE
    OPT_STORAGE *data,          // - options data
    const char *str );          // - scan position in command line

static void handleOptionFC( OPT_STORAGE *data, int value )
{
    /* unused parameters */ (void)value;

    if( data->fc ) {
        data->fc = false;
        if( CompFlags.batch_file_primary ) {
            if( CompFlags.batch_file_processing ) {
                BadCmdLine( ERR_FC_IN_BATCH_FILE );
            } else {
                BadCmdLine( ERR_FC_MORE_THAN_ONCE );
            }
        } else {
            CompFlags.batch_file_primary = true;
            if( CompFlags.batch_file_processing ) {
                VBUF buf;
                if( CmdLnBatchRead( &buf ) ) {
                    CmdLnCtxPush( CTX_CLTYPE_FC );
                    procOptions( data, VbufString( &buf ) );
                    CmdLnCtxPop();
                }
                CmdLnBatchFreeRecord( &buf );
            } else {
                CmdLnBatchOpen( data->fc_value->data );
                CMemFreePtr( &data->fc_value );
            }
        }
    }
}

static void handleOptionEQ( OPT_STORAGE *data, bool value )
{
    /* unused parameters */ (void)data;

    CompFlags.eq_switch_used = value;
}

static void handleOptionEW( OPT_STORAGE *data, bool value )
{
    /* unused parameters */ (void)data;

    CompFlags.ew_switch_used = value;
}

#include "cmdlnprs.gc"

static bool openCmdFile(        // OPEN A COMMAND FILE
    char const *filename,       // - file name
    size_t size )               // - size of name
{
    char fnm[_MAX_PATH];        // - buffer for name

    stxvcpy( fnm, filename, size );
    StripQuotes( fnm );
    return( IoSuppOpenSrc( fnm, FT_CMD ) );
}

static const char *get_env(     // GET ENVIRONMENT VAR
    const char *var,            // - variable name
    size_t len )                // - length of name
{
    char        buf[128];       // - used to make a string
    const char  *env;           // - environment name

    if( len >= sizeof( buf ) ) {
        env = NULL;
    } else {
        stxvcpy( buf, var, len );
        env = CppGetEnv( buf );
    }
    return( env );
}

static void scanInputFile(       // PROCESS NAME OF INPUT FILE
    void )
{
    char filename[_MAX_PATH];   // - scanned file name
    size_t len;                 // - length of file name
    char const *fnm;            // - file name in command line

    len = CmdScanFilename( &fnm );
    ++CompInfo.compfile_max;
    if( CompInfo.compfile_max == CompInfo.compfile_cur ) {
        if( WholeFName == NULL ) {
            stxvcpy( filename, fnm, len );
            StripQuotes( filename );
            WholeFName = FNameAdd( filename );
        } else {
            CErr1( ERR_CAN_ONLY_COMPILE_ONE_FILE );
        }
    }
}


static void processCmdFile(     // PROCESS A COMMAND FILE
    OPT_STORAGE *data )         // - option data
{
    VBUF rec;                   // - record for file
    int c;                      // - next character

    VbufInit( &rec );
    for(;;) {
        for(;;) {
            c = NextChar();
            if( c == LCHR_EOF )
                break;
            if( c == '\n' )
                break;
            if( c == '\r' )
                break;
            VbufConcChr( &rec, (char)c );
        }
        procOptions( data, VbufString( &rec ) );
        for( ; ( c == '\n' ) || ( c == '\r' ); c = NextChar() );
        if( c == LCHR_EOF )
            break;
        VbufRewind( &rec );
        VbufConcChr( &rec, (char)c );
    }
    VbufFree( &rec );
}

static void procOptions(        // PROCESS AN OPTIONS LINE
    OPT_STORAGE *data,          // - options data
    const char *str )           // - scan position in command line
{
    int c;                      // - next character
    char const *fnm;            // - scanned @ name
    const char *env;            // - environment name
    size_t len;                 // - length of file name

    if( indirectionLevel >= MAX_INDIRECTION ) {
        BadCmdLine( ERR_MAX_CMD_INDIRECTION );
    } else if( str != NULL ) {
        ++indirectionLevel;
        CtxSetSwitchAddr( str );
        CmdScanInit( str );
        for(;;) {
            c = CmdScanWhiteSpace();
            if( c == '\0' )
                break;
            CmdScanSwitchBegin();
            CmdLnCtxSwitch( CmdScanAddr() - 1 );
            if( c == '-'  ||  c == SwitchChar ) {
                if( OPT_PROCESS( data ) ) {
                    BadCmdLine( ERR_INVALID_OPTION );
                }
            } else if( c == '@' ) {
                CmdScanWhiteSpace();
                CmdScanUngetChar();
                len = CmdScanFilename( &fnm );
                env = get_env( fnm, len );
                if( NULL == env ) {
                    if( openCmdFile( fnm, len ) ) {
                        CmdLnCtxPushCmdFile( SrcFileCurrent() );
                        processCmdFile( data );
                        CmdLnCtxPop();
                    } else {
                        CmdLnCtxPushEnv( fnm );
                        BadCmdLine( ERR_BAD_CMD_INDIRECTION );
                        CmdLnCtxPop();
                    }
                } else {
                    CmdLnCtxPushEnv( fnm );
                    procOptions( data, env );
                    CmdLnCtxPop();
                }
            } else {
                CmdScanUngetChar();
                scanInputFile();
            }
        }
        --indirectionLevel;
    }
}

static FILE *openUnicodeFile( char *filename )
{
    FILE *fp;
    char fullpath[_MAX_PATH];

#if defined(__QNX__)
    _searchenv( filename, "ETC_PATH", fullpath );
    if( fullpath[0] == '\0' ) {
        #define ETC "/usr/watcom"
        strcpy( fullpath, ETC );
        strcpy( &fullpath[sizeof( ETC ) - 1], filename );
    }
#else
    _searchenv( filename, "PATH", fullpath );
#endif
    fp = NULL;
    if( fullpath[0] != '\0' ) {
        fp = fopen( fullpath, "r" );
    }
    return( fp );
}

static void loadUnicodeTable( unsigned code_page )
{
    FILE *fp;
    char filename[20];

    sprintf( filename, "unicode.%3.3u", code_page );
    if( filename[11] != '\0' ) {
        filename[7] = filename[8];
        filename[8] = '.';
    }
    fp = openUnicodeFile( filename );
    if( fp != NULL ) {
        if( fread( UniCode, sizeof( unicode_type ), 256, fp ) != 256 ) {
            CErr( ERR_IO_ERR, filename, strerror( errno ) );
        }
        fclose( fp );
    } else {
        CErr2p( ERR_CANT_OPEN_FILE, filename );
    }
}

static bool debugOptionAfterOptOption( OPT_STORAGE *data )
{
    if( data->debug_info_timestamp > data->opt_level_timestamp ) {
        if( data->debug_info_timestamp > data->opt_size_time_timestamp ) {
            return( true );
        }
    }
    return( false );
}

static void analyseAnyTargetOptions( OPT_STORAGE *data )
{
    // quickly do the quiet option so the banner can be printed
    if( data->q || data->zq ) {
        CompFlags.quiet_mode = true;
    }
    switch( data->char_set ) {
    case OPT_ENUM_char_set_zku:
        CompFlags.use_unicode = true;
        loadUnicodeTable( data->zku_value );
        break;
    case OPT_ENUM_char_set_zk0u:
        CompFlags.jis_to_unicode = true;
        /* fall through */
    case OPT_ENUM_char_set_zk0:
        SetDBChar( 0 );
        break;
    case OPT_ENUM_char_set_zk1:
        SetDBChar( 1 );
        break;
    case OPT_ENUM_char_set_zk2:
        SetDBChar( 2 );
        break;
    case OPT_ENUM_char_set_zkl:
        SetDBChar( -1 );
        break;
    }
    switch( data->exc_level ) {
    case OPT_ENUM_exc_level_xs:
        CompFlags.excs_enabled = true;
        CompInfo.dt_method_speced = DTM_TABLE;
        break;
    case OPT_ENUM_exc_level_xst:
        CompFlags.excs_enabled = true;
        CompInfo.dt_method_speced = DTM_DIRECT_TABLE;
        break;
    case OPT_ENUM_exc_level_xss:
        CompFlags.excs_enabled = true;
        CompInfo.dt_method_speced = DTM_TABLE_SMALL;
        break;
    case OPT_ENUM_exc_level_xds:
        CompFlags.excs_enabled = false;
        CompInfo.dt_method_speced = DTM_DIRECT_SMALL;
        break;
    case OPT_ENUM_exc_level_xd:
    case OPT_ENUM_exc_level_xdt:
    default:
        CompFlags.excs_enabled = false;
        CompInfo.dt_method_speced = DTM_DIRECT;
        break;
    }
    switch( data->warn_level ) {
    case OPT_ENUM_warn_level_w:
        WngLevel = data->w_value;
        break;
    case OPT_ENUM_warn_level_wx:
        WngLevel = WLEVEL_WX;
        break;
    default:
        WngLevel = WLEVEL_DEFAULT;
        break;
    }
    switch( data->file_83 ) {
    case OPT_ENUM_file_83_fx:
        CompFlags.check_truncated_fnames = false;
        break;
    case OPT_ENUM_file_83_ft:
    default:
        CompFlags.check_truncated_fnames = true;
        break;
    }
    switch( data->opt_level ) {
    case OPT_ENUM_opt_level_ox:  /* -ox => -obmiler -s */
        GenSwitches &= ~ CGSW_GEN_NO_OPTIMIZATION;
        GenSwitches |= CGSW_GEN_BRANCH_PREDICTION;       // -ob
        GenSwitches |= CGSW_GEN_LOOP_OPTIMIZATION;       // -ol
        GenSwitches |= CGSW_GEN_INS_SCHEDULING;          // -or
        CmdSysSetMaxOptimization();             // -om
        CompFlags.inline_intrinsics = true;     // -oi
#if 0   // Disabled - introduces too many problems which no one is ready to fix
        if( ! data->oe ) {
            data->oe = 1;                       // -oe
            // keep in sync with options.gml
            data->oe_value = 100;
        }
#endif
        TOGGLE( check_stack ) = false;         // -s
        break;
    case OPT_ENUM_opt_level_od:
        GenSwitches |= CGSW_GEN_NO_OPTIMIZATION;
        break;
    }
    switch( data->opt_size_time ) {
    case OPT_ENUM_opt_size_time_ot:
        OptSize = 0;
        GenSwitches &= ~ CGSW_GEN_NO_OPTIMIZATION;
        break;
    case OPT_ENUM_opt_size_time_os:
        OptSize = 100;
        GenSwitches &= ~ CGSW_GEN_NO_OPTIMIZATION;
        break;
    default:
        OptSize = 50;
        break;
    }
    switch( data->iso ) {
    case OPT_ENUM_iso_za:
        CompFlags.extensions_enabled = false;
        CompFlags.non_iso_compliant_names_enabled = false;
        CompFlags.unique_functions = true;
        break;
    case OPT_ENUM_iso_ze:
    default:
        CompFlags.extensions_enabled = true;
        CompFlags.non_iso_compliant_names_enabled = true;
        break;
    }
    if( data->zam ) {
        CompFlags.non_iso_compliant_names_enabled = false;
    }
    // following must precede processing of data->oe
    switch( data->debug_info ) {
    case OPT_ENUM_debug_info_d3s:
        CompFlags.static_inline_fns = true;
        /* fall through */
    case OPT_ENUM_debug_info_d3i:
        CompFlags.inline_functions = false;
        /* fall through */
    case OPT_ENUM_debug_info_d3:
        // this flag may be turned on when PCHs are written if we will be
        // optimizing the writing of the debugging info by referring back
        // to the info in another module
        CompFlags.all_debug_type_names = true;
        GenSwitches |= CGSW_GEN_DBG_NUMBERS | CGSW_GEN_DBG_TYPES | CGSW_GEN_DBG_LOCALS;
        if( debugOptionAfterOptOption( data ) ) {
            GenSwitches |= CGSW_GEN_NO_OPTIMIZATION;
        }
        data->oe = 0;
        break;
    case OPT_ENUM_debug_info_d2s:
        CompFlags.static_inline_fns = true;
        /* fall through */
    case OPT_ENUM_debug_info_d2i:
        CompFlags.inline_functions = false;
        /* fall through */
    case OPT_ENUM_debug_info_d2:
        GenSwitches |= CGSW_GEN_DBG_NUMBERS | CGSW_GEN_DBG_TYPES | CGSW_GEN_DBG_LOCALS;
        if( debugOptionAfterOptOption( data ) ) {
            GenSwitches |= CGSW_GEN_NO_OPTIMIZATION;
        }
        data->oe = 0;
        break;
    case OPT_ENUM_debug_info_d2t:
        CompFlags.no_debug_type_names = true;
        GenSwitches |= CGSW_GEN_DBG_NUMBERS | CGSW_GEN_DBG_TYPES | CGSW_GEN_DBG_LOCALS;
        if( debugOptionAfterOptOption( data ) ) {
            GenSwitches |= CGSW_GEN_NO_OPTIMIZATION;
        }
        data->oe = 0;
        break;
    case OPT_ENUM_debug_info_d1:
        GenSwitches |= CGSW_GEN_DBG_NUMBERS;
        break;
    case OPT_ENUM_debug_info_d0:
        break;
    }
    switch( data->enum_size ) {
    case OPT_ENUM_enum_size_ei:
        CompFlags.make_enums_an_int = true;
        CompFlags.original_enum_setting = true;
        break;
    case OPT_ENUM_enum_size_em:
        CompFlags.make_enums_an_int = false;
        CompFlags.original_enum_setting = false;
        break;
    /* default set in CmdSysInit() */
    }
    if( data->bd ) {
        CompFlags.bd_switch_used = true;
        GenSwitches |= CGSW_GEN_DLL_RESIDENT_CODE;
    }
    if( data->bm ) {
        CompFlags.bm_switch_used = true;
    }
    if( data->bw ) {
        CompFlags.bw_switch_used = true;
    }
    if( data->bc ) {
        CompFlags.bc_switch_used = true;
    }
    if( data->bg ) {
        CompFlags.bg_switch_used = true;
    }
    if( data->db ) {
        CompFlags.emit_browser_info = true;
#if 0
        if( data->fhd ) {
            CompFlags.pch_debug_info_opt = true;
        }
#endif
    }
    if( data->ee ) {
        CompFlags.ee_switch_used = true;
    }
    if( data->ef ) {
        CompFlags.error_use_full = true;
    }
    if( data->ep ) {
        CompFlags.ep_switch_used = true;
        ProEpiDataSize = data->ep_value;
    }
    if( data->en ) {
        CompFlags.emit_names = true;
    }
    if( data->er ) {
        CompFlags.no_error_sym_injection = true;
    }
    if( data->ew ) {
        CompFlags.ew_switch_used = true;
    }
    if( data->e ) {
        ErrLimit = data->e_value;
    } else {
        ErrLimit = 20;
    }
    if( data->fhd ) {
        CompFlags.use_pcheaders = true;
    }
    if( data->fhr_exclamation ) {
        CompFlags.pch_min_check = true;
        data->fhr = true;
    }
    if( data->fhr ) {
        CompFlags.use_pcheaders = true;
        CompFlags.fhr_switch_used = true;
    }
    if( data->fhw ) {
        CompFlags.use_pcheaders = true;
        CompFlags.fhw_switch_used = true;
    }
    if( data->fhwe ) {
        CompFlags.use_pcheaders = true;
        CompFlags.fhwe_switch_used = true;
    }
    if( data->fh || data->fhq ) {
        char *fh_name;
        char *fhq_name;
        char *p;
        if( data->fhq ) {
            CompFlags.no_pch_warnings = true;
        }
        CompFlags.use_pcheaders = true;
        fh_name = reduceToOneString( &(data->fh_value) );
        fhq_name = reduceToOneString( &(data->fhq_value) );
        if( fh_name != NULL ) {
            p = fh_name;
            if( fhq_name != NULL ) {
                /* use the latest file-name specified */
                if( data->fh_timestamp > data->fhq_timestamp ) {
                    CMemFree( fhq_name );
                } else {
                    CMemFree( fh_name );
                    p = fhq_name;
                }
            }
        } else {
            p = fhq_name;
        }
        PCHSetFileName( p );
    }
    if( data->fi ) {
        SetStringOption( &ForceInclude, &(data->fi_value) );
    }
    if( data->fip ) {
        SetStringOption( &ForcePreInclude, &(data->fip_value) );
    }
    if( data->ad ) {
        SetStringOption( &DependFileName, &(data->ad_value) );
        CompFlags.generate_auto_depend = true;
    }
    if( data->adt ) {
        SetStringOption( &TargetFileName, &(data->adt_value) );
        CompFlags.generate_auto_depend = true;
    }
    if( data->add ) {
        SetStringOption( &SrcDepFileName, &(data->add_value) );
        CompFlags.generate_auto_depend = true;
    }
    if( data->adhp ) {
        SetStringOption( &DependHeaderPath, &(data->adhp_value) );
        CompFlags.generate_auto_depend = true;
    }
    if( data->adfs ) {
        ForceSlash = '/';
    }
    if( data->adbs ) {
        ForceSlash = '\\';
    }
    if( data->fo ) {
        SetStringOption( &ObjectFileName, &(data->fo_value) );
        CompFlags.cpp_output_to_file = true;    /* in case '-p' option */
    }
    if( data->fr ) {
        SetStringOption( &ErrorFileName, &(data->fr_value) );
    }
    if( data->i ) {
        OPT_STRING *s;
        reverseList( &(data->i_value) );
        for( s = data->i_value; s != NULL; s = s->next ) {
            HFileAppend( s->data );
        }
    }
    if( data->jw ) {
        data->j = true;
        CompFlags.plain_char_promotion = true;
    }
    if( data->j ) {
        PTypeSignedChar();
        CompFlags.signed_char = true;
    }
    if( data->k ) {
        CompFlags.batch_file_continue = true;
    }
    if( data->oa ) {
        GenSwitches |= CGSW_GEN_RELAX_ALIAS;
    }
    if( data->ob ) {
        GenSwitches |= CGSW_GEN_BRANCH_PREDICTION;
    }
    // following must follow processing of debug options
    if( data->oe ) {
        CgBackSetOeSize( data->oe_value );
    }
    if( data->oh ) {
        GenSwitches |= CGSW_GEN_SUPER_OPTIMAL;
    }
    if( data->oi ) {
        CompFlags.inline_intrinsics = true;
    }
    if( data->oi_plus ) {
        CgBackSetInlineDepth( MAX_INLINE_DEPTH );
#if 0
        // 98/01/08 -- this is disabled because it blows the code up too much
        //             and slows compiles down over much (jww)
        CgBackSetInlineRecursion( true );
#endif
    }
    if( data->ok ) {
        GenSwitches |= CGSW_GEN_FLOW_REG_SAVES;
    }
    if( data->ol ) {
        GenSwitches |= CGSW_GEN_LOOP_OPTIMIZATION;
    }
    if( data->ol_plus ) {
        GenSwitches |= CGSW_GEN_LOOP_UNROLLING;
    }
    if( data->on ) {
        GenSwitches |= CGSW_GEN_FP_UNSTABLE_OPTIMIZATION;
    }
    if( data->oo ) {
        GenSwitches &= ~ CGSW_GEN_MEMORY_LOW_FAILS;
    }
    if( data->op ) {
        CompFlags.op_switch_used = true;
    }
    if( data->or ) {
        GenSwitches |= CGSW_GEN_INS_SCHEDULING;
    }
    if( data->ou ) {
        CompFlags.unique_functions = true;
    }
    if( data->oz ) {
        GenSwitches |= CGSW_GEN_NULL_DEREF_OK;
    }
    if( data->pil ) {
        CompFlags.cpp_ignore_line = true;
    }
    if( data->p ) {
        CompFlags.cpp_mode = true;
    }
    if( data->pc ) {
        CompFlags.cpp_mode = true;
        CompFlags.cpp_keep_comments = true;
    }
    if( data->pe ) {
        CompFlags.cpp_mode = true;
        CompFlags.encrypt_preproc_output = true;
    }
    if( data->pj ) {
        data->pl = true;
        CompFlags.cpp_line_comments = true;
    }
    if( data->pl ) {
        CompFlags.cpp_mode = true;
        CompFlags.cpp_line_wanted = true;
    }
    if( data->pw ) {
        CompFlags.cpp_mode = true;
        PpSetWidth( data->pw_value );
    } else {
        // #line directives get screwed by wrapped lines but we don't want
        // to interfere with a user's setting of the width
        if( data->pl ) {
            PpSetWidth( 0 );
        }
    }
    if( CompFlags.cpp_mode ) {
        CompFlags.cpp_output = true;
        CompFlags.quiet_mode = true;
    }
    if( data->p_sharp ) {
        PreProcChar = (char)data->p_sharp_value;
    }
    if( data->rod ) {
        OPT_STRING *s;
        for( s = data->rod_value; s != NULL; s = s->next ) {
            SrcFileReadOnlyDir( s->data );
        }
    }
    if( data->s ) {
        TOGGLE( check_stack ) = false;
    }
    if( data->t ) {
        SrcFileSetTab( data->t_value );
    }
    if( data->v ) {
        CompFlags.dump_prototypes = true;
    }
    if( data->wcd ) {
        OPT_NUMBER *n;
        for( n = data->wcd_value; n != NULL; n = n->next ) {
            WarnEnableDisable( false, n->number );
        }
    }
    if( data->wce ) {
        OPT_NUMBER *n;
        for( n = data->wce_value; n != NULL; n = n->next ) {
            WarnEnableDisable( true, n->number );
        }
    }
    if( data->we ) {
        CompFlags.warnings_cause_bad_exit = true;
    }
    if( data->x ) {
        CompFlags.cpp_ignore_env = true;
    }
    if( data->xbnm ) {
        CompFlags.fixed_name_mangling = true;
    }
    if( data->xbsa ) {
        CompFlags.dont_align_segs = true;
    }
    if( data->xbov1 ) {
        CompFlags.overload_13332 = true;
    }
    if( data->xcmb ) {
        CompFlags.modifier_bind_compatibility = true;
    }
    if( data->xcpi ) {
        CompFlags.prototype_instantiate = true;
    }
    if( data->xr ) {
        CompFlags.rtti_enabled = true;
    }
    if( data->xto ) {
        CompFlags.obfuscate_typesig_names = true;
    }
    if( data->xx ) {
        CompFlags.ignore_default_dirs = true;
    }
    if( data->zat ) {
        CompFlags.no_alternative_tokens = true;
    }
    if( data->za0x ) {
        CompFlags.enable_std0x = true;
    }
    if( data->zf ) {
        CompFlags.use_old_for_scope = true;
    }
    if( data->zg ) {
        CompFlags.use_base_types = true;
    }
    if( data->zld ) {
        CompFlags.emit_dependencies = false;
    }
    if( data->zlf ) {
        CompFlags.emit_all_default_libs = true;
    }
    if( data->zls ) {
        CompFlags.emit_targimp_symbols = false;
    }
    if( data->fzh ) {   /* Define the switch macros for STLPort */
        CompFlags.dont_autogen_ext_inc = true;
        DefSwitchMacro( "FZH" );
    }
    if( data->fzs ) {   /* Define the switch macros for STLPort */
        CompFlags.dont_autogen_ext_src = true;
        DefSwitchMacro( "FZS" );
    }
    if( data->zl ) {
        CompFlags.emit_library_names = false;
    }
    if( data->zp ) {
        PackAmount = data->zp_value;
        GblPackAmount = PackAmount;
    }
    if( data->zpw ) {
        CompFlags.warn_about_padding = true;
    }
    if( data->zs ) {
        CompFlags.check_syntax = true;
    }
    if( data->zv ) {
#if COMP_CFG_COFF == 0
        CompFlags.virtual_stripping = true;
#endif
    }
#ifndef NDEBUG
    if( data->tp ) {
        OPT_STRING *str;
        while( (str = data->tp_value) != NULL ) {
            data->tp_value = str->next;
            PragmaSetToggle( str->data, 1, false );
            CMemFree( str );
        }
    }
    if( data->zi ) {
        CompFlags.extra_stats_wanted = true;
        // try to prevent distortions caused by debug stuff
        TOGGLEDBG( no_mem_cleanup ) = true;
    }
#endif
    CBanner();
}

void BadCmdLineChar( void )             // BAD CHAR DETECTED
{
    BadCmdLine( ERR_INVALID_OPTION_CHAR );
}
void BadCmdLineId( void )               // BAD ID DETECTED
{
    BadCmdLine( ERR_INVALID_OPTION_ID );
}
void BadCmdLineNumber( void )           // BAD NUMBER DETECTED
{
    BadCmdLine( ERR_INVALID_OPTION_NUMBER );
}
void BadCmdLinePath( void )             // BAD PATH DETECTED
{
    BadCmdLine( ERR_INVALID_OPTION_PATH );
}
void BadCmdLineFile( void )             // BAD FILE DETECTED
{
    BadCmdLine( ERR_INVALID_OPTION_FILE );
}

static void postOptions( void )
{
}

void GenCOptions(               // PROCESS ALL OPTIONS
    char **argv )               // - command line vector
{
    OPT_STORAGE data;
    char* env_var;              // - environment var for compiler

    indirectionLevel = 0;
    InitModInfo();
    CmdLnCtxInit();
    CmdSysInit();
    OPT_INIT( &data );
    if( !CompFlags.ignore_environment ) {
        CtxSetCurrContext( CTX_CMDLN_ENV );
        env_var = CmdSysEnvVar();
        CmdLnCtxPushEnv( env_var );
        procOptions( &data, CppGetEnv( env_var ) );
        CmdLnCtxPop();
    }
    CtxSetCurrContext( CTX_CMDLN_PGM );
    CmdLnCtxPush( CTX_CLTYPE_PGM );
    while( *argv != NULL ) {
        procOptions( &data, *argv );
        ++argv;
    }
    CmdLnCtxPop();
    CmdLnCtxFini();
    CtxSetCurrContext( CTX_CMDLN_VALID );
    analyseAnyTargetOptions( &data );
    CmdSysAnalyse( &data );
    postOptions();
    OPT_FINI( &data );
    MiscMacroDefs();
    DbgAssert( GblPackAmount == PackAmount );
    DbgAssert( GblPackAmount != 0 );
}

static void fini(               // FINALIZATION
    INITFINI* defn )            // - definition
{
    /* unused parameters */ (void)defn;

    CMemFreePtr( &ObjectFileName );
    CmdSysFini();
}

INITDEFN( cmdlnany, InitFiniStub, fini );


void MacroDefsSysind            // SYSTEM-INDEPENDENT MACRO DEFINITIONS
    ( void )
{
#ifdef OPT_BR
    if( BrinfActive() ) {
        DefSwitchMacro( "FBI" );
    } else {
        BrinfMacroRelease();
    }
#endif
}
