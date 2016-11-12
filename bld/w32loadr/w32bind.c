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
* Description:  Bind utility for OSI executables.
*
****************************************************************************/


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wio.h"
#include "loader.h"

#include "clibext.h"


#ifdef _WIN64
#define posix_read      __w64_read
#define posix_write     __w64_write
#else
#define posix_read      read
#define posix_write     write
#endif

#define BUFSIZE         4096

#define _RoundUp(x,a)   (((x)+((a)-1)) & ~((a)-1))

#define Align4K(x)      _RoundUp((x),4096L)

#define SET_OSI_SIGN(x)        (x)->sig[0]='C';(x)->sig[1]='F';(x)->sig[2]=0;(x)->sig[3]=0
#define SET_OSI_COMPR_SIGN(x)  (x)->sig[0]='C';(x)->sig[1]='C';(x)->sig[2]='F';(x)->sig[3]=0

#define W32PutByte(c)   *out_buffer_ptr++ = (char)(c)
#define W32PutBuf(b,s)  memcpy(out_buffer_ptr,b,s);out_buffer_ptr+=s

struct padded_hdr {
    rex_exe     hdr;
    uint_16     pad;
};

typedef struct {
    uint_32     datastart;
    uint_32     stackstart;
} exe_data;

static char             *RelocBuffer;

static char             *out_buffer = NULL;
static char             *out_buffer_ptr;

/* Function prototypes */
static int lookup( unsigned char, unsigned char );

static void normalizeFName( char *dst, size_t maxlen, const char *src )
/***********************************************************************
 * Removes doublequote characters from filename and copies other content
 * from src to dst. Only maxlen number of characters are copied to dst
 * including terminating NUL character. Returns value 1 when quotes was
 * removed from orginal filename, 0 otherwise.
 */
{
    char    string_open = 0;
    size_t  pos = 0;
    char    c;

    // leave space for NUL terminator
    maxlen--;

    while( (c = *src++) != '\0' && pos < maxlen ) {
        if( c == '"' ) {
            string_open = !string_open;
            continue;
        }
        if( string_open && c == '\\' ) {
            c = *src++;
            if( c != '"' ) {
                *dst++ = '\\';
                pos++;
                if( pos >= maxlen ) {
                    break;
                }
            }
        }
#ifndef __UNIX__
        if( c == '/' )
            c = '\\';
#endif
        *dst++ = c;
        pos++;
    }
    *dst = '\0';
}

static int CmpReloc( const void *p, const void *q )
{
    uint_32     reloc1;
    uint_32     reloc2;

    reloc1 = *(const uint_32 *)p & 0x7FFFFFFF;
    reloc2 = *(const uint_32 *)q & 0x7FFFFFFF;
    if( reloc1 == reloc2 )
        return( 0 );        // shouldn't happen
    if( reloc1 < reloc2 )
        return( -1 );
    return( 1 );
}

static uint_32 RelocSize( uint_32 *relocs, unsigned n )
{
    uint_32     size;
    uint_32     page;
    unsigned    i;

    i = 0;
    size = 0;
    while( i < n ) {
        size += 2 * sizeof(unsigned short);
        page = relocs[i] & 0x7FFF0000;
        while( i < n ) {
            if( (relocs[i] & 0x7FFF0000) != page )
                break;
            i++;
            size += sizeof(unsigned short);
        }
    }
    size += sizeof(unsigned short);
    return( size );
}

static int CreateRelocs( uint_32 *relocs, char *buf, unsigned n )
{
    uint_32         page;
    unsigned        i;
    unsigned        j;
    unsigned        k;
    unsigned short  *newrelocs;

    newrelocs = (unsigned short *)buf;
    i = 0;
    k = 0;
    while( i < n ) {
        page = relocs[i] & 0x7FFF0000;
        j = i;
        while( j < n ) {
            if( (relocs[j] & 0x7FFF0000) != page )
                break;
            j++;
        }
        //printf( "Page: %4.4x  Count: %u\n", page >> 16, j - i );
        newrelocs[k++] = j - i;
        newrelocs[k++] = page >> 16;
        newrelocs[k++] = (unsigned short)relocs[i];
        i++;
        for( ; i < j; i++, k++ ) {
            newrelocs[k] = (unsigned short)(relocs[i] - relocs[i - 1]);
        }
        i = j;
    }
    newrelocs[k++] = 0;
    return( 0 );
}

