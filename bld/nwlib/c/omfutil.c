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


#include "wlib.h"
static OmfRecord    *omfRec;
static unsigned     omfRecLen;
static unsigned long charCount;
static unsigned long symCount;


static unsigned                         PrimeNos[] = {
  2,      3,      5,      7,     11,     13,     17,     19,     23,     29,
 31,     37,     41,     43,     47,     53,     59,     61,     67,     71,
 73,     79,     83,     89,     97,    101,    103,    107,    109,    113,
127,    131,    137,    139,    149,    151,    157,    163,    167,    173,
179,    181,    191,    193,    197,    199,    211,    223,    227,    229,
233,    239,    241,    251,      0 /* terminator */
};

void InitOmfUtil()
{
    omfRec = MemAlloc( INIT_OMF_REC_SIZE );
    omfRecLen = INIT_OMF_REC_SIZE;
    charCount = 0;
    symCount = 0;
}

void FiniOmfUtil()
{
    MemFree( omfRec );
    omfRecLen = 0;
}


static void CheckForOverflow(file_offset current)
{
    char buffer[10];

    if ((current / Options.page_size) > (unsigned long)(USHRT_MAX)) {
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

static bool ReadOmfRecord( libfile io )
{
    if( LibRead( io, omfRec, 3 ) != 3 ) {
        return( FALSE );
    }

    if ( omfRec->basic.len + 3 > omfRecLen ){
        OmfRecord *new;
        omfRecLen = omfRec->basic.len + 3;
        new = MemAlloc( omfRecLen );
        new->basic.len = omfRec->basic.len;
        new->basic.type = omfRec->basic.type;
        MemFree( omfRec );
        omfRec = new;
    }
    if( LibRead( io, ( omfRec )->basic.contents, omfRec->basic.len ) != omfRec->basic.len ) {
        return( FALSE );
    }
    return( TRUE );
}

static void WriteOmfRecord( )
{
    WriteNew( omfRec, omfRec->basic.len + 3 );
}

static void WriteTimeStamp( sym_file *file )
{
    OmfRecord   rec;
    unsigned_8  sum;
    unsigned    i;

    rec.time.type = CMD_COMENT;
    rec.time.len = sizeof( rec.time ) - 3;
    rec.time.attribute = CMT_TNP | CMT_TNL;
    rec.time.class = CMT_LINKER_DIRECTIVE;
    rec.time.subclass = LDIR_OBJ_TIMESTAMP;
    rec.time.stamp = file->arch.date;
    rec.time.chksum = 0;
    sum = 0;
    for( i = 0; i < sizeof( rec.time ); ++i ) {
        sum += rec.chkcalc[i];
    }
    rec.time.chksum = -sum;
    WriteNew( &rec, sizeof( rec.time ) );
}

void WriteOmfLibTrailer()
{
    unsigned    size;

    size = DIC_REC_SIZE - LibTell( NewLibrary ) % DIC_REC_SIZE;
    if( omfRecLen < size ){
        omfRecLen = size;
        MemFree( omfRec );
        omfRec = MemAlloc( size );
    }
    memset( omfRec, 0, size );
    omfRec->basic.type = LIB_TRAILER_REC;
    omfRec->basic.len = size - 3;
    WriteOmfRecord();
}

void WriteOmfLibHeader( unsigned_32 dict_offset, unsigned_16 dict_size )
{
    OmfRecord  rec;     // i didn't use omfRec because page size can be quite big
    LibSeek( NewLibrary, 0, SEEK_SET );
    rec.lib_header.type = LIB_HEADER_REC;
    rec.lib_header.page_size = Options.page_size - 3;
    rec.lib_header.dict_offset = dict_offset;
    rec.lib_header.dict_size = dict_size;
    if( Options.respect_case ){
        rec.lib_header.flags = 1;
    } else {
        rec.lib_header.flags = 0;
    }
    WriteNew( &rec, sizeof( rec.lib_header ) );
}

static int isPrime( unsigned num )
{
    unsigned *test_p;
    unsigned p;

    for( test_p = PrimeNos;; ++test_p ) {
        if( *test_p == 0 ){
            return ( -1 );
        }
        p = *test_p;
        if(( p * p ) > num ) break;
        if(( num % p ) == 0 ) {
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

    if( maj > 2 )  {
        maj |= 1;               /* make it odd */
        do{
            test = isPrime( maj );
            maj += 2;
        } while( test == 0 );
        maj -= 2;
        if ( test == -1 ){
            return( 0 );
        }
    }
    return( maj );
}

#define _rotl( a, b )   ( ( a << b ) | ( a >> ( 16 - b ) ) )
#define _rotr( a, b )   ( ( a << ( 16 - b ) ) | ( a  >> b ) )

static bool InsertOmfDict( OmfLibBlock *lib_block, unsigned num_blocks,
    char *sym, unsigned len, unsigned_16 offset )
{
    char            *leftptr;
    char            *rightptr;
    unsigned_16     curr;
    unsigned_16     block;
    unsigned_16     bucket;
    unsigned_16     dblock;
    unsigned_16     dbucket;
    unsigned        count;
    unsigned int    loc;
    unsigned int    entry_len;
    unsigned int    i;
    unsigned int    j;

    count = len;
    leftptr = sym;
    rightptr = leftptr + count;
    block = count | 0x20;
    dblock = 0;
    bucket = 0;
    dbucket = count | 0x20;
    for( ;; ) {
        --rightptr;
        /* zap to lower case (sort of) */
        curr = *rightptr | 0x20;
        dblock = curr ^ _rotl( dblock, 2 );
        bucket = curr ^ _rotr( bucket, 2 );
        if( --count == 0 ) {
            break;
        }
        curr = *leftptr | 0x20;
        ++leftptr;
        block = curr ^ _rotl( block, 2 );
        dbucket = curr ^ _rotr( dbucket, 2 );
    }
    bucket %= NUM_BUCKETS;
    dbucket %= NUM_BUCKETS;
    if( dbucket == 0 ) {
        dbucket = 1;
    }
    block %= num_blocks;
    dblock %= num_blocks;
    if( dblock == 0 ) {
        dblock = 1;
    }
    entry_len = (len | 1) + 3  ;
    for( i = 0; i < num_blocks; i++ ) {
        loc = lib_block[block].fflag * 2;
        for( j = 0; j < NUM_BUCKETS; j++ ){
            if( lib_block[block].htab[bucket] == 0 ) {
                if( ( DIC_REC_SIZE - loc - 2 ) < entry_len ){
                    lib_block[block].fflag = LIB_FULL_PAGE;
                    break;
                    }
                lib_block[block].htab[bucket] = lib_block[block].fflag;
                lib_block[block].fflag += entry_len / 2;
                loc -= NUM_BUCKETS + 1;
                lib_block[block].name[loc ] = len;
                loc ++;
                memcpy( &(lib_block[block].name[loc]), sym, len );
                loc += len;
                *((unsigned_16 *) &( lib_block[block].name[loc] ) ) = offset;
                return( TRUE );
                }
            bucket += dbucket;
            if( bucket >= NUM_BUCKETS ) {
                bucket -= NUM_BUCKETS;
            }
        }
        block += dblock;
        if( block >= num_blocks ){
            block -= num_blocks;
        }
   }
   return( FALSE );
}


static bool HashOmfSymbols( OmfLibBlock *lib_block, unsigned num_blocks, sym_file *file )
{
    bool        ret;
    sym_entry   *sym;
    unsigned    str_len;
    char        *fname;

    for( ; file != NULL; file = file->next ){
        if( file->import ) {
            fname = file->import->symName;
        } else {
            fname = MakeFName( file->full_name);
        }
        str_len = strlen( fname );
        fname[ str_len ] ='!';
        ret = InsertOmfDict( lib_block, num_blocks, fname,
            str_len + 1, file->new_offset );
        fname[ str_len ] = 0;
        if( ret == FALSE ){
            return( ret );
        }
        for( sym = file->first; sym != NULL; sym = sym->next ){
            ret = InsertOmfDict( lib_block, num_blocks, sym->name,
                sym->len, file->new_offset );
            if( ret == FALSE ){
                return( ret );
            }
        }
    }
    return( ret );
}

//return size of dict
unsigned WriteOmfDict( sym_file *first )
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
    do{
        num_blocks ++;
        num_blocks = NextPrime( num_blocks );
        if( num_blocks == 0 ){
            return( 0 );
        }
        dict_size = num_blocks * sizeof( OmfLibBlock );
        lib_block = MemRealloc( lib_block, dict_size );
        memset( lib_block, 0, dict_size );
        for( i = 0; i < num_blocks; i ++ ) {
            lib_block[i].fflag = (NUM_BUCKETS + 1 ) / 2;
        }
        done = HashOmfSymbols( lib_block, num_blocks, first );
    } while( done == FALSE );
    for( i = 0; i < num_blocks; i++ ) {
        for( j = 0; j < NUM_BUCKETS; j++ ) {
           if( lib_block[i].htab[j] == 0 ) {
               break;
           }
        }
    }
    WriteNew( lib_block, dict_size );
    MemFree( lib_block );
    return( num_blocks );
}

static void trimOmfHeader()
{
    char *new_name;
    unsigned_8  sum;
    unsigned    i;

    omfRec->basic.contents[ omfRec->basic.len - 1 ] = '\0';
    new_name = TrimPath( omfRec->basic.contents + 1 );
    omfRec->basic.contents[0] = strlen( new_name );
    omfRec->basic.len = omfRec->basic.contents[0] + 2;
    strcpy( omfRec->basic.contents + 1, new_name );
    sum = 0;
    for( i = 0; i < omfRec->basic.len + 2; ++i ) {
        sum += omfRec->chkcalc[i];
    }
    omfRec->basic.contents[ omfRec->basic.len - 1 ] = -sum;
}

void WriteOmfFile( sym_file *file )
{
    libfile     io;
    sym_entry   *sym;
    bool        time_stamp;
    char        new_line[] = { '\n' };
    file_offset current;

    symCount ++;
    //add one for ! after name and make sure odd so whole name record will
    //be word aligned
    current = LibTell(NewLibrary);
    CheckForOverflow(current);
    file->new_offset = current / Options.page_size ;
    if( file->import == NULL ) {
        charCount += ( strlen( MakeFName( file->arch.name ) ) + 1 ) | 1;
        // Options.page_size is always a power of 2 so someone should optimize
        //this sometime. maybe store page_size as a log
        time_stamp = FALSE;
        if( file->inlib_offset != 0 ) {
            io = InLibHandle( file->inlib );
            LibSeek( io, file->inlib_offset, SEEK_SET );
        } else {
            io = LibOpen( file->full_name, LIBOPEN_BINARY_READ );
        }
        do {
            ReadOmfRecord( io );
            switch( omfRec->basic.type ){
            case CMD_THEADR:
                trimOmfHeader();
                break;
            case CMD_LINSYM:
            case CMD_LINS32:
            case CMD_LINNUM:
            case CMD_LINN32:
                if( Options.strip_line ){
                    continue;
                }
                break;
            case CMD_COMENT:
                switch( omfRec->basic.contents[1] ) {
                case CMT_LINKER_DIRECTIVE:
                    if( omfRec->time.subclass == LDIR_OBJ_TIMESTAMP ){
                        time_stamp = TRUE;
                    }
                    break;
                case CMT_DLL_ENTRY:
                    if( omfRec->basic.contents[2] == DLL_EXPDEF ) {
                        if( ExportListFile != NULL ) {
                            LibWrite( ExportListFile,&( omfRec->basic.contents[5] ), omfRec->basic.contents[4] );
                            LibWrite( ExportListFile, new_line, sizeof( new_line ) );
                        }
                        if( Options.strip_expdef ) {
                            continue;
                        }
                    }
                    break;
                }
                break;
            case CMD_MODEND:
            case CMD_MODE32:
                if( !time_stamp ){
                    WriteTimeStamp( file );
                }
                break;
            }
            WriteOmfRecord();
            } while( omfRec->basic.type != CMD_MODEND && omfRec->basic.type != CMD_MODE32 );
        if( file->inlib_offset == 0 ) {
            LibClose( io );
        }
    } else {
        unsigned_8 sum;
        unsigned sym_len;
        unsigned file_len;
        unsigned i;

        omfRec->basic.type = CMD_THEADR;
        sym_len = strlen( file->import->symName );
        omfRec->basic.len = sym_len + 2;
        omfRec->basic.contents[0] = sym_len;
        charCount += ( sym_len + 1 ) | 1;
        strcpy( omfRec->basic.contents + 1, file->import->symName );
        sum = 0;
        for( i = 0; i < sym_len + 4; ++i ) {
            sum += omfRec->chkcalc[i];
        }
        omfRec->basic.contents[ sym_len + 1 ] = -sum;
        WriteOmfRecord();
        file_len = strlen( file->import->DLLName );
        omfRec->basic.type = CMD_COMENT;
        omfRec->basic.len = 9 + file_len + sym_len;
        omfRec->basic.contents[0] = CMT_TNP;
        omfRec->basic.contents[1] = CMT_DLL_ENTRY;
        omfRec->basic.contents[2] = MOMF_IMPDEF;
        if( file->import->type == ORDINAL ) {
            omfRec->basic.contents[3] = 1;
        } else {
            omfRec->basic.contents[3] = 0;
        }
        omfRec->basic.contents[4] = sym_len;
        strcpy( omfRec->basic.contents + 5, file->import->symName );
        omfRec->basic.contents[5 + sym_len] = file_len;
        strcpy( omfRec->basic.contents + 6 + sym_len, file->import->DLLName );
        if( file->import->type == ORDINAL ) {
           *( (unsigned_16 *)&( omfRec->basic.contents[6 + sym_len + file_len ] ) ) = (unsigned_16)file->import->ordinal;
        } else {
            if( file->import->exportedName ) {
                i = strlen( file->import->exportedName );
                omfRec->basic.contents[ 6 + sym_len + file_len ] = i ;
                memcpy( omfRec->basic.contents + 7 + sym_len + file_len, file->import->exportedName, i + 1 ) ;
                sym_len += i + 1;
                omfRec->basic.len += i + 1;
            } else {
                omfRec->basic.contents[6 + sym_len + file_len] = 0;
                omfRec->basic.contents[7 + sym_len + file_len] = 0;
                }
        }
        sum = 0;
        for( i = 0; i < sym_len + file_len + 11; ++i ) {
            sum += omfRec->chkcalc[i];
        }
        omfRec->basic.contents[ sym_len + file_len +  8 ] = -sum;
        WriteOmfRecord();
        WriteTimeStamp( file );
        omfRec->basic.type = CMD_MODEND;
        omfRec->basic.len = 2;
        omfRec->basic.contents[0] = 0;
        omfRec->basic.contents[1] = 0;
        sum = 0;
        for( i = 0; i < 4; ++i ) {
            sum += omfRec->chkcalc[i];
        }
        omfRec->basic.contents[1] = -sum;
        WriteOmfRecord();
    }
    PadOmf( FALSE );
    for( sym = file->first; sym != NULL; sym = sym->next ) {
        ++symCount;
        charCount += sym->len | 1;
    }
}
