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
* Description:  Memory usage tracking.
*
****************************************************************************/


#include <stddef.h>
#include <stdarg.h>

#define ALLOC_BYTE      0xA5    /* the fill value for allocated memory */
#define FREED_BYTE      0xBD    /* the fill value for freed memory */
#define TRACK_SIZE      0xFF0

enum { FALSE = 0==1, TRUE = 0==0 };

#ifndef FAR
    #ifdef _EXTRA_MEM
        #define FAR far
    #else
        #define FAR
    #endif
#endif

typedef struct track_entry FAR * TRPTR;

typedef struct tracker {
        TRPTR           allocated_list;
        size_t          memory_in_use;
        size_t          max_memory_used;
        void            *(*alloc)(size_t);
        void            (*free)(void*);
        void            (*print_line)(char*,unsigned);
        char            name[1];
} tracker;

typedef struct track_entry {
        TRPTR                   next;
        void                    *mem;
        void                    (*who)();
        unsigned int            size;
} track_entry;

#ifdef _SYS_CHKS

extern  char            MemOk( void *, int );
extern  void            *MemOtherOk( void );

#endif

#ifdef _EXTRA_MEM

extern  void FAR *      ExtMemAlloc(unsigned int);
#pragma aux ExtMemAlloc = \
        0xB4 0x48       /*      mov     ax,048H */ \
        0xCD 0x21       /*      int     21H */ \
        0x73 0x02       /*      jnb     L1 */ \
        0x31 0xC0       /*      xor     ax,ax */ \
        0x89 0xC2       /*L1:   mov     dx,ax */ \
        0x31 0xC0       /*      xor     ax,ax */ \
        parm routine [ bx ] \
        ;

extern  unsigned int    ExtMemAvail();
#pragma aux ExtMemAvail = \
        0xBB 0xFF 0xFF  /*      mov     bx,0FFFFH */ \
        0xB4 0x48       /*      mov     ah,48H */ \
        0xCD 0x21       /*      int     21H */ \
        0xD3 0xE3       /*      shl     bx,cl */ \
        value [ bx ] \
        ;

extern  void            ExtMemFree(void FAR *);
#pragma aux ExtMemFree = \
        0x06            /*      push    es */ \
        0x8E 0xC2       /*      mov     es,dx */ \
        0xB4 0x49       /*      mov     ah,49H */ \
        0xCD 0x21       /*      int     21H */ \
        0x07            /*      pop     es */ \
        parm routine [ dx ax ] \
        ;


static TRPTR    TrkFrl;
static TRPTR    ExtMem;
static TRPTR    TrkBeg;

#endif

#define _POINTER_ADD( p, i )    ((void *)((char *)(p) + i))
#define _POINTER( p )           ((void *) p)

#define CODE_PTR_TYPE           void *

#define TR_NO_ROUTINE           ((void (*)())0)

extern  int     TrValidate( void *, void (*ra)(), tracker * );
extern  int     TrFree( void *, tracker * );
extern  int     TrFreeSize( void *, unsigned, tracker * );

static char *CopyStr( char *src, char *dest )
{
    while( (*dest = *src) != '\0' ) {
        ++src;
        ++dest;
    }
    return( dest );
}

static char *FormHex( char *ptr, size_t data, unsigned size )
{
    char            *str;

    size *= 2;
    ptr += size;
    str = ptr;
    for( ; size > 0; size-- ) {
        *--str = "0123456789abcdef"[data & 0x0f];
        data >>= 4;
    }
    return( ptr );
}

void TrPrt( tracker *trk, char *fmt, ... )
{
    va_list         args;
    char            buff[80];
    char            *ptr;
    char            ch;
    unsigned        ui;
    size_t          ul;
    void            *dp;
    CODE_PTR_TYPE   cp;

    va_start( args, fmt );
    ptr = buff;
    for( ;; ) {
        ch = *fmt++;
        if( ch == '\0' ) break;
        if( ch == '%' ) {
            ch = *fmt++;
            switch( ch ) {
            case 'N':   /* name of tracker invoker */
                ptr = CopyStr( trk->name, ptr );
                break;
            case 'W':   /* "%Na1(a2):" */
                ptr = CopyStr( trk->name, ptr );
                ptr = CopyStr( va_arg( args, char * ), ptr );
                cp = va_arg( args, CODE_PTR_TYPE );
                if( cp != TR_NO_ROUTINE ) {
                    *ptr++ = '(';
                    ptr = FormHex( ptr, (size_t)cp, sizeof( cp ) );
                    *ptr++ = ')';
                }
                *ptr++ = ':';
                break;
            case 'C':   /* code pointer */
                cp = va_arg( args, CODE_PTR_TYPE );
                ptr = FormHex( ptr, (size_t)cp, sizeof( cp ) );
                break;
            case 'D':   /* data pointer */
                dp = va_arg( args, void * );
                ptr = FormHex( ptr, (size_t)dp, sizeof( dp ) );
                break;
            case 'U':   /* unsigned integer */
                ui = va_arg( args, unsigned );
                ptr = FormHex( ptr, (unsigned long)ui, sizeof( ui ) );
                break;
            case 'L':   /* size_t */
                ul = va_arg( args, size_t );
                ptr = FormHex( ptr, ul, sizeof( ul ) );
                break;
            default:
                *ptr++ = ch;
                break;
            }
        } else {
           *ptr++ = ch;
        }
    }
    va_end( args );
    *ptr = '\0';
    trk->print_line( buff, ptr - buff );
}


