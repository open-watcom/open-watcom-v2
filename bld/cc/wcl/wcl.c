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


#include <sys\types.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <io.h>
#include <direct.h>
#include <process.h>
#include <malloc.h>
#include <conio.h>

#include "banner.h"

#if defined(__OS2) || defined(__NT)
#define MAX_CMD 10240
#else
#define MAX_CMD 130
#endif

#ifdef WCLAXP
  #define WCLNAME     "wclaxp"          /* Name of Compile and Link Utility*/
  #define CC          "wccaxp"          /* WATCOM C compiler               */
  #define CCXX        "wppaxp"          /* WATCOM C++ compiler             */
  #define WCLENV      "WCLAXP"          /* name of environment variable    */
  #define STACKSIZE   "8192"            /* default stack size              */
  #define _NAME_      "Watcom C/C++ Alpha AXP "
#elif defined(WCL386)
  #define WCLNAME     "wcl386"          /* Name of Compile and Link Utility*/
  #define CC          "wcc386"          /* WATCOM C compiler               */
  #define CCXX        "wpp386"          /* WATCOM C++ compiler             */
  #define WCLENV      "WCL386"          /* name of environment variable    */
  #define STACKSIZE   "4096"            /* default stack size              */
  #define _NAME_      "Watcom C/C++32 "
#else
  #define WCLNAME     "wcl"             /* Name of Compile and Link Utility*/
  #define CC          "wcc"             /* WATCOM C compiler               */
  #define CCXX        "wpp"             /* WATCOM C++ compiler             */
  #define WCLENV      "WCL"             /* name of environment variable    */
  #define STACKSIZE   "2048"            /* default stack size              */
  #define _NAME_      "Watcom C/C++16 "
#endif
#define OBJ_EXT     ".obj"
#define LINK        "wlink"             /* WATCOM linker                   */
#define TEMPFILE    "@__WCL__.LNK"      /* temporary linker directive file */
#define NULLCHAR    '\0'
#define ATTR_MASK   _A_HIDDEN + _A_SYSTEM + _A_VOLID + _A_SUBDIR
                                        /* mask for illegal file types     */
#define TRUE        1
#define FALSE       0


struct  list {
    char        *filename;
    struct list *next;
};
struct  directives {
    struct directives   *next;
    char                *directive;
};


static  char    *Cmd;               /* command line parameters            */
static  char    *Word;              /* one parameter                      */
static  char    *SystemName;        /* system to link for                 */
static  char    Files[MAX_CMD];     /* list of filenames from Cmd         */
static  char    Libs[MAX_CMD];      /* list of libraires from Cmd         */
static  char    CC_Opts[MAX_CMD];   /* list of compiler options from Cmd  */
static  char    CC_Path[_MAX_PATH]; /* path name for wcc.exe              */
static  char    PathBuffer[_MAX_PATH];/* buffer for path name of tool     */
static  FILE    *Fp;                /* file pointer for Temp_Link         */
static  char    *Link_Name;         /* name for Temp_Link if /fd specified*/
static  char    *Temp_Link;         /* temporary linker directive file    */
static  struct  list *Obj_List;     /* linked list of object filenames    */
static  struct directives *Directive_List; /* linked list of directives   */
static  char    Switch_Chars[4];    /* valid switch characters            */
static  char    Exe_Name[_MAX_PATH];/* name of executable              */
static  char    *Map_Name;          /* name of map file                   */
static  char    *Obj_Name;          /* object file name pattern           */
static  char    *StackSize;         /* size of stack                      */
static  char    DebugFlag;          /* debug info wanted                  */
static  char    Conventions;        /* 'r' for -3r or 's' for -3s         */

