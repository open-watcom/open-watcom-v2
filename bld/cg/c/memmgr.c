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


#include <stdlib.h>

#include "standard.h"
#include "ptrint.h"
#include "hostsys.h"
#include "cg.h"
#if defined( M_I86 ) || defined( M_I386 )
#include <i86.h>
#endif
#include <unistd.h>
#if defined( __NT__ )
#include <windows.h>
#endif

#ifdef __DOS__

    #include "tinyio.h"

    typedef struct {
            unsigned long largest_free;
            unsigned long max_unlocked_page_alloc;
            unsigned long max_locked_page_alloc;
            unsigned long linear_addr_space_in_pages;
            unsigned long total_unlocked_pages;
            unsigned long free_pages;
            unsigned long physical_pages;
            unsigned long free_linear_addr_space_in_pages;
            unsigned long size_of_page_file_in_pages;
            unsigned long fill[4];
    } dpmi_mem;

    extern long _TinyDPMIGetFreeMemoryInformation( dpmi_mem * );
    #pragma aux _TinyDPMIGetFreeMemoryInformation = \
            0x06                  /* push es */ \
            0x1E                  /* push ds */ \
            0x07                  /* pop es */ \
            _MOV_AH 0x05          /* mov ah,05h */ \
            _MOV_AL 0x00          /* mov al,00h */ \
            _INT 0x31         /* int 31h */ \
            0x07                  /* pop es */ \
            _SBB_AX_AX    /* sbb ax,ax */ \
            parm[edi] value[eax];

    #include "extender.h"

    extern char int2f( short );
    #pragma aux int2f = 0xcd 0x2f parm [ ax ] value [ al ];

#endif

#if defined(__COMPACT__) || defined(__LARGE__) || defined(__HUGE__)
    #define PtrGreater( a, b ) ( FP_SEG( a ) > FP_SEG( b ) || \
                                 ( FP_SEG( a ) == FP_SEG( b ) && \
                                   FP_OFF( a ) > FP_OFF( b ) ) )
#else
    #define PtrGreater( a,b ) ( (a) > (b) )
#endif

typedef    unsigned_32  tag;

#ifdef __AXP__
#define WORD_SIZE 8
#else
#define WORD_SIZE (_HOST_INTEGER/8)
#endif

extern      bool    GetEnvVar(char*,char*,int);
extern      char    *CopyStr( char *src, char *dst );
extern      uint    Length(char*);


#if defined( __DOS__ )
    #pragma     aux     __psp "*";
    extern      short   __psp;
#endif

#if defined( __OSI__ )
    #define __CALL21__
    #include "tinyio.h"
#elif defined( __QNX__ )
    #include <sys/osinfo.h>
    #include <sys/seginfo.h>
#elif defined( __DOS__ )
    #include    "dosmem.h"
#endif

#define PARAGRAPH_SIZE  16L
#define _64K            (4096L*PARAGRAPH_SIZE)
#define _1K             1024L
#define _4K             (4*_1K)
#define _1M             (_1K*_1K)
#define _4M             (4*_1M)
#define _16M            (16*_1M)

extern  void    FatalError(char *);
static  char    *MemFromSys( tag );

static  pointer_int     AllocSize = { 0 };
static  pointer_int     MemorySize;


#define ALLOCATED       1

#ifdef __AXP__
#define MAX_SIZE        15 /* 32k */
#define MIN_SIZE        8  /* 32 */
#else
#define MAX_SIZE        11 /* 2048 */
#define MIN_SIZE        4  /* 16 */
#endif

#if (1<<MIN_SIZE) < (4*WORD_SIZE)
    #error "Free list will not fit into freed chunk"
#endif

#define MAX_CLASS       (MAX_SIZE-MIN_SIZE)

typedef struct next_prev {
        struct  frl     *next;
        struct  frl     *prev;
} next_prev;

typedef struct frl {
        tag             length;
        int             class;
        next_prev       link[ MAX_CLASS+1 ];
} frl;

#ifdef __AXP__
#define TAG_SIZE        8
#else
#define TAG_SIZE        sizeof( tag )
#endif

#define MIN_ALLOC (((TAG_SIZE+sizeof(int)+sizeof(next_prev))+(WORD_SIZE-1))&~(WORD_SIZE-1))

int     Initialized     = 0;

frl     _FreeList = {
        ALLOCATED,
        0,
        &_FreeList,
        &_FreeList,
        &_FreeList,
        &_FreeList,
        &_FreeList,
        &_FreeList,
        &_FreeList,
        &_FreeList,
        &_FreeList,
        &_FreeList,
        &_FreeList,
        &_FreeList,
        &_FreeList,
        &_FreeList,
        &_FreeList,
        &_FreeList
        };

#define _WALKTAG( free ) ((frl *)((char *)(free) + (free)->length ))

