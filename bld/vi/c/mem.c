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
    #include <fcntl.h>
    #include <unistd.h>
    #include "trmem.h"

    static int                  trmemOutput;
    static _trmem_hdl           trmemHandle;
    #define MSIZE( x )          _trmem_msize( x, trmemHandle )
#else
    typedef void *_trmem_who;
    #define _trmem_guess_who()  0
    #define MSIZE( x )          _msize( x )
#endif

/*
 * getMem - get and clear memory
 */
static void *getMem( unsigned size, _trmem_who who )
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
static fcb *getLRU( unsigned upper_bound )
{
    long lru = MAX_LONG;
    fcb *cfcb, *tfcb = NULL;
    int bootlimit = MAX_IO_BUFFER / 2;

    while( 1 ) {

        cfcb = FcbThreadHead;
        while( cfcb != NULL ) {
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
            cfcb = cfcb->thread_next;
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
static void *trySwap( unsigned size, unsigned upper_bound, _trmem_who who )
{
    void        *tmp = NULL;
    fcb         *tfcb;

    while( 1 ) {

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
    if( EditFlags.BoundData ) {
        if( !EditFlags.BndMemoryLocked ) {
            MemFree2( &BndMemory );
        }
    }

} /* tossBoundData */



/*
 * doMemAllocUnsafe - see above
 */
void *doMemAllocUnsafe( unsigned size, _trmem_who who )
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

    tmp = doMemAllocUnsafe( size, _trmem_guess_who() );
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
    return( doMemAllocUnsafe( size, _trmem_guess_who() ) );

} /* MemAllocUnsafe */

/*
 * MemFree - free up memory
 */
void MemFree( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), trmemHandle );
#else
    free( ptr );
#endif

} /* MemFree */

/*
 * doMemReallocUnsafe - reallocate a block, return NULL if it fails
 */
void *doMemReAllocUnsafe( void *ptr, unsigned size, _trmem_who who )
{
    void        *tmp;

    unsigned    orig_size;
    unsigned    tsize;

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
    return( doMemReAllocUnsafe( ptr, size, _trmem_guess_who() ) );
}

/*
 * MemReAlloc - reallocate a block, and it will succeed.
 */
void *MemReAlloc( void *ptr, unsigned size )
{
    void        *tmp;

    tmp = doMemReAllocUnsafe( ptr, size, _trmem_guess_who() );
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
    bs = MaxLine + 2;
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

extern void trmemPrint( int * handle, const char * buff, size_t len )
/*******************************************************************/
{
    write( *handle, buff, len );
}

void DumpTRMEM( void )
{
    _trmem_prt_list( trmemHandle );
    _trmem_close( trmemHandle );
    if( trmemOutput != -1 ) {
        close( trmemOutput );
    }
}

void InitTRMEM( void )
{
    char        file[FILENAME_MAX];

    strcpy( file, getenv( "EDPATH" ) );
    strcat( file, "\\trmem.out" );
    trmemOutput = open( file, O_RDWR | O_CREAT | O_TEXT );

    trmemHandle = _trmem_open( malloc, free, realloc, _expand,
        &trmemOutput, trmemPrint,
        _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 |
        _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
    // atexit( DumpTRMEM );
}
#endif
