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
* Description:  Fast memory manager used by the code generator.
*
****************************************************************************/


/****************************************************************************

This is a free list based memory manager optimized for codegen performance.
It will allocate fixed size blocks of several classes (sizes), any freed
memory blocks will be kept in a singly linked free list. If an attempt is
made to allocate memory and a free block of appropriate size exists, it will
be found extremely quickly. The memory manager allocates memory from the
system via malloc() and free(), but it could use OS functions that allocate
memory pages directly.

Memory classes/sizes are powers of two, but they might be arbitrary sizes
designed to fit typical allocation sizes. Allocations greater than certain
maximum size will go directly to the OS.

Expected usage is great number of mostly small allocs and frees. The
allocator is designed to reuse memory well, and do it fast.

Disadvantages: May waste memory in certain scenarios. No attempts are made
to coalesce free memory blocks. Since the codegen extremely rarely allocates
more than even 256KB memory, some wastage is considered acceptable.

Advantages: Speed. Memory allocation is extremely fast, with no searches if
appropriately sized block is available. Memory frees are instantaneous, with
no searching whatsoever. Did I mention speed? Memory blocks will be reused
immediately, which should produce helpful memory access patterns. There is
essentially no worst case performance scenario.

****************************************************************************/


#include "cgstd.h"
#include <stdlib.h>
#include "cg.h"
#if defined( _M_IX86 ) && defined( __WATCOMC__ )
    #include <i86.h>
#endif
#include "wio.h"
#if defined( __NT__ )
    #include <windows.h>
#elif defined( __OSI__ )
    #define __CALL21__
    #include "tinyio.h"
#elif defined( __QNX__ )
    #include <sys/osinfo.h>
    #include <sys/seginfo.h>
#endif
#include "utils.h"

#ifdef __DOS__

#include "tinyio.h"
#include "dosmem.h"

typedef struct {
    unsigned largest_free;
    unsigned max_unlocked_page_alloc;
    unsigned max_locked_page_alloc;
    unsigned linear_addr_space_in_pages;
    unsigned total_unlocked_pages;
    unsigned free_pages;
    unsigned physical_pages;
    unsigned free_linear_addr_space_in_pages;
    unsigned size_of_page_file_in_pages;
    unsigned fill[4];
} dpmi_mem;

extern int _TinyDPMIGetFreeMemoryInformation( dpmi_mem * );
#pragma aux _TinyDPMIGetFreeMemoryInformation = \
        "push es"    \
        "push ds"    \
        "pop es"     \
        "mov ah,05h" \
        "mov al,00h" \
        "int 31h"    \
        "pop es"     \
        "sbb ax,ax"  \
        parm[edi] value[eax];

#include "extender.h"

extern char int2f( short );
#pragma aux int2f parm [ ax ] value [ al ] = "int 2fh";

#pragma     aux     __psp "*";
extern      short   __psp;

#endif

typedef pointer_int     tag;

extern bool     GetEnvVar( char *, char *, int );
extern void     FatalError( const char * );


#define _1K             1024L
#define _4K             (4*_1K)
#define _64K            (64*_1K)
#define _1M             (_1K*_1K)
#define _4M             (4*_1M)
#define _16M            (16*_1M)

static  pointer     MemFromSys( size_t );

static pointer_int  AllocSize = { 0 };
static pointer_int  MemorySize;
static int          Initialized = 0;
#ifdef MEMORY_STATS
static pointer_int  PeakAlloc    = 0;
#endif


#define ALLOCATED       1

#define MAX_SIZE        14 /* 16384 */
#define MIN_SIZE        4  /* 16 */
#if defined( LONG_IS_64BITS ) || defined( _WIN64 )
#define WORD_SIZE       8  /* Needed to keep alignment. */
#else
#define WORD_SIZE       4
#endif
#define MAX_CLASS       (MAX_SIZE-MIN_SIZE)

/* Free list structure - length holds the size of memory block, which
 * is necessary for freeing memory. Note that the length field is set
 * when the block is first allocated and never changes afterwards.
 * The link member holds the address of the next free block of the
 * same size, or NULL if it's the last free block.
 */
typedef struct frl {
    tag         length;
    struct frl  *link;
} frl;

#if (1 << MIN_SIZE) < (2 * WORD_SIZE)
    #error "Free list will not fit into freed chunk"
#endif

/* Memory block structure - memory is allocated from the OS in large
 * chunks (perhaps 64K, perhaps more or less than that). If there is
 * no memory in a free list, it will be allocated from a block, and
 * will end up on a free list when (if) it's freed.
 */
typedef struct mem_blk {
    struct mem_blk  *next;
    tag             free;
    tag             size;   /* This must be the last member! */
} mem_blk;

typedef struct blk_hdr {
    struct mem_blk  *block;
    tag             size;   /* This must be the last member! */
} blk_hdr;

#define TAG_SIZE        sizeof( tag )

#define MIN_ALLOC _RoundUp( sizeof( frl ), WORD_SIZE )
#define MAX_ALLOC (1 << MAX_SIZE)
#define _WALKTAG( free ) ((frl *)((char *)(free) + (free)->length ))

