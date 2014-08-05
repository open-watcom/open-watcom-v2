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
* Description:  Librarian command line parsing.
*
****************************************************************************/


#include "wlib.h"
#include "wio.h"
#include "clibext.h"
#ifndef IDE_PGM
#include "clibint.h"
#endif

#define AR_MODE_ENV "WLIB$AR"

#define eatwhite( c ) while( *(c) && isspace( *(unsigned char *)(c) ) ) ++(c);
#define notwhite( c ) ( (c) != '\0' && !isspace( (unsigned char)(c) ) )

#define my_tolower( c ) tolower( (unsigned char)(c) )

options_def     Options;
lib_cmd         *CmdList;

static lib_cmd  **CmdListEnd;

static char *GetString( char *c, char *buff, bool singlequote, bool ignoreSpaceInQuotes )
{
    char    quote;

    eatwhite(c);
    if( (*c == '\"') || ( singlequote && (*c == '\'') ) ) {
        quote = *c;
        c++;
        while( (*c != '\0') && (*c != quote) ) {
            *buff++ = *c++;
        }
        if( *c == quote ) {
            c++;
        }
    } else {
        int inquote = FALSE;

        while( inquote || notwhite( *c ) ) {
            if( ignoreSpaceInQuotes ) {
                if( *c == '\0' ) {
                    break;
                } else if( (*c == '\"') || (*c == '\'') ) {
                    inquote = !inquote;
                }
            }
            *buff++ = *c++;
        }
    }
    *buff = '\0';
    return( c );
}

static char *GetEqual( char *c, char *buff, char *ext, char **ret )
{
    char    *start = c;

    eatwhite( c );
    if( *c == '=' ) {
        ++c;
        eatwhite( c );
    } else {
        c = start;
    }
    if( *c == ' ' || *c == '\0' ) {
        *ret = NULL;
    } else {
        c = GetString( c, buff, FALSE, FALSE );
        if( ext != NULL ) {
            DefaultExtension( buff, ext );
        }
        *ret = DupStr( buff );
    }
    return( c );
}

static void SetPageSize( unsigned short new_size )
{
    unsigned int i;

    Options.page_size = MIN_PAGE_SIZE;
    for( i = 4; i < 16; i++ ) {
        if( new_size & (1 << i) ) {
            Options.page_size = 1 << i;
        }
    }
    if( Options.page_size < new_size ) {
        Options.page_size <<= 1;
    }
}

static void DuplicateOption( char *c )
{
    FatalError( ERR_DUPLICATE_OPTION, c );
}

