/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
#ifdef __WATCOMC__
#include <process.h>
#endif
#include "wio.h"
#include "cmdlinea.h"
#include "cmdlinew.h"


#define AR_MODE_ENV     "WLIB_AR"
#define MAX_TOKEN_LEN   260

#define eatwhite( c ) while( *(c) != '\0' && isspace( *(unsigned char *)(c) ) ) ++(c);
#define notwhite( c ) ( (c) != '\0' && !isspace( (unsigned char)(c) ) )

options_def     Options;
lib_cmd         *CmdList;

static lib_cmd  **CmdListEnd;

const char *GetString( const char *c, char *token_buff, bool singlequote, bool ignoreSpaceInQuotes )
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

const char *GetFilenameExt( const char *c, bool equal, char *token_buff, const char *ext, char **ret )
{
    const char  *start = c;

    eatwhite( c );
    if( equal && *c == '=' ) {
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

void AddCommand( operation ops, const char *name )
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
    const char  *p;
    lib_cmd     *cmd;
    operation   ar_mode;

    p = getWlibModeInfo();

    if( ( p == NULL || *p == '\0' ) && ( argv[1] == NULL || *argv[1] == '\0' ) ) {
        Usage();
    }

    ar_mode = OP_NONE;
    if( p != NULL && *p != '\0' ) {
        if( Options.ar ) {
            ParseOneLineAr( p, &ar_mode );
        } else {
            ParseOneLineWlib( p );
        }
    }
    for( argv++; (p = *argv) != NULL; argv++ ) {
        if( *p != '\0' ) {
            if( Options.ar ) {
                ParseOneLineAr( p, &ar_mode );
            } else {
                ParseOneLineWlib( p );
            }
        }
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
