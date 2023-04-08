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
* Description:  Librarian command line parsing.
*
****************************************************************************/


#include "wlib.h"
#include <errno.h>
#ifdef __WATCOMC__
#include <process.h>
#endif
#include "wio.h"

#include "clibext.h"
#ifndef IDE_PGM
#include "clibint.h"
#endif


#define AR_MODE_ENV     "WLIB_AR"
#define MAX_TOKEN_LEN   260

#define eatwhite( c ) while( *(c) != '\0' && isspace( *(unsigned char *)(c) ) ) ++(c);
#define notwhite( c ) ( (c) != '\0' && !isspace( (unsigned char)(c) ) )

#define my_tolower( c ) tolower( (unsigned char)(c) )

typedef struct {
    FILE    *fp;
    char    *buffer;
    size_t  size;
} getline_data;

options_def     Options;
lib_cmd         *CmdList;

static lib_cmd  **CmdListEnd;

static const char *GetString( const char *c, char *token_buff, bool singlequote, bool ignoreSpaceInQuotes )
{
    char    quote;

    eatwhite(c);
    if( (*c == '\"') || ( singlequote && (*c == '\'') ) ) {
        quote = *c;
        c++;
        while( (*c != '\0') && (*c != quote) ) {
            *token_buff++ = *c++;
        }
        if( *c == quote ) {
            c++;
        }
    } else {
        bool inquote = false;

        while( inquote || notwhite( *c ) ) {
            if( ignoreSpaceInQuotes ) {
                if( *c == '\0' ) {
                    break;
                } else if( (*c == '\"') || (*c == '\'') ) {
                    inquote = !inquote;
                }
            }
            *token_buff++ = *c++;
        }
    }
    *token_buff = '\0';
    return( c );
}

static const char *GetEqualString( const char *c, char *token_buff, const char *ext, char **ret )
{
    const char  *start = c;

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
        c = GetString( c, token_buff, false, false );
        if( ext != NULL && *ext != '\0' ) {
            DefaultExtension( token_buff, ext );
        }
        *ret = DupStr( token_buff );
    }
    return( c );
}

static void SetPageSize( unsigned_16 new_size )
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

static void DuplicateOption( const char *c )
{
    FatalError( ERR_DUPLICATE_OPTION, c );
}

