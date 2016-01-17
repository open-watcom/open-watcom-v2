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
#include <string.h>
#include <ctype.h>
#if defined( _M_IX86 )
#include <i86.h>
#endif
#if defined( __WINDOWS__ )
#define INCLUDE_TOOLHELP_H
#include <windows.h>
#endif
#include "trmem.h"


#if defined( _M_I86 ) && defined( __WINDOWS__ )
#pragma library (toolhelp);
#endif

typedef unsigned long   uint_32;
typedef unsigned        uint;

#define MEMSET(p,c,l)   memset(p,c,l)

/*
    _PtrCmp( a, op, b ) compares two pointer as in ( a ) op ( b )
*/
#if defined( _M_I86CM ) || defined( _M_I86LM ) || defined( _M_I86HM )
    #define _PtrAdd( p, i ) ((void *)((char __huge *)(p) + i))
    #define _PtrSub( p, i ) ((void *)((char __huge *)(p) - i))
    #define _PtrCmp(a,op,b) ((void __huge *)(a) op (void __huge *)(b))
#else
    #define _PtrAdd( p, i ) ((void *)((char *)(p) + i))
    #define _PtrSub( p, i ) ((void *)((char *)(p) - i))
    #define _PtrCmp(a,op,b) ((void *)(a) op (void *)(b))
#endif


#define msg(a,b)     static const char MSG_##a[]=b

msg(OUT_OF_MEMORY,      "Tracker out of memory" );
msg(CHUNK_BYTE_UNFREED, "%U chunks (%L bytes) unfreed" );
msg(SIZE_ZERO,          "%W size zero" );
msg(OVERRUN_ALLOCATION, "%W %D overrun allocation by %C of %U bytes" );
//msg(UNDERRUN_ALLOCATION,"%W %D underrun allocation by %C of %U bytes" );
msg(UNOWNED_CHUNK,      "%W unowned chunk %D" );
msg(NULL_PTR,           "%W NULL pointer" );
msg(NO_ROUTINE,         "Tracker was not given a %S routine!" );
msg(NOT_IN_ALLOCATION,  "%W %D not in any allocation" );
msg(OVERRUN_2,          "%W %D+%U overruns allocation %D+%U" );
msg(PRT_USAGE,          "Current usage: %L bytes; Peak usage: %L bytes" );
msg(MIN_ALLOC,          "%W allocation of %U less than minimum size" );
#if defined( _M_I86SM )
    msg(PRT_LIST_1,     "Who  Addr Size   Call   Contents" );
    msg(PRT_LIST_2,     "==== ==== ==== ======== ===========================================" );
#elif defined( _M_I86MM )
    msg(PRT_LIST_1,     "   Who    Addr Size   Call   Contents" );
    msg(PRT_LIST_2,     "========= ==== ==== ======== ===========================================" );
#elif defined( _M_I86CM )
    msg(PRT_LIST_1,     "Who    Addr    Size   Call   Contents" );
    msg(PRT_LIST_2,     "==== ========= ==== ======== ===========================================" );
#elif defined( _M_I86LM ) || defined( _M_I86HM )
    msg(PRT_LIST_1,     "   Who      Addr    Size   Call   Contents" );
    msg(PRT_LIST_2,     "========= ========= ==== ======== ===========================================" );
#else
    msg(PRT_LIST_1,     "  Who      Addr     Size     Call     Contents" );
    msg(PRT_LIST_2,     "======== ======== ======== ======== ===========================================" );
#endif
msg(PRT_LIST_3,         "%C %D %U %L %X" );

#undef msg

#define ALLOC_BYTE      0xA5
#define FREED_BYTE      0xBD

/*
   SIZE_DELTA is the maximum allowed difference between the requested size
              for allocation and what was actually allocated.
              It has been selected based on the assumption that the worst
              case delta is a request of 1 byte that gets allocated as
              64 bytes.  We can't cut it too close because skip list
              allocators often have extreme minimum sizes.
*/
#define SIZE_DELTA      64

