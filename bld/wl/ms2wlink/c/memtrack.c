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


#include <stdarg.h>

#define ALLOC_BYTE      0xA5
#define FREED_BYTE      0xBD
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
        TRPTR                   mem_a_list;
        unsigned long           mem_used;
        unsigned long           max_mem;
        void                    *(*alloc)(int);
        void                    *(*realloc)(void *,int);
        void                    (*free)(void*);
        void                    (*prt_line)(char*,unsigned);
        char                    name[1];
} tracker;

typedef struct track_entry {
        TRPTR                   next;
        void                    *mem;
        void                    (*who)();
        unsigned int            size;
} track_entry;

#ifdef _SYS_CHKS

extern  char            MemOk(void *,int);
extern  void            *MemOtherOk();

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

#if defined(__SMALL__) || defined(__MEDIUM__)

#define _PtrAdd( p, i )     ((void *)((char *)(p) + i))
#define _PT( p )            ((void *) p)

#else

#define _PtrAdd( p, i )     ((void *)((char huge *)(p) + i))
#define _PT( p )            ((void huge *) p)


#endif

typedef void (*code_ptr)();

#define _PrtLine                trk->prt_line
#define _Free                   trk->free
#define _Alloc                  trk->alloc
#define _ReAlloc                trk->realloc

#define Next(trk)               trk->next
#define Size(trk)               trk->size
#define Who(trk)                trk->who
#define Mem(trk)                trk->mem

#define SetNext(trk,n)         trk->next = n
#define SetSize(trk,s)         trk->size = s
#define SetWho(trk,w)          trk->who = w
#define SetMem(trk,m)          trk->mem = m

#define TR_NO_ROUTINE       ((void (*)())0)


static char *CopyStr( char *src, char *dest )
{
    while( *dest = *src ) {
        ++src;
        ++dest;
    }
    return( dest );
}

static char *FormHex( char *ptr, unsigned long data, unsigned size )
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
    va_list     args;
    char        buff[80];
    char        *ptr;
    char        ch;
    unsigned    ui;
    unsigned long ul;
    void        *dp;
    code_ptr    cp;

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
                cp = va_arg( args, code_ptr );
                if( cp != TR_NO_ROUTINE ) {
                    *ptr++ = '(';
                    ptr = FormHex( ptr, (unsigned long)cp, sizeof( cp ) );
                    *ptr++ = ')';
                }
                *ptr++ = ':';
                break;
            case 'C':   /* code pointer */
                cp = va_arg( args, code_ptr );
                ptr = FormHex( ptr, (unsigned long)cp, sizeof( cp ) );
                break;
            case 'D':   /* data pointer */
                dp = va_arg( args, void * );
                ptr = FormHex( ptr, (unsigned long)dp, sizeof( dp ) );
                break;
            case 'U':   /* unsigned integer */
                ui = va_arg( args, unsigned );
                ptr = FormHex( ptr, (unsigned long)ui, sizeof( ui ) );
                break;
            case 'L':   /* unsigned long */
                ul = va_arg( args, unsigned long );
                ptr = FormHex( ptr, (unsigned long)ul, sizeof( ul ) );
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
    _PrtLine( buff, ptr - buff );
}


#pragma off(unreferenced);
static TRPTR   AllocTrk( tracker *trk ) {
#pragma on(unreferenced);
/***********************************/

    TRPTR       entry;

#ifdef _EXTRA_MEM
    if( TrkFrl != 0 ) {
        entry = TrkFrl;
        TrkFrl = Next( TrkFrl );
    } else {
        entry = TrkBeg++;
    }
#else
    entry = _Alloc( sizeof( track_entry ) );
#endif
    return( entry );
}

#pragma off(unreferenced);
static  void    FreeTrk( TRPTR  entry, tracker *trk ) {
#pragma on(unreferenced);
/*********************************************/

#ifdef _EXTRA_MEM
    SetNext( entry, TrkFrl );
    TrkFrl = entry;
#else
    _Free( entry );
#endif
}


static  void    Fill( void *start, unsigned len, char filler ) {
/**********************************************************/

    char    *ptr;

    ptr = start;
    while( len != 0 ) {
        *ptr++ = filler;
        --len;
    }
}

