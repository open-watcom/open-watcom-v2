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
#include <conio.h>
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
#define CC          BPRFX "wcc"           /* Open Watcom C compiler (16-bit)   */
#define CCXX        BPRFX "wpp"           /* Open Watcom C++ compiler (16-bit) */
#define ASM         BPRFX "wasm"          /* Open Watcom assembler             */
#define _TARGET_    "x86 16-bit"
#elif defined( __AXP__ )
#define CC          BPRFX "wccaxp"        /* Open Watcom C compiler (32-bit)   */
#define CCXX        BPRFX "wppaxp"        /* Open Watcom C++ compiler (32-bit) */
#define ASM         BPRFX "wasaxp"        /* Open Watcom assembler             */
#define _TARGET_    "Alpha AXP"
#elif defined( __PPC__ )
#define CC          BPRFX "wccppc"        /* Open Watcom C compiler (32-bit)   */
#define CCXX        BPRFX "wppppc"        /* Open Watcom C++ compiler (32-bit) */
#define ASM         BPRFX "wasppc"        /* Open Watcom assembler             */
#define _TARGET_    "PowerPC"
#elif defined( __MIPS__ )
#define CC          BPRFX "wccmps"        /* Open Watcom C compiler (32-bit)   */
#define CCXX        BPRFX "wppmps"        /* Open Watcom C++ compiler (32-bit) */
#define ASM         BPRFX "wasmps"        /* Open Watcom assembler             */
#define _TARGET_    "MIPS"
#else
#define CC          BPRFX "wcc386"        /* Open Watcom C compiler (32-bit)   */
#define CCXX        BPRFX "wpp386"        /* Open Watcom C++ compiler (32-bit) */
#define ASM         BPRFX "wasm"          /* Open Watcom assembler             */
#define _TARGET_    "x86 32-bit"
#endif
#define PACK              "cvpack"        /* Open Watcom executable packer      */
#define LINK        BPRFX "wlink"         /* Open Watcom linker                 */
#define DIS               "wdis"          /* Open Watcom disassembler           */
#define SPECS_FILE  BPRFX "specs.owc"     /* spec file with target definition   */

#define WCLENV      "OWCC"

#define OUTPUTFILE  "a.out"
#define TEMPFILE    "__owcc__.lnk"  /* temporary linker directive file    */

#ifdef __UNIX__
#define PATH_SEPS_STR   SYS_DIR_SEP_STR
#else
#define PATH_SEPS_STR   SYS_DIR_SEP_STR "/"
#endif

char *OptEnvVar = WCLENV;           /* Data interface for GetOpt()        */

static  char    *Word;              /* one parameter                      */
static  char    *SystemName;        /* system to link for                 */
static  list    *Files_List;        /* list of filenames from Cmd         */
static  char    CC_Opts[MAX_CMD];   /* list of compiler options from Cmd  */
static  char    target_CC[20] = CC; /* name of the wcc variant to use     */
static  char    target_CCXX[20] = CCXX; /* name of the wpp variant to use     */
static  char    target_ASM[20] = ASM; /* name of the assembler to use     */
static  char    PathBuffer[_MAX_PATH];/* buffer for path name of tool     */
static  char    *Link_Name;         /* Temp_Link copy if /fd specified    */
static  list    *Directive_List;    /* linked list of directives   */
static  char    *StackSize;         /* size of stack                      */
static  int     DebugFlag;          /* debug info wanted                  */
static  char    CPU_Class;          /* [0..6]86, 'm'ips or 'a'xp          */
static  char    Conventions[2];     /* 'r' for -3r or 's' for -3s         */
static  char    *O_Name;            /* name of -o option                  */

static  char    preprocess_only;    /* flag: -E option used?              */
static  char    cpp_want_lines;     /* flag: want #lines output?          */
static  char    cpp_keep_comments;  /* flag: keep comments in output?     */
static  char    cpp_encrypt_names;  /* flag: encrypt C++ names?           */
static  char    *cpp_linewrap;      /* line length for cpp output         */

