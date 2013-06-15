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

#ifdef TRMEM
    #define MSIZE( x )          _trmem_msize( x, trmemHandle )
#else
    #define MSIZE( x )          _msize( x )
#endif

#ifdef TRMEM
    static int                  trmemOutput;
    static _trmem_hdl           trmemHandle;
#endif

static char     *StaticBuffer = NULL;

/*
 * getMem - get and clear memory
 */
static void *getMem( unsigned size, void *who )
{
    void        *tmp;

#ifdef TRMEM
    tmp = _trmem_alloc( size, who, trmemHandle );
#else
    who = who;
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
static void *trySwap( unsigned size, int upper_bound, void *who )
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
            MemFreePtr( (void **)&BndMemory );
        }
    }

} /* tossBoundData */



/*
 * doMemAllocUnsafe - see above
 */
static void *doMemAllocUnsafe( unsigned size, void *who )
{
    void        *tmp;

    tmp = getMem( size, who );
    if( tmp == NULL ) {
        tossBoundData();
        tmp = trySwap( size, 1, who );
        if( tmp == NULL ) {
            SwapAllWindows();
            tmp = getMem( size, who );
            while( tmp == NULL ) {
                if( !TossUndos() ) {
                    return( NULL );
                }
                tmp = getMem( size, who );
            }
        }
    }
    return( tmp );
}

/*
 * MemAlloc - allocate some memory (always works, or editor aborts)
 */
void *MemAlloc( unsigned size )
{
    void        *tmp;

#ifdef TRMEM
#ifndef __WATCOMC__
    tmp = doMemAllocUnsafe( size, 1 );
#else
    tmp = doMemAllocUnsafe( size, _trmem_guess_who() );
#endif
#else
    tmp = doMemAllocUnsafe( size, 0 );
#endif
    if( tmp == NULL ) {
        AbandonHopeAllYeWhoEnterHere( ERR_NO_MEMORY );
    }
    return( tmp );

} /* MemAlloc */

/*
 * MemAllocUnsafe - allocate some memory, return null if it fails
 */
void *MemAllocUnsafe( unsigned size )
{
#ifdef TRMEM
#ifndef __WATCOMC__
    return( doMemAllocUnsafe( size, 2 ) );
#else
    return( doMemAllocUnsafe( size, _trmem_guess_who() ) );
#endif
#else
    return( doMemAllocUnsafe( size, 0 ) );
#endif

} /* MemAllocUnsafe */

/*
 * MemFree - free up memory
 */
void MemFree( void *ptr )
{
#ifdef TRMEM
#ifndef __WATCOMC__
    _trmem_free( ptr, 3, trmemHandle );
#else
    _trmem_free( ptr, _trmem_guess_who(), trmemHandle );
#endif
#else
    free( ptr );
#endif

} /* MemFree */

/*
 * MemFreePtr - free up memory
 */
void MemFreePtr( void **ptr )
{
#ifdef TRMEM
#ifndef __WATCOMC__
    _trmem_free( *ptr, 4, trmemHandle );
#else
    _trmem_free( *ptr, _trmem_guess_who(), trmemHandle );
#endif
#else
    free( *ptr );
#endif
    *ptr = NULL;

} /* MemFreePtr */


/*
 * MemFreeList - free up memory
 */
void MemFreeList( int count, char **ptr )
{
    if( ptr != NULL ) {
        int i;
        for( i = 0; i < count; i++ ) {
            MemFree( ptr[i] );
        }
        MemFree( ptr );
    }

} /* MemFreeList */


/*
 * doMemReallocUnsafe - reallocate a block, return NULL if it fails
 */
