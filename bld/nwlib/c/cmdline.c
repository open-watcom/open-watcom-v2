/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
#include "cmdline.h"
#include "cmdlnprs.h"


#define AR_MODE_ENV     "WLIB_AR"

options_def         Options;
lib_cmd             *CmdList;
const char          *option_start = "";

static lib_cmd      **CmdListEnd;

static void BadCmdLine( int error_code )
/***************************************
 * SIGNAL CMD-LINE ERROR
 */
{
    FatalError( error_code, option_start );
}

/* BAD CHAR DETECTED */
void BadCmdLineChar( void )
{
    BadCmdLine( ERR_BAD_CMDLINE );
}
/* BAD ID DETECTED */
void BadCmdLineId( void )
{
    BadCmdLine( ERR_BAD_CMDLINE );
}
/* BAD NUMBER DETECTED */
void BadCmdLineNumber( void )
{
    BadCmdLine( ERR_BAD_CMDLINE );
}
/* BAD PATH DETECTED */
void BadCmdLinePath( void )
{
    BadCmdLine( ERR_BAD_CMDLINE );
}
/* BAD FILE DETECTED */
void BadCmdLineFile( void )
{
    BadCmdLine( ERR_BAD_CMDLINE );
}
/* BAD TEXT DETECTED */
void BadCmdLineOption( void )
{
    BadCmdLine( ERR_BAD_CMDLINE );
}

static size_t checkExt( const char *fname, size_t len, const char *ext )
{
    bool        has_ext;
    size_t      i;

    if( fname == NULL
      || len == 0
      || ext == NULL
      || *ext == '\0' ) {
        return( 0 );
    }
    has_ext = false;
    for( i = 0; i < len; i++ ) {
        switch( fname[i] ) {
        case '\\':
        case '/':
        case ':':
            has_ext = false;
            break;
        case '.':
            has_ext = true;
            break;
        }
    }
    if( has_ext )
        return( 0 );
    return( strlen( ext ) + 1 );
}

char *CopyFilenameExt( OPT_STRING *src, const char *ext )
{
    size_t      len;
    size_t      len2;
    char        *dst;

    dst = NULL;
    if( src != NULL ) {
        len = strlen( src->data );
        if( len > 0 ) {
            len2 = checkExt( src->data, len, ext );
            dst = MemAlloc( len + len2 + 1 );
            strncpy( dst, src->data, len );
            if( len2 > 0 ) {
                dst[len] = '.';
                strncpy( dst + len + 1, ext, len2 - 1 );
            }
            dst[len + len2] = '\0';
        }
    }
    return( dst );
}

char *GetFilenameExt( const char *ext )
{
    char        *dst;
    OPT_STRING  *src;

    src = NULL;
    OPT_GET_FILE( &src );
    dst = CopyFilenameExt( src, ext );
    OPT_CLEAN_STRING( &src );
    return( dst );
}

char *CopyFilename( OPT_STRING *src )
{
    size_t      len;
    char        *dst;

    dst = NULL;
    if( src != NULL ) {
        len = strlen( src->data );
        if( len > 0 ) {
            dst = MemAlloc( len + 1 );
            strncpy( dst, src->data, len );
            dst[len] = '\0';
        }
    }
    return( dst );
}

char *GetFilename( void )
{
    char        *dst;
    OPT_STRING  *src;

    src = NULL;
    OPT_GET_FILE( &src );
    dst = CopyFilename( src );
    OPT_CLEAN_STRING( &src );
    return( dst );
}

void AddCommand( operation ops )
{
    lib_cmd         *cmd;
    OPT_STRING      *src;
    size_t          len;

    src = NULL;
    OPT_GET_FILE( &src );
    if( src != NULL ) {
        len = strlen( src->data );
        if( len > 0 ) {
            cmd = MemAlloc( sizeof( lib_cmd ) + len );
            strncpy( cmd->name, src->data, len );
            cmd->name[len] = '\0';
            cmd->fname = NULL;
            if( ops == OP_EXTRACT ) {
                char    *p;

                p = strchr( cmd->name, '=' );
                if( p != NULL ) {
                    *p = '\0';
                    cmd->fname = p + 1;
                }
            }
            cmd->ops = ops;
            cmd->next = NULL;
            *CmdListEnd = cmd;
            CmdListEnd = &cmd->next;
        }
    }
    OPT_CLEAN_STRING( &src );
}

static void FreeCommands( void )
{
    lib_cmd     *cmd;

    while( (cmd = CmdList) != NULL ) {
        CmdList = cmd->next;
        MemFree( cmd );
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
        Options.ar_name = MemStrdup( env );
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
    const char      *p;
    lib_cmd         *cmd;
    OPT_STORAGE_A   dataa;
    OPT_STORAGE_W   dataw;

    p = getWlibModeInfo();

    if( ( p == NULL
      || *p == '\0' )
      && ( argv[1] == NULL
      || *argv[1] == '\0' ) ) {
        Usage();
    }

    if( Options.ar ) {
        OPT_INIT_A( &dataa );
    } else {
        OPT_INIT_W( &dataw );
    }

    if( p != NULL
      && *p != '\0' ) {
        if( Options.ar ) {
            ParseOneLineAr( p, &dataa, false );
        } else {
            ParseOneLineWlib( p, &dataw, false );
        }
    }
    for( argv++; (p = *argv) != NULL; argv++ ) {
        if( *p != '\0' ) {
            if( Options.ar ) {
                ParseOneLineAr( p, &dataa, false );
            } else {
                ParseOneLineWlib( p, &dataw, false );
            }
        }
    }

    if( Options.ar ) {
        SetOptionsAr( &dataa );
        OPT_FINI_A( &dataa );
    } else {
        SetOptionsWlib( &dataw );
        OPT_FINI_W( &dataw );
    }

    if( !Options.ar ) {
        Banner( false );
    }
    if( Options.input_name == NULL ) {
        FatalError( ERR_NO_LIBNAME );
    }
    if( access( Options.input_name, F_OK ) != 0
      && !Options.new_library ) {
        if( Options.ar ) {
            if( !Options.no_create_warn ) {
                Warning( ERR_CREATING_LIBRARY, Options.input_name );
            }
        } else {
            if( !Options.quiet ) {
                Warning( ERR_CREATING_LIBRARY, Options.input_name );
            }
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
    if( Options.explode_ext != NULL ) {
        MemFree( Options.explode_ext );
        Options.explode_ext = NULL;
    }
    FreeCommands();
}
