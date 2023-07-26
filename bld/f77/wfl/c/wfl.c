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
* Description:  Compile and link utility for Open Watcom FORTRAN.
*
****************************************************************************/


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#ifdef __UNIX__
  #include <dirent.h>
#else
  #include <direct.h>
#endif
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include <process.h>
#endif
#if defined( __WATCOMC__ ) || defined( __UNIX__ )
#include <fnmatch.h>
#endif
#include "switch.h"
#include "bool.h"
#include "wio.h"
#include "banner.h"
#include "swchar.h"
#include "pathgrp2.h"
#include "compcfg.h"
#include "wressetr.h"
#include "wresset2.h"
#include "wreslang.h"
#include "wfl.rh"

#include "clibint.h"
#include "clibext.h"


#if _CPU == 8086
    #define _TARGET_    "x86 16-bit"
    #define WFC         "wfc"           // copmiler name
    #define WFL_ENV     "WFL"           // "WFL" environment variable
    #define WFL_NAME    "wfl"
#elif _CPU == 386
    #define _TARGET_    "x86 32-bit"
    #define WFC         "wfc386"        // compiler name
    #define WFL_ENV     "WFL386"        // "WFL" environment variable
    #define WFL_NAME    "wfl386"
#elif _CPU == _AXP
    #define _TARGET_    "Alpha AXP"
    #define WFC         "wfcaxp"        // copmiler name
    #define WFL_ENV     "WFLAXP"        // "WFL" environment variable
    #define WFL_NAME    "wflaxp"
#elif _CPU == _PPC
    #define _TARGET_    "PowerPC"
    #define WFC         "wfcppc"        // copmiler name
    #define WFL_ENV     "WFLPPC"        // "WFL" environment variable
    #define WFL_NAME    "wflppc"
#else
    #error Unknown Target CPU
#endif

#define ERR_BUFF_SIZE   256     // error file buffer size

#define MAX_SUBSTITUTABLE_ARGS  8
#define MAX_INT_SIZE    11              //  buffer for 32-bit integer strings

#define TOOL_FOR_EXT    "for"
#define TOOL_LNK_EXT    "lnk"

#define LINK            "wlink"         // linker name
#define PACK            "cvpack"        // packer name
#define TEMPFILE        "__wfl__" "." TOOL_LNK_EXT  // temporary linker directive file 8.3

#if defined(__UNIX__)
#define fname_cmp   strcmp
#else
#define fname_cmp   stricmp
#endif

#if defined( __UNIX__ )
  #define OBJ_EXT       "o"             // object file extension
  #define TOOL_EXE_EXT  ""              // tool executable file extension
#else
  #define OBJ_EXT       "obj"           // object file extension
  #define TOOL_EXE_EXT  ".exe"          // tool executable file extension
#endif

#ifdef __UNIX__
  #define ISVALIDENTRY(e)   (1)
  #define FNMATCH_FLAGS     (FNM_PATHNAME | FNM_NOESCAPE)
#else
  #define ISVALIDENTRY(e)   ((e->d_attr & (_A_HIDDEN | _A_SYSTEM | _A_VOLID | _A_SUBDIR)) == 0 )
  #define FNMATCH_FLAGS     (FNM_PATHNAME | FNM_NOESCAPE | FNM_IGNORECASE)
#endif

#if defined( __OS2__ ) || defined( __NT__ ) || defined( __UNIX__ )
#define MAX_CMD 10240
#else
#define MAX_CMD 130
#endif

#define MAX_OPTIONS     64

#define IS_OBJ(x)       (x[0] == '.' && fname_cmp(x + 1, OBJ_EXT) == 0)
#define IS_WS(x)        ((x)==' ' || (x)=='\t')

enum {
    MSG_USAGE_COUNT = 0
    #define pick(c,e,j) + 1
    #include "usage.gh"
    #undef pick
};

typedef struct list {
    char        *item;
    struct list *next;
} list;

