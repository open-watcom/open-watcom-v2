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


#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <stdarg.h>
#include <dos.h>
#include "watcom.h"
//#include "tinyio.h"

typedef unsigned short  WORD;
typedef unsigned long   DWORD;
#include "loader.h"

#define FALSE   0
#define TRUE    1
#define BUFSIZE 4096
#define Align4K( x ) (((x)+0xfffL) & ~0xfffL )


struct padded_hdr {
    rex_exe     hdr;
    WORD        pad;
};

typedef struct {
    DWORD datastart;
    DWORD stackstart;
} exe_data;

DWORD   StackSize;
DWORD   BaseAddr;
char    *CompressedBuffer;
char    *CompressedBufPtr;
unsigned short  *RelocBuffer;
#define W32Putc(c) *CompressedBufPtr++ = (c)

int CmpReloc( DWORD *p, DWORD *q )
{
    DWORD       reloc1;
    DWORD       reloc2;

    reloc1 = *p & 0x7FFFFFFF;
    reloc2 = *q & 0x7FFFFFFF;
    if( reloc1 == reloc2 ) return( 0 ); // shouldn't happen
    if( reloc1 < reloc2 )  return( -1 );
    return( 1 );
}


int CopyRexFile( int handle, int newfile, DWORD filesize )
{
    char        *buf;
    unsigned    amt1;
    unsigned    len;

    buf = malloc( BUFSIZE );
    if( buf == NULL ) {
        printf( "Out of memory\n" );
        return( -1 );
    }
    for(;;) {
        len = filesize;
        if( len > BUFSIZE )  len = BUFSIZE;
        amt1 = read( handle, buf, len );
        if( amt1 != len ) {
            printf( "Error reading REX file\n" );
            free( buf );
            return( -1 );
        }
        if( write( newfile, buf, len ) != len ) {
            printf( "Error writing file\n" );
            free( buf );
            return( -1 );
        }
        filesize -= len;
        if( filesize == 0 ) break;
    }
    free( buf );
    return( 0 );
}

DWORD RelocSize( DWORD *relocs, unsigned n )
{
    DWORD       size;
    DWORD       page;
    unsigned    i;

    i = 0;
    size = 0;
    while( i < n ) {
        size += 2 * sizeof(unsigned short);
        page = relocs[i] & 0x7FFF0000;
        while( i < n ) {
            if( (relocs[i] & 0x7FFF0000) != page ) break;
            i++;
            size += sizeof(unsigned short);
        }
    }
    size += sizeof(unsigned short);
    return( size );
}

int CreateRelocs( DWORD *relocs, unsigned short *newrelocs, unsigned n )
{
    DWORD       page;
    unsigned    num;
    unsigned    i;
    unsigned    j;
    unsigned    k;

    i = 0;
    k = 0;
    while( i < n ) {
        page = relocs[i] & 0x7FFF0000;
        j = i;
        while( j < n ) {
            if( (relocs[j] & 0x7FFF0000) != page ) break;
            j++;
        }
        //printf( "Page: %4.4x  Count: %u\n", page >> 16, j - i );
        newrelocs[k++] = j - i;
        newrelocs[k++] = page >> 16;
        newrelocs[k++] = (unsigned short)relocs[i];
        i++;
        for( ; i < j; i++, k++ ) {
            newrelocs[k] = (unsigned short)(relocs[i] - relocs[i-1]);
        }
        i = j;
    }
    newrelocs[k++] = 0;
    return( 0 );
}

