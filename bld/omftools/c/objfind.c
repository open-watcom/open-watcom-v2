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
* Description:  Find all modules which define any symbol
*               included in a list of symbols in input file.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "watcom.h"
#include "pcobj.h"
#include "hashtab.h"
#include "misc.h"

#define MAX_LINE_LEN 512

typedef unsigned char byte;
typedef byte *data_ptr;

static symbol      **pubdef_tab;
static symbol      **extdef_tab;
static data_ptr    NamePtr;
static byte        NameLen;
static unsigned_16 RecLen;
static data_ptr    RecBuff;
static data_ptr    RecPtr;
static unsigned_16 RecMaxLen;
static int         isMS386;

static void usage( void )
/***********************/
{
    SymbolFini( pubdef_tab );
    printf( "Usage: objfind <options> <list of object or library files>\n" );
    printf( "  <options>   -i=<file with symbols>\n" );
}

static int EndRec( void )
/***********************/
{
    return( RecPtr >= (RecBuff + RecLen) );
}

static byte GetByte( void )
/*************************/
{
    byte        ret;

    ret = *RecPtr;
    RecPtr++;
    return( ret );
}

static unsigned_16 GetUInt( void )
/********************************/
{
    unsigned_16 word;

    word = *(unsigned_16 *)RecPtr;
    CONV_LE_16( word );
    RecPtr += 2;
    return( word );
}

static unsigned_32 GetOffset( void )
/**********************************/
{
    if( isMS386 ) {
        unsigned_32 dword;

        dword = *(unsigned_32 *)RecPtr;
        CONV_LE_32( dword );
        RecPtr += 4;
        return( dword );
    } else {
        unsigned_16 word;

        word = *(unsigned_16 *)RecPtr;
        CONV_LE_16( word );
        RecPtr += 2;
        return( word );
    }
}

static unsigned_16 GetIndex( void )
/*********************************/
{
    unsigned_16 index;

    index = GetByte();
    if( index & 0x80 ) {
      index = ( (index & 0x7f) << 8 ) + GetByte();
    }
    return( index );
}

static byte *GetName( void )
/**************************/
{
    NameLen = GetByte();
    NamePtr = RecPtr;
    RecPtr += NameLen;
    return( NamePtr );
}

static void ResizeBuff( unsigned_16 reqd_len )
/********************************************/
{
    if( reqd_len > RecMaxLen ) {
        RecMaxLen = reqd_len;
        if( RecBuff != NULL ) {
            free( RecBuff );
        }
        RecBuff = malloc( RecMaxLen );
        if( RecBuff == NULL ) {
            printf( "**FATAL** Out of memory!\n" );
            exit( -1 );
        }
    }
}

static void ProcFileModRef( FILE *fp )
/************************************/
{
    byte        hdr[ 3 ];
    unsigned_16 page_len;
    unsigned_32 offset;
    char        *module_name;

    page_len = 0;
    RecBuff = NULL;
    RecMaxLen = 0;
    module_name = NULL;
    for(;;) {
        offset = ftell( fp );
        if( fread( hdr, 1, 3, fp ) != 3 )
            break;
        RecLen = hdr[ 1 ] | ( hdr[ 2 ] << 8 );
        ResizeBuff( RecLen );
        RecPtr = RecBuff;
        if( fread( RecBuff, RecLen, 1, fp ) == 0 )
            break;
        RecLen--;
        isMS386 = hdr[ 0 ] & 1;
        switch( hdr[ 0 ] & ~1 ) {
        case CMD_THEADR:
            if( module_name != NULL )
                free( module_name );
            GetName();
            *RecPtr = 0;
            module_name = malloc( strlen( (char *)NamePtr ) + 1 );
            strcpy( module_name, (char *)NamePtr );
            break;
        case CMD_MODEND:
            if( module_name != NULL )
                free( module_name );
            module_name = NULL;
            if( page_len != 0 ) {
                offset = ftell( fp );
                offset = page_len - offset % page_len;
                if( offset != page_len ) {
                    fseek( fp, offset, SEEK_CUR );
                }
            }
            break;
        case CMD_PUBDEF:
            if( ( GetIndex() | GetIndex() ) == 0 )
                GetUInt();
            while( ! EndRec() ) {
                GetName();
                *RecPtr = 0;
                if( SymbolExists( pubdef_tab, (char *)NamePtr ) != 0 ) {
                    if( SymbolExists( extdef_tab, module_name ) == 0 ) {
                        AddSymbol( extdef_tab, module_name, NULL );
                        printf( "%s\n", module_name );
                    }
                }
                GetOffset();
                GetIndex();
            }
            break;
        case LIB_HEADER_REC:
            if( isMS386 ) {
                fseek( fp, 0L, SEEK_END );
                page_len = 0;
            } else {
                page_len = RecLen + 4;
            }
            break;
        default:
            break;
        }
    }
    free( RecBuff );
}

static void process_except_file( char *filename )
/***********************************************/
{
    FILE    *fp;
    char    line[ MAX_LINE_LEN ];
    char    *p;

    if( filename != NULL ) {
        fp = fopen( filename, "rt" );
        if( fp == NULL ) {
            printf( "Cannot open input file: %s.\n", filename );
            return;
        }
        while( fgets( line, MAX_LINE_LEN , fp ) != NULL ) {
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
            AddSymbol( pubdef_tab, line, NULL );
        }
        fclose( fp );
    }
}        

static int process_file_modref( char *filename )
/**********************************************/
{
    FILE    *fp;

    fp = fopen( filename, "rb" );
    if( fp == NULL ) {
        printf( "Cannot open input file: %s.\n", filename );
        return( 0 );
    }
    ProcFileModRef( fp );
    fclose( fp );
    return( 1 );
}

int main( int argc, char *argv[] )
/********************************/
{
    int     i;
    char    *fn;

    pubdef_tab = SymbolInit();
    for( i = 1; i < argc; ++i ) {
        if( argv[i][0] == '-' ) {
            switch( tolower( argv[i][1] ) ) {
            case 'i':
                if( argv[i][2] == '=' ) {
                    process_except_file( argv[i] + 3 );
                    break;
                }
            default:
                usage();
                return( 1 );
            }
        } else {
            break;
        }
    }
    if( i == argc ) {
        usage();
        return( 1 );
    }
    extdef_tab = SymbolInit();
    for( ; i < argc; ++i ) {
        fn = DoWildCard( argv[i] );
        while( fn != NULL ) {
            process_file_modref( fn );
            fn = DoWildCard( NULL );
        }
        DoWildCardClose();
    }
    SymbolFini( pubdef_tab );
    SymbolFini( extdef_tab );
    return( 0 );
}
