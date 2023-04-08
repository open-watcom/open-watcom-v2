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
* Description:  C/C++ Compile and Link utility.
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
    #include <process.h>
#endif
#ifdef __UNIX__
    #include <dirent.h>
#else
    #include <direct.h>
#endif
#include "bool.h"
#include "wio.h"
#include "watcom.h"
#include "swchar.h"
#include "diskos.h"
#include "clcommon.h"
#include "banner.h"
#include "pathgrp2.h"

#include "clibext.h"


#ifdef BOOTSTRAP
#define BPRFX   "b"
#else
#define BPRFX   ""
#endif

#if defined( WCLI86 )
  #define WCLNAME       BPRFX "wcl"         /* Name of Compile and Link Utility */
  #define CC            BPRFX "wcc"         /* Open Watcom C compiler           */
  #define CPP           BPRFX "wpp"         /* Open Watcom C++ compiler         */
  #define AS            BPRFX "wasm"        /* Open Watcom assembler            */
  #define FC            BPRFX "wfc"         /* Open Watcom F77 compiler         */
#elif defined( WCLAXP )
  #define WCLNAME       BPRFX "wclaxp"      /* Name of Compile and Link Utility */
  #define CC            BPRFX "wccaxp"      /* Open Watcom C compiler           */
  #define CPP           BPRFX "wppaxp"      /* Open Watcom C++ compiler         */
  #define AS            BPRFX "wasaxp"      /* Open Watcom assembler            */
  #define FC            BPRFX "wfcaxp"      /* Open Watcom F77 compiler         */
#elif defined( WCLPPC )
  #define WCLNAME       BPRFX "wclppc"      /* Name of Compile and Link Utility */
  #define CC            BPRFX "wccppc"      /* Open Watcom C compiler           */
  #define CPP           BPRFX "wppppc"      /* Open Watcom C++ compiler         */
  #define AS            BPRFX "wasppc"      /* Open Watcom assembler            */
  #define FC            BPRFX "wfcppc"      /* Open Watcom F77 compiler         */
#elif defined( WCLMPS )
  #define WCLNAME       BPRFX "wclmps"      /* Name of Compile and Link Utility */
  #define CC            BPRFX "wccmps"      /* Open Watcom C compiler           */
  #define CPP           BPRFX "wppmps"      /* Open Watcom C++ compiler         */
  #define AS            BPRFX "wasmps"      /* Open Watcom assembler            */
  #define FC            BPRFX "wfcmps"      /* Open Watcom F77 compiler         */
#else
  #define WCLNAME       BPRFX "wcl386"      /* Name of Compile and Link Utility */
  #define CC            BPRFX "wcc386"      /* Open Watcom C compiler           */
  #define CPP           BPRFX "wpp386"      /* Open Watcom C++ compiler         */
  #define AS            BPRFX "wasm"        /* Open Watcom assembler            */
  #define FC            BPRFX "wfc386"      /* Open Watcom F77 compiler         */
#endif
#define PACK                  "cvpack"      /* Open Watcom executable packer    */
#define LINK            BPRFX "wlink"       /* Open Watcom linker               */
#define DIS                   "wdis"        /* Open Watcom disassembler         */

#if defined( WCLI86 )
  #define WCLENV        "WCL"               /* name of environment variable     */
  #define STACKSIZE     "4096"              /* default stack size               */
  #define _TARGET_      "x86 16-bit"
#elif defined( WCLAXP )
  #define WCLENV        "WCLAXP"            /* name of environment variable     */
  #define STACKSIZE     "8192"              /* default stack size               */
  #define _TARGET_      "Alpha AXP"
#elif defined( WCLPPC )
  #define WCLENV        "WCLPPC"            /* name of environment variable     */
  #define STACKSIZE     "8192"              /* default stack size               */
  #define _TARGET_      "PowerPC"
#elif defined( WCLMPS )
  #define WCLENV        "WCLMPS"            /* name of environment variable     */
  #define STACKSIZE     "8192"              /* default stack size               */
  #define _TARGET_      "MIPS"
#else
  #define WCLENV        "WCL386"            /* name of environment variable     */
  #define STACKSIZE     "8192"              /* default stack size               */
  #define _TARGET_      "x86 32-bit"
#endif

#define TEMPFILE        "__wcl__" TOOL_LNK_EXT  /* temporary linker directive file 8.3 */

