/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  POSIX style compiler driver.
*
****************************************************************************/


/* FIXME
 *  if linking is done, remove objects afterwards?
 *  unrecognized options should warn, possibly error
 *  should owcc output a warning message if -b names unknown target?
 *  -S should remove .o files
 */

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include <process.h>
#endif
#include "wio.h"
#include "watcom.h"
#include "getopt.h"
#include "bool.h"
#include "swchar.h"
#include "diskos.h"
#include "clcommon.h"
#include "banner.h"

#include "clibext.h"


#ifdef BOOTSTRAP
#define BPRFX   "b"
#else
#define BPRFX   ""
#endif

#if defined( _M_I86 )
#define CC          BPRFX "wcc"         /* Open Watcom C compiler (16-bit)   */
#define CCXX        BPRFX "wpp"         /* Open Watcom C++ compiler (16-bit) */
#define FC          BPRFX "wfc"         /* Open Watcom F77 compiler (16-bit) */
#define ASM         BPRFX "wasm"        /* Open Watcom assembler             */
#define _TARGET_    "x86 16-bit"
#define ARCH        TARGET_ARCH_I86
#elif defined( __AXP__ )
#define CC          BPRFX "wccaxp"      /* Open Watcom C compiler (32-bit)   */
#define CCXX        BPRFX "wppaxp"      /* Open Watcom C++ compiler (32-bit) */
#define FC          BPRFX "wfcaxp"      /* Open Watcom F77 compiler (32-bit) */
#define ASM         BPRFX "wasaxp"      /* Open Watcom assembler             */
#define _TARGET_    "Alpha AXP"
#define ARCH        TARGET_ARCH_AXP
#elif defined( __PPC__ )
#define CC          BPRFX "wccppc"      /* Open Watcom C compiler (32-bit)   */
#define CCXX        BPRFX "wppppc"      /* Open Watcom C++ compiler (32-bit) */
#define FC          BPRFX "wfcppc"      /* Open Watcom F77 compiler (32-bit) */
#define ASM         BPRFX "wasppc"      /* Open Watcom assembler             */
#define _TARGET_    "PowerPC"
#define ARCH        TARGET_ARCH_PPC
#elif defined( __MIPS__ )
#define CC          BPRFX "wccmps"      /* Open Watcom C compiler (32-bit)   */
#define CCXX        BPRFX "wppmps"      /* Open Watcom C++ compiler (32-bit) */
#define FC          BPRFX "wfcmps"      /* Open Watcom F77 compiler (32-bit) */
#define ASM         BPRFX "wasmps"      /* Open Watcom assembler             */
#define _TARGET_    "MIPS"
#define ARCH        TARGET_ARCH_MIPS
#else
#define CC          BPRFX "wcc386"      /* Open Watcom C compiler (32-bit)   */
#define CCXX        BPRFX "wpp386"      /* Open Watcom C++ compiler (32-bit) */
#define FC          BPRFX "wfc386"      /* Open Watcom F77 compiler (32-bit) */
#define ASM         BPRFX "wasm"        /* Open Watcom assembler             */
#define _TARGET_    "x86 32-bit"
#define ARCH        TARGET_ARCH_X86
#endif
#define PACK              "cvpack"      /* Open Watcom executable packer      */
#define LINK        BPRFX "wlink"       /* Open Watcom linker                 */
#define DIS               "wdis"        /* Open Watcom disassembler           */
#define SPECS_FILE  BPRFX "specs.owc"   /* spec file with target definition   */

#define WCLENV      "OWCC"

#define OUTPUTFILE  "a.out"
#define TEMPFILE    "__owcc__" TOOL_LNK_EXT  /* temporary linker directive file    */

#define MAX_CC_OPTS 256

#define IS_ASM(x)   (x[0] == '.' && (fname_cmp(x + 1, ASM_EXT) == 0 || stricmp(x + 1, ASMS_EXT) == 0))
#define IS_FOR(x)   (x[0] == '.' && (fname_cmp(x + 1, "f") == 0 || stricmp(x + 1, "for") == 0 || fname_cmp(x + 1, "ftn") == 0))
#define IS_LIB(x)   (HasFileExtension( x, LIB_EXT ) || HasFileExtension( x, LIB_EXT_SECONDARY ))

typedef enum {
    TARGET_ARCH_DEFAULT,
    TARGET_ARCH_I86,
    TARGET_ARCH_X86,
    TARGET_ARCH_AXP,
    TARGET_ARCH_MIPS,
    TARGET_ARCH_PPC,
    TARGET_ARCH_COUNT
} owcc_target_arch;

char *OptEnvVar = WCLENV;                   /* Data interface for GetOpt()        */

static  char        *SystemName;            /* system to link for                 */
static  list        *Files_List;            /* list of filenames from Cmd         */
static  char        *CC_Opts[MAX_CC_OPTS];  /* list of compiler options from Cmd  */
static  char        *Link_Name;             /* Temp_Link copy if /fd specified    */
static  char        CPU_Class;              /* [0..6]86, 'm'ips or 'a'xp          */
static  owcc_target_arch CPU_Arch;          /* CPU architecture TARGET_ARCH_...   */
static  char        Conventions[2];         /* 'r' for -3r or 's' for -3s         */
static  char        *O_Name;                /* name of -o option                  */

static  char        preprocess_only;        /* flag: -E option used?              */
static  char        cpp_want_lines;         /* flag: want #lines output?          */
static  char        cpp_keep_comments;      /* flag: keep comments in output?     */
static  char        cpp_encrypt_names;      /* flag: encrypt C++ names?           */
static  char        *cpp_linewrap;          /* line length for cpp output         */

/*
 *  Static function prototypes
 */

const char *WclMsgs[] = {
    #define pick(code,english)      english
    #include "wclmsg.h"
    #undef pick
};

static const char *UsageText[] = {
    #include "usage.gh"
    NULL
};

typedef struct {
    char    *LongName;  /* if ending in ':', copy rest to OW option */
    char    *WatcomName;
} option_mapping;