static TRPTR   AllocTrk( tracker *trk )
/*************************************/
{
    TRPTR       entry;

    trk = trk;
#ifdef _EXTRA_MEM
    if( TrkFrl != 0 ) {
        entry = TrkFrl;
        TrkFrl = TrkFrl->next;
    } else {
        entry = TrkBeg++;
    }
#else
    entry = trk->alloc( sizeof( track_entry ) );
#endif
    return( entry );
}

static  void    FreeTrk( TRPTR  entry, tracker *trk )
{
    trk = trk;
#ifdef _EXTRA_MEM
    entry->next = TrkFrl;
    TrkFrl = entry;
#else
    trk->free( entry );
#endif
}


static  void    Fill( void *start, unsigned len, unsigned char filler )
{
    char    *ptr;

    ptr = start;
    while( len != 0 ) {
        *ptr++ = filler;
        --len;
    }
}

extern  tracker *TrMemInit( char *name,
                            void *(*alloc)(size_t),
                            void (*free)(void *),
                            void (*print_line)(char *, unsigned) )
{
    tracker     *trk;
    char        *p1;

    p1 = name;
    while( *p1 ) {
        ++p1;
    }
    trk = alloc( sizeof( tracker ) + ( p1 - name ) );
#ifdef _EXTRA_MEM
    if( ExtMem == 0 ) {
        ExtMem = ExtMemAlloc( TRACK_SIZE );
        if( ExtMem == (void FAR *)0 ) {
            ExtMem = ExtMemAlloc( ExtMemAvail() );
        }
        TrkFrl = 0;
        TrkBeg = ExtMem;
    }
#endif
    if( trk == 0 ) return( 0 );
    trk->allocated_list = 0;
    trk->memory_in_use   = 0;
    trk->max_memory_used = 0;
    trk->alloc      = alloc;
    trk->free       = free;
    trk->print_line   = print_line;
    CopyStr( name, trk->name );
    return( trk );
}

extern int TrFree( void *chunk, tracker *trk );
static int ValidChunk( TRPTR entry, char *who, void (*ra)(), tracker *trk );

extern  int TrValidate( void *chunk, void (*ra)(), tracker *trk )
{
    TRPTR       entry;

    entry = trk->allocated_list;
    for(;;) {
        if( entry == 0 ) {
            TrPrt( trk, "%W unowned chunk %D", "Validate", ra, chunk );
            return( FALSE );
        }
        if( _POINTER( entry->mem ) == _POINTER( chunk ) ) break;
        entry = entry->next;
    }
    return( ValidChunk( entry, "Validate", ra, trk ) );
}

extern  void    TrCheck( tracker *trk )
{
    TRPTR       alloc;

    alloc = trk->allocated_list;
    while( alloc != 0 ) {
        TrValidate( alloc->mem, TR_NO_ROUTINE, trk );
        alloc = alloc->next;
    }
}

extern  void    TrMemFini( tracker *trk )
{
    unsigned    chunks;
    TRPTR       alloc;

    chunks = 0;
    alloc = trk->allocated_list;
    while( alloc != 0 ) {
        ++chunks;
        alloc = alloc->next;
    }
    if( chunks != 0 ) {
        TrPrt( trk, "%NMemFini: %U chunks (%L bytes) unfreed", chunks, trk->memory_in_use );
    }
    while( trk->allocated_list != 0 ) {
        TrValidate( trk->allocated_list->mem, TR_NO_ROUTINE, trk );
        TrFree( trk->allocated_list->mem, trk );
    }
    trk->free( trk );
#ifdef _EXTRA_MEM

    ExtMemFree( ExtMem );

#endif
}

extern  void    TrPrtUsage( tracker *trk )
{
    TrPrt( trk, "%N Memory usage: %L bytes", trk->max_memory_used );
}