#ifdef __UNIX__
#define IS_OPT(x)       ((x)=='-')
#else
#define IS_OPT(x)       ((x)=='-' || (x)==alt_switch_char)
#endif

#define IS_WS(x)        ((x)==' ' || (x)=='\t')

#define IS_ASM(x)       (x[0] == '.' && fname_cmp(x + 1, ASM_EXT) == 0)
#define IS_LIB(x)       HasFileExtension(x, LIB_EXT)
#define IS_RES(x)       HasFileExtension(x, RES_EXT)

#define SKIP_SPACES(x)  while( IS_WS( *x ) ) ++x

static  char    *Word;                      /* one parameter                        */
static  char    *SystemName;                /* system to link for                   */
static  list    *Files_List;                /* list of filenames from Cmdl          */
static  list    *Res_List;                  /* list of resources from Cmdl          */
static  char    *CC_Opts;                   /* list of compiler options from Cmdl   */
static  char    *Link_Name;                 /* Temp_Link copy if /fd specified      */
static  char    Conventions;                /* 'r' for -3r or 's' for -3s           */
#ifndef __UNIX__
static  char    alt_switch_char;            /* alternate switch character           */
#endif
static  int     via_environment = false;

/*
 *  Static function prototypes
 */
static int     Parse( const char *cmd );

const char *WclMsgs[] = {
    #define pick( code, english )   english
    #include "wclmsg.h"
    #undef pick
};

static const char EngUsageText[] = {
    #include "usage.gh"
};


static etool tools[TYPE_MAX] = {
    { LINK, LINK TOOL_EXE_EXT,   NULL },
    { PACK, PACK TOOL_EXE_EXT,   NULL },
    { DIS,  DIS  TOOL_EXE_EXT,   NULL },
    { AS,   AS   TOOL_EXE_EXT,   NULL },
    { CC,   CC   TOOL_EXE_EXT,   NULL },
    { CPP,  CPP  TOOL_EXE_EXT,   NULL },
    { FC,   FC   TOOL_EXE_EXT,   NULL }
};