static const char *ParseOption( const char *c, char *token_buff )
{
    unsigned long   page_size;
    const char      *start;
    char            *page;
    char            *endptr;
    ar_format       libformat;

    start = c++;
    eatwhite( c );
    switch( my_tolower( *c++ ) ) {
    case '?':
        Usage();
        break;
    case 'b': //                       (don't create .bak file)
        Options.no_backup = true;
        break;
    case 'c': //                       (case sensitive)
        Options.respect_case = true;
        break;
    case 'd': // = <object_output_directory>
        if( Options.output_directory != NULL ) {
            DuplicateOption( start );
        }
        c = GetEqualString( c, token_buff, NULL, &Options.output_directory );
        if( access( Options.output_directory, F_OK ) != 0 ) {
            FatalError( ERR_DIR_NOT_EXIST, Options.output_directory );
        }
        break;
    case 'i':
        switch( my_tolower( *c++ ) ) {
        case 'n':
            switch( my_tolower( *c++ ) ) {
            case 'n':
                Options.nr_ordinal = false;
                break;
            case 'o':
                Options.nr_ordinal = true;
                break;
            default:
                c = start;
                break;
            }
            break;
        case 'r':
            switch( my_tolower( *c++ ) ) {
            case 'n':
                Options.r_ordinal = false;
                break;
            case 'o':
                Options.r_ordinal = true;
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
        case 'm':
            if( Options.processor != WL_PROC_NONE ) {
                DuplicateOption( start );
            }
            Options.processor = WL_PROC_MIPS;
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
                Options.coff_import_long = true;
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
        Usage();
        break;
    case 'l': // [ = <list_file_name> ]
        if( Options.list_contents ) {
            DuplicateOption( start );
        }
        Options.list_contents = true;
        c = GetEqualString( c, token_buff, EXT_LST, &Options.list_file );
        break;
    case 'm': //                       (display C++ mangled names)
        Options.mangled = true;
        break;
    case 'o': // = <out_library_name>
        if( Options.output_name != NULL ) {
            DuplicateOption( start );
        }
        c = GetEqualString( c, token_buff, EXT_LIB, &Options.output_name );
        break;
    case 'q': //                       (don't print header)
        Options.quiet = true;
        break;
    case 'v': //                       (don't print header)
        Options.quiet = false;
        break;
    case 'x': //                       (explode all objects in library)
        Options.explode = true;
#ifndef NDEBUG
        Options.explode_count = 0;
        if( my_tolower( *c ) == 'n' ) {
            Options.explode_count = 1;
            ++c;
        }
        c = GetEqualString( c, token_buff, NULL, &Options.explode_ext );
        if( Options.explode_ext == NULL )
            Options.explode_ext = "." EXT_OBJ;
        if( Options.explode_count ) {
            char    cn[20] = FILE_TEMPLATE_MASK;
            strcpy( cn + sizeof( FILE_TEMPLATE_MASK ) - 1, Options.explode_ext );
            Options.explode_ext = DupStr( cn );
        }
#else
        c = GetEqualString( c, token_buff, NULL, &Options.explode_ext );
#endif
        break;
    case 'z':
        if( ( my_tolower( *c ) == 'l' ) && ( my_tolower( *(c + 1) ) == 'd' ) ) {
            c += 2;
            if( Options.strip_dependency ) {
                DuplicateOption( start );
            }
            Options.strip_dependency = true; //(strip dependency info)
            break;
        } else if( ( my_tolower( *c ) == 'l' ) && ( my_tolower( *(c + 1) ) == 'l' ) ) {
            c += 2;
            if( Options.strip_library ) {
                DuplicateOption( start );
            }
            Options.strip_library = true;  //(strip library info)
            break;
        } else if( Options.strip_expdef ) {
            DuplicateOption( start );
        }
        Options.strip_expdef = true;       // JBS 99/07/09
        c = GetEqualString( c, token_buff, NULL, &Options.export_list_file );
        break;
    case 't':
        if( my_tolower( *c ) == 'l' ) {
            ++c;
            Options.list_contents = true;
            Options.terse_listing = true; // (internal terse listing option)
        } else {
            Options.trim_path = true; //(trim THEADR pathnames)
        }
        break;
    case 'f':
        switch( my_tolower( *c++ ) ) {
        case 'm':
            if( Options.libtype != WL_LTYPE_NONE ) {
                DuplicateOption( start );
            }
            Options.libtype = WL_LTYPE_MLIB;
            Options.elf_found = true;
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
            Options.coff_found = true;
            break;
        case 'o':
            if( Options.libtype != WL_LTYPE_NONE ) {
                DuplicateOption( start );
            }
            Options.libtype = WL_LTYPE_OMF;
            Options.omf_found = true;
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
            Options.page_size = (unsigned_16)-1;
        } else {
            if( Options.page_size ) {
                DuplicateOption( start );
            }
            c = GetEqualString( c, token_buff, NULL, &page );
            errno = 0;
            page_size = strtoul( page, &endptr, 0 );
            if( *endptr != '\0' ) {
                FatalError( ERR_BAD_CMDLINE, start );
            } else if( errno == ERANGE || page_size > MAX_PAGE_SIZE ) {
                FatalError( ERR_PAGE_RANGE );
            }
            MemFree( page );
            SetPageSize( (unsigned_16)page_size );
        }
        break;
    case 'n': //                       (always create a new library)
        Options.new_library = true;
        break;
    case 's':
        Options.strip_line = true;
        break;
    default:
        c = start;
        break;
    }
    return( c );
}

static void AddCommand( operation ops, const char *name )
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
    lib_cmd     *cmd;

    while( (cmd = CmdList) != NULL ) {
        CmdList = cmd->next;
        MemFreeGlobal( cmd );
    }
    CmdListEnd = &CmdList;
}

static const char *ParseCommand( const char *c )
{
    bool            doquotes = true;
    bool            ignoreSpacesInQuotes = false;
    const char      *start;
    operation       ops = 0;
    //char        buff[_MAX_PATH];
    char            buff[MAX_IMPORT_STRING];

    start = c;
    eatwhite( c );
    switch( *c++ ) {
    case '-':
        ops = OP_DELETE;
        switch( *c ) {
        case '+':
            ops |= OP_ADD;
            doquotes = false;
            ++c;
            break;
#if defined(__UNIX__)
        case ':':
#else
        case '*':
#endif
            ops |= OP_EXTRACT;
            doquotes = false;
            ++c;
            break;
        }
        break;
    case '+':
        ops = OP_ADD;
        switch( *c ) {
        case '-':
            ops |= OP_DELETE;
            doquotes = false;
            ++c;
            break;
        case '+':
            ops |= OP_IMPORT;
            doquotes = false;
            ignoreSpacesInQuotes = true;
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
            doquotes = false;
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

static void my_getline_init( const char *name, getline_data *fd )
{
    fd->fp = fopen( name, "rb" );
    if( fd->fp == NULL ) {
        FatalError( ERR_CANT_OPEN, name, strerror( errno ) );
    }
    fd->size = MAX_TOKEN_LEN;
    fd->buffer = MemAlloc( MAX_TOKEN_LEN );
    if( fd->buffer == NULL ) {
        fd->size = 0;
    }
}

static char *my_getline( getline_data *fd )
{
    char    *p;
    size_t  len_used;
    size_t  maxlen;
    size_t  len;

    p = fd->buffer;
    maxlen = fd->size;
    len_used = 0;
    while( fgets( p, maxlen, fd->fp ) != NULL ) {
        len = strlen( p );
        if( len == 0 )
            continue;
        len_used += len;
        if( p[len - 1] == '\n' ) {
            break;
        }
        if( len < maxlen - 1 ) {
            break;
        }
        if( feof( fd->fp ) ) {
            break;
        }
        len = fd->size + MAX_TOKEN_LEN;
        p = MemAlloc( len );
        memcpy( p, fd->buffer, len_used + 1 );
        MemFree( fd->buffer );
        fd->buffer = p;
        fd->size = len;
        p += len_used;
        maxlen = len - len_used;
    }
    if( len_used ) {
        p = fd->buffer;
        if( p[len_used - 1] == '\n' ) {
            len_used--;
            p[len_used] = '\0';
        }
        if( len_used && p[len_used - 1] == '\r' ) {
            len_used--;
            p[len_used] = '\0';
        }
        return( p );
    }
    return( NULL );
}

static void my_getline_fini( getline_data *fd )
{
    fclose( fd->fp );
    MemFree( fd->buffer );
}

static void ParseOneLineWlib( const char *c )
{
    char        token_buff[MAX_TOKEN_LEN];
    const char  *start;

    for( ;; ) {
        eatwhite( c );
        start = c;
        switch( *c ) {
#if !defined(__UNIX__)
        case '/':
            c = ParseOption( c, token_buff );
            if( c == start )
                FatalError( ERR_BAD_OPTION, c[1] );
            break;
#endif

        case '-':
            if( CmdList == NULL && Options.input_name == NULL ) {
                c = ParseOption( c, token_buff );
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
            c = GetString( c, token_buff, true, false );
            {
                const char *env;

                env = WlibGetEnv( token_buff );
                if( env != NULL ) {
                    ParseOneLineWlib( env );
                } else {
                    getline_data    fd;
                    char            *p;

                    DefaultExtension( token_buff, EXT_CMD );
                    my_getline_init( token_buff, &fd );
                    while( (p = my_getline( &fd )) != NULL ) {
                        ParseOneLineWlib( p );
                    }
                    my_getline_fini( &fd );
                }
            }
            break;
        case '\0':
        case '#':
            // comment - blow away line
            return;
        default:
            c = GetString( c, token_buff, true, false );
            if( strcmp( token_buff, "?" ) == 0 ) {
                Usage();
            }
            if( Options.input_name != NULL ) {
                AddCommand( OP_ADD|OP_DELETE, token_buff );
            } else {
                DefaultExtension( token_buff, EXT_LIB );
                Options.input_name = DupStr( token_buff );
            }
            break;
        }
    }
}

static const char *ParseArOption( const char *c, operation *ar_mode )
{
    const char  *start = c;

    while( notwhite( *c ) ) {
        switch( my_tolower( *c ) ) {
        case '?':
            Usage();
            break;
        case 'c':
            if( Options.no_c_warn ) {
                DuplicateOption( start );
            }
            Options.no_c_warn = true;
            break;
        case 'd':
            if( *ar_mode != OP_NONE ) {
                FatalError( ERR_BAD_OPTION, c[0] );
            }
            *ar_mode = OP_DELETE;
            break;
        case 'p':
            //ignore not implemented
            break;
        case 'r':
            if( *ar_mode != OP_NONE ) {
                FatalError( ERR_BAD_OPTION, c[0] );
            }
            *ar_mode = OP_ADD | OP_DELETE;
            break;
        case 't':
            if( *ar_mode != OP_NONE ) {
                FatalError( ERR_BAD_OPTION, c[0] );
            }
            *ar_mode = OP_TABLE;
            Options.list_contents = true;
            break;
        case 'u':
            if( Options.update ) {
                DuplicateOption( start );
            }
            Options.update = true;
            break;
        case 'v':
            if( Options.verbose ) {
                DuplicateOption( start );
            }
            Options.verbose = true;
            break;
        case 'x':
            if( *ar_mode != OP_NONE ) {
                FatalError( ERR_BAD_OPTION, c[0] );
            }
            *ar_mode = OP_EXTRACT;
            break;
        case '-':
            break;
        default:
            FatalError( ERR_BAD_OPTION, c[0] );
        }
        c++;
    }
    return( c );
}

static void ParseOneLineAr( const char *c, operation *ar_mode )
{
    char        token_buff[MAX_TOKEN_LEN];
    bool        done_options;

    done_options = false;
    for( ;; ) {
        eatwhite( c );
        switch( *c ) {
        case '\0':
            if( *ar_mode == OP_EXTRACT ) {
                Options.explode = true;
            }
            return;
        case '-':
            if( !done_options ) {
                if( *(c + 1) == '-' ) {
                    c += 2;
                    done_options = true;
                } else {
                    c = ParseArOption( c, ar_mode );
                }
                break;
            }
            //fall to default
        default:
            if( *ar_mode == OP_NONE ) {
                c = ParseArOption( c, ar_mode );
                break;
            }
            c = GetString( c, token_buff, true, false );
            if( Options.input_name != NULL ) {
                AddCommand( *ar_mode, token_buff );
                break;
            } else {
                DefaultExtension( token_buff, EXT_LIB );
                Options.input_name = DupStr( token_buff );
            }
            break;
        }
    }
}

static const char *getWlibModeInfo( void )
{
    const char  *env;
    char        *s;

    env = WlibGetEnv( AR_MODE_ENV );
    if( env != NULL ) {
        Options.ar = true;
        Options.ar_name = DupStr( env );
        for( s = Options.ar_name; *s != '\0'; s++ ) {
            *s = tolower( *(unsigned char *)s );
        }
        env = WlibGetEnv( env );
    } else {
        env = WlibGetEnv( "WLIB" );
    }
    return( env );
}

void ProcessCmdLine( char *argv[] )
{
    char        *parse;
    const char  *env;
    lib_cmd     *cmd;
    operation   ar_mode;

    env = getWlibModeInfo();

    if( ( env == NULL || *env == '\0' ) && ( argv[1] == NULL || *argv[1] == '\0' ) ) {
        Usage();
    }

    ar_mode = OP_NONE;
    if( env != NULL && *env != '\0' ) {
        parse = DupStr( env );
        if( Options.ar ) {
            ParseOneLineAr( parse, &ar_mode );
        } else {
            ParseOneLineWlib( parse );
        }
        MemFree( parse );
    }
    argv++;
    while( *argv != NULL ) {
        if( **argv != '\0' ) {
            parse = DupStr( *argv );
            if( Options.ar ) {
                ParseOneLineAr( parse, &ar_mode );
            } else {
                ParseOneLineWlib( parse );
            }
            MemFree( parse );
        }
        argv++;
    }
    if( Options.ar && CmdList != NULL && Options.explode ) {
        Options.explode = false;
    } else if( CmdList == NULL && !Options.list_contents && !Options.explode && !Options.new_library ) {
        /* Default action: List the input lib */
        if( Options.output_name == NULL ) {
            Options.list_contents = true;
            Options.list_file = DupStr( "" );
        } else { /* Or copy it to the output lib */
            Options.modified = true;
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
        Options.new_library = true;
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

void FiniCmdLine( void )
{
    if( Options.output_directory != NULL ) {
        MemFree( Options.output_directory );
        Options.output_directory = NULL;
    }
    if( Options.list_file != NULL ) {
        MemFree( Options.list_file );
        Options.list_file = NULL;
    }
    if( Options.output_name != NULL ) {
        MemFree( Options.output_name );
        Options.output_name = NULL;
    }
    if( Options.input_name != NULL ) {
        MemFree( Options.input_name );
        Options.input_name = NULL;
    }
    if( Options.ar_name != NULL ) {
        MemFree( Options.ar_name );
        Options.ar_name = NULL;
    }
    FreeCommands();
}
