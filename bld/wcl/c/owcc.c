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
//#include "misc.h"
#include "bool.h"
#include "swchar.h"
#include "diskos.h"
#include "clcommon.h"
#include "banner.h"

#include "clibext.h"


#if defined(__UNIX__)
#define strfcmp strcmp
#else
#define strfcmp stricmp
#endif

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
#define TEMPFILE    "__owcc__" LNK_EXT  /* temporary linker directive file    */

#ifdef __UNIX__
#define PATH_SEPS_STR   SYS_DIR_SEP_STR
#else
#define PATH_SEPS_STR   SYS_DIR_SEP_STR "/"
#endif

#define MAX_CC_OPTS     256

#define IS_LIB(x)       (HasFileExtension( x, LIB_EXT ) || HasFileExtension( x, LIB_EXT_SECONDARY ))

typedef enum {
    TARGET_ARCH_DEFAULT,
    TARGET_ARCH_I86,
    TARGET_ARCH_X86,
    TARGET_ARCH_AXP,
    TARGET_ARCH_MIPS,
    TARGET_ARCH_PPC,
    TARGET_ARCH_COUNT
} owcc_target_arch;

char *OptEnvVar = WCLENV;               /* Data interface for GetOpt()        */

static  char    *Word;                  /* one parameter                      */
static  char    *SystemName;            /* system to link for                 */
static  list    *Files_List;            /* list of filenames from Cmd         */
static  char    *CC_Opts[MAX_CC_OPTS];  /* list of compiler options from Cmd  */
static  char    PathBuffer[_MAX_PATH];  /* buffer for path name of tool       */
static  char    *Link_Name;             /* Temp_Link copy if /fd specified    */
static  char    CPU_Class;              /* [0..6]86, 'm'ips or 'a'xp          */
static  owcc_target_arch CPU_Arch;      /* CPU architecture TARGET_ARCH_...   */
static  char    Conventions[2];         /* 'r' for -3r or 's' for -3s         */
static  char    *O_Name;                /* name of -o option                  */

static  char    preprocess_only;        /* flag: -E option used?              */
static  char    cpp_want_lines;         /* flag: want #lines output?          */
static  char    cpp_keep_comments;      /* flag: keep comments in output?     */
static  char    cpp_encrypt_names;      /* flag: encrypt C++ names?           */
static  char    *cpp_linewrap;          /* line length for cpp output         */

/*
 *  Static function prototypes
 */

const char *WclMsgs[] = {
    #define pick(code,english)      english
    #include "wclmsg.h"
    #undef pick
};

static const char *EnglishHelp[] = {
    #include "owcchelp.gh"
    NULL
};

typedef struct {
    char    *LongName;  /* if ending in ':', copy rest to OW option */
    char    *WatcomName;
} option_mapping;

/* Map of options which don't need special treatment */
option_mapping mappings[] = {
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
    { "fomit-fp-rounting",              "zro" },
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
    { LINK, LINK EXE_EXT,   NULL },
    { PACK, PACK EXE_EXT,   NULL },
    { DIS,  DIS EXE_EXT,    NULL }
};

static etool tools_asm_arch[TARGET_ARCH_COUNT] = {
    { ASM,            ASM EXE_EXT,            NULL },   // default
    { BPRFX "wasm",   BPRFX "wasm" EXE_EXT,   NULL },   // i86
    { BPRFX "wasm",   BPRFX "wasm" EXE_EXT,   NULL },   // i386
    { BPRFX "wasaxp", BPRFX "wasaxp" EXE_EXT, NULL },   // axp
    { BPRFX "wasmps", BPRFX "wasmps" EXE_EXT, NULL },   // mips
    { BPRFX "wasppc", BPRFX "wasppc" EXE_EXT, NULL },   // ppc
};

static etool tools_cc_arch[TARGET_ARCH_COUNT] = {
    { CC,             CC EXE_EXT,             NULL },   // default
    { BPRFX "wcc",    BPRFX "wcc" EXE_EXT,    NULL },   // i86
    { BPRFX "wcc386", BPRFX "wcc386" EXE_EXT, NULL },   // i386
    { BPRFX "wccaxp", BPRFX "wccaxp" EXE_EXT, NULL },   // axp
    { BPRFX "wccmps", BPRFX "wccmps" EXE_EXT, NULL },   // mips
    { BPRFX "wccppc", BPRFX "wccppc" EXE_EXT, NULL },   // ppc
};