static void initialize_Flags( void )
/**********************************/
{
    Flags.math_8087       = 1;
    Flags.map_wanted      = 0;
    Flags.link_ignorecase = 0;
    Flags.tiny_model      = 0;
    Flags.be_quiet        = 0;
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


static int handle_environment_variable( const char *env )
/*******************************************************/
// This is an adaptation of code in sdk/rc/rc/c/param.c
{
    typedef struct EnvVarInfo {
        struct EnvVarInfo       *next;
        char                    *varname;
        // An anonymous copy of Word and *env is appended here
    } EnvVarInfo;
    EnvVarInfo          *info;
    static EnvVarInfo   *stack = 0; // Needed to detect recursion.
    size_t              argbufsize;
    size_t              varlen;     // size to hold varname copy.
    int                 result;     // Parse Result.

    for( info = stack; info != NULL; info = info->next ) {
        if( fname_cmp( Word, info->varname ) == 0 ) {
            PrintMsg( WclMsgs[RECURSIVE_ENVIRONMENT_VARIABLE], Word );
            return( 1 );
        }
    }
    argbufsize = strlen( env ) + 1;         // inter-parameter spaces map to 0
    varlen = strlen( Word ) + 1;            // Copy taken to detect recursion.
    info = MemAlloc( sizeof( *info ) + argbufsize + varlen );
    info->next = stack;
    stack = info;                           // push info on stack
    info->varname = (char *)info + sizeof( *info );
    strcpy( info->varname, Word );
    result = Parse( strcpy( info->varname + varlen, env ) );
    stack = info->next;                     // pop stack
    MemFree( info );
    return( result );
}


/*
 * makeTmpEnv() and killTmpEnv() ar adapted from synonyms in wmake/c/mexec.c.
 * They are simpler because support for recursion is not needed.
 */
static char *makeTmpEnv( char *arg )
/**********************************/
/*
 *  Copy arg into an environment var if possible.  If succeeds, then changes
 *  arg to just "@WCLxxxxx", and returns non-zero.  Otherwise leaves
 *  arg alone and returns zero.
 */
{
    int         wcl_index;
    char        buf[20];    /* "WCLxxxxx=" + '\0' = 11 + room for FmtStr */
    size_t      len;
    size_t      buflen;
    char        *env;

    wcl_index = 1;
    while( sprintf( buf, "WCL%d", wcl_index ), getenv( buf ) != NULL ) {
        ++wcl_index;
    }
    len = strlen( arg );
    buflen = strlen( buf );
    if( len < 4 + buflen )  /* need room for " @WCLxxxxx" */
        return( NULL );
                            /* "WCLxxxxx=" + arg + '\0' */
    env = MemAlloc( len + buflen + 2 );
    sprintf( env, "%s=%s", buf, arg );
    if( putenv( env ) != 0 ) {
        MemFree( env );
        return( NULL );
    }
    if( !Flags.be_quiet )
        PrintMsg( "\tset %s\n", env );
    sprintf( arg, " @%s", buf );
    return( env );
}


static void killTmpEnv( char *env )
/*********************************/
{
    char const * const  equals = strchr( env, '=' );

    if( equals != NULL ) {
        size_t const    chars = 1 + (size_t)(equals - env);
        char * const    never_free = MemAlloc( 1 + chars );

        strncpy( never_free, env, chars );
        never_free[chars] = 0;
        if( !Flags.be_quiet )
            PrintMsg( "\tset %s\n", never_free );
        if( putenv( never_free ) == 0 )
            MemFree( env );
        MemFree( never_free ); /* Actually OK */
    }
}


static void print_banner( void )
/******************************/
{
    static bool printed = false;

    if( !printed ) {
        printed = true;
        if( !Flags.be_quiet ) {
#if defined( _BETAVER )
            puts( banner1w1( "C/C++ " _TARGET_ " Compile and Link Utility" ) );
            puts( banner1w2( _WCL_VERSION_ ) );
#else
            puts( banner1w( "C/C++ " _TARGET_ " Compile and Link Utility", _WCL_VERSION_ ) );
#endif
            puts( banner2 );
            puts( banner2a( 1988 ) );
            puts( banner3 );
            puts( banner3a );
        }
    }
}


static void  Usage( void )
/************************/
{
    char const  *p;

    print_banner();
    if( !Flags.be_quiet ) {
        puts( "" );
    }
    for( p = EngUsageText; *p != '\0'; ) {
        puts( p );
        while( *p++ != '\0' ) ;
    }
}


static const char *ScanFName( const char *end, char *buff )
/*********************************************************/
/* Allow switch chars in unquoted filename */
{
    if( end[-1] != '"' ) {
        for( ; *end != '\0'; ) {
            if( IS_WS( *end ) )
                break;
            *buff++ = *end++;
        }
        *buff = '\0';
    }
    return( end );
}

static int isWSOrOpt( char ch, char opt )
{
    if( IS_WS( ch ) )
        return( 1 );
    /* if we are processing a switch, stop at a switch */
    if( opt == '-' && IS_OPT( ch ) )
        return( 1 );
    return( 0 );
}

static int UnquoteDirective( char *dst, size_t maxlen, const char *src )
/***********************************************************************
 * Removes doublequote characters from filename and copies other content
 * from src to dst. Only maxlen number of characters are copied to dst
 * including terminating NUL character. Returns value 1 when quotes was
 * removed from orginal filename, 0 otherwise.
 */
{
    char    string_open = 0;
    size_t  pos = 0;
    char    c;
    int     un_quoted = 0;

    // leave space for NUL terminator
    maxlen--;

    while( (c = *src++) != '\0' && pos < maxlen ) {
        if( c == '\\' ) {
            c = *src++;
            if( c == '"' ) {
                un_quoted = 1;
            } else {
                *dst++ = '\\';
                pos++;
                if( pos >= maxlen ) {
                    break;
                }
            }
        } else if( c == '"' ) {
            string_open = !string_open;
            un_quoted = 1;
            continue;
        } else if( !string_open && IS_WS( c ) ) {
            break;
        }
        *dst++ = c;
        pos++;
    }
    *dst = '\0';

    return( un_quoted );
}

static void NormalizeFName( char *dst, size_t maxlen, const char *src )
/***********************************************************************
 * Removes doublequote characters from filename and copies other content
 * from src to dst. Only maxlen number of characters are copied to dst
 * including terminating NUL character. Returns value 1 when quotes was
 * removed from orginal filename, 0 otherwise.
 */
{
    char    string_open = 0;
    size_t  pos = 0;
    char    c;

    // leave space for NUL terminator
    maxlen--;

    while( (c = *src++) != '\0' && pos < maxlen ) {
        if( c == '"' ) {
            string_open = !string_open;
            continue;
        }
        if( string_open && c == '\\' ) {
            c = *src++;
            if( c != '"' ) {
                *dst++ = '\\';
                pos++;
                if( pos >= maxlen ) {
                    break;
                }
            }
        }
#ifndef __UNIX__
        if( c == '/' )
            c = '\\';
#endif
        *dst++ = c;
        pos++;
    }
    *dst = '\0';
}

static const char *FindNextWSOrOpt( const char *str, char opt )
/**************************************************************
 * Finds next free white space character, allowing doublequotes to
 * be used to specify strings with white spaces.
 */
{
    char    string_open = 0;
    char    c;

    while( (c = *str) != '\0' ) {
        if( c == '\\' ) {
            c = *(++str);
            if( c == '\0' || !string_open && isWSOrOpt( c, opt ) ) {
                break;
            }
        } else if( c == '"' ) {
            string_open = !string_open;
            /* stop on next character after end quote */
            if( string_open == 0 ) {
                str++;
                break;
            }
        } else if( !string_open && isWSOrOpt( c, opt ) ) {
            break;
        }
        str++;
    }
    return( str );
}

static int Parse( const char *cmd )
/*********************************/
{
    char        opt;
    const char  *end;
    const char  *file_end;
    FILE        *atfp;
    char        buffer[_MAX_PATH];
    size_t      len;
    char        *p;
    int         wcc_option;
    list        *new_item;

    /* cmd will always begin with at least one */
    /* non-space character if we get this far  */

    for( ;; ) {
        SKIP_SPACES( cmd );
        if( *cmd == '\0' )
            break;
        if( IS_OPT( *cmd ) ) {
            cmd++;
            opt = '-';
        } else if( *cmd == '@' ) {
            cmd++;
            opt = '@';
        } else {
            opt = ' ';
        }
        end = FindNextWSOrOpt( cmd, opt );
        len = end - cmd;
        if( len > 0 ) {
            memcpy( Word, cmd, len );
            Word[len] = '\0';
            if( opt == ' ' ) {          /* if filename, add to list */
                end = ScanFName( end, Word + len );
                NormalizeFName( Word, MAX_CMD, Word );
                new_item = MemAlloc( sizeof( list ) );
                new_item->next = NULL;
                new_item->item = MemStrDup( Word );
                if( IS_LIB( Word ) ) {
                    ListAppend( &Libs_List, new_item );
                } else if( IS_RES( Word ) ) {
                    ListAppend( &Res_List, new_item );
                } else {
                    ListAppend( &Files_List, new_item );
                }
            } else if( opt == '@' ) {
                if( len > 0 ) {
                    char const  *env;

                    env = getenv( Word );
                    if( env != NULL ) {
                        if( handle_environment_variable( env ) ) {
                            return( 1 );          // Recursive call failed
                        }
                        via_environment = true;
                        cmd = end;
                        continue;
                    }
                    end = ScanFName( end, Word + len );
                    NormalizeFName( Word, MAX_CMD, Word );
                    MakeName( Word, TOOL_LNK_EXT );
                    errno = 0;
                    if( (atfp = fopen( Word, "r" )) == NULL ) {
                        PrintMsg( WclMsgs[UNABLE_TO_OPEN_DIRECTIVE_FILE], Word, strerror(  errno ) );
                        return( 1 );
                    }
                    while( fgets( buffer, sizeof( buffer ), atfp ) != NULL ) {
                        p = strchr( buffer, '\n' );
                        if( p != NULL )
                            *p = '\0';
                        if( strnicmp( buffer, "file ", 5 ) == 0 ) {
                            /* look for names separated by ','s */
                            AddNameObj( buffer + 5 );
                            Flags.do_link = true;
                        } else {
                            AddDirective( buffer );
                        }
                    }
                    fclose( atfp );
                }
                wcc_option = 0;
            } else {                    /* otherwise, do option */
                wcc_option = 1;         /* assume it's a wcc option */
                switch( tolower( Word[0] ) ) {
                case 'b':               /* possibly -bcl */
                    if( strnicmp( Word + 1, "cl=", 3 ) == 0 ) {
                        Flags.link_for_sys = true;
                        MemFree( SystemName );
                        SystemName = MemStrDup( Word + 4 );
                        strcpy( Word, "bt=" );
                        strcpy( Word + 3, SystemName );
                    }
                    break;
                case 'f':               /* files option */
                    file_end = ScanFName( end, Word + len );
                    switch( tolower( Word[1] ) ) {
                    case 'd':           /* name of linker directive file */
                        if( Link_Name != NULL )
                            MemFree( Link_Name );
                        if( Word[2] == '=' || Word[2] == '#' ) {
                            end = file_end;
                            NormalizeFName( Word, MAX_CMD, Word + 3 );
                            MakeName( Word, TOOL_LNK_EXT );  /* add extension */
                            Link_Name = MemAlloc( strlen( Word ) + 1 );
                            strcpy( Link_Name, Word );
                        } else {
                            Link_Name = MemStrDup( TEMPFILE );
                        }
                        wcc_option = 0;
                        break;
                    case 'e':           /* name of exe file */
                        if( Word[2] == '=' || Word[2] == '#' ) {
                            end = file_end;
                            NormalizeFName( Word, MAX_CMD, Word + 3 );
                            MemFree( Exe_Name );
                            Exe_Name = MemStrDup( Word );
                        }
                        wcc_option = 0;
                        break;
                    case 'i':           /* name of forced include file */
                        end = file_end;
                        break;
                    case 'm':           /* name of map file */
                        Flags.map_wanted = true;
                        if( Word[2] == '=' || Word[2] == '#' ) {
                            end = file_end;
                            NormalizeFName( Word, MAX_CMD, Word + 3 );
                            MemFree( Map_Name );
                            Map_Name = MemStrDup( Word );
                        }
                        wcc_option = 0;
                        break;
                    case 'o':           /* name of object file */
                        end = file_end;
                        p = Word + 2;
                        if( *p == '=' || *p == '#' )
                            ++p;
                        NormalizeFName( p, MAX_CMD, p );
                        MemFree( Obj_Name );
                        Obj_Name = MemStrDup( p );
                        break;
#if defined( WCLI86 ) || defined( WCL386 )
                    case 'p':           /* floating-point option */
                        if( tolower( Word[2] ) == 'c' ) {
                            Flags.math_8087 = 0;
                        }
                        break;
#endif
                    default:
                        end = file_end;
                        break;
                    }
                    break;
                case 'k':               /* stack size option */
                    if( len > 1 ) {
                        MemFree( StackSize );
                        StackSize = MemStrDup( Word + 1 );
                    }
                    wcc_option = 0;
                    break;
                case 'l':               /* link target option */
                    opt = (char)tolower( Word[1] );
                    if( opt == 'p' && Word[2] == '\0' ) {
                        Flags.link_for_dos = 0;
                        Flags.link_for_os2 = true;
                    } else if( opt == 'r' && Word[2] == '\0' ) {
                        Flags.link_for_dos = true;
                        Flags.link_for_os2 = 0;
                    } else {
                        Flags.link_for_sys = true;
                        p = Word + 1;
                        if( opt == '=' || opt == '#' )
                            ++p;
                        MemFree( SystemName );
                        SystemName = MemStrDup( p );
                    }
                    wcc_option = 0;
                    break;

                /* compiler options that affect the linker */
#ifdef WCL386
                case '3':
                case '4':
                case '5':
                    Conventions = (char)tolower( Word[1] );
                    break;
#endif
                case 'd':
                    if( len == 2 ) {
                        switch( Word[1] ) {
                        case '0':
                            DebugFlag = DBG_NONE;
                            break;
                        case '1':
                            DebugFlag = DBG_LINES;
                            break;
                        case '2':
                            DebugFlag = DBG_ALL;
                            break;
                        case '3':
                            DebugFlag = DBG_ALL;
                            break;
                        default:
                            break;
                        }
                    } else if( len == 3 ) {
                        opt = (char)tolower( Word[2] );
                        if( Word[1] == '1' && opt == '+' ) {
                            DebugFlag = DBG_ALL;
                        } else if( Word[1] == '2' && opt == 'i' ) {
                            DebugFlag = DBG_ALL;
                        } else if( Word[1] == '2' && opt == 's' ) {
                            DebugFlag = DBG_ALL;
                        } else if( Word[1] == '3' && opt == 'i' ) {
                            DebugFlag = DBG_ALL;
                        } else if( Word[1] == '3' && opt == 's' ) {
                            DebugFlag = DBG_ALL;
                        }
                    }
                    break;
                case 'h':
                    if( len == 2 ) {
                        switch( tolower( Word[1] ) ) {
                        case 'w':
                            DebugFormat = DBG_FMT_WATCOM;
                            break;
                        case 'c':
                            Flags.do_cvpack = 1;
                            DebugFormat = DBG_FMT_CODEVIEW;
                            break;
                        case 'd':
                            DebugFormat = DBG_FMT_DWARF;
                            break;
                        default:
                            break;
                        }
                    }
                    break;
                case 'i':           /* include file path */
                    end = ScanFName( end, Word + len );
                    break;
                case 'c':           /* compile only */
                    if( tolower( Word[1] ) == 'c' && Word[2] == '\0' ) {
                        Flags.force_c = true;
                    } else if( stricmp( Word + 1, "c++" ) == 0 ) {
                        Flags.force_c_plus = true;
                    } else {
                        Flags.no_link = true;
                    }
                    /* fall through */
                case 'y':
                    wcc_option = 0;
                    break;
#if defined( WCLI86 ) || defined( WCL386 )
                case 'm':           /* memory model */
                    /* if tiny model specified then change to small for compilers */
                    if( tolower( Word[1] ) == 't' && Word[2] == '\0' ) {
                        Word[1] = 's';
                        Flags.tiny_model = true;
                    }
                    break;
#endif
                case 'p':
                    Flags.no_link = true;
                    break;      /* this is a preprocessor option */
                case 'q':
                    Flags.be_quiet = true;
                    break;
                case 'z':
                    if( len == 2 ) {
                        switch( tolower( Word[1] ) ) {
                        case 's':
                            Flags.no_link = true;
                            break;
                        case 'q':
                            Flags.be_quiet = true;
                            break;
#ifdef WCLI86
                        case 'w':
                            Flags.windows = true;
                            break;
#endif
                        default:
                            break;
                        }
                    }
                    break;
                case '"':
                    /* As parameter passing to linker is a special case, we need to pass
                     * whole command instead of first character removed. This allows us
                     * to parse also string literals in AddDirective.
                     */
                    UnquoteDirective( Word, MAX_CMD, Word );
                    AddDirective( Word );
                    wcc_option = 0;
                    break;
                }

                /* don't add linker-specific options */
                /* to compiler command line:     */

                if( wcc_option ) {
                    p = CC_Opts + strlen( CC_Opts );
                    *p++ = ' ';
                    *p++ = '-';
                    strcpy( p, Word );      /* keep original case */
                }
            }
            cmd = end;
        }
    }

    return( 0 );
}


static int useCPlusPlus( const char *p )
/**************************************/
{
    return( *p++ == '.' && (
        fname_cmp( p, "cpp" ) == 0 ||
        fname_cmp( p, "cxx" ) == 0 ||
        fname_cmp( p, "cc" )  == 0 ||
        fname_cmp( p, "hpp" ) == 0 ||
        fname_cmp( p, "hxx" ) == 0 ) );
}


static etool *FindToolGetPath( tool_type utl )
/********************************************/
{
    char    *buffer;

    if( tools[utl].path == NULL ) {
        buffer = MemAlloc( MAX_CMD );
        FindPath( tools[utl].exename, buffer );
        tools[utl].path = MemAlloc( strlen( buffer ) + 1 );
        strcpy( tools[utl].path, buffer );
        MemFree( buffer );
    }
    return( &tools[utl] );
}

static int tool_exec( tool_type utl, const char *p1, const char *p2 )
/*******************************************************************/
{
    int     rc;
    etool   *tool;

    tool = FindToolGetPath( utl );
    if( !Flags.be_quiet ) {
        if( p2 == NULL ) {
            PrintMsg( "\t%s %s\n", tool->name, p1 );
        } else {
            PrintMsg( "\t%s %s %s\n", tool->name, p1, p2 );
        }
    }
    fflush( NULL );
    if( p2 == NULL ) {
        rc = (int)spawnlp( P_WAIT, tool->path, tool->name, p1, NULL );
    } else {
        rc = (int)spawnlp( P_WAIT, tool->path, tool->name, p1, p2, NULL );
    }
    if( rc != 0 ) {
        if( (rc == -1) || (rc == 255) ) {
            PrintMsg( WclMsgs[UNABLE_TO_INVOKE_EXE], tool->path );
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
    pgroup2     pg;
    tool_type   utl;

    _splitpath2( name, pg.buffer, NULL, NULL, NULL, &pg.ext );
    if( pg.ext[0] == '\0' ) {
        pg.ext = name + strlen( name );
        strcpy( pg.ext, ".cxx" );
        if( access( name, F_OK ) != 0 ) {
            strcpy( pg.ext, ".cpp" );
            if( access( name, F_OK ) != 0 ) {
                strcpy( pg.ext, ".cc" );
                if( access( name, F_OK ) != 0 ) {
                    strcpy( pg.ext, "." ASM_EXT );
                    if( access( name, F_OK ) != 0 ) {
                        strcpy( pg.ext, ".c" );
                    }
                }
            }
        }
    }
    if( IS_ASM( pg.ext ) ) {
        utl = TYPE_ASM;
    } else {
        utl = TYPE_C;               // assume C compiler
        if( !Flags.force_c ) {
            if( Flags.force_c_plus || useCPlusPlus( pg.ext ) ) {
                utl = TYPE_CPP;     // use C++ compiler
            }
        }
    }
    return( utl );
}

void BuildSystemLink( FILE *fp )
{
    list    *itm;

    if( Flags.link_for_sys ) {
        fputs( "system ", fp );
        Fputnl( SystemName, fp );
    } else {
#if defined( WCLI86 )
        if( Flags.windows ) {
            Fputnl( "system windows", fp );
        } else if( Flags.tiny_model ) {
            Fputnl( "system com", fp );
        } else if( Flags.link_for_dos ) {
            Fputnl( "system dos", fp );
        } else if( Flags.link_for_os2 ) {
            Fputnl( "system os2", fp );
        } else {
  #if defined( __OS2__ )
            Fputnl( "system os2", fp );
  #else
            Fputnl( "system dos", fp );
  #endif
        }
#elif defined( WCLAXP )
        Fputnl( "system ntaxp", fp );
#elif defined( WCLPPC )
  #if defined( __LINUX__ )
        Fputnl( "system linuxppc", fp );
  #else
        Fputnl( "system ntppc", fp );
  #endif
#elif defined( WCLMPS )
  #if defined( __LINUX__ )
        Fputnl( "system linuxmips", fp );
  #else
        Fputnl( "system ntmips", fp );
  #endif
#else
  #if defined( __OS2__ )
        Fputnl( "system os2v2", fp );
  #elif defined( __NT__ )
        Fputnl( "system nt", fp );
  #elif defined( __LINUX__ )
        Fputnl( "system linux", fp );
  #else
    #if defined( CAUSEWAY )
        Fputnl( "system causeway", fp );
    #elif defined( PHARLAP )
        Fputnl( "system pharlap", fp );
    #elif defined( DOS4G )
        Fputnl( "system dos4g", fp );
    #else
        Fputnl( "system dos4g", fp );
    #endif
  #endif
#endif
    }

    for( itm = Res_List; itm != NULL; itm = itm->next ) {
        AddDirectivePath( "option resource=", itm->item );
    }
    ListFree( Res_List );
    Res_List = NULL;
}

static FILE *OpenWlinkTmpFile( char *name )
{
    int     i;
    int     fh;

    for( i = 0; i < 100; i++ ) {
        sprintf( name + 1 + 6, "%2.2d" TOOL_LNK_EXT, i );
        fh = open( name + 1, O_RDWR | O_CREAT | O_EXCL | O_BINARY, PMODE_RW );
        if( fh != -1 ) {
            close( fh );
            errno = 0; /* Standard C does not require fopen failure to set errno */
            return( fopen( name + 1, "w" ) );
        }
    }
    errno = EEXIST;
    return( NULL );
}

static  int  CompLink( void )
/***************************/
{
    int         rc;
    char        *file;
    char        *path;
    list        *itm;
    char        errors_found;
    char        *tmp_env;
    tool_type   utl;
    int         i;

    tmp_env = NULL;
    if( via_environment && strlen( CC_Opts ) >= 20 ) // 20 to allow wclxxxxx=y
        tmp_env = makeTmpEnv( CC_Opts );
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

                rc = tool_exec( utl, DoQuoted( fname, Word, '"' ), CC_Opts );
                if( rc != 0 ) {
                    errors_found = 1;
                }
                strcpy( Word, RemoveExt( file ) );
            }
            AddNameObj( Word );
            if( Exe_Name == NULL ) {
                Exe_Name = MemStrDup( RemoveExt( Word ) );
            }
            file = GetName( NULL, NULL );   /* get next filename */
        }
        MemFree( path );
    }
    if( tmp_env != NULL )
        killTmpEnv( tmp_env );
    if( errors_found ) {
        rc = 1;
    } else {
        FILE    *fp;
        char    tmp_name[1 + 12 + 1] = "@" TEMPFILE;

        if( (fp = OpenWlinkTmpFile( tmp_name )) == NULL ) {
            PrintMsg( WclMsgs[UNABLE_TO_OPEN_TEMPORARY_FILE], tmp_name + 1, strerror( errno ) );
            rc = 2;
        } else {
            rc = 0;
            BuildLinkFile( fp );
            fclose( fp );
            if( ( Obj_List != NULL || Flags.do_link ) && !Flags.no_link ) {
                rc = tool_exec( TYPE_LINK, tmp_name, NULL );
                if( rc == 0 && Flags.do_cvpack ) {
                    char fname[_MAX_PATH];

                    rc = tool_exec( TYPE_PACK, DoQuoted( fname, Exe_Name, '"' ), NULL );
                }
            }
            if( Link_Name != NULL ) {
                remove( Link_Name );
                rename( tmp_name + 1, Link_Name );
            }
            remove( tmp_name + 1 );
        }
    }
    for( i = 0; i < TYPE_MAX; ++i ) {
        if( tools[i].path != NULL ) {
            MemFree( tools[i].path );
            tools[i].path = NULL;
        }
    }
    return( rc );
}

