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
* Description:  Compile and link utility for Open Watcom FORTRAN.
*
****************************************************************************/


#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#ifdef __UNIX__
  #include <dirent.h>
#else
  #include <direct.h>
#endif
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include <process.h>
#endif
#include <malloc.h>
#if defined( __WATCOMC__ ) || defined( __UNIX__ )
#include <fnmatch.h>
#endif
#include "wio.h"
#include "watcom.h"
#include "ftnstd.h"
#include "ftextfun.h"
#include "errcod.h"
#include "optflags.h"
#include "cioconst.h"
#include "banner.h"
#include "errrtns.h"
#include "swchar.h"
#include "clibext.h"
#include "pathgrp.h"

#if _CPU == 8086
    #define _TARGET_    "x86 16-bit"
    #define WFC         "wfc"           // copmiler name
    #define WFLENV      "WFL"           // "WFL" environment variable
#elif _CPU == 386
    #define _TARGET_    "x86 32-bit"
    #define WFC         "wfc386"        // compiler name
    #define WFLENV      "WFL386"        // "WFL" environment variable
#elif _CPU == _AXP
    #define _TARGET_    "Alpha AXP"
    #define WFC         "wfcaxp"        // copmiler name
    #define WFLENV      "WFLAXP"        // "WFL" environment variable
#elif _CPU == _PPC
    #define _TARGET_    "PowerPC"
    #define WFC         "wfcppc"        // copmiler name
    #define WFLENV      "WFLPPC"        // "WFL" environment variable
#else
    #error Unknown Target CPU
#endif

#define LINK            "wlink"         // linker name
#define PACK            "cvpack"        // packer name
#define TEMPFILE        "__wfl__.lnk"   // temporary linker directive file

#if defined( __UNIX__ )
  #define OBJ_EXT       ".o"          // object file extension
  #define EXE_EXT       ""            // executable file extension
#else
  #define OBJ_EXT       ".obj"        // object file extension
  #define EXE_EXT       ".exe"        // executable file extension
#endif

#ifdef __UNIX__
  #define ISVALIDENTRY(x)   (1)
  #define FNM_OPTIONS       (FNM_PATHNAME | FNM_NOESCAPE)
#else
  // mask for illegal file types
  #define ATTR_MASK         (_A_HIDDEN + _A_SYSTEM + _A_VOLID + _A_SUBDIR)
  #define ISVALIDENTRY(x)   ((x->d_attr & ATTR_MASK) == 0 )
  #define FNM_OPTIONS       (FNM_PATHNAME | FNM_NOESCAPE | FNM_IGNORECASE)
#endif

#define TRUE            1
#define FALSE           0

#if defined( __OS2__ ) || defined( __NT__ ) || defined( __UNIX__ )
#define MAX_CMD 10240
#else
#define MAX_CMD 130
#endif

#define MAX_OPTIONS     64

typedef struct list {
    char        *filename;
    struct list *next;
} list;

static char *DebugOptions[] = {
    "",
    "debug dwarf\n",
    "debug dwarf\n",
    "debug watcom all\n",
    "debug codeview\n",
    "debug dwarf\n"
};

static  struct flags {
    unsigned quiet        : 1;  // compile quietly
    unsigned no_link      : 1;  // compile only, no link step
    unsigned link_for_sys : 1;  // system specified
#if _CPU == 8086
    unsigned windows      : 1;  // Windows application
    unsigned link_for_dos : 1;  // produce DOS executable
    unsigned link_for_os2 : 1;  // produce OS/2 executable
#else
    unsigned default_win  : 1;  // OS/2 default windowed application
#endif
    unsigned do_cvpack    : 1;  // do codeview cvpack
} Flags;

typedef enum tool_type {
    TYPE_FOR,
    TYPE_LINK,
    TYPE_PACK,
    TYPE_MAX
} tool_type;