static etool tools_ccxx_arch[TARGET_ARCH_COUNT] = {
    { CCXX,           CCXX EXE_EXT,           NULL },   // default
    { BPRFX "wpp",    BPRFX "wpp" EXE_EXT,    NULL },   // i86
    { BPRFX "wpp386", BPRFX "wpp386" EXE_EXT, NULL },   // i386
    { BPRFX "wppaxp", BPRFX "wppaxp" EXE_EXT, NULL },   // axp
    { BPRFX "wppmps", BPRFX "wppmps" EXE_EXT, NULL },   // mips
    { BPRFX "wppppc", BPRFX "wppppc" EXE_EXT, NULL },   // ppc
};

static etool tools_f77_arch[TARGET_ARCH_COUNT] = {
    { FC,             FC EXE_EXT,             NULL },   // default
    { BPRFX "wfc",    BPRFX "wfc" EXE_EXT,    NULL },   // i86
    { BPRFX "wfc386", BPRFX "wfc386" EXE_EXT, NULL },   // i386
    { BPRFX "wfcaxp", BPRFX "wfcaxp" EXE_EXT, NULL },   // axp
    { BPRFX "wfcmps", BPRFX "wfcmps" EXE_EXT, NULL },   // mips
    { BPRFX "wfcppc", BPRFX "wfcppc" EXE_EXT, NULL },   // ppc
};

static void print_banner( void )
{
    static int  done;

    if( done )
        return;
#if defined( _BETAVER )
    puts( banner1w1( "C/C++ " _TARGET_ " Compiler Driver Program" ) );
    puts( banner1w2( _WCL_VERSION_ ) );
#else
    puts( banner1w( "C/C++ " _TARGET_ " Compiler Driver Program", _WCL_VERSION_ ) );
#endif
    puts( banner2 );
    puts( banner2a( "1988" ) );
    puts( banner3 );
    puts( banner3a );
    done = 1;
}