/* Map of options which don't need special treatment */
static option_mapping mappings[] = {
    { "fpmath=87",                      "fp0" },
    { "fpmath=287",                     "fp2" },
    { "fpmath=387",                     "fp3" },
    { "fptune=586",                     "fp5" },
    { "fptune=686",                     "fp6" },
    { "fno-short-enum",                 "ei" },
    { "fshort-enum",                    "em" },
    { "fsigned-char",                   "j" },
    { "fpack-struct=:",                 "zp" },
    { "ffar-data-threshold=:",          "zt" },
    { "frtti",                          "xr" },
    { "fmessage-full-path",             "ef" },
    { "femit-names",                    "en" },
    { "fbrowser",                       "db" },
    { "fhook-epilogue",                 "ee" },
    { "fhook-prologue=:",               "ep" },
    { "fhook-prologue",                 "ep" },
    { "fwrite-def",                     "v" },
    { "fwrite-def-without-typedefs",    "zg" },
    { "fno-stack-check",                "s" },
    { "fgrow-stack",                    "sg" },
    { "fstack-probe",                   "st" },
    { "fno-writable-strings",           "zc" },
    { "fnostdlib",                      "zl" },
    { "ffunction-sections",             "zm" },
    { "fno-strict-aliasing",            "oa" },
    { "fguess-branch-probability",      "ob" },
    { "fno-optimize-sibling-calls",     "oc" },
    { "finline-functions",              "oe" },
    { "finline-limit=:",                "oe=" },
    { "fno-omit-frame-pointer",         "of" },
    { "fno-omit-leaf-frame-pointer",    "of+" },
    { "frerun-optimizer",               "oh" },
    { "finline-intrinsics-max",         "oi+" },
    { "finline-intrinsics",             "oi" },
    { "finline-fp-rounding",            "zri" },
    { "fomit-fp-rounding",              "zro" },
    { "fschedule-prologue",             "ok" },
    { "floop-optimize",                 "ol" },
    { "funroll-loops",                  "ol+" },
    { "finline-math",                   "om" },
    { "funsafe-math-optimizations",     "on" },
    { "ffloat-store",                   "op" },
    { "fschedule-insns",                "or" },
    { "fkeep-duplicates",               "ou" },
    { "fno-eh",                         "xd" },
    { "feh-direct",                     "xst" },
    { "feh-table",                      "xss" },
    { "feh",                            "xs" },
    { "fnonconst-initializers",         "aa" },
    { "ftabstob=:",                     "t=" },
    /* { "mcmodel=:",                      "m" }, --- handled explicitly */
    { "mabi=cdecl",                     "ecc" },
    { "mabi=stdcall",                   "ecd" },
    { "mabi=fastcall",                  "ecf" },
    { "mabi=pascal",                    "ecp" },
    { "mabi=fortran",                   "ecr" },
    { "mabi=syscall",                   "ecs" },
    { "mabi=watcall",                   "ecw" },
    { "mwindows",                       "bg" },
    { "mconsole",                       "bc" },
    { "mthreads",                       "bm" },
    { "mrtdll",                         "br" },
    { "mdefault-windowing",             "bw" },
    { "mhard-emu-float",                "fpi" },
    { "msoft-float",                    "fpc" },
    { "w",                              "w0" },
    { "Wlevel:",                        "w" },
    { "Wall",                           "w4" },
    { "Wextra",                         "wx" },
    { "Werror",                         "we" },
    { "Wno-n:",                         "wcd=" }, /* NOTE: this needs to be listed before -Wn to work */
    { "Wn:",                            "wce=" },
    { "Woverlay",                       "wo" },
    { "Wpadded",                        "zpw" },
    { "Wc,-:",                          "" },
    { "Wstop-after-errors=:",           "e" },
    { "ansi",                           "za" },
    { "std=c99",                        "za99" },
    { "std=c89",                        "za" },
    { "std=ow",                         "ze" },
    { "O0",                             "od" },
    { "O1",                             "oil" },
    { "O2",                             "onatx" },
    { "O3",                             "onatxl+" },
    { "Os",                             "os" },
    { "Ot",                             "ot" },
    { "O",                              "oil" },
    { "H",                              "fti" },
    { "fignore-line-directives",        "pil" },
    { "fvoid-ptr-arithmetic",           "zev" },
    { "shared",                         "bd" },
};

/* Others to be checked:
    { "-tp=<name>                      (C) set #pragma on <name>",

OW options that might be useful to add:
    -ft / -fx  non-8.3 include search options

*/

static etool tools_allarch[TYPE_ALLARCH_COUNT] = {
    { LINK, LINK TOOL_EXE_EXT,   NULL },
    { PACK, PACK TOOL_EXE_EXT,   NULL },
    { DIS,  DIS TOOL_EXE_EXT,    NULL }
};

static etool tools_asm_arch[TARGET_ARCH_COUNT] = {
    { ASM,            ASM TOOL_EXE_EXT,            NULL },   // default
    { BPRFX "wasm",   BPRFX "wasm" TOOL_EXE_EXT,   NULL },   // i86
    { BPRFX "wasm",   BPRFX "wasm" TOOL_EXE_EXT,   NULL },   // i386
    { BPRFX "wasaxp", BPRFX "wasaxp" TOOL_EXE_EXT, NULL },   // axp
    { BPRFX "wasmps", BPRFX "wasmps" TOOL_EXE_EXT, NULL },   // mips
    { BPRFX "wasppc", BPRFX "wasppc" TOOL_EXE_EXT, NULL },   // ppc
};

static etool tools_cc_arch[TARGET_ARCH_COUNT] = {
    { CC,             CC TOOL_EXE_EXT,             NULL },   // default
    { BPRFX "wcc",    BPRFX "wcc" TOOL_EXE_EXT,    NULL },   // i86
    { BPRFX "wcc386", BPRFX "wcc386" TOOL_EXE_EXT, NULL },   // i386
    { BPRFX "wccaxp", BPRFX "wccaxp" TOOL_EXE_EXT, NULL },   // axp
    { BPRFX "wccmps", BPRFX "wccmps" TOOL_EXE_EXT, NULL },   // mips
    { BPRFX "wccppc", BPRFX "wccppc" TOOL_EXE_EXT, NULL },   // ppc
};

static etool tools_ccxx_arch[TARGET_ARCH_COUNT] = {
    { CCXX,           CCXX TOOL_EXE_EXT,           NULL },   // default
    { BPRFX "wpp",    BPRFX "wpp" TOOL_EXE_EXT,    NULL },   // i86
    { BPRFX "wpp386", BPRFX "wpp386" TOOL_EXE_EXT, NULL },   // i386
    { BPRFX "wppaxp", BPRFX "wppaxp" TOOL_EXE_EXT, NULL },   // axp
    { BPRFX "wppmps", BPRFX "wppmps" TOOL_EXE_EXT, NULL },   // mips
    { BPRFX "wppppc", BPRFX "wppppc" TOOL_EXE_EXT, NULL },   // ppc
};

static etool tools_f77_arch[TARGET_ARCH_COUNT] = {
    { FC,             FC TOOL_EXE_EXT,             NULL },   // default
    { BPRFX "wfc",    BPRFX "wfc" TOOL_EXE_EXT,    NULL },   // i86
    { BPRFX "wfc386", BPRFX "wfc386" TOOL_EXE_EXT, NULL },   // i386
    { BPRFX "wfcaxp", BPRFX "wfcaxp" TOOL_EXE_EXT, NULL },   // axp
    { BPRFX "wfcmps", BPRFX "wfcmps" TOOL_EXE_EXT, NULL },   // mips
    { BPRFX "wfcppc", BPRFX "wfcppc" TOOL_EXE_EXT, NULL },   // ppc
};