static mem_blk  *_Blks;
static frl      *_FreeList[ MAX_CLASS + 1 ];
static unsigned _ClassSizes[ MAX_CLASS + 1 ];

#if  defined( __DOS__ ) || defined( __QNX__ )

static  void    NotEnoughMem( void )
/**********************************/
{
    FatalError( "Not enough memory to satisfy WCGMEMORY" );
}

#endif


static int myatoi( char *p )
/**************************/
{
    int         i;

    i = 0;
    while( *p != '\0' ) {
        i *= 10;
        i += *p - '0';
        ++p;
    }
    return( i );
}


static  void    CalcMemSize( void )
/*********************************/
{
    bool        max_size_queried;
    bool        size_queried;
    pointer_int size_requested;
    pointer_int memory_available;
    char        buff[80];

    Initialized = 2;
    size_requested = 0;
    size_queried = FALSE;
    max_size_queried = FALSE;
    if( GetEnvVar( "WCGMEMORY", buff, 9 ) ) {
        if( buff[0] == '?' && buff[1] == '\0' ) {
            max_size_queried = TRUE;
        } else if( buff[0] == '#' && buff[1] == '\0' ) {
            size_queried = TRUE;
        } else {
            size_requested = myatoi( buff ) * _1K;
        }
    }
#if defined( __DOS__ )
    {
        char        *memstart;

        MemorySize = 0;
        if( _IsRational() ) {

            dpmi_mem            mem_info;

            _TinyDPMIGetFreeMemoryInformation( &mem_info );
            memory_available = mem_info.largest_free - _1K;

        } else { // PharLap or win386

            memstart = sbrk( 0 );
            if( int2f( 0x1686 ) == 0 ) { // DPMI HOST
                dpmi_mem    mem_info;
                _TinyDPMIGetFreeMemoryInformation( &mem_info );
                if( max_size_queried ) {
                    memory_available = mem_info.largest_free;
                } else {
                    memory_available = ( mem_info.physical_pages / 2 ) * _4K;
                    if( memory_available > _16M ) { // OS/2 reports 4G
                        memory_available = mem_info.largest_free;
                        if( memory_available > _16M ) {
                            memory_available = _16M;
                        }
                    }
                }
            } else {
                memory_available = *(char * __far *)MK_FP( __psp, 0x60 ) - memstart;
            }
            if( memory_available < _1M ) memory_available = _1M;
        }
        if( size_requested != 0 ) {
            if( memory_available < size_requested ) {
                NotEnoughMem();
            }
            MemorySize = size_requested;
        } else {
            MemorySize = memory_available;
        }
    }
#elif defined( __NT__ )
    {
        MEMORYSTATUS    info;

        GlobalMemoryStatus( &info );
        memory_available = info.dwAvailVirtual;
        if( size_requested != 0 ) {
            MemorySize = size_requested;
        } else {
            MemorySize = memory_available;
        }
    }
#elif defined( __OS2__ ) || defined( __OSI__ )
    if( size_requested != 0 ) {
        MemorySize = size_requested;
    } else {
        MemorySize = _16M;
    }
    memory_available = _16M;
#elif defined( __QNX__ )
    {
        struct _osinfo  data;

        qnx_osinfo( 0, &data );
        memory_available = (unsigned long)data.freememk * 1024;
        if( size_requested != 0 ) {
            if( memory_available < size_requested ) {
                NotEnoughMem();
            }
            MemorySize = size_requested;
        } else {
            MemorySize = memory_available;
        }
    }
#else
    if( size_requested != 0 ) {
        MemorySize = size_requested;
    } else {
        MemorySize = _16M;
    }
    memory_available = _16M;
#endif
    if( max_size_queried || size_queried ) {
        sprintf( buff, "Maximum WCGMEMORY=%d\n", (int)(memory_available/_1K) );
        FEMessage( MSG_INFO, buff );
    }
}

static  void    MemInit( void )
/*****************************/
{
    if( !Initialized ) {
        int i;

        for( i = 0; i <= MAX_CLASS; ++i ) {
            _FreeList[i]   = NULL;
            _ClassSizes[i] = 1 << ( MIN_SIZE + i );
        }

        Initialized = 1;
        AllocSize   = 0;
        _Blks       = NULL;
#if !defined( __QNX__ )
        CalcMemSize();
#endif
    }
}


static int SizeToClass( size_t amount )
/*************************************/
{
    int mem_class;

    if( amount >= MAX_ALLOC ) {
        return( MAX_CLASS );
    }
    if( amount <= ( 1 << MIN_SIZE ) ) {
        return( 0 );
    }
    --amount;
    amount >>= MIN_SIZE;
    mem_class = 0;
    for( ;; ) {
        ++mem_class;
        amount >>= 1;
        if( amount == 0 )
            break;
    }
    return( mem_class );
}


static unsigned ClassToSize( int mem_class )
/******************************************/
{
    return( _ClassSizes[mem_class] );
}


static pointer  GetFromFrl( size_t amount, int frl_class )
/********************************************************/
{
    frl     *free;

    amount = amount;
    free = _FreeList[ frl_class ];
    if( free != NULL ) {
        assert( !( free->length & ALLOCATED ) );
        free->length |= ALLOCATED;
        _FreeList[ frl_class ] = free->link;
        return( free );
    }
    return( NULL );
}