static struct {
    char *name;
    char *exename;
    char *path;
} tools[ TYPE_MAX ] = {
    { WFC,      WFC EXE_EXT,        NULL },
    { LINK,     LINK EXE_EXT,       NULL },
    { PACK,     PACK EXE_EXT,       NULL }
};

static  char    *Word;                  // one parameter
static  char    *CmpOpts[MAX_CMD];      // list of compiler options from Cmd
static  char    PathBuffer[_MAX_PATH];  // path for compiler or linker executable file
static  FILE    *Fp;                    // file pointer for TempFile
static  char    *LinkName;              // name for TempFile if /fd specified
static  list    *ObjList;               // linked list of object filenames
static  list    *FileList;              // list of filenames from Cmd
static  list    *LibList;               // list of libraries from Cmd
static  char    SwitchChars[3];         // valid switch characters
static  char    ExeName[_MAX_PATH];     // name of executable
static  char    *ObjName;               // object file name pattern
static  char    *SystemName;            // system name
static  int     DebugFlag;              // debugging flag

/* forward declarations */
static  void    Usage( void );
static  int     Parse( int, char ** );
static  void    FindPath( char *name, char *buf );
static  int     CompLink( void );
static  void    MakeName( char *name, char *ext );
static  void    Fputnl( char *text, FILE *fptr );
static  int     IsOption( char *cmd, size_t cmd_len, char *opt );
static  void    AddName( char *name, FILE *link_fp );


static  void    wfl_exit( int rc ) {
//==================================

    __ErrorFini();
    exit( rc );
}


static  void    PrintMsg( uint msg, ... ) {
//=========================================

    va_list     args;
    char        buff[LIST_BUFF_SIZE+1];

    va_start( args, msg );
    __BldErrMsg( msg, buff, args );
    va_end( args );
    puts( &buff[1] ); // skip leading blank
}


static  void    PrtBanner( void ) {
//===========================

#if defined( _BETAVER )
    puts( banner1w1( "F77 " _TARGET_ " Compile and Link Utility" ) );
    puts( banner1w2( _WFL_VERSION_ ) );
#else
    puts( banner1w( "F77 " _TARGET_ " Compile and Link Utility", _WFL_VERSION_ ) );
#endif
    puts( banner2 );
    puts( banner2a( "1990" ) );
    puts( banner3 );
    puts( banner3a );
}


static  char    *SkipSpaces( char *ptr ) {
//========================================

    while( *ptr == ' ' ) ptr++;
    return( ptr );
}


static  void    *MemAlloc( size_t size ) {
//=====================================

    void            *ptr;

    ptr = malloc( size );
    if( ptr == NULL ) {
        PrintMsg( CL_OUT_OF_MEMORY );
        wfl_exit( 1 );
    }
    return( ptr );
}

static void     AddFile( list **l, char *fname )
{
    list *p;

    p = MemAlloc( sizeof( list ) );
    p->filename = strdup( fname );
    p->next = *l;
    *l = p;
}

