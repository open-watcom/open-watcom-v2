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
* Description:  Platform independent command line processing.
*
****************************************************************************/


#include "plusplus.h"

#include "compcfg.h"

#include <ctype.h>
#include "wio.h"

#include "memmgr.h"
#include "errdefns.h"
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

#include "cmdlnpr1.gh"
#include "cmdlnsys.h"


#define MAX_INDIRECTION                 5       // max. '@' indirections
static unsigned indirectionLevel;               // # '@' indirections

static void checkPacking( unsigned *p )
{
    *p = VerifyPackAmount( *p );
}

static void checkWarnLevel( unsigned *p )
{
    if( *p > 10 ) {
        *p = 10;
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
    p = p;
}

static void checkPrologSize( unsigned *p )
{
    *p = _RoundUp( *p, TARGET_UINT );
}

static void checkErrorLimit( unsigned *p )
{
    p = p;
}

static int scanDefine( OPT_STRING **p )
{
    MEPTR cmdln_mac;

    p = p;
    cmdln_mac = DefineCmdLineMacro( CompFlags.extended_defines );
    if( cmdln_mac != NULL ) {
        cmdln_mac->macro_flags |= MFLAG_USER_DEFINED;
    }
    return( 1 );
}

static int scanDefinePlus( OPT_STRING **p )
{
    MEPTR cmdln_mac;

    p = p;
    if( CmdScanSwEnd() ) {
        CompFlags.extended_defines = 1;
    } else {
        cmdln_mac = DefineCmdLineMacro( TRUE );
        if( cmdln_mac != NULL ) {
            cmdln_mac->macro_flags |= MFLAG_USER_DEFINED;
        }
    }
    return( 1 );
}

static int scanUndefine( OPT_STRING **p )
{
    p = p;
    AddUndefName();
    return( 1 );
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

static int scanFBIopts          // SCAN FBI/FBX OPTIONS
    ( FBI_KIND* a_kind          // - addr[ option kinds ]
    , FBI_KIND def_kind )       // - default kind
{
    int retn;                   // - return: 1 ==> ok, 0 ==> error
    FBI_KIND kind;              // - options scanned

    kind = 0;
    CmdRecogEquals();
    for( ; ; ) {
        if( CmdScanSwEnd()
         || CmdPeekChar() == '-' ) {
            if( 0 == kind ) {
                kind = def_kind;
            }
            retn = 1;
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
            retn = 0;
            break;
        }
        break;
    }
    *a_kind = kind;
    return retn;
}

#endif

static int scanFBX( OPT_STRING **p )
{
#ifdef OPT_BR
    int retn;                   // - return: 1 ==> ok, 0 ==> error
    FBI_KIND options;           // - options scanned

    p = p;
    if( scanFBIopts( &options, FBI_DEFAULT_OFF ) ) {
        if( options & FBI_VAR ) {
            CompFlags.optbr_v = FALSE;
        }
        if( options & FBI_TYPE ) {
            CompFlags.optbr_t = FALSE;
        }
        if( options & FBI_MEMB_DATA ) {
            CompFlags.optbr_m = FALSE;
        }
        if( options & FBI_MACRO ) {
            CompFlags.optbr_p = FALSE;
        }
        if( options & FBI_FUN ) {
            CompFlags.optbr_f = FALSE;
        }
        retn = 1;
    } else {
        retn = 0;
    }
    return retn;
#else
    p = p;
    BadCmdLine( ERR_INVALID_OPTION );
    return 0;
#endif
}

static int scanFBI( OPT_STRING **p )
{
#ifdef OPT_BR
    int retn;                   // - return: 1 ==> ok, 0 ==> error
    FBI_KIND options;           // - options scanned

    p = p;
    if( scanFBIopts( &options, FBI_DEFAULT_ON ) ) {
        if( options & FBI_VAR ) {
            CompFlags.optbr_v = TRUE;
        }
        if( options & FBI_TYPE ) {
            CompFlags.optbr_t = TRUE;
        }
        if( options & FBI_MEMB_DATA ) {
            CompFlags.optbr_m = TRUE;
        }
        if( options & FBI_MACRO ) {
            CompFlags.optbr_p = TRUE;
        }
        if( options & FBI_FUN ) {
            CompFlags.optbr_f = TRUE;
        }
        retn = 1;
    } else {
        retn = 0;
    }
    return retn;
#else
    p = p;
    BadCmdLine( ERR_INVALID_OPTION );
    return 0;
#endif
}

int OPT_GET_LOWER( void )
{
    return( CmdScanLowerChar() );
}

int OPT_RECOG_LOWER( int c )
{
    return( CmdRecogLowerChar( c ) );
}

int OPT_RECOG( int c )
{
    return( CmdRecogChar( c ) );
}

void OPT_UNGET( void )
{
    CmdScanUngetChar();
}

int OPT_END( void )
{
    return( CmdDelimitChar() );
}

static int scanOffNumber( unsigned *pvalue )
{
    int number_scanned;
    unsigned value;
    int c;

    CmdRecogEquals();
    number_scanned = 0;
    value = 0;
    for(;;) {
        c = CmdScanLowerChar();
        if( ! isdigit( c ) ) {
            CmdScanUngetChar();
            break;
        }
        value *= 10;
        value += c - '0';
        number_scanned = 1;
    }
    if( number_scanned ) {
        *pvalue = value;
    }
    return( number_scanned );
}

int OPT_GET_NUMBER_DEFAULT( unsigned *p, unsigned default_value )
{
    unsigned value;

    if( scanOffNumber( &value ) ) {
        *p = value;
    } else {
        *p = default_value;
    }
    return( 1 );
}

int OPT_GET_NUMBER( unsigned *p )
{
    unsigned value;

    if( scanOffNumber( &value ) ) {
        *p = value;
        return( 1 );
    }
    BadCmdLine( ERR_INVALID_OPTION_NUMBER );
    return( 0 );
}

static void addNumber( OPT_NUMBER **h, unsigned number )
{
    OPT_NUMBER *value;

    value = CMemAlloc( sizeof( *value ) );
    value->number = number;
    value->next = *h;
    *h = value;
}

int OPT_GET_NUMBER_MULTIPLE( OPT_NUMBER **h )
{
    unsigned value;

    if( scanOffNumber( &value ) ) {
        addNumber( h, value );
        return( 1 );
    }
    BadCmdLine( ERR_INVALID_OPTION_NUMBER );
    return( 0 );
}

int OPT_GET_CHAR( int *p )
{
    int c;

    if( ! OPT_END() ) {
        CmdRecogEquals();
        if( ! OPT_END() ) {
            c = CmdScanChar();
            if( isprint( c ) ) {
                *p = c;
                return( 1 );
            }
        }
    }
    BadCmdLine( ERR_INVALID_OPTION );
    return( 0 );
}

static void addString( OPT_STRING **h, char const *s, size_t len )
{
    OPT_STRING *value;

    value = CMemAlloc( sizeof( *value ) + len );
    stvcpy( value->data, s, len );
    value->next = *h;
    *h = value;
}

static void stripQuotes( char *fname )
{
    char *s;
    char *d;

    if( *fname == '"' ) {
        // string will shrink so we can reduce in place
        d = fname;
        for( s = d + 1; *s && *s != '"'; ++s ) {
            // collapse double backslashes, only then look for escaped quotes
            if( s[0] == '\\' && s[1] == '\\' ) {
                ++s;
            } else if( s[0] == '\\' && s[1] == '"' ) {
                ++s;
            }
            *d++ = *s;
        }
        *d = '\0';
    }
}

static void OPT_CLEAN_STRING( OPT_STRING **h )
{
    OPT_STRING *s;

    while( *h ) {
        s = *h;
        *h = s->next;
        CMemFree( s );
    }
}

static void OPT_CLEAN_NUMBER( OPT_NUMBER **h )
{
    OPT_NUMBER *s;

    while( *h ) {
        s = *h;
        *h = s->next;
        CMemFree( s );
    }
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
            p = (char*)s;
            strcpy( p, s->data );
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

int MergeIncludeFromEnv( char *env )
/**********************************/
{
    char *env_value;

    if( CompFlags.cpp_ignore_env )
        return( 0 );

    env_value = CppGetEnv( env );
    if( NULL != env_value ) {
        HFileAppend( env_value, strlen( env_value ) );
        return( 1 );
    }
    return( 0 );
}

void DefSwitchMacro( char *n )
/****************************/
{
    char *p;
    char buff[64];

    p = stpcpy( buff, "__SW_" );
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
    ultoa( num, dest, 10 );
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

int OPT_GET_ID( OPT_STRING **p )
{
    size_t len;
    char const *id;

    CmdRecogEquals();
    CmdScanChar();
    len = CmdScanId( &id );
    if( len != 0 ) {
        addString( p, id, len );
        return( 1 );
    }
    BadCmdLine( ERR_INVALID_OPTION_ID );
    return( 0 );
}

int OPT_GET_ID_OPT( OPT_STRING **p )
{
    if( CmdRecogEquals() || ! CmdDelimitChar() ) {
        return OPT_GET_ID( p );
    }
    addString( p, "", 0 );
    return( 1 );
}

int OPT_GET_FILE( OPT_STRING **p )
{
    size_t len;
    char const *fname;

    CmdRecogEquals();
    len = CmdScanFilename( &fname );
    if( len != 0 ) {
        addString( p, fname, len );
        stripQuotes( (*p)->data );
        return( 1 );
    }
    BadCmdLine( ERR_INVALID_OPTION_FILE );
    return( 0 );
}

int OPT_GET_FILE_OPT( OPT_STRING **p )
{
    size_t len;
    char const *fname;

    // handle leading option char specially
    if( CmdRecogEquals() || ! CmdDelimitChar() ) {
        // specified an '=' so accept -this-is-a-file-name.fil or /tmp/ack.tmp
        len = CmdScanFilename( &fname );
        if( len != 0 ) {
            addString( p, fname, len );
            stripQuotes( (*p)->data );
        } else {
            OPT_CLEAN_STRING( p );
        }
    }
    return( 1 );
}

int OPT_GET_PATH( OPT_STRING **p )
{
    size_t len;
    char const *path;

    CmdRecogEquals();
    len = CmdScanFilename( &path );
    if( len != 0 ) {
        addString( p, path, len );
        stripQuotes( (*p)->data );
        return( 1 );
    }
    BadCmdLine( ERR_INVALID_OPTION_PATH );
    return( 0 );
}

static void handleOptionEW( OPT_STORAGE *data, int value )
{
    data = data;
    CompFlags.ew_switch_used = value;
}

static void handleOptionEQ( OPT_STORAGE *data, int value )
{
    data = data;
    CompFlags.eq_switch_used = value;
}

static void procOptions(        // PROCESS AN OPTIONS LINE
    OPT_STORAGE *data,          // - options data
    char *str );                // - scan position in command line

static void handleOptionFC( OPT_STORAGE *data, int value )
{
    value = value;
    if( data->fc ) {
        data->fc = FALSE;
        if( CompFlags.batch_file_primary ) {
            if( CompFlags.batch_file_processing ) {
                BadCmdLine( ERR_FC_IN_BATCH_FILE );
            } else {
                BadCmdLine( ERR_FC_MORE_THAN_ONCE );
            }
        } else {
            CompFlags.batch_file_primary = TRUE;
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

#include "cmdlnpr2.gh"

static boolean openCmdFile(     // OPEN A COMMAND FILE
    char const *filename,       // - file name
    size_t size )               // - size of name
{
    char fnm[ _MAX_PATH ];      // - buffer for name

    stvcpy( fnm, filename, size );
    stripQuotes( fnm );
    return IoSuppOpenSrc( fnm, FT_CMD );
}

static char *get_env(           // GET ENVIRONMENT VAR
    const char *var,            // - variable name
    unsigned len )              // - length of name
{
    char buf[128];              // - used to make a string
    char *env;                  // - environment name

    if( len >= sizeof( buf ) ) {
        env = NULL;
    } else {
        stvcpy( buf, var, len );
        env = CppGetEnv( buf );
    }
    return env;
}

static void scanInputFile(       // PROCESS NAME OF INPUT FILE
    void )
{
    char filename[ _MAX_PATH ]; // - scanned file name
    size_t len;                 // - length of file name
    char const *fnm;            // - file name in command line

    len = CmdScanFilename( &fnm );
    ++CompInfo.compfile_max;
    if( CompInfo.compfile_max == CompInfo.compfile_cur ) {
        if( WholeFName == NULL ) {
            stvcpy( filename, fnm, len );
            stripQuotes( filename );
            WholeFName = FNameAdd( filename );
        } else {
            CErr1( ERR_CAN_ONLY_COMPILE_ONE_FILE );
        }
    }
}


static void processCmdFile(     // PROCESS A COMMAND FILE
    OPT_STORAGE *data )         // - option data
;


static void procOptions(        // PROCESS AN OPTIONS LINE
    OPT_STORAGE *data,          // - options data
    char *str )                 // - scan position in command line
{
    int c;                      // - next character
    char const *fnm;            // - scanned @ name
    char *env;                  // - environment name
    unsigned len;               // - length of file name

    if( indirectionLevel >= MAX_INDIRECTION ) {
        BadCmdLine( ERR_MAX_CMD_INDIRECTION );
    } else if( str != NULL ) {
        ++ indirectionLevel;
        CtxSwitchAddr( str );
        CmdScanInit( str );
        for(;;) {
            c = CmdScanWhiteSpace();
            if( c == '\0' ) break;
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
        -- indirectionLevel;
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
            if( c == LCHR_EOF ) break;
            if( c == '\n' ) break;
            if( c == '\r' ) break;
            VbufConcChr( &rec, c );
        }
        procOptions( data, VbufString( &rec ) );
        for( ; ( c == '\n' ) || ( c == '\r' ); c = NextChar() );
        if( c == LCHR_EOF ) break;
        VbufRewind( &rec );
        VbufConcChr( &rec, c );
    }
    VbufFree( &rec );
}

static int openUnicodeFile( char *filename )
{
    int fh;
    char fullpath[ _MAX_PATH ];

#if defined(__QNX__)
    _searchenv( filename, "ETC_PATH", fullpath );
    if( fullpath[0] == '\0' ) {
        #define ETC "/usr/watcom"
        strcpy( fullpath, ETC );
        strcpy( &fullpath[ sizeof( ETC ) - 1 ], filename );
    }
#else
    _searchenv( filename, "PATH", fullpath );
#endif
    fh = -1;
    if( fullpath[0] != '\0' ) {
        fh = open( fullpath, O_RDONLY | O_BINARY );
    }
    return( fh );
}

static void loadUnicodeTable( unsigned code_page )
{
    size_t amt;
    int fh;
    char filename[ 20 ];

    sprintf( filename, "unicode.%3.3u", code_page );
    if( filename[ 11 ] != '\0' ) {
        filename[ 7 ] = filename[ 8 ];
        filename[ 8 ] = '.';
    }
    fh = openUnicodeFile( filename );
    if( fh != -1 ) {
        amt = 256 * sizeof( unsigned short );
        if( read( fh, UniCode, amt ) != amt ) {
            CErr( ERR_IO_ERR, filename, strerror( errno ) );
        }
        close( fh );
    } else {
        CErr2p( ERR_CANT_OPEN_FILE, filename );
    }
}

static int debugOptionAfterOptOption( OPT_STORAGE *data )
{
    if( data->debug_info_timestamp > data->opt_level_timestamp ) {
        if( data->debug_info_timestamp > data->opt_size_time_timestamp ) {
            return( 1 );
        }
    }
    return( 0 );
}

static void analyseAnyTargetOptions( OPT_STORAGE *data )
{
    // quickly do the quiet option so the banner can be printed
    if( data->q || data->zq ) {
        CompFlags.quiet_mode = 1;
    }
    switch( data->char_set ) {
    case OPT_char_set_zku:
        CompFlags.use_unicode = 1;
        loadUnicodeTable( data->zku_value );
        break;
    case OPT_char_set_zk0u:
        CompFlags.jis_to_unicode = 1;
        /* fall through */
    case OPT_char_set_zk0:
        SetDBChar( 0 );
        break;
    case OPT_char_set_zk1:
        SetDBChar( 1 );
        break;
    case OPT_char_set_zk2:
        SetDBChar( 2 );
        break;
    case OPT_char_set_zkl:
        SetDBChar( -1 );
        break;
    }
    switch( data->exc_level ) {
    case OPT_exc_level_xs:
        CompFlags.excs_enabled = TRUE;
        CompInfo.dt_method_speced = DTM_TABLE;
        break;
    case OPT_exc_level_xst:
        CompFlags.excs_enabled = TRUE;
        CompInfo.dt_method_speced = DTM_DIRECT_TABLE;
        break;
    case OPT_exc_level_xss:
        CompFlags.excs_enabled = TRUE;
        CompInfo.dt_method_speced = DTM_TABLE_SMALL;
        break;
    case OPT_exc_level_xds:
        CompFlags.excs_enabled = FALSE;
        CompInfo.dt_method_speced = DTM_DIRECT_SMALL;
        break;
    case OPT_exc_level_xd:
    case OPT_exc_level_xdt:
    default:
        CompFlags.excs_enabled = FALSE;
        CompInfo.dt_method_speced = DTM_DIRECT;
        break;
    }
    switch( data->warn_level ) {
    case OPT_warn_level_w:
        WngLevel = data->w_value;
        break;
    case OPT_warn_level_wx:
        WngLevel = WLEVEL_MAX;
        break;
    default:
        WngLevel = WLEVEL_DEFAULT;
        break;
    }
    switch( data->file_83 ) {
    case OPT_file_83_fx:
        CompFlags.check_truncated_fnames = 0;
        break;
    case OPT_file_83_ft:
    default:
        CompFlags.check_truncated_fnames = 1;
        break;
    }
    switch( data->opt_level ) {
    case OPT_opt_level_ox:  /* -ox => -obmiler -s */
        GenSwitches &= ~ NO_OPTIMIZATION;
        GenSwitches |= BRANCH_PREDICTION;       // -ob
        GenSwitches |= LOOP_OPTIMIZATION;       // -ol
        GenSwitches |= INS_SCHEDULING;          // -or
        CmdSysSetMaxOptimization();             // -om
        CompFlags.inline_intrinsics = 1;        // -oi
#if 0   // Disabled - introduces too many problems which no one is ready to fix
        if( ! data->oe ) {
            data->oe = 1;                       // -oe
            // keep in sync with options.gml
            data->oe_value = 100;
        }
#endif
        PragToggle.check_stack = 0;             // -s
        break;
    case OPT_opt_level_od:
        GenSwitches |= NO_OPTIMIZATION;
        break;
    }
    switch( data->opt_size_time ) {
    case OPT_opt_size_time_ot:
        OptSize = 0;
        GenSwitches &= ~ NO_OPTIMIZATION;
        break;
    case OPT_opt_size_time_os:
        OptSize = 100;
        GenSwitches &= ~ NO_OPTIMIZATION;
        break;
    default:
        OptSize = 50;
        break;
    }
    switch( data->iso ) {
    case OPT_iso_za:
        CompFlags.extensions_enabled = 0;
        CompFlags.unique_functions = 1;
        break;
    case OPT_iso_ze:
    default:
        CompFlags.extensions_enabled = 1;
        break;
    }
    // following must precede processing of data->oe
    switch( data->debug_info ) {
    case OPT_debug_info_d3s:
        CompFlags.static_inline_fns = 1;
        /* fall through */
    case OPT_debug_info_d3i:
        CompFlags.inline_functions = 0;
        /* fall through */
    case OPT_debug_info_d3:
        // this flag may be turned on when PCHs are written if we will be
        // optimizing the writing of the debugging info by referring back
        // to the info in another module
        CompFlags.all_debug_type_names = 1;
        GenSwitches |= NUMBERS | DBG_TYPES | DBG_LOCALS;
        if( debugOptionAfterOptOption( data ) ) {
            GenSwitches |= NO_OPTIMIZATION;
        }
        data->oe = 0;
        break;
    case OPT_debug_info_d2s:
        CompFlags.static_inline_fns = 1;
        /* fall through */
    case OPT_debug_info_d2i:
        CompFlags.inline_functions = 0;
        /* fall through */
    case OPT_debug_info_d2:
        GenSwitches |= NUMBERS | DBG_TYPES | DBG_LOCALS;
        if( debugOptionAfterOptOption( data ) ) {
            GenSwitches |= NO_OPTIMIZATION;
        }
        data->oe = 0;
        break;
    case OPT_debug_info_d2t:
        CompFlags.no_debug_type_names = 1;
        GenSwitches |= NUMBERS | DBG_TYPES | DBG_LOCALS;
        if( debugOptionAfterOptOption( data ) ) {
            GenSwitches |= NO_OPTIMIZATION;
        }
        data->oe = 0;
        break;
    case OPT_debug_info_d1:
        GenSwitches |= NUMBERS;
        break;
    case OPT_debug_info_d0:
        break;
    }
    switch( data->enum_size ) {
    case OPT_enum_size_ei:
        CompFlags.make_enums_an_int = 1;
        CompFlags.original_enum_setting = 1;
        break;
    case OPT_enum_size_em:
        CompFlags.make_enums_an_int = 0;
        CompFlags.original_enum_setting = 0;
        break;
    /* default set in CmdSysInit() */
    }
    if( data->bd ) {
        CompFlags.bd_switch_used = 1;
        GenSwitches |= DLL_RESIDENT_CODE;
    }
    if( data->bm ) {
        CompFlags.bm_switch_used = 1;
    }
    if( data->bw ) {
        CompFlags.bw_switch_used = 1;
    }
    if( data->bc ) {
        CompFlags.bc_switch_used = 1;
    }
    if( data->bg ) {
        CompFlags.bg_switch_used = 1;
    }
    if( data->db ) {
        CompFlags.emit_browser_info = 1;
#if 0
        if( data->fhd ) {
            CompFlags.pch_debug_info_opt = 1;
        }
#endif
    }
    if( data->ee ) {
        CompFlags.ee_switch_used = 1;
    }
    if( data->ef ) {
        CompFlags.error_use_full = 1;
    }
    if( data->ep ) {
        CompFlags.ep_switch_used = 1;
        ProEpiDataSize = data->ep_value;
    }
    if( data->en ) {
        CompFlags.emit_names = 1;
    }
    if( data->er ) {
        CompFlags.no_error_sym_injection = 1;
    }
    if( data->ew ) {
        CompFlags.ew_switch_used = 1;
    }
    if( data->e ) {
        ErrLimit = data->e_value;
    } else {
        ErrLimit = 20;
    }
    if( data->fhd ) {
        CompFlags.use_pcheaders = TRUE;
    }
    if( data->fhr_exclamation ) {
        CompFlags.pch_min_check = TRUE;
        data->fhr = TRUE;
    }
    if( data->fhr ) {
        CompFlags.use_pcheaders = TRUE;
        CompFlags.fhr_switch_used = TRUE;
    }
    if( data->fhw ) {
        CompFlags.use_pcheaders = TRUE;
        CompFlags.fhw_switch_used = TRUE;
    }
    if( data->fhwe ) {
        CompFlags.use_pcheaders = TRUE;
        CompFlags.fhwe_switch_used = TRUE;
    }
    if( data->fh || data->fhq ) {
        char *fh_name;
        char *fhq_name;
        char *p;
        if( data->fhq ) {
            CompFlags.no_pch_warnings = TRUE;
        }
        CompFlags.use_pcheaders = TRUE;
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
    if( data->ad ) {
        SetStringOption( &DependFileName, &(data->ad_value) );
        CompFlags.generate_auto_depend = 1;
    }
    if( data->adt ) {
        SetStringOption( &TargetFileName, &(data->adt_value) );
        CompFlags.generate_auto_depend = 1;
    }
    if( data->add ) {
        SetStringOption( &SrcDepFileName, &(data->add_value) );
        CompFlags.generate_auto_depend = 1;
    }
    if( data->adhp ) {
        SetStringOption( &DependHeaderPath, &(data->adhp_value) );
        CompFlags.generate_auto_depend = 1;
    }
    if( data->adfs ) {
        ForceSlash = '/';
    }
    if( data->adbs ) {
        ForceSlash = '\\';
    }
    if( data->fo ) {
        SetStringOption( &ObjectFileName, &(data->fo_value) );
        CompFlags.cpp_output_to_file = 1;   /* in case '-p' option */
    }
    if( data->fr ) {
        SetStringOption( &ErrorFileName, &(data->fr_value) );
    }
    if( data->i ) {
        OPT_STRING *s;
        reverseList( &(data->i_value) );
        for( s = data->i_value; s != NULL; s = s->next ) {
            HFileAppend( s->data, strlen( s->data ) );
        }
    }
    if( data->jw ) {
        data->j = TRUE;
        CompFlags.plain_char_promotion = 1;
    }
    if( data->j ) {
        PTypeSignedChar();
        CompFlags.signed_char = 1;
    }
    if( data->k ) {
        CompFlags.batch_file_continue = 1;
    }
    if( data->oa ) {
        GenSwitches |= RELAX_ALIAS;
    }
    if( data->ob ) {
        GenSwitches |= BRANCH_PREDICTION;
    }
    // following must follow processing of debug options
    if( data->oe ) {
        CgBackSetOeSize( data->oe_value );
    }
    if( data->oh ) {
        GenSwitches |= SUPER_OPTIMAL;
    }
    if( data->oi ) {
        CompFlags.inline_intrinsics = 1;
    }
    if( data->oi_plus ) {
        CgBackSetInlineDepth( MAX_INLINE_DEPTH );
#if 0
        // 98/01/08 -- this is disabled because it blows the code up too much
        //             and slows compiles down over much (jww)
        CgBackSetInlineRecursion( TRUE );
#endif
    }
    if( data->ok ) {
        GenSwitches |= FLOW_REG_SAVES;
    }
    if( data->ol ) {
        GenSwitches |= LOOP_OPTIMIZATION;
    }
    if( data->ol_plus ) {
        GenSwitches |= LOOP_UNROLLING;
    }
    if( data->on ) {
        GenSwitches |= FP_UNSTABLE_OPTIMIZATION;
    }
    if( data->oo ) {
        GenSwitches &= ~ MEMORY_LOW_FAILS;
    }
    if( data->op ) {
        CompFlags.op_switch_used = 1;
    }
    if( data->or ) {
        GenSwitches |= INS_SCHEDULING;
    }
    if( data->ou ) {
        CompFlags.unique_functions = 1;
    }
    if( data->oz ) {
        GenSwitches |= NULL_DEREF_OK;
    }
    if( data->pil ) {
        CompFlags.cpp_ignore_line = 1;
    }
    if( data->p ) {
        CompFlags.cpp_output_requested = 1;
    }
    if( data->pc ) {
        CompFlags.cpp_output_requested = 1;
        CompFlags.keep_comments = 1;
        CompFlags.comments_wanted = 1;
    }
    if( data->pe ) {
        CompFlags.cpp_output_requested = 1;
        CompFlags.encrypt_preproc_output = 1;
    }
    if( data->pj ) {
        data->pl = TRUE;
        CompFlags.line_comments = 1;
    }
    if( data->pl ) {
        CompFlags.cpp_output_requested = 1;
        CompFlags.cpp_line_wanted = 1;
    }
    if( data->pw ) {
        CompFlags.cpp_output_requested = 1;
        PpSetWidth( data->pw_value );
    } else {
        // #line directives get screwed by wrapped lines but we don't want
        // to interfere with a user's setting of the width
        if( data->pl ) {
            PpSetWidth( 0 );
        }
    }
    if( CompFlags.cpp_output_requested ) {
        CompFlags.cpp_output = 1;
        CompFlags.quiet_mode = 1;
    }
    if( data->p_sharp ) {
        PreProcChar = data->p_sharp_value;
    }
    if( data->rod ) {
        OPT_STRING *s;
        for( s = data->rod_value; s != NULL; s = s->next ) {
            SrcFileReadOnlyDir( s->data );
        }
    }
    if( data->s ) {
        PragToggle.check_stack = 0;
    }
    if( data->t ) {
        SrcFileSetTab( data->t_value );
    }
    if( data->v ) {
        CompFlags.dump_prototypes = 1;
    }
    if( data->wcd ) {
        OPT_NUMBER *n;
        for( n = data->wcd_value; n != NULL; n = n->next ) {
            WarnChangeLevel( WLEVEL_DISABLE, n->number );
        }
    }
    if( data->wce ) {
        OPT_NUMBER *n;
        for( n = data->wce_value; n != NULL; n = n->next ) {
            WarnChangeLevel( WLEVEL_ENABLE, n->number );
        }
    }
    if( data->we ) {
        CompFlags.warnings_cause_bad_exit = 1;
    }
    if( data->x ) {
        CompFlags.cpp_ignore_env = 1;
    }
    if( data->xbnm ) {
        CompFlags.fixed_name_mangling = 1;
    }
    if( data->xbsa ) {
        CompFlags.dont_align_segs = 1;
    }
    if( data->xbov1 ) {
        CompFlags.overload_13332 = 1;
    }
    if( data->xcmb ) {
        CompFlags.modifier_bind_compatibility = 1;
    }
    if( data->xcpi ) {
        CompFlags.prototype_instantiate = 1;
    }
    if( data->xr ) {
        CompFlags.rtti_enabled = 1;
    }
    if( data->xto ) {
        CompFlags.obfuscate_typesig_names = 1;
    }
    if( data->zat ) {
        CompFlags.no_alternative_tokens = 1;
    }
    if( data->za0x ) {
        CompFlags.enable_std0x = 1;
    }
    if( data->zf ) {
        CompFlags.use_old_for_scope = 1;
    }
    if( data->zg ) {
        CompFlags.use_base_types = 1;
    }
    if( data->zld ) {
        CompFlags.emit_dependencies = 0;
    }
    if( data->zlf ) {
        CompFlags.emit_all_default_libs = 1;
    }
    if( data->zls ) {
        CompFlags.emit_targimp_symbols = 0;
    }
    if( data->fzh ) {   /* Define the switch macros for STLPort */
        CompFlags.dont_autogen_ext_inc = 1;
        DefSwitchMacro( "FZH" );
    }
    if( data->fzs ) {   /* Define the switch macros for STLPort */
        CompFlags.dont_autogen_ext_src = 1;
        DefSwitchMacro( "FZS" );
    }
    if( data->zl ) {
        CompFlags.emit_library_names = 0;
    }
    if( data->zp ) {
        PackAmount = data->zp_value;
        GblPackAmount = PackAmount;
    }
    if( data->zpw ) {
        CompFlags.warn_about_padding = 1;
    }
    if( data->zs ) {
        CompFlags.check_syntax = 1;
    }
    if( data->zv ) {
    #if COMP_CFG_COFF == 0
        CompFlags.virtual_stripping = TRUE;
    #endif
    }
    if( data->na ) {
        CompFlags.disable_ialias = 1;
    }
#ifndef NDEBUG
    if( data->tp ) {
        for(;;) {
            OPT_STRING* str = data->tp_value;
            if( NULL == str ) break;
            data->tp_value = str->next;
            strcpy( Buffer, str->data );
            CMemFree( str );
            PragmaSetToggle( TRUE );
        }
    }
    if( data->zi ) {
        CompFlags.extra_stats_wanted = 1;
        // try to prevent distortions caused by debug stuff
        PragDbgToggle.no_mem_cleanup = 1;
    }
#endif
    CBanner();
}

static void postOptions( void )
{
}

void GenCOptions(               // PROCESS ALL OPTIONS
    char **argv )               // - command line vector
{
    auto OPT_STORAGE data;
    char* env_var;              // - environment var for compiler

    indirectionLevel = 0;
    InitModInfo();
    CmdLnCtxInit();
    CmdSysInit();
    OPT_INIT( &data );
    if( ! CompFlags.ignore_environment ) {
        CtxSetContext( CTX_CMDLN_ENV );
        env_var = CmdSysEnvVar();
        CmdLnCtxPushEnv( env_var );
        procOptions( &data, CppGetEnv( env_var ) );
        CmdLnCtxPop();
    }
    CtxSetContext( CTX_CMDLN_PGM );
    CmdLnCtxPush( CTX_CLTYPE_PGM );
    while( *argv != NULL ) {
        procOptions( &data, *argv );
        ++argv;
    }
    CmdLnCtxPop();
    CmdLnCtxFini();
    CtxSetContext( CTX_CMDLN_VALID );
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
    defn = defn;
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