static char *ParseOption( char *c, char *buff )
{
    unsigned long   page_size;
    char            *start;
    char            *page;
    char            *endptr;
    ar_format       libformat;

    start = c++;
    eatwhite( c );
    switch( my_tolower( *c++ ) ) {
    case '?':
        Banner();
        Usage();
        break;
    case 'b': //                       (don't create .bak file)
        Options.no_backup = 1;
        break;
    case 'c': //                       (case sensitive)
        Options.respect_case = 1;
        break;
    case 'd': // = <object_output_directory>
        if( Options.output_directory ) {
            DuplicateOption( start );
        }
        c = GetEqual( c, buff, NULL, &Options.output_directory );
        if( access( Options.output_directory, F_OK ) != 0 ) {
            FatalError( ERR_DIR_NOT_EXIST, Options.output_directory );
        }
        break;
    case 'i':
        switch( my_tolower( *c++ ) ) {
        case 'n':
            switch( my_tolower( *c++ ) ) {
            case 'n':
                Options.nr_ordinal = FALSE;
                break;
            case 'o':
                Options.nr_ordinal = TRUE;
                break;
            default:
                c = start;
                break;
            }
            break;
        case 'r':
            switch( my_tolower( *c++ ) ) {
            case 'n':
                Options.r_ordinal = FALSE;
                break;
            case 'o':
                Options.r_ordinal = TRUE;
                break;
            default:
                c = start;
                break;
            }
            break;
        case 'a':
            if( Options.processor != WL_PROC_NONE ) {
                DuplicateOption( start );
            }
            Options.processor = WL_PROC_AXP;
            break;
        case 'p':
            if( Options.processor != WL_PROC_NONE ) {
                DuplicateOption( start );
            }
            Options.processor = WL_PROC_PPC;
            break;
        case 'i':
            if( Options.processor != WL_PROC_NONE ) {
                DuplicateOption( start );
            }
            Options.processor = WL_PROC_X86;
            break;
        case '6':
            if( Options.processor != WL_PROC_NONE ) {
                DuplicateOption( start );
            }
            Options.processor = WL_PROC_X64;
            break;
        case 'e':
            if( Options.filetype != WL_FTYPE_NONE ) {
                DuplicateOption( start );
            }
            Options.filetype = WL_FTYPE_ELF;
            break;
        case 'c':
            if( ( my_tolower( *c ) == 'l' ) ) {
                Options.coff_import_long = 1;
                ++c;
            }
            if( Options.filetype != WL_FTYPE_NONE ) {
                DuplicateOption( start );
            }
            Options.filetype = WL_FTYPE_COFF;
            break;
        case 'o':
            if( Options.filetype != WL_FTYPE_NONE ) {
                DuplicateOption( start );
            }
            Options.filetype = WL_FTYPE_OMF;
            break;
        default:
            c = start;
            break;
        }
        break;
    case 'h':
        Banner();
        Usage();
        break;
    case 'l': // [ = <list_file_name> ]
        if( Options.list_contents ) {
            DuplicateOption( start );
        }
        Options.list_contents = 1;
        c = GetEqual( c, buff, EXT_LST, &Options.list_file );
        break;
    case 'm': //                       (display C++ mangled names)
        Options.mangled = 1;
        break;
    case 'o': // = <out_library_name>
        if( Options.output_name ) {
            DuplicateOption( start );
        }
        c = GetEqual( c, buff, EXT_LIB, &Options.output_name );
        break;
    case 'q': //                       (don't print header)
        Options.quiet = 1;
        break;


    case 'v': //                       (don't print header)
        Options.quiet = 0;
        break;
    case 'x': //                       (explode all objects in library)
        Options.explode = 1;
#ifndef NDEBUG
        Options.explode_count = 0;
        if( ( my_tolower( *c ) == 'n' ) ) {
            Options.explode_count = 1;
            ++c;
        }
        c = GetEqual( c, buff, NULL, &Options.explode_ext );
        if( Options.explode_ext == NULL )
            Options.explode_ext = EXT_OBJ;
        if( Options.explode_count ) {
            char  cn[20] = "00000000";
            strcat( cn, Options.explode_ext );
            Options.explode_ext = DupStr( cn );
        }
#else
        c = GetEqual( c, buff, NULL, &Options.explode_ext );
#endif
        break;
    case 'z':
        if( ( my_tolower( *c ) == 'l' ) && ( my_tolower( *(c + 1) ) == 'd' ) ) {
            c += 2;
            if( Options.strip_dependency ) {
                DuplicateOption( start );
            }
            Options.strip_dependency = 1; //(strip dependency info)
            break;
        } else if( ( my_tolower( *c ) == 'l' ) && ( my_tolower( *(c + 1) ) == 'l' ) ) {
            c += 2;
            if( Options.strip_library ) {
                DuplicateOption( start );
            }
            Options.strip_library = 1;  //(strip library info)
            break;
        } else if( Options.strip_expdef ) {
            DuplicateOption( start );
        }
        Options.strip_expdef = 1;       // JBS 99/07/09
        c = GetEqual( c, buff, NULL, &Options.export_list_file );
        break;
    case 't':
        if( my_tolower( *c ) == 'l' ) {
            ++c;
            Options.list_contents = 1;
            Options.terse_listing = 1; // (internal terse listing option)
        } else {
            Options.trim_path = 1; //(trim THEADR pathnames)
        }
        break;
    case 'f':
        switch( my_tolower( *c++ ) ) {
        case 'm':
            if( Options.libtype != WL_LTYPE_NONE ) {
                DuplicateOption( start );
            }
            Options.libtype = WL_LTYPE_MLIB;
            Options.elf_found = 1;
            break;
        case 'a':
            switch( my_tolower( *c++ ) ) {
            case 'b':
                libformat = AR_FMT_BSD;
                break;
            case 'c':
                libformat = AR_FMT_COFF;
                break;
            case 'g':
                libformat = AR_FMT_GNU;
                break;
            default:
                --c;
                libformat = AR_FMT_NONE;
                break;
            }
            if( Options.libtype != WL_LTYPE_NONE ) {
                DuplicateOption( start );
            } else if( libformat != AR_FMT_NONE ) {
                Options.ar_libformat = libformat;
            }
            Options.libtype = WL_LTYPE_AR;
            Options.coff_found = 1;
            break;
        case 'o':
            if( Options.libtype != WL_LTYPE_NONE ) {
                DuplicateOption( start );
            }
            Options.libtype = WL_LTYPE_OMF;
            Options.omf_found = 1;
            break;
        default:
            c = start;
            break;
        }
        break;
// following only used by OMF libary format
    case 'p':
        if( my_tolower( *c ) == 'a' ) {
            c++;
            if( Options.page_size ) {
                DuplicateOption( start );
            }
            Options.page_size = (unsigned short)-1;
        } else {
            if( Options.page_size ) {
                DuplicateOption( start );
            }
            c = GetEqual( c, buff, NULL, &page );
            errno = 0;
            page_size = strtoul( page, &endptr, 0 );
            if( *endptr != '\0' ) {
                FatalError( ERR_BAD_CMDLINE, start );
            } else if( errno == ERANGE || page_size > MAX_PAGE_SIZE ) {
                FatalError( ERR_PAGE_RANGE );
            }
            MemFree( page );
            SetPageSize( (unsigned short)page_size );
        }
        break;
    case 'n': //                       (always create a new library)
        Options.new_library = 1;
        break;
    case 's':
        Options.strip_line = 1;
        break;
    default:
        c = start;
        break;
    }
    return( c );
}