typedef enum tool_type {
    TYPE_FOR,
    TYPE_LINK,
    TYPE_PACK,
    TYPE_MAX
} tool_type;

typedef union msg_arg {
    char                *s;
    int                 d;
    unsigned int        u;
    long                i;
} msg_arg;


static const char *DebugOptions[] = {
    "",
    "debug dwarf\n",
    "debug dwarf\n",
    "debug watcom all\n",
    "debug codeview\n",
    "debug dwarf\n"
};

static  struct flags {
    boolbit     quiet        : 1;  // compile quietly
    boolbit     no_link      : 1;  // compile only, no link step
    boolbit     link_for_sys : 1;  // system specified
#if _CPU == 8086
    boolbit     link_for_win : 1;  // Windows application
    boolbit     link_for_dos : 1;  // produce DOS executable
    boolbit     link_for_os2 : 1;  // produce OS/2 executable
#else
    boolbit     default_win  : 1;  // OS/2 default windowed application
#endif
    boolbit     do_cvpack    : 1;  // do codeview cvpack
    boolbit     map_wanted   : 1;  // create map file
} Flags;

static struct {
    char *name;
    char *exename;
    char *path;
} tools[ TYPE_MAX ] = {
    { WFC,      WFC TOOL_EXE_EXT,       NULL },
    { LINK,     LINK TOOL_EXE_EXT,      NULL },
    { PACK,     PACK TOOL_EXE_EXT,      NULL }
};

static  char    *Word;                  // one parameter
static  char    *CmpOpts[MAX_CMD];      // list of compiler options from Cmd
static  char    PathBuffer[_MAX_PATH];  // path for compiler or linker executable file
static  char    *Link_Name = NULL;      // name for TempFile if /fd specified
static  char    *Map_Name = NULL;       // map file name
static  char    *Indir_Name = NULL;     // indirect name
static  list    *Obj_List;              // linked list of object filenames
static  list    *File_List;             // list of filenames from Cmd
static  list    *Lib_List;               // list of libraries from Cmd
static  list    *Directive_List;        // list of libraries from Cmd
static  char    SwitchChars[3];         // valid switch characters
static  char    *Exe_Name = NULL;       // name of executable
static  char    *Obj_Name = NULL;       // object file name pattern
static  char    *SystemName = NULL;     // system name
static  char    *StackSize = NULL;      // stack size
static  int     DebugFlag;              // debugging flag

static  HANDLE_INFO     hInstance = { 0 };
static  unsigned        MsgShift;

static bool LoadMsg( unsigned msg, char *buffer, int buff_size )
/***************************************************************
 * Load a message into the specified buffer.  This function is called
 * by WLINK when linked with 16-bit version of WATFOR-77.
 */
{
    return( hInstance.status && ( WResLoadString( &hInstance, msg + MsgShift, buffer, buff_size ) > 0 ) );
}

static char *GetMsg( unsigned msg )
/**********************************
 * Build error message.
 */
{
    static char    msg_buf[ERR_BUFF_SIZE];

    if( !LoadMsg( msg, msg_buf, ERR_BUFF_SIZE - 1 ) ) {
        msg_buf[0] = '\0';
    }
    return( msg_buf );
}

static void ErrorInit( const char *pgm_name )
/*******************************************/
{
    hInstance.status = 0;
    if( OpenResFile( &hInstance, pgm_name ) ) {
        MsgShift = _WResLanguage() * MSG_LANG_SPACING;
        return;
    }
    CloseResFile( &hInstance );
}

static void ErrorFini( void )
/***************************/
{
    CloseResFile( &hInstance );
}

static  void    printfMsg( unsigned msg, ... )
/********************************************/
{
    va_list     args;
    char        buff[ERR_BUFF_SIZE+1];

    va_start( args, msg );
    vsprintf( buff, GetMsg( msg ), args );
    va_end( args );
    puts( buff ); // skip leading blank
}


