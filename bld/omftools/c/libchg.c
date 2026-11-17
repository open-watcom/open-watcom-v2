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
* Description:  Tool for changing LNAMES entries and modify
*               EXTDEF/PUBDEF based on a pattern.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include "bool.h"
#include "watcom.h"
#include "roundmac.h"
#include "pcobj.h"
#include "hashtab.h"
#include "misc.h"
#include "omfhash.h"
#include "pathgrp2.h"

#include "clibext.h"


#define MAX_LINE_LEN 512

#define SET_RECLEN(h,s) h[1] = (byte)s; h[2] = (s >> 8)

#define BLOCK_NAME_LEN      ( DIC_REC_SIZE - NUM_BUCKETS - 1 )

typedef enum {
    MOD_OK,
    MOD_LIB_END,
    MOD_PAGE_FULL,
    MOD_ERROR
} mod_status;

typedef enum {
    PASS_OK,
    PASS_PAGE_FULL,
    PASS_DICT_FULL,
    PASS_ERROR
} pass_status;

typedef struct sym_file {
    struct sym_file *next;
    char            *fname;
} sym_file;

typedef struct lname {
    struct lname    *next;
    unsigned char   len;
    char            name[1];
} lname;

typedef struct {
    lname           *names;
    lname           **names_end;
} lnames_list;

typedef struct {
    unsigned_8      htab[NUM_BUCKETS];
    unsigned_8      fflag;
    unsigned_8      name[BLOCK_NAME_LEN];
} OmfLibBlock;

typedef struct {
    OmfLibBlock     *lib_block;
    unsigned        num_blocks;
} dictionary;

static symbol       **pubdef_tab;
static symbol       **extdef_tab;
static unsigned_16  WriteRecLen;
static char         *WriteRecBuff;
static char         *WriteRecPtr;
static unsigned_16  WriteRecMaxLen;
static char         *symbol_name_pattern = NULL;
static size_t       symbol_name_pattern_len = 0;
static sym_file     *sym_files = NULL;
static bool         backup_opt = true;

static unsigned     PrimeNos[] = {
      2,      3,      5,      7,     11,     13,     17,     19,     23,     29,
     31,     37,     41,     43,     47,     53,     59,     61,     67,     71,
     73,     79,     83,     89,     97,    101,    103,    107,    109,    113,
    127,    131,    137,    139,    149,    151,    157,    163,    167,    173,
    179,    181,    191,    193,    197,    199,    211,    223,    227,    229,
    233,    239,    241,    251,      0 /* terminator */
};

static void usage( void )
/***********************/
{
    printf( "Usage: objchg <options> <list of object or library files>\n" );
    printf( "  <options> -b              don't create .bak file" );
    printf( "            -l=<old>=<new>  rename LNAMES item\n" );
    printf( "            -m=....         symbol name pattern\n" );
    printf( "            -s=<file>       file with symbols\n" );
}

static char *PutUInt( char *p, unsigned_16 data )
/***********************************************/
{
    MPUT_LE_16_UN( p, data );
    p += sizeof( unsigned_16 );
    return( p );
}