extern  tracker *TrMemInit( char *name,
                            void *(*alloc)(int),
                            void *(*realloc)(void *,int),
                            void (*free)(void *),
                            void (*prt_line)(char *, unsigned) ){

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
    trk->mem_a_list = 0;
    trk->mem_used   = 0;
    trk->max_mem    = 0;
    trk->alloc      = alloc;
    trk->realloc    = realloc;
    trk->free       = free;
    trk->prt_line   = prt_line;
    CopyStr( name, trk->name );
    return( trk );
}

extern  void    TrCheck( tracker *trk ) {
/***************************************/

    TRPTR       alloc;

    alloc = trk->mem_a_list;
    while( alloc != 0 ) {
        TrValidate( Mem( alloc ), TR_NO_ROUTINE, trk );
        alloc = Next( alloc );
    }
}

extern  void    TrMemFini( tracker *trk ) {
/*****************************************/

    unsigned    chunks;
    TRPTR       alloc;

    chunks = 0;
    alloc = trk->mem_a_list;
    while( alloc != 0 ) {
        ++chunks;
        alloc = Next( alloc );
    }
    if( chunks != 0 ) {
        TrPrt( trk, "%NMemFini: %U chunks (%L bytes) unfreed",
             chunks, trk->mem_used );
    }
    while( trk->mem_a_list != 0 ) {
        TrValidate( Mem( trk->mem_a_list ), TR_NO_ROUTINE, trk );
        TrFree( Mem( trk->mem_a_list ), trk );
    }
    _Free( trk );
#ifdef _EXTRA_MEM

    ExtMemFree( ExtMem );

#endif
}

extern  void    TrPrtUsage( tracker *trk ) {
/******************************************/

    TrPrt( trk, "%N Memory usage: %L bytes", trk->max_mem );
}

extern  void    TrPrtMemUse( tracker *trk ) {
/*******************************************/

    TRPTR       alloc;

    TrPrtUsage( trk );
    alloc = trk->mem_a_list;
    if( alloc == 0 ) return;
    TrPrt( trk, " Who      Addr     Size" );
    TrPrt( trk, " ======== ======== ====" );
    do {
        TrPrt( trk, "&%C %D %U", Who( alloc ), Mem( alloc ), Size( alloc ) );
        alloc = Next( alloc );
    } while( alloc != 0 );
}

extern  void *TrAlloc( unsigned size, void (*ra)(), tracker *trk ) {
/******************************************************************/

    void        *chunk;
    TRPTR       trchunk;

    if( size == 0 ) {
        TrPrt( trk, "%W size zero allocation", "Alloc", ra );
        return( 0 );
    }
    chunk = _Alloc( size + 1 );
    if( chunk != 0 ) {
        trchunk = AllocTrk( trk );
        if( trchunk == 0 ) {
            _Free( chunk );
            return( 0 );
        }
        SetNext( trchunk, trk->mem_a_list );
        SetWho( trchunk, ra );
        SetSize( trchunk, size );
        SetMem( trchunk, chunk );
        trk->mem_a_list = trchunk;
        trk->mem_used += size;
        if( trk->mem_used > trk->max_mem ) {
            trk->max_mem = trk->mem_used;
        }
        Fill( chunk, size + 1, ALLOC_BYTE );
    }
    return( chunk );
}

static int ValidChunk( TRPTR entry, char *who, void (*ra)(), tracker *trk ) {
/****************************************************************************/

    void    *chunk;

    chunk = Mem( entry );
    if( *((char *)_PtrAdd( chunk, Size(entry) )) != ALLOC_BYTE ) {
        TrPrt( trk, "%W %D overrun allocation of %U bytes", who, ra,
             chunk, Size(entry) );
        return( FALSE );
#ifdef _SYS_CHKS
    } else if( !MemOk( chunk, Size( entry ) + 1 ) ) {
        TrPrt( trk, "%W boundry tag hammered %D", who, ra, chunk );
        return( FALSE );
    } else if( ( chunk = MemOtherOk() ) != 0 ) {
        TrPrt( trk, "%W bad free list entry at %D", who, ra, chunk );
        return( FALSE );
#endif
    }
    return( TRUE );
}

extern  int    TrValidate( void *chunk, void (*ra)(), tracker *trk ) {
/*************************************************************************/

    TRPTR       entry;

    entry = trk->mem_a_list;
    for(;;) {
        if( entry == 0 ) {
            TrPrt( trk, "%W unowned chunk %D", "Validate", ra, chunk );
            return( FALSE );
        }
        if( _PT( Mem( entry ) ) == _PT( chunk ) ) break;
        entry = Next( entry );
    }
    return( ValidChunk( entry, "Validate", ra, trk ) );
}


