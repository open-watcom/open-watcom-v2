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
* Description:  C/C++ Compile and Link utility.
*
****************************************************************************/


#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include <process.h>
#include <conio.h>
#endif
#ifdef __UNIX__
#include <dirent.h>
#else
#include <direct.h>
#endif
#include "wio.h"
#include "watcom.h"
#include "swchar.h"
#include "diskos.h"
#include "cmdlhelp.h"
#include "clcommon.h"
#include "banner.h"
#undef  _BANEXTRA
#define _BANEXTRA _BANEXSHORT

#ifdef BOOTSTRAP
#define BPRFX   "b"
#else
#define BPRFX   ""
#endif

#ifdef WCLAXP
  #define WCLNAME     BPRFX "wclaxp"          /* Name of Compile and Link Utility*/
  #define CC          BPRFX "wccaxp"          /* Open Watcom C compiler          */
  #define CPP         BPRFX "wppaxp"          /* Open Watcom C++ compiler        */
  #define AS          BPRFX "wasaxp"          /* Open Watcom assembler           */
#elif defined( WCLPPC )
  #define WCLNAME     BPRFX "wclppc"          /* Name of Compile and Link Utility*/
  #define CC          BPRFX "wccppc"          /* Open Watcom C compiler          */
  #define CPP         BPRFX "wppppc"          /* Open Watcom C++ compiler        */
  #define AS          BPRFX "wasppc"          /* Open Watcom assembler           */
#elif defined( WCLMPS )
  #define WCLNAME     BPRFX "wclmps"          /* Name of Compile and Link Utility*/
  #define CC          BPRFX "wccmps"          /* Open Watcom C compiler          */
  #define CPP         BPRFX "wppmps"          /* Open Watcom C++ compiler        */
  #define AS          BPRFX "wasmps"          /* Open Watcom assembler           */
#elif defined( WCL386 )
  #define WCLNAME     BPRFX "wcl386"          /* Name of Compile and Link Utility*/
  #define CC          BPRFX "wcc386"          /* Open Watcom C compiler          */
  #define CPP         BPRFX "wpp386"          /* Open Watcom C++ compiler        */
  #define AS          BPRFX "wasm"            /* Open Watcom assembler           */
#else
  #define WCLNAME     BPRFX "wcl"             /* Name of Compile and Link Utility*/
  #define CC          BPRFX "wcc"             /* Open Watcom C compiler          */
  #define CPP         BPRFX "wpp"             /* Open Watcom C++ compiler        */
  #define AS          BPRFX "wasm"            /* Open Watcom assembler           */
#endif
#define PACK                "cvpack"          /* Open Watcom executable packer   */
#define LINK          BPRFX "wlink"           /* Open Watcom linker              */

#ifdef WCLAXP
  #define WCLENV      "WCLAXP"          /* name of environment variable    */
  #define STACKSIZE   "8192"            /* default stack size              */
  #define _NAME_      "C/C++ Alpha AXP "
#elif defined( WCLPPC )
  #define WCLENV      "WCLPPC"          /* name of environment variable    */
  #define STACKSIZE   "8192"            /* default stack size              */
  #define _NAME_      "C/C++ PowerPC "
#elif defined( WCLMPS )
  #define WCLENV      "WCLMPS"          /* name of environment variable    */
  #define STACKSIZE   "8192"            /* default stack size              */
  #define _NAME_      "C/C++ MIPS "
#elif defined( WCL386 )
  #define WCLENV      "WCL386"          /* name of environment variable    */
  #define STACKSIZE   "8192"            /* default stack size              */
  #define _NAME_      "C/C++32 "
#else
  #define WCLENV      "WCL"             /* name of environment variable    */
  #define STACKSIZE   "4096"            /* default stack size              */
  #define _NAME_      "C/C++16 "
#endif

#define TEMPFILE      "__wcl__.lnk"     /* temporary linker directive file */