static void PrtBanner( void )
/***************************/
{
#if defined( _BETAVER )
    puts( banner1t( "F77 " _TARGET_ " Compile and Link Utility" ) );
    puts( banner1v( _WFL_VERSION_ ) );
#else
    puts( banner1w( "F77 " _TARGET_ " Compile and Link Utility", _WFL_VERSION_ ) );
#endif
    puts( banner2 );
    puts( banner2a( 1990 ) );
    puts( banner3 );
    puts( banner3a );
}


static  void    Usage( void )
/***************************/
{
    unsigned    msg;
    char        *str;

    PrtBanner();
    puts( "" );
    for( msg = MSG_USAGE_BASE; msg < MSG_USAGE_BASE + MSG_USAGE_COUNT; msg++ ) {
        str = GetMsg( msg );
        puts( str );
    }
}


static  void    *MemAlloc( size_t size )
/**************************************/
{
    void            *ptr;

    ptr = malloc( size );
    if( ptr == NULL ) {
        printfMsg( CL_OUT_OF_MEMORY );
        ErrorFini();
        exit( 1 );
    }
    return( ptr );
}

static  void    MemFree( void *ptr )
/**********************************/
{
    free( ptr );
}

static char     *MemStrDup( const char *str )
/*******************************************/
{
    size_t      size;

    size = strlen( str ) + 1;
    return( memcpy( MemAlloc( size ), str, size ) );
}

static void ListAppend( list **itm_list, list *new )
/**************************************************/
{
    list    *itm;

    if( *itm_list == NULL ) {
        *itm_list = new;
    } else {
        itm = *itm_list;
        while( itm->next != NULL ) {
            itm = itm->next;
        }
        itm->next = new;
    }
}

static list *ListItemFree( list *itm_list )
/*****************************************/
{
    list    *next;

    next = itm_list->next;
    MemFree( itm_list->item );
    MemFree( itm_list );
    return( next );
}

static void ListFree( list *itm_list )
/************************************/
{
    while( itm_list != NULL ) {
        itm_list = ListItemFree( itm_list );
    }
}

static void     ListAppendString( list **l, const char *fname )
/*************************************************************/
{
    list *p;

    p = MemAlloc( sizeof( list ) );
    p->item = MemStrDup( fname );
    ListAppend( l, p );
}