void    main( int argc, char *argv[] ) {
//======================================

    int         rc;
    char        *wfl_env;
    char        *p;
    char        *q;
    char        *cmd;

    argc = argc;

    __InitResource();
    __ErrorInit( argv[0] );

    CmpOpts[0] = NULL;

    SwitchChars[0] = '-';
    SwitchChars[1] = _dos_switch_char();
    SwitchChars[2] = '\0';

    Word = MemAlloc( MAX_CMD );
    cmd = MemAlloc( 2*MAX_CMD ); // for "WFL" environment variable and command line

    // add "WFL" environment variable to "cmd" unless "/y" is specified
    // in "cmd" or the "WFL" environment string

    wfl_env = getenv( WFLENV );
    if( wfl_env != NULL ) {
        strcpy( cmd, wfl_env );
        strcat( cmd, " " );
        p = cmd + strlen( cmd );
        getcmd( p );
        for( q = cmd; (q = strpbrk( q, SwitchChars )) != NULL; ) {
            if( tolower( *(++q) ) == 'y' ) {
                getcmd( cmd );
                p = cmd;
                break;
            }
        }
    } else {
        getcmd( cmd );
        p = cmd;
    }
    p = SkipSpaces( p );
    if( ( *p == '\0' ) || ( strncmp( p, "? ", 2 ) == 0 ) ) {
        Usage();
        rc = 1;
    } else {
        Fp = fopen( TEMPFILE, "w" );
        if( Fp == NULL ) {
            PrintMsg( CL_ERROR_OPENING_TMP_FILE );
            rc = 1;
        } else {
            ObjName = NULL;
            rc = Parse( argc, argv );
            if( rc == 0 ) {
                if( !Flags.quiet ) {
                    PrtBanner();
                }
                rc = CompLink();
            }
            if( rc == 1 )
                fclose( Fp );
            if( LinkName != NULL ) {
                if( stricmp( LinkName, TEMPFILE ) != 0 ) {
                    remove( LinkName );
                    rename( TEMPFILE, LinkName );
                }
            } else {
                remove( TEMPFILE );
            }
        }
    }
    free( Word );
    free( cmd );
    wfl_exit( rc == 0 ? 0 : 1 );
}