#ifdef __UNIX__
#define PATH_SEPS_STR   SYS_DIR_SEP_STR
#define fname_cmp       strcmp
#else
#define PATH_SEPS_STR   SYS_DIR_SEP_STR "/"
#define fname_cmp       stricmp
#endif

static  char    *Word;              /* one parameter                      */
static  char    *SystemName;        /* system to link for                 */
static  list    *Files_List;        /* list of filenames from Cmd         */
static  list    *Res_List;          /* list of resources from Cmd         */
static  char    CC_Opts[MAX_CMD];   /* list of compiler options from Cmd  */
static  char    PathBuffer[_MAX_PATH];/* buffer for path name of tool     */
static  char    *Link_Name;         /* Temp_Link copy if /fd specified    */
static  list    *Directive_List;    /* linked list of directives   */
static  char    *StackSize;         /* size of stack                      */
static  int     DebugFlag = 0;      /* debug info wanted                  */
static  char    Conventions;        /* 'r' for -3r or 's' for -3s         */
static  char    Switch_Chars[3];    /* valid switch characters            */
static  int     via_environment = FALSE;

/*
 *  Static function prototypes
 */
static int     Parse( char *Cmd );

const char *WclMsgs[] = {
    #define pick( code, english )   english
    #include "wclmsg.h"
    #undef pick
};

static const char *EnglishHelp[] = {
    #include "wclhelp.h"
    NULL
};


typedef enum tool_type {
    TYPE_ASM,
    TYPE_C,
    TYPE_CPP,
    TYPE_LINK,
    TYPE_PACK,
    TYPE_MAX
} tool_type;

