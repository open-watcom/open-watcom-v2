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
#include "bool.h"
#include "watcom.h"
#include "pcobj.h"
#include "hashtab.h"
#include "misc.h"


#define MAX_LINE_LEN 512

#define SET_RECLEN(h,s) h[1] = (byte)s; h[2] = (s >> 8)

typedef struct sym_file {
    struct sym_file *next;
    char            *fname;
} sym_file;

static symbol       **pubdef_tab;
static symbol       **extdef_tab;
static unsigned_16  WriteRecLen;
static char         *WriteRecBuff;
static char         *WriteRecPtr;
static unsigned_16  WriteRecMaxLen;
static char         *symbol_name_pattern = NULL;
static size_t       symbol_name_pattern_len = 0;
static sym_file     *sym_files = NULL;
static byte         RecHdr[3];

static byte tbyte = 0;
static byte *tptr = NULL;

static void NameTermSave( void )
/******************************/
{
    tptr = GetReadPtr();
    tbyte = *tptr;
    *tptr = '\0';
}

static void NameTermPtr( void )
/*****************************/
{
    tptr = GetReadPtr();
}

static void NameTermData( void )
/******************************/
{
    tbyte = *tptr;
    *tptr = '\0';
}

static void NameTermRest( void )
/******************************/
{
    *tptr = tbyte;
}

static void usage( void )
/***********************/
{
    printf( "Usage: objchg <options> <list of object or library files>\n" );
    printf( "  <options> -l=<old>=<new>  rename LNAMES item\n" );
    printf( "            -m=....         symbol name pattern\n" );
    printf( "            -s=<file>       file with symbols\n" );
}

static char *PutUInt( char *p, unsigned_16 data )
/***********************************************/
{
    CONV_LE_16( data );
    *((unsigned_16 *)p) = data;
    p += sizeof( unsigned_16 );
    return( p );
}

static char *PutOffset( char *p, unsigned_32 data )
/*************************************************/
{
    if( RecHdr[0] & 1 ) {
        CONV_LE_32( data );
        *((unsigned_32 *)p) = data;
        p += sizeof( unsigned_32 );
    } else {
        CONV_LE_16( data );
        *((unsigned_16 *)p) = (unsigned_16)data;
        p += sizeof( unsigned_16 );
    }
    return( p );
}

static char *PutIndex( char *p, unsigned_16 index )
/*************************************************/
{
    if( index > 0x7f )
        *p++ = ( index >> 8 ) | 0x80;
    *p++ = (char)index;
    return( p );
}

static byte create_chksum( void )
/*******************************/
{
    size_t  i;
    byte    chksum;

    chksum  = RecHdr[0];
    chksum += RecHdr[1];
    chksum += RecHdr[2];
    for( i = 0; i < WriteRecLen - 1; i++ ) {
        chksum += WriteRecBuff[i];
    }
    return( chksum );
}

static bool ExtendWriteRecBuff( unsigned_16 size )
/************************************************/
{
    if( WriteRecMaxLen < size ) {
        WriteRecMaxLen = size;
        if( WriteRecBuff != NULL ) {
            free( WriteRecBuff );
        }
        WriteRecBuff = malloc( WriteRecMaxLen );
        if( WriteRecBuff == NULL ) {
            printf( "**FATAL** Out of memory!\n" );
            return( false );
        }
    }
    return( true );
}

static bool WriteLNAMES( FILE *fo )
/*********************************/
{
    bool        ok;
    char        *n;

    SET_RECLEN( RecHdr, WriteRecLen );
    ok = ( fwrite( RecHdr, 1, 3, fo ) == 3 );
    if( ok ) {
        ok = ExtendWriteRecBuff( WriteRecLen );
        if( ok ) {
            WriteRecPtr = WriteRecBuff;
            RewindReadRec();
            while( IsDataToRead() ) {
                GetName();
                NameTermSave();
                n = SymbolExists( extdef_tab, NamePtr );
                if( n != NULL ) {
                    NameLen = n[0];
                    NamePtr = n + 1;
                }
                *WriteRecPtr++ = NameLen;
                memcpy( WriteRecPtr, NamePtr, NameLen );
                WriteRecPtr += NameLen;
                NameTermRest();
            }
            *WriteRecPtr = 0 - create_chksum();
            ok = ( fwrite( WriteRecBuff, 1, WriteRecLen, fo ) == WriteRecLen );
        }
    }
    return( ok );
}