static void AddCommand( operation ops, char *name )
{
    lib_cmd         *new;

    new = MemAllocGlobal( sizeof( lib_cmd ) + strlen( name ) );
    strcpy( new->name, name );
    new->fname = NULL;
    if( ops == OP_EXTRACT ) {
        char    *p;

        p = strchr( new->name, '=' );
        if( p != NULL ) {
            *p = '\0';
            new->fname = p + 1;
        }
    }
    new->ops = ops;
    new->next = *CmdListEnd;
    *CmdListEnd = new;
    CmdListEnd = &new->next;
}

static void FreeCommands( void )
{
    lib_cmd     *cmd, *next;

    for( cmd = CmdList; cmd != NULL; cmd = next ) {
        next = cmd->next;
        MemFreeGlobal( cmd );
    }
}

static char *ParseCommand( char *c )
{
    bool            doquotes = TRUE;
    bool            ignoreSpacesInQuotes = FALSE;
    char            *start;
    operation       ops = 0;
    //char        buff[_MAX_PATH];
    char            buff[ MAX_IMPORT_STRING ];

    start = c;
    eatwhite( c );
    switch( *c++ ) {
    case '-':
        ops = OP_DELETE;
        switch( *c ) {
        case '+':
            ops |= OP_ADD;
            doquotes = FALSE;
            ++c;
            break;
#if defined(__UNIX__)
        case ':':
#else
        case '*':
#endif
            ops |= OP_EXTRACT;
            doquotes = FALSE;
            ++c;
            break;
        }
        break;
    case '+':
        ops = OP_ADD;
        switch( *c ) {
        case '-':
            ops |= OP_DELETE;
            doquotes = FALSE;
            ++c;
            break;
        case '+':
            ops |= OP_IMPORT;
            doquotes = FALSE;
            ignoreSpacesInQuotes = TRUE;
            ++c;
            break;
        }
        break;
#if defined(__UNIX__)
    case ':':
#else
    case '*':
#endif
        ops |= OP_EXTRACT;
        if( *c == '-' ) {
            ops |= OP_DELETE;
            doquotes = FALSE;
            ++c;
        }
        break;
    default:
        FatalError( ERR_BAD_CMDLINE, start );
    }
    eatwhite( c );
    c = GetString( c, buff, doquotes, ignoreSpacesInQuotes );
    AddCommand( ops, buff );
    return( c );
}