static void print_banner( void )
{
    static bool printed = false;

    if( !printed ) {
        printed = true;
        if( !Flags.be_quiet ) {
#if defined( _BETAVER )
            puts( banner1w1( "C/C++ " _TARGET_ " Compiler Driver Program" ) );
            puts( banner1w2( _WCL_VERSION_ ) );
#else
            puts( banner1w( "C/C++ " _TARGET_ " Compiler Driver Program", _WCL_VERSION_ ) );
#endif
            puts( banner2 );
            puts( banner2a( 1988 ) );
            puts( banner3 );
            puts( banner3a );
        }
    }
}

static char *xlate_fname( char *name )
{
#ifndef __UNIX__
    /* On non-POSIX hosts, pathnames must be translated to format
     * expected by other tools.
     */
    char    *p;

    for( p = name; *p != '\0'; ++p ) {
        if( *p == '/' ) {
            *p = '\\';
        }
    }
#endif
    return( name );
}

/* The following 'f' functions are designed to handle filenames */
static char *strfcat( char *target, const char *source )
{
    return( xlate_fname( strcat( target, source ) ) );
}

static char *strfdup( const char *source )
{
    return( xlate_fname( MemStrDup( source ) ) );
}

static void addccstring( const char *string )
/*******************************************/
{
    char    *op;
    int     i;

    if( string == NULL || *string == '\0' )
        return;

    op = MemAlloc( ( strlen( string ) + 1 ) * sizeof( char ) );
    strcpy( op, string );

    i = 0;
    while( CC_Opts[i] != NULL )
        i++;
    CC_Opts[i] = op;
    CC_Opts[i + 1] = NULL;
}

static void addcclongopt( const char *option, const char *tail )
/**************************************************************/
{
    char    *op;
    size_t  len;

    /* Calculate our necessary memory here for readability */
    len = strlen( option ) + 2;
    if( tail != NULL && *tail != '\0' )
        len += strlen( tail );

    op = MemAlloc( len * sizeof( char ) );
    op[0] = '-';
    strcpy( op + 1, option );
    if( tail != NULL && *tail != '\0' )
        strcat( op, tail );

    addccstring( op );
    MemFree( op );
}

static void addccopt( char option, const char *opt )
/**************************************************/
{
    char    *op;
    size_t  len;

    len = 3;
    if( opt != NULL && *opt != '\0' ) {
        len += strlen( opt );
    }
    op = MemAlloc( len * sizeof( char ) );

    op[0] = '-';
    op[1] = option;
    op[2] = '\0';
    if( len > 3 )
        strcpy( op + 2, opt );
    addccstring( op );
    MemFree( op );
}

static  FILE *OpenSpecsFile( char *buffer )
/*****************************************/
{
    FILE    *specs;

    FindPath( SPECS_FILE, buffer );
    specs = fopen( buffer, "r" );
    if( specs == NULL ) {
        fprintf( stderr, "Could not open specs file '%s' for reading!\n", buffer );
        exit( EXIT_FAILURE );
    }
    return( specs );
}

#define SYSTEM_BEGIN    "system begin "
#define SYSTEM_END      "end"

#if 0
/*static*/  int  ListSpecsFile( void )
/************************************/
{
    FILE    *specs;
    char    *line;
    size_t  begin_len;
    char    *p;

    line = MemAlloc( MAX_CMD );
    specs = OpenSpecsFile( line );
    begin_len = sizeof( SYSTEM_BEGIN ) - 1;
    while( fgets( line, MAX_CMD, specs ) != NULL ) {
        p = strchr( line, '\n' );
        if( p != NULL ) {
            *p = '\0';
        }
        if( strncmp( line, SYSTEM_BEGIN, begin_len ) == 0 ) {
            printf( "%s\n", line + begin_len);
        }
    }
    fclose( specs );
    MemFree( line );
    return( 0 );
}
#endif

static  int  ConsultSpecsFile( const char *target )
/*************************************************/
{
    FILE    *specs;
    char    *line;
    char    *start_line;
    bool    in_target = false;
    char    *p;
    int     rc = 0;

    line = MemAlloc( MAX_CMD );
    specs = OpenSpecsFile( line );
    start_line = MemAlloc( sizeof( SYSTEM_BEGIN ) + strlen( target ) );
    strcpy( start_line, SYSTEM_BEGIN );
    strcat( start_line, target );
    /* search for a block whose first line is "system begin <target>" ... */
    while( fgets( line, MAX_CMD, specs ) != NULL ) {
        p = strchr( line, '\n' );
        if( p != NULL ) {
            *p = '\0';
        }
        if( stricmp( line, start_line ) == 0 ) {
            in_target = true;
        } else if( stricmp( line, SYSTEM_END ) == 0 ) {
            in_target = false;
        } else if( in_target ) {
            for( p = line; isspace( *(unsigned char *)p ); p++ )
                ; /* do nothing else */
            p = strtok( p, " \t=" );
            if( p == NULL )
                continue;
            if( strcmp( p, "ARCH" ) == 0 ) {
                p = strtok( NULL, " \t" );
                switch( *p ) {
                case 'i':
                    switch( p[1] ) {
                    case '8':   // i86
                        CPU_Arch = TARGET_ARCH_I86;
                        break;
                    case '3':   // i386
                        CPU_Arch = TARGET_ARCH_X86;
                        break;
                    }
                    break;
                case 'a':       // axp
                    CPU_Arch = TARGET_ARCH_AXP;
                    break;
                case 'm':       // mips
                    CPU_Arch = TARGET_ARCH_MIPS;
                    break;
                case 'p':       // ppc
                    CPU_Arch = TARGET_ARCH_PPC;
                    break;
                default:
                    CPU_Arch = TARGET_ARCH_DEFAULT;
                    break;
                }
                p = strtok( NULL, "\n" );
                if( p != NULL ) {
                    /* if there are further options, copy them */
                    p = strtok( p, " " );
                    while(p != NULL) {
                        addccstring(p);
                        p = strtok( NULL, " ");
                    }
                }
                rc = 1;
                break;
            }
        }
    }
    fclose( specs );
    MemFree( start_line );
    MemFree( line );
    return( rc );
}