extern  void    TrPrtMemUse( tracker *trk )
{
    TRPTR       alloc;

    TrPrtUsage( trk );
    alloc = trk->allocated_list;
    if( alloc == 0 ) return;
    TrPrt( trk, " Who      Addr     Size" );
    TrPrt( trk, " ======== ======== ====" );
    do {
        TrPrt( trk, "&%C %D %U", alloc->who, alloc->mem, alloc->size );
        alloc = alloc->next;
    } while( alloc != 0 );
}

extern  void *TrAlloc( unsigned size, void (*ra)(), tracker *trk )
{
    void        *chunk;
    TRPTR       trchunk;

    if( size == 0 ) {
        TrPrt( trk, "%W size zero allocation", "Alloc", ra );
        return( 0 );
    }
    chunk = trk->alloc( size + 1 );
    if( chunk != 0 ) {
        trchunk = AllocTrk( trk );
        trchunk->next = trk->allocated_list;
        trchunk->who = ra;
        trchunk->size = size;
        trchunk->mem = chunk;
        trk->allocated_list = trchunk;
        trk->memory_in_use += size;
        if( trk->memory_in_use > trk->max_memory_used ) {
            trk->max_memory_used = trk->memory_in_use;
        }
        Fill( chunk, size + 1, ALLOC_BYTE );
    }
    return( chunk );
}

static int ValidChunk( TRPTR entry, char *who, void (*ra)(), tracker *trk )
{
    void    *chunk;

    chunk = entry->mem;
    if( *((unsigned char *)_POINTER_ADD( chunk, entry->size )) != ALLOC_BYTE ) {
        TrPrt( trk, "%W %D overrun allocation of %U bytes", who, ra,
             chunk, entry->size );
        return( FALSE );
#ifdef _SYS_CHKS
    } else if( !MemOk( chunk, entry->size + 1 ) ) {
        TrPrt( trk, "%W boundry tag hammered %D", who, ra, chunk );
        return( FALSE );
    } else if( ( chunk = MemOtherOk() ) != 0 ) {
        TrPrt( trk, "%W bad free list entry at %D", who, ra, chunk );
        return( FALSE );
#endif
    }
    return( TRUE );
}

extern  int TrChkRange( void *start, unsigned len, void (*ra)(), tracker *trk )
{

    TRPTR       ptr;
    void        *end;
    void        *end_chunk;

    ptr = trk->allocated_list;
    for(;;) {
        if( ptr == 0 ) {
            TrPrt( trk, "%W %D not in any allocation", "ChkRange", ra, start );
            return( FALSE );
        }
        end_chunk = _POINTER_ADD( ptr->mem, ptr->size );
        if( _POINTER( start ) >= _POINTER( ptr->mem )
            && _POINTER( start ) < _POINTER( end_chunk ) ) break;
        ptr = ptr->next;
    }
    end = _POINTER_ADD( start, len );
    if( _POINTER( end ) > _POINTER( end_chunk ) ) {
        TrPrt( trk, "%W %D+%U overruns allocation %D+%U", "ChkRange", ra,
                start, len, ptr->mem, ptr->size );
        return( FALSE );
    }
    return( ValidChunk( ptr, "ChkRange", ra, trk ) );
}

extern  int     TrFreeSize( void *chunk, unsigned given_size, tracker *trk )
{
    unsigned    size;
    TRPTR       entry;
    TRPTR       prev;
    int         ret;

    if( chunk == 0 ) return( TRUE );
    prev = 0;
    entry = trk->allocated_list;
    for(;;) {
        if( entry == 0 ) {
            TrPrt( trk, "%W unowned chunk %D", "Free", TR_NO_ROUTINE, chunk );
            return( FALSE );
        }
        if( _POINTER( chunk ) == _POINTER( entry->mem ) ) break;
        prev = entry;
        entry = entry->next;
    }
    ret = ValidChunk( entry, "Free", TR_NO_ROUTINE, trk );
    if( prev == 0 ) {
        trk->allocated_list = entry->next;
    } else {
        prev->next = entry->next;
    }
    size = entry->size;
    trk->memory_in_use -= size;
    FreeTrk( entry, trk );
    Fill( chunk, size + 1, FREED_BYTE );
    trk->free( chunk );
    if( given_size != 0 && given_size != size ) {
        TrPrt( trk, "%W %D allocated %U, freed %U", "FreeSize", TR_NO_ROUTINE,
            chunk, size, given_size );
        return( FALSE );
    }
    return( ret );
}

extern  int TrFree( void *chunk, tracker *trk )
{
    return( TrFreeSize( chunk, 0, trk ) );
}