static int     IsOption( const char *cmd, size_t cmd_len, const char *opt )
/*************************************************************************/
{
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

static char *DoQuoted( char *buffer, const char *name, char quote_char )
/**********************************************************************/
{
    char *p;
    int  quotes;

    p = buffer;
    if( name != NULL ) {
        quotes = ( strchr( name, ' ' ) != NULL );
        if( quotes )
            *p++ = quote_char;
        while( (*p = *name) != '\0' ) {
            ++p;
            ++name;
        }
        if( quotes ) {
            *p++ = quote_char;
        }
    }
    *p = '\0';
    return( buffer );
}

static void AddDirectivePath( const char *directive, const char *path )
/*********************************************************************/
{
    list        *new_item;
    size_t      len;
    char        *p;

    len = strlen( directive );
    new_item = MemAlloc( sizeof( list ) );
    new_item->next = NULL;
    p = new_item->item = MemAlloc( len + strlen( path ) + 2 + 1 );
    memcpy( p, directive, len );
    p += len;
    DoQuoted( p, path, '\'' );
#ifndef __UNIX__
    while( (p = strchr( p, '/' )) != NULL ) {
        *p++ = '\\';
    }
#endif
    ListAppend( &Directive_List, new_item );
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

static DIR  *wild_dirp = NULL;  /* we need this across invocations */
static char *wildpath = NULL;
static char *wildpattern = NULL;

static void DoWildCardClose( void )
/*********************************/
{
    if( wildpath != NULL ) {
        MemFree( wildpath );
        wildpath = NULL;
    }
    if( wildpattern != NULL ) {
        MemFree( wildpattern );
        wildpattern = NULL;
    }
    if( wild_dirp != NULL ) {
        closedir( wild_dirp );
        wild_dirp = NULL;
    }
}

static const char *DoWildCard( const char *base )
/***********************************************/
{
    pgroup2         pg;
    struct dirent   *dire;

    if( base != NULL ) {
        /* clean up from previous invocation */
        DoWildCardClose();
        if( strpbrk( base, "*?" ) == NULL ) {
            return( base );
        }
        // create directory name and pattern
        wildpath = MemAlloc( _MAX_PATH );
        wildpattern = MemAlloc( _MAX_PATH );
        strcpy( wildpath, base );
        _splitpath2( wildpath, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
        _makepath( wildpath, pg.drive, pg.dir, ".", NULL );
        // create file name pattern
        _makepath( wildpattern, NULL, NULL, pg.fname, pg.ext );
        wild_dirp = opendir( wildpath );
        if( wild_dirp == NULL ) {
            DoWildCardClose();
            return( NULL );
        }
    }
    if( wild_dirp == NULL ) {
        return( NULL );
    }
    while( (dire = readdir( wild_dirp )) != NULL ) {
        if( ISVALIDENTRY( dire ) ) {
            if( fnmatch( wildpattern, dire->d_name, FNMATCH_FLAGS ) == 0 ) {
                break;
            }
        }
    }
    if( dire == NULL ) {
        DoWildCardClose();
        return( NULL );
    }
    _splitpath2( wildpath, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    _makepath( wildpath, pg.drive, pg.dir, dire->d_name, NULL );
    return( wildpath );
}

static char *FindToolPath( tool_type utl )
/****************************************/
{
    if( tools[utl].path == NULL ) {
        _searchenv( tools[utl].exename, "PATH", PathBuffer );
        if( PathBuffer[0] == '\0' ) {
            printfMsg( CL_UNABLE_TO_FIND, tools[utl].exename );
            ErrorFini();
            exit( 1 );
        }
        tools[utl].path = MemStrDup( PathBuffer );
    }
    return( tools[utl].path );
}

static int tool_exec( tool_type utl, char *target, char **options )
/*****************************************************************/
{
    int     rc;
    int     pass_argc;
    char    *pass_argv[MAX_OPTIONS + 3];


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

    rc = (int)spawnvp( P_WAIT, FindToolPath( utl ), (char const *const *)pass_argv );

    if( rc != 0 ) {
        if( (rc == -1) || (rc == 255) ) {
            if( utl == TYPE_LINK ) {
                printfMsg( CL_UNABLE_TO_INVOKE_LINKER );
            } else if( utl == TYPE_PACK ) {
                printfMsg( CL_UNABLE_TO_INVOKE_CVPACK );
            } else {
                printfMsg( CL_UNABLE_TO_INVOKE_COMPILER );
            }
        } else {
            if( utl == TYPE_LINK ) {
                printfMsg( CL_BAD_LINK );
            } else if( utl == TYPE_PACK ) {
                printfMsg( CL_BAD_LINK );
            } else {
                printfMsg( CL_BAD_COMPILE, target );
            }
        }
    }
    return( rc );
}


static  void    Fputnl( const char *text, FILE *fp )
/**************************************************/
{
    fputs( text, fp );
    fputs( "\n", fp );
}


static  void    MakeName( char *name, const char *ext )
/*****************************************************/
{
    pgroup2 pg;

    _splitpath2( name, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    if( pg.ext[0] != '\0' )
        ext = pg.ext;
    _makepath( name, pg.drive, pg.dir, pg.fname, ext );
}


static  void    AddName( const char *name )
/*****************************************/
{
    list        *curr_name;
    list        *last_name;
    char        path[_MAX_PATH];
    pgroup2     pg1;
    pgroup2     pg2;

    last_name = NULL;
    for( curr_name = Obj_List; curr_name != NULL; curr_name = curr_name->next ) {
        if( fname_cmp( name, curr_name->item ) == 0 )
            return;
        last_name = curr_name;
    }
    ListAppendString( &Obj_List, name );
    if( Obj_Name != NULL ) {
        // construct full name of object file from Obj_Name information
        _splitpath2( Obj_Name, pg1.buffer, &pg1.drive, &pg1.dir, &pg1.fname, &pg1.ext );
        if( pg1.ext[0] == '\0' )
            pg1.ext = OBJ_EXT;
        if( ( pg1.fname[0] == '\0' )
          || ( ( pg1.fname[0] == '*' ) && ( pg1.fname[1] == '\0' ) ) ) {
            _splitpath2( name, pg2.buffer, NULL, NULL, &pg1.fname, &pg2.ext );
            if( pg2.ext[0] != '\0' ) {
                pg1.ext = pg2.ext;
            }
        }
        _makepath( path, pg1.drive, pg1.dir, pg1.fname, pg1.ext );
        name = path;
        MemFree( Obj_Name );
        Obj_Name = NULL;
    }
    AddDirectivePath( "file ", name );
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

static  int     Parse( char *cmd )
/********************************/
{
    char        opt;
    char        c;
    char        *end;
//    char        *cmd;
    size_t      len;
    bool        cmp_option;
    int         cmp_opt_index;
    bool        in_quotes;

    Flags.no_link      = false;
    Flags.link_for_sys = false;
    Flags.quiet        = false;
#if _CPU == 8086
    Flags.link_for_win = false;
    Flags.link_for_dos = false;
    Flags.link_for_os2 = false;
#else
    Flags.default_win  = false;
#endif
    Flags.do_cvpack    = false;
    Flags.map_wanted   = false;

    DebugFlag = 0;
    Directive_List = NULL;

    cmp_opt_index = 0;
    while( *cmd != '\0' ) {
        opt = *cmd;
        if( ( opt == SwitchChars[0] ) || ( opt == SwitchChars[1] ) ) {
            cmd++;
        } else {
            opt = ' ';
        }
        in_quotes = false;
        for( end = cmd; (c = *end) != '\0'; end++ ) {
            if( c == '"' ) {
                if( in_quotes )
                    break;
                in_quotes = true;
            }
            if( !in_quotes ) {
                if( c == ' '  )
                    break;
                if( c == SwitchChars[0] )
                    break;
                if( c == SwitchChars[1] ) {
                    break;
                }
            }
        }
        len = end - cmd;

        if( len != 0 ) {
            if( opt == ' ' ) {  // if filename, add to list
                strncpy( Word, cmd, len );
                Word[len] = '\0';
                strlwr( Word );
                if( strstr( Word, ".lib" ) != NULL ) {
                    ListAppendString( &Lib_List, Word );
                    AddDirectivePath( "library ", Word );
                } else {
                    ListAppendString( &File_List, Word );
                }
            } else {            // otherwise, do option
                --len;
                strncpy( Word, cmd + 1, len );
                Word[len] = '\0';
                cmp_option = true; // assume its a compiler option
                switch( tolower( *cmd ) ) {
                case 'f':       // files option
                    switch( tolower( Word[0] ) ) {
                    case 'd':   // name of linker directive file
                        if( Link_Name != NULL )
                            MemFree( Link_Name );
                        if( (Word[1] == '=') || (Word[1] == '#') ) {
                            MakeName( Word + 2, TOOL_LNK_EXT ); // add extension
                            Link_Name = MemAlloc( strlen( Word + 2 ) + 1 );
                            strcpy( Link_Name, Word + 2 );
                        } else {
                            Link_Name = MemStrDup( TEMPFILE );
                        }
                        cmp_option = false;
                        break;
                    case 'e':   // name of exe file
                        if( ( Word[1] == '=' ) || ( Word[1] == '#' ) ) {
                            if( Exe_Name != NULL )
                                MemFree( Exe_Name );
                            Exe_Name = MemStrDup( Word + 2 );
                            cmp_option = false;
                        }
                        break;
                    case 'm':   // name of map file
                        Flags.map_wanted = true;
                        if( (Word[1] == '=') || (Word[1] == '#') ) {
                            if( Map_Name != NULL )
                                MemFree( Map_Name );
                            Map_Name = MemStrDup( Word + 2 );
                        }
                        cmp_option = false;
                        break;
                    case 'i':
                        if( ( Word[1] == '=' ) || ( Word[1] == '#' ) ) {
                            if( Indir_Name != NULL )
                                MemFree( Indir_Name );
                            Indir_Name = MemStrDup( Word + 2 );
                            cmp_option = false;
                        }
                        break;
                    case 'o':   // name of object file
                        // parse off argument, so we get right filename
                        // in linker command file
                        if( ( Word[1] == '=' ) || ( Word[1] == '#' ) ) {
                            if( Obj_Name != NULL )
                                MemFree( Obj_Name );
                            Obj_Name = MemStrDup( Word + 2 );
                        }
                        break;
                    default:
                        break;
                    }
                    break;
                case 'k':       // stack size option
                    if( ( Word[0] == '=' ) || ( Word[0] == '#' ) ) {
                        if( StackSize != NULL )
                            MemFree( StackSize );
                        StackSize = MemStrDup( Word + 2 );
                        cmp_option = false;
                    }
                    break;
                case 'c':       // compile only
                    if( Word[0] == '\0' ) {
                        Flags.no_link = true;
                        cmp_option = false;
                    }
                    break;
                case 'y':
                    if( Word[0] == '\0' ) {
                        cmp_option = false;
                    }
                    break;
                case 'p':
                    // ignore the /p option - we now only
                    // have a protect-mode compiler
                    if( Word[0] == '\0' ) {
                        cmp_option = false;
                    }
                    break;
                case 'l':
                    if( ( Word[0] == '=' ) || ( Word[0] == '#' ) ) {
                        Flags.link_for_sys = true;
                        if( SystemName != NULL )
                            MemFree( SystemName );
                        SystemName = MemStrDup( &Word[1] );
#if _CPU == 8086
                        Flags.link_for_dos = false;
                        Flags.link_for_os2 = false;
                        Flags.link_for_win = false;
#endif
                        cmp_option = false;
#if _CPU == 8086
                    } else if( stricmp( Word, "r" ) == 0 ) {
                        Flags.link_for_sys = false;
                        Flags.link_for_dos = true;
                        Flags.link_for_os2 = false;
                        Flags.link_for_win = false;
                        cmp_option = false;
                    } else if( stricmp( Word, "p" ) == 0 ) {
                        Flags.link_for_sys = false;
                        Flags.link_for_os2 = true;
                        Flags.link_for_dos = false;
                        Flags.link_for_win = false;
                        cmp_option = false;
#endif
                    }
                    break;
                case '"':
                    UnquoteDirective( Word, MAX_CMD, Word );
                    ListAppendString( &Directive_List, Word );
                    cmp_option = false;
                    break;

                // compiler options that affect the linker

#if _CPU != 8086
                case 'b':
                    if( stricmp( Word, "w" ) ) {
                        Flags.default_win = true;
                    }
                    break;
#endif

                case 'q':
                    if( IsOption( cmd, len + 1, "Quiet" ) ) {
                        Flags.quiet = true;
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
                        Flags.do_cvpack = true;
                        DebugFlag = 4;
                    } else if( strcmp( Word, "d" ) == 0 ) {
                        DebugFlag = 5;
                    }
                    break;
                case 's':
                    if( IsOption( cmd, len + 1, "SYntax" ) ) {
                        Flags.no_link = true;
                    }
                    break;
#if _CPU == 8086
                case 'w':
                    if( IsOption( cmd, len + 1, "WIndows" ) ) {
                        Flags.link_for_sys = false;
                        Flags.link_for_dos = false;
                        Flags.link_for_os2 = false;
                        Flags.link_for_win = true;
                    }
                    break;
#endif
                default:
                    break;
                }

                // don't add linker-specific options to compiler command line
                if( cmp_option ) {
                    CmpOpts[cmp_opt_index] = (char *)MemAlloc( strlen( Word ) + 2 + 1 );
                    CmpOpts[cmp_opt_index][0] = opt;
                    CmpOpts[cmp_opt_index][1] = *cmd;
                    CmpOpts[cmp_opt_index][2] = '\0';
                    strcat( CmpOpts[cmp_opt_index], Word );
                    CmpOpts[++cmp_opt_index] = NULL;
                }
            }
            cmd = end;
        }
        while( *cmd == ' ' ) {
            cmd++;
        }
    }
    return( 0 );
}

static void BuildLinkFile( FILE *fp )
{
    char tmp[_MAX_PATH];
    list *currobj;

    if( Flags.quiet ) {
        Fputnl( "option quiet", fp );
    }
    fputs( DebugOptions[ DebugFlag ], fp );
    if( Flags.link_for_sys ) {
        fputs( "system ", fp );
        Fputnl( SystemName, fp );
        MemFree( SystemName );
        SystemName = NULL;
    } else {
#if defined( __QNX__ )
        Fputnl( "system qnx", fp );
#elif defined( __LINUX__ )
        Fputnl( "system linux", fp );
#elif _CPU == 386
    #if defined( __OS2__ )
        Fputnl( "system os2v2", fp );
    #elif defined( __NT__ )
        Fputnl( "system nt", fp );
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
#elif _CPU == 8086
        if( Flags.link_for_win ) {
            Fputnl( "system windows", fp );
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
#elif _CPU == _AXP
        Fputnl( "system ntaxp", fp );
#else
    #error Unknown System
#endif
    }

    fputs( "name ", fp );
    Fputnl( DoQuoted( tmp, Exe_Name, '\'' ), fp );

    if( StackSize != NULL && StackSize[0] != '\0' ) {
        fputs( "option stack=", fp );
        Fputnl( StackSize, fp );
    }

    if( Flags.map_wanted ) {
        if( Map_Name == NULL || Map_Name[0] == '\0' ) {
            Fputnl( "option map", fp );
        } else {
            AddDirectivePath( "option map=", Map_Name );
        }
    }

    if( Indir_Name != NULL ) {
        AddDirectivePath( "@", Indir_Name );
    }

    for( currobj = Directive_List; currobj != NULL; ) {
        Fputnl( currobj->item, fp );
        currobj = ListItemFree( currobj );
    }
}

static FILE *OpenWlinkTmpFile( char *name )
{
    int     i;
    int     fh;

    for( i = 0; i < 100; i++ ) {
        sprintf( name + 1 + 6, "%2.2d" "." TOOL_LNK_EXT, i );
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

static  int     CompLink( void )
/******************************/
{
    int         rc;
    const char  *file;
    bool        comp_err;
    pgroup2     pg;
    int         i;
    list        *currobj;

    comp_err = false;
    Obj_List = NULL;
    for( currobj = File_List; currobj != NULL; ) {
        strcpy( Word, currobj->item );
        MakeName( Word, "." TOOL_FOR_EXT ); // if no extension, assume "for"
        file = DoWildCard( Word );
        while( file != NULL ) {     // while more filenames:
            strcpy( Word, file );
#ifndef __UNIX__
            strlwr( Word );
#endif
            _splitpath2( Word, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
            if( !IS_OBJ( pg.ext ) ) {   // if not object, compile
                rc = tool_exec( TYPE_FOR, Word, CmpOpts );
                if( rc != 0 ) {
                    comp_err = true;
                    if( ( rc == -1 ) || ( rc == 255 ) ) {
                        rc = 1;
                        break;
                    }
                }
            }
            _makepath( Word, NULL, NULL, pg.fname, NULL );
            if( Exe_Name == NULL || Exe_Name[0] == '\0' ) {
                if( Exe_Name != NULL )
                    MemFree( Exe_Name );
                Exe_Name = MemStrDup( Word );
            }
            _makepath( Word, NULL, NULL, pg.fname, OBJ_EXT );
            AddName( Word );            // add obj filename

            file = DoWildCard( NULL );  // get next filename
        }
        DoWildCardClose();
        currobj = ListItemFree( currobj );
    }

    if( comp_err ) {
        rc = 1;
    } else {
        FILE    *fp;
        char    temp_name[1 + 12 + 1] = "@" TEMPFILE;

        if( (fp = OpenWlinkTmpFile( temp_name )) == NULL ) {
            printfMsg( CL_ERROR_OPENING_TMP_FILE );
            rc = 1;
        } else {
            rc = 0;
            BuildLinkFile( fp );
            fclose( fp );   // close TempFile
            if( ( Obj_List != NULL ) && !Flags.no_link ) {
                rc = tool_exec( TYPE_LINK, temp_name, NULL );
                if( rc == 0 && Flags.do_cvpack ) {
                    rc = tool_exec( TYPE_PACK, Exe_Name, NULL );
                }
                if( rc != 0 ) {
                    rc = 2;    // return 2 to show Temp_File already closed
                }
            }
            if( Link_Name != NULL ) {
                remove( Link_Name );
                rename( temp_name + 1, Link_Name );
            }
            remove( temp_name + 1 );
        }
    }
    ListFree( Obj_List );
    for( i = 0; i < TYPE_MAX; ++i ) {
        if( tools[i].path != NULL ) {
            MemFree( tools[i].path );
            tools[i].path = NULL;
        }
    }
    return( rc );
}

static bool check_y_opt( const char *cmdl )
{
    while( (cmdl = strpbrk( cmdl, SwitchChars )) != NULL ) {
        ++cmdl;
        if( tolower( *cmdl ) == 'y' ) {
            return( true );
        }
    }
    return( false );
}

int     main( int argc, char *argv[] )
/************************************/
{
    int         rc;
    char        *wfl_env;
    int         cmd_len;
    char        *cmd_line;
    char        *cmd;

#if !defined( __WATCOMC__ )
    _argc = argc;
    _argv = argv;
#else
    /* unused parameters */ (void)argc;
#endif

    ErrorInit( argv[0] );

    CmpOpts[0] = NULL;

    SwitchChars[0] = '-';
    SwitchChars[1] = _dos_switch_char();
    SwitchChars[2] = '\0';

    Word = MemAlloc( MAX_CMD );
    /*
     * add "WFL" environment variable to "cmd" unless "/y" is specified
     * in "cmd" or the "WFL" environment string
     */
    cmd_len = _bgetcmd( NULL, 0 ) + 1;  /* check cmd line len */
    wfl_env = getenv( WFL_ENV );
    if( wfl_env != NULL ) {
        size_t  envlen;
        envlen = strlen( wfl_env );
        /*
         * allocate space enough for wfl variable and cmd line
         */
        cmd_line = MemAlloc( envlen + 1 + cmd_len );
        strcpy( cmd_line, wfl_env );
        cmd_line[envlen++] = ' ';
        _bgetcmd( cmd_line + envlen, cmd_len );
        if( check_y_opt( cmd_line ) ) {
            _bgetcmd( cmd_line, cmd_len );
        }
    } else {
        /*
         * allocate space enough for cmd line
         */
        cmd_line = MemAlloc( cmd_len );
        _bgetcmd( cmd_line, cmd_len );
    }
    cmd = cmd_line;
    while( *cmd == ' ' )
        cmd++;
    if( ( *cmd == '\0' ) || ( strncmp( cmd, "? ", 2 ) == 0 ) ) {
        Usage();
        rc = 1;
    } else {
        rc = Parse( cmd );
        if( rc == 0 ) {
            if( !Flags.quiet ) {
                PrtBanner();
            }
            rc = CompLink();
        }
    }
    MemFree( Word );
    MemFree( cmd_line );
    ErrorFini();
    return( rc );
}