static  struct flags {
        unsigned math_8087    : 1;  /* 0 ==> no 8087, otherwise /7 option */
        unsigned map_wanted   : 1;  /* -fm option specified               */
        unsigned two_case     : 1;  /* two case option                    */
        unsigned tiny_model   : 1;  /* tiny memory model                  */
        unsigned be_quiet     : 1;  /* -zq option to be quiet             */
        unsigned no_link      : 1;  /* -c compile only, no link step      */
        unsigned do_link      : 1;  /* flag for link if no .obj in Cmd    */
        unsigned do_cvpack    : 1;  /* flag for link do codeview cvpack   */
        unsigned link_for_dos : 1;  /* -lr produce DOS executable         */
        unsigned link_for_os2 : 1;  /* -lp produce OS/2 executable        */
        unsigned windows      : 1;  /* -zw specified for Windows          */
        unsigned link_for_sys : 1;  /* -l<system> option given            */
        unsigned force_c      : 1;  /* -cc option                         */
        unsigned force_c_plus : 1;  /* -cc++ option                       */
} Flags;

static char *DebugOptions[] = {
        "",
        "debug dwarf\n",
        "debug dwarf\n",
        "debug watcom all\n",
        "debug codeview\n",
        "debug dwarf\n",
};

/*
 *  Static function prototypes
 */

int     Parse( void );
int     CompLink( void );
char    *SkipSpaces( char * );
void    Fputnl( char *, FILE * );
void    *MemAlloc( int );
void    MakeName( char *, char * );
void    AddName( char *, FILE * );
char    *MakePath( char * );
char    *GetName( char * );
void    Usage( void );
#if defined( __OS2 ) || defined( __NT )
  #define _dos_switch_char() '/'
#else
  extern  int     _dos_switch_char();
#endif

enum {
#undef pick
#undef E
#undef J
#define E(msg)  msg
#define J(msg)
#define pick(code,msg)  code
#include "wclmsg.h"
};

char *EnglishMsgs[] = {
#undef pick
#undef E
#undef J
#define E(msg)  msg
#define J(msg)
#define pick(code,english)      english
#include "wclmsg.h"
};

char *JapaneseMsgs[] = {
#undef pick
#undef E
#undef J
#define E(msg)
#define J(msg)  msg
#define pick(code,japanese)     japanese
#include "wclmsg.h"
};

char    **WclMsgs = EnglishMsgs;

void PrintMsg( char *fmt, ... )
{
    char        c;
    int         i;
    char        *p;
    unsigned    value;
    va_list     args;
    int         len;
    char        buf[128];

    value = value;
    va_start( args, fmt );
    len = 0;
    for(;;) {
        c = *fmt++;
        if( c == '\0' ) break;
        if( c == '%' ) {
            c = *fmt++;
            if( c == 's' ) {
                p = va_arg( args, char * );
                for(;;) {
                    c = *p++;
                    if( c == '\0' ) break;
                    buf[len++] = c;
                }
            } else if( c == 'd' ) {
                i = va_arg( args, int );
                itoa( i, &buf[len], 10 );
                while( buf[len] != '\0' ) ++len;
            }
        } else {
            buf[len++] = c;
        }
    }
    buf[len] = '\0';
    puts( buf );
}


void print_banner( void )
{
    static int done;

    if( done ) return;
    puts( banner1( _NAME_ "Compile and Link Utility",_WCL_VERSION_) );
    puts( banner2( "1988" ) );
    puts( banner3 );
    done = 1;
}