int main( int argc, char *argv[] )
{
    int                 handle;
    int                 loader_handle;
    int                 newfile;
    long                rc;
    char                *file;
    DWORD               size;
    DWORD               codesize;
    DWORD               relocsize;
    DWORD               minmem,maxmem;
    DWORD               relsize,exelen;
    DWORD               *relocs;
    DWORD               file_header_size;
    unsigned            len;
    char                *loader_code;
    dos_hdr             *dos_header;
    w32_hdr             *w32_header;
    struct padded_hdr   exehdr;
    char                compress;

    if( argc < 3 ) {
        printf( "Usage: w32bind REX_filename new_filename [os2ldr.exe]\n" );
        printf( "w32bind binds REX file with 32-bit loader\n" );
//      printf( "-c   compresses the executable\n" );
        exit( 1 );
    }
    argc = 1;
    compress = 0;
    file = argv[argc];
#if 0
    if( strcmp( file, "-c" ) == 0 ) {
        compress = 1;
        ++argc;
        file = argv[argc];
    }
#endif
    ++argc;
    handle = open( file, O_RDONLY | O_BINARY );
    if( handle == -1 ) {
        printf( "Error opening file '%s'\n", file );
        exit( 1 );
    }

    exelen = 0;
    /*
     * validate header signature
     */
    read( handle, &exehdr, sizeof( rex_exe ) );
    if( !(exehdr.hdr.sig[0] == 'M' && exehdr.hdr.sig[1] == 'Q') ) {
        printf( "Invalid EXE\n" );
        exit( 1 );
    }
    file_header_size = (DWORD) exehdr.hdr.file_header * 16L;
    /*
     * exe.one is supposed to always contain a 1 for a .REX file.
     * However, to allow relocation tables bigger than 64K, the
     * we extended the linker to have the .one field contain the
     * number of full 64K chunks of relocations, minus 1.
     */
    file_header_size += (exehdr.hdr.one-1)*0x10000L*16L;

    /*
     * get file size
     */
    size = (long) exehdr.hdr.file_size2 * 512L;
    if( exehdr.hdr.file_size1 > 0 ) {
        size += (long) exehdr.hdr.file_size1 - 512L;
    }

    /*
     * get minimum/maximum amounts of heap, then add in exe size
     * to get total area
     */
    minmem = (DWORD) exehdr.hdr.min_data *(DWORD) 4096L;
    if( exehdr.hdr.max_data == (unsigned short)-1 ) {
        maxmem = 4096L;
    } else {
        maxmem = (DWORD) exehdr.hdr.max_data*4096L;
    }
    minmem = Align4K( minmem + size );
    maxmem = Align4K( maxmem + size );
    if( minmem > maxmem ) {
        maxmem = minmem;
    }
    //printf( "minmem = %lu, maxmem = %lu\n", minmem, maxmem );
    //printf( "size = %lu, file_header_size = %lu\n", size, file_header_size );
    codesize = size - file_header_size;
    //printf( "code+data size = %lu\n", codesize );

    /*
     * get and apply relocation table
     */
    relsize = sizeof( DWORD ) * (DWORD) exehdr.hdr.reloc_cnt;
    {
        DWORD   realsize;
        WORD    kcnt;

        realsize = file_header_size - (DWORD) exehdr.hdr.first_reloc;
        kcnt = realsize / (0x10000L*sizeof(DWORD));
        relsize += kcnt * (0x10000L*sizeof(DWORD));
    }
    //printf( "relocation size = %lu", relsize );
    //printf( " => %lu relocation entries\n", relsize / sizeof(DWORD) );
    lseek( handle, exelen + (DWORD) exehdr.hdr.first_reloc, SEEK_SET );
    if( relsize != 0 ) {
        relocs = (DWORD *)malloc( relsize );
        if( relocs == NULL ) {
            printf( "Out of memory\n" );
            return( -1 );
        }
        len = read( handle, relocs, relsize );
        if( len != relsize ) {
            printf( "Error reading relocation information\n" );
            exit( 1 );
        }
        qsort( relocs, relsize / sizeof(DWORD), sizeof(DWORD), CmpReloc );
        if( relocs[0] < 0x80000000 ) {
            printf( "REX file contains 16-bit relocations\n" );
            exit( 1 );
        }
    }
    relocsize = RelocSize( relocs, relsize / sizeof(DWORD) );
    RelocBuffer = (unsigned short *)malloc( relocsize );
    if( RelocBuffer == NULL ) {
        printf( "Out of memory\n" );
        exit( 1 );
    }
    CreateRelocs( relocs, RelocBuffer, relsize / sizeof(DWORD) );

    file = argv[argc++];
    newfile = open( file, O_WRONLY | O_BINARY | O_CREAT | O_TRUNC,
                        S_IREAD | S_IWRITE | S_IEXEC );
    if( newfile == -1 ) {
        printf( "Error opening file '%s'\n", file );
        exit( 1 );
    }
    file = argv[argc++];
    if( file == NULL )  file = "os2ldr.exe";
    loader_handle = open( file, O_RDONLY | O_BINARY );
    if( loader_handle == -1 ) {
        printf( "Error opening file '%s'\n", file );
        exit( 1 );
    }
    size = filelength( loader_handle );
    loader_code = calloc( (size + 511) & -512L, 1 );
    if( loader_code == NULL ) {
        printf( "Out of memory\n" );
        return( -1 );
    }
    len = read( loader_handle, loader_code, size );
    close( loader_handle );
    if( len != size ) {
        printf( "Error reading '%s'\n", file );
        exit( 1 );
    }
    size = (size + 511) & -512L;        /* round up to multiple of 512 */

    /* patch header in the loader */
    dos_header = (dos_hdr *)loader_code;
    w32_header = (w32_hdr *)
        (loader_code + dos_header->size_of_DOS_header_in_paras * 16);

    w32_header->start_of_W32_file = size;
    w32_header->size_of_W32_file = codesize + relocsize;
    w32_header->offset_to_relocs = codesize;
    maxmem = max( w32_header->size_of_W32_file, maxmem );
    maxmem = Align4K( maxmem );
    w32_header->memory_size = maxmem;
    w32_header->initial_EIP = exehdr.hdr.initial_eip;
    if( compress ) {
        w32_header->sig = 'FCC';
        CompressedBuffer = malloc( codesize + relocsize );
        if( CompressedBuffer == NULL ) {
            printf( "Out of memory\n" );
            exit( 1 );
        }
        CompressedBufPtr = CompressedBuffer;
        lseek( handle, exelen + file_header_size, SEEK_SET );
        CompressFile( handle, codesize );
        close( handle );
        CompressRelocs( relocsize );
        w32_header->size_of_W32_file = CompressedBufPtr - CompressedBuffer;
        len = write( newfile, loader_code, size );
        if( len != size ) {
            printf( "Error writing output file\n" );
            close( newfile );
            exit( 1 );
        }
        size = CompressedBufPtr - CompressedBuffer;
        len = write( newfile, CompressedBuffer, size );
        if( len != size ) {
            printf( "Error writing output file\n" );
            close( newfile );
            exit( 1 );
        }
    } else {
        w32_header->sig = 'FC';
        len = write( newfile, loader_code, size );
        if( len != size ) {
            printf( "Error writing output file\n" );
            close( newfile );
            close( handle );
            exit( 1 );
        }
        lseek( handle, exelen + file_header_size, SEEK_SET );
        CopyRexFile( handle, newfile, codesize );
        close( handle );
        len = write( newfile, RelocBuffer, relocsize );
        if( len != relocsize ) {
            printf( "Error writing output file\n" );
            close( newfile );
            exit( 1 );
        }
    }
    if( relsize != 0 ) {
        free( relocs );
    }
    free( CompressedBuffer );
    free( RelocBuffer );
    close( newfile );
    return( 0 );
}

