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


#include <wlib.h>

#define eatwhite( c ) while( *(c) && isspace( *(c) ) ) ++(c);
#define notwhite( c ) ( (c) != '\0' && !isspace( c ) )

options_def     Options;
lib_cmd         *CmdList;

void GetString( char **pc, char *buff, int singlequote, int ignoreSpaceInQuotes )
{
    char *c = *pc;
    char  quote;

    eatwhite(c);

    if ((*c == '\"') || (singlequote && (*c == '\''))) {
        quote = *c;
        c++;
        while ((*c != '\0') && (*c != quote)) {
            *buff++ = *c++;
        }
        if (*c == quote) {
            c++;
        }
    } else {
        int inquote = FALSE;

        while( inquote || notwhite( *c ) ) {
            if (ignoreSpaceInQuotes) {
                if (*c == 0x00) {
                    break;
                } else if ((*c == '\"') || (*c == '\'')) {
                    inquote = !inquote;
                }
            }
            *buff++ = *c++;
        }
    }
    *buff = '\0';
    *pc = c;
}

char *GetEqual( char **pc, char *buff, char *ext )
{
    char *c = *pc;
    char *ret;

    eatwhite( c );
    if( *c == '=' ) {
        ++c;
        eatwhite( c );
    } else {
        c = *pc;
    }
    if( *c == ' ' || *c == '\0' ) return( NULL );
    GetString( &c, buff, FALSE, FALSE );
    if( ext != NULL ) {
        DefaultExtension( buff, ext );
    }
    ret = DupStr( buff );
    *pc = c;
    return( ret );
}
static void SetPageSize(unsigned short new_size)
{
    unsigned int i;
    Options.page_size = MIN_PAGE_SIZE;
    for( i = 4; i < 16; i++ ){
        if( new_size & 1<<i ){
            Options.page_size = 1<<i;
        }
    }
    if( Options.page_size < new_size ){
        Options.page_size <<= 1;
    }
}

static void DuplicateOption( char *c )
{
    FatalError( ERR_DUPLICATE_OPTION, c );
}