/* BPE (Byte Pair Encoding) */
/* compress.c */
/* Copyright 1994 by Philip Gage */
/* from "The C Users Journal" - Feb 1994 */
/* Program speeded up by F.W.Crigger by adding high_count array */


//static int     MaxChars = 200;
static int     Threshold = 3;

#define BLOCKSIZE  5000         /* Maximum block size */
#define HASHSIZE   4096         /* Size of hash table */
//#define MAXCHARS   MaxChars     /* Char set per block */
#define THRESHOLD  Threshold    /* Minimum pair count */

static unsigned char buffer[BLOCKSIZE];        /* Data block */
static unsigned char leftcode[256];            /* Pair table */
static unsigned char rightcode[256];           /* Pair table */
static unsigned char left[HASHSIZE];           /* Hash table */
static unsigned char right[HASHSIZE];          /* Hash table */
static unsigned char count[HASHSIZE];          /* Pair count */
/* high_count contains indices into count[] where count[i] >= THRESHOLD */
static int     high_count[HASHSIZE];
static int     high_index;             /* next index into high_count to fill in */
static int     old_size;
static int     chars_used;
//static int     BlockSize;

/* Return index of character pair in hash table */
/* Deleted nodes have count of 1 for hashing */

static int lookup( unsigned char a, unsigned char b )
{
    int         index;

    /* Compute hash key from both characters */
    index = (a ^ (b << 5)) & (HASHSIZE - 1);

    /* Search for pair or first empty slot */
    while( (left[index] != a || right[index] != b) && count[index] != 0 ) {
        index = (index + 1) & (HASHSIZE - 1);
    }

    /* Store pair in table */
    left[index] = a;
    right[index] = b;
    return( index );
}

static size_t HashBlock( size_t len )
{
    int         c;
    int         index;
    int         used = 0;
    size_t      size;

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
            index = lookup( buffer[size - 1], c );
            if( count[index] == (THRESHOLD - 1) ) {
                high_count[high_index++] = index;
            }
            if( count[index] < 255 ) {
                ++count[index];
            }
        }

        /* Use rightcode to flag data chars found */
        if( ! rightcode[c] ) {
            rightcode[c] = 1;
            used++;
        }
    }
    old_size = size;
    chars_used = used;
    return( size );
}

/* Write each pair table and data block to output */
static size_t filewrite( int newfile, size_t inp_size )
{
    int         i;
    int         len;
    int         unused = 0;
    int         missed;
    int         c = 0;
    size_t      out_size;

    out_buffer_ptr = out_buffer;

    /* For each character 0..255 */
    while( c < 256 ) {

        /* If not a pair code, count run of literals */
        if( c == leftcode[c] ) {
            len = 1;
            c++;
            while( len < 127  &&  c < 256  &&  c == leftcode[c] ) {
                if( ! rightcode[c] )
                    ++unused;
                len++;
                c++;
            }
            W32PutByte( len + 127 );
            len = 0;
            if( c == 256 ) {
                break;
            }
        } else {                /* Else count run of pair codes */
            len = 0;
            c++;
            while( len < 127 && c < 256 && c != leftcode[c] || len < 125 && c < 254 && ( c + 1 ) != leftcode[c + 1] ) {
                len++;
                c++;
            }
            W32PutByte( len );
            c -= len + 1;
        }

        /* Write range of pairs to output */
        for( i = 0; i <= len; i++ ) {
            W32PutByte( leftcode[c] );
            if( c != leftcode[c] ) {
                W32PutByte( rightcode[c] );
            }
            c++;
        }
    }
    /* Write size bytes and compressed data block */
    W32PutByte( inp_size / 256 );
    W32PutByte( inp_size % 256 );
    W32PutBuf( buffer, inp_size );
    out_size = out_buffer_ptr - out_buffer;
    if( out_size > 0 ) {
        if( posix_write( newfile, out_buffer, out_size ) != out_size ) {
            printf( "Error writing file\n" );
            return( -1 );
        }
    }
    missed = 0;
    for( i = 0; i < high_index; i++ ) {
        c = high_count[i];
        if( count[c] >= THRESHOLD ) {
            missed += count[c];
        }
    }
//printf( "Data block: old_size=%u, new_size=%u, chars=%u, unused pairs=%u missed=%u\n",
//            old_size, size, chars_used, unused, missed );
    return( out_size );
}