#define MAX_CMDLINE     (10*1024)

static void ParseOneLine( char *c )
{
    char        *buff;
    char        *start;

    buff = MemAlloc( MAX_CMDLINE );
    for( ;; ) {
        eatwhite( c );
        start = c;
        switch( *c ) {
#if !defined(__UNIX__)
        case '/':
            c = ParseOption( c, buff );
            if( c == start )
                FatalError( ERR_BAD_OPTION, c[ 1 ] );
            break;
#endif

        case '-':
            if( CmdList == NULL && Options.input_name == NULL ) {
                c = ParseOption( c, buff );
                if( c != start ) {
                    break;
                }
            }
#if !defined(__UNIX__)
        case '*':
#else
        case ':':
#endif
        case '+':
            c = ParseCommand( c );
            break;
        case '@':
            ++c;
            c = GetString( c, buff, TRUE, FALSE );
            {
                char *env = WlibGetEnv( buff );


                if( env != NULL ) {
                    ParseOneLine(env);
                } else {
                    FILE    *io;
                    DefaultExtension( buff, EXT_CMD );
                    io = fopen( buff, "r" );
                    if( io == NULL ) {
                        FatalError( ERR_CANT_OPEN, buff, strerror( errno ) );
                    }
                    while( fgets( buff, MAX_CMDLINE, io ) != NULL ) {
                        ParseOneLine( buff );
                    }
                    fclose( io );
                }
            }
            break;
        case '\0':
        case '#':
            // comment - blow away line
            MemFree( buff );
            return;
        default:
            c = GetString( c, buff, TRUE, FALSE );
            if( strcmp( buff, "?" ) == 0 ) {
                Banner();
                Usage();
            }
            if( Options.input_name ) {
                AddCommand( OP_ADD|OP_DELETE, buff );
            } else {
                DefaultExtension( buff, EXT_LIB );
                Options.input_name = DupStr( buff );
            }
            break;
        }
    }
}

static char *ParseArOption( char *c, operation *mode )
{
    char    *start = c;

    while( notwhite( *c ) ) {
        switch( my_tolower( *c ) ) {
        case '?':
            Banner();
            Usage();
            break;
        case 'c':
            if( Options.no_c_warn ) {
                DuplicateOption( start );
            }
            Options.no_c_warn = TRUE;
            break;
        case 'd':
            if( *mode != OP_NONE ) {
                FatalError( ERR_BAD_OPTION, c[ 0 ] );
            }
            *mode = OP_DELETE;
            break;
        case 'p':
            //ignore not implemented
            break;
        case 'r':
            if( *mode != OP_NONE ) {
                FatalError( ERR_BAD_OPTION, c[ 0 ] );
            }
            *mode = OP_ADD | OP_DELETE;
            break;
        case 't':
            if( *mode != OP_NONE ) {
                FatalError( ERR_BAD_OPTION, c[ 0 ] );
            }
            *mode = OP_TABLE;
            Options.list_contents = 1;
            break;
        case 'u':
            if( Options.update ) {
                DuplicateOption( start );
            }
            Options.update = TRUE;
            break;
        case 'v':
            if( Options.verbose ) {
                DuplicateOption( start );
            }
            Options.verbose = TRUE;
            break;
        case 'x':
            if( *mode != OP_NONE ) {
                FatalError( ERR_BAD_OPTION, c[ 0 ] );
            }
            *mode = OP_EXTRACT;
            break;
        case '-':
            break;
        default:
            FatalError( ERR_BAD_OPTION, c[ 0 ] );
        }
        c++;
    }
    return( c );
}

