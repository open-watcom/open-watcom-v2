/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Utility routines for OMF libraries.
*
****************************************************************************/


#include "wlib.h"
#include "roundmac.h"

#include "clibext.h"


static unsigned long    charCount;
static unsigned long    symCount;

static unsigned         PrimeNos[] = {
  2,      3,      5,      7,     11,     13,     17,     19,     23,     29,
 31,     37,     41,     43,     47,     53,     59,     61,     67,     71,
 73,     79,     83,     89,     97,    101,    103,    107,    109,    113,
127,    131,    137,    139,    149,    151,    157,    163,    167,    173,
179,    181,    191,    193,    197,    199,    211,    223,    227,    229,
233,    239,    241,    251,      0 /* terminator */
};

void InitOmfUtil( void )
{
    InitOmfRec();
    charCount = 0;
    symCount = 0;
}

void FiniOmfUtil( void )
{
    FiniOmfRec();
}

static unsigned_16 CheckForOverflow( unsigned long curr_offset )
{
    char buffer[10];

    curr_offset /= Options.page_size;
    if( curr_offset > (unsigned long)USHRT_MAX ) {
        sprintf( buffer, "%u", Options.page_size );
        FatalError( ERR_LIB_TOO_LARGE, buffer );
    }
    return( (unsigned_16)curr_offset );
}

void WriteOmfRecHeader( libfile io, unsigned_8 type, unsigned_16 len )
{
    LibWriteU8( io, type );
    LibWriteU16LE( io, len );
}

static int isPrime( unsigned num )
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

static bool InsertOmfDict( OmfLibBlock *lib_block, unsigned num_blocks, const char *sym, unsigned len, unsigned_16 offset )
{
    unsigned int    loc;
    unsigned int    entry_len;
    unsigned int    i;
    unsigned int    j;
    hash_entry      h;

    omflib_hash( sym, len, &h, num_blocks );

    /* + length byte */
    entry_len = __ROUND_UP_SIZE_EVEN( len + 1 ) + 2;
    for( i = 0; i < num_blocks; i++ ) {
        loc = lib_block[h.block].fflag * 2;
        for( j = 0; j < NUM_BUCKETS; j++ ) {
            if( lib_block[h.block].htab[h.bucket] == 0 ) {
                if( ( DIC_REC_SIZE - loc - 2 ) < entry_len ) {
                    lib_block[h.block].fflag = LIB_FULL_PAGE;
                    break;
                }
                lib_block[h.block].htab[h.bucket] = lib_block[h.block].fflag;
                lib_block[h.block].fflag += (unsigned_8)( entry_len / 2 );
                loc -= NUM_BUCKETS + 1;
                lib_block[h.block].name[loc] = (unsigned_8)len;
                loc++;
                memcpy( lib_block[h.block].name + loc, sym, len );
                loc += len;
                *(unsigned_16 *)( lib_block[h.block].name + loc ) = offset;
                return( true );
            }
            h.bucket += h.bucketd;
            if( h.bucket >= NUM_BUCKETS ) {
                h.bucket -= NUM_BUCKETS;
            }
        }
        h.block += h.blockd;
        if( h.block >= num_blocks ) {
            h.block -= num_blocks;
        }
   }
   return( false );
}


static bool HashOmfSymbols( OmfLibBlock *lib_block, unsigned num_blocks, sym_file *sfile )
{
    bool        ret;
    sym_entry   *sym;
    unsigned    str_len;
    const char  *cp;
    char        fname[256];     /* OMF maximum name len is 255 characters */

    ret = true;
    for( ; sfile != NULL; sfile = sfile->next ) {
        if( sfile->impsym == NULL ) {
            cp = MakeFName( sfile->full_name );
        } else {
#ifdef IMP_MODULENAME_DLL
            cp = sfile->impsym->dllName.name;
#else
            cp = sfile->impsym->u.omf_coff.symName;
#endif
        }
        str_len = 0;
        while( *cp != '\0' ) {
            fname[str_len++] = *cp++;
        }
        fname[str_len++] = '!';
        ret = InsertOmfDict( lib_block, num_blocks, fname, str_len, sfile->u.new_offset_omf );
        if( !ret ) {
            return( ret );
        }
        for( sym = sfile->first; sym != NULL; sym = sym->next ) {
            ret = InsertOmfDict( lib_block, num_blocks, sym->name, sym->len, sfile->u.new_offset_omf );
            if( !ret ) {
                return( ret );
            }
        }
    }
    return( ret );
}

unsigned WriteOmfDict( libfile io, sym_file *first_sfile )
/*********************************************************
 * return size of dict
 */
{
    bool        done;
    unsigned    num_blocks;
    OmfLibBlock *lib_block;
    size_t      dict_size;
    unsigned    int i;
    unsigned    int j;

    num_blocks = ( symCount + NUM_BUCKETS - 1 ) / NUM_BUCKETS;
    if( num_blocks < ( charCount + 3 + BLOCK_NAME_LEN - 1 ) / BLOCK_NAME_LEN )
        num_blocks = ( charCount + 3 + BLOCK_NAME_LEN - 1 ) / BLOCK_NAME_LEN;
    --num_blocks;

    lib_block = NULL;
    do {
        num_blocks++;
        num_blocks = NextPrime( num_blocks );
        if( num_blocks == 0 ) {
            MemFree( lib_block );
            return( 0 );
        }
        dict_size = num_blocks * sizeof( OmfLibBlock );
        lib_block = MemRealloc( lib_block, dict_size );
        memset( lib_block, 0, dict_size );
        for( i = 0; i < num_blocks; i++ ) {
            lib_block[i].fflag = ( NUM_BUCKETS + 1 ) / 2;
        }
        done = HashOmfSymbols( lib_block, num_blocks, first_sfile );
    } while( !done );
    for( i = 0; i < num_blocks; i++ ) {
        for( j = 0; j < NUM_BUCKETS; j++ ) {
           if( lib_block[i].htab[j] == 0 ) {
               break;
           }
        }
    }
    LibWrite( io, lib_block, dict_size );
    MemFree( lib_block );
    return( num_blocks );
}

void WriteOmfFile( libfile io, sym_file *sfile )
{
    sym_entry   *sym;
    const char  *fname;

    ++symCount;
    sfile->u.new_offset_omf = CheckForOverflow( LibTell( io ) );
    if( sfile->impsym == NULL ) {
        fname = MakeFName( sfile->full_name );
        /*
         * Options.page_size is always a power of 2 so someone should optimize
         * this sometime. maybe store page_size as a log
         */
    } else {
#ifdef IMP_MODULENAME_DLL
        fname = sfile->impsym->dllName.name;
#else
        fname = sfile->impsym->u.omf_coff.symName;
#endif
    }
    /*
     * the first data consists of the length of the name (one byte),
     * the name characters and '!' character
     * make sure whole data will be word aligned
     */
    charCount += __ROUND_UP_SIZE_EVEN( 1 + strlen( fname ) + 1 );
    WriteFileBody( io, sfile );
    for( sym = sfile->first; sym != NULL; sym = sym->next ) {
        ++symCount;
        /*
         * the next data consists of the length of the name (one byte)
         * and the name characters
         * make sure whole data will be word aligned
         */
        charCount += __ROUND_UP_SIZE_EVEN( 1 + sym->len );
    }
}