/*
 *  Static function prototypes
 */

static void MakeName( char *, char * );


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
    { "fpmath=287", "fp2" },
    { "fpmath=387", "fp3" },
    { "fptune=586", "fp5" },
    { "fptune=686", "fp6" },
    { "fno-short-enum", "ei" },
    { "fshort-enum", "em" },
    { "fsigned-char", "j" },
    { "fpack-struct=:", "zp" },
    { "ffar-data-threshold=:", "zt" },
    { "frtti", "xr" },
    { "fmessage-full-path", "ef" },
    { "femit-names", "en" },
    { "fbrowser", "db" },
    { "fhook-epilogue", "ee" },
    { "fhook-prologue=:", "ep" },
    { "fhook-prologue", "ep" },
    { "fwrite-def", "v" },
    { "fwrite-def-without-typedefs", "zg" },
    { "fno-stack-check", "s" },
    { "fgrow-stack", "sg" },
    { "fstack-probe", "st" },
    { "fno-writable-strings", "zc" },
    { "fnostdlib", "zl" },
    { "ffunction-sections", "zm" },
    { "fno-strict-aliasing", "oa" },
    { "fguess-branch-probability", "ob" },
    { "fno-optimize-sibling-calls", "oc" },
    { "finline-functions", "oe" },
    { "finline-limit=:", "oe=" },
    { "fno-omit-frame-pointer", "of" },
    { "fno-omit-leaf-frame-pointer", "of+" },
    { "frerun-optimizer", "oh" },
    { "finline-intrinsics-max", "oi+" },
    { "finline-intrinsics", "oi" },
    { "finline-fp-rounding", "zri" },
    { "fomit-fp-rounting", "zro" },
    { "fschedule-prologue", "ok" },
    { "floop-optimize", "ol" },
    { "funroll-loops", "ol+" },
    { "finline-math", "om" },
    { "funsafe-math-optimizations", "on" },
    { "ffloat-store", "op" },
    { "fschedule-insns", "or" },
    { "fkeep-duplicates", "ou" },
    { "fno-eh", "xd" },
    { "feh-direct", "xst" },
    { "feh-table", "xss" },
    { "feh", "xs" },
    { "ftabstob=:", "t=" },
    /* { "mcmodel=:", "m" }, --- handled explicitly */
    { "mabi=cdecl", "ecc" },
    { "mabi=stdcall", "ecd" },
    { "mabi=fastcall", "ecf" },
    { "mabi=pascal", "ecp" },
    { "mabi=fortran", "ecr" },
    { "mabi=syscall", "ecs" },
    { "mabi=watcall", "ecw" },
    { "mwindows", "bg" },
    { "mconsole", "bc" },
    { "mthreads", "bm" },
    { "mrtdll", "br" },
    { "mdefault-windowing", "bw" },
    { "msoft-float", "fpc" },
    { "w", "w0" },
    { "Wlevel:", "w" },
    { "Wall", "w4" },
    { "Wextra", "wx" },
    { "Werror", "we" },
    { "Wno-n:", "wcd=" }, /* NOTE: this needs to be listed before -Wn to work */
    { "Wn:", "wce=" },
    { "Woverlay", "wo" },
    { "Wpadded", "zpw" },
    { "Wc,-:", "" },
    { "Wstop-after-errors=:", "e" },
    { "ansi", "za" },
    { "std=c99", "za99" },
    { "std=c89", "za" },
    { "std=ow", "ze" },
    { "O0", "od" },
    { "O1", "oil" },
    { "O2", "onatx" },
    { "O3", "onatxl+" },
    { "Os", "os" },
    { "Ot", "ot" },
    { "O", "oil" },
    { "H", "fti" },
    { "fignore-line-directives", "pil" },
    { "fvoid-ptr-arithmetic", "zev" },
    { "shared", "bd" },
};