static  int     Parse( int argc, char **argv ) {
//==================================

    char        opt;
    //char        *end;
    char        *cmd;
    size_t      len;
    int         cmp_option;
    int         opt_index;
    int         cmp_opt_index;

    Flags.no_link = 0;
    Flags.link_for_sys = 0;
    Flags.quiet        = 0;
#if _CPU == 8086
    Flags.windows      = 0;
    Flags.link_for_dos = 0;
    Flags.link_for_os2 = 0;
#else
    Flags.default_win  = 0;
#endif
    Flags.do_cvpack    = 0;

    DebugFlag = 0;

    // Skip the first entry - it's the current program's name
    opt_index = 1;
    cmp_opt_index = 0;
    
    while( opt_index < argc ) {
        cmd = argv[opt_index];
        opt = *cmd;
        
        if( ( opt == '-' ) || ( opt == SwitchChars[1] ) ) {
            cmd++;
        } else {
            opt = ' ';
        }

        len = strlen(cmd);
        if( len != 0 ) {
            if( opt == ' ' ) {  // if filename, add to list
                strncpy( Word, cmd, len );
                Word[len] = '\0';
                strlwr( Word );
                if( strstr( Word, ".lib" ) != NULL ) {
                    AddFile( &LibList, Word );
                } else {
                    AddFile( &FileList, Word );
                }
            } else {            // otherwise, do option
                --len;
                strncpy( Word, cmd + 1, len );
                Word[len] = '\0';
                cmp_option = 1; // assume its a compiler option
                switch( tolower( *cmd ) ) {
                case 'f':       // files option
                    switch( tolower( Word[0] ) ) {
                    case 'd':   // name of linker directive file
                        if( Word[1] == '\0' ) {
                            LinkName = TEMPFILE;
                            cmp_option = 0;
                        } else if( (Word[1] == '=') || (Word[1] == '#') ) {
                            MakeName( Word, ".lnk" );    // add extension
                            LinkName = strdup( Word + 2 );
                            cmp_option = 0;
                        }
                        break;
                    case 'e':   // name of exe file
                        if( ( Word[1] == '=' ) || ( Word[1] == '#' ) ) {
                            fputs( "name ", Fp );
                            Fputnl( Word + 2, Fp );
                            strcpy( ExeName, Word + 2 );
                            cmp_option = 0;
                        }
                        break;
                    case 'm':   // name of map file
                        if( Word[1] == '\0' ) {
                            fputs( "option map\n", Fp );
                            cmp_option = 0;
                        } else if( (Word[1] == '=') || (Word[1] == '#') ) {
                            fputs( "option map=", Fp );
                            Fputnl( Word + 2, Fp );
                            cmp_option = 0;
                        }
                        break;
                    case 'i':
                        if( ( Word[1] == '=' ) || ( Word[1] == '#' ) ) {
                            fputs( "@", Fp );
                            Fputnl( Word + 2, Fp );
                            cmp_option = 0;
                        }
                        break;
                    case 'o':   // name of object file
                        // parse off argument, so we get right filename
                        // in linker command file
                        if( ( Word[1] == '=' ) || ( Word[1] == '#' ) ) {
                            ObjName = strdup( &Word[2] );
                        }
                        break;
                    default:
                        break;
                    }
                    break;
                case 'k':       // stack size option
                    if( ( Word[0] == '=' ) || ( Word[0] == '#' ) ) {
                        fputs( "option stack=", Fp );
                        Fputnl( Word + 1, Fp );
                        cmp_option = 0;
                    }
                    break;
                case 'c':       // compile only
                    if( Word[0] == '\0' ) {
                        Flags.no_link = 1;
                        cmp_option = 0;
                    }
                    break;
                case 'y':
                    if( Word[0] == '\0' ) {
                        cmp_option = 0;
                    }
                    break;
                case 'p':
                    // ignore the /p option - we now only
                    // have a protect-mode compiler
                    if( Word[0] == '\0' ) {
                        cmp_option = 0;
                    }
                    break;
                case 'l':
                    if( ( Word[0] == '=' ) || ( Word[0] == '#' ) ) {
                        Flags.link_for_sys = 1;
                        SystemName = strdup( &Word[1] );
                        cmp_option = 0;
#if _CPU == 8086
                    } else if( stricmp( Word, "r" ) == 0 ) {
                        Flags.link_for_dos = 1;
                        Flags.link_for_os2 = 0;
                        cmp_option = 0;
                    } else if( stricmp( Word, "p" ) == 0 ) {
                        Flags.link_for_os2 = 1;
                        Flags.link_for_dos = 0;
                        cmp_option = 0;
#endif
                    }
                    break;
                case '"':
                    Fputnl( &Word[0], Fp );
                    cmp_option = 0;
                    break;

                // compiler options that affect the linker

#if _CPU != 8086
                case 'b':
                    if( stricmp( Word, "w" ) ) {
                        Flags.default_win = 1;
                    }
                    break;
#endif

                case 'q':
                    if( IsOption( cmd, len + sizeof( char ), "Quiet" ) ) {
                        Flags.quiet = 1;
                    }
                    break;
                case 'd':
                    if( DebugFlag == 0 ) { // not set by -h yet
                        if( strcmp( Word, "1" ) == 0 ) {
                            DebugFlag = 1;
                        } else if( strcmp( Word, "2" ) == 0 ) {
                            DebugFlag = 2;
                        }
                    }
                    break;
                case 'h':
                    if( strcmp( Word, "w" ) == 0 ) {
                        DebugFlag = 3;
                    } else if( strcmp( Word, "c" ) == 0 ) {
                        Flags.do_cvpack = 1;
                        DebugFlag = 4;
                    } else if( strcmp( Word, "d" ) == 0 ) {
                        DebugFlag = 5;
                    }
                    break;
                case 's':
                    if( IsOption( cmd, len + sizeof( char ), "SYntax" ) ) {
                        Flags.no_link = 1;
                    }
                    break;
#if _CPU == 8086
                case 'w':
                    if( IsOption( cmd, len + sizeof( char ), "WIndows" ) ) {
                        Flags.windows = 1;
                    }
                    break;
#endif
                default:
                    break;
                }

                // don't add linker-specific options to compiler command line
                if( cmp_option != 0 ) {
                    CmpOpts[cmp_opt_index] = (char *)MemAlloc((3+strlen(Word))*sizeof(char));
                    CmpOpts[cmp_opt_index][0] = opt;
                    CmpOpts[cmp_opt_index][1] = *cmd;
                    CmpOpts[cmp_opt_index][2] = '\0';
                    strcat( CmpOpts[cmp_opt_index], Word );
                    CmpOpts[++cmp_opt_index] = NULL;
                }
            }
        }
        opt_index++;
    }
    return( 0 );
}