static void initialize_Flags( void )
/**********************************/
{
    Flags.math_8087       = 1;
    Flags.map_wanted      = 0;
    Flags.link_ignorecase = 0;
    Flags.tiny_model      = 0;
    Flags.be_quiet        = 1;
    Flags.no_link         = 0;
    Flags.do_link         = 0;
    Flags.do_disas        = 0;
    Flags.do_cvpack       = 0;
    Flags.link_for_dos    = 0;
    Flags.link_for_os2    = 0;
    Flags.windows         = 0;
    Flags.link_for_sys    = 0;
    Flags.force_c         = 0;
    Flags.force_c_plus    = 0;
    Flags.strip_all       = 0;
    Flags.want_errfile    = 0;
    Flags.keep_exename    = 0;
}

static unsigned ParseEnvVar( const char *env, char **argv, char *buf )
/********************************************************************/
{
    /*
     * Returns a count of the "command line" parameters in *env.
     * Unless argv is NULL, both argv and buf are completed.
     *
     * This function ought to be fairly similar to clib(initargv@_SplitParms).
     * Parameterisation does the same as _SplitParms with historical = 0.
     */

    const char  *start;
    int         switchchar;
    unsigned    argc;
    char        *bufend;
    bool        got_quote;

    switchchar = _dos_switch_char();
    bufend = buf;
    argc = 0;
    for( ; *env != '\0'; ) {
        got_quote = false;
        while( isspace( *env ) && *env != '\0' )
            env++;
        start = env;
        if( buf != NULL ) {
            argv[ argc ] = bufend;
        }
        if( *env == switchchar || *env == '-' ) {
            if( buf != NULL ) {
                *bufend = *env;
                bufend++;
            }
            env ++;
        }
        while( ( got_quote || !isspace( *env ) ) && *env != '\0' ) {
            if( *env == '\"' ) {
                got_quote = !got_quote;
            }
            if( buf != NULL ) {
                *bufend = *env;
                bufend++;
            }
            env++;
        }
        if( start != env ) {
            argc++;
            if( buf != NULL ) {
                *bufend = '\0';
                bufend++;
            }
        }
    }
    return( argc );
}

typedef struct stack {
    struct stack    *next;
    char            *cmdbuf;
    char            **argv;
} stack;

static bool find_mapping( char c )
/*******************************/
{
    int     i;

    for( i = 0; i < sizeof( mappings ) / sizeof( mappings[0] ); i++ ) {
        option_mapping  *m;
        const char      *tail;

        m = mappings + i;
        if( c != m->LongName[0] )
            continue;
        if( OptArg == NULL ) {
            if( m->LongName[1] != '\0' )
                /* non-existant argument can't match other cases */
                continue;
            addcclongopt( m->WatcomName, NULL );
            return( true );
        }
        tail = strchr( m->LongName, ':' );
        if( tail != NULL ) {
            if( strncmp( OptArg, m->LongName + 1, tail - m->LongName - 1 ) == 0 ) {
                addcclongopt( m->WatcomName, OptArg + ( tail - m->LongName - 1 ) );
                return( true );
            }
        } else if( strcmp( OptArg, m->LongName + 1 ) == 0 ) {
            addcclongopt( m->WatcomName, NULL );
            return( true );
        }
    }
    return( false );
}