static char *xlate_fname( char *name )
{
#ifndef __UNIX__
    /* On non-POSIX hosts, pathnames must be translated to format
     * expected by other tools.
     */
    char    *run = name;

    while( *run ) {
        if( *run == '/' )
            *run = '\\';
        run++;
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

static void addccstring( char *string )
{
    char    *op;
    int     i;

    if( string == NULL || strlen( string ) == 0 )
        return;

    op = MemAlloc( ( strlen( string ) + 1 ) * sizeof( char ) );
    strcpy(op, string);

    i = 0;
    while( CC_Opts[i] != NULL )
        i++;
    CC_Opts[i] = op;
    CC_Opts[i + 1] = NULL;
}

static void addcclongopt( char *option, char *tail )
{
    char    *op;
    size_t  len;

    if( option == NULL )
        return;
    len = strlen( option );
    if( len == 0 )
        return;

    /* Calculate our necessary memory here for readability */
    len += ( ( tail == NULL ) ? 0 : strlen( tail ) ) + 2;

    op = MemAlloc( len * sizeof( char ) );
    op[0] = '-';
    strcpy( op + 1, option );
    if( tail != NULL )
        strcat( op, tail );

    addccstring( op );
    MemFree( op );
}

static void addccopt( char option, char *opt )
/*************************************/
{
    char    *op;

    op = MemAlloc( (3 + (opt == NULL ? 0 : strlen(opt)))*sizeof(char) );

    op[0] = '-';
    op[1] = option;
    op[2] = '\0';
    if( opt != NULL ) {
        strcat( op, opt );
    }
    addccstring( op );
    MemFree( op );
}

static  void  MakeName( char *name, char *ext )
/*********************************************/
{
    /* If the last '.' is before the last path seperator character */
    if( strrchr( name, '.' ) <= strpbrk( name, PATH_SEPS_STR ) ) {
        strcat( name, ext );
    }
}

static  FILE *OpenSpecsFile( void )
/*********************************/
{
    FILE    *specs;

    FindPath( SPECS_FILE, PathBuffer );
    specs = fopen( PathBuffer, "r" );
    if( specs == NULL ) {
        fprintf( stderr, "Could not open specs file '%s' for reading!\n", PathBuffer );
        exit( EXIT_FAILURE );
    }
    return( specs );
}

#if 0
/*static*/  int  ListSpecsFile( void )
/************************************/
{
    FILE    *specs;
    char    line[MAX_CMD];
    size_t  begin_len;
    char    *p;

    specs = OpenSpecsFile();
    begin_len = strlen( "system begin " );
    while( fgets( line, MAX_CMD, specs ) != NULL ) {
        p = strchr( line, '\n' );
        if( p != NULL ) {
            *p = '\0';
        }
        if( strncmp( line, "system begin ", begin_len ) == 0 ) {
            printf( "%s\n", line + begin_len);
        }
    }

    fclose( specs );
    return( 0 );
}
#endif

static  int  ConsultSpecsFile( const char *target )
/*************************************************/
{
    FILE    *specs;
    char    line[MAX_CMD];
    char    start_line[MAX_CMD] = "system begin ";
    int     in_target = FALSE;
    char    *p;
    int     rc = 0;

    specs = OpenSpecsFile();

    /* search for a block whose first line is "system begin <target>" ... */
    strcat( start_line, target );
    while( fgets( line, MAX_CMD, specs ) != NULL ) {
        p = strchr( line, '\n' );
        if( p != NULL ) {
            *p = '\0';
        }
        if( stricmp( line, start_line ) == 0 ) {
            in_target = TRUE;
        } else if( stricmp( line, "end" ) == 0 ) {
            in_target = FALSE;
        } else if( in_target ) {
            for( p = line; isspace( (unsigned char)*p ); p++ )
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
        got_quote = FALSE;
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

static  int  ParseArgs( int argc, char **argv )
/*********************************************/
{
    char        *p;
    int         wcc_option;
    char        c;
    int         i;
    list        *new_item;
    char        pelc[6];

    initialize_Flags();
    DebugFlag          = 1;
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
                        "HI:i::k:L:l:M::m:"
                        "O::o:P::QSs::U:vW::wx:yz::",
#else
                        "b:CcD:Ef:g::"
                        "HI:i::k:L:l:M::m:"
                        "O::o:P::QSs::U:vW::wx::yz::",
#endif
                        EnglishHelp )) != -1 ) {

        char    *Word = NULL;
        int     found_mapping = FALSE;

        c = (char)i;
        for( i = 0; i < sizeof( mappings ) / sizeof( mappings[0] ); i++ ) {
            option_mapping  *m    = mappings + i;
            char            *tail = strchr( m->LongName, ':' );

            if( c != m->LongName[0] )
                continue;
            if( OptArg == NULL ) {
                if( m->LongName[1] == '\0' ) {
                    addcclongopt( m->WatcomName, NULL );
                    found_mapping = TRUE;
                    break;
                }
                /* non-existant argument can't match other cases */
                continue;
            }
            if( tail != NULL ) {
                if( strncmp( OptArg, m->LongName + 1, tail - m->LongName - 1 ) == 0 ) {
                    addcclongopt( m->WatcomName, OptArg + ( tail - m->LongName - 1) );
                    found_mapping = TRUE;
                    break;
                }
            } else if( strcmp( OptArg, m->LongName + 1 ) == 0 ) {
                addcclongopt( m->WatcomName, NULL );
                found_mapping = TRUE;
                break;
            }
        }
        if( found_mapping )
            continue;

        if( OptArg != NULL ) {
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
                MemFree( cpp_linewrap );
                Word[7] = 'w';
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
                if( Word[1] == '='  ||  Word[1] == '#' ) {
                    MakeName( Word, LNK_EXT );  /* add extension */
                    MemFree( Link_Name );
                    Link_Name = strfdup( Word + 2 );
                } else {
                    MemFree( Link_Name );
                    Link_Name = MemStrDup( TEMPFILE );
                }
                wcc_option = 0;
                break;
            case 'm':           /* name of map file */
                Flags.map_wanted = TRUE;
                if( Word[1] == '='  ||  Word[1] == '#' ) {
                    MemFree( Map_Name );
                    Map_Name = strfdup( Word + 2 );
                }
                wcc_option = 0;
                break;
            case 'o':           /* name of object file */
                /* parse off argument, so we get right filename
                   in linker command file */
                p = &Word[1];
                if( Word[1] == '='  ||  Word[1] == '#' ) {
                    ++p;
                }
                MemFree( Obj_Name );
                Obj_Name = strfdup( p );        /* 08-mar-90 */
                break;
            case 'r':           /* name of error report file */
                Flags.want_errfile = TRUE;
                break;
            }
            /* avoid passing on unknown options */
            wcc_option = 0;
            break;

        case 'k':               /* stack size option */
            if( Word[0] != '\0' ) {
                MemFree( StackSize );
                StackSize = MemStrDup( Word );
            }
            wcc_option = 0;
            break;

        /* compiler options that affect the linker */
        case 'c':           /* compile only */
            Flags.no_link = TRUE;
            wcc_option = 0;
            break;
        case 'x':           /* change source language */
            if( strcmp( Word, "c" ) == 0 ) {
                Flags.force_c = TRUE;
            } else if( strcmp( Word, "c++" ) == 0 ) {
                Flags.force_c_plus = TRUE;
            } else {
                Flags.no_link = TRUE;
            }
            wcc_option = 0;
            break;

        case 'm':
            if( ( strncmp( "cmodel=", Word, 7 ) == 0 )
                && ( Word[8] == '\0' ) ) {
                if( Word[7] == 't' ) {      /* tiny model */
                    Word[0] = 's';              /* change to small */
                    Flags.tiny_model = TRUE;
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
            wcc_option = 0;     /* dont' pass on unknown options */
            break;

        case 'z':                   /* 12-jan-89 */
            switch( tolower( Word[0] ) ) {
            case 's':
                Flags.no_link = TRUE;
                break;
            case 'q':
                Flags.be_quiet = TRUE;
                break;
            case 'w':
                Flags.windows = TRUE;
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
            O_Name = strfdup( OptArg );
            wcc_option = 0;
            break;
        case 'g':
            if( OptArg == NULL ) {
                Word = "2";
            } else if( !isdigit( OptArg[0] ) ) {
                c = 'h';
                if( strcmp( Word, "w" ) == 0 ) {
                    DebugFlag = 3;
                } else if( strcmp( Word, "c" ) == 0 ) { /* 02-mar-91 */
                    Flags.do_cvpack = 1;
                    DebugFlag = 4;
                } else if( strcmp( Word, "d" ) == 0 ) {
                    DebugFlag = 5;
                }
                break;
            }
            c = 'd';
        parse_d:
            if( DebugFlag == 0 ) {  /* not set by -h yet */
                if( strcmp( Word, "1" ) == 0 ) {
                    DebugFlag = 1;
                } else if( strcmp( Word, "1+" ) == 0 ) { /* 02-mar-91 */
                    DebugFlag = 2;
                } else if( strcmp( Word, "2" ) == 0 ) {
                    DebugFlag = 2;
                } else if( strcmp( Word, "2i" ) == 0 ) {
                    DebugFlag = 2;
                } else if( strcmp( Word, "2s" ) == 0 ) {
                    DebugFlag = 2;
                } else if( strcmp( Word, "3" ) == 0 ) {
                    DebugFlag = 2;
                } else if( strcmp( Word, "3i" ) == 0 ) {
                    DebugFlag = 2;
                } else if( strcmp( Word, "3s" ) == 0 ) {
                    DebugFlag = 2;
                }
            }
            break;
        case 'S':
            Flags.do_disas = TRUE;
            Flags.no_link = TRUE;
            if( DebugFlag == 0 ) {
                c = 'd';
                Word = "1";
                goto parse_d;
            }
            wcc_option = 0;
            break;
        case 's':
            if( OptArg != NULL ) {
                /* leave -shared to mapping table */
                wcc_option = 0;
                break;
            }
            Flags.strip_all = 1;
            DebugFlag = 0;
            wcc_option = 0;
            break;
        case 'v':
            Flags.be_quiet = 0;
            wcc_option = 0;
            break;
        case 'W':
            if( OptArg != NULL && strncmp( OptArg, "l,", 2 ) == 0 ) {
                AddDirective( OptArg + 2 );
                wcc_option = 0;
            }
            /* other cases handled by table */
            break;
        case 'I':
            xlate_fname( Word );
            break;
        case 'b':
            Flags.link_for_sys = TRUE;
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
            p = MemAlloc( strlen( OptArg ) + 2 + 1 );
            strcpy( p, OptArg );
            strcat( p, ".a" );
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
            if( OptArg == NULL ) {
                wcc_option = 0;
                break;
            }
            if( strcmp( OptArg, "nclude" ) == 0 ) {
                c = 'f';
                Word = MemReAlloc( Word, strlen( argv[OptInd] ) + 6 );
                if( OptInd >= argc - 1 ) {
                    MemFree( cpp_linewrap );
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
            if( OptArg == NULL ) {
                wcc_option = 0;
                break;
            }
            c = 'a';
            if( strcmp( OptArg, "D" ) == 0 ||
                strcmp( OptArg, "MD" ) == 0 ) {
                /* NB: only -MMD really matches OW's behaviour, but
                 * for now, let's accept -MD to mean the same */
                /* translate to -adt=.o */
                strcpy( Word, "dt=.o" );
            } else if( strcmp( OptArg, "F" ) == 0 ) {
                Word = MemReAlloc( Word, strlen( argv[OptInd] ) + 6 );
                if( OptInd >= argc - 1 ) {
                    MemFree( cpp_linewrap );
                    PrintMsg( "Argument of -MF missing\n", OptArg );
                    return( 1 );
                }
                strcpy( Word, "d=" );
                strfcat( Word, argv[OptInd] );
                argv[OptInd++][0] = '\0';
            } else if( strcmp( OptArg, "T") == 0 ) {
                Word = MemReAlloc( Word, strlen( argv[OptInd] ) + 6 );
                if( OptInd >= argc - 1 ) {
                    MemFree( cpp_linewrap );
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
        if( Word != NULL ) {
            MemFree( Word );
            Word = NULL;
        }
    }

    if( preprocess_only ) {
        Flags.no_link = TRUE;
        if( O_Name == NULL ) {
            MemFree( Obj_Name );           /* preprocess to stdout by default */
            Obj_Name = NULL;
        }
        pelc[0] = 'p';
        pelc[1] = '\0';

        if( cpp_encrypt_names )
            strcat( pelc, "e" );
        if( cpp_want_lines )
            strcat( pelc, "l" );
        if( cpp_keep_comments )
            strcat( pelc, "c" );

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
        addcclongopt("fo=", Obj_Name );
    }
    if( !Flags.want_errfile ) {
        addcclongopt("fr", NULL );
    }
    for( i = 1; i < argc ; i++ ) {
        Word = argv[i];
        if( Word == NULL || Word[0] == '\0' )
            /* HBB 20060217: argument was used up */
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
    int     old_argc;
    char    **old_argv;
    char    *cmdbuf;
    char    *env;
    int     ret;
    int     i;

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

static int useCPlusPlus( char *p )
/********************************/
{
    return(
        strfcmp( p, ".cp" ) == 0 ||
        strfcmp( p, ".cpp" ) == 0 ||
#ifdef __UNIX__
        strcmp( p, ".c++" ) == 0 ||
        strcmp( p, ".C" ) == 0 ||
#endif
        strfcmp( p, ".cxx" ) == 0 ||
        strfcmp( p, ".cc" )  == 0 ||
        strfcmp( p, ".hpp" ) == 0 ||
        strfcmp( p, ".hxx" ) == 0 );
}


static etool *FindToolGetPath( tool_type utl )
/********************************************/
{
    etool   *tool;

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
        FindPath( tool->exename, PathBuffer );
        tool->path = MemAlloc( strlen( PathBuffer ) + 1 );
        strcpy( tool->path, PathBuffer );
    }
    return( tool );
}

static int tool_exec( tool_type utl, char *fn, char **options )
/*******************************************************/
{
    int     rc;
    etool   *tool;
    int     i;
    int     pass_argc;
    char    *pass_argv[MAX_CC_OPTS+5];

    tool = FindToolGetPath( utl );

    pass_argv[0] = tool->name;
    pass_argc = 1; 

    while(options != NULL && options[pass_argc-1] != NULL && pass_argc < MAX_CC_OPTS) {
        pass_argv[pass_argc] = options[pass_argc-1];
        pass_argc++;
    } 

    if( utl == TYPE_DIS ) {
        pass_argv[pass_argc++] = "-s";
        pass_argv[pass_argc++] = "-a";
    }

    pass_argv[pass_argc++] = fn;
    pass_argv[pass_argc] = NULL;    

    if( !Flags.be_quiet ) {
        printf("\t");
        for( i=0; i<pass_argc; i++ )
            printf("%s ", pass_argv[i]);
        printf("\n");
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

static tool_type SrcName( char *name )
/************************************/
{
    char        *p;
    tool_type   utl;

    p = strrchr( name, '.' );
    if( p == NULL || strpbrk( p, PATH_SEPS_STR ) != NULL )
        p = name + strlen( name );
    if( strfcmp( p, ASM_EXT ) == 0 || stricmp( p, ASMS_EXT ) == 0 ) {
        utl = TYPE_ASM;
    } else if( strfcmp( p, ".f" ) == 0 || 
                stricmp( p, ".for" ) == 0 ||
                strfcmp( p, ".ftn" ) == 0 ) {
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
  #if defined(_M_I86)
    } else if( CPU_Arch == TARGET_ARCH_X86 ) {
  #else
    } else if( CPU_Arch == TARGET_ARCH_DEFAULT || CPU_Arch == TARGET_ARCH_X86 ) {
  #endif
  #if defined(__OS2__)
        Fputnl( "system os2v2", fp );
  #elif defined(__NT__)
        Fputnl( "system nt", fp );
  #elif defined(__LINUX__)
        Fputnl( "system linux", fp );
        if( !Flags.strip_all )
            Fputnl( "option exportall", fp );
  #else
        Fputnl( "system dos4g", fp );
  #endif
  #if defined(_M_I86)
    } else if( CPU_Arch == TARGET_ARCH_DEFAULT || CPU_Arch == TARGET_ARCH_I86 ) {
  #else
    } else if( CPU_Arch == TARGET_ARCH_I86 ) {
  #endif
        if( Flags.windows ) {
            Fputnl( "system windows", fp );
        } else if( Flags.tiny_model ) {
            Fputnl( "system com", fp );
        } else {
#if defined(__OS2__)
            Fputnl( "system os2", fp );
#else
            Fputnl( "system dos", fp );
#endif
        }
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
            if( !IS_OBJ( file ) ) {
                char fname[_MAX_PATH];

                rc = tool_exec( utl, DoQuoted( fname, Word, '"' ), CC_Opts );
                if( rc != 0 ) {
                    errors_found = 1;
                }
                strcpy( Word, RemoveExt( file ) );
            }
            AddNameObj( Word );
            if( Obj_List != NULL && Flags.do_disas ) {
                char    sfname[_MAX_PATH + 3];
                char    ofname[_MAX_PATH];
                char    *dis_args[2];

                sfname[0] = '-';
                sfname[1] = 'l';
                sfname[2] = '=';

                if( Exe_Name != NULL ) {     /* have "-S -o output.name" */
                    DoQuoted( ofname, file, '"' );
                    DoQuoted( sfname + 3, Exe_Name, '"' );
                } else {
                    if( IS_OBJ( file ) ) {
                        DoQuoted( ofname, file, '"' );
                        strcpy( Word, RemoveExt( file ) );
                    } else {            /* wdis needs extension */
                        DoQuoted( ofname, Obj_Name, '"' );
                    }
                    strcat( Word, ASMS_EXT );
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
            if( ( Obj_List != NULL || Flags.do_link ) && Flags.no_link == FALSE ) {
                rc = tool_exec( TYPE_LINK, "@" TEMPFILE, NULL );
                if( rc == 0 && Flags.do_cvpack ) {
                    char fname[_MAX_PATH];

                    rc = tool_exec( TYPE_PACK, DoQuoted( fname, Exe_Name, '"' ), NULL );
                }
            }
            if( Link_Name != NULL ) {
                if( strfcmp( Link_Name, TEMPFILE ) != 0 ) {
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
    Obj_Name = MemStrDup( OBJ_EXT );
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
        if( !Flags.be_quiet ) {
            print_banner();
        }
        rc = CompLink();
    }
    ProcMemFini();
    MemFini();
    return( ( rc != 0 ) );
}