static char *PutOffset( char *p, unsigned_32 data )
/*************************************************/
{
    if( IS_CMD32() ) {
        MPUT_LE_32_UN( p, data );
        p += sizeof( unsigned_32 );
    } else {
        MPUT_LE_16_UN( p, data );
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
    unsigned_16 i;
    byte        chksum;

    chksum  = RecHdr[0];
    chksum += RecHdr[1];
    chksum += RecHdr[2];
    for( i = 0; i < WriteRecLen - 1; i++ ) {
        chksum += WriteRecBuff[i];
    }
    return( chksum );
}

static void dictFini( dictionary *dict )
{
    if( dict->lib_block != NULL ) {
        free( dict->lib_block );
        dict->lib_block = NULL;
    }
}

static void dictInit( dictionary *dict )
{
    dict->lib_block = NULL;
    dict->num_blocks = 0;
}

static bool dictCreate( dictionary *dict )
{
    unsigned    i;
    unsigned    size;

    if( dict->lib_block != NULL ) {
        free( dict->lib_block );
        dict->lib_block = NULL;
    }
    if( dict->num_blocks ) {
        size = dict->num_blocks * sizeof( OmfLibBlock );
        dict->lib_block = malloc( size );
        if( dict->lib_block == NULL ) {
            printf( "**FATAL** Out of memory!\n" );
            return( false );
        }
        memset( dict->lib_block, 0, size );
        for( i = 0; i < dict->num_blocks; i++ ) {
            dict->lib_block[i].fflag = ( NUM_BUCKETS + 1 ) / 2;
        }
    }
    return( true );
}

static int isPrime( unsigned num )
/********************************/
{
    unsigned *test_p;
    unsigned p;

    for( test_p = PrimeNos; (p = *test_p) != 0; ++test_p ) {
        if( ( p * p ) > num )
            return( 1 );
        if( ( num % p ) == 0 ) {
            return( 0 );
        }
    }
    return( -1 );
}

static unsigned NextPrime( unsigned maj )
/****************************************
 * Find the prime number of dictionary pages
 */
{
    int test;

    if( maj > 2 ) {
        /*
         * make it odd
         */
        maj |= 1;
        do {
            test = isPrime( maj );
            maj += 2;
        } while( test == 0 );
        maj -= 2;
        if( test == -1 ) {
            return( 0 );
        }
    }
    return( maj );
}

static bool InsertOmfDict( dictionary *dict, const char *sym, unsigned len, int pageno, bool *dict_full )
/*******************************************************************************************************/
{
    unsigned int    loc;
    unsigned int    entry_len;
    unsigned int    i;
    unsigned int    j;
    hash_entry      h;

    omflib_hash( sym, len, &h, dict->num_blocks );

    /* + length byte */
    entry_len = __ROUND_UP_SIZE_EVEN( len + 1 ) + 2;
    for( i = 0; i < dict->num_blocks; i++ ) {
        loc = dict->lib_block[h.block].fflag * 2;
        for( j = 0; j < NUM_BUCKETS; j++ ) {
            if( dict->lib_block[h.block].htab[h.bucket] == 0 ) {
                if( ( DIC_REC_SIZE - loc - 2 ) < entry_len ) {
                    dict->lib_block[h.block].fflag = LIB_FULL_PAGE;
                    break;
                }
                dict->lib_block[h.block].htab[h.bucket] = dict->lib_block[h.block].fflag;
                dict->lib_block[h.block].fflag += (unsigned_8)( entry_len / 2 );
                loc -= NUM_BUCKETS + 1;
                dict->lib_block[h.block].name[loc] = (unsigned_8)len;
                loc++;
                memcpy( dict->lib_block[h.block].name + loc, sym, len );
                loc += len;
                MPUT_LE_16_UN( dict->lib_block[h.block].name + loc, pageno );
                return( true );
            }
            h.bucket += h.bucketd;
            if( h.bucket >= NUM_BUCKETS ) {
                h.bucket -= NUM_BUCKETS;
            }
        }
        h.block += h.blockd;
        if( h.block >= dict->num_blocks ) {
            h.block -= dict->num_blocks;
        }
   }
   *dict_full = true;
   return( false );
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
    char        name[256];

    SET_RECLEN( RecHdr, WriteRecLen );
    ok = ( fwrite( RecHdr, 1, 3, fo ) == 3 );
    if( ok ) {
        ok = ExtendWriteRecBuff( WriteRecLen );
        if( ok ) {
            WriteRecPtr = WriteRecBuff;
            RewindReadRec();
            while( IsDataToRead() ) {
                GetNameBuf( name );
                n = SymbolExists( extdef_tab, NamePtr );
                if( n != NULL ) {
                    NameLen = GET_LEN( n );
                    NamePtr = n + 1;
                }
                *WriteRecPtr++ = NameLen;
                memcpy( WriteRecPtr, NamePtr, NameLen );
                WriteRecPtr += NameLen;
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
    char        name[256];

    SET_RECLEN( RecHdr, WriteRecLen );
    ok = ( fwrite( RecHdr, 1, 3, fo ) == 3 );
    if( ok ) {
        ok = ExtendWriteRecBuff( WriteRecLen );
        if( ok ) {
            WriteRecPtr = WriteRecBuff;
            RewindReadRec();
            while( IsDataToRead() ) {
                GetNameBuf( name );
                indx = GetIndex();
                n = SymbolExists( pubdef_tab, NamePtr );
                if( n != NULL ) {
                    NameLen = GET_LEN( n );
                    NamePtr = n + 1;
                }
                *WriteRecPtr++ = NameLen;
                memcpy( WriteRecPtr, NamePtr, NameLen );
                WriteRecPtr += NameLen;
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
    char        name[256];

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
                GetNameBuf( name );
                offs = GetOffset();
                n = SymbolExists( pubdef_tab, NamePtr );
                if( n != NULL ) {
                    NameLen = GET_LEN( n );
                    NamePtr = n + 1;
                }
                *WriteRecPtr++ = NameLen;
                memcpy( WriteRecPtr, NamePtr, NameLen );
                WriteRecPtr += NameLen;
                WriteRecPtr = PutOffset( WriteRecPtr, offs );
                WriteRecPtr = PutIndex( WriteRecPtr, GetIndex() );
            }
            *WriteRecPtr = 0 - create_chksum();
            ok = ( fwrite( WriteRecBuff, 1, WriteRecLen, fo ) == WriteRecLen );
        }
    }
    return( ok );
}

#if 0
static bool writeLIBSTR()
{
    WriteRecLen = page_len - 3;
    SET_RECLEN( RecHdr, WriteRecLen );
    ok = ( fwrite( RecHdr, 1, 3, fo ) == 3 );
    ok = ExtendWriteRecBuff( WriteRecLen );
    memset( WriteRecBuff, 0, WriteRecLen );
    WriteRecBuff[6] = flags;
    ok = ( fwrite( WriteRecBuff, 1, WriteRecLen, fo ) == WriteRecLen );
}
#endif

static bool WriteLIBTrailer( FILE *fo )
/**************************************
 * output OMF library end page
 *
 * struct {
 *     // OMF record header
 *     unsigned_8  type;
 *     unsigned_16 len;
 *     // OMF Library trailer
 *     ...
 *     zeros up to dictionary page size alignment (512 bytes)
 *     ...
 * }
 *
 * it doesn't use omfRec because it is OMF record without check sum
 * and it is filled by zeros up to full size (aligned to 512 bytes)
 */
{
    long        offset;

    /*
     * output OMF record header
     */
    RecHdr[0] = LIB_TRAILER_REC;
    offset = ftell( fo );
    if( offset == -1 )
        return( false );
    WriteRecLen = -( offset + sizeof( RecHdr ) ) & ( DIC_REC_SIZE - 1 );
    SET_RECLEN( RecHdr, WriteRecLen );
    if( fwrite( RecHdr, 1, sizeof( RecHdr ), fo ) != sizeof( RecHdr ) )
        return( false );
    /*
     * output OMF Library trailer data (all zero)
     */
    if( !ExtendWriteRecBuff( WriteRecLen ) )
        return( false );
    memset( WriteRecBuff, 0, WriteRecLen );
    return( fwrite( WriteRecBuff, 1, WriteRecLen, fo ) == WriteRecLen );
}

static bool WriteLIBHeader( FILE *fo, int page_len, unsigned_32 dict_offset, unsigned_16 dict_size, unsigned_8 lib_flags )
/*************************************************************************************************************************
 * output OMF library first page
 *
 * struct {
 *     // OMF record header
 *     unsigned_8  type;
 *     unsigned_16 page_size;  //really page size - 3
 *     // OMF Library header
 *     unsigned_32 dict_offset;
 *     unsigned_16 dict_size;
 *     unsigned_8  lib_flags;
 *     ...
 *     zeros up to OMF library page size
 *     ...
 * }
 *
 * it doesn't use omfRec because it is OMF record without check sum
 * and it is used as overlay for already zeroed OMF library full page size
 */
{
    RecHdr[0] = LIB_HEADER_REC;
    WriteRecLen = (unsigned_16)( page_len - (int)sizeof( RecHdr ) );
    SET_RECLEN( RecHdr, WriteRecLen );
    if( fwrite( RecHdr, 1, sizeof( RecHdr ), fo ) != sizeof( RecHdr ) )
        return( false );
    /*
     * output OMF library header data with trailing zeros
     */
    if( !ExtendWriteRecBuff( WriteRecLen ) )
        return( false );
    memset( WriteRecBuff, 0, WriteRecLen );
    MPUT_LE_32( WriteRecBuff, dict_offset );
    MPUT_LE_16( WriteRecBuff + sizeof( uint_32 ), dict_size );
    WriteRecBuff[sizeof( uint_32 ) + sizeof( uint_16 )] = lib_flags;
    return( fwrite( WriteRecBuff, 1, WriteRecLen, fo ) == WriteRecLen );
}

static bool WriteDictionary( FILE *fo, const dictionary *dict )
/*************************************************************/
{
    size_t size;

    size = (size_t)dict->num_blocks * sizeof( OmfLibBlock );
    return( fwrite( dict->lib_block, 1, size, fo ) == size );
}

static bool UpdateLIBHeader( FILE *fo, unsigned_32 dict_offset, unsigned_16 dict_size )
/**************************************************************************************
 * update OMF library header (dictionary info)
 */
{
    if( fseek( fo, sizeof( RecHdr ), SEEK_SET ) )
        return( false );
    CONV_LE_32( dict_offset );
    if( fwrite( &dict_offset, 1, sizeof( dict_offset ), fo ) != sizeof( dict_offset ) )
        return( false );
    CONV_LE_16( dict_size );
    return( fwrite( &dict_size, 1, sizeof( dict_size ), fo ) == sizeof( dict_size ) );
}

static void lnamesInit( lnames_list *lnames )
/*******************************************/
{
    lnames->names = NULL;
    lnames->names_end = NULL;
}

static void lnamesFini( lnames_list *lnames )
/*******************************************/
{
    lname   *ln;

    while( (ln = lnames->names) != NULL ) {
        lnames->names = ln->next;
        free( ln );
    }
    lnames->names_end = NULL;
}

static bool add_lname( lnames_list *lnames, char *name, unsigned_8 len )
/**********************************************************************/
{
    lname   *ln;

    ln = malloc( sizeof( lname ) + len );
    if( ln == NULL ) {
        printf( "**FATAL** Out of memory!\n" );
        return( false );
    }
    ln->len = len;
    strncpy( ln->name, name, len );
    ln->name[len] = '\0';
    ln->next = NULL;
    if( lnames->names_end == NULL ) {
        lnames->names = NULL;
        lnames->names_end = &lnames->names;
    }
    *lnames->names_end = ln;
    lnames->names_end = &ln->next;
    return( true );
}

static bool InsertOmfDictIdxLName( dictionary *dict, lnames_list *lnames, int pageno, bool *dict_full )
{
    unsigned        idx;
    lname           *ln;

    idx = GetIndex();   /* public name (LNAMES) */
    if( idx ) {
        idx--;
        /*
         * get LNAME from linked list
         */
        ln = lnames->names;
        while( idx-- > 0 ) {
            if( ln == NULL )
                return( false );
            ln = ln->next;
        }
        if( ln == NULL )
            return( false );
        InsertOmfDict( dict, ln->name, ln->len, pageno, dict_full );
    }
    return( true );
}

static void GetComLen( void )
{
    switch( GetByte() ) {
    case COMDEF_LEAF_4:
        ReadSkip( 4 );
        break;
    case COMDEF_LEAF_3:
        ReadSkip( 3 );
        break;
    case COMDEF_LEAF_2:
        ReadSkip( 2 );
        break;
    default:
        break;
    }
}

static mod_status ProcessModule( FILE *fi, FILE *fo,
                                 dictionary *dict,
                                 int page_len_in,
                                 int page_len,
                                 bool *dict_full,
                                 bool *changed )
/***************************************************
 * process single module and align position in
 * input and output files to next module start
 *
 * returns:
 *  MOD_OK:         the complete module has been processed and both streams
 *                      have been aligned
 *  MOD_LIB_END:    a trailer was loaded instead of the next module
 *  MOD_PAGE_FULL:  another module exists, but its page number can no longer
 *                      be stored
 *  MOD_ERROR:      a read or write operation failed
 */
{
    long            offset;
    lnames_list     lnames;
    int             pageno;
    mod_status      status;
    bool            done;
    char            name[256];
    char            *n;
    byte            byte;
    bool            rec_changed;
    int             cmd;

    status = MOD_ERROR;
    offset = ftell( fo );
    if( offset == -1 )
        return( status );
    if( offset / page_len > USHRT_MAX ) {
        status = MOD_PAGE_FULL;
    }
    pageno = (int)( offset / page_len );
    lnamesInit( &lnames );
    done = false;
    while( !done ) {
        if( ReadRec( fi ) <= 0 ) {
            status = MOD_ERROR;
            break;
        }
        WriteRecLen = GET_RECLEN();
        cmd = GET_RECCMD();
        if( cmd == LIB_TRAILER_REC ) {
            status = MOD_LIB_END;
            done = true;
        } else if( status == MOD_PAGE_FULL ) {
            done = true;
        } else {
            switch( cmd ) {
            case CMD_THEADR:
                GetNameBuf( name );
                if( NameLen > 254 )
                    NameLen = 254;
                NamePtr[NameLen++] = '!';
                InsertOmfDict( dict, NamePtr, NameLen, pageno, dict_full );
                if( !WriteReadRec( fo ) ) {
                    done = true;
                }
                break;
            case CMD_LNAMES:
                rec_changed = false;
                while( IsDataToRead() ) {
                    GetNameBuf( name );
                    n = SymbolExists( extdef_tab, NamePtr );
                    if( n != NULL ) {
                        WriteRecLen += GET_LEN( n ) - NameLen;
                        *changed = rec_changed = true;
                        NameLen = GET_LEN( n );
                        NamePtr = n + 1;
                    }
                    if( !add_lname( &lnames, NamePtr, NameLen ) ) {
                        done = true;
                        break;
                    }
                }
                if( !done ) {
                    if( rec_changed ) {
                        if( !WriteLNAMES( fo ) ) {
                            done = true;
                        }
                    } else {
                        if( !WriteReadRec( fo ) ) {
                            done = true;
                        }
                    }
                }
                break;
            case CMD_EXTDEF:
                rec_changed = false;
                while( IsDataToRead() ) {
                    GetNameBuf( name );
                    n = SymbolExists( pubdef_tab, NamePtr );
                    if( n != NULL ) {
                        WriteRecLen += GET_LEN( n ) - NameLen;
                        *changed = rec_changed = true;
                    }
                    GetIndex();
                }
                if( rec_changed ) {
                    if( !WriteEXTDEF( fo ) ) {
                        done = true;
                    }
                } else {
                    if( !WriteReadRec( fo ) ) {
                        done = true;
                    }
                }
                break;
            case CMD_PUBDEF:
            case CMD_PUBDEF32:
                rec_changed = false;
                if( ( GetIndex() | GetIndex() ) == 0 )
                    GetUInt();
                while( IsDataToRead() ) {
                    GetNameBuf( name );
                    n = SymbolExists( pubdef_tab, NamePtr );
                    if( n != NULL ) {
                        WriteRecLen += GET_LEN( n ) - NameLen;
                        *changed = rec_changed = true;
                        NameLen = GET_LEN( n );
                        NamePtr = n + 1;
                    }
                    InsertOmfDict( dict, NamePtr, NameLen, pageno, dict_full );
                    GetOffset();
                    GetIndex();
                }
                if( rec_changed ) {
                    if( !WritePUBDEF( fo ) ) {
                        done = true;
                    }
                } else {
                    if( !WriteReadRec( fo ) ) {
                        done = true;
                    }
                }
                break;
            case CMD_COMDEF:
                while( IsDataToRead() ) {
                    GetNameBuf( name );     /* name */
                    InsertOmfDict( dict, NamePtr, NameLen, pageno, dict_full );
                    GetIndex();             /* type index */
                    switch( GetByte() ) {   /* data type */
                    case COMDEF_FAR:
                        GetComLen();        /* number of element, size of element */
                        /* fall through */
                    case COMDEF_NEAR:
                        GetComLen();        /* size */
                        break;
                    }
                }
                if( !WriteReadRec( fo ) ) {
                    done = true;
                }
                break;
            case CMD_COMDAT:
            case CMD_COMDAT32:
                byte = GetByte();       /* flags */
                if( (byte & (COMDAT_CONTINUE | COMDAT_LOCAL)) == 0 ) {
                    byte = GetByte();   /* attributes */
                    GetByte();          /* align */
                    GetOffset();
                    GetIndex();         /* type index */
                    if( (byte & COMDAT_ALLOC_MASK) == COMDAT_EXPLICIT ) {
                        if( (GetIndex() | GetIndex()) == 0 ) {  /* base index */
                            GetUInt();                          /* base frame */
                        }
                    }
                    /* Publi base */
                    if( !InsertOmfDictIdxLName( dict, &lnames, pageno, dict_full ) ) {
                        done = true;
                        break;
                    }
                }
                if( !WriteReadRec( fo ) ) {
                    done = true;
                }
                break;
            case CMD_ALIAS:
                while( IsDataToRead() ) {
                    GetNameBuf( name );
                    InsertOmfDict( dict, NamePtr, NameLen, pageno, dict_full );
                    GetNameBuf( name );
                }
                if( !WriteReadRec( fo ) ) {
                    done = true;
                }
                break;
            case CMD_MODEND:
            case CMD_MODEND32:
                if( WriteReadRec( fo )              /* copy MODEND record */
                  && AlignFile( fi, page_len_in )   /* align input file */
                  && AlignFile( fo, page_len ) ) {  /* align output file */
                    status = MOD_OK;
                }
                done = true;
                break;
            default:
                if( !WriteReadRec( fo ) ) {
                    done = true;
                }
                break;
            }
        }
    }
    lnamesFini( &lnames );
    return( status );
}

static pass_status ProcessLibraryPass( FILE *fi, FILE *fo,
                                       dictionary *dict,
                                       int page_len_in,
                                       int page_len,
                                       unsigned_8 lib_flags,
                                       bool *changed )
/****************************************************************
 * process one complete library pass
 *
 * The output header contains placeholder dictionary information.
 * The dictionary itself and its final header values are written by
 * ProcFile() only after a successful pass.
 */
{
    mod_status      mod_rc;
    bool            dict_full;

    if( !WriteLIBHeader( fo, page_len, 0, 0, lib_flags ) )
        return( PASS_ERROR );

    dict_full = false;
    for( ;; ) {
        mod_rc = ProcessModule( fi, fo, dict, page_len_in, page_len, &dict_full, changed );
        switch( mod_rc ) {
        case MOD_OK:
            break;
        case MOD_LIB_END:
            if( !WriteLIBTrailer( fo ) )
                return( PASS_ERROR );
            if( dict_full )
                return( PASS_DICT_FULL );
            return( PASS_OK );
        case MOD_PAGE_FULL:
            return( PASS_PAGE_FULL );
        case MOD_ERROR:
        default:
            return( PASS_ERROR );
        }
    }
}

static void WriteRecInit( void )
{
    WriteRecBuff = NULL;
    WriteRecMaxLen = 0;
}

static void WriteRecFini( void )
{
    if( WriteRecBuff != NULL ) {
        free( WriteRecBuff );
        WriteRecBuff = NULL;
        WriteRecMaxLen = 0;
    }
}

static int ProcFile( FILE *fi, const char *tmp )
/**********************************************/
{
    int             page_len_in;
    int             page_len;
    int             rc;
    int             result;
    long            fi_start;
    long            offset;
    unsigned        new_blocks;
    unsigned_32     dict_offset;
    unsigned_8      lib_flags;
    dictionary      dict;
    pass_status     pass_rc;
    bool            changed;
    bool            finished;
    FILE            *fo;

    ReadRecInit();
    WriteRecInit();
    dictInit( &dict );
    fo = NULL;
    result = -1;
    finished = false;
    page_len_in = 0;
    page_len = 0;
    lib_flags = 0;
    fi_start = 0;

    rc = ReadRec( fi );
    if( rc <= 0 ) {
        if( rc == 0 )
            result = -1;
        finished = true;
    } else if( RecHdr[0] != LIB_HEADER_REC ) {
        finished = true;
    } else {
        /*
         * Get the original library parameters.  page_len remains an int;
         * it is narrowed only where the OMF header field is serialized.
         */
        page_len = page_len_in = GET_RECLEN() - 1 + 4;
        dict.num_blocks = MGET_LE_U16( GetReadPtr() + 4 );
        lib_flags = GetReadPtr()[6];
        fi_start = ftell( fi );
        if( fi_start == -1 ) {
            finished = true;
        }
    }

    while( !finished ) {
        if( fseek( fi, fi_start, SEEK_SET ) ) {
            break;
        }
        if( !dictCreate( &dict ) ) {
            break;
        }

        fo = fopen( tmp, "wb" );
        if( fo == NULL ) {
            printf( "Cannot open output file: %s.\n", tmp );
            break;
        }

        changed = false;
        pass_rc = ProcessLibraryPass( fi, fo, &dict, page_len_in, page_len, lib_flags, &changed );
        if( pass_rc == PASS_OK ) {
            offset = ftell( fo );
            if( offset == -1 ) {
                pass_rc = PASS_ERROR;
            } else {
                dict_offset = (unsigned_32)offset;
                if( !WriteDictionary( fo, &dict )
                  || !UpdateLIBHeader( fo, dict_offset, (unsigned_16)dict.num_blocks ) ) {
                    pass_rc = PASS_ERROR;
                }
            }
        }

        if( fclose( fo ) != 0 && pass_rc == PASS_OK )
            pass_rc = PASS_ERROR;
        fo = NULL;

        switch( pass_rc ) {
        case PASS_OK:
            result = changed;
            finished = true;
            break;
        case PASS_PAGE_FULL:
            if( page_len > ( USHRT_MAX + (int)sizeof( RecHdr ) ) / 2 ) {
                printf( "OMF library page size is too large.\n" );
                finished = true;
            } else {
                page_len *= 2;
            }
            break;
        case PASS_DICT_FULL:
            if( dict.num_blocks >= USHRT_MAX ) {
                printf( "OMF library dictionary is too large.\n" );
                finished = true;
                break;
            }
            new_blocks = NextPrime( dict.num_blocks + 1 );
            if( new_blocks == 0 || new_blocks > USHRT_MAX ) {
                printf( "OMF library dictionary is too large.\n" );
                finished = true;
            } else {
                dict.num_blocks = new_blocks;
            }
            break;
        case PASS_ERROR:
        default:
            finished = true;
            break;
        }
    }

    if( fo != NULL )
        fclose( fo );
    dictFini( &dict );
    WriteRecFini();
    ReadRecFini();
    fclose( fi );
    return( result );
}

static bool process_symbol_file( const char *filename )
/*****************************************************/
{
    FILE    *fp;
    char    line[MAX_LINE_LEN];
    char    newname[MAX_LINE_LEN];
    char    *oldname;
    char    *p;
    size_t  i;
    size_t  len;

    if( filename != NULL ) {
        fp = fopen( filename, "rt" );
        if( fp == NULL )
            return( false );
        while( fgets( line, sizeof( line ), fp ) != NULL ) {
            oldname = strtok( line, " \t=\r\n" );
            if( oldname == NULL )
                continue;
            len = strlen( oldname );
            if( len > 255 )
                len = 255;
            oldname[len] = '\0';
            if( SymbolExists( pubdef_tab, oldname ) != NULL )
                continue;
            p = newname + 1;
            for( i = 0; i < symbol_name_pattern_len; i++ ) {
                if( symbol_name_pattern[i] == '*' ) {
                    memcpy( p, oldname, len );
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
            AddSymbol( pubdef_tab, oldname, newname, len + 2 );
        }
        fclose( fp );
    }
    return( true );
}

static bool process_lnames( char *cmd )
/*************************************/
{
    char    *newname;
    size_t  len;
    char    oldname[256];

    newname = strchr( cmd, '=' );
    if( newname == NULL )
        return( false );
    len = newname - cmd;
    if( len > 255 )
        len = 255;
    strncpy( oldname, cmd, len );
    oldname[len] = '\0';
    len = strlen( newname ) - 1;
    if( len > 255 )
        len = 255;
    *newname = (byte)len;
    newname[1 + len] = '\0';
    AddSymbol( extdef_tab, oldname, newname, len + 2 );
    return( true );
}

static bool process_library( const char *filename )
/*************************************************/
{
    FILE        *fi;
    int         rc;
    char        *backup_file;
    char        *tmp;
    pgroup2     pg;
    bool        ok;
    size_t      backup_file_len;
    size_t      tmp_len;
    int         msg;

    _splitpath2( filename, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );

    /*
     * Space for the original path, with individual sections replaced.
     */
    backup_file_len = 0;
    if( backup_opt )
        backup_file_len = strlen( filename ) - strlen( pg.ext ) + sizeof( ".bak" );
    tmp_len = strlen( filename ) - strlen( pg.fname ) - strlen( pg.ext ) + sizeof( "tmp.tmp" );

    backup_file = NULL;
    tmp = NULL;
    ok = false;

    /*
     * Replace the original file name and extension by "tmp.tmp".
     */
    tmp = malloc( tmp_len );
    if( tmp == NULL ) {
        printf( "**FATAL** Out of memory while creating temporary file name.\n" );
        return( false );
    }
    _makepath( tmp, pg.drive, pg.dir, "tmp.tmp", NULL );

    /*
     * Do not let an old temporary file affect the current operation.
     */
    if( remove( tmp ) != 0 && errno != ENOENT ) {
        printf( "Cannot remove old temporary file '%s': %s.\n", tmp, strerror( errno ) );
        free( tmp );
        return( false );
    }

    fi = fopen( filename, "rb" );
    if( fi == NULL ) {
        printf( "Cannot open input library '%s': %s.\n",
                filename, strerror( errno ) );
        free( tmp );
        return( false );
    }

    rc = ProcFile( fi, tmp );

    if( rc == -1 ) {
        printf( "Processing of library '%s' failed.\n", filename );
        remove( tmp );
        free( tmp );
        return( false );
    }

    if( rc == 0 ) {
        /*
         * No replacement is needed.
         */
        remove( tmp );
        free( tmp );
        return( true );
    }

    msg = 0;
    if( backup_opt ) {
        backup_file = malloc( backup_file_len );
        if( backup_file == NULL ) {
            printf( "**FATAL** Out of memory while creating backup file name.\n" );
            remove( tmp );
            free( tmp );
            return( false );
        }
        _makepath( backup_file, pg.drive, pg.dir, pg.fname, ".bak" );
        /*
         * Remove a backup left by an earlier successful invocation.
         * A missing backup is not an error.
         */
        if( remove( backup_file ) != 0 && errno != ENOENT ) {
            printf( "Cannot remove old backup file '%s': %s.\n",
                    backup_file, strerror( errno ) );
            msg = 1;
        } else if( rename( filename, backup_file ) != 0 ) {
            printf( "Cannot rename original library '%s' to backup '%s': %s.\n",
                    filename, backup_file, strerror( errno ) );
            msg = 1;
        } else if( rename( tmp, filename ) == 0 ) {
            ok = true;
        } else {
            int install_errno;

            /*
             * Preserve the installation error because the rollback rename
             * can change errno.
             */
            install_errno = errno;

            if( rename( backup_file, filename ) ) {
                msg = 2;
                printf( "**FATAL** Cannot restore backup '%s' as '%s': %s.\n",
                        backup_file, filename, strerror( errno ) );
            } else {
                msg = 1;
                printf( "Cannot install temporary library '%s' as '%s': %s.\n",
                    tmp, filename, strerror( install_errno ) );
            }
        }
        free( backup_file );
    } else {
        if( remove( filename ) != 0 && errno != ENOENT ) {
            printf( "Cannot remove library file '%s': %s.\n", filename, strerror( errno ) );
            msg = 1;
        } else if( rename( tmp, filename ) == 0 ) {
            ok = true;
        } else {
            printf( "Cannot install temporary library '%s' as '%s': %s.\n",
                    tmp, filename, strerror( errno ) );
            msg = 3;
        }
    }
    if( ok ) {
        printf( "Library '%s' was updated successfully.\n", filename );
        if( backup_opt ) {
            printf( "The original library was preserved as '%s'.\n", backup_file );
        }
    } else if( msg == 1 ) {
        printf( "The original library remains available as '%s'.\n", filename );
        printf( "The generated library remains available as '%s'.\n", tmp );
    } else if( msg == 2 ) {
        printf( "The original library remains available as '%s'.\n", backup_file );
        printf( "The generated library remains available as '%s'.\n", tmp );
    } else if( msg == 3 ) {
        printf( "The original library '%s' was lost.\n", filename );
        printf( "The generated library remains available as '%s'.\n", tmp );
    }
    free( tmp );

    return( ok );
}

int main( int argc, char *argv[] )
/********************************/
{
    int         i;
    char        *fn;
    bool        ok;
    int         c;
    sym_file    *sf;

    ok = true;
    extdef_tab = SymbolInit();
    pubdef_tab = SymbolInit();
    for( i = 1; i < argc; ++i ) {
        if( argv[i][0] == '-' ) {
            c = tolower( (unsigned char)argv[i][1] );
            if( c == 'b' && argv[i][2] == '\0' ) {
                backup_opt = false;
            } else if( c == 'l' && argv[i][2] == '=' ) {
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
                if( sf != NULL ) {
                    sf->next = NULL;
                    sf->fname = argv[i] + 3;
                }
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
                ok &= process_library( fn );
                fn = DoWildCard( NULL );
            }
            DoWildCardClose();
        }
    }
    SymbolFini( pubdef_tab );
    SymbolFini( extdef_tab );
    return( ok ? EXIT_SUCCESS : EXIT_FAILURE );
}