/* Others to be checked:
    { "-tp=<name>                      (C) set #pragma on <name>",

OW options that might be useful to add:
    -ft / -fx  non-8.3 include search options

*/

typedef enum tool_type {
    TYPE_ASM,
    TYPE_C,
    TYPE_CPP,
    TYPE_LINK,
    TYPE_PACK,
    TYPE_DIS,
    TYPE_MAX
} tool_type;

static struct {
    char *name;
    char *exename;
    char *path;
} tools[TYPE_MAX] = {
    { ASM,  ASM EXE_EXT,    NULL },
    { CC,   CC EXE_EXT,     NULL },
    { CCXX, CCXX EXE_EXT,   NULL },
    { LINK, LINK EXE_EXT,   NULL },
    { PACK, PACK EXE_EXT,   NULL },
    { DIS,  DIS EXE_EXT,    NULL }
};

void print_banner( void )
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

void addccopt( int option, char *opt )
{
    char    op[4];

    op[0] = ' ';
    op[1] = '-';
    op[2] = option;
    op[3] = '\0';
    strcat( CC_Opts, op );
    if( opt != NULL ) {
        strcat( CC_Opts, opt );
    }
}

static int FileExtension( char *p, char *ext )
/********************************************/
{
    char        *dot;

    dot = NULL;
    while( *p != '\0' ) {
        if( *p == '.' )
            dot = p;
        ++p;
    }
    if( dot != NULL ) {
        if( strfcmp( dot, ext ) == 0 ) {
            return( 1 );                /* indicate file extension matches */
        }
    }
    return( 0 );                        /* indicate no match */
}