extern  int    TrChkRange( void *start, unsigned len,
                            void (*ra)(), tracker *trk ) {

    TRPTR       ptr;
    void        *end;
    void        *end_chunk;

    ptr = trk->mem_a_list;
    for(;;) {
        if( ptr == 0 ) {
            TrPrt( trk, "%W %D not in any allocation", "ChkRange", ra, start );
            return( FALSE );
        }
        end_chunk = _PtrAdd( Mem( ptr ), Size( ptr ) );
        if( _PT( start ) >= _PT( Mem( ptr ) )
            && _PT( start ) < _PT( end_chunk ) ) break;
        ptr = Next( ptr );
    }
    end = _PtrAdd( start, len );
    if( _PT( end ) > _PT( end_chunk ) ) {
        TrPrt( trk, "%W %D+%U overruns allocation %D+%U", "ChkRange", ra,
                start, len, Mem( ptr ), Size( ptr ) );
        return( FALSE );
    }
    return( ValidChunk( ptr, "ChkRange", ra, trk ) );
}

extern  int     TrFree( void *chunk, tracker *trk ) {
/*****************************************************/

    return( TrFreeSize( chunk, 0, trk ) );
}

extern  int     TrFreeSize( void *chunk, unsigned given_size, tracker *trk ) {
/**************************************************************************/

    unsigned    size;
    TRPTR       entry;
    TRPTR       prev;
    int         ret;

    if( chunk == 0 ) return( TRUE );
    prev = 0;
    entry = trk->mem_a_list;
    for(;;) {
        if( entry == 0 ) {
            TrPrt( trk, "%W unowned chunk %D", "Free", TR_NO_ROUTINE, chunk );
            return( FALSE );
        }
        if( _PT( chunk ) == _PT( Mem( entry ) ) ) break;
        prev = entry;
        entry = Next( entry );
    }
    ret = ValidChunk( entry, "Free", TR_NO_ROUTINE, trk );
    if( prev == 0 ) {
        trk->mem_a_list = Next( entry );
    } else {
        SetNext( prev, Next( entry ) );
    }
    size = Size( entry );
    trk->mem_used -= size;
    FreeTrk( entry, trk );
    Fill( chunk, size + 1, FREED_BYTE );
    _Free( chunk );
    if( given_size != 0 && given_size != size ) {
        TrPrt( trk, "%W %D allocated %U, freed %U", "FreeSize", TR_NO_ROUTINE,
            chunk, size, given_size );
        return( FALSE );
    }
    return( ret );
}

extern void * TrReAlloc( void *old, unsigned size, void (*ra)(), tracker *trk )
/*****************************************************************************/
// reallocate a block of memory
{
    unsigned    sizeold;
    TRPTR       entry;
    TRPTR       prev;
    void        *chunk;

    if( size == 0 ) {
        TrPrt( trk, "%W size zero reallocation", "ReAlloc", ra );
        return( 0 );
    }
    if( old == 0 ) {
        TrPrt( trk, "%W reallocation of a NULL pointer", "ReAlloc", ra );
        return( 0 );
    }
    prev = 0;
    entry = trk->mem_a_list;
    for(;;) {
        if( entry == 0 ) {
            TrPrt( trk, "%W unowned chunk %D", "ReAlloc", TR_NO_ROUTINE, old );
            return( 0 );
        }
        if( _PT( old ) == _PT( Mem( entry ) ) ) break;
        prev = entry;
        entry = Next( entry );
    }
    if( !ValidChunk( entry, "ReAlloc", TR_NO_ROUTINE, trk ) ) {
        return( 0 );
    }
    sizeold = Size( entry );
    chunk = _ReAlloc( old, size + 1 );
    if( chunk != 0 ) {
        SetWho( entry, ra );
        SetSize( entry, size );
        SetMem( entry, chunk );
        trk->mem_used -= sizeold;
        trk->mem_used += size;
        if( trk->mem_used > trk->max_mem ) {
            trk->max_mem = trk->mem_used;
        }
        *((char *)chunk + size) = ALLOC_BYTE;
    }
    return( chunk );
}