typedef struct mem_blk {
    struct mem_blk   *next;
    tag               len;
}mem_blk;
#define BLK_ADJUST  ((sizeof( mem_blk) + PARAGRAPH_SIZE)/PARAGRAPH_SIZE*PARAGRAPH_SIZE)
mem_blk  *Blks;

static  int     SizeToClass( tag amount ) {
/*****************************************/

    int class;

    if( amount >= ( 1 << MAX_SIZE ) ) {
        return( MAX_CLASS );
    }
    if( amount < ( 1 << ( MIN_SIZE + 1 ) ) ) {
        return( 0 );
    }
    amount >>= MIN_SIZE;
    class = 0;
    for( ;; ) {
        ++class;
        amount >>= 1;
        if( amount == 0 ) break;
    }
    return( class );
}


static  pointer GetFromFrl( tag amount, int frl_class ) {
/***************************************************/

    frl                 *alloc;
    frl                 *free;
    int                 i;
    int                 class;
    signed_pointer_int  amount_left;

    free = _FreeList.link[ frl_class ].next;
    while( free != &_FreeList ) {
        amount_left = (signed_pointer_int)free->length - amount;
        if( amount_left >= 0 ) {
            if( amount_left >= MIN_ALLOC ) {
                class = SizeToClass( amount_left );
                for( i = class + 1; i <= free->class; ++i ) {
                    free->link[ i ].next->link[ i ].prev = free->link[ i ].prev;
                    free->link[ i ].prev->link[ i ].next = free->link[ i ].next;
                }
                free->class = class;
                free->length = amount_left;
                alloc = _WALKTAG( free );
                alloc->length = amount;
            } else {
                for( i = 0; i <= free->class; ++i ) {
                    free->link[ i ].next->link[ i ].prev = free->link[ i ].prev;
                    free->link[ i ].prev->link[ i ].next = free->link[ i ].next;
                }
                alloc = free;
            }
            AllocSize += alloc->length;
            alloc->length |= ALLOCATED;
            return( alloc );
        }
        free = free->link[ frl_class ].next;
    }
    return( NULL );
}


extern  pointer MemAlloc( tag amount ) {
/**************************************/

    char        *chunk;
    int         class;

    if( !Initialized ) MemInit();
    if( amount == 0 ) return( NULL );
    amount = ( amount + TAG_SIZE + (WORD_SIZE-1) ) & ~(WORD_SIZE-1);
    if( amount < MIN_ALLOC ) {
        amount = MIN_ALLOC;
    }
    class = SizeToClass( amount );
    for( ;; ) {
        chunk = GetFromFrl( amount, class );
        if( chunk != NULL ) return( chunk + TAG_SIZE );
        if( class == MAX_CLASS ) break;
        ++class;
    }
    return( (pointer)MemFromSys( amount ) );
}


static  void    UnHook( frl *unhook, frl *neighbours ) {
/******************************************************/

    int         i;
    next_prev   *neighbour_link;
    next_prev   *unhook_link;

    unhook_link = unhook->link;
    neighbour_link = neighbours->link;
    for( i = 0; i <= unhook->class; ++i ) {
        if( neighbour_link->next == unhook ) {
            neighbour_link->next = unhook_link->next;
        }
        if( neighbour_link->prev == unhook ) {
            neighbour_link->prev = unhook_link->prev;
        }
        ++neighbour_link;
        ++unhook_link;
    }
    unhook_link = unhook->link;
    for( i = 0; i <= unhook->class; ++i ) {
        unhook_link->next->link[ i ].prev = unhook_link->prev;
        unhook_link->prev->link[ i ].next = unhook_link->next;
        ++unhook_link;
    }
}


extern  tag     MemBlkSize( char *block )
/***************************************/
{
    frl         *alloc;

    block -= TAG_SIZE;
    alloc = (frl *)block;
    return( ( alloc->length & ~ALLOCATED ) - TAG_SIZE );
}


extern  void    MemFree( char *block ) {
/**************************************/

    int         i;
    frl         *free;
    frl         *prev;
    frl         *next;
    frl         *search;
    frl         neighbours;
    next_prev   *neighbour_link;
    next_prev   *free_link;

    block -= TAG_SIZE;
    free = (frl *)block;
    free->length &= ~ALLOCATED;
    AllocSize -= free->length;
    search = &_FreeList;
    neighbour_link = &neighbours.link[ MAX_CLASS ];
    for( i = MAX_CLASS; i >= 0; --i ) {
        for( ;; ) {
            prev = search;
            search = search->link[ i ].next;
            if( PtrGreater( search, free ) ) break;
            if( search->length == ALLOCATED ) break;
        }
        search = prev;
        neighbour_link->prev = prev;
        neighbour_link->next = prev->link[ i ].next;
        --neighbour_link;
    }
    next = prev->link[ 0 ].next;
    if( _WALKTAG( prev ) == free ) {
        UnHook( prev, &neighbours );
        prev->length += free->length;
        free = prev;
    }
    if( _WALKTAG( free ) == next ) {
        UnHook( next, &neighbours );
        free->length += next->length;
    }
    free->class = SizeToClass( free->length );
    neighbour_link = neighbours.link;
    free_link = free->link;
    for( i = 0; i <= free->class; ++i ) {
        neighbour_link->prev->link[ i ].next = free;
        neighbour_link->next->link[ i ].prev = free;
        free_link->next = neighbour_link->next;
        free_link->prev = neighbour_link->prev;
        ++free_link;
        ++neighbour_link;
    }
}

