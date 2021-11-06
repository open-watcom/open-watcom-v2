/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Find all modules which define any symbol
*               included in a list of symbols in input file.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bool.h"
#include "watcom.h"
#include "pcobj.h"
#include "hashtab.h"
#include "misc.h"

#define MAX_LINE_LEN    512

static symbol           **pubdef_tab;
static symbol           **extdef_tab;
static byte             RecHdr[3];

static void usage( void )
/***********************/
{
    printf( "Usage: objfind <options> <list of object or library files>\n" );
    printf( "  <options>   -i=<file with symbols>\n" );
}

static bool ProcFileModRef( FILE *fp )
/************************************/
{
    unsigned_16 page_len;
    unsigned_32 offset;
    char        *module_name;
    bool        ok;
    int         rc;

    page_len = 0;
    ReadRecInit();
    module_name = NULL;
    ok = true;
    while( ok ) {
        offset = ftell( fp );
        rc = ReadRec( fp, RecHdr );
        if( rc <= 0 ) {
            if( rc == 0 )
                ok = false;
            break;
        }
        switch( RecHdr[0] & ~1 ) {
        case CMD_THEADR:
            if( module_name != NULL )
                free( module_name );
            GetName();
            NameTerm();
            module_name = malloc( strlen( NamePtr ) + 1 );
            strcpy( module_name, NamePtr );
            break;
        case CMD_MODEND:
            if( page_len != 0 ) {
                offset = ftell( fp );
                offset = page_len - ( offset % page_len );
                if( offset != page_len ) {
                    fseek( fp, offset, SEEK_CUR );
                }
            }
            break;
        case CMD_PUBDEF:
            if( module_name == NULL )
                break;
            if( ( GetIndex() | GetIndex() ) == 0 )
                GetUInt();
            while( IsDataToRead() ) {
                GetName();
                NameTerm();
                if( SymbolExists( pubdef_tab, NamePtr ) != NULL ) {
                    if( SymbolExists( extdef_tab, module_name ) == NULL ) {
                        AddSymbol( extdef_tab, module_name, NULL, 0 );
                        printf( "%s\n", module_name );
                    }
                }
                GetOffset( RecHdr[0] & 1 );
                GetIndex();
            }
            break;
        case LIB_HEADER_REC:
            if( RecHdr[0] & 1 ) {
                fseek( fp, 0L, SEEK_END );
                page_len = 0;
            } else {
                page_len = GET_RECLEN( RecHdr ) - 1 + 4;
            }
            break;
        default:
            break;
        }
    }
    free( module_name );
    ReadRecFini();
    return( ok );
}

static void process_except_file( const char *filename )
/*****************************************************/
{
    FILE    *fp;
    char    line[MAX_LINE_LEN];
    char    *p;

    if( filename != NULL ) {
        fp = fopen( filename, "rt" );
        if( fp == NULL ) {
            printf( "Cannot open input file: %s.\n", filename );
            return;
        }
        while( fgets( line, sizeof( line ), fp ) != NULL ) {
            p = line + strlen( line );
            while( p >= line ) {
                if( ( *p == '\0' )
                  || ( *p == ' ' )
                  || ( *p == '\n' ) ) {
                    *p = '\0';
                     p--;
                } else {
                    break;
                }
            }
            AddSymbol( pubdef_tab, line, NULL, 0 );
        }
        fclose( fp );
    }
}

static bool process_file_modref( const char *filename )
/*****************************************************/
{
    FILE    *fp;
    bool    ok;

    fp = fopen( filename, "rb" );
    if( fp == NULL ) {
        printf( "Cannot open input file: %s.\n", filename );
        return( false );
    }
    ok = ProcFileModRef( fp );
    fclose( fp );
    return( ok );
}

int main( int argc, char *argv[] )
/********************************/
{
    int     i;
    char    *fn;
    char    c;
    bool    ok;

    ok = true;
    pubdef_tab = SymbolInit();
    extdef_tab = SymbolInit();
    for( i = 1; i < argc; ++i ) {
        if( argv[i][0] == '-' ) {
            c = tolower( (unsigned char)argv[i][1] );
            if( c == 'i' && argv[i][2] == '=' ) {
                process_except_file( argv[i] + 3 );
            } else {
                ok = false;
                break;
            }
        } else {
            break;
        }
    }
    if( i == argc ) {
        ok = false;
    }
    if( !ok ) {
        usage();
    } else {
        for( ; i < argc; ++i ) {
            fn = DoWildCard( argv[i] );
            while( fn != NULL ) {
                ok &= process_file_modref( fn );
                fn = DoWildCard( NULL );
            }
            DoWildCardClose();
        }
    }
    SymbolFini( pubdef_tab );
    SymbolFini( extdef_tab );
    return( ok ? EXIT_SUCCESS : EXIT_FAILURE );
}