static void *doMemReAllocUnsafe( void *ptr, unsigned size, void *who )
{
    void        *tmp;

    unsigned    orig_size;
#ifdef __WATCOMC__
    unsigned    tsize;
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
    tmp = _trmem_realloc( ptr, size, who, trmemHandle );
#else
    who = who;
    tmp = realloc( ptr, size );
#endif
#ifdef __WATCOMC__
    if( tmp == NULL ) {
        tmp = doMemAllocUnsafe( size, who );
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
    } else
#endif
    {
#ifdef __WATCOMC__
        size = MSIZE( tmp );
#endif
        if( size > orig_size ) {
            memset( &(((char *)tmp)[orig_size]), 0, size - orig_size );
        }
    }
    return( tmp );

} /* doMemReAllocUnsafe */

void *MemReAllocUnsafe( void *ptr, unsigned size )
{
#ifdef TRMEM
#ifndef __WATCOMC__
    return( doMemReAllocUnsafe( ptr, size, 5 ) );
#else
    return( doMemReAllocUnsafe( ptr, size, _trmem_guess_who() ) );
#endif
#else
    return( doMemReAllocUnsafe( ptr, size, 0 ) );
#endif
}

/*
 * MemReAlloc - reallocate a block, and it will succeed.
 */
void *MemReAlloc( void *ptr, unsigned size )
{
    void        *tmp;

#ifdef TRMEM
#ifndef __WATCOMC__
    tmp = doMemReAllocUnsafe( ptr, size, 6 );
#else
    tmp = doMemReAllocUnsafe( ptr, size, _trmem_guess_who() );
#endif
#else
    tmp = doMemReAllocUnsafe( ptr, size, 0 );
#endif
    if( tmp == NULL ) {
        AbandonHopeAllYeWhoEnterHere( ERR_NO_MEMORY );
    }
    return( tmp );

} /* MemReAlloc */

static char *staticBuffs[MAX_STATIC_BUFFERS];
static bool staticUse[MAX_STATIC_BUFFERS];
int         maxStatic = 0;

/*
 * StaticAlloc - allocate one of the static buffers
 */
void *StaticAlloc( void )
{
    int i;

    for( i = 0; i < MAX_STATIC_BUFFERS; i++ ) {
        if( !staticUse[i] ) {
            staticUse[i] = TRUE;
            {
                int j, k = 0;
                for( j = 0; j < MAX_STATIC_BUFFERS; j++ ) {
                    if( staticUse[j] ) {
                        k++;
                    }
                }
                if( k > maxStatic ) {
                    maxStatic = k;
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
            staticUse[i] = FALSE;
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
    bs = EditVars.MaxLine + 2;
    StaticBuffer = MemAlloc( MAX_STATIC_BUFFERS * bs );
    for( i = 0; i < MAX_STATIC_BUFFERS; i++ ) {
        staticUse[i] = FALSE;
        staticBuffs[i] = &StaticBuffer[i * bs];
    }

} /* StaticStart */

void StaticFini( void )
{
    MemFree( StaticBuffer );
}

/*
 * MemStrDup - Safe strdup()
 */
char *MemStrDup( char *string )
{
    char *rptr;

    if( string == NULL ){
        rptr = NULL;
    } else {
        rptr = (char *)MemAlloc( strlen( string ) + 1 );
        strcpy( rptr, string );
    }
    return( rptr );
}


#ifdef TRMEM

extern void trmemPrint( void *handle, const char *buff, size_t len )
/******************************************************************/
{
    write( *(int *)handle, buff, len );
}

#endif

void FiniMem( void )
{
#ifdef TRMEM
    _trmem_prt_list( trmemHandle );
    _trmem_close( trmemHandle );
    if( trmemOutput != -1 ) {
        close( trmemOutput );
    }
#endif
}

void InitMem( void )
{
#ifdef TRMEM
    trmemOutput = open( "trmem.out", O_RDWR | O_CREAT | O_TEXT, PMODE_RW );
    trmemHandle = _trmem_open( malloc, free, realloc, NULL,
        &trmemOutput, trmemPrint,
        _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 |
        _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
    // atexit( DumpTRMEM );
#endif
}