typedef struct Entry entry, *entry_ptr, **entry_ptr_ptr;
struct Entry {
    entry_ptr       next;
    void            *mem;
    _trmem_who      who;
    size_t          size;       // real size = tr ^ mem ^ who ^ size
    uint_32         when;
};

struct _trmem_internal {
    entry_ptr   alloc_list;
    uint_32     mem_used;
    uint_32     max_mem;
    uint_32     alloc_no;
    void *      (*alloc)( size_t );
    void        (*free)( void * );
    void *      (*realloc)( void *, size_t );
    void *      (*expand)( void *, size_t );
    void *      prt_parm;
    void        (*prt_line)( void *, const char *, size_t );
    uint        flags;
    size_t      min_alloc;
#ifdef __WINDOWS__
    uint            use_code_seg_num;
#endif
};

//#pragma warning 579 9;  // shut up pointer truncated warning
static void setSize( entry_ptr p, size_t size )
{
    p->size = size ^ (size_t)p->mem ^ (size_t)p->who ^ (size_t)p;
}

static size_t getSize( entry_ptr p )
{
    return( p->size ^ (size_t)p->mem ^ (size_t)p->who ^ (size_t)p );
}
//#pragma warning 579 4;  // reenable pointer truncated warning.

static char *stpcpy( char *dest, const char *src )
{
    *dest = *src;
    while( *dest ) {
        ++dest;
        ++src;
        *dest = *src;
    }
    return( dest );
}

static char *formHex( char *ptr, uint_32 data, uint size )
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

#if defined(_M_I86LM) || defined(_M_I86HM) || defined(_M_I86MM) || defined(_M_I86CM)
static char * formFarPtr( char *ptr, void far *data )
/***************************************************/
{
    ptr = formHex( ptr, FP_SEG(data), 2 );
    *ptr = ':';
    ptr++;
//#pragma warning 579 9;  // shut up pointer truncated warning for FP_OFF
    return formHex( ptr, FP_OFF(data), sizeof( void near * ) );
//#pragma warning 579 4;  // reenable pointer truncated warning
}
#endif

static char * formCodePtr( _trmem_hdl hdl, char *ptr, _trmem_who who )
{
#ifdef __WINDOWS__
//#pragma warning 579 9;  // shut up pointer truncated warning for FP_OFF
    GLOBALENTRY     entry;

    if( hdl->use_code_seg_num ) {
        MEMSET( &entry, 0, sizeof( GLOBALENTRY ) );
        entry.dwSize = sizeof( GLOBALENTRY );
        if( GlobalEntryHandle( &entry, (HGLOBAL) GlobalHandle( FP_SEG( who ) ) ) ) {
            if( entry.wType == GT_CODE ) {
                who = (_trmem_who) MK_FP( entry.wData, FP_OFF( who ) );
            }
        }
    }
//#pragma warning 579 4;  // reenable pointer truncated warning
#else
    hdl = hdl;
#endif
#if defined( _M_I86LM ) || defined( _M_I86HM ) || defined( _M_I86MM )
    return formFarPtr( ptr, who );
#else
    return formHex( ptr, (uint_32) who, sizeof(who) );
#endif
}

