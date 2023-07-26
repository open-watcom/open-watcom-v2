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
* Description:  Standalone disassembler command line processing.
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#if defined( __WATCOMC__ )
    #include <process.h>
#endif
#include "wio.h"
#include "dis.h"
#include "global.h"
#include "banner.h"
#include "args.h"
#include "buffer.h"
#include "memfuncs.h"
#include "print.h"
#include "pathgrp2.h"

#include "clibext.h"


#if defined( __UNIX__ )
    #define OBJ_FILE_EXTENSION  "o"
    #define IS_OPT_DELIM( c )   ((c) == '-')
#else
    #define OBJ_FILE_EXTENSION  "obj"
    #define IS_OPT_DELIM( c )   ((c)=='-' || (c) == '/')
#endif

#define LIST_FILE_EXTENSION     "lst"

enum {
    MSG_USAGE_COUNT = 0
    #define pick(n,e,j)     + 1
        #include "usage.gh"
    #undef pick
};

static void printUsage( int msg )
{
    int                 id;
    char                buff[MAX_RESOURCE_SIZE];

    if( msg != 0 ) {
        MsgGet( msg, buff );
        fputs( buff, stderr );
    }
    puts(
        banner1t( "Multi-processor Disassembler" ) "\n"
        banner1v( _WDISASM_VERSION_ ) "\n"
        banner2 "\n"
        banner2a( 1995 ) "\n"
        banner3 "\n"
        banner3a "\n"
    );
    for( id = MSG_USAGE_BASE; id < MSG_USAGE_BASE + MSG_USAGE_COUNT; id++ ) {
        MsgGet( id, buff );
        puts( buff );
    }
}

static bool is_ws( char ch )
{
    return( isspace( ch ) != 0 );
}

static bool is_ws_or_option( char ch )
{
    return( isspace( ch ) || IS_OPT_DELIM( ch ) );
}

static char *skipBlanks( const char *cmd )
{
    while( isspace( *cmd ) ) {
        cmd++;
    }
    return( (char *)cmd );
}

static char *FindNextSep( const char *str, bool (*chk_sep)(char) )
/*****************************************************************
 * Finds next free white space character, allowing doublequotes to
 * be used to specify strings with white spaces.
 */
{
    bool        string_open;
    char        c;

    string_open = false;
    while( (c = *str) != '\0' ) {
        if( c == '\"' ) {
            string_open = !string_open;
        } else if( c == '\\' ) {
            if( string_open ) {
                if( str[1] == '\"' || str[1] == '\\' ) {
                    ++str;
                }
            }
        } else if( !string_open && chk_sep( c ) ) {
            break;
        }
        ++str;
    }

    return( (char *)str );
}

static char *findNextWS( const char *cmd )
{
    return( FindNextSep( cmd, is_ws ) );
}

static char *findNextArg( const char *cmd )
{
    return( skipBlanks( FindNextSep( cmd, is_ws_or_option ) ) );
}

static void UnquoteItem( char *dst, size_t maxlen, const char *src, bool (*chk_sep)(char) )
/******************************************************************************************
 * Removes doublequote characters from filename and copies other content
 * from src to dst. Only maxlen number of characters are copied to dst
 * including terminating NUL character. Returns value 1 when quotes was
 * removed from orginal filename, 0 otherwise.
 */
{
    size_t  pos;
    char    c;
    bool    string_open;

    // leave space for NUL terminator
    maxlen--;
    pos = 0;
    string_open = false;
    while( pos < maxlen && (c = *src++) != '\0' ) {
        if( c == '\"' ) {
            string_open = !string_open;
            continue;
        }
        if( c == '\\' ) {
            if( string_open ) {
                if( *src == '\"' || *src == '\\' ) {
                    c = *src++;
                }
            }
        } else if( !string_open && chk_sep( c ) ) {
            break;
        }
        *dst++ = c;
        pos++;
    }
    *dst = '\0';
}

static char *getFileName( const char *start, const char *following )
{
    size_t      length;
    char        *name;
    char        *tmp;

    length = following - start;
    tmp = (char *)MemAlloc( length + 1 );
    memcpy( tmp, start, length );
    tmp[length] = 0;

    if( strchr( tmp, '\"' ) == NULL )
        return tmp;

    name = (char *)MemAlloc( length + 1 );
    UnquoteItem( name, length + 1, tmp, is_ws );
    MemFree( tmp );

    return( name );
}