static pointer  GetFromBlk( size_t amount )
/*****************************************/
{
    mem_blk     *block;
    tag         *alloc;

    for( block = _Blks; block && (block->free < amount); ) {
        block = block->next;
    }
    if( block ) {
        alloc = (tag *)((char *)block + block->size - block->free);
        assert( *alloc == 0 );
        *alloc = amount | ALLOCATED;
        block->free -= amount;
        return( ++alloc );
    }
    if( MemFromSys( 0 ) == NULL)    // Grab one empty block
        return( NULL );
    return( GetFromBlk( amount ) ); // We know this will now succeed
}


extern pointer  MemAlloc( size_t amount )
/***************************************/
{
    char        *chunk;
    int         mem_class;

    if( !Initialized )
        MemInit();
    if( amount == 0 )
        return( NULL );
    amount = _RoundUp( amount + TAG_SIZE, WORD_SIZE );
    if( amount > MAX_ALLOC ) {
        return( MemFromSys( amount ) );
    }

    if( amount < MIN_ALLOC )
        amount = MIN_ALLOC;

    mem_class = SizeToClass( amount );
    chunk = GetFromFrl( amount, mem_class );
    if( chunk != NULL )
        return( chunk + TAG_SIZE );

    return( GetFromBlk( ClassToSize( mem_class ) ) );
}


extern void     MemFree( char *block )
/************************************/
{
    frl     *free;
    int     mem_class;
    tag     length;

    free   = (frl *)( block - TAG_SIZE );
    assert( free->length & ALLOCATED );
    free->length &= ~ALLOCATED;
    length = free->length;
    if( length > MAX_ALLOC ) {   // This was a full block
        blk_hdr     *header;
        mem_blk     *blk;

        header = (blk_hdr *)( block - sizeof( blk_hdr ) );
        blk    = header->block;
        blk->free += header->size + sizeof( blk_hdr );
        blk->size += header->size + sizeof( blk_hdr );
#ifndef NDEBUG
        // Must zero the memory for later checks in GetFromBlk
        memset( header, 0, length + sizeof( blk_hdr ) );
#endif
    } else {
        mem_class  = SizeToClass( length );
        free->link = _FreeList[ mem_class ];
        _FreeList[ mem_class ] = free;
    }
}

extern void     MemCoalesce( void )
/*********************************/
{
    return;
}


extern pointer_int      MemInUse( void )
/**************************************/
{
    if( !Initialized )
        MemInit();
    return( AllocSize );
}


extern pointer_int      MemSize( void )
/*************************************/
{
    switch( Initialized ) {
    case 0:
        MemInit();
#if defined( __QNX__ )
        /* fall through */
    case 1:
        CalcMemSize();
#endif
    }
    return( MemorySize );
}


static  pointer MemFromSys( size_t amount )
/*****************************************/
{
    pointer     ptr;
    blk_hdr     *allocated;
    mem_blk     *blk;
    size_t      size;

    // round up size to multiple of 64K
    size = _RoundUp( amount + sizeof( mem_blk ) + sizeof( blk_hdr ), _64K );
#ifdef NDEBUG
    ptr = malloc( size );
#else
    ptr = calloc( 1, size );   // Need to clear memory for later assert() calls
#endif
    if( ptr != NULL ) {
        AllocSize += size;
#ifdef MEMORY_STATS
        if( AllocSize > PeakAlloc )
            PeakAlloc = AllocSize;
#endif
        blk = ptr;
        // If amount was zero, this block will be chopped up into
        // small chunks immediately. If nonzero, return pointer
        // to memory at the end of the block and use the rest for
        // small chunks. If the initial large allocation is freed,
        // it will also be reused for small blocks.
        blk->next = _Blks;
        _Blks = blk;
        blk->free = size - amount - sizeof( mem_blk );
        blk->size = size;
        if( amount ) {
            blk->size -= amount + sizeof( blk_hdr );
            blk->free -= sizeof( blk_hdr );
            allocated = (blk_hdr *)((char *)blk + sizeof( mem_blk ) + blk->free);
            allocated->block = blk;
            allocated->size  = amount | ALLOCATED;
            ptr = (char *)allocated + sizeof( blk_hdr );
        } else {
            ptr = (char *)blk + sizeof( mem_blk );
        }
        return( ptr );
    }
    return( NULL );
}


static  void MemToSys( mem_blk *what )
/************************************/
{
    free( what );
}


extern void MemFini( void )
/*************************/
{
    mem_blk     *curr;
    mem_blk     *next;

    for( curr = _Blks; curr != NULL; curr = next ) {
        next = curr->next;
        MemToSys( curr );
    }
#ifdef MEMORY_STATS
    {
        char    buff[80];

        sprintf( buff, "Peak WCG memory usage (KB): %d\n", (int)(PeakAlloc/_1K) );
        FEMessage( MSG_INFO, buff );
        PeakAlloc = 0;
    }
#endif
    Initialized = 0;
}