static struct {
    char *name;
    char *exename;
    char *path;
} tools[TYPE_MAX] = {
    { AS,   AS   EXE_EXT,   NULL },
    { CC,   CC   EXE_EXT,   NULL },
    { CPP,  CPP  EXE_EXT,   NULL },
    { LINK, LINK EXE_EXT,   NULL },
    { PACK, PACK EXE_EXT,   NULL }
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
    Flags.is32bit         = 0;
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
    unsigned            argbufsize;
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
static void *makeTmpEnv( char *arg )
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


void print_banner( void )
/***********************/
{
    static int done;

    if( done )
        return;
    puts( banner1w( _NAME_ "Compile and Link Utility", _WCL_VERSION_ ) );
    puts( banner2( "1988" ) );
    puts( banner3 );
    puts( banner3a );
    done = 1;
}


static  char  *SkipSpaces( char *ptr )
/************************************/
{
    while( *ptr == ' ' || *ptr == '\t' ) {  /* 16-mar-91 */
        ptr++;
    }
    return( ptr );
}


static  void  MakeName( char *name, char *ext )
/*********************************************/
{
    /* If the last '.' is before the last path seperator character */
    if( strrchr( name, '.' ) <= strpbrk( name, PATH_SEPS_STR ) ) {
        strcat( name, ext );
    }
}


static void  Usage( void )
/************************/
{
    char const  **list;
    char const  *p;
    int         lines_printed;
    unsigned int i, n;
    auto        char buf[82];
#ifndef __UNIX__
    int const   paging = isatty( fileno( stdout ) );
    int const   height = 24; /* Number of lines assumed on screen */
#endif

    print_banner();
    lines_printed = 4;
    list = EnglishHelp;
    while( *list ) {
        memset( buf, ' ', 80 );
        if( **list == '[' ) {                   /* title to be centered */
            i = strlen( *list );
            strcpy( &buf[38 - i / 2], *list );
            ++list;
            for( n = 0; list[n]; ++n ) {        /* count number in list */
                if( *list[n] == '[' ) {
                    break;
                }
            }
            n = (n + 1) / 2;                    /* half way through list */
#ifndef __UNIX__
            if( paging && lines_printed != 0 && lines_printed >= height ) {
                fputs( WclMsgs[PRESS_ANY_KEY_TO_CONTINUE], stdout );
                fflush( stdout );
                getch();
                puts( "" );
                lines_printed = 0;
            }
#endif
            puts( buf );
            lines_printed++;
            for( ;; ) {
                memset( buf, ' ', 80 );
                p = *list;
                if( p == NULL )
                    break;
                for( i = 0; *p; )
                    buf[i++] = *p++;
                p = list[n];
                if( p != NULL  &&  *p != '[' ) {
                    for( i = 38; *p; ) {
                        buf[i++] = *p++;
                    }
                }
                buf[i] = '\0';
                puts( buf );
                lines_printed++;
#ifndef __UNIX__
                if( paging && lines_printed != 0 && lines_printed >= height ) {
                    fputs( WclMsgs[PRESS_ANY_KEY_TO_CONTINUE], stdout );
                    fflush( stdout );
                    getch();
                    puts( "" );
                    lines_printed = 0;
                }
#endif
                p = list[n];
                if( p == NULL )
                    break;
                if( *p == '[' )
                    break;
                list[n] = NULL; /* indicate already printed */
                ++list;
            }
            list = &list[n];
        } else {
            puts( *list );
            lines_printed++;
            ++list;
        }
    }
}


static char *ScanFName( char *end, int len )
/******************************************/
{
    for( ;; ) { /* 15-jan-89: Allow switch char in filenames */
        if( *end == '\0' )
            break;
        if( *end == ' '  )
            break;
        if( *end == '\t'  )
            break;
        Word[len] = *end;
        ++len;
        ++end;
    }
    Word[len] = '\0';
    return( end );
}


static int FileExtension( char *p, char *ext )
/********************************************/
{
    char        *dot;

    dot = NULL;
    while( *p != '\0' ) {
        if( *p == '.' ) {
            dot = p;
        }
        ++p;
    }
    if( dot != NULL ) {
        if( fname_cmp( dot, ext ) == 0 ) {
            return( 1 );                // indicate file extension matches
        }
    }
    return( 0 );                        // indicate no match
}


static void AddDirective( int len )
/*********************************/
{
    list    *p;

    p = MemAlloc( sizeof( list ) );
    p->next = NULL;
    p->item = MemAlloc( len + 1 );
    UnquoteFName( p->item, len + 1, Word );
    ListAppend( &Directive_List, p );
}


static int Parse( char *Cmd )
/***************************/
{
    char        opt;
    char        *end;
    FILE        *atfp;
    char        buffer[_MAX_PATH];
    char        unquoted[_MAX_PATH];
    int         len;
    char        *p;
    int         wcc_option;
    list        *new_item;

    /* Cmd will always begin with at least one */
    /* non-space character if we get this far  */

    for( ;; ) {
        Cmd = SkipSpaces( Cmd );
        if( *Cmd == '\0' )
            break;
        opt = *Cmd;
        if( opt == '-'  ||  opt == Switch_Chars[1] ) {
            Cmd++;
        } else if( opt != '@' ) {
            opt = ' ';
        }

        end = Cmd;
        if( *Cmd == '"' ) {
            end = FindNextWS( end );
        } else {
            end = FindNextWSOrOpt( end, opt, Switch_Chars );
        }
        len = end - Cmd;
        if( len != 0 ) {
            if( opt == ' ' ) {          /* if filename, add to list */
                strncpy( Word, Cmd, len );
                Word[len] = '\0';
                end = ScanFName( end, len );
                UnquoteFName( unquoted, sizeof( unquoted ), Word );
                new_item = MemAlloc( sizeof( list ) );
                new_item->next = NULL;
                new_item->item = MemStrDup( unquoted );
                if( FileExtension( Word, ".lib" ) ) {
                    ListAppend( &Libs_List, new_item );
                } else if( FileExtension( Word, ".res" ) ) {
                    ListAppend( &Res_List, new_item );
                } else {
                    ListAppend( &Files_List, new_item );
                }
            } else {                    /* otherwise, do option */
                --len;
                strncpy( Word, Cmd + 1, len );
                Word[len] = '\0';
                wcc_option = 1;         /* assume it's a wcc option */

                switch( tolower( *Cmd ) ) {
                case 'b':               /* possibly -bcl */
                    if( strnicmp( Word, "cl=", 3 ) == 0 ) {
                        strcat( CC_Opts, " -bt=" );
                        strcat( CC_Opts, Word+3 );
                        Flags.link_for_sys = TRUE;
                        MemFree( SystemName );
                        SystemName = MemStrDup( Word+3 );
                        wcc_option = 0;
                    }
                    break;

                case 'f':               /* files option */
                    p = ScanFName( end, len );
                    switch( tolower( Word[0] ) ) {
                    case 'd':           /* name of linker directive file */
                        if( Word[1] == '='  ||  Word[1] == '#' ) {
                            end = p;
                            /* remove quotes from target linker control filename */
                            UnquoteFName( unquoted, sizeof( unquoted ), Word + 2 );

                            MakeName( unquoted, ".lnk" );    /* add extension */

                            MemFree( Link_Name );
                            Link_Name = MemStrDup( unquoted );
                        } else {
                            MemFree( Link_Name );
                            Link_Name = MemStrDup( TEMPFILE );
                        }
                        wcc_option = 0;
                        break;
                    case 'e':           /* name of exe file */
                        if( Word[1] == '='  ||  Word[1] == '#' ) {
                            end = p;
                            /* remove quotes from target executable filename */
                            UnquoteFName( unquoted, sizeof( unquoted ), Word + 2 );
                            strcpy( Exe_Name, unquoted );
                        }
                        wcc_option = 0;
                        break;
                    case 'i':           /* name of forced include file */
                        end = p;
                        break;
                    case 'm':           /* name of map file */
                        Flags.map_wanted = TRUE;
                        if( Word[1] == '='  ||  Word[1] == '#' ) {
                            end = p;
                            /* remove quotes from target map filename */
                            UnquoteFName( unquoted, sizeof( unquoted ), Word + 2 );

                            MemFree( Map_Name );
                            Map_Name = MemStrDup( unquoted );
                        }
                        wcc_option = 0;
                        break;
                    case 'o':           /* name of object file */
                        end = p;
                        /* parse off argument, so we get right filename
                            in linker command file */
                        p = &Word[1];
                        if( Word[1] == '='  ||  Word[1] == '#' )
                            ++p;

                        /* remove quotes from object name */
                        UnquoteFName( unquoted, sizeof( unquoted ), p );

                        MemFree( Obj_Name );
                        Obj_Name = MemStrDup( unquoted );
                        break;
#if defined( WCLI86 ) || defined( WCL386 )
                    case 'p':           /* floating-point option */
                        end = p;
                        if( tolower( Word[1] ) == 'c' ) {
                            Flags.math_8087 = 0;
                        }
                        break;
#endif
                    default:
                        end = p;
                        break;
                    }
                    break;
                case 'k':               /* stack size option */
                    if( Word[0] != '\0' ) {
                        MemFree( StackSize );
                        StackSize = MemStrDup( Word );
                    }
                    wcc_option = 0;
                    break;
                case 'l':               /* link target option */
                    switch( (Word[1] << 8) | tolower( Word[0] ) ) {
                    case 'p':
                        Flags.link_for_dos = 0;
                        Flags.link_for_os2 = TRUE;
                        break;
                    case 'r':
                        Flags.link_for_dos = TRUE;
                        Flags.link_for_os2 = 0;
                        break;
                    default:                    /* 10-jun-91 */
                        Flags.link_for_sys = TRUE;
                        p = &Word[0];
                        if( Word[0] == '='  ||  Word[0] == '#' )
                            ++p;
                        MemFree( SystemName );
                        SystemName = MemStrDup( p );
                        break;
                    }
                    wcc_option = 0;
                    break;
                case '@':
                    if( Word[0] != '\0' ) {
                        char const * const      env = getenv( Word );

                        if( env != NULL ) {
                            if( handle_environment_variable( env ) ) {
                                return( 1 );          // Recursive call failed
                            }
                            via_environment = TRUE;
                            Cmd = end;
                            continue;
                        }

                        end = ScanFName( end, len );

                        /* remove quotes from additional linker options file */
                        UnquoteFName( unquoted, sizeof( unquoted ), Word );
                        strcpy( Word, unquoted );

                        MakeName( Word, ".lnk" );
                        errno = 0;
                        if( (atfp = fopen( Word, "r" )) == NULL ) {
                            PrintMsg( WclMsgs[UNABLE_TO_OPEN_DIRECTIVE_FILE], Word, strerror(  errno ) );
                            return( 1 );
                        }
                        while( fgets( buffer, sizeof( buffer ), atfp ) != NULL ) {
                            if( strnicmp( buffer, "file ", 5 ) == 0 ) {

                                /* look for names separated by ','s */
                                p = strchr( buffer, '\n' );
                                if( p != NULL )
                                    *p = '\0';
                                AddName( &buffer[5], Fp );
                                Flags.do_link = TRUE;
                            } else {
                                fputs( buffer, Fp );
                            }
                        }
                        fclose( atfp );
                    }
                    wcc_option = 0;
                    break;

                /* compiler options that affect the linker */
#ifdef WCL386
                case '3':
                case '4':
                case '5':                           /* 22-sep-92 */
                    Conventions = tolower( Word[0] );
                    break;
#endif
                case 'd':
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
                case 'h':
                    if( strcmp( Word, "w" ) == 0 ) {
                        DebugFlag = 3;
                    } else if( strcmp( Word, "c" ) == 0 ) { /* 02-mar-91 */
                        Flags.do_cvpack = 1;
                        DebugFlag = 4;
                    } else if( strcmp( Word, "d" ) == 0 ) {
                        DebugFlag = 5;
                    }
                    break;
                case 'i':           /* include file path */
                    end = ScanFName( end, len );
                    break;
                case 'c':           /* compile only */
                    if( stricmp( Word, "c" ) == 0 ) {
                        Flags.force_c = TRUE;
                    } else if( stricmp( Word, "c++" ) == 0 ) {
                        Flags.force_c_plus = TRUE;
                    } else {
                        Flags.no_link = TRUE;
                    }
                    /* fall through */
                case 'y':
                    wcc_option = 0;
                    break;
#if defined( WCLI86 ) || defined( WCL386 )
                case 'm':           /* memory model */
                    if( Cmd[1] == 't' || Cmd[1] == 'T' ) { /* tiny model*/
                        Word[0] = 's';              /* change to small */
                        Flags.tiny_model = TRUE;
                    }
                    break;
#endif
                case 'p':
                    Flags.no_link = TRUE;
                    break;      /* this is a preprocessor option */
                case 'q':
                    Flags.be_quiet = TRUE;
                    break;
                case 'z':                   /* 12-jan-89 */
                    switch( tolower( Cmd[1] ) ) {
                    case 's':
                        Flags.no_link = TRUE;
                        break;
                    case 'q':
                        Flags.be_quiet = TRUE;
                        break;
#ifdef WCLI86
                    case 'w':
                        Flags.windows = TRUE;
#endif
                    }
                    break;
                case '"':                           /* 17-dec-91 */
                    /* As parameter passing to linker is a special case, we need to pass
                     * whole command instead of first character removed. This allows us
                     * to parse also string literals in AddDirective.
                     */
                    wcc_option = 0;
                    strncpy( Word, Cmd, ++len );
                    Word[len] = '\0';
                    AddDirective( len );
                    break;
                }

                /* don't add linker-specific options */
                /* to compiler command line:     */

                if( wcc_option ) {
                    len = strlen( CC_Opts );
                    CC_Opts[len++] = ' ';
                    CC_Opts[len++] = opt;
                    CC_Opts[len++] = *Cmd;    /* keep original case */
                    CC_Opts[len] = '\0';
                    strcat( CC_Opts, Word );
                }
            }
            Cmd = end;
        }
    }

    return( 0 );
}


static int useCPlusPlus( char *p )
/********************************/
{
    return
        fname_cmp( p, ".cpp" ) == 0 ||
        fname_cmp( p, ".cxx" ) == 0 ||
        fname_cmp( p, ".cc" )  == 0 ||
        fname_cmp( p, ".hpp" ) == 0 ||
        fname_cmp( p, ".hxx" ) == 0 ;
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
        if( p2 == NULL ) {
            PrintMsg( "\t%s %s\n", tools[utl].name, p1 );
        } else {
            PrintMsg( "\t%s %s %s\n", tools[utl].name, p1, p2 );
        }
    }
    fflush( NULL );
    if( p2 == NULL ) {
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
    char        buffer[_MAX_PATH2];
    char        *ext;
    tool_type   utl;

    _splitpath2( name, buffer, NULL, NULL, NULL, &ext );
    p = &ext[0];
    if( ext[0] == '\0' ) {
        p = name + strlen( name );
        strcpy( p, ".cxx" );
        if( access( name, F_OK ) != 0 ) {
            strcpy( p, ".cpp" );
            if( access( name, F_OK ) != 0 ) {
                strcpy( p, ".cc" );
                if( access( name, F_OK ) != 0 ) {
                    strcpy( p, ".asm" );
                    if( access( name, F_OK ) != 0 ) {
                        strcpy( p, ".c" );
                    }
                }
            }
        }
    }
    if( fname_cmp( p, ".asm" ) == 0 ) {
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
    void        *tmp_env;
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
    if( Flags.link_for_sys ) {                  /* 10-jun-91 */
        fputs( "system ", Fp );
        Fputnl( SystemName, Fp );
    } else {
#if defined( WCLAXP )
        Fputnl( "system ntaxp", Fp );
#elif defined( WCLPPC )
  #if defined( __LINUX__ )
        Fputnl( "system linuxppc", Fp );
  #else
        Fputnl( "system ntppc", Fp );
  #endif
#elif defined( WCLMPS )
        Fputnl( "system linuxmips", Fp );
#elif defined( WCL386 )
  #if defined( __OS2__ )
        Fputnl( "system os2v2", Fp );           /* 04-feb-92 */
  #elif defined( __NT__ )
        Fputnl( "system nt", Fp );
  #elif defined( __LINUX__ )
        Fputnl( "system linux", Fp );
  #else
        Fputnl( "system dos4g", Fp );
  #endif
#else
        if( Flags.windows ) {                   /* 15-mar-90 */
            Fputnl( "system windows", Fp );
        } else if( Flags.tiny_model ) {
            Fputnl( "system com", Fp );
        } else if( Flags.link_for_dos ) {
            Fputnl( "system dos", Fp );
        } else if( Flags.link_for_os2 ) {
            Fputnl( "system os2", Fp );
        } else {
#if defined( __OS2__ )
            Fputnl( "system os2", Fp );
#else
            Fputnl( "system dos", Fp );
#endif
        }
#endif
    }

    /* pass given resources to linker */
    for( itm = Res_List; itm != NULL; itm = itm->next ) {
        fputs( "option resource=", Fp );
        FputnlQuoted( itm->item, Fp );
    }
    /* pass given directives to linker */
    for( itm = Directive_List; itm != NULL; itm = itm->next ) {
        Fputnl( itm->item, Fp );
    }

    tmp_env = NULL;
    if( via_environment && strlen( CC_Opts ) >= 20 ) // 20 to allow wclxxxxx=y
        tmp_env = makeTmpEnv( CC_Opts );
    errors_found = 0;
    for( itm = Files_List; itm != NULL; itm = itm->next ) {
        strcpy( Word, itm->item );
        utl = SrcName( Word );          /* if no extension, assume .c */
        file = GetName( Word );         /* get first matching filename */
        path = MakePath( Word );        /* isolate path portion of filespec */
        while( file != NULL ) {         /* while more filenames: */
            strcpy( Word, path );
            strcat( Word, file );
            if( !FileExtension( Word, OBJ_EXT ) &&  /* if not .obj or .o, compile */
                !FileExtension( Word, OBJ_EXT_SECONDARY ) ) {
                rc = tool_exec( utl, Word, CC_Opts );
                if( rc != 0 ) {
                    errors_found = 1;
                }
                p = strrchr( file, '.' );
                if( p != NULL )
                    *p = '\0';
                strcpy( Word, file );
            }
            AddName( Word, Fp );
            if( Exe_Name[0] == '\0' ) {
                p = strrchr( Word, '.' );
                if( p != NULL )
                    *p = '\0';
                strcpy( Exe_Name, Word );
            }
#ifdef __UNIX__
            MemFree( file );
#endif
            file = GetName( NULL );     /* get next filename */
        }
        MemFree( path );
    }
    if( tmp_env != NULL )
        killTmpEnv( tmp_env );
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
    *Exe_Name = '\0';
    Conventions = 'r';
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
    return( 0 );
}

static int ProcMemFini( void )
{
    MemFree( Map_Name );
    MemFree( Obj_Name );
    MemFree( Link_Name );
    MemFree( SystemName );
    MemFree( StackSize );
    ListFree( Directive_List );
    ListFree( Obj_List );
    ListFree( Files_List );
    ListFree( Libs_List );
    ListFree( Res_List );
    return( 0 );
}

int  main( int argc, char **argv )
/********************************/
{
    int         rc;
    char        *wcl_env;
    char        *p;
    char        *q;
    char        *Cmd;               /* command line parameters            */

#ifndef __WATCOMC__
    _argc = argc;
    _argv = argv;
#endif

    CC_Opts[0] = '\0';

    Switch_Chars[0] = '-';
    Switch_Chars[1] = _dos_switch_char();
    Switch_Chars[2] = '\0';

    MemInit();
    ProcMemInit();
    Word = MemAlloc( MAX_CMD );
    Cmd = MemAlloc( MAX_CMD * 2 );  /* enough for cmd line & wcl variable */

    /* add wcl environment variable to Cmd             */
    /* unless /y is specified in either Cmd or wcl */

    wcl_env = getenv( WCLENV );
    if( wcl_env != NULL ) {
        strcpy( Cmd, wcl_env );
        strcat( Cmd, " " );
        p = Cmd + strlen( Cmd );
        getcmd( p );
        q = Cmd;
        while( (q = strpbrk( q, Switch_Chars )) != NULL ) {
            if( tolower( *(++q) ) == 'y' ) {
                getcmd( Cmd );
                p = Cmd;
                break;
            }
        }
    } else {
        getcmd( Cmd );
        p = Cmd;
    }
    p = SkipSpaces( p );
    if( *p == '\0' || p[0] == '?' && ( p[1] == '\0' || p[1] == ' ' )
        || p[0] == '-' && p[1] == '?' ) {
        Usage();
        rc = 1;
    } else {
        errno = 0; /* Standard C does not require fopen failure to set errno */
        if( (Fp = fopen( TEMPFILE, "w" )) == NULL ) {
            /* Message before banner decision as '@' option uses Fp in Parse() */
            PrintMsg( WclMsgs[UNABLE_TO_OPEN_TEMPORARY_FILE], TEMPFILE,
                strerror( errno ) );
            rc = 1;
        } else {
            initialize_Flags();
            rc = Parse( Cmd );
            if( rc == 0 ) {
                if( !Flags.be_quiet ) {
                    print_banner();
                }
                rc = CompLink();
            }
            if( rc == 1 ) {
                fclose( Fp );
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
    ProcMemFini();
    MemFree( Cmd );
    MemFree( Word );
    MemFini();
    return( rc == 0 ? 0 : 1 );
}