static void composeFileNames( bool list_file )
{
    pgroup2     pg;
    size_t      length;

    // check extension
    _splitpath2( ObjFileName, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    if( pg.ext[0] == '\0' ) {
        length = strlen( ObjFileName ) + 1 + strlen( OBJ_FILE_EXTENSION ) + 1;
        MemFree( ObjFileName );
        ObjFileName = (char *)MemAlloc( length );
        _makepath( ObjFileName, pg.drive, pg.dir, pg.fname, OBJ_FILE_EXTENSION );
    }
    if( list_file ) {
        if( ListFileName == NULL ) {
            // create list file name
            length = strlen( pg.drive ) + strlen( pg.dir ) + strlen( pg.fname ) + 1 + strlen( LIST_FILE_EXTENSION ) + 1;
            ListFileName = (char *)MemAlloc( length );
            _makepath( ListFileName, pg.drive, pg.dir, pg.fname, LIST_FILE_EXTENSION );
        } else {
            // check extension
            _splitpath2( ListFileName, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
            if( pg.ext[0] == '\0' ) {
                length = strlen( ListFileName ) + 1 + strlen( LIST_FILE_EXTENSION ) + 1;
                MemFree( ListFileName );
                ListFileName = (char *)MemAlloc( length );
                _makepath( ListFileName, pg.drive, pg.dir, pg.fname, LIST_FILE_EXTENSION );
            }
        }
    }
}

return_val HandleArgs( void )
{
    const char  *ptr;
    bool        list_file = false;
    int         cmd_len;
    char        *cmd_line;
    char        *cmd;
    return_val  error;

    DFormat |= DFF_PSEUDO | DFF_SYMBOLIC_REG;

    cmd_len = _bgetcmd( NULL, 0 ) + 1;
    cmd_line = malloc( cmd_len );
    if( cmd_line == NULL ) {
        return( RC_OUT_OF_MEMORY );
    }
    _bgetcmd( cmd_line, cmd_len );
    cmd = cmd_line;
    cmd = skipBlanks( cmd );
    if( *cmd == '\0' || *cmd == '?' ) {
        printUsage( 0 );
        error = RC_ERROR;
    } else {
        error = RC_OKAY;
        while( *cmd != '\0' ) {
            if( IS_OPT_DELIM( *cmd ) ) {
                cmd++;
                switch( tolower( *cmd ) ) {
                case 'a':
                    DFormat |= DFF_ASM;
                    if( cmd[1] == 'u' ) {
                        ++cmd;
                        DFormat |= DFF_UNIX;
                    }
                    break;
                case 'e':
                    Options |= PRINT_EXTERNS;
                    break;
                case 'i':
                    cmd++;
                    if( *cmd == '=' ) {
                        cmd++;
                        if( !isspace( *cmd ) ) {
                            LabelChar = (char)toupper( *(unsigned char *)cmd );
                            break;
                        }
                    }
                    printUsage( INVALID_I );
                    error = RC_ERROR;
                    break;
                case 'l':
                    if( ListFileName != NULL ) {
                        printUsage( ONLY_ONE_LISTING );
                        error = RC_ERROR;
                        break;
                    }
                    list_file = true;
                    cmd++;
                    if( *cmd == '=' ) {
                        cmd++;
                        ptr = cmd;
                        cmd = findNextWS( cmd );
                        ListFileName = getFileName( ptr, cmd );
                    }
                    break;
                case 'f':
                    switch( cmd[1] ) {
                    case 'p':
                        DFormat ^= DFF_PSEUDO;
                        ++cmd;
                        break;
                    case 'r':
                        DFormat ^= DFF_SYMBOLIC_REG;
                        ++cmd;
                        break;
                    case 'f':
                        Options |= PRINT_FPU_EMU_FIXUP;
                        break;
                    case 'i':
                        DFormat ^= DFF_ALT_INDEXING;
                        ++cmd;
                        break;
                    case 'u':
                        ++cmd;
                        switch( cmd[1] ) {
                        case 'r':
                            DFormat ^= DFF_REG_UP;
                            ++cmd;
                            break;
                        case 'i':
                            DFormat ^= DFF_INS_UP;
                            ++cmd;
                            break;
                        default:
                            DFormat ^= DFF_INS_UP | DFF_REG_UP;
                        }
                        break;
                    }
                    break;
                case 'm':
                    if( cmd[1] == 'w' ) {
                        Options |= METAWARE_COMPATIBLE;
                        ++cmd;
                        break;
                    }
                    Options |= NODEMANGLE_NAMES;
                    break;
                case 'p':
                    Options |= PRINT_PUBLICS;
                    break;
                case 's':
                    if( SourceFileName != NULL ) {
                        printUsage( ONLY_ONE_SOURCE );
                        error = RC_ERROR;
                        break;
                    }
                    source_mix = true;
                    cmd++;
                    if( *cmd == '=' ) {
                        cmd++;
                        ptr = cmd;
                        cmd = findNextWS( cmd );
                        SourceFileName = getFileName( ptr, cmd );
                    }
                    break;
                default:
                    BufferMsg( INVALID_OPTION );
                    BufferConcat( "  -" );
                    BufferConcatChar( *cmd );
                    BufferConcatNL();
                    BufferConcatNL();
                    BufferPrint();
                    printUsage( 0 );
                    error = RC_ERROR;
                    break;
                }
                if( error != RC_OKAY ) {
                    break;
                }
            } else {
                if( ObjFileName != NULL ) {
                    printUsage( ONLY_ONE_OBJECT );
                    error = RC_ERROR;
                    break;
                }
                ptr = cmd;
                cmd = findNextWS( cmd );
                ObjFileName = getFileName( ptr, cmd );
            }
            cmd = findNextArg( cmd );
        }
    }
    if( error == RC_OKAY ) {
        if( ObjFileName == NULL ) {
            printUsage( NO_OBJECT );
            error = RC_ERROR;
        } else {
            composeFileNames( list_file );
            if( DFormat & DFF_ASM ) {
                Options &= ~(PRINT_PUBLICS | PRINT_EXTERNS);
            }
        }
    }
    free( cmd_line );
    return( error );
}