static bool WriteEXTDEF( FILE *fo )
/*********************************/
{
    unsigned_16 indx;
    bool        ok;
    char        *n;

    SET_RECLEN( RecHdr, WriteRecLen );
    ok = ( fwrite( RecHdr, 1, 3, fo ) == 3 );
    if( ok ) {
        ok = ExtendWriteRecBuff( WriteRecLen );
        if( ok ) {
            WriteRecPtr = WriteRecBuff;
            RewindReadRec();
            while( IsDataToRead() ) {
                GetName();
                NameTermPtr();
                indx = GetIndex();
                NameTermData();
                n = SymbolExists( pubdef_tab, NamePtr );
                if( n != NULL ) {
                    NameLen = n[0];
                    NamePtr = n + 1;
                }
                *WriteRecPtr++ = NameLen;
                memcpy( WriteRecPtr, NamePtr, NameLen );
                WriteRecPtr += NameLen;
                NameTermRest();
                WriteRecPtr = PutIndex( WriteRecPtr, indx );
            }
            *WriteRecPtr = 0 - create_chksum();
            ok = ( fwrite( WriteRecBuff, 1, WriteRecLen, fo ) == WriteRecLen );
        }
    }
    return( ok );
}

static bool WritePUBDEF( FILE *fo )
/*********************************/
{
    unsigned_16 idx1;
    unsigned_16 idx2;
    unsigned_32 offs;
    bool        ok;
    char        *n;

    SET_RECLEN( RecHdr, WriteRecLen );
    ok = ( fwrite( RecHdr, 1, 3, fo ) == 3 );
    if( ok ) {
        ok = ExtendWriteRecBuff( WriteRecLen );
        if( ok ) {
            RewindReadRec();
            WriteRecPtr = WriteRecBuff;
            idx1 = GetIndex();
            WriteRecPtr = PutIndex( WriteRecPtr, idx1 );
            idx2 = GetIndex();
            WriteRecPtr = PutIndex( WriteRecPtr, idx2 );
            if( ( idx1 | idx2 ) == 0 )
                WriteRecPtr = PutUInt( WriteRecPtr, GetUInt() );
            while( IsDataToRead() ) {
                GetName();
                NameTermPtr();
                offs = GetOffset( RecHdr[0] & 1 );
                NameTermData();
                n = SymbolExists( pubdef_tab, NamePtr );
                if( n != NULL ) {
                    NameLen = n[0];
                    NamePtr = n + 1;
                }
                *WriteRecPtr++ = NameLen;
                memcpy( WriteRecPtr, NamePtr, NameLen );
                WriteRecPtr += NameLen;
                NameTermRest();
                WriteRecPtr = PutOffset( WriteRecPtr, offs );
                WriteRecPtr = PutIndex( WriteRecPtr, GetIndex() );
            }
            *WriteRecPtr = 0 - create_chksum();
            ok = ( fwrite( WriteRecBuff, 1, WriteRecLen, fo ) == WriteRecLen );
        }
    }
    return( ok );
}

