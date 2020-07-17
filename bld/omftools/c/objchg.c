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
* Description:  Tool for changing LNAMES entries and modify
*               EXTDEF/PUBDEF based on a pattern.
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

typedef struct sym_file {
    struct sym_file *next;
    char            *fname;
} sym_file;

static symbol       **pubdef_tab;
static symbol       **extdef_tab;
static char         *NamePtr;
static byte         NameLen;
static unsigned_16  RecLen;
static char         *RecBuff;
static char         *RecPtr;
static unsigned_16  RecMaxLen;
static int          isMS386;
static char         *symbol_name_pattern = NULL;
static size_t       symbol_name_pattern_len = 0;
static sym_file     *sym_files = NULL;

static void usage( void )
/***********************/
{
    printf( "Usage: objchg <options> <list of object or library files>\n" );
    printf( "  <options> -l=<old>=<new>  rename LNAMES item\n" );
    printf( "            -m=....         symbol name pattern\n" );
    printf( "            -s=<file>       file with symbols\n" );
}

static int EndRec( void )
/***********************/
{
    return( RecPtr >= (RecBuff + RecLen) );
}

static byte GetByte( void )
/*************************/
{
    return( *RecPtr++ );
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

static char *GetName( void )
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
        *((unsigned_16 *)p) = (unsigned_16)data;
        p += 2;
    }
    return( p );
}

static char *PutIndex( char *p, unsigned_16 index )
/*************************************************/
{
    if( index > 0x7f )
        *p++ = ( index >> 8 ) | 0x80;
    *p++ = index;
    return( p );
}

static byte create_chksum( const char *data, size_t newlen, byte cksum )
/**********************************************************************/
{
    size_t  i;

    for( i = 0; i < newlen - 1; i++ ) {
        cksum += data[i];
    }
    return( cksum );
}

static int ChangeLNAMES( byte rec_type, FILE *fo, unsigned_16 newlen )
/********************************************************************/
{
    byte        hdr[3];
    char        *data;
    char        *p;
    byte        cksum;
    int         ok;
    char        *n;
    char        b;

    hdr[0] = rec_type;
    hdr[1] = newlen;
    hdr[2] = newlen >> 8;
    if( fwrite( hdr, 1, 3, fo ) != 3 )
        return( 0 );
    cksum  = hdr[0];
    cksum += hdr[1];
    cksum += hdr[2];
    data = malloc( newlen );
    p = data;
    while( ! EndRec() ) {
        GetName();
        b = *RecPtr;
        *RecPtr = 0;
        n = SymbolExists( extdef_tab, NamePtr );
        if( n != NULL ) {
            NameLen = n[0];
            NamePtr = n + 1;
        }
        *p++ = NameLen;
        memcpy( p, NamePtr, NameLen );
        p += NameLen;
        *RecPtr = b;
    }
    *p++ = 0 - create_chksum( data, newlen, cksum );
    ok = ( fwrite( data, 1, newlen, fo ) == newlen );
    free( data );
    return( ok );
}

static int ChangeEXTDEF( byte rec_type, FILE *fo, unsigned_16 newlen )
/********************************************************************/
{
    byte        hdr[3];
    char        *data;
    char        *p;
    char        *tmp;
    unsigned_16 indx;
    byte        cksum;
    int         ok;
    char        *n;

    hdr[0] = rec_type;
    hdr[1] = newlen;
    hdr[2] = newlen >> 8;
    if( fwrite( hdr, 1, 3, fo ) != 3 )
        return( 0 );
    cksum  = hdr[0];
    cksum += hdr[1];
    cksum += hdr[2];
    data = malloc( newlen );
    p = data;
    while( ! EndRec() ) {
        GetName();
        tmp = RecPtr;
        indx = GetIndex();
        *tmp = 0;
        n = SymbolExists( pubdef_tab, NamePtr );
        if( n != NULL ) {
            NameLen = n[0];
            NamePtr = n + 1;
        }
        *p++ = NameLen;
        memcpy( p, NamePtr, NameLen );
        p += NameLen;
        p = PutIndex( p, indx );
    }
    *p++ = 0 - create_chksum( data, newlen, cksum );
    ok = ( fwrite( data, 1, newlen, fo ) == newlen );
    free( data );
    return( ok );
}