static void trPrt( _trmem_hdl hdl, const char *fmt, ... )
{
    va_list     args;
    char        buff[100];
    char *      ptr;
    char        ch;
    uint        ui;
    uint_32     ul;
    void        *dp;
    _trmem_who  who;
    char *      start;
    char *      xptr;
    int         i;
    size_t      size;

    va_start( args, fmt );
    ptr = buff;
    for(;;) {
        ch = *fmt++;
        if( ch == '\0' ) break;
        if( ch == '%' ) {
            ch = *fmt++;
            switch( ch ) {
            case 'W':   /* "a1(a2):" */
                ptr = stpcpy( ptr, va_arg( args, const char * ) );
                who = va_arg( args, _trmem_who );
                if( who != _TRMEM_NO_ROUTINE ) {
                    *ptr++ = '(';
                    ptr = formHex( ptr, (uint_32)who, sizeof( who ) );
                    *ptr++ = ')';
                }
                *ptr++ = ':';
                break;
            case 'C':   /* code pointer */
                who = va_arg( args, _trmem_who );
                ptr = formCodePtr( hdl, ptr, who );
                break;
            case 'D':   /* data pointer */
                dp = va_arg( args, void * );
#if defined( _M_I86LM ) || defined( _M_I86HM ) || defined( _M_I86CM )
                ptr = formFarPtr( ptr, dp );
#else
                ptr = formHex( ptr, (uint_32)dp, sizeof( dp ) );
#endif
                break;
            case 'S':   /* char * (string) pointer */
                ptr = stpcpy( ptr, va_arg( args, char * ) );
                break;
            case 'U':   /* unsigned integer */
                ui = va_arg( args, uint );
                ptr = formHex( ptr, (uint_32)ui, sizeof( ui ) );
                break;
            case 'L':   /* unsigned long */
                ul = va_arg( args, uint_32 );
                ptr = formHex( ptr, (uint_32)ul, sizeof( ul ) );
                break;
            case 'X':   /* 14 bytes of hex data */
                start = va_arg( args, char* );
                size = va_arg( args, size_t );
                if( size > 14 ) size = 14;
                xptr = start;
                for( i=0; i<14; i++ ) {
                    if( i < size ) {
                        ptr = formHex( ptr, *xptr, sizeof( char ) );
                        xptr++;
                    } else {    // no more to print, so make things line up.
                        *ptr = ' ';
                        *(ptr + 1) = ' ';
                        ptr += 2;
                    }
                    if( i == 7 ) {
                        *ptr = ' ';
                        ptr++;
                    }
                }
                for( i=0; i < size; i++ ) {
                    if( isprint( *start ) ) {
                        *ptr = *start;
                    } else {
                        *ptr = '.';
                    }
                    ptr++;
                    start++;
                }
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
    *ptr++ = '\r';
    *ptr++ = '\n';
    *ptr = '\0';
    hdl->prt_line( hdl->prt_parm, buff, ptr - buff );
}

static entry_ptr allocEntry( _trmem_hdl hdl )
{
    entry_ptr   tr;

    tr = (entry_ptr) hdl->alloc( sizeof( entry ) );
    if( tr == NULL && ( hdl->flags & _TRMEM_OUT_OF_MEMORY ) ) {
        trPrt( hdl, MSG_OUT_OF_MEMORY );
    }
    return( tr );
}

static void freeEntry( entry_ptr tr, _trmem_hdl hdl )
{
    hdl->free( tr );
}

static void addToList( entry_ptr tr, _trmem_hdl hdl )
{
    tr->next = hdl->alloc_list;
    hdl->alloc_list = tr;
}

static entry_ptr findOnList( void *mem, _trmem_hdl hdl )
{
    entry_ptr       walk;

    walk = hdl->alloc_list;
    while( walk ) {
        if( _PtrCmp( walk->mem, ==, mem ) ) {
            return( walk );
        }
        walk = walk->next;
    }
    return( NULL );
}

static entry_ptr removeFromList( void *mem, _trmem_hdl hdl )
{
    entry_ptr_ptr   walk;
    entry_ptr       found;

    walk = &hdl->alloc_list;
    while( *walk ) {
        if( _PtrCmp( (*walk)->mem, ==, mem ) ) {
            found = *walk;
            *walk = found->next;
            return( found );
        }
        walk = &(*walk)->next;
    }
    return( NULL );
}

_trmem_hdl _trmem_open(
    void *( *alloc )( size_t ),
    void ( *free )( void * ),
    void *( *realloc )( void *, size_t ),
    void *( *expand )( void *, size_t ),
    void *prt_parm,
    void ( *prt_line )( void *, const char *, size_t ),
    unsigned flags )
/*****************************************************/
{
    _trmem_hdl  hdl;

    hdl = (_trmem_hdl) alloc( sizeof( struct _trmem_internal ) );
    if( hdl == NULL ) {
        return( NULL );
    }
    hdl->alloc          = alloc;
    hdl->free           = free;
    hdl->realloc        = realloc;
    hdl->expand         = expand;
    hdl->prt_parm       = prt_parm;
    hdl->prt_line       = prt_line;
    hdl->flags          = flags;
    hdl->alloc_list     = NULL;
    hdl->mem_used       = 0;
    hdl->max_mem        = 0;
    hdl->min_alloc      = 0;
    hdl->alloc_no       = 0;
#ifdef __WINDOWS__
    hdl->use_code_seg_num   = FALSE;
#endif
    return( hdl );
}

int _trmem_validate_all( _trmem_hdl hdl )
/****************************************/
{
    entry_ptr   walk;
    int result = 1;

    walk = hdl->alloc_list;
    while( walk ) {
        if( !isValidChunk( walk, "Validate", 0, hdl ) ) {
            result = 0;
        }
        walk = walk->next;
    }
    return result;
}

unsigned _trmem_close( _trmem_hdl hdl )
/*************************************/
{
    uint        chunks;
    uint_32     mem_used;
    entry_ptr   walk;
    entry_ptr   next;

    chunks = 0;
    if( hdl->flags & _TRMEM_CLOSE_CHECK_FREE ) {
        mem_used = hdl->mem_used;
        walk = hdl->alloc_list;
        while( walk ) {
            next = walk->next;
            ++chunks;
            _trmem_free( walk->mem, _TRMEM_NO_ROUTINE, hdl );
            walk = next;
        }
        if( chunks ) {
            trPrt( hdl, MSG_CHUNK_BYTE_UNFREED, chunks, mem_used );
        }
    } else {
        walk = hdl->alloc_list;
        while( walk ) {
            next = walk->next;
            ++chunks;
            freeEntry( walk, hdl );
            walk = next;
        }
    }
    hdl->free( hdl );
    return( chunks );
}

void _trmem_set_min_alloc( size_t size, _trmem_hdl hdl )
/******************************************************/
{
    hdl->min_alloc = size;
}

void *_trmem_alloc( size_t size, _trmem_who who, _trmem_hdl hdl )
/***************************************************************/
{
    void        *mem;
    entry_ptr   tr;

    hdl->alloc_no += 1;
    if( size == 0 && ( hdl->flags & _TRMEM_ALLOC_SIZE_0 ) ) {
        trPrt( hdl, MSG_SIZE_ZERO, "Alloc", who );
        return ( NULL );
    } else if( size < hdl->min_alloc ) {
        trPrt( hdl, MSG_MIN_ALLOC, "Alloc", who, size );
    }
    mem = hdl->alloc( size + 1 );
    if( mem != NULL ) {
        MEMSET( mem, ALLOC_BYTE, size + 1 );
        tr = allocEntry( hdl );
        if( tr != NULL ) {
            tr->mem = mem;
            tr->who = who;
            tr->when = hdl->alloc_no;
            setSize( tr, size );
            addToList( tr, hdl );
        }
        hdl->mem_used += size;
        if( hdl->mem_used > hdl->max_mem ) {
            hdl->max_mem = hdl->mem_used;
        }
    }
    return( mem );
}

static int isValidChunk( entry_ptr tr, const char *rtn,
    _trmem_who who, _trmem_hdl hdl )
{
    void *mem;
    size_t size;
    size_t blk_size;

    size = getSize( tr );
    mem = tr->mem;
    blk_size = *(size_t*)_PtrSub( mem, sizeof( size_t ) );
#ifndef __NETWARE__
#if 0
    if(( blk_size & 1 ) == 0 ) {
        trPrt( hdl, MSG_UNDERRUN_ALLOCATION, rtn, who, mem, tr->who, size );
        return( 0 );
    }
    blk_size &= ~1;
    if( size > blk_size || ( blk_size - size ) > SIZE_DELTA ) {
        trPrt( hdl, MSG_UNDERRUN_ALLOCATION, rtn, who, mem, tr->who, size );
        return( 0 );
    }
#endif
#endif
    if( *(char *)_PtrAdd( mem, size ) != ALLOC_BYTE ) {
        trPrt( hdl, MSG_OVERRUN_ALLOCATION, rtn, who, mem, tr->who, size );
        return( 0 );
    }
    return( 1 );
}

int _trmem_validate( void *mem, _trmem_who who, _trmem_hdl hdl )
/**************************************************************/
{
    entry_ptr tr;

    tr = findOnList( mem, hdl );
    if( tr == NULL ) {
        trPrt( hdl, MSG_UNOWNED_CHUNK, "Validate", who, mem );
        return( 0 );
    }
    return( isValidChunk( tr, "Validate", who, hdl ) );
}

void _trmem_free( void *mem, _trmem_who who, _trmem_hdl hdl )
/***********************************************************/
{
    entry_ptr   tr;
    size_t      size;

    if( mem == NULL ) {
        if( hdl->flags & _TRMEM_FREE_NULL ) {
            trPrt( hdl, MSG_NULL_PTR, "Free", who );
        }
        hdl->free( mem );
        return;
    }
    tr = removeFromList( mem, hdl );
    if( tr == NULL ) {
        trPrt( hdl, MSG_UNOWNED_CHUNK, "Free", who, mem );
        return;
    }
    isValidChunk( tr, "Free", who, hdl );
    size = getSize( tr );
    hdl->mem_used -= size;
    MEMSET( mem, FREED_BYTE, size + 1 );
    freeEntry( tr, hdl );
    hdl->free( mem );
}

static void * ChangeAlloc( void *old, size_t size, _trmem_who who,
                           _trmem_hdl hdl, void * (*fn)(void *,size_t),
                           char * name )
/*********************************************************************/
{
    entry_ptr   tr;
    void *      new_block;
    size_t      old_size;

    if( fn == (void *) _TRMEM_NO_ROUTINE ) {
        trPrt( hdl, MSG_NO_ROUTINE, name );
        return( NULL );
    }

    if( size == 0 ) {
        if( hdl->flags & _TRMEM_REALLOC_SIZE_0 ) {
            trPrt( hdl, MSG_SIZE_ZERO, name, who );
        }
        if( old == NULL ) {
            if( hdl->flags & _TRMEM_REALLOC_NULL ) {
                trPrt( hdl, MSG_NULL_PTR, name, who );
            }
            return( fn( NULL, 0 ) );
        }

        /* old != NULL */
        tr = removeFromList( old, hdl );
        if( tr == NULL ) {
            trPrt( hdl, MSG_UNOWNED_CHUNK, name, who, old );
            return( NULL );
        }
        isValidChunk( tr, name, who, hdl );
        size = getSize( tr );
        hdl->mem_used -= size;
        MEMSET( old, FREED_BYTE, size + 1 );
        freeEntry( tr, hdl );
        return( fn( old, 0 ) );
    }

    /* size != 0 */
    if( old == NULL ) {
        if( hdl->flags & _TRMEM_REALLOC_NULL ) {
            trPrt( hdl, MSG_NULL_PTR, name, who );
        }
        new_block = fn( NULL, size + 1 );
        if( new_block != NULL ) {
            MEMSET( new_block, ALLOC_BYTE, size + 1 );
            tr = allocEntry( hdl );
            if( tr != NULL ) {
                tr->mem = new_block;
                tr->who = who;
                setSize( tr, size );
                addToList( tr, hdl );
            }
            hdl->mem_used += size;
            if( hdl->mem_used > hdl->max_mem ) {
                hdl->max_mem = hdl->mem_used;
            }
        }
        return( new_block );
    }

    /* old != NULL && size != 0 */
    tr = removeFromList( old, hdl );
    if( tr == NULL ) {
        trPrt( hdl, MSG_UNOWNED_CHUNK, name, who, old );
        return( NULL );
    }
    if( !isValidChunk( tr, name, who, hdl ) ) {
        return( NULL );
    }
    new_block = fn( old, size + 1 );
    if( new_block == NULL ) {
        addToList( tr, hdl );   /* put back on list without change */
        return( new_block );
    }
    old_size = getSize( tr );
    if( size > old_size ) {
        MEMSET(_PtrAdd( new_block, old_size ), ALLOC_BYTE, size + 1 - old_size);
    } else {
        *(char *)_PtrAdd( new_block, size ) = ALLOC_BYTE;
    }
    hdl->mem_used -= old_size;
    hdl->mem_used += size;
    if( hdl->mem_used > hdl->max_mem ) {
        hdl->max_mem = hdl->mem_used;
    }
    tr->mem = new_block;
    tr->who = who;
    setSize( tr, size );
    addToList( tr, hdl );
    return( new_block );
}

void *_trmem_realloc( void *old, size_t size, _trmem_who who, _trmem_hdl hdl )
/****************************************************************************/
{
    return( ChangeAlloc( old, size, who, hdl, hdl->realloc, "Realloc" ) );
}

void *_trmem_expand( void *old, size_t size, _trmem_who who, _trmem_hdl hdl )
/***************************************************************************/
{
    return( ChangeAlloc( old, size, who, hdl, hdl->expand, "Expand" ) );
}

int _trmem_chk_range( void *start, size_t len,
                _trmem_who who, _trmem_hdl hdl )
/**********************************************/
{
    entry_ptr   tr;
    void        *end;
    void        *end_of_mem;

    tr = hdl->alloc_list;
    for(;;) {
        if( tr == 0 ) {
            trPrt( hdl, MSG_NOT_IN_ALLOCATION, "ChkRange", who,
                start );
            return( 0 );
        }
        end_of_mem = _PtrAdd( tr->mem, getSize( tr ) );
        if( _PtrCmp( start, >=, tr->mem ) &&
            _PtrCmp( start, < , end_of_mem ) ) break;
        tr = tr->next;
    }
    end = _PtrAdd( start, len );
    if( _PtrCmp( end, >, end_of_mem ) ) {
        trPrt( hdl, MSG_OVERRUN_2, "ChkRange", who,
            start, len, tr->mem, getSize( tr ) );
        return( 0 );
    }
    return( isValidChunk( tr, "ChkRange", who, hdl ) );
}

void _trmem_prt_usage( _trmem_hdl hdl )
/*************************************/
{
    trPrt( hdl, MSG_PRT_USAGE, hdl->mem_used, hdl->max_mem );
}

unsigned _trmem_prt_list( _trmem_hdl hdl )
/****************************************/
{
    entry_ptr   tr;
    unsigned    chunks;
    size_t      size;

    tr = hdl->alloc_list;
    if( tr == 0 ) return( 0 );
    _trmem_prt_usage( hdl );
    trPrt( hdl, MSG_PRT_LIST_1 );
    trPrt( hdl, MSG_PRT_LIST_2 );
    chunks = 0;
    do {
        size = getSize( tr );
        trPrt( hdl, MSG_PRT_LIST_3, tr->who, tr->mem, size, tr->when, tr->mem, size );
        ++chunks;
        tr = tr->next;
    } while( tr );
    return( chunks );
}

size_t _trmem_msize( void *mem, _trmem_hdl hdl ) {
/************************************************/
    return( getSize( findOnList( mem, hdl ) ) );
}

unsigned long _trmem_get_current_usage( _trmem_hdl hdl ) {
/********************************************************/
    return hdl->mem_used;
}

unsigned long _trmem_get_peak_usage( _trmem_hdl hdl ) {
/*****************************************************/
    return hdl->max_mem;
}

#ifdef __AXP__
_trmem_who  _trmem_guess_who( void )
/**********************************/
/* NYI: stubbed for now */
{
    return 0;
}

_trmem_who  _trmem_whoami( void )
/*******************************/
/* NYI: stubbed for now */
{
    return 0;
}
#endif

#ifdef __WINDOWS__
int _trmem_prt_use_seg_num( _trmem_hdl hdl, int use_seg_num )
/***********************************************************/
{
    int     old;

    old = hdl->use_code_seg_num;
    hdl->use_code_seg_num = use_seg_num;
    return( old );
}
#endif