static void inc_count( int index )
{
    if( count[index] < 255 ) {
        if( count[index] == (THRESHOLD - 1) ) {
            if( high_index < HASHSIZE ) {
                high_count[high_index++] = index;
            }
        }
        ++count[index];
    }
}

static size_t CompressBlock( size_t inp_size )
{
    unsigned char   leftch;
    unsigned char   rightch;
    unsigned char   best;
    int             code;
    int             oldsize;
    int             index, r, w, best_index;

    code = 256;
    for(;;) {
        /* Get next unsed char for pair code */
        for( code--; code >= 0; code-- ) {
            if( code == leftcode[code] && !rightcode[code] ) {
                break;
            }
        }

        /* Must quit if no unused chars left */
        if( code < 0 )
            break;

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
        if( best < THRESHOLD )
            break;

        /* Replace pairs in data, adjust pair counts */
        leftch = left[best_index];
        rightch = right[best_index];
        oldsize = inp_size - 1;
        for( w = 0, r = 0; r < oldsize; r++ ) {
            if( buffer[r] == leftch  &&  buffer[r + 1] == rightch ) {
                if( r > 0 ) {
                    index = lookup( buffer[w - 1], leftch );
                    if( count[index] > 1 )
                        --count[index];
                    inc_count( lookup( buffer[w - 1], code ) );
                }
                if( r < oldsize - 1 ) {
                    index = lookup( rightch, buffer[r + 2] );
                    if( count[index] > 1 )
                        --count[index];
                    inc_count( lookup( code, buffer[r + 2] ) );
                }
                buffer[w++] = code;
                r++;
                inp_size--;
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
    return( inp_size );
}

static uint_32 WriteRelocs( int newfile, const char *buf, uint_32 inp_size, int compress )
{
    size_t      len;
    size_t      size;
    uint_32     out_size;

    len = 4096;
    out_size = 0;
    while( inp_size > 0 ) {
        if( len > inp_size )
            len = inp_size;
        memcpy( buffer, buf, len );
        buf += len;
        if( compress ) {
            size = HashBlock( len );
            size = CompressBlock( size );
            size = filewrite( newfile, size );
            if( size == -1 ) {
                printf( "Error writing output file\n" );
                return( -1 );
            }
            out_size += size;
        } else {
            if( posix_write( newfile, buf, len ) != len ) {
                printf( "Error writing output file\n" );
                return( -1 );
            }
            out_size += len;
        }
        inp_size -= len;
    }
    return( out_size );
}

static uint_32 WriteCode( int handle, int newfile, uint_32 inp_size, int compress )
{
    size_t      len;
    size_t      size;
    uint_32     out_size;

    len = 2048;
    out_size = 0;
    while( inp_size > 0 ) {
        if( len > inp_size )
            len = inp_size;
        if( posix_read( handle, buffer, len ) != len ) {
            printf( "Error reading REX file\n" );
            return( -1 );
        }
        if( compress ) {
            /* Compress each data block until end of file */
            size = HashBlock( len );
            size = CompressBlock( size );
            size = filewrite( newfile, size );
            if( size == -1 ) {
                printf( "Error writing output file\n" );
                return( -1 );
            }
            out_size += size;
        } else {
            if( posix_write( newfile, buffer, len ) != len ) {
                printf( "Error writing output file\n" );
                return( -1 );
            }
            out_size += len;
        }
        inp_size -= len;
    }
    return( out_size );
}

int main( int argc, char *argv[] )
{
    int                 handle;
    int                 loader_handle;
    int                 newfile;
    char                *file;
    uint_32             size;
    uint_32             codesize;
    uint_32             relocsize;
    uint_32             minmem,maxmem;
    uint_32             relsize,exelen;
    uint_32             *relocs;
    uint_32             file_header_size;
    char                *loader_code;
    dos_hdr             *dos_header;
    w32_hdr             *w32_header;
    struct padded_hdr   exehdr;
    char                compress;
    size_t              loadersize_read;
    size_t              loadersize;
    uint_32             out_size;

    if( argc < 3 ) {
        printf( "Usage: w32bind REX_filename new_filename [os2ldr.exe]\n" );
        printf( "w32bind binds REX file with 32-bit loader\n" );
//      printf( "-c   compresses the executable\n" );
        return( 1 );
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
    normalizeFName( file, strlen( file ) + 1, file );
    handle = open( file, O_RDONLY | O_BINARY );
    if( handle == -1 ) {
        printf( "Error opening file '%s'\n", file );
        return( 1 );
    }

    exelen = 0;
    /*
     * validate header signature
     */
    posix_read( handle, &exehdr, sizeof( rex_exe ) );
    if( !(exehdr.hdr.sig[0] == 'M' && exehdr.hdr.sig[1] == 'Q') ) {
        printf( "Invalid EXE\n" );
        return( 1 );
    }
    file_header_size = (uint_32)exehdr.hdr.file_header * 16L;
    /*
     * exe.one is supposed to always contain a 1 for a .REX file.
     * However, to allow relocation tables bigger than 64K, the
     * we extended the linker to have the .one field contain the
     * number of full 64K chunks of relocations, minus 1.
     */
    file_header_size += ( exehdr.hdr.one - 1 ) * 0x10000L * 16L;

    /*
     * get file size
     */
    size = (long)exehdr.hdr.file_size2 * 512L;
    if( exehdr.hdr.file_size1 > 0 ) {
        size += (long)exehdr.hdr.file_size1 - 512L;
    }

    /*
     * get minimum/maximum amounts of heap, then add in exe size
     * to get total area
     */
    minmem = (uint_32)exehdr.hdr.min_data * 4096L;
    if( exehdr.hdr.max_data == (unsigned short)-1 ) {
        maxmem = 4096L;
    } else {
        maxmem = (uint_32)exehdr.hdr.max_data * 4096L;
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
    relsize = sizeof( uint_32 ) * (uint_32)exehdr.hdr.reloc_cnt;
    {
        uint_32 realsize;
        uint_16 kcnt;

        realsize = file_header_size - (uint_32)exehdr.hdr.first_reloc;
        kcnt = realsize / ( 0x10000L * sizeof( uint_32 ) );
        relsize += kcnt * ( 0x10000L * sizeof( uint_32 ) );
    }
    //printf( "relocation size = %lu", relsize );
    //printf( " => %lu relocation entries\n", relsize / sizeof( uint_32 ) );
    lseek( handle, exelen + (uint_32)exehdr.hdr.first_reloc, SEEK_SET );
    relocs = NULL;
    if( relsize != 0 ) {
        relocs = (uint_32 *)malloc( relsize );
        if( relocs == NULL ) {
            printf( "Out of memory\n" );
            return( -1 );
        }
        if( posix_read( handle, relocs, relsize ) != relsize ) {
            printf( "Error reading relocation information\n" );
            return( 1 );
        }
        qsort( relocs, relsize / sizeof( uint_32 ), sizeof( uint_32 ), CmpReloc );
        if( relocs[0] < 0x80000000 ) {
            printf( "REX file contains 16-bit relocations\n" );
            return( 1 );
        }
    }
    relocsize = RelocSize( relocs, relsize / sizeof( uint_32 ) );
    RelocBuffer = (char *)malloc( relocsize );
    if( RelocBuffer == NULL ) {
        printf( "Out of memory\n" );
        return( -1 );
    }
    CreateRelocs( relocs, RelocBuffer, relsize / sizeof( uint_32 ) );

    file = argv[argc++];
    normalizeFName( file, strlen( file ) + 1, file );
    newfile = open( file, O_WRONLY | O_BINARY | O_CREAT | O_TRUNC,
                        S_IREAD | S_IWRITE | S_IEXEC );
    if( newfile == -1 ) {
        printf( "Error opening file '%s'\n", file );
        return( 1 );
    }
    file = argv[argc++];
    if( file == NULL ) {
        file = "os2ldr.exe";
    } else {
        normalizeFName( file, strlen( file ) + 1, file );
    }
    loader_handle = open( file, O_RDONLY | O_BINARY );
    if( loader_handle == -1 ) {
        printf( "Error opening file '%s'\n", file );
        return( 1 );
    }
    loadersize_read = filelength( loader_handle );
    loadersize = _RoundUp( loadersize_read, 512L );
    loader_code = calloc( loadersize, 1 );
    if( loader_code == NULL ) {
        printf( "Out of memory\n" );
        return( -1 );
    }
    if( posix_read( loader_handle, loader_code, loadersize_read ) != loadersize_read ) {
        close( loader_handle );
        printf( "Error reading '%s'\n", file );
        return( 1 );
    }
    close( loader_handle );
    size = _RoundUp( size, 512L );        /* round up to multiple of 512 */

    /* patch header in the loader */
    dos_header = (dos_hdr *)loader_code;
    w32_header = (w32_hdr *)( loader_code + dos_header->size_of_DOS_header_in_paras * 16 );

    w32_header->start_of_W32_file = size;
    w32_header->size_of_W32_file = codesize + relocsize;
    w32_header->offset_to_relocs = codesize;
    if( maxmem < w32_header->size_of_W32_file )
        maxmem = w32_header->size_of_W32_file;
    maxmem = Align4K( maxmem );
    w32_header->memory_size = maxmem;
    w32_header->initial_EIP = exehdr.hdr.initial_eip;
    if( compress ) {
        SET_OSI_COMPR_SIGN( w32_header );
    } else {
        SET_OSI_SIGN( w32_header );
    }
    if( (size_t)posix_write( newfile, loader_code, loadersize ) != loadersize ) {
        printf( "Error writing output file\n" );
        close( newfile );
        close( handle );
        return( 1 );
    }
    lseek( handle, exelen + file_header_size, SEEK_SET );
    if( compress ) {
        out_buffer = malloc( 2 * 4096 );
        if( out_buffer == NULL ) {
            printf( "Out of memory\n" );
            return( -1 );
        }
    }
    out_size = WriteCode( handle, newfile, codesize, compress );
    close( handle );
    if( out_size == -1 ) {
        return( 1 );
    }
    if( compress ) {
        w32_header->size_of_W32_file = out_size ;
    }
    out_size = WriteRelocs( newfile, RelocBuffer, relocsize, compress );
    if( out_size == -1 ) {
        return( 1 );
    }
    if( compress ) {
        w32_header->size_of_W32_file += out_size ;
        lseek( newfile, dos_header->size_of_DOS_header_in_paras * 16, SEEK_SET );
        if( (size_t)posix_write( newfile, w32_header, sizeof( *w32_header ) ) != sizeof( *w32_header ) ) {
            close( newfile );
            printf( "Error writing output file\n" );
            return( 1 );
        }
        free( out_buffer );
    }
    close( newfile );
    if( relsize != 0 ) {
        free( relocs );
    }
    free( RelocBuffer );
    return( 0 );
}