static  int  ParseArgs( int argc, char **argv )
/*********************************************/
{
    char        *p;
    int         wcc_option;
    char        c;
    int         i;
    list        *new_item;
    char        pelc[5];
    char        *Word;

    initialize_Flags();
    DebugFlag          = DBG_LINES;
    DebugFormat        = DBG_FMT_DWARF;
    StackSize          = NULL;
    Conventions[0]     = '\0';
    Conventions[1]     = '\0';
    CPU_Arch           = TARGET_ARCH_DEFAULT;
    CPU_Class          = -1;
    preprocess_only    = 0;
    cpp_want_lines     = 1; /* NB: wcc and wcl default to 0 here */
    cpp_keep_comments  = 0;
    cpp_encrypt_names  = 0;
    cpp_linewrap       = NULL;
    O_Name             = NULL;

    AltOptChar = '-'; /* Suppress '/' as option herald */
    while( (i = GetOpt( &argc, argv,
#if 0
                        "b:Cc::D:Ef:g::"
                        "HI:i::L:l:M::m:"
                        "O::o:P::QSs::U:vW::wx:yz::",
#else
                        "b:CcD:Ef:g::"
                        "HI:i::L:l:M::m:"
                        "O::o:P::QSs::U:vW::wx::yz::",
#endif
                        UsageText )) != -1 ) {

        c = (char)i;
        if( find_mapping( c ) )
            continue;

        Word = "";
        if( OptArg != NULL && *OptArg != '\0' ) {
            Word = MemAlloc( strlen( OptArg ) + 6 );
            strcpy( Word, OptArg );
        }

        wcc_option = 1;
        switch( c ) {
        case 'f':
            if( strcmp( Word, "syntax-only" ) == 0 ) {
                c = 'z';
                strcpy( Word, "s" );
                Flags.no_link = 1;
                break;
            }
            if( strncmp( Word, "cpp-wrap=", 9 ) == 0 ) {
                Word[7] = 'w';
                MemFree( cpp_linewrap );
                cpp_linewrap = MemStrDup( Word + 7 );
                wcc_option = 0;
                break;
            }
            if( strcmp( Word, "mangle-cpp" ) == 0 ) {
                cpp_encrypt_names = 1;
                wcc_option = 0;
                break;
            }
            switch( Word[0] ) {
            case 'd':           /* name of linker directive file */
                if( Word[1] == '=' || Word[1] == '#' ) {
                    MakeName( Word, TOOL_LNK_EXT );  /* add extension */
                    MemFree( Link_Name );
                    Link_Name = strfdup( Word + 2 );
                } else {
                    MemFree( Link_Name );
                    Link_Name = MemStrDup( TEMPFILE );
                }
                wcc_option = 0;
                break;
            case 'm':           /* name of map file */
                Flags.map_wanted = true;
                if( Word[1] == '=' || Word[1] == '#' ) {
                    MemFree( Map_Name );
                    Map_Name = strfdup( Word + 2 );
                }
                wcc_option = 0;
                break;
            case 'o':           /* name of object file */
                /* parse off argument, so we get right filename
                   in linker command file */
                p = Word + 1;
                if( *p == '=' || *p == '#' )
                    ++p;
                MemFree( Obj_Name );
                Obj_Name = strfdup( p );
                break;
            case 'r':           /* name of error report file */
                Flags.want_errfile = true;
                break;
            }
            /* avoid passing on unknown options */
            wcc_option = 0;
            break;

        /* compiler options that affect the linker */
        case 'c':           /* compile only */
            Flags.no_link = true;
            wcc_option = 0;
            break;
        case 'x':           /* change source language */
            if( strcmp( Word, "c" ) == 0 ) {
                Flags.force_c = true;
            } else if( strcmp( Word, "c++" ) == 0 ) {
                Flags.force_c_plus = true;
            } else {
                Flags.no_link = true;
            }
            wcc_option = 0;
            break;
        case 'm':
            if( ( strncmp( "cmodel=", Word, 7 ) == 0 ) && ( Word[8] == '\0' ) ) {
                if( Word[7] == 't' ) {      /* tiny model */
                    Word[0] = 's';          /* change to small */
                    Flags.tiny_model = true;
                } else {
                    Word[0] = Word[7];
                }
                Word[1] = '\0';
                break;
            }
            if( strncmp( "regparm=", Word, 8 ) == 0 ) {
                if( strcmp( Word + 8, "0" ) == 0 ) {
                    Conventions[0] = 's';
                } else {
                    Conventions[0] = 'r';
                }
                wcc_option = 0;
                break;
            }
            if( strncmp( "arch=", Word, 5 ) == 0 ) {
                switch( Word[5] ) {
                case 'i':
                    switch( Word[6] ) {
                    case '8':   // i86
                        CPU_Arch = TARGET_ARCH_I86;
                        break;
                    case '1':   // i186
                        CPU_Arch = TARGET_ARCH_I86;
                        CPU_Class = '1';
                        break;
                    case '2':   // i286
                        CPU_Arch = TARGET_ARCH_I86;
                        CPU_Class = '2';
                        break;
                    case '3':   // i386
                        CPU_Arch = TARGET_ARCH_X86;
                        break;
                    }
                    break;
                case 'a':       // axp
                    CPU_Arch = TARGET_ARCH_AXP;
                    break;
                case 'm':       // mips
                    CPU_Arch = TARGET_ARCH_MIPS;
                    break;
                case 'p':       // ppc
                    CPU_Arch = TARGET_ARCH_PPC;
                    break;
                default:
                    CPU_Arch = TARGET_ARCH_DEFAULT;
                }
                wcc_option = 0;
                break;
            }
            if( strncmp( "tune=i", Word, 6 ) == 0 ) {
                switch( Word[6] ) {
                case '3':   // i386
                case '4':   // i486
                case '5':   // i586
                case '6':   // i686
                    CPU_Class = Word[6];
                    break;
                default:
                    /* Unknown CPU type --- disable generation of this
                     * option */
                    CPU_Class = -1;
                }
                wcc_option = 0;
                break;
            }
            if( strncmp( "stack-size=", Word, 11) == 0 ) {
                MemFree( StackSize );
                StackSize = MemStrDup( Word + 11 );
                wcc_option = 0;
            }
            wcc_option = 0;     /* dont' pass on unknown options */
            break;
        case 'z':
            switch( tolower( Word[0] ) ) {
            case 's':
                Flags.no_link = true;
                break;
            case 'q':
                Flags.be_quiet = true;
                break;
            case 'w':
                Flags.windows = true;
            }
            break;
        case 'E':
            preprocess_only = 1;
            wcc_option = 0;
            break;
        case 'P':
            cpp_want_lines = 0;
            wcc_option = 0;
            break;
        case 'C':
            cpp_keep_comments = 1;
            wcc_option = 0;
            break;
        case 'o':
            MemFree( O_Name );
            O_Name = strfdup( Word );
            wcc_option = 0;
            break;
        case 'g':
            if( Word[0] == '\0' ) {
                Word = MemAlloc( 1 + 6 );
                strcpy( Word, "2" );
            } else if( !isdigit( Word[0] ) ) {
                c = 'h';
                if( strcmp( Word, "watcom" ) == 0
                  || strcmp( Word, "codeview" ) == 0
                  || strcmp( Word, "dwarf" ) == 0 ) {
                    Word[1] = '\0';
                }
                if( Word[1] == '\0' ) {
                    if( Word[0] == 'w' ) {
                        DebugFormat = DBG_FMT_WATCOM;
                    } else if( Word[0] == 'c' ) {
                        Flags.do_cvpack = 1;
                        DebugFormat = DBG_FMT_CODEVIEW;
                    } else if( Word[0] == 'd' ) {
                        DebugFormat = DBG_FMT_DWARF;
                    }
                }
                break;
            }
            c = 'd';
            if( strcmp( Word, "0" ) == 0 ) {
                DebugFlag = DBG_NONE;
            } else if( strcmp( Word, "1" ) == 0 ) {
                DebugFlag = DBG_LINES;
            } else if( strcmp( Word, "1+" ) == 0 ) {
                DebugFlag = DBG_ALL;
            } else if( strcmp( Word, "2" ) == 0 ) {
                DebugFlag = DBG_ALL;
            } else if( strcmp( Word, "2i" ) == 0 ) {
                DebugFlag = DBG_ALL;
            } else if( strcmp( Word, "2s" ) == 0 ) {
                DebugFlag = DBG_ALL;
            } else if( strcmp( Word, "3" ) == 0 ) {
                DebugFlag = DBG_ALL;
            } else if( strcmp( Word, "3i" ) == 0 ) {
                DebugFlag = DBG_ALL;
            } else if( strcmp( Word, "3s" ) == 0 ) {
                DebugFlag = DBG_ALL;
            }
            break;
        case 'S':
            Flags.do_disas = true;
            Flags.no_link = true;
            if( DebugFlag == DBG_NONE ) {
                c = 'd';
                if( Word[0] == '\0' )
                    Word = MemAlloc( 1 + 6 );
                strcpy( Word, "1" );
                DebugFlag = DBG_LINES;
                break;
            }
            wcc_option = 0;
            break;
        case 's':
            if( Word[0] != '\0' ) {
                /* leave -shared to mapping table */
                wcc_option = 0;
                break;
            }
            Flags.strip_all = 1;
            DebugFlag = DBG_NONE;
            wcc_option = 0;
            break;
        case 'v':
            Flags.be_quiet = 0;
            wcc_option = 0;
            break;
        case 'W':
            if( strncmp( Word, "l,", 2 ) == 0 ) {
                AddDirective( Word + 2 );
                wcc_option = 0;
            }
            /* other cases handled by table */
            break;
        case 'I':
            xlate_fname( Word );
            break;
        case 'b':
            Flags.link_for_sys = true;
            MemFree( SystemName );
            SystemName = MemStrDup( Word );
            /* if Word found in specs.owc, add options from there: */
            if( ConsultSpecsFile( Word ) ) {
                /* all set */
                wcc_option = 0;
            } else {
                /* not found --- default to bt=<system> */
                strcpy( Word, "t=" );
                strcat( Word, SystemName );
            }
            break;
        case 'l':
            new_item = MemAlloc( sizeof( list ) );
            new_item->next = NULL;
            p = MemAlloc( strlen( Word ) + 2 + 1 );
            strcpy( p, Word );
            strcat( p, "." LIB_EXT_SECONDARY );
            new_item->item = strfdup( p );
            MemFree( p );
            ListAppend( &Libs_List, new_item );
            wcc_option = 0;
            break;
        case 'L':
            AddDirectivePath( "libpath ", Word );
            wcc_option = 0;
            break;
        case 'i':       /* -include <file> --> -fi=<file> */
            if( Word[0] == '\0' ) {
                wcc_option = 0;
                break;
            }
            if( strcmp( Word, "nclude" ) == 0 ) {
                c = 'f';
                Word = MemRealloc( Word, strlen( argv[OptInd] ) + 6 );
                if( OptInd >= argc - 1 ) {
                    MemFree( cpp_linewrap );
                    MemFree( Word );
                    PrintMsg( "Argument of -include missing\n", OptArg );
                    return( 1 );
                }
                strcpy( Word, "i=" );
                strfcat( Word, argv[OptInd] );
                argv[OptInd++][0] = '\0';
                break;
            }
            /* avoid passing un unknown options */
            wcc_option = 0;
            break;

        case 'M':               /* autodepend information for Unix makes */
            if( Word[0] == '\0' ) {
                wcc_option = 0;
                break;
            }
            c = 'a';
            if( strcmp( Word, "D" ) == 0 || strcmp( Word, "MD" ) == 0 ) {
                /* NB: only -MMD really matches OW's behaviour, but
                 * for now, let's accept -MD to mean the same */
                /* translate to -adt=.o */
                strcpy( Word, "dt=" "." OBJ_EXT );
            } else if( strcmp( Word, "F" ) == 0 ) {
                Word = MemRealloc( Word, strlen( argv[OptInd] ) + 6 );
                if( OptInd >= argc - 1 ) {
                    MemFree( cpp_linewrap );
                    MemFree( Word );
                    PrintMsg( "Argument of -MF missing\n", OptArg );
                    return( 1 );
                }
                strcpy( Word, "d=" );
                strfcat( Word, argv[OptInd] );
                argv[OptInd++][0] = '\0';
            } else if( strcmp( Word, "T" ) == 0 ) {
                Word = MemRealloc( Word, strlen( argv[OptInd] ) + 6 );
                if( OptInd >= argc - 1 ) {
                    MemFree( cpp_linewrap );
                    MemFree( Word );
                    PrintMsg( "Argument of -M%s missing\n", OptArg );
                    return( 1 );
                }
                strcpy( Word, "dt=" );
                strcat( Word, argv[OptInd] );
                argv[OptInd++][0] = '\0';
            } else {
                /* avoid passing on incompatible options */
                wcc_option = 0;
            }
            break;
        }

        /* don't add linker-specific options */
        /* to compiler command line:     */
        if( wcc_option ) {
            addccopt( c, Word );
        }
        if( Word[0] != '\0' ) {
            MemFree( Word );
        }
    }

    if( preprocess_only ) {
        Flags.no_link = true;
        if( O_Name == NULL ) {
            MemFree( Obj_Name );           /* preprocess to stdout by default */
            Obj_Name = NULL;
        }
        p = pelc;
        *p++ = 'p';
        if( cpp_encrypt_names )
            *p++ = 'e';
        if( cpp_want_lines )
            *p++ = 'l';
        if( cpp_keep_comments )
            *p++ = 'c';
        *p = '\0';

        /* cpp_linewrap may be NULL, and that's fine */
        addcclongopt( pelc, cpp_linewrap );
    }
    if( CPU_Arch != TARGET_ARCH_DEFAULT || CPU_Class != -1 || *Conventions != '\0' ) {
        owcc_target_arch    arch;
        char                cpu;

        arch = CPU_Arch;
        if( arch == TARGET_ARCH_DEFAULT ) {
            arch = ARCH;
        }
        switch( arch ) {
        case TARGET_ARCH_I86:
            cpu = '0';
            *Conventions = '\0';
            switch( CPU_Class ) {
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
                cpu = CPU_Class;
                break;
            default:
                CPU_Class = cpu;
                break;
            }
            addccopt( cpu, Conventions );
            break;
        case TARGET_ARCH_X86:
            cpu = '3';
            if( *Conventions == '\0' ) {
                *Conventions = 'r';
            }
            switch( CPU_Class ) {
            case '4':
            case '5':
            case '6':
                cpu = CPU_Class;
                break;
            default:
                CPU_Class = cpu;
                break;
            }
            addccopt( cpu, Conventions );
            break;
        }
    }
    if( Flags.be_quiet )
        addccopt( 'z', "q" );
    if( O_Name != NULL ) {
        if( Flags.no_link && !Flags.do_disas ) {
            MemFree( Obj_Name );
            Obj_Name = O_Name;
        } else {
            Exe_Name = O_Name;
            Flags.keep_exename = 1;
        }
        O_Name = NULL;
    }
    if( Obj_Name != NULL ) {
        addcclongopt( "fo=", Obj_Name );
    }
    if( !Flags.want_errfile ) {
        addcclongopt( "fr", NULL );
    }
    for( i = 1; i < argc ; i++ ) {
        Word = argv[i];
        if( Word == NULL || Word[0] == '\0' )
            continue;
        new_item = MemAlloc( sizeof( list ) );
        new_item->next = NULL;
        new_item->item = strfdup( Word );
        if( IS_LIB( Word ) ) {
            ListAppend( &Libs_List, new_item );
        } else {
            ListAppend( &Files_List, new_item );
        }
    }
    MemFree( cpp_linewrap );
    return( 0 );
}