static void ParseOneArLine( char *c )
{
    char        *buff;
    operation   mode;
    bool        done_options;

    mode = OP_NONE;
    done_options = FALSE;
    buff = MemAlloc( MAX_CMDLINE );
    for( ;; ) {
        eatwhite( c );
        switch( *c ) {
        case '\0':
            if( mode == OP_EXTRACT ) {
                Options.explode = TRUE;
            }
            MemFree( buff );
            return;
        case '-':
            if( !done_options ) {
                if( *(c + 1) == '-' ) {
                    c += 2;
                    done_options = TRUE;
                } else {
                    c = ParseArOption( c, &mode );
                }
                break;
            }
            //fall to default
        default:
            if( mode == OP_NONE ) {
                c = ParseArOption( c, &mode );
                break;
            }
            c = GetString( c, buff, TRUE, FALSE );
            if( Options.input_name ) {
                AddCommand( mode, buff );
                break;
            } else {
                DefaultExtension( buff, EXT_LIB );
                Options.input_name = DupStr( buff );
            }
            break;
        }
    }
}

void ProcessCmdLine( char *argv[] )
{
    char        *parse;
    char        *env;
    lib_cmd     *cmd;
    char        *fname;
    char        buffer[ PATH_MAX ];

    fname = MakeFName( _cmdname( buffer ) );
    if( FNCMP( fname, "ar" ) == 0 || WlibGetEnv( AR_MODE_ENV ) != NULL ) {
        Options.ar = TRUE;
    }
    if( Options.ar ) {
        env = WlibGetEnv( "AR" );
    } else {
        env = WlibGetEnv( "WLIB" );
    }
    if( env == NULL && argv[ 1 ] == NULL || argv[ 1 ][ 0 ] == '\0' ) {
        Banner();
        Usage();
    }
    if( env != NULL ) {
        parse = DupStr( env );
        if( Options.ar ) {
            ParseOneArLine( parse );
        } else {
            ParseOneLine( parse );
        }
        MemFree( parse );
    }
    argv++;
    while( *argv != NULL ) {
        parse = DupStr( *argv );
        if( Options.ar ) {
            ParseOneArLine( parse );
        } else {
            ParseOneLine( parse );
        }
        MemFree( parse );
        argv++;
    }
    if( Options.ar && CmdList != NULL && Options.explode ) {
        Options.explode = 0;
    } else if( CmdList == NULL && !(Options.list_contents) && !(Options.explode)
            && !(Options.new_library) ) {
        /* Default action: List the input lib */
        if( Options.output_name == NULL ) {
            Options.list_contents = 1;
            Options.list_file = DupStr("");
        } else { /* Or copy it to the output lib */
            Options.modified = TRUE;
        }
    }

    if( !Options.ar ) {
        Banner();
    }
    if( Options.input_name == NULL ) {
        FatalError( ERR_NO_LIBNAME );
    }
    if( access( Options.input_name, F_OK ) != 0 && !Options.new_library ) {
        if( !Options.no_c_warn ) {
            Warning( ERR_CREATING_LIBRARY, Options.input_name );
        }
        Options.new_library = 1;
    }
    if( Options.new_library ) {
        for( cmd = CmdList; cmd != NULL; cmd = cmd->next ) {
            if( cmd->ops & OP_EXTRACT ) {
                FatalError( ERR_DELETE_AND_CREATE );
            }
        }
    }
}

void InitCmdLine( void )
{
    CmdList = NULL;
    CmdListEnd = &CmdList;
    memset( &Options, 0, sizeof( Options ) );
#if defined( __LINUX__ )
    Options.ar_libformat = AR_FMT_GNU;
#elif defined( __BSD__ )
    Options.ar_libformat = AR_FMT_BSD;
#else
    Options.ar_libformat = AR_FMT_COFF;
#endif
}

void ResetCmdLine( void )
{
    MemFree( Options.output_directory );
    MemFree( Options.list_file );
    MemFree( Options.output_name );
    MemFree( Options.input_name );
    FreeCommands();
    InitCmdLine();
}
