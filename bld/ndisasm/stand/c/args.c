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
* Description:  Standalone disassembler command line processing.
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "wio.h"
#include "dis.h"
#include "global.h"
#include "banner.h"
#include "args.h"
#include "buffer.h"
#include "memfuncs.h"
#include "print.h"
#include "cmdlhelp.h"

#include "clibext.h"


static const char * const banner[]={
    banner1w( "Multi-processor Disassembler", _WDISASM_VERSION_ ),
    banner2,
    banner2a( "1995" ),
    banner3,
    banner3a,
    NULL
};

extern wd_options               Options;
extern char                     LabelChar;
extern char *                   ObjFileName;
extern char *                   ListFileName;
extern char *                   SourceFileName;
extern bool                     source_mix;
extern dis_format_flags         DFormat;

static void printUsage( int msg )
{
    int                 id;
    const char * const  *text;
    char                buff[MAX_RESOURCE_SIZE];

    ChangePrintDest( STDERR_FILENO );
    if( msg != 0 ) {
        BufferMsg( msg );
        BufferConcatNL();
        BufferConcatNL();
        BufferPrint();
    }
    text = banner;
    while( *text ) {
        Print( *text++ );
        Print( "\n" );
    }
    id = MSG_USAGE_BASE;
    if( MsgGet( id, buff ) ) {
        ++id;
        while( MsgGet( id, buff ) ) {
            if( buff[0] == '.' && buff[1] == '\0' ) {
                break;
            }
            BufferConcat( buff );
            BufferConcatNL();
            BufferPrint();
            ++id;
        }
    }
    if( ObjFileName != NULL ) {
        MemFree( ObjFileName );
    }
    MemClose();
    exit( 1 );
}

static int iswsOrOpt( char ch )
{
    if( isspace( ch ) ) return( 1 );

    if( IS_OPT_DELIM( ch ) ) return( 1 );

    return( 0 );
}

static char *skipBlanks( char *cmd ) {
    while( isspace( *cmd ) ) {
        cmd++;
    }
    return( cmd );
}

static char *skipToNextArg( char * cmd ) {
    char    string_open = 0;

    while( *cmd != '\0' ) {
        if( *cmd == '\\' ) {
            cmd++;
            if( *cmd != '\0' ) {
                if( !string_open && iswsOrOpt( *cmd ) ) {
                    break;
                }
                cmd++;
            }
        } else {
            if( *cmd == '\"' ) {
                string_open = !string_open;
                cmd++;
            } else {
                if( !string_open && iswsOrOpt( *cmd ) ) {
                    break;
                }
                cmd++;
            }
        }
    }

    cmd = skipBlanks( cmd );

    return( cmd );
}

static char *getFileName( char *start, char *following )
{
    size_t      length;
    char        *name;
    char        *tmp;

    length = following - start;
    tmp = (char *) MemAlloc( length + 1 );
    memcpy( tmp, start, length );
    tmp[length] = 0;

    if( strchr( tmp, '\"' ) == NULL )
        return tmp;

    name = (char *) MemAlloc( length + 1 );
    UnquoteFName( name, length + 1, tmp );
    MemFree( tmp );

    return( name );
}

static void composeFileNames( bool list_file )
{
    char        path[_MAX_PATH2];
    char        *drive;
    char        *dir;
    char        *file_name;
    char        *extension;
    size_t      length;

    // object file name
    _splitpath2( ObjFileName, path, &drive, &dir, &file_name, &extension );
#ifndef __UNIX__
    // tacking on an extension is self-defeating on UNIX, and the extra
    // dot at end trick doesn't work either
    if( strlen( extension ) == 0 ) {
        length = strlen( ObjFileName );
        MemFree( ObjFileName );
        ObjFileName = (char *) MemAlloc( length + strlen( OBJ_FILE_EXTENSION ) + 1 );
        _makepath( ObjFileName, drive, dir, file_name, OBJ_FILE_EXTENSION );
    } // else file name has an extension - leave as is
#endif
    if( list_file ) {
        if( ListFileName == NULL ) {
            length = strlen( drive ) + strlen( dir ) + strlen( dir ) +
                strlen( file_name ) + strlen( LIST_FILE_EXTENSION );
            ListFileName = (char *) MemAlloc( length + 1 );
            _makepath( ListFileName, drive, dir, file_name, LIST_FILE_EXTENSION );
        } else {
            // check extension
            _splitpath2( ListFileName, path, &drive, &dir, &file_name, &extension );
            if( strlen( extension ) == 0 ) {
                length = strlen( ListFileName );
                MemFree( ListFileName );
                ListFileName = (char *) MemAlloc( length + strlen( LIST_FILE_EXTENSION ) + 1 );
                _makepath( ListFileName, drive, dir, file_name, LIST_FILE_EXTENSION );
            } // else has extension, leave it as is
        }
    }
}

void HandleArgs( char *cmd )
{
    char *                      ptr;
    bool                        list_file = FALSE;

    DFormat |= DFF_PSEUDO | DFF_SYMBOLIC_REG;
    cmd = skipBlanks( cmd );
    if( *cmd == '\0' || *cmd == '?' ) {
        printUsage( 0 );
    } else {
        while( *cmd ) {
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
                            } else {
                                printUsage( INVALID_I );
                            }
                        } else {
                            printUsage( INVALID_I );
                        }
                        break;
                    case 'l':
                        if( ListFileName ) {
                            printUsage( ONLY_ONE_LISTING );
                        }
                        list_file = TRUE;
                        cmd++;
                        if( *cmd == '=' ) {
                            cmd++;
                            ptr = cmd;
                            cmd = FindNextWS( cmd );
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
                        if( SourceFileName ) {
                            printUsage( ONLY_ONE_SOURCE );
                        }
                        source_mix = TRUE;
                        cmd++;
                        if( *cmd == '=' ) {
                            cmd++;
                            ptr = cmd;
                            cmd = FindNextWS( cmd );
                            SourceFileName = getFileName( ptr, cmd );
                        }
                        break;
                    default:
                        BufferMsg( INVALID_OPTION );
                        BufferStore( "  -%c\n\n", *cmd );
                        BufferPrint();
                        printUsage( 0 );
                        break;
                }
            } else {
                if( ObjFileName ) {
                    printUsage( ONLY_ONE_OBJECT );
                }
                ptr = cmd;
                cmd = FindNextWS( cmd );
                ObjFileName = getFileName( ptr, cmd );
            }
            cmd = skipToNextArg( cmd );
        }
    }
    if( !ObjFileName ) {
        printUsage( NO_OBJECT );
    }
    composeFileNames( list_file );
    if( DFormat & DFF_ASM ) {
        Options &= ~(PRINT_PUBLICS | PRINT_EXTERNS);
    }
}