void  main()
/**********/
{
    int         rc;
    char        *wcl_env;
    char        *p;
    char        *q;

//      determine language to use and set WclMsgs

    CC_Opts[0] = '\0';

    Switch_Chars[0] = '-';
    Switch_Chars[1] = _dos_switch_char();
    Switch_Chars[2] = '\0';

    Word = MemAlloc( MAX_CMD );
    Cmd = MemAlloc( MAX_CMD*2 ); /*enough for cmd line & wcl variable*/

    /* add wcl environment variable to Cmd             */
    /* unless /y is specified in either Cmd or wcl */

    wcl_env = getenv( WCLENV );
    if( wcl_env != NULL ) {
        strcpy( Cmd, wcl_env );
        strcat( Cmd, " " );
        p = Cmd + strlen( Cmd );
        getcmd( p );
        q = Cmd;
        while( ( q = strpbrk( q, Switch_Chars ) ) != NULL ) {
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
    if( *p == NULLCHAR || strncmp( p, "? ", 2 ) == NULL ) {
        Usage();
        exit( 1 );
    }

    Temp_Link = TEMPFILE;
    if( ( Fp = fopen( &Temp_Link[ 1 ], "w" ) ) == NULL ) {
        PrintMsg( WclMsgs[ UNABLE_TO_OPEN_TEMPORARY_FILE ] );
        exit( 1 );
    }
    Map_Name = NULL;
    Obj_Name = NULL;
    Directive_List = NULL;
    rc = Parse();
    if( rc == 0 ) {
        if( ! Flags.be_quiet ) {
            print_banner();
        }
        rc = CompLink();
    }
    if( rc == 1 )  fclose( Fp );
    if( Link_Name != NULL ) {
        if( stricmp( Link_Name, &Temp_Link[ 1 ] ) != 0 ) {
            remove( Link_Name );
            rename( &Temp_Link[ 1 ], Link_Name );
        }
    } else {
        remove( &Temp_Link[ 1 ] );
    }
    exit( rc == 0 ? 0 : 1 );
}


static char *ScanFName( char *end, int len )
{
    for(;;) {   /* 15-jan-89: Allow '-' in filenames */
        if( *end == '\0' ) break;
        if( *end == ' '  ) break;
        if( *end == '\t'  ) break;                  /* 16-mar-91 */
        if( *end == Switch_Chars[1] ) break;
        Word[ len ] = *end;
        ++len;
        ++end;
    }
    Word[ len ] = NULLCHAR;
    return( end );
}

static int FileExtension( char *p, char *ext )
{
    char        *dot;

    dot = NULL;
    while( *p != '\0' ) {
        if( *p == '.' )  dot = p;
        ++p;
    }
    if( dot != NULL ) {
        if( stricmp( dot, ext ) == 0 ) {
            return( 1 );                // indicate file extension matches
        }
    }
    return( 0 );                        // indicate no match
}

static  int  Parse( void )
/************************/
{
    char        opt;
    char        *end;
    FILE        *atfp;
    char        buffer[_MAX_PATH];
    int         len;
    char        *p;
    int         wcc_option;

    Flags.map_wanted   = 0;
    Flags.two_case     = 0;
    Flags.tiny_model   = 0;
    Flags.be_quiet     = 0;
    Flags.no_link      = 0;
    Flags.do_link      = 0;
    Flags.do_cvpack    = 0;
    Flags.link_for_dos = 0;
    Flags.link_for_os2 = 0;
    Flags.windows      = 0;
    Flags.math_8087    = 1;
    DebugFlag          = 0;
    StackSize = NULL;
    Conventions = 'r';

    /* Cmd will always begin with at least one */
    /* non-space character if we get this far  */

    for(;;) {
        Cmd = SkipSpaces( Cmd );
        if( *Cmd == NULLCHAR ) break;
        opt = *Cmd;
        if( opt == '-'  ||  opt == Switch_Chars[1] ) {
            Cmd++;
        } else if( opt != '@' ) {
            opt = ' ';
        }

        end = Cmd;
        if( *Cmd == '"' ) {
            for(;;) {
                ++end;
                if( *end == '\0' ) break;
                if( *end == '"'  ) break;
            }
        } else {
            for(;;) {
                if( *end == '\0' ) break;
                if( *end == ' '  ) break;
                if( *end == '\t'  ) break;              /* 16-mar-91 */
                if( *end == Switch_Chars[1] ) break;
                if( opt == '-'  ||  opt == Switch_Chars[1] ) {
                    /* if we are processing a switch, stop at a '-' */
                    if( *end == '-' ) break;
                }
                ++end;
            }
        }
        len = end - Cmd;
        if( len != 0 ) {

            if( opt == ' ' ) {          /* if filename, add to list */
                strncpy( Word, Cmd, len );
                Word[ len ] = NULLCHAR;
                if( FileExtension( Word, ".lib" ) ) {
                    strcat( Libs, Libs[0] != '\0' ? "," : " " );
                    strcat( Libs, Word );
                } else {
                    strcat( Files, Word );
                    strcat( Files, " " );
                }
            } else {                    /* otherwise, do option */
                --len;
                strncpy( Word, Cmd + 1, len );
                Word[ len ] = NULLCHAR;
                wcc_option = 1;         /* assume its a wcc option */
                switch( tolower( *Cmd ) ) {

                case 'f':               /* files option */
                    end = ScanFName( end, len );
                    switch( tolower( Word[0] ) ) {
                    case 'd':           /* name of linker directive file */
                        Link_Name = "__WCL__.LNK";
                        if( Word[1] == '='  ||  Word[1] == '#' ) {
                            MakeName( Word, ".lnk" );    /* add extension */
                            Link_Name = strdup( Word + 2 );
                        }
                        wcc_option = 0;
                        break;
                    case 'e':           /* name of exe file */
                        if( Word[1] == '='  ||  Word[1] == '#' ) {
                            strcpy( Exe_Name, Word + 2 );
                        }
                        wcc_option = 0;
                        break;
                    case 'i':           /* name of forced include file */
                        break;
                    case 'm':           /* name of map file */
                        Flags.map_wanted = TRUE;
                        if( Word[1] == '='  ||  Word[1] == '#' ) {
                            Map_Name = strdup( Word + 2 );
                        }
                        wcc_option = 0;
                        break;
                    case 'o':           /* name of object file */
                        /* parse off argument, so we get right filename
                            in linker command file */
                        p = &Word[1];
                        if( Word[1] == '='  ||  Word[1] == '#' ) ++p;
                        Obj_Name = strdup( p );         /* 08-mar-90 */
                        break;
#if defined(WCLI86) || defined(WCL386)
                    case 'p':           /* floating-point option */
                        if( tolower( Word[1] ) == 'c' ) {
                            Flags.math_8087 = 0;
                        }
                        break;
#endif
                    default:
                        break;
                    }
                    break;
                case 'k':               /* stack size option */
                    if( Word[0] != '\0' ) {
                        StackSize = strdup( Word );
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
                        if( Word[0] == '='  ||  Word[0] == '#' ) ++p;
                        SystemName = strdup( p );
                        break;
                    }
                    wcc_option = 0;
                    break;
                case 'x':
                    if( Word[0] == '\0' ) {
                        Flags.two_case = TRUE;
                        wcc_option = 0;
                    }
                    break;
                case '@':
                    if( Word[0] != '\0' ) {
                        end = ScanFName( end, len );
                        MakeName( Word, ".lnk" );
                        if( ( atfp = fopen( Word, "r" ) ) == NULL ) {
                            PrintMsg( WclMsgs[UNABLE_TO_OPEN_DIRECTIVE_FILE], Word );
                            return( 1 );
                        }
                        while( fgets( buffer, sizeof(buffer), atfp ) != NULL ) {
                            if( strnicmp( buffer, "file ", 5 ) == 0 ) {

                                /* look for names separated by ','s */

                                p = strchr( buffer, '\n' );
                                if( p )  *p = NULLCHAR;
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
                    if( DebugFlag == 0 ){ /* not set by -h yet */
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
#if defined(WCLI86) || defined(WCL386)
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
                    wcc_option = 0;
                    if( *end == '"'  )  ++end;
                    AddDirective( len );
                    break;
                }

                /* don't add linker-specific options */
                /* to compiler command line:     */

                if( wcc_option ) {
                    len = strlen( CC_Opts );
                    CC_Opts[ len++ ] = ' ';
                    CC_Opts[ len++ ] = opt;
                    CC_Opts[ len++ ] = *Cmd;    /* keep original case */
                    CC_Opts[ len   ] = NULLCHAR;
                    strcat( CC_Opts, Word );
                }
            }
            Cmd = end;
        }
    }

    return( 0 );
}


static void AddDirective( int len )
/*********************************/
{
    struct directives *p;
    struct directives *p2;

    p = MemAlloc( sizeof(struct directives) );
    p->next = NULL;
    p->directive = MemAlloc( len + 1 );
    strcpy( p->directive, Word );
    if( Directive_List == NULL ) {
        Directive_List = p;
    } else {
        p2 = Directive_List;
        while( p2->next != NULL )  p2 = p2->next;
        p2->next = p;
    }
}

static int useCPlusPlus( char *p )
{
    return
        stricmp( p, ".cpp" ) == 0 ||
        stricmp( p, ".cxx" ) == 0 ||
        stricmp( p, ".cc" )  == 0 ||
        stricmp( p, ".hpp" ) == 0 ||
        stricmp( p, ".hxx" ) == 0 ;
}

static char *SrcName( char *name )
/********************************/
{
    char        *cc_name;
    char        *exename;
    char        *p;
    char        buffer[_MAX_PATH2];
    char        *ext;

    _splitpath2( name, buffer, NULL, NULL, NULL, &ext );
    p = &ext[0];
    if( ext[0] == '\0' ) {
        p = strchr( name, '\0' );
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
    if( stricmp( p, ".asm" ) == 0 ) {
        exename = "wasm.exe";
        cc_name = "wasm";
    } else {
        exename = CC ".exe";            // assume C compiler
        cc_name = CC;
        if( ! Flags.force_c ) {
            if( Flags.force_c_plus || useCPlusPlus( p ) ) {
                exename = CCXX ".exe";  // use C++ compiler
                cc_name = CCXX;
            }
        }
    }
    FindPath( exename, CC_Path );
    return( cc_name );
}

static  int  CompLink( void )
/***************************/
{
    int         rc;
    char        *p;
    char        *file;
    char        *path;
    char        *cc_name;
    struct directives *d_list;
    char        errors_found;

    if( Flags.be_quiet ) {
        Fputnl( "option quiet", Fp );
    }

    fputs( DebugOptions[ DebugFlag ], Fp );
#if defined(WCL386) || defined(WCLI86)
    Fputnl( "option dosseg", Fp );
#endif
    if( StackSize != NULL ) {
        fputs( "option stack=", Fp );
        Fputnl( StackSize, Fp );
    }
    if( Flags.link_for_sys ) {                  /* 10-jun-91 */
        fputs( "system ", Fp );
        Fputnl( SystemName, Fp );
    } else {
#if defined(WCLAXP)
        Fputnl( "system ntaxp", Fp );
#elif defined(WCL386)
  #if defined(__OS2)
        Fputnl( "system os2v2", Fp );           /* 04-feb-92 */
  #elif defined(__NT)
        Fputnl( "system nt", Fp );
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
            Fputnl( "system dos", Fp );
        }
#endif
    }
    for( d_list = Directive_List; d_list; d_list = d_list->next ) {
        Fputnl( d_list->directive, Fp );
    }

    errors_found = 0;                   /* 21-jan-92 */
    p = strtok( Files, " " );           /* get first filespec */
    while( p != NULL ) {
        strcpy( Word, p );
        cc_name = SrcName( Word );      /* if no extension, assume .c */

        file = GetName( Word );         /* get first matching filename */
        path = MakePath( Word );        /* isolate path portion of filespec */
        while( file != NULL ) {         /* while more filenames: */
            strcpy( Word, path );
            strcat( Word, file );
            if( ! FileExtension( Word, OBJ_EXT ) ) { // if not .obj, compile
                if( ! Flags.be_quiet ) {
                    PrintMsg( "       %s %s %s", cc_name, Word, CC_Opts );
                    fflush( stdout );
                }
                rc = spawnlp( P_WAIT, CC_Path, cc_name, Word, CC_Opts, NULL );
                if( rc != 0 ) {
                    if( rc == -1  ||  rc == 255 ) {
                        PrintMsg( WclMsgs[ UNABLE_TO_INVOKE_EXE ], CC_Path );
                    } else {
                        PrintMsg( WclMsgs[ COMPILER_RETURNED_A_BAD_STATUS ],
                                    Word );
                    }
                    errors_found = 1;           /* 21-jan-92 */
                }
                p = strrchr( file, '.' );
                if( p != NULL )  *p = NULLCHAR;
                strcpy( Word, file );
            }
            AddName( Word, Fp );
            if( Exe_Name[0] == '\0' ) {
                p = strrchr( Word, '.' );
                if( p != NULL )  *p = NULLCHAR;
                strcpy( Exe_Name, Word );
            }
            file = GetName( NULL );     /* get next filename */
        }
        p = strtok( NULL, " " );        /* get next filespec */
    }
    if( errors_found )  return( 1 );            /* 21-jan-92 */
    BuildLinkFile();

    if( ( Obj_List != NULL || Flags.do_link )  &&  Flags.no_link == FALSE ) {
        FindPath( "wlink.exe", PathBuffer );
        if( ! Flags.be_quiet ) {
            puts( "" );
        }
        fflush( stdout );
        rc = spawnlp( P_WAIT, PathBuffer, LINK, Temp_Link, NULL );
        if( rc != 0 ) {
            if( rc == -1  ||  rc == 255 ) {
                PrintMsg( WclMsgs[ UNABLE_TO_INVOKE_EXE ], PathBuffer );
            } else {
                PrintMsg( WclMsgs[ LINKER_RETURNED_A_BAD_STATUS ] );
            }
            return( 2 );        /* return 2 to show Temp_File already closed */
        }
        if( Flags.do_cvpack ){
            FindPath( "cvpack.exe", PathBuffer );
            rc = spawnlp( P_WAIT, PathBuffer, "cvpack", Exe_Name, NULL );
            if( rc != 0 ) {
                if( rc == -1  ||  rc == 255 ) {
                    PrintMsg( WclMsgs[ UNABLE_TO_INVOKE_EXE ], PathBuffer );
                } else {
                    PrintMsg( WclMsgs[ CVPACK_RETURNED_A_BAD_STATUS ] );
                }
                return( 2 );  /* return 2 to show Temp_File already closed */
            }
        }
    }
    return( 0 );
}


static void BuildLinkFile()
{
    fputs( "name ", Fp );
    Fputnl( Exe_Name, Fp );
    if( Flags.map_wanted ) {
        if( Map_Name == NULL ) {
            Fputnl( "option map", Fp );
        } else {
            fputs( "option map=", Fp );
            Fputnl( Map_Name, Fp );
        }
    }
    if( Libs[0] != '\0' ) {
        fputs( "library ", Fp );
        Fputnl( Libs, Fp );
    }
    if( Flags.two_case )  Fputnl( "option caseexact", Fp );
    fclose( Fp );       /* close Temp_Link */
}


static  char  *SkipSpaces( char *ptr )
/************************************/
{
    while( *ptr == ' ' || *ptr == '\t' )  ptr++;    /* 16-mar-91 */
    return( ptr );
}


static  void  Fputnl( char *text, FILE *fptr )
/********************************************/
{
    fputs( text, fptr );
    fputs( "\n", fptr );
}


static  void  *MemAlloc( int size )
/*********************************/
{
    void        *ptr;

    if( ( ptr = malloc( size ) ) == NULL ) {
        PrintMsg( WclMsgs[ OUT_OF_MEMORY ] );
        exit( 1 );
    }
    return( ptr );
}


static  void  MakeName( char *name, char *ext )
/*********************************************/
{
    if( strrchr( name, '.' ) <= strstr( name, "\\" ) ) {
        strcat( name, ext );
    }
}

static  void  AddName( char *name, FILE *link_fp )
/************************************************/
{
    struct list *curr_name, *last_name, *new_name;
    char path  [_MAX_PATH ];
    char buff1[_MAX_PATH2];
    char buff2[_MAX_PATH2];
    char *drive;
    char *dir;
    char *fname;
    char *ext1;
    char *ext2;
    char *extension;

    curr_name = Obj_List;
    while( curr_name != NULL ) {
        if( strcmp( name, curr_name->filename ) == 0 )  return;
        last_name = curr_name;
        curr_name = curr_name->next;
    }
    new_name = MemAlloc( sizeof( struct list ) );
    if( Obj_List == NULL ) {
        Obj_List = new_name;
    } else {
        last_name->next = new_name;
    }
    new_name->filename = strdup( name );
    new_name->next = NULL;
    fputs( "file ", link_fp );
    if( Obj_Name != NULL ) {
        /* construct full name of object file from Obj_Name information */
        _splitpath2( Obj_Name, buff1, &drive, &dir, &fname, &ext1 );
        extension = ext1;
        if( ext1[0] == '\0' )  extension = OBJ_EXT;
        if( fname[0] == '\0' || fname[0] == '*' ) {
            _splitpath2( name, buff2, NULL, NULL, &fname, &ext2 );
            if( ext2[0] != '\0' )  extension = ext2;
        }
        _makepath( path, drive, dir, fname, extension );
        name = path;
    }
    Fputnl( name, link_fp );
}


static  char  *MakePath( char *path )
/***********************************/
{
    char        *p;

    p = strrchr( path ,'\\' );
    if( p != NULL ) {
        p[ 1 ] = NULLCHAR;
    } else {
        p = strchr( path, ':' );
        if( p != NULL ) {
            p[ 1 ] = NULLCHAR;
        } else {
            *path = NULLCHAR;
        }
    }
    return( strdup( path ) );
}


static  char  *GetName( char *path )
/**********************************/
{
    static      DIR     *dirp;
    struct      dirent  *direntp;

    if( path != NULL ) {                /* if given a filespec to open,  */
        if( *path == NULLCHAR ) {       /*   but filespec is empty, then */
            closedir( dirp );           /*   close directory and return  */
            return( NULL );             /*   (for clean up after error)  */
        }
        dirp = opendir( path );         /* try to find matching filenames */
        if( dirp == NULL ) {
            PrintMsg( WclMsgs[ UNABLE_TO_OPEN ], path );
            return( NULL );
        }
    }

    while( ( direntp = readdir( dirp ) ) != NULL ) {
        if( ( direntp->d_attr & ATTR_MASK ) == 0 ) {    /* valid file? */
            return( direntp->d_name );
        }
    }
    closedir( dirp );
    return( NULL );
}


void FindPath( char *name, char *buf )
/************************************/
{
    _searchenv( name, "PATH", buf );
    if( buf[0] == '\0' ) {
        PrintMsg( WclMsgs[ UNABLE_TO_FIND ], name );
        exit( 1 );
    }
}



char *EnglishHelp[] = {
    #undef E
    #undef J
    #define E(msg)      msg,
    #define J(msg)
    #include "wclhelp.h"
 NULL };

char *JapaneseHelp[] = {
    #undef E
    #undef J
    #define E(msg)
    #define J(msg)      msg,
    #include "wclhelp.h"
 NULL };

static  void  Usage( void )
/*************************/
{
    char        **list;
    char        *p;
    int         lines_printed;
    unsigned int i, n;
    auto        char buf[82];

    print_banner();
    lines_printed = 3;
    list = EnglishHelp;
    while( *list ) {
        memset( buf, ' ', 80 );
        if( **list == '[' ) {                   /* title to be centered */
            i = strlen( *list );
            strcpy( &buf[38-i/2], *list );
            ++list;
            for( n = 0; list[n]; ++n ) {        /* count number in list */
                if( *list[n] == '[' ) break;
            }
            n = (n+1) / 2;                      /* half way through list */
            if( isatty( fileno( stdout ) ) ) {
                if( lines_printed != 0 ) {
                    if( lines_printed + n > 25 ) {
                        fputs( WclMsgs[ PRESS_ANY_KEY_TO_CONTINUE ], stdout );
                        fflush( stdout );
                        getch();
                        puts( "" );
                        lines_printed = 0;
                    }
                }
            }
            puts( buf );
            lines_printed++;
            for(;;) {
                memset( buf, ' ', 80 );
                p = *list;
                if( p == NULL ) break;
                for( i = 0; *p; )  buf[i++] = *p++;
                p = list[n];
                if( p != NULL  &&  *p != '[' ) {
                    for( i = 38; *p; )  buf[i++] = *p++;
                }
                buf[i] = '\0';
                puts( buf );
                lines_printed++;
                p = list[n];
                if( p == NULL ) break;
                if( *p == '[' ) break;
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
