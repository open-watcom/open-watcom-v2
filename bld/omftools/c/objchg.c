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
* Description:  Tool for changing LNAMES entries and modify 
*               EXTDEF/PUBDEF based on a pattern.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
static char        *symbol_name_change = NULL;

static void usage( void )
/***********************/
{
    SymbolFini( extdef_tab );
    SymbolFini( pubdef_tab );
    printf( "Usage: objchg <options> <list of object or library files>\n" );
    printf( "  <options> -l=<old>=<new>  rename LNAMES item\n" );
    printf( "            -m=....         symbol name pattern\n" );
    printf( "            -s=<file>       file with symbols\n" );
    exit( -1 );
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

static char *PutUInt( char *p, unsigned_16 data )
/***********************************************/
{
    CONV_LE_16( data );
    *((unsigned_16 *)p) = data;
    p += 2;
    return( p );
}

static char *PutOffset( char *p, unsigned_32 data )
/*************************************************/
{
    if( isMS386 ) {
        CONV_LE_32( data );
        *((unsigned_32 *)p) = data;
        p += 4;
    } else {
        CONV_LE_16( data );
        *((unsigned_16 *)p) = data;
        p += 2;
    }
    return( p );
}

static char *PutIndex( char *p, unsigned_16 index )
/*************************************************/
{
    if( index > 0x7f )
        *(p++) = ( index >> 8 ) | 0x80;
    *(p++) = index;
    return( p );
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

static byte create_chksum( char *data, int newlen, byte cksum )
/*************************************************************/
{
    int     i;

    for( i = 0; i < newlen - 1; i++ ) {
        cksum += data[i];
    }
    return( cksum );
}

static char *change_name( char *dst, byte *src )
/**********************************************/
{
    int     i;
    char    *p;

    p = dst++;
    if( symbol_name_change != NULL ) {
        for( i = 0; i < strlen( symbol_name_change ); i++ ) {
            if( symbol_name_change[i] == '*' ) {
                int     len;

                len = strlen( (char *)src );
                memcpy( dst, src, len );
                dst += len;
            } else {
                *(dst++) = symbol_name_change[i];
            }
        }
    }
    *p = dst - p - 1;
    return( dst );
}

static int ChangeLNAMES( byte rec_type, FILE *fo, unsigned_16 newlen )
/********************************************************************/
{
    byte        hdr[ 3 ];
    char        *data;
    char        *p;
    byte        cksum;

    hdr[ 0 ] = rec_type;
    hdr[ 1 ] = newlen;
    hdr[ 2 ] = newlen >> 8;
    if( fwrite( hdr, 1, 3, fo ) != 3 )
        return( 0 );
    cksum  = hdr[ 0 ];
    cksum += hdr[ 1 ];
    cksum += hdr[ 2 ];
    data = malloc( newlen );
    p = data;
    while( ! EndRec() ) {
        char     *n;
        byte     b;

        GetName();
        b = *RecPtr;
        *RecPtr = 0;
        n = SymbolExists( extdef_tab, (char *)NamePtr );
        if( n != NULL ) {
            NameLen = strlen( n );
            NamePtr = (byte *)n;
        }
        *(p++) = NameLen;
        memcpy( p, NamePtr, NameLen );
        p += NameLen;
        *RecPtr = b;
    }
    *(p++) = 0 - create_chksum( data, newlen, cksum );
    if( fwrite( data, 1, newlen, fo ) != newlen ) {
        free( data );
        return( 0 );
    }
    free( data );
    return( 1 );
}

static int ChangeEXTDEF( byte rec_type, FILE *fo, unsigned_16 newlen )
/********************************************************************/
{
    byte        hdr[ 3 ];
    char        *data;
    char        *p;
    byte        *tmp;
    long        lng1;
    byte        cksum;

    hdr[ 0 ] = rec_type;
    hdr[ 1 ] = newlen;
    hdr[ 2 ] = newlen >> 8;
    if( fwrite( hdr, 1, 3, fo ) != 3 )
        return( 0 );
    cksum  = hdr[ 0 ];
    cksum += hdr[ 1 ];
    cksum += hdr[ 2 ];
    data = malloc( newlen );
    p = data;
    while( ! EndRec() ) {
        GetName();
        tmp = RecPtr;
        lng1 = GetIndex();
        *tmp = 0;
        if( SymbolExists( pubdef_tab, (char *)NamePtr ) != NULL ) {
            p = change_name( p, NamePtr );
        } else {
            *(p++) = NameLen;
            memcpy( p, NamePtr, NameLen );
            p += NameLen;
        }
        p = PutIndex( p, lng1 );
    }
    *(p++) = 0 - create_chksum( data, newlen, cksum );
    if( fwrite( data, 1, newlen, fo ) != newlen ) {
        free( data );
        return( 0 );
    }
    free( data );
    return( 1 );
}

static int ChangePUBDEF( byte rec_type, FILE *fo, unsigned_16 newlen )
/********************************************************************/
{
    byte        hdr[ 3 ];
    char        *data;
    char        *p;
    byte        *tmp;
    int         idx1;
    int         idx2;
    long        lng1;
    byte        cksum;

    hdr[ 0 ] = rec_type;
    hdr[ 1 ] = newlen;
    hdr[ 2 ] = newlen >> 8;
    if( fwrite( hdr, 1, 3, fo ) != 3 )
        return( 0 );
    cksum  = hdr[ 0 ];
    cksum += hdr[ 1 ];
    cksum += hdr[ 2 ];
    data = malloc( newlen );
    p = data;
    idx1 = GetIndex();
    p = PutIndex( p, idx1 );
    idx2 = GetIndex();
    p = PutIndex( p, idx2 );
    if( ( idx1 | idx2 ) == 0 )
        p = PutUInt( p, GetUInt() );
    while( ! EndRec() ) {
        GetName();
        tmp = RecPtr;
        lng1 = GetOffset();
        *tmp = 0;
        if( SymbolExists( pubdef_tab, (char *)NamePtr ) != NULL ) {
            p = change_name( p, NamePtr );
        } else {
            *(p++) = NameLen;
            memcpy( p, NamePtr, NameLen );
            p += NameLen;
        }
        p = PutOffset( p, lng1 );
        p = PutIndex( p, GetIndex() );
    }
    *(p++) = 0 - create_chksum( data, newlen, cksum );
    if( fwrite( data, 1, newlen, fo ) != newlen ) {
        free( data );
        return( 0 );
    }
    free( data );
    return( 1 );
}

static int ProcFile( FILE *fp, FILE *fo )
/**************************************/
{
    byte        hdr[ 3 ];
    unsigned_16 page_len;
    unsigned_32 offset;
    fpos_t      pos;
    int         isChanged;
    int         renameIt;
    unsigned_16 newlen;
    int         symbol_name_change_len;

    if( symbol_name_change == NULL ) {
        symbol_name_change_len = 0;
    } else {
        symbol_name_change_len = strlen( symbol_name_change ) - 1;
    }
    renameIt = 1;
    page_len = 0;
    RecBuff = NULL;
    RecMaxLen = 0;
    for( ; renameIt; ) {
        isChanged = 0;
        fgetpos( fp, &pos );
        offset = ftell( fp );
        if( fread( hdr, 1, 3, fp ) != 3 ) {
            if( ferror( fp ) )
                renameIt = 0;
            break;
        }
        RecLen = hdr[ 1 ] | ( hdr[ 2 ] << 8 );
        ResizeBuff( RecLen );
        RecPtr = RecBuff;
        if( fread( RecBuff, RecLen, 1, fp ) == 0 ) {
            renameIt = 0;
            break;
        }
        newlen = RecLen;
        RecLen--;
        isMS386 = hdr[ 0 ] & 1;
        switch( hdr[ 0 ] & ~1 ) {
        case CMD_LNAMES:
            while( ! EndRec() ) {
                char     *n;
                byte     b;

                GetName();
                b = *RecPtr;
                *RecPtr = 0;
                n = SymbolExists( extdef_tab, (char *)NamePtr );
                if( n != NULL ) {
                    newlen += strlen( n ) - NameLen;
                    isChanged = 1;
                }
                *RecPtr = b;
            }
            fsetpos( fp, &pos );
            fread( hdr, 1, 3, fp );
            fread( RecBuff, RecLen + 1, 1, fp );
            RecPtr = RecBuff;
            break;
        case CMD_EXTDEF:
            while( ! EndRec() ) {
                GetName();
                *RecPtr = 0;
                if( SymbolExists( pubdef_tab, (char *)NamePtr ) != NULL ) {
                    newlen += symbol_name_change_len;
                    isChanged = 1;
                }
                GetIndex();
            }
            fsetpos( fp, &pos );
            fread( hdr, 1, 3, fp );
            fread( RecBuff, RecLen + 1, 1, fp );
            RecPtr = RecBuff;
            break;
        case CMD_PUBDEF:
            if( ( GetIndex() | GetIndex() ) == 0 )
                GetUInt();
            while( ! EndRec() ) {
                GetName();
                *RecPtr = 0;
                if( SymbolExists( pubdef_tab, (char *)NamePtr ) != NULL ) {
                    newlen += symbol_name_change_len;
                    isChanged = 1;
                }
                GetOffset();
                GetIndex();
            }
            fsetpos( fp, &pos );
            fread( hdr, 1, 3, fp );
            fread( RecBuff, RecLen + 1, 1, fp );
            RecPtr = RecBuff;
            break;
        case LIB_HEADER_REC:
            free( RecBuff );
            fclose( fo );
            fclose( fp );
            return( 0 );
        default:
            break;
        }
        if( isChanged ) {
            switch( hdr[ 0 ] & ~1 ) {
            case CMD_LNAMES:
                if( ! ChangeLNAMES( hdr[ 0 ], fo, newlen ) )
                    renameIt = 0;
                break;
            case CMD_EXTDEF:
                if( ! ChangeEXTDEF( hdr[ 0 ], fo, newlen ) )
                    renameIt = 0;
                break;
            case CMD_PUBDEF:
                if( ! ChangePUBDEF( hdr[ 0 ], fo, newlen ) )
                    renameIt = 0;
                break;
            default:
                break;
            }
        } else {
            if( fwrite( hdr, 1, 3, fo ) != 3 ) {
                renameIt = 0;
            }
            if( fwrite( RecBuff, 1, RecLen + 1, fo ) != RecLen + 1 ) {
                renameIt = 0;
            }
        }
    }
    free( RecBuff );
    fclose( fo );
    fclose( fp );
    return( renameIt );
}

static int process_symbol_file( char *filename )
/**********************************************/
{
    FILE    *fp;
    char    line[ MAX_LINE_LEN ];
    char    *p;

    if( filename != NULL ) {
        fp = fopen( filename, "rt" );
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
    return( 0 );
}

static int process_lnames( char *cmd )
/************************************/
{
    char    *p1;
    char    *p2;

    p1 = strtok( cmd, "=" );
    p2 = strtok( NULL, "" );
    AddSymbol( extdef_tab, p1, p2 );
    return( 0 );
}

static int process_module( char *filename )
/*****************************************/
{
    FILE    *fp;
    FILE    *fo;
    int     isChanged;

    fp = fopen( filename, "rb" );
    if( fp == NULL ) {
        printf( "Cannot open input file: %s.\n", filename );
        return( -1 );
    }
    fo = fopen( "tmp.tmp", "wb" );
    if( fo == NULL ) {
        printf( "Cannot open input file: tmp.tmp.\n" );
        fclose( fp );
        return( -1 );
    }
    isChanged = ProcFile( fp, fo );
    if( isChanged ) {
        remove( filename );
        rename( "tmp.tmp", filename );
    }
    return( 0 );
}

int main( int argc, char *argv[] )
/********************************/
{
    int     i;
    char    *fn;

    extdef_tab = SymbolInit();
    pubdef_tab = SymbolInit();
    for( i = 1; i < argc; ++i ) {
        if( argv[i][0] == '-' ) {
            switch( tolower( argv[i][1] ) ) {
            case 'l':
                if( argv[i][2] == '=' ) {
                    process_lnames( argv[i] + 3 );
                } else {
                    usage();
                }
                break;
            case 's':
                if( argv[i][2] == '=' ) {
                    process_symbol_file( argv[i] + 3 );
                } else {
                    usage();
                }
                break;
            case 'm':
                if( argv[i][2] == '=' ) {
                    symbol_name_change = argv[i] + 3;
                } else {
                    usage();
                }
                break;
            default:
                usage();
            }
        } else {
            break;
        }
    }
    if( i == argc )
        usage();
    for( ; i < argc; ++i ) {
        fn = DoWildCard( argv[i] );
        while( fn != NULL ) {
            process_module( fn );
            fn = DoWildCard( NULL );
        }
        DoWildCardClose();
    }
    SymbolFini( pubdef_tab );
    SymbolFini( extdef_tab );
    return( 0 );
}
