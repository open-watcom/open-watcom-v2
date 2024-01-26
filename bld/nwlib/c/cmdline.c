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
#include "cmdline.h"


#define AR_MODE_ENV     "WLIB_AR"

options_def         Options;
lib_cmd             *CmdList;

static lib_cmd      **CmdListEnd;
static const char   *cmd_ptr = NULL;

const char *CmdSetPos( const char *new_cmd )
{
    const char  *old_cmd;

    old_cmd = cmd_ptr;
    cmd_ptr = new_cmd;
    return( old_cmd );
}

const char *CmdGetPos( void )
{
    return( cmd_ptr );
}

int CmdPeekChar( void )
{
    return( *(unsigned char *)cmd_ptr );
}

int CmdPeekLowerChar( void )
{
    return( tolower( *(unsigned char *)cmd_ptr ) );
}

int CmdGetChar( void )
{
    return( *(unsigned char *)cmd_ptr++ );
}

int CmdGetLowerChar( void )
{
    return( tolower( *(unsigned char *)cmd_ptr++ ) );
}

bool CmdRecogChar( int ch )
{
    if( *(unsigned char *)cmd_ptr == ch ) {
        ++cmd_ptr;
        return( true );
    }
    return( false );
}

bool CmdRecogLowerChar( int ch )
{
    if( tolower( *(unsigned char *)cmd_ptr ) == ch ) {
        ++cmd_ptr;
        return( true );
    }
    return( false );
}

void CmdUngetChar( void )
{
    --cmd_ptr;
}

void CmdSkipWhite( void )
{
    while( isspace( *(unsigned char *)cmd_ptr ) ) {
        ++cmd_ptr;
    }
}

void CmdSkipEqual( void )
{
    const char  *start;

    start = cmd_ptr;
    CmdSkipWhite();
    if( CmdRecogChar( '=' ) ) {
        CmdSkipWhite();
    } else {
        cmd_ptr = start;
    }
}

static size_t checkExt( const char *fname, size_t len, const char *ext )
{
    bool        has_ext;
//    bool        has_path;
    size_t      i;

    if( fname == NULL || len == 0 || ext == NULL || *ext == '\0' )
        return( 0 );
    has_ext = false;
//    has_path = false;
    for( i = 0; i < len; i++ ) {
        switch( fname[i] ) {
        case '\\':
        case '/':
        case ':':
//            has_path = true;
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

static size_t cmdScanString( const char **start, scan_ctrl sctrl )
{
    int         quote;
    int         ch;
    unsigned    offset;

    if( sctrl == SCTRL_EQUAL ) {
        CmdSkipEqual();
    } else {
        CmdSkipWhite();
    }
    quote = *(unsigned char *)cmd_ptr;
    offset = ( ( quote == '\"' ) || ( sctrl == SCTRL_SINGLE ) && ( quote == '\'' ) ) ? 1 : 0;
    *start = cmd_ptr = cmd_ptr + offset;
    if( offset > 0 ) {
        for( ; (ch = *(unsigned char *)cmd_ptr) != '\0'; cmd_ptr++ ) {
            if( ch == quote ) {
                cmd_ptr++;
                break;
            }
        }
    } else if( sctrl == SCTRL_IMPORT ){
        bool inquote = false;

        for( ; (ch = *(unsigned char *)cmd_ptr) != '\0'; cmd_ptr++ ) {
            if( !inquote && isspace( ch ) ) {
                break;
            }
            if( ( ch == '\"' ) || ( ch == '\'' ) ) {
                inquote = !inquote;
            }
        }
    } else {
        for( ; (ch = *(unsigned char *)cmd_ptr) != '\0'; cmd_ptr++ ) {
            if( isspace( ch ) ) {
                break;
            }
        }
    }
    return( cmd_ptr - *start - offset );
}

char *GetString( scan_ctrl sctrl )
{
    const char  *src;
    size_t      len;
    char        *dst;

    dst = NULL;
    len = cmdScanString( &src, sctrl );
    if( len > 0 ) {
        dst = MemAlloc( len + 1 );
        strncpy( dst, src, len );
        dst[len] = '\0';
    }
    return( dst );
}

char *GetFilenameExt( scan_ctrl sctrl, const char *ext )
{
    const char  *src;
    size_t      len;
    size_t      len2;
    char        *dst;

    dst = NULL;
    len = cmdScanString( &src, sctrl );
    if( len > 0 ) {
        len2 = checkExt( src, len, ext );
        dst = MemAlloc( len + len2 + 1 );
        strncpy( dst, src, len );
        if( len2 > 0 ) {
            dst[len] = '.';
            strncpy( dst + len + 1, ext, len2 - 1 );
        }
        dst[len + len2] = '\0';
    }
    return( dst );
}

void AddCommand( operation ops, scan_ctrl sctrl )
{
    lib_cmd         *cmd;
    const char      *src;
    size_t          len;

    len = cmdScanString( &src, sctrl );
    if( len > 0 ) {
        cmd = MemAllocGlobal( sizeof( lib_cmd ) + len );
        strncpy( cmd->name, src, len );
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
        cmd->next = *CmdListEnd;
        *CmdListEnd = cmd;
        CmdListEnd = &cmd->next;
    }
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
        if( !Options.no_create_warn ) {
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
    if( Options.explode_ext != NULL ) {
        MemFree( Options.explode_ext );
        Options.explode_ext = NULL;
    }
    FreeCommands();
}