static int ProcMemInit( void )
{
    Conventions = 'r';
    Exe_Name = NULL;
    Map_Name = NULL;
    Obj_Name = NULL;
    Link_Name = NULL;
    SystemName = NULL;
    StackSize = NULL;
    Directive_List = NULL;
    Obj_List = NULL;
    Files_List = NULL;
    Libs_List = NULL;
    Res_List = NULL;
    Word = MemAlloc( MAX_CMD );
    CC_Opts = MemAlloc( MAX_CMD );
    Word[0] = '\0';
    CC_Opts[0] = '\0';
    return( 0 );
}

static int ProcMemFini( void )
{
    MemFree( Exe_Name );
    MemFree( Map_Name );
    MemFree( Obj_Name );
    MemFree( Link_Name );
    MemFree( SystemName );
    MemFree( StackSize );
    MemFree( Word );
    MemFree( CC_Opts );
    ListFree( Directive_List );
    ListFree( Obj_List );
    ListFree( Files_List );
    ListFree( Libs_List );
    ListFree( Res_List );
    return( 0 );
}

static int check_y_opt( const char *cmdl )
{
    char        switch_chars[3];

    switch_chars[0] = '-';
#ifndef __UNIX__
    switch_chars[1] = alt_switch_char;
    switch_chars[2] = '\0';
#else
    switch_chars[1] = '\0';
#endif
    while( (cmdl = strpbrk( cmdl, switch_chars )) != NULL ) {
        ++cmdl;
        if( tolower( *cmdl ) == 'y' ) {
            return( 1 );
        }
    }
    return( 0 );
}