extern  void    MemCoalesce() {
/*****************************/

    return;
}


extern  pointer_int     MemInUse( ) {
/***********************************/

    if( !Initialized ) MemInit();
    return( AllocSize );
}


extern  pointer_int     MemSize( ) {
/**********************************/


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


#if  defined( __DOS__ ) || defined( __QNX__ )

static  void    NotEnoughMem()
/****************************/
{
    FatalError( "Not enough memory to satisfy WCGMEMORY" );
}

#endif

static  tag     AddBlock( char *block, int paragraphs ) {
/*****************************************************/

    tag         length;
    frl         *free;

    if( paragraphs == 0 ) return( 0 );
    length = ( paragraphs-1 ) * PARAGRAPH_SIZE + ( PARAGRAPH_SIZE-TAG_SIZE );
    ((frl *)(block + length))->length = ALLOCATED;
    free = (frl *)block;
    free->length = length;
    AllocSize += length;
    MemFree( block + TAG_SIZE );
    return( length );
}


extern  void    MemAddBlock( pointer blk ) {
/******************************************/


#if defined( __DOS__ )
    unsigned_16 seg;
    unsigned_16 paragraphs;
    char        *free;

    seg = (unsigned_16)blk;
    free = (char *)MK_FP( seg, 0 );
    paragraphs = *(unsigned_16 *)free / PARAGRAPH_SIZE;
    AddBlock( free, paragraphs );
    MemorySize += (pointer_int)paragraphs * PARAGRAPH_SIZE;
#endif
    blk = blk;
}


static  char    *MemFromSys( tag amount ) {
/*****************************************/
    char    *ptr;
    mem_blk *blk;
    unsigned size;

    size = (amount+sizeof( mem_blk ) + (_64K - 1)) & - _64K;  // round up to multiple of 64K
    ptr = malloc( size );
    if( ptr != NULL ) {
        blk = (void *)ptr;
        blk->len = size;
        blk->next = Blks;
        Blks = blk;
        ptr += BLK_ADJUST;
        size -= BLK_ADJUST;
        AddBlock( ptr, ( size / PARAGRAPH_SIZE ) );
        return( MemAlloc( amount ) );
    }
    return( NULL );
}

static  void MemtoSys( mem_blk *what ) {
/*****************************************/
    free( what );
}

static  void    CalcMemSize() {
/*****************************/

    bool        max_size_queried;
    bool        size_queried;
    unsigned_32 size_requested;
    unsigned_32 memory_available;
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
                #ifdef __DOS__
                    memory_available = *(char * far *)MK_FP( __psp, 0x60 ) - memstart;
                #endif
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
    #endif
    if( max_size_queried || size_queried ) {
        myitoa( (int)(memory_available/_1K), CopyStr( (char *)"Maximum WCGMEMORY=", (char *)buff ) );
        write( 1, buff, Length( buff ) );
        write( 1, "\r\n", 2 );
    }
}

static  void    MemInit() {
/*************************/

    if( !Initialized ) {
        int i;

        _FreeList.length = ALLOCATED;
        _FreeList.class = 0;
        for( i=0; i<= MAX_CLASS; ++i ){
            _FreeList.link[i].next = &_FreeList;
            _FreeList.link[i].prev = &_FreeList;
        }
        Initialized = 1;
        AllocSize = 0;
        Blks = NULL;
#if !defined( __QNX__ )
        CalcMemSize();
#endif
    }
}

extern void MemFini( void ){
/**************************/
    mem_blk *curr;
    mem_blk *next;

    curr = Blks;
    while( curr != NULL ){
        next = curr->next;
        MemtoSys( curr );
        curr = next;
    }
    Initialized = 0;
}

int myatoi( char *p ) {
/*********************/

    int         i;

    i = 0;
    while( *p != '\0' ) {
        i *= 10;
        i += *p - '0';
        ++p;
    }
    return( i );
}


void myitoa( int i, char *p ) {
/*****************************/

    char        buff[20];
    char        *q;

    q = buff+20;
    *--q = '\0';
    while( i != 0 ) {
        *--q = ( i % 10 ) + '0';
        i /= 10;
    }
    CopyStr( q, p );
}