static int     IsOption( char *cmd, size_t cmd_len, char *opt ) {
//============================================================

    size_t      len;

    len = 0;
    for(;;) {
        if( len == cmd_len )
            break;
        if( toupper( *cmd ) != toupper( *opt ) )
            return( 0 );
        ++cmd;
        ++opt;
        ++len;
    }
    if( *opt == '\0' )
        return( 1 );
    if( isupper( *opt ) )
        return( 0 );
    return( 1 );
}


/*
 * THIS FUNCTION IS NOT RE-ENTRANT!
 *
 * It returns a pointer to a character string, after doing wildcard
 * substitutions. It returns NULL when there are no more substitutions
 * possible.
 *
 * DoWildCard behaves similarly to strtok.  You first pass it a pointer
 * to a substitution string. It checks if the string contains wildcards,
 * and if not it simply returns this string. If the string contains
 * wildcards, it attempts an opendir with the string path.
 *
 * If you pass DoWildCard a NULL pointer, it reads the next normal file
 * from the directory, and returns the filename.
 *
 * If there are no more files in the directory, or no directory is open,
 * DoWildCard returns NULL.
 *
 */

static DIR  *parent = NULL;  /* we need this across invocations */
static char *path = NULL;
static char *pattern = NULL;

void DoWildCardClose( void )
/*********************************/
{
    if( path != NULL ) {
        free( path );
        path = NULL;
    }
    if( pattern != NULL ) {
        free( pattern );
        pattern = NULL;
    }
    if( parent != NULL ) {
        closedir( parent );
        parent = NULL;
    }
}