static bool ParseOption( char **pc, char *buff )
{
    bool        rc;
    long        page_size;
    char        *c = *pc;
    char        *start = c;
    char        *page;
    char        *endptr;

    rc = TRUE;
    switch( *c++ ) {
    case '-':
    case '/':
        eatwhite( c );
        switch( tolower( *c++ ) ) {
        case '?': //                       (don't create .bak file)
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
            Options.output_directory = GetEqual( &c, buff, NULL );
            if( access( Options.output_directory, F_OK ) != 0 ) {
                FatalError( ERR_DIR_NOT_EXIST, Options.output_directory );
            }
            break;
        case 'i':
            switch( tolower(*c++) ) {
                case 'n':
                    switch( tolower(*c++) ) {
                        case 'n':
                            Options.nr_ordinal = FALSE;
                            break;
                        case 'o':
                            Options.nr_ordinal = TRUE;
                            break;
                        default:
                            return( FALSE );
                    }
                    break;
                case 'r':
                    switch( tolower(*c++) ) {
                        case 'n':
                            Options.r_ordinal = FALSE;
                            break;
                        case 'o':
                            Options.r_ordinal = TRUE;
                            break;
                        default:
                            return( FALSE );
                    }
                    break;
                case 'a':
                    if( Options.processor ) {
                        DuplicateOption( start );
                    }
                    Options.processor = WL_PROC_AXP;
                    break;
                case 'p':
                    if( Options.processor ) {
                        DuplicateOption( start );
                    }
                    Options.processor = WL_PROC_PPC;
                    break;
                case 'i':
                    if( Options.processor ) {
                        DuplicateOption( start );
                    }
                    Options.processor = WL_PROC_X86;
                    break;
                case 'e':
                    if( Options.filetype ) {
                        DuplicateOption( start );
                    }
                    Options.filetype = WL_TYPE_ELF;
                    break;
                case 'c':
                    if( Options.filetype ) {
                        DuplicateOption( start );
                    }
                    Options.filetype = WL_TYPE_COFF;
                    break;
                case 'o':
                    if( Options.filetype ) {
                        DuplicateOption( start );
                    }
                    Options.filetype = WL_TYPE_OMF;
                    break;
                default:
                    return ( FALSE );
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
            Options.list_file = GetEqual( &c, buff, EXT_LST );
            break;
        case 'm': //                       (display C++ mangled names)
            Options.mangled = 1;
            break;
        case 'o': // = <out_library_name>
            if( Options.output_name ) {
                DuplicateOption( start );
            }
            Options.output_name = GetEqual( &c, buff, EXT_LIB );
            break;
        case 'q': //                       (don't print header)
            Options.quiet = 1;
            break;
        case 'v': //                       (don't print header)
            Options.quiet = 0;
            break;
        case 'x': //                       (explode all objects in library)
            Options.explode = 1;
            break;
        case 'z':
            if( Options.strip_expdef ) {
                DuplicateOption( start );
            }
            Options.strip_expdef = 1;       // JBS 99/07/09
            Options.export_list_file = GetEqual( &c, buff, NULL );
            break;
        case 't':
            if (*c == 'l') {
                ++c;
                Options.list_contents = 1;
                Options.terse_listing = 1; // (internal terse listing option)
            } else {
                Options.trim_path = 1; //(trim THEADR pathnames)
            }
            break;
        case 'f':
            switch( tolower(*c++) ) {
                case 'm':
                    if( Options.libtype ) {
                        DuplicateOption( start );
                    }
                    Options.libtype = WL_TYPE_MLIB;
                    break;
                case 'a':
                    if( Options.libtype ) {
                        DuplicateOption( start );
                    }
                    Options.libtype = WL_TYPE_AR;
                    break;
                case 'o':
                    if( Options.libtype ) {
                        DuplicateOption( start );
                    }
                    Options.libtype = WL_TYPE_OMF;
                    break;
                default:
                    return ( FALSE );
            }
            break;
    // following only used by OMF libary format
        case 'p':
            if( Options.page_size ) {
                DuplicateOption( start );
            }
            page = GetEqual( &c, buff, NULL );
            errno = 0;
            page_size = strtoul( page, &endptr, 0 );
            if( *endptr != '\0' ){
                FatalError( ERR_BAD_CMDLINE, start );
            } else if ( errno == ERANGE || page_size > MAX_PAGE_SIZE ) {
                FatalError( ERR_PAGE_RANGE );
            }
            MemFree( page );
            SetPageSize( page_size );
            break;
        case 'n': //                       (always create a new library)
            Options.new_library = 1;
            break;
        case 's':
            Options.strip_line = 1;
            break;
        default:
            return( FALSE );
        }
        break;
    default:
        return( FALSE );
    }
    *pc = c;
    return( TRUE );
}

void AddCommand( operation ops, char *name )
{
    lib_cmd     *new;

    new = MemAllocGlobal( sizeof( *new ) + strlen( name ) );
    strcpy( new->name, name );
    new->next = CmdList;
    new->ops = ops;
    CmdList = new;

}

static void FreeCommands()
{
    lib_cmd     *cmd, *next;

    for( cmd = CmdList; cmd != NULL; cmd = next ) {
        next = cmd->next;
        MemFreeGlobal( cmd );
    }
}

static void ParseCommand( char **pc )
{
    int          doquotes = TRUE;
    int          ignoreSpacesInQuotes = FALSE;
    char        *c = *pc;
    char        *start;
    operation   ops = 0;
    //char        buff[_MAX_PATH];
    char        buff[MAX_IMPORT_STRING];

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
#if defined(__QNX__)
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
#if defined(__QNX__)
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
    GetString( &c, buff, doquotes, ignoreSpacesInQuotes );
    AddCommand( ops, buff );
    *pc = c;
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
#if !defined(__QNX__)
        case '/':
            if( !ParseOption( &c, buff ) ) {
                FatalError( ERR_BAD_OPTION, c[1] );
            }
            break;
#endif

#if !defined(__QNX__)
        case '*':
#else
        case ':':
#endif
        case '+':
        case '-':
            if( CmdList != NULL ||
                Options.input_name != NULL ||
                !ParseOption( &c, buff ) ) {
                ParseCommand( &c );
            }
            break;
        case '@':
            ++c;
            GetString( &c, buff, TRUE, FALSE );
            {
                char *env = getenv(buff);
                if (env) {
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
            GetString( &c, buff, TRUE, FALSE );
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

static void ParseArOption( char **init_c, operation *mode )
{
    char        *c;

    c = *init_c;
    while( *c != '\0' && !isspace( *c ) ) {
        switch( tolower( *c ) ) {
        case 'c':
            if( Options.no_c_warn ) {
                DuplicateOption( *init_c );
            }
            Options.no_c_warn = TRUE;
            break;
        case 'd':
            if( *mode != OP_NONE ) {
                FatalError( ERR_BAD_OPTION, c[0] );
            }
            *mode = OP_DELETE;
            break;
        case 'p':
            //ignore not implemented
            break;
        case 'r':
            if( *mode != OP_NONE ) {
                FatalError( ERR_BAD_OPTION, c[0] );
            }
            *mode = OP_ADD | OP_DELETE;
            break;
        case 't':
            if( *mode != OP_NONE ) {
                FatalError( ERR_BAD_OPTION, c[0] );
            }
            *mode = OP_TABLE;
            Options.list_contents = 1;
            break;
        case 'u':
            if( Options.update ) {
                DuplicateOption( *init_c );
            }
            Options.update = TRUE;
            break;
        case 'v':
            if( Options.verbose ) {
                DuplicateOption( *init_c );
            }
            Options.verbose = TRUE;
            break;
        case 'x':
            if( *mode != OP_NONE ) {
                FatalError( ERR_BAD_OPTION, c[0] );
            }
            *mode = OP_EXTRACT;
            break;
        case '-':
            break;
        default:
            FatalError( ERR_BAD_OPTION, c[0] );
        }
        c++;
    }
    *init_c = c;
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
                    c+=2;
                    done_options = TRUE;
                } else {
                    ParseArOption( &c, &mode );
                }
            break;
            }
            //fall to default
        default:
            if( mode == OP_NONE ) {
                ParseArOption( &c, &mode );
                break;
            }
            GetString( &c, buff, TRUE, FALSE );
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

    if( FNCMP( MakeFName( ImageName ), "ar" ) == 0 ) {
        Options.ar = TRUE;
    }
    if( Options.ar ) {
        env = WlibGetEnv( "AR" );
    } else {
        env = WlibGetEnv( "WLIB" );
    }
    if( env != NULL ) {
        parse = DupStr( env );
        if( Options.ar ) {
            ParseOneLine( parse );
        } else {
            ParseOneArLine( parse );
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

    Banner();
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

void InitCmdLine()
{
    CmdList = NULL;
    memset( &Options, 0, sizeof( Options ) );
}

void ResetCmdLine()
{
    MemFree( Options.output_directory );
    MemFree( Options.list_file );
    MemFree( Options.output_name );
    MemFree( Options.input_name );
    FreeCommands();
    InitCmdLine();
}