int  main( int argc, char **argv )
/********************************/
{
    int         rc;
    const char  *wcl_env;
    const char  *p;
    char        *cmd;           /* command line parameters  */

#if !defined( __WATCOMC__ )
    _argc = argc;
    _argv = argv;
#else
    /* unused parameters */ (void)argc; (void)argv;
#endif

#ifndef __UNIX__
    alt_switch_char = _dos_switch_char();
#endif
    MemInit();
    ProcMemInit();
    cmd = MemAlloc( MAX_CMD * 2 );  /* enough for cmd line & wcl variable */

    /* add wcl environment variable to cmd             */
    /* unless /y is specified in either cmd or wcl */

    wcl_env = getenv( WCLENV );
    if( wcl_env != NULL ) {
        strcpy( cmd, wcl_env );
        strcat( cmd, " " );
        p = getcmd( cmd + strlen( cmd ) );
        if( check_y_opt( cmd ) ) {
            p = getcmd( cmd );
        }
    } else {
        p = getcmd( cmd );
    }
    SKIP_SPACES( p );
    if( *p == '\0' || p[0] == '?' && ( p[1] == '\0' || p[1] == ' ' ) || IS_OPT( p[0] ) && p[1] == '?' ) {
        Usage();
        rc = 1;
    } else {
        initialize_Flags();
        rc = Parse( cmd );
        if( rc == 0 ) {
            print_banner();
            rc = CompLink();
        }
    }
    MemFree( cmd );
    ProcMemFini();
    MemFini();
    return( ( rc != 0 ) );
}