static  void AddDirective( char *directive )
/******************************************/
{
    list    *itm;

    itm = MemAlloc( sizeof( list ) );
    itm->next = NULL;
    itm->item = MemAlloc( strlen( directive ) + 1 );
    strcpy( itm->item, directive );
    ListAppend( &Directive_List, itm );
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

/*static*/  int  ListSpecsFile( void )
/********************************/
{
    FILE    *specs;
    char    line[MAX_CMD];
    int     begin_len;
    char    *p;

    specs = OpenSpecsFile();
    begin_len = strlen( "system begin " );
    while( fgets( line, MAX_CMD, specs ) ) {
        p = strchr( line, '\n' );
        if( p ) {
            *p = '\0';
        }
        if( !strncmp( line, "system begin ", begin_len ) ) {
            printf( "%s\n", line + begin_len);
        }
    }

    fclose( specs );
    return( 0 );
}

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
    while( fgets( line, MAX_CMD, specs ) ) {
        p = strchr( line, '\n' );
        if( p != NULL ) {
            *p = '\0';
        }
        if( !stricmp( line, start_line ) ) {
            in_target = TRUE;
        } else if( !stricmp( line, "end" ) ) {
            in_target = FALSE;
        } else if( in_target ) {
            for( p = line; isspace( (unsigned char)*p ); p++ )
                ; /* do nothing else */
            p = strtok( p, " \t=" );
            if( p == NULL )
                continue;
            if( strcmp ( p, "CC" ) ) {
                p = strtok( NULL, " \t" );
                strcpy( target_CC, p );
            } else if( strcmp ( p, "CPP" ) ) {
                p = strtok( NULL, " \t" );
                strcpy( target_CCXX, p );
            } else if( strcmp ( p, "AS" ) ) {
                p = strtok( NULL, " \t" );
                strcpy( target_ASM, p );
            } else {
                continue;
            }
            p = strtok( NULL, "\n" );
            if( p != NULL ) {
                /* if there are further options, copy them */
                strcat( CC_Opts, " " );
                strcat( CC_Opts, p );
            }
            rc = 1;
            break;
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
#if defined( _M_I86 )
    Flags.is32bit         = 0;
#else
    Flags.is32bit         = 1;
#endif
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
    int         c;
    int         i;
    list        *new_item;

    initialize_Flags();
    DebugFlag          = 1;
    StackSize = NULL;
    Conventions[0]     = 'r';
    Conventions[1]     = '\0';
    preprocess_only    = 0;
    cpp_want_lines     = 1; /* NB: wcc and wcl default to 0 here */
    cpp_keep_comments  = 0;
    cpp_encrypt_names  = 0;
    cpp_linewrap       = NULL;
    O_Name             = NULL;

    AltOptChar = '-'; /* Suppress '/' as option herald */
    while( (c = GetOpt( &argc, argv,
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

        char    *Word = "";
        int     found_mapping = FALSE;

        for( i = 0; i < sizeof( mappings ) / sizeof( mappings[0] ); i++ ) {
            option_mapping  *m    = mappings + i;
            char            *tail = strchr( m->LongName, ':' );

            if( c != m->LongName[0] )
                continue;
            if( OptArg == NULL ) {
                if( m->LongName[1] == '\0' ) {
                    strcat( CC_Opts, " -" );
                    strcat( CC_Opts, m->WatcomName );
                    found_mapping = TRUE;
                    break;
                }
                /* non-existant argument can't match other cases */
                continue;
            }
            if( tail != NULL ) {
                if( !strncmp( OptArg, m->LongName + 1,
                              tail - m->LongName - 1 ) ) {
                    strcat( CC_Opts, " -" );
                    strcat( CC_Opts, m->WatcomName );
                    strcat( CC_Opts, OptArg + ( tail - m->LongName - 1) );
                    found_mapping = TRUE;
                    break;
                }
            } else if( !strcmp( OptArg, m->LongName + 1 ) ) {
                strcat( CC_Opts, " -" );
                strcat( CC_Opts, m->WatcomName );
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
            if( !strcmp( Word, "syntax-only" ) ) {
                c = 'z';
                strcpy( Word, "s" );
                Flags.no_link = 1;
                break;
            }
            if( !strncmp( Word, "cpp-wrap=", 9 ) ) {
                MemFree( cpp_linewrap );
                Word[7] = 'w';
                cpp_linewrap = MemStrDup( Word + 7 );
                wcc_option = 0;
                break;
            }
            if( !strcmp( Word, "mangle-cpp" ) ) {
                cpp_encrypt_names = 1;
                wcc_option = 0;
                break;
            }
            switch( Word[0] ) {
            case 'd':           /* name of linker directive file */
                if( Word[1] == '='  ||  Word[1] == '#' ) {
                    MakeName( Word, ".lnk" );    /* add extension */
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
            if( ( !strncmp( "cmodel=", Word, 7 ) )
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
            if( !strncmp( "regparm=", Word, 8 ) ) {
                if( !strcmp( Word + 8, "0" ) ) {
                    Conventions[0] =  's';
                } else {
                    Conventions[0] = 'r';
                }
                wcc_option = 0;
                break;
            }
            if( !strncmp( "tune=i", Word, 6 ) ) {
                switch( Word[6] ) {
                case '0':
                case '1':
                case '2':
                    CPU_Class = Word[6];
                    Conventions[0] = '\0';
                    break;
                case '3':
                case '4':
                case '5':
                case '6':
                    CPU_Class = Word[6];
                    break;
                default:
                    /* Unknown CPU type --- disable generation of this
                     * option */
                    CPU_Class = '\0';
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
            xlate_fname( Word );
            fputs( "libpath ", Fp );
            Fputnl( Word, Fp );
            wcc_option = 0;
            break;
        case 'i':       /* -include <file> --> -fi=<file> */
            if( OptArg == NULL ) {
                wcc_option = 0;
                break;
            }
            if( !strcmp( OptArg, "nclude" ) ) {
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
            if( !strcmp( OptArg, "D" ) ||
                !strcmp( OptArg, "MD" ) ) {
                /* NB: only -MMD really matches OW's behaviour, but
                 * for now, let's accept -MD to mean the same */
                /* translate to -adt=.o */
                strcpy( Word, "dt=.o" );
            } else if( !strcmp( OptArg, "F" ) ) {
                Word = MemReAlloc( Word, strlen( argv[OptInd] ) + 6 );
                if( OptInd >= argc - 1 ) {
                    MemFree( cpp_linewrap );
                    PrintMsg( "Argument of -MF missing\n", OptArg );
                    return( 1 );
                }
                strcpy( Word, "d=" );
                strfcat( Word, argv[OptInd] );
                argv[OptInd++][0] = '\0';
            } else if( !strcmp( OptArg, "T") ) {
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
        if( OptArg != NULL ) {
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
        strcat( CC_Opts, " -p" );
        if( cpp_encrypt_names )
            strcat( CC_Opts, "e" );
        if( cpp_want_lines )
            strcat( CC_Opts, "l" );
        if( cpp_keep_comments )
            strcat( CC_Opts, "c" );
        if( cpp_linewrap != NULL ) {
            strcat( CC_Opts, cpp_linewrap );
        }
    }
    if( CPU_Class )
        addccopt( CPU_Class, Conventions );
    if( Flags.be_quiet )
        addccopt( 'z', "q" );
    if( O_Name != NULL ) {
        if( Flags.no_link && !Flags.do_disas ) {
            MemFree( Obj_Name );
            Obj_Name = O_Name;
        } else {
            strcpy( Exe_Name, O_Name );
            Flags.keep_exename = 1;
            MemFree( O_Name );
        }
        O_Name = NULL;
    }
    if( Obj_Name != NULL ) {
        strcat( CC_Opts, " -fo=" );
        strcat( CC_Opts, Obj_Name );
    }
    if( !Flags.want_errfile ) {
        strcat( CC_Opts, " -fr" );
    }
    for( i = 1; i < argc ; i++ ) {
        Word = argv[i];
        if( Word == NULL || Word[0] == '\0' )
            /* HBB 20060217: argument was used up */
            continue;
        new_item = MemAlloc( sizeof( list ) );
        new_item->next = NULL;
        new_item->item = strfdup( Word );
        if( FileExtension( Word, ".lib" ) || FileExtension( Word, ".a" ) ) {
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


static char *FindToolPath( tool_type utl )
/****************************************/
{
    if( tools[utl].path == NULL ) {
        FindPath( tools[utl].exename, PathBuffer );
        tools[utl].path = MemAlloc( strlen( PathBuffer ) + 1 );
        strcpy( tools[utl].path, PathBuffer );
    }
    return( tools[utl].path );
}

static int tool_exec( tool_type utl, char *p1, char *p2 )
/*******************************************************/
{
    int     rc;

    FindToolPath( utl );
    if( !Flags.be_quiet ) {
        if( utl == TYPE_DIS ) {
            PrintMsg( "\t%s -s -a %s %s\n", tools[utl].name, p1, p2 );
        } else if( p2 == NULL ) {
            PrintMsg( "\t%s %s\n", tools[utl].name, p1 );
        } else {
            PrintMsg( "\t%s %s %s\n", tools[utl].name, p1, p2 );
        }
    }
    fflush( NULL );
    if( utl == TYPE_DIS ) {
        rc = spawnlp( P_WAIT, tools[utl].path, tools[utl].name, "-s", "-a", p1, p2, NULL );
    } else if( p2 == NULL ) {
        rc = spawnlp( P_WAIT, tools[utl].path, tools[utl].name, p1, NULL );
    } else {
        rc = spawnlp( P_WAIT, tools[utl].path, tools[utl].name, p1, p2, NULL );
    }
    if( rc != 0 ) {
        if( (rc == -1) || (rc == 255) ) {
            PrintMsg( WclMsgs[UNABLE_TO_INVOKE_EXE], tools[utl].path );
        } else {
            if( utl == TYPE_LINK ) {
                PrintMsg( WclMsgs[LINKER_RETURNED_A_BAD_STATUS] );
            } else if( utl == TYPE_PACK ) {
                PrintMsg( WclMsgs[CVPACK_RETURNED_A_BAD_STATUS] );
            } else {
                PrintMsg( WclMsgs[COMPILER_RETURNED_A_BAD_STATUS], p1 );
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
    if( strfcmp( p, ".asm" ) == 0 || stricmp( p, ".s" ) == 0 ) {
        utl = TYPE_ASM;
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

static  int  CompLink( void )
/***************************/
{
    int         rc;
    char        *p;
    char        *file;
    char        *path;
    list        *itm;
    char        errors_found;
    tool_type   utl;
    int         i;

    if( Flags.be_quiet ) {
        Fputnl( "option quiet", Fp );
    }
    fputs( DebugOptions[DebugFlag], Fp );
    if( StackSize != NULL ) {
        fputs( "option stack=", Fp );
        Fputnl( StackSize, Fp );
    }
    if( Flags.link_for_sys ) {
        fputs( "system ", Fp );
        Fputnl( SystemName, Fp );
    } else if( Flags.is32bit ) {
  #if defined(__OS2__)
        Fputnl( "system os2v2", Fp );
  #elif defined(__NT__)
        Fputnl( "system nt", Fp );
  #elif defined(__LINUX__)
        Fputnl( "system linux", Fp );
        if( !Flags.strip_all )
            Fputnl( "option exportall", Fp );
  #else
        Fputnl( "system dos4g", Fp );
  #endif
    } else {
        if( Flags.windows ) {                   /* 15-mar-90 */
            Fputnl( "system windows", Fp );
        } else if( Flags.tiny_model ) {
            Fputnl( "system com", Fp );
        } else {
#if defined(__OS2__)
            Fputnl( "system os2", Fp );
#else
            Fputnl( "system dos", Fp );
#endif
        }
    }
    for( itm = Directive_List; itm != NULL; itm = itm->next ) {
        Fputnl( itm->item, Fp );
    }
    Word = MemAlloc( MAX_CMD );
    errors_found = 0;
    for( itm = Files_List; itm != NULL; itm = itm->next ) {
        strcpy( Word, itm->item );
        utl = SrcName( Word );          /* if no extension, assume .c */
        file = GetName( Word );         /* get first matching filename */
        path = MakePath( Word );        /* isolate path portion of filespec */
        while( file != NULL ) {         /* while more filenames: */
            strcpy( Word, path );
            strcat( Word, file );
            if( !FileExtension( Word, OBJ_EXT ) &&  // if not .obj or .o, compile
                !FileExtension( Word, OBJ_EXT_SECONDARY ) ) {
                rc = tool_exec( utl, Word, CC_Opts );
                if( rc != 0 ) {
                    errors_found = 1;
                }
                p = strrchr( file, '.' );
                if( p != NULL )  {
                    *p = '\0';
                }
                strcpy( Word, file );
            }
            AddName( Word, Fp );
            if( Obj_List != NULL && Flags.do_disas ) {
                char    *sfile;
                char    *ofile;

                ofile = MemAlloc( strlen( file ) + 6 );
                strcpy( ofile, file );

                if( Exe_Name[0] != '\0' ) {     /* have "-S -o output.name" */
                    sfile = Exe_Name;
                } else {
                    if( FileExtension( Word, OBJ_EXT ) ||
                        FileExtension( Word, OBJ_EXT_SECONDARY ) ) {
                        p = strrchr( file, '.' );
                        if( p != NULL )  {
                            *p = '\0';
                        }
                        strcpy( Word, file );
                    } else {            /* wdis needs extension */
                        strcat( ofile, Obj_Name );
                    }
                    sfile = Word;
                    strcat( Word, ".s" );
                }
                memmove( sfile + 3, sfile, strlen( sfile ) + 1 );
                sfile[0] = '-';
                sfile[1] = 'l';
                sfile[2] = '=';
                rc = tool_exec( TYPE_DIS, ofile, sfile );
                MemFree( ofile );
            }
            if( Exe_Name[0] == '\0' ) {
#ifdef __UNIX__
                strcpy( Exe_Name, OUTPUTFILE );
                Flags.keep_exename = 1;
#else
                p = strrchr( Word, '.' );
                if( p != NULL ) {
                    *p = '\0';
                }
                strcpy( Exe_Name, Word );
#endif
            }
#ifdef __UNIX__
            MemFree( file );
#endif
            file = GetName( NULL );     /* get next filename */
        }
        MemFree( path );
    }
    if( errors_found ) {
        rc = 1;
    } else {
        rc = 0;
        BuildLinkFile();
        if(( Obj_List != NULL || Flags.do_link ) && Flags.no_link == FALSE ) {
            rc = tool_exec( TYPE_LINK, "@" TEMPFILE, NULL );
            if( rc == 0 && Flags.do_cvpack ) {
                rc = tool_exec( TYPE_PACK, Exe_Name, NULL );
            }
            if( rc != 0 ) {
                rc = 2;     /* return 2 to show Temp_File already closed */
            }
        }
    }
    if( Word != NULL ) {
        MemFree( Word );
        Word = NULL;
    }
    for( i = 0; i < TYPE_MAX; ++i ) {
        if( tools[i].path != NULL ) {
            MemFree( tools[i].path );
            tools[i].path = NULL;
        }
    }
    return( rc );
}


static  void  MakeName( char *name, char *ext )
/*********************************************/
{
    /* If the last '.' is before the last path seperator character */
    if( strrchr( name, '.' ) <= strpbrk( name, PATH_SEPS_STR ) ) {
        strcat( name, ext );
    }
}

static void ExitHandler( void )
/*****************************/
{
    if( Fp != NULL )
        fclose( Fp );
    remove( TEMPFILE );
}

static int ProcMemInit( void )
{
    Map_Name = NULL;
    Obj_Name = MemStrDup( ".o" );
    Link_Name = NULL;
    SystemName = NULL;
    StackSize = NULL;
    Directive_List = NULL;
    Files_List = NULL;
    Obj_List = NULL;
    Libs_List = NULL;
    return( 0 );
}

static int ProcMemFini( void )
{
    ListFree( Directive_List );
    ListFree( Files_List );
    ListFree( Obj_List );
    ListFree( Libs_List );
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

    errno = 0; /* Standard C does not require fopen failure to set errno */
    if( (Fp = fopen( TEMPFILE, "w" )) == NULL ) {
        /* Message before banner decision as '@' option uses Fp in Parse() */
        PrintMsg( WclMsgs[UNABLE_TO_OPEN_TEMPORARY_FILE], TEMPFILE,
            strerror( errno ) );
        exit( EXIT_FAILURE );
    }
    MemInit();
    ProcMemInit();
    /* destruct the temp. linker script in case of -? or parse errors */
    atexit( ExitHandler );
    rc = Parse( argc, argv );
    if( rc == 0 ) {
        if( !Flags.be_quiet ) {
            print_banner();
        }
        rc = CompLink();
    }
    if( rc == 1 ) {
        fclose( Fp );
        Fp = NULL;
    }
    if( Link_Name != NULL ) {
        if( strfcmp( Link_Name, TEMPFILE ) != 0 ) {
            remove( Link_Name );
            rename( TEMPFILE, Link_Name );
        }
    } else {
        remove( TEMPFILE );
    }
    ProcMemFini();
    MemFini();
    return( rc == 0 ? 0 : 1 );
}