static  int  Parse( int argc, char **argv )
/*****************************************/
{
    int         old_argc;
    char        **old_argv;
    char        *cmdbuf;
    const char  *env;
    int         ret;
    int         i;

    env = getenv( WCLENV );
    if( env != NULL ) {
        old_argc = argc;
        old_argv = argv;
        argc = ParseEnvVar( env, NULL, NULL ) + 1;
        argv = MemAlloc( ( argc + old_argc ) * sizeof( char * ) );
        cmdbuf = MemAlloc( strlen( env ) + argc );
        argv[0] = old_argv[0];
        ParseEnvVar( env, argv + 1, cmdbuf );
        for( i = 1; i < old_argc; ++i, ++argc ) {
            argv[argc] = old_argv[i];
        }
        argv[argc] = NULL;        // last element of the array must be NULL
        ret = ParseArgs( argc, argv );
        MemFree( argv );
        MemFree( cmdbuf );
    } else {
        ret = ParseArgs( argc, argv );
    }
    return( ret );
}

static int useCPlusPlus( const char *p )
/**************************************/
{
    return( *p++ == '.' && (
        fname_cmp( p, "cp" ) == 0 ||
        fname_cmp( p, "cpp" ) == 0 ||
#ifdef __UNIX__
        strcmp( p, "c++" ) == 0 ||
        strcmp( p, "C" ) == 0 ||
#endif
        fname_cmp( p, "cxx" ) == 0 ||
        fname_cmp( p, "cc" )  == 0 ||
        fname_cmp( p, "hpp" ) == 0 ||
        fname_cmp( p, "hxx" ) == 0 ) );
}