static int ChangePUBDEF( byte rec_type, FILE *fo, unsigned_16 newlen )
/********************************************************************/
{
    byte        hdr[3];
    char        *data;
    char        *p;
    char        *tmp;
    unsigned_16 idx1;
    unsigned_16 idx2;
    unsigned_32 offs;
    byte        cksum;
    int         ok;
    char        *n;

    hdr[0] = rec_type;
    hdr[1] = newlen;
    hdr[2] = newlen >> 8;
    if( fwrite( hdr, 1, 3, fo ) != 3 )
        return( 0 );
    cksum  = hdr[0];
    cksum += hdr[1];
    cksum += hdr[2];
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
        offs = GetOffset();
        *tmp = 0;
        n = SymbolExists( pubdef_tab, NamePtr );
        if( n != NULL ) {
            NameLen = n[0];
            NamePtr = n + 1;
        }
        *p++ = NameLen;
        memcpy( p, NamePtr, NameLen );
        p += NameLen;
        p = PutOffset( p, offs );
        p = PutIndex( p, GetIndex() );
    }
    *p++ = 0 - create_chksum( data, newlen, cksum );
    ok = ( fwrite( data, 1, newlen, fo ) == newlen );
    free( data );
    return( ok );
}

static int ProcFile( FILE *fp, FILE *fo )
/**************************************/
{
    byte            hdr[3];
//    unsigned_16   page_len;
//    unsigned_32   offset;
    fpos_t          pos;
    int             isChanged;
    int             renameIt;
    unsigned_16     newlen;
    char            *n;
    char            b;

    renameIt = 1;
//    page_len = 0;
    RecBuff = NULL;
    RecMaxLen = 0;
    for( ; renameIt == 1; ) {
        isChanged = 0;
        fgetpos( fp, &pos );
//        offset = ftell( fp );
        if( fread( hdr, 1, 3, fp ) != 3 ) {
            if( ferror( fp ) )
                renameIt = -1;
            break;
        }
        RecLen = hdr[1] | ( hdr[2] << 8 );
        if( RecMaxLen < RecLen ) {
            RecMaxLen = RecLen;
            if( RecBuff != NULL ) {
                free( RecBuff );
            }
            RecBuff = malloc( RecMaxLen );
            if( RecBuff == NULL ) {
                printf( "**FATAL** Out of memory!\n" );
                renameIt = -1;
                break;
            }
        }
        if( fread( RecBuff, RecLen, 1, fp ) == 0 ) {
            renameIt = 0;
            break;
        }
        RecPtr = RecBuff;
        newlen = RecLen;
        RecLen--;
        isMS386 = hdr[0] & 1;
        switch( hdr[0] & ~1 ) {
        case CMD_LNAMES:
            while( ! EndRec() ) {
                GetName();
                b = *RecPtr;
                *RecPtr = 0;
                n = SymbolExists( extdef_tab, NamePtr );
                if( n != NULL ) {
                    newlen += n[0] - NameLen;
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
                n = SymbolExists( pubdef_tab, NamePtr );
                if( n != NULL ) {
                    newlen += n[0] - NameLen;
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
                n = SymbolExists( pubdef_tab, NamePtr );
                if( n != NULL ) {
                    newlen += n[0] - NameLen;
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
            switch( hdr[0] & ~1 ) {
            case CMD_LNAMES:
                if( ! ChangeLNAMES( hdr[0], fo, newlen ) )
                    renameIt = 0;
                break;
            case CMD_EXTDEF:
                if( ! ChangeEXTDEF( hdr[0], fo, newlen ) )
                    renameIt = 0;
                break;
            case CMD_PUBDEF:
                if( ! ChangePUBDEF( hdr[0], fo, newlen ) )
                    renameIt = 0;
                break;
            default:
                break;
            }
        } else {
            if( fwrite( hdr, 1, 3, fo ) != 3 ) {
                renameIt = -1;
            }
            if( fwrite( RecBuff, 1, RecLen + 1, fo ) != RecLen + 1 ) {
                renameIt = -1;
            }
        }
    }
    free( RecBuff );
    fclose( fo );
    fclose( fp );
    return( renameIt );
}

static int process_symbol_file( const char *filename )
/****************************************************/
{
    FILE    *fp;
    char    line[MAX_LINE_LEN];
    char    newname[MAX_LINE_LEN];
    char    *old;
    char    *p;
    size_t  i;
    size_t  len;

    if( filename != NULL ) {
        fp = fopen( filename, "rt" );
        while( fgets( line, sizeof( line ), fp ) != NULL ) {
            old = strtok( line, " \t=\r\n" );
            if( old == NULL )
                continue;
            if( SymbolExists( pubdef_tab, old ) != NULL )
                continue;
            p = newname + 1;
            for( i = 0; i < symbol_name_pattern_len; i++ ) {
                if( symbol_name_pattern[i] == '*' ) {
                    len = strlen( old );
                    memcpy( p, old, len );
                    p += len;
                } else {
                    *p++ = symbol_name_pattern[i];
                }
            }
            *p = '\0';
            len = p - newname - 1;
            newname[0] = len;
            AddSymbol( pubdef_tab, old, newname, len + 2 );
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
    char    *newname;
    size_t  len;

    len = strlen( cmd ) + 2;
    newname = malloc( len );
    if( newname != NULL ) {
        p1 = strtok( cmd, "=" );
        p2 = strtok( NULL, "" );
        len = strlen( p2 );
        newname[0] = len;
        memcpy( newname + 1, p2, len );
        newname[len + 1] = '\0';
        AddSymbol( extdef_tab, p1, newname, len + 2 );
        free( newname );
    }
    return( 0 );
}

static int process_module( const char *filename )
/***********************************************/
{
    FILE    *fp;
    FILE    *fo;
    int     isChanged;

    fp = fopen( filename, "rb" );
    if( fp == NULL ) {
        printf( "Cannot open input file: %s.\n", filename );
        return( 0 );
    }
    fo = fopen( "tmp.tmp", "wb" );
    if( fo == NULL ) {
        printf( "Cannot open input file: tmp.tmp.\n" );
        fclose( fp );
        return( 0 );
    }
    isChanged = ProcFile( fp, fo );
    if( isChanged == 1 ) {
        remove( filename );
        rename( "tmp.tmp", filename );
    } else {
        remove( "tmp.tmp" );
    }
    return( isChanged != -1 );
}

int main( int argc, char *argv[] )
/********************************/
{
    int         i;
    char        *fn;
    int         ok;
    char        c;
    sym_file    *sf;

    ok = 1;
    extdef_tab = SymbolInit();
    pubdef_tab = SymbolInit();
    for( i = 1; i < argc; ++i ) {
        if( argv[i][0] == '-' ) {
            c = tolower( argv[i][1] );
            if( c == 'l' && argv[i][2] == '=' ) {
                process_lnames( argv[i] + 3 );
            } else if( c == 's' && argv[i][2] == '=' ) {
                if( sym_files == NULL ) {
                    sf = sym_files = malloc( sizeof( sym_file ) );
                } else {
                    sf = sym_files;
                    while( sf->next != NULL ) {
                        sf = sf->next;
                    }
                    sf->next = malloc( sizeof( sym_file ) );
                    sf = sf->next;
                }
                sf->next = NULL;
                sf->fname = argv[i] + 3;
            } else if( c == 'm' && argv[i][2] == '=' ) {
                symbol_name_pattern = argv[i] + 3;
                symbol_name_pattern_len = strlen( symbol_name_pattern );
            } else {
                ok = 0;
                break;
            }
        } else {
            break;
        }
    }
    if( i == argc ) {
        ok = 0;
    }
    if( ok == 0 ) {
        usage();
    } else {
        while( (sf = sym_files) != NULL ) {
            sym_files = sf->next;
            if( symbol_name_pattern != NULL ) {
                process_symbol_file( sf->fname );
            }
            free( sf );
        }
        for( ; i < argc; ++i ) {
            fn = DoWildCard( argv[i] );
            while( fn != NULL ) {
                ok &= process_module( fn );
                fn = DoWildCard( NULL );
            }
            DoWildCardClose();
        }
    }
    SymbolFini( pubdef_tab );
    SymbolFini( extdef_tab );
    return( ok == 0 );
}