static int ProcFile( FILE *fp, FILE *fo )
/**************************************/
{
//    unsigned_16   page_len;
//    unsigned_32   offset;
    bool            isChanged;
    bool            renameIt;
    char            *n;
    bool            ok;
    int             rc;

//    page_len = 0;
    ReadRecInit();
    WriteRecBuff = NULL;
    WriteRecMaxLen = 0;
    renameIt = false;
    ok = true;
    while( ok ) {
//        offset = ftell( fp );
        rc = ReadRec( fp, RecHdr );
        if( rc <= 0 ) {
            if( rc == 0 )
                ok = false;
            break;
        }
        WriteRecLen = GET_RECLEN( RecHdr );
        switch( RecHdr[0] & ~1 ) {
        case CMD_LNAMES:
            isChanged = false;
            while( IsDataToRead() ) {
                GetName();
                NameTermSave();
                n = SymbolExists( extdef_tab, NamePtr );
                if( n != NULL ) {
                    WriteRecLen += n[0] - NameLen;
                    isChanged = true;
                    renameIt = true;
                }
                NameTermRest();
            }
            if( isChanged ) {
                ok = WriteLNAMES( fo );
            } else {
                ok = WriteReadRec( fo, RecHdr );
            }
            break;
        case CMD_EXTDEF:
            isChanged = false;
            while( IsDataToRead() ) {
                GetName();
                NameTermSave();
                n = SymbolExists( pubdef_tab, NamePtr );
                if( n != NULL ) {
                    WriteRecLen += n[0] - NameLen;
                    isChanged = true;
                    renameIt = true;
                }
                NameTermRest();
                GetIndex();
            }
            if( isChanged ) {
                ok = WriteEXTDEF( fo );
            } else {
                ok = WriteReadRec( fo, RecHdr );
            }
            break;
        case CMD_PUBDEF:
            isChanged = false;
            if( ( GetIndex() | GetIndex() ) == 0 )
                GetUInt();
            while( IsDataToRead() ) {
                GetName();
                NameTermSave();
                n = SymbolExists( pubdef_tab, NamePtr );
                if( n != NULL ) {
                    WriteRecLen += n[0] - NameLen;
                    isChanged = true;
                    renameIt = true;
                }
                NameTermRest();
                GetOffset( RecHdr[0] & 1 );
                GetIndex();
            }
            if( isChanged ) {
                ok = WritePUBDEF( fo );
            } else {
                ok = WriteReadRec( fo, RecHdr );
            }
            break;
        case LIB_HEADER_REC:
            ok = false;
            break;
        default:
            ok = WriteReadRec( fo, RecHdr );
            break;
        }
    }
    free( WriteRecBuff );
    ReadRecFini();
    fclose( fo );
    fclose( fp );
    if( !ok )
        return( -1 );
    return( renameIt );
}

static bool process_symbol_file( const char *filename )
/*****************************************************/
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
        if( fp == NULL )
            return( false );
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
            if( len > 255 ) {
                len = 255;
                newname[len + 1] = '\0';
            }
            newname[0] = (byte)len;
            AddSymbol( pubdef_tab, old, newname, len + 2 );
        }
        fclose( fp );
    }
    return( true );
}

static bool process_lnames( char *cmd )
/*************************************/
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
        if( len > 255 ) {
            len = 255;
            newname[len + 1] = '\0';
        }
        newname[0] = (byte)len;
        memcpy( newname + 1, p2, len );
        newname[len + 1] = '\0';
        AddSymbol( extdef_tab, p1, newname, len + 2 );
        free( newname );
    }
    return( true );
}

static bool process_module( const char *filename )
/************************************************/
{
    FILE    *fp;
    FILE    *fo;
    int     rc;

    fp = fopen( filename, "rb" );
    if( fp == NULL ) {
        printf( "Cannot open input file: %s.\n", filename );
        return( false );
    }
    fo = fopen( "tmp.tmp", "wb" );
    if( fo == NULL ) {
        printf( "Cannot open input file: tmp.tmp.\n" );
        fclose( fp );
        return( false );
    }
    rc = ProcFile( fp, fo );
    if( rc == true ) {
        remove( filename );
        rename( "tmp.tmp", filename );
    } else {
        remove( "tmp.tmp" );
    }
    return( rc != -1 );
}

int main( int argc, char *argv[] )
/********************************/
{
    int         i;
    char        *fn;
    bool        ok;
    char        c;
    sym_file    *sf;

    ok = true;
    extdef_tab = SymbolInit();
    pubdef_tab = SymbolInit();
    for( i = 1; i < argc; ++i ) {
        if( argv[i][0] == '-' ) {
            c = tolower( (unsigned char)argv[i][1] );
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
    return( ok ? EXIT_SUCCESS : EXIT_FAILURE );
}