static etool *FindToolGetPath( tool_type utl )
/********************************************/
{
    etool   *tool;
    char    *buffer;

    switch( utl ) {
    case TYPE_LINK:
    case TYPE_PACK:
    case TYPE_DIS:
        tool = &tools_allarch[utl];
        break;
    case TYPE_ASM:
        tool = &tools_asm_arch[CPU_Arch];
        break;
    case TYPE_C:
        tool = &tools_cc_arch[CPU_Arch];
        break;
    case TYPE_CPP:
        tool = &tools_ccxx_arch[CPU_Arch];
        break;
    case TYPE_FORT:
        tool = &tools_f77_arch[CPU_Arch];
        break;
    default:
        return( NULL );
    }
    if( tool->path == NULL ) {
        buffer = MemAlloc( MAX_CMD );
        FindPath( tool->exename, buffer );
        tool->path = MemAlloc( strlen( buffer ) + 1 );
        strcpy( tool->path, buffer );
        MemFree( buffer );
    }
    return( tool );
}

static int tool_exec( tool_type utl, const char *fn, const char **options )
/*************************************************************************/
{
    int         rc;
    etool       *tool;
    int         i;
    int         pass_argc;
    const char  *pass_argv[MAX_CC_OPTS+5];

    tool = FindToolGetPath( utl );

    pass_argv[0] = tool->name;
    pass_argc = 1;

    while( options != NULL && options[pass_argc - 1] != NULL && pass_argc < MAX_CC_OPTS ) {
        pass_argv[pass_argc] = options[pass_argc - 1];
        pass_argc++;
    }

    if( utl == TYPE_DIS ) {
        pass_argv[pass_argc++] = "-s";
        pass_argv[pass_argc++] = "-a";
    }

    pass_argv[pass_argc++] = fn;
    pass_argv[pass_argc] = NULL;

    if( !Flags.be_quiet ) {
        printf( "\t" );
        for( i = 0; i < pass_argc; i++ )
            printf( "%s ", pass_argv[i] );
        printf( "\n" );
    }
    fflush( NULL );

    rc = (int)spawnvp( P_WAIT, tool->path, (char const *const *)pass_argv );

    if( rc != 0 ) {
        if( (rc == -1) || (rc == 255) ) {
            PrintMsg( WclMsgs[UNABLE_TO_INVOKE_EXE], tool->path );
        } else {
            if( utl == TYPE_LINK ) {
                PrintMsg( WclMsgs[LINKER_RETURNED_A_BAD_STATUS] );
            } else if( utl == TYPE_PACK ) {
                PrintMsg( WclMsgs[CVPACK_RETURNED_A_BAD_STATUS] );
            } else {
                PrintMsg( WclMsgs[COMPILER_RETURNED_A_BAD_STATUS], fn );
            }
        }
    }
    return( rc );
}

static tool_type SrcName( const char *name )
/******************************************/
{
    const char  *p;
    tool_type   utl;

    p = strrchr( name, '.' );
    if( p == NULL || strpbrk( p, PATH_SEPS_STR ) != NULL )
        p = name + strlen( name );
    if( IS_ASM( p ) ) {
        utl = TYPE_ASM;
    } else if( IS_FOR( p ) ) {
        utl = TYPE_FORT;
    } else {
        utl = TYPE_C;               // assume C compiler
        if( !Flags.force_c ) {
            if( Flags.force_c_plus || useCPlusPlus( p ) ) {
                utl = TYPE_CPP;     // use C++ compiler
            }
        }
    }
    return( utl );
}

void BuildSystemLink( FILE *fp )
{
    if( Flags.link_for_sys ) {
        fputs( "system ", fp );
        Fputnl( SystemName, fp );
    /*
     * Intel 16-bit Architecture specific
     */
  #if defined(_M_I86)
    } else if( CPU_Arch == TARGET_ARCH_DEFAULT || CPU_Arch == TARGET_ARCH_I86 ) {
  #else
    } else if( CPU_Arch == TARGET_ARCH_I86 ) {
  #endif
        if( Flags.windows ) {
            Fputnl( "system windows", fp );
        } else if( Flags.tiny_model ) {
            Fputnl( "system com", fp );
        } else if( Flags.link_for_dos ) {
            Fputnl( "system dos", fp );
        } else if( Flags.link_for_os2 ) {
            Fputnl( "system os2", fp );
        } else {
#if defined(__OS2__)
            Fputnl( "system os2", fp );
#else
            Fputnl( "system dos", fp );
#endif
        }
    /*
     * Intel 32-bit Architectures specific
     */
  #if defined(_M_X86) && !defined(_M_I86)
    } else if( CPU_Arch == TARGET_ARCH_DEFAULT || CPU_Arch == TARGET_ARCH_X86 ) {
  #else
    } else if( CPU_Arch == TARGET_ARCH_X86 ) {
  #endif
  #if defined(__OS2__)
        Fputnl( "system os2v2", fp );
  #elif defined(__NT__)
        Fputnl( "system nt", fp );
  #elif defined(__LINUX__)
        Fputnl( "system linux", fp );
        if( !Flags.strip_all ) {
            Fputnl( "option exportall", fp );
        }
  #else
        Fputnl( "system dos4g", fp );
  #endif
    /*
     * Alpha Architecture specific
     */
  #if defined( __AXP__ )
    } else if( CPU_Arch == TARGET_ARCH_DEFAULT || CPU_Arch == TARGET_ARCH_AXP ) {
  #else
    } else if( CPU_Arch == TARGET_ARCH_AXP ) {
  #endif
  #if defined(__NT__)
        Fputnl( "system ntaxp", fp );
  #endif
    /*
     * MIPS Architecture specific
     */
  #if defined( __MIPS__ )
    } else if( CPU_Arch == TARGET_ARCH_DEFAULT || CPU_Arch == TARGET_ARCH_MIPS ) {
  #else
    } else if( CPU_Arch == TARGET_ARCH_MIPS ) {
  #endif
  #if defined(__LINUX__)
        Fputnl( "system linuxmips", fp );
        if( !Flags.strip_all ) {
            Fputnl( "option exportall", fp );
        }
  #endif
    /*
     * PPC Architecture specific
     */
  #if defined( __PPC__ )
    } else if( CPU_Arch == TARGET_ARCH_DEFAULT || CPU_Arch == TARGET_ARCH_PPC ) {
  #else
    } else if( CPU_Arch == TARGET_ARCH_PPC ) {
  #endif
  #if defined(__NT__)
        Fputnl( "system ntppc", fp );
  #elif defined(__LINUX__)
        Fputnl( "system linuxppc", fp );
        if( !Flags.strip_all ) {
            Fputnl( "option exportall", fp );
        }
  #endif
    }
}