/* BPE (Byte Pair Encoding) */
/* compress.c */
/* Copyright 1994 by Philip Gage */
/* from "The C Users Journal" - Feb 1994 */
/* Program speeded up by F.W.Crigger by adding high_count array */


int     MaxChars = 200;
int     Threshold = 3;

#define BLOCKSIZE  5000         /* Maximum block size */
#define HASHSIZE   4096         /* Size of hash table */
#define MAXCHARS   MaxChars     /* Char set per block */
#define THRESHOLD  Threshold    /* Minimum pair count */

unsigned char buffer[BLOCKSIZE];        /* Data block */
unsigned char leftcode[256];            /* Pair table */
unsigned char rightcode[256];           /* Pair table */
unsigned char left[HASHSIZE];           /* Hash table */
unsigned char right[HASHSIZE];          /* Hash table */
unsigned char count[HASHSIZE];          /* Pair count */
/* high_count contains indices into count[] where count[i] >= THRESHOLD */
int     high_count[HASHSIZE];
int     high_index;             /* next index into high_count to fill in */
int     size;                   /* Size of current data block */
int     old_size;
int     chars_used;
int     BlockSize;

/* Function prototypes */
int lookup( unsigned char, unsigned char );
int fileread( FILE * );

/* Return index of character pair in hash table */
/* Deleted nodes have count of 1 for hashing */

int lookup( unsigned char a, unsigned char b )
{
    int         index;

    /* Compute hash key from both characters */
    index = (a ^ (b << 5)) & (HASHSIZE-1);

    /* Search for pair or first empty slot */
    while( (left[index] != a  ||  right[index] != b)  && count[index] != 0 ) {
        index = (index + 1) & (HASHSIZE-1);
    }

    /* Store pair in table */
    left[index] = a;
    right[index] = b;
    return( index );
}

void HashBlock( int len )
{
    int         c;
    int         i;
    int         index;
    int         used = 0;

    /* Reset hash table and pair table */
    for( c = 0; c < HASHSIZE; c++ ) {
        count[c] = 0;
    }
    for( c = 0; c < 256; c++ ) {
        leftcode[c] = c;
        rightcode[c] = 0;
    }
    high_index = 0;

    for( size = 0; size < len; size++ ) {
        c = buffer[size];
        if( size > 0 ) {
            index = lookup( buffer[size-1], c );
            if( count[index] == (THRESHOLD-1) ) {
                high_count[high_index++] = index;
            }
            if( count[index] < 255 )  ++count[index];
        }

        /* Use rightcode to flag data chars found */
        if( ! rightcode[c] ) {
            rightcode[c] = 1;
            used++;
        }
    }
    old_size = size;
    chars_used = used;
}

