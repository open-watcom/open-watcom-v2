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
* Description:  Utility routines for OMF libraries.
*
****************************************************************************/


#include "wlib.h"
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

static void CheckForOverflow(file_offset current)
{
    char buffer[ 10 ];

    if( current / Options.page_size > (unsigned long)USHRT_MAX ) {
        sprintf(buffer, "%u", Options.page_size);
        FatalError(ERR_LIB_TOO_LARGE, buffer);
    }
}

void PadOmf( bool force )
{
    unsigned    padding;
    char        *tmpbuf;

    // page size is always a power of 2
    // therefor x % Options.page_size == x & ( Options.page_size - 1 )

    padding = Options.page_size -( LibTell( NewLibrary ) & ( Options.page_size - 1 ) );
    if( padding != Options.page_size || force ) {
        tmpbuf = MemAlloc( padding );
        memset( tmpbuf, 0, padding );
        WriteNew( tmpbuf, padding );
        MemFree( tmpbuf);
    }
}

static int isPrime( unsigned num )
{
    unsigned *test_p;
    unsigned p;

    for( test_p = PrimeNos;; ++test_p ) {
        if( *test_p == 0 ) {
            return( -1 );
        }
        p = *test_p;
        if( ( p * p ) > num )
            break;
        if( ( num % p ) == 0 ) {
            return( FALSE );
        }
    }
    return( TRUE );
}


/*
 * Find the prime number of dictionary pages
 */
static unsigned NextPrime( unsigned maj )
{
    int test;

    if( maj > 2 ) {
        maj |= 1;               /* make it odd */
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

static bool InsertOmfDict( OmfLibBlock *lib_block, unsigned num_blocks, char *sym, unsigned len, unsigned_16 offset )
{
    unsigned int    loc;
    unsigned int    entry_len;
    unsigned int    i;
    unsigned int    j;
    hash_entry      h;

    omflib_hash( sym, len, &h, num_blocks );
    
    entry_len = (len | 1) + 3;
    for( i = 0; i < num_blocks; i++ ) {
        loc = lib_block[h.block].fflag * 2;
        for( j = 0; j < NUM_BUCKETS; j++ ) {
            if( lib_block[h.block].htab[h.bucket] == 0 ) {
                if( ( DIC_REC_SIZE - loc - 2 ) < entry_len ) {
                    lib_block[h.block].fflag = LIB_FULL_PAGE;
                    break;
                }
                lib_block[h.block].htab[h.bucket] = lib_block[h.block].fflag;
                lib_block[h.block].fflag += entry_len / 2;
                loc -= NUM_BUCKETS + 1;
                lib_block[h.block].name[loc] = len;
                loc++;
                memcpy( &(lib_block[h.block].name[loc]), sym, len );
                loc += len;
                *( (unsigned_16 *)&(lib_block[h.block].name[loc]) ) = offset;
                return( TRUE );
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
   return( FALSE );
}


static bool HashOmfSymbols( OmfLibBlock *lib_block, unsigned num_blocks, sym_file *sfile )
{
    bool        ret = TRUE;
    sym_entry   *sym;
    unsigned    str_len;
    char        *fname;

    for( ; sfile != NULL; sfile = sfile->next ) {
        if( sfile->import == NULL ) {
            fname = MakeFName( sfile->full_name );
        } else {
#ifdef IMP_MODULENAME_DLL
            fname = sfile->import->DLLName;
#else
            fname = sfile->import->u.sym.symName;
#endif
        }
        str_len = strlen( fname );
        fname[ str_len ] ='!';
        ret = InsertOmfDict( lib_block, num_blocks, fname,
            str_len + 1, sfile->new_offset );
        fname[ str_len ] = 0;
        if( ret == FALSE ) {
            return( ret );
        }
        for( sym = sfile->first; sym != NULL; sym = sym->next ) {
            ret = InsertOmfDict( lib_block, num_blocks, sym->name,
                sym->len, sfile->new_offset );
            if( ret == FALSE ) {
                return( ret );
            }
        }
    }
    return( ret );
}

//return size of dict
unsigned WriteOmfDict( sym_file *first_sfile )
{
    bool        done;
    unsigned    num_blocks;
    OmfLibBlock *lib_block;
    unsigned    dict_size;
    unsigned    int i;
    unsigned    int j;

    num_blocks = max( (symCount + NUM_BUCKETS - 1 ) / NUM_BUCKETS,
                    ( charCount + 3 + BLOCK_NAME_LEN - 1 ) / BLOCK_NAME_LEN ) -1;

    lib_block = NULL;
    do {
        num_blocks ++;
        num_blocks = NextPrime( num_blocks );
        if( num_blocks == 0 ) {
            return( 0 );
        }
        dict_size = num_blocks * sizeof( OmfLibBlock );
        lib_block = MemRealloc( lib_block, dict_size );
        memset( lib_block, 0, dict_size );
        for( i = 0; i < num_blocks; i++ ) {
            lib_block[ i ].fflag = ( NUM_BUCKETS + 1 ) / 2;
        }
        done = HashOmfSymbols( lib_block, num_blocks, first_sfile );
    } while( done == FALSE );
    for( i = 0; i < num_blocks; i++ ) {
        for( j = 0; j < NUM_BUCKETS; j++ ) {
           if( lib_block[ i ].htab[ j ] == 0 ) {
               break;
           }
        }
    }
    WriteNew( lib_block, dict_size );
    MemFree( lib_block );
    return( num_blocks );
}

void WriteOmfFile( sym_file *sfile )
{
    sym_entry   *sym;
    file_offset current;

    ++symCount;
    //add one for ! after name and make sure odd so whole name record will
    //be word aligned
    current = LibTell(NewLibrary);
    CheckForOverflow(current);
    sfile->new_offset = current / Options.page_size;
    if( sfile->import == NULL ) {
        charCount += ( strlen( MakeFName( sfile->full_name ) ) + 1 ) | 1;
        // Options.page_size is always a power of 2 so someone should optimize
        //this sometime. maybe store page_size as a log
    } else {
#ifdef IMP_MODULENAME_DLL
        charCount += ( strlen( sfile->import->DLLName ) + 1 ) | 1;
#else
        charCount += ( strlen( sfile->import->u.sym.symName ) + 1 ) | 1;
#endif
    }
    WriteFileBody( sfile );
    PadOmf( FALSE );
    for( sym = sfile->first; sym != NULL; sym = sym->next ) {
        ++symCount;
        charCount += sym->len | 1;
    }
}