static  int  CompLink( void )
/***************************/
{
    int         rc;
    char        *file;
    char        *path;
    list        *itm;
    char        errors_found;
    tool_type   utl;
    int         i;
    char        *Word;

    Word = MemAlloc( MAX_CMD );
    errors_found = 0;
    for( itm = Files_List; itm != NULL; itm = itm->next ) {
        char    buffer[_MAX_PATH];

        strcpy( Word, itm->item );
        utl = SrcName( Word );          /* if no extension, assume .c */
        file = GetName( Word, buffer ); /* get first matching filename */
        path = MakePath( Word );        /* isolate path portion of filespec */
        while( file != NULL ) {         /* while more filenames: */
            strcpy( Word, path );
            strcat( Word, file );
            // if not .obj or .o, compile
            if( !IS_OBJ( file ) ) {
                char fname[_MAX_PATH];

                rc = tool_exec( utl, DoQuoted( fname, Word, '"' ), (const char **)CC_Opts );
                if( rc != 0 ) {
                    errors_found = 1;
                }
                strcpy( Word, RemoveExt( file ) );
            }
            AddNameObj( Word );
            if( Obj_List != NULL && Flags.do_disas ) {
                char        sfname[_MAX_PATH + 3];
                char        ofname[_MAX_PATH];
                const char  *dis_args[2];

                sfname[0] = '-';
                sfname[1] = 'l';
                sfname[2] = '=';

                if( Exe_Name != NULL ) {    /* have "-S -o output.name" */
                    DoQuoted( ofname, file, '"' );
                    DoQuoted( sfname + 3, Exe_Name, '"' );
                } else {
                    if( IS_OBJ( file ) ) {
                        DoQuoted( ofname, file, '"' );
                        strcpy( Word, RemoveExt( file ) );
                    } else {                /* wdis needs extension */
                        DoQuoted( ofname, Obj_Name, '"' );
                    }
                    strcat( Word, "." ASMS_EXT );
                    DoQuoted( sfname + 3, Word, '"' );
                }
                dis_args[0] = sfname;
                dis_args[1] = NULL;
                rc = tool_exec( TYPE_DIS, ofname, dis_args );
            }
            if( Exe_Name == NULL ) {
#ifdef __UNIX__
                Exe_Name = MemStrDup( OUTPUTFILE );
                Flags.keep_exename = 1;
#else
                Exe_Name = MemStrDup( RemoveExt( Word ) );
#endif
            }
            file = GetName( NULL, NULL );   /* get next filename */
        }
        MemFree( path );
    }
    if( errors_found ) {
        rc = 1;
    } else {
        FILE    *fp;

        errno = 0; /* Standard C does not require fopen failure to set errno */
        if( (fp = fopen( TEMPFILE, "w" )) == NULL ) {
            PrintMsg( WclMsgs[UNABLE_TO_OPEN_TEMPORARY_FILE], TEMPFILE, strerror( errno ) );
            rc = 2;
        } else {
            rc = 0;
            BuildLinkFile( fp );
            fclose( fp );
            if( ( Obj_List != NULL || Flags.do_link ) && !Flags.no_link ) {
                rc = tool_exec( TYPE_LINK, "@" TEMPFILE, NULL );
                if( rc == 0 && Flags.do_cvpack ) {
                    char fname[_MAX_PATH];

                    rc = tool_exec( TYPE_PACK, DoQuoted( fname, Exe_Name, '"' ), NULL );
                }
            }
            if( Link_Name != NULL ) {
                if( fname_cmp( Link_Name, TEMPFILE ) != 0 ) {
                    remove( Link_Name );
                    rename( TEMPFILE, Link_Name );
                }
            } else {
                remove( TEMPFILE );
            }
        }
    }
    if( Word != NULL ) {
        MemFree( Word );
        Word = NULL;
    }
    for( i = 0; i < TYPE_ALLARCH_COUNT; ++i ) {
        if( tools_allarch[i].path != NULL ) {
            MemFree( tools_allarch[i].path );
            tools_allarch[i].path = NULL;
        }
    }
    for( i = 0; i < TARGET_ARCH_COUNT; ++i ) {
        if( tools_asm_arch[i].path != NULL ) {
            MemFree( tools_asm_arch[i].path );
            tools_asm_arch[i].path = NULL;
        }
        if( tools_cc_arch[i].path != NULL ) {
            MemFree( tools_cc_arch[i].path );
            tools_cc_arch[i].path = NULL;
        }
        if( tools_ccxx_arch[i].path != NULL ) {
            MemFree( tools_ccxx_arch[i].path );
            tools_ccxx_arch[i].path = NULL;
        }
    }
    return( rc );
}

static int ProcMemInit( void )
{
    Exe_Name = NULL;
    Map_Name = NULL;
    Obj_Name = MemStrDup( "." OBJ_EXT );
    Link_Name = NULL;
    SystemName = NULL;
    StackSize = NULL;
    Directive_List = NULL;
    Files_List = NULL;
    Obj_List = NULL;
    Libs_List = NULL;
    CC_Opts[0] = NULL;
    return( 0 );
}

static int ProcMemFini( void )
{
    int i;

    for( i = 0; i < MAX_CC_OPTS; ++i ) {
        if( CC_Opts[i] == NULL )
            break;
        MemFree( CC_Opts[i] );
    }
    ListFree( Directive_List );
    ListFree( Files_List );
    ListFree( Obj_List );
    ListFree( Libs_List );
    MemFree( Exe_Name );
    MemFree( Map_Name );
    MemFree( Obj_Name );
    MemFree( Link_Name );
    MemFree( SystemName );
    MemFree( StackSize );
    return( 0 );
}

int main( int argc, char **argv )
/*******************************/
{
    int     rc;

#ifndef __WATCOMC__
    _argc = argc;
    _argv = argv;
#endif

    if( argc <= 1 ) {
        /* no arguments: just tell the user who I am */
        puts( "Usage: owcc [-?] [options] file ..." );
        exit( EXIT_SUCCESS );
    }

    MemInit();
    ProcMemInit();
    rc = Parse( argc, argv );
    if( rc == 0 ) {
        print_banner();
        rc = CompLink();
    }
    ProcMemFini();
    MemFini();
    return( ( rc != 0 ) );
}