/* Write each pair table and data block to output */
void filewrite( void )
{
    int         i;
    int         len;
    int         unused = 0;
    int         missed;
    int         c = 0;

    /* For each character 0..255 */
    while( c < 256 ) {

        /* If not a pair code, count run of literals */
        if( c == leftcode[c] ) {
            len = 1;
            c++;
            while( len < 127  &&  c < 256  &&  c == leftcode[c] ) {
                if( ! rightcode[c] )  ++unused;
                len++;
                c++;
            }
            W32Putc( len + 127 );
            len = 0;
            if( c == 256 ) break;
        } else {                /* Else count run of pair codes */
            len = 0;
            c++;
            while( len < 127  &&  c < 256  &&  c != leftcode[c]  ||
                   len < 125  &&  c < 254  &&  c+1 != leftcode[c+1] ) {
                len++;
                c++;
            }
            W32Putc( len );
            c -= len + 1;
        }

        /* Write range of pairs to output */
        for( i = 0; i <= len; i++ ) {
            W32Putc( leftcode[c] );
            if( c != leftcode[c] ) {
                W32Putc( rightcode[c] );
            }
            c++;
        }
    }
    /* Write size bytes and compressed data block */
    W32Putc( size / 256 );
    W32Putc( size % 256 );
    memcpy( CompressedBufPtr, buffer, size );
    CompressedBufPtr += size;
    missed = 0;
    for( i = 0; i < high_index; i++ ) {
        c = high_count[i];
        if( count[c] >= THRESHOLD ) {
            missed += count[c];
        }
    }
  //printf( "Data block: old_size=%u, new_size=%u, chars=%u, unused pairs=%u missed=%u\n",
  //            old_size, size, chars_used, unused, missed );
}

void inc_count( int index )
{
    if( count[index] < 255 ) {
        if( count[index] == (THRESHOLD-1) ) {
            if( high_index < HASHSIZE ) {
                high_count[high_index++] = index;
            }
        }
        ++count[index];
    }
}

/* Compress from input file to output file */
void CompressFile( int handle, DWORD filesize )
{
    int         done = 0;
    int         len;

    /* Compress each data block until end of file */
    BlockSize = 2048;
    while( filesize != 0 ) {
        len = BlockSize;
        if( len > filesize )  len = filesize;
        if( read( handle, buffer, len ) != len ) {
            printf( "Read error\n" );
            exit( 1 );
        }
        HashBlock( len );
        CompressBlock();
        filewrite();
        filesize -= len;
    }
}

void CompressRelocs( DWORD relocsize )
{
    int         len;
    char        *p;

    p = (char *)RelocBuffer;
    while( relocsize != 0 ) {
        if( relocsize < BLOCKSIZE ) {
            len = relocsize;
        } else {
            len = 4096;
        }
        memcpy( buffer, p, len );
        p += len;
        HashBlock( len );
        CompressBlock();
        filewrite();
        relocsize -= len;
    }
}

void CompressBlock( void )
{
    unsigned char       leftch;
    unsigned char       rightch;
    int         best;
    int         code;
    int         oldsize;
    int         index, r, w, best_index;

    code = 256;
    for(;;) {
        /* Get next unsed char for pair code */
        for( code--; code >= 0; code-- ) {
            if( code == leftcode[code]  &&  ! rightcode[code] ) break;
        }

        /* Must quit if no unused chars left */
        if( code < 0 )  break;

        /* Find most frequent pair of chars */
        best_index = 0;
#if 0
        for( best = 2, index = 0; index < HASHSIZE; index++ ) {
            if( count[index] > best ) {
                best = count[index];
                leftch = left[index];
                rightch = right[index];
                best_index = index;
            }
        }
#else
        for( best = 2, r = 0; r < high_index; r++ ) {
            index = high_count[r];
            if( count[index] > best ) {
                best = count[index];
                best_index = index;
            }
        }
#endif

        /* Done if no more compression possible */
        if( best < THRESHOLD )  break;

        /* Replace pairs in data, adjust pair counts */
        leftch = left[best_index];
        rightch = right[best_index];
        oldsize = size - 1;
        for( w = 0, r = 0; r < oldsize; r++ ) {
            if( buffer[r] == leftch  &&  buffer[r+1] == rightch ) {
                if( r > 0 ) {
                    index = lookup( buffer[w-1], leftch );
                    if( count[index] > 1 )  --count[index];
                    inc_count( lookup( buffer[w-1], code ) );
                }
                if( r < oldsize - 1 ) {
                    index = lookup( rightch, buffer[r+2] );
                    if( count[index] > 1 )  --count[index];
                    inc_count( lookup( code, buffer[r+2] ) );
                }
                buffer[w++] = code;
                r++;
                size--;
            } else {
                buffer[w++] = buffer[r];
            }
        }
        buffer[w] = buffer[r];

        /* Add to pair substitution table */
        leftcode[code] = leftch;
        rightcode[code] = rightch;

        /* Delete pair from hash table */
//          index = lookup( leftch, rightch );
        count[best_index] = 1;
    }
}
