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


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

#include "global.h"
#include "dis.h"
#include "banner.h"
#include "args.h"
#include "buffer.h"
#include "memfuncs.h"
#include "print.h"

extern wd_options               Options;
extern char                     LabelChar;
extern char *                   ObjFileName;
extern char *                   ListFileName;
extern char *                   SourceFileName;
extern bool                     source_mix;
extern dis_format_flags         DFormat;

static void printUsage( int msg ) {
    int i;
    static const char * const banner[]={
        banner1w( "Multi-processor Disassembler", _WDISASM_VERSION_ ),
        banner2( "1995" ),
        banner3,
        NULL
    };
    const char * const *text;

    ChangePrintDest( STDERR_FILENO );
    if( msg != 0 ) {
        BufferMsg( msg );
        BufferConcatNL();
        BufferConcatNL();
        BufferPrint();
    }
    text = banner;
    while( *text ) {
        Print( (char *)*text++ );
        Print( "\n" );
    }
    for( i = USAGE_1; i <= USAGE_LAST; i++ ) {
        BufferMsg( i );
        BufferConcatNL();
        BufferPrint();
    }
    if( ObjFileName != NULL ) {
        MemFree( ObjFileName );
    }
    MemClose();
    exit( 1 );
}

static char *skipBlanks( char *cmd ) {
    while( isspace( *cmd ) ) {
        cmd++;
    }
    return( cmd );
}

static char *skipFileName( char * cmd ) {
    do {
        cmd++;
    } while( !isspace( *cmd ) && *cmd );
    return( cmd );
}

static char *skipToNextArg( char * cmd ) {
    while( !isspace( *cmd ) && *cmd != '/' && *cmd != '-' && *cmd ) {
        cmd++;
    }
    while( isspace( *cmd ) && *cmd ) {
        cmd++;
    }
    return( cmd );
}

static char *getFileName( char *start, char *following )
{
    int                                 length;
    char *                              name;

    length = following - start;
    name = (char *) MemAlloc( length + 1 );
    memcpy( name, start, length );
    name[length] = 0;
    return( name );
}

static void composeFileNames( bool list_file )
{
    char        path[_MAX_PATH2];
    char *      drive;
    char *      dir;
    char *      file_name;
    char *      extension;
    int         length;

    // object file name
    _splitpath2( ObjFileName, path, &drive, &dir, &file_name, &extension );
    if( strlen( extension ) == 0 ) {
        length = strlen( ObjFileName );
        MemFree( ObjFileName );
        ObjFileName = (char *) MemAlloc( length + strlen( OBJ_FILE_EXTENSION ) + 1 );
        _makepath( ObjFileName, drive, dir, file_name, OBJ_FILE_EXTENSION );
    } // else file name has an extension - leave as is
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

    DFormat |= DFF_PSEUDO | DFF_AXP_SYMBOLIC_REG;
    cmd = skipBlanks( cmd );
    if( *cmd == '\0' || *cmd == '?' ) {
        printUsage( NULL );
    } else {
        while( *cmd ) {
            if( IS_OPT_DELIM( *cmd ) ) {
                cmd++;
                switch( tolower( *cmd ) ) {
                    case 'a':
                        DFormat |= DFF_ASM;
                        if( cmd[1] == 'u' ) {
                            ++cmd;
                            DFormat |= DFF_X86_UNIX;
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
                                LabelChar = toupper( *cmd );
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
                            cmd = skipFileName( cmd );
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
                            DFormat ^= DFF_AXP_SYMBOLIC_REG;
                            ++cmd;
                            break;
                        case 'i':
                            DFormat ^= DFF_X86_ALT_INDEXING;
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
                            cmd = skipFileName( cmd );
                            SourceFileName = getFileName( ptr, cmd );
                        }
                        break;
                    default:
                        BufferMsg( INVALID_OPTION );
                        BufferStore( "  -%c\n\n", *cmd );
                        BufferPrint();
                        printUsage( NULL );
                        break;
                }
            } else {
                if( ObjFileName ) {
                    printUsage( ONLY_ONE_OBJECT );
                }
                ptr = cmd;
                cmd = skipFileName( cmd );
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