const char *DoWildCard( const char *base )
/***********************************************/
{
    PGROUP          pg;
    struct dirent   *entry;

    if( base != NULL ) {
        /* clean up from previous invocation */
        DoWildCardClose();
        if( strpbrk( base, "*?" ) == NULL ) {
            return( base );
        }
        // create directory name and pattern
        path = MemAlloc( _MAX_PATH );
        pattern = MemAlloc( _MAX_PATH );
        strcpy( path, base );
        _splitpath2( path, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
        _makepath( path, pg.drive, pg.dir, ".", NULL );
        // create file name pattern
        _makepath( pattern, NULL, NULL, pg.fname, pg.ext );
        parent = opendir( path );
        if( parent == NULL ) {
            DoWildCardClose();
            return( NULL );
        }
    }
    if( parent == NULL ) {
        return( NULL );
    }
    while( (entry = readdir( parent )) != NULL ) {
        if( ISVALIDENTRY( entry ) ) {
            if( fnmatch( pattern, entry->d_name, FNM_OPTIONS ) == 0 ) {
                break;
            }
        }
    }
    if( entry == NULL ) {
        DoWildCardClose();
        return( NULL );
    }
    _splitpath2( path, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    _makepath( path, pg.drive, pg.dir, entry->d_name, NULL );
    return( path );
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

static int tool_exec( tool_type utl, char *target, char **options )
/*******************************************************/
{
    int     rc;
    int     pass_argc;
    char    *pass_argv[MAX_OPTIONS+3];
    
    FindToolPath( utl );
    
    pass_argv[0] = tools[utl].name;
    pass_argc = 1; 
    
    while(options != NULL && options[pass_argc-1] != NULL && pass_argc < MAX_OPTIONS) {
        pass_argv[pass_argc] = options[pass_argc-1];
        pass_argc++;
    } 
    
    pass_argv[pass_argc++] = target;
    pass_argv[pass_argc] = NULL;
    
    if( !Flags.quiet ) {
        fputs( "\t", stdout );
        for( pass_argc=0; pass_argv[pass_argc] != NULL; pass_argc++ ) {
            fputs( pass_argv[pass_argc], stdout );
            fputs( " ", stdout );
        }
        fputs( "\n", stdout );
    }
    fflush( NULL );

    rc = (int)spawnvp( P_WAIT, tools[utl].path, (char const *const *)pass_argv );

    if( rc != 0 ) {
        if( (rc == -1) || (rc == 255) ) {
            if( utl == TYPE_LINK ) {
                PrintMsg( CL_UNABLE_TO_INVOKE_LINKER );
            } else if( utl == TYPE_PACK ) {
                PrintMsg( CL_UNABLE_TO_INVOKE_CVPACK );
            } else {
                PrintMsg( CL_UNABLE_TO_INVOKE_COMPILER );
            }
        } else {
            if( utl == TYPE_LINK ) {
                PrintMsg( CL_BAD_LINK );
            } else if( utl == TYPE_PACK ) {
                PrintMsg( CL_BAD_LINK );
            } else {
                PrintMsg( CL_BAD_COMPILE, target );
            }
        }
    }
    return( rc );
}

static  int     CompLink( void ) {
//================================

    int         rc;
    const char  *file;
    int         comp_err;
    PGROUP      pg;
    int         i;
    list        *currobj;
    list        *nextobj;
    
    if( Flags.quiet ) {
        Fputnl( "option quiet", Fp );
    }
    fputs( DebugOptions[ DebugFlag ], Fp );
    if( Flags.link_for_sys ) {
        fputs( "system ", Fp );
        Fputnl( SystemName, Fp );
    } else {
#if defined( __QNX__ )
        Fputnl( "system qnx", Fp );
#elif defined( __LINUX__ )
        Fputnl( "system linux", Fp );
#elif _CPU == 386
    #if defined( __OS2__ )
        Fputnl( "system os2v2", Fp );
    #elif defined( __NT__ )
        Fputnl( "system nt", Fp );
    #else
        Fputnl( "system dos4g", Fp );
    #endif
#elif _CPU == 8086
        if( Flags.windows ) {
            Fputnl( "system windows", Fp );
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
#elif _CPU == _AXP
        Fputnl( "system ntaxp", Fp );
#else
    #error Unknown System
#endif

    }

    comp_err = FALSE;
    ObjList = NULL;
    for( currobj = FileList; currobj != NULL; currobj = nextobj ) {
        strcpy( Word, currobj->filename );
        MakeName( Word, ".for" );   // if no extension, assume ".for"
        file = DoWildCard( Word );
        while( file != NULL ) {     // while more filenames:
            strcpy( Word, file );
#ifndef __UNIX__
            strlwr( Word );
#endif
            _splitpath2( Word, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
            if( strcmp( pg.ext, OBJ_EXT ) != 0 ) {  // if not object, compile
                rc = tool_exec( TYPE_FOR, Word, CmpOpts );
                if( rc != 0 ) {
                    comp_err = TRUE;
                    if( ( rc == -1 ) || ( rc == 255 ) ) {
                        rc = 1;
                        break;
                    }
                }
            }
            _makepath( Word, NULL, NULL, pg.fname, NULL );
            if( ExeName[0] == '\0' ) {
                fputs( "name '", Fp );
                fputs( Word, Fp );
                Fputnl( "'", Fp );
                strcpy( ExeName, Word );
            }
            _makepath( Word, NULL, NULL, pg.fname, OBJ_EXT );
            AddName( Word, Fp );        // add obj filename

            file = DoWildCard( NULL );  // get next filename
        }
        DoWildCardClose();
        nextobj = currobj->next;
        free( currobj->filename );
        free( currobj );
    }
    for( currobj = LibList; currobj != NULL; currobj = nextobj ) {
        fputs( "library ", Fp );
        Fputnl( currobj->filename, Fp );
        nextobj = currobj->next;
        free( currobj->filename );
        free( currobj );
    }
    fclose( Fp );   // close TempFile

    if( comp_err ) {
        rc = 1;
    } else {
        rc = 0;
        if( ( ObjList != NULL ) && !Flags.no_link ) {
            rc = tool_exec( TYPE_LINK, "@" TEMPFILE, NULL );
            if( rc == 0 && Flags.do_cvpack ) {
                rc = tool_exec( TYPE_PACK, ExeName, NULL );
            }
            if( rc != 0 ) {
                rc = 2;    // return 2 to show Temp_File already closed
            }
        }
    }
    for( currobj = ObjList; currobj != NULL; currobj = nextobj ) {
        nextobj = currobj->next;
        free( currobj->filename );
        free( currobj );
    }
    for( i = 0; i < TYPE_MAX; ++i ) {
        if( tools[i].path != NULL ) {
            free( tools[i].path );
            tools[i].path = NULL;
        }
    }
    return( rc );
}


static  void    Fputnl( char *text, FILE *fptr ) {
//================================================

    fputs( text, fptr );
    fputs( "\n", fptr );
}


static  void    MakeName( char *name, char *ext ) {
//=================================================

    PGROUP  pg;
    
    _splitpath2( name, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    if( pg.ext[0] == '\0' )
        pg.ext = ext;
    _makepath( name, pg.drive, pg.dir, pg.fname, pg.ext );
}


static  void    AddName( char *name, FILE *link_fp ) {
//====================================================

    list        *curr_name;
    list        *last_name;
    list        *new_name;
    char        path[_MAX_PATH];
    PGROUP      pg1;
    PGROUP      pg2;

    last_name = NULL;
    for( curr_name = ObjList; curr_name != NULL; curr_name = curr_name->next ) {
        if( stricmp( name, curr_name->filename ) == 0 )
            return;
        last_name = curr_name;
    }
    new_name = MemAlloc( sizeof( struct list ) );
    if( ObjList == NULL ) {
        ObjList = new_name;
    } else {
        last_name->next = new_name;
    }
    new_name->filename = strdup( name );
    new_name->next = NULL;
    fputs( "file '", link_fp );
    if( ObjName != NULL ) {
        // construct full name of object file from ObjName information
        _splitpath2( ObjName, pg1.buffer, &pg1.drive, &pg1.dir, &pg1.fname, &pg1.ext );
        if( pg1.ext[0] == '\0' )
            pg1.ext = OBJ_EXT;
        if( ( pg1.fname[0] == '\0' ) ||
            ( ( pg1.fname[0] == '*' ) && ( pg1.fname[1] == '\0' ) ) ) {
            _splitpath2( name, pg2.buffer, NULL, NULL, &pg1.fname, &pg2.ext );
            if( pg2.ext[0] != '\0' ) {
                pg1.ext = pg2.ext;
            }
        }
        _makepath( path, pg1.drive, pg1.dir, pg1.fname, pg1.ext );
        name = path;
    }
    fputs( name, link_fp );
    Fputnl( "'", link_fp );
}


static  void    FindPath( char *name, char *buf ) {
//=================================================

    _searchenv( name, "PATH", buf );
    if( buf[0] == '\0' ) {
        PrintMsg( CL_UNABLE_TO_FIND, name );
        wfl_exit( 1 );
    }
}


void    TOutNL( char *msg ) {
//===========================

    puts( msg );
}


void    TOut( char *msg ) {
//===========================

    fputs( msg, stdout );
}


#define opt( name, bit, flags, actionstr, actionneg, desc ) name, desc, flags

#include "cpopt.h"
#include "optinfo.h"


static  void    Usage( void ) {
//=============================

    char        buff[LIST_BUFF_SIZE+1];

    PrtBanner();
    puts( "" );
    MsgBuffer( CL_USAGE_LINE, buff );
    puts( buff );
    puts( "" );
    ShowOptions( buff );
}
