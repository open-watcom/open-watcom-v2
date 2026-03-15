/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Internal editor memory management routines.
*
****************************************************************************/


#include "vi.h"
#include "fcbmem.h"
#include "win.h"
#ifdef __WATCOMC__
    #include <malloc.h>
#endif
#ifdef TRMEM
    #include "wio.h"
    #include "trmem.h"
#endif
#include "memdmp.h"


#if defined( TRMEM ) && defined( _M_IX86 ) && ( __WATCOMC__ > 1290 )
#define _XSTR(s)    # s
#define TRMEMAPI(x) _Pragma(_XSTR(aux x __frame))
#else
#define TRMEMAPI(x)
#endif

#ifdef TRMEM
    #define MSIZE( x )          _trmem_msize( x, TrHdl )
    #define WHO_PTR             _trmem_who
#else
    #define MSIZE( x )          _msize( x )
    #define WHO_PTR             void *
#endif

static char                 *StaticBuffer = NULL;

#ifdef TRMEM

static FILE                 *TrFile = NULL;
static _trmem_hdl           TrHdl = _TRMEM_HDL_NONE;

static void trmemPrintLine( void *handle, const char *buff, size_t len )
/**********************************************************************/
{
    /* unused parameters */ (void)handle; (void)len;

    if( TrFile != NULL ) {
        fprintf( TrFile, "%s\n", buff );
    }
}

#endif  /* TRMEM */

void FiniMem( void )
{
#ifdef TRMEM
    _trmem_prt_list( TrHdl );
    _trmem_close( TrHdl );
    if( TrFile != NULL ) {
        fclose( TrFile );
        TrFile = NULL;
    }
#endif
}

void InitMem( void )
{
#ifdef TRMEM
    TrFile = fopen( "trmem.out", "w" );
    TrHdl = _trmem_open( malloc, free, realloc, strdup,
        NULL, trmemPrintLine, _TRMEM_DEF );
    // atexit( DumpTRMEM );
#endif
}

/*
 * getMem - get and clear memory
 */
static void *getMem( size_t size, WHO_PTR who )
{
    void        *tmp;

#ifdef TRMEM
    tmp = _trmem_alloc( size, who, TrHdl );
#else
    /* unused parameters */ (void)who;

    tmp = malloc( size );
#endif
    if( tmp != NULL ) {
#ifdef __WATCOMC__
        size = MSIZE( tmp );
#endif
        memset( tmp, 0, size );
    }
    return( tmp );

} /* getMem */

/*
 * getLRU - get lru block from fcb list
 */
static fcb *getLRU( int upper_bound )
{
    long lru = MAX_LONG;
    fcb *cfcb, *tfcb = NULL;
    int bootlimit = MAX_IO_BUFFER / 2;

    for( ;; ) {

        for( cfcb = FcbThreadHead; cfcb != NULL; cfcb = cfcb->thread_next ) {
            if( !cfcb->on_display && !cfcb->non_swappable && cfcb->in_memory
                && cfcb->byte_cnt >= bootlimit && cfcb != CurrentFcb ) {
                if( cfcb->last_swap == 0 ) {
                    return( cfcb );
                }
                if( cfcb->last_swap < lru ) {
                    lru = cfcb->last_swap;
                    tfcb = cfcb;
                }
            }
        }
        if( tfcb == NULL && bootlimit > 64 ) {
            bootlimit /= 2;
            if( bootlimit < upper_bound ) {
                return( NULL );
            }
            continue;
        }
        return( tfcb );
    }

} /* getLRU */

/*
 * trySwap - try to swap an fcb
 */
static void *trySwap( size_t size, int upper_bound, WHO_PTR who )
{
    void        *tmp = NULL;
    fcb         *tfcb;

    for( ;; ) {

        /*
         * find LRU fcb
         */
        tfcb = getLRU( upper_bound );
        if( tfcb == NULL ) {
            break;
        }

        /*
         * swap the LRU fcb, and re-attempt allocation
         */
        SwapFcb( tfcb );
        tmp = getMem( size, who );
        if( tmp != NULL ) {
            break;
        }

    }

    return( tmp );

} /* trySwap */

/*
 * tossBoundData - get rid of bound data
 */
static void tossBoundData( void )
{
    if( BoundData ) {
        if( !EditFlags.BndMemoryLocked ) {
            MemFree( BndMemory );
            BndMemory = NULL;
        }
    }

} /* tossBoundData */



/*
 * doAlloc - see above
 */
static void *doAlloc( size_t size, WHO_PTR who )
{
    void        *tmp;

    tmp = getMem( size, who );
    if( tmp == NULL ) {
        tossBoundData();
        tmp = trySwap( size, 1, who );
        if( tmp == NULL ) {
            SwapAllWindows();
            while( (tmp = getMem( size, who )) == NULL ) {
                if( !TossUndos() ) {
                    return( NULL );
                }
            }
        }
    }
    return( tmp );
}

/*
 * MemAllocSafe - allocate some memory (always works, or editor aborts)
 */
TRMEMAPI( MemAllocSafe )
void *MemAllocSafe( size_t size )
{
    void        *tmp;

#ifdef TRMEM
    tmp = doAlloc( size, _TRMEM_WHO( 1 ) );
#else
    tmp = doAlloc( size, NULL );
#endif
    if( tmp == NULL ) {
        AbandonHopeAllYesWhoEnterHere( ERR_NO_MEMORY );
    }
    return( tmp );

} /* MemAllocSafe */

/*
 * MemAlloc - allocate some memory, return null if it fails
 */
TRMEMAPI( MemAlloc )
void *MemAlloc( size_t size )
{
#ifdef TRMEM
    return( doAlloc( size, _TRMEM_WHO( 2 ) ) );
#else
    return( doAlloc( size, NULL ) );
#endif

} /* MemAlloc */

/*
 * MemStrdup - allocate memory and duplicate string
 */
TRMEMAPI( MemStrdup )
char *MemStrdup( const char *str )
{
    char        *ptr;

#ifdef TRMEM
    ptr = _trmem_strdup( str, _TRMEM_WHO( 3 ), TrHdl );
#else
    ptr = strdup( str );
#endif
    if( ptr == NULL ) {
        AbandonHopeAllYesWhoEnterHere( ERR_NO_MEMORY );
    }
    return( ptr );

} /* MemStrdup */

/*
 * MemStrdupSafe - allocate memory and duplicate string
 */
TRMEMAPI( MemStrdupSafe )
char *MemStrdupSafe( const char *str )
{
    char        *ptr;

#ifdef TRMEM
    ptr = _trmem_strdup( str, _TRMEM_WHO( 3 ), TrHdl );
#else
    ptr = strdup( str );
#endif
    if( ptr == NULL ) {
        AbandonHopeAllYesWhoEnterHere( ERR_NO_MEMORY );
    }
    return( ptr );

} /* MemStrdupSafe */

/*
 * MemFree - free up memory
 */
TRMEMAPI( MemFree )
void MemFree( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _TRMEM_WHO( 4 ), TrHdl );
#else
    free( ptr );
#endif

} /* MemFree */

/*
 * MemFreePtrArray - free up memory
 */
void MemFreePtrArray( void **ptr, size_t count, void(*free_fn)(void *) )
{
    if( ptr != NULL ) {
        if( free_fn != NULL ) {
            while( count-- > 0 ) {
                free_fn( ptr[count] );
            }
        }
        MemFree( ptr );
    }

} /* MemFreePtrArray */


/*
 * doRealloc - reallocate a block, return NULL if it fails
 */
static void *doRealloc( void *ptr, size_t size, WHO_PTR who )
{
    void        *tmp;

    size_t      orig_size;
#ifdef __WATCOMC__
    size_t      tsize;
#endif

    if( ptr != NULL ) {
#ifdef __WATCOMC__
        orig_size = MSIZE( ptr );
#else
        orig_size = 0xffffffff;
#endif
    } else {
        orig_size = 0;
    }

#ifdef TRMEM
    tmp = _trmem_realloc( ptr, size, who, TrHdl );
#else
    /* unused parameters */ (void)who;

    tmp = realloc( ptr, size );
#endif
#ifdef __WATCOMC__
    if( tmp == NULL ) {
        tmp = doAlloc( size, who );
        if( tmp == NULL ) {
            return( NULL );
        }
        size = MSIZE( tmp );
        if( orig_size != 0 ) {
            tsize = orig_size;
            if( tsize > size ) {
                tsize = size;
            }
            memcpy( tmp, ptr, tsize );
            MemFree( ptr );
        }
    } else {
        size = MSIZE( tmp );
#else
    {
#endif
        if( size > orig_size ) {
            memset( (char *)tmp + orig_size, 0, size - orig_size );
        }
    }
    return( tmp );

} /* doRealloc */

TRMEMAPI( MemRealloc )
void *MemRealloc( void *ptr, size_t size )
{
#ifdef TRMEM
    return( doRealloc( ptr, size, _TRMEM_WHO( 5 ) ) );
#else
    return( doRealloc( ptr, size, NULL ) );
#endif
}

/*
 * MemReallocSafe - reallocate a block, and it will succeed.
 */
TRMEMAPI( MemReallocSafe )
void *MemReallocSafe( void *ptr, size_t size )
{
    void        *tmp;

#ifdef TRMEM
    tmp = doRealloc( ptr, size, _TRMEM_WHO( 6 ) );
#else
    tmp = doRealloc( ptr, size, NULL );
#endif
    if( tmp == NULL ) {
        AbandonHopeAllYesWhoEnterHere( ERR_NO_MEMORY );
    }
    return( tmp );

} /* MemReallocSafe */

static char *staticBuffs[MAX_STATIC_BUFFERS];
static bool staticUse[MAX_STATIC_BUFFERS];

/*
 * StaticAlloc - allocate one of the static buffers
 */
void *StaticAlloc( void )
{
    int i;

    for( i = 0; i < MAX_STATIC_BUFFERS; i++ ) {
        if( !staticUse[i] ) {
            staticUse[i] = true;
            {
                int j, k = 0;
                for( j = 0; j < MAX_STATIC_BUFFERS; j++ ) {
                    if( staticUse[j] ) {
                        k++;
                    }
                }
                if( k > MaxStatic ) {
                    MaxStatic = k;
                }
            }
            return( staticBuffs[i] );
        }
    }
    return( NULL );

} /* StaticAlloc */

/*
 * StaticFree - free a static buffer
 */
void StaticFree( char *item )
{
    int i;

    for( i = 0; i < MAX_STATIC_BUFFERS; i++ ) {
        if( item == staticBuffs[i] ) {
            staticUse[i] = false;
            return;
        }
    }

} /* StaticFree */

/*
 * StaticStart - start up static buffer
 */
void StaticStart( void )
{
    int i, bs;

    MemFree( StaticBuffer );
    bs = EditVars.MaxLineLen + 2;
    StaticBuffer = _MemAllocArraySafe( char, MAX_STATIC_BUFFERS * bs );
    for( i = 0; i < MAX_STATIC_BUFFERS; i++ ) {
        staticUse[i] = false;
        staticBuffs[i] = &StaticBuffer[i * bs];
    }

} /* StaticStart */

void StaticFini( void )
{
    MemFree( StaticBuffer );
}

