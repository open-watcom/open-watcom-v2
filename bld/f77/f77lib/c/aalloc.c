/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  allocation/deallocation of allocatable arrays
*
****************************************************************************/


#include "ftnstd.h"
#include <limits.h>
#if defined( _M_IX86 )
  #include <i86.h>
  #if defined( _M_I86 )
    #include <malloc.h>
  #endif
  #if defined( __WINDOWS_386__ )
    #include <win386.h>
  #endif
#endif
#include "rterr.h"
#include "rtenv.h"
#include "errcod.h"
#include "falloc.h"


#define STAT_OK                 0
#define STAT_NO_MEM             1
#define STAT_ALREADY_ALLOC      2
#define STAT_NOT_ALLOCATED      3

#define FAR2NEAR(f)  ((void *)(pointer_uint)(f))

static unsigned_16 *getFlagsPtr( void **arr, unsigned_16 flags )
//==============================================================
{
    if( flags & ALLOC_STRING )
        return( (unsigned_16 *)( (string *)arr + 1 ) );
#if defined( _M_IX86 )
    if( flags & ALLOC_EXTENDED )
        return( (unsigned_16 *)((void __far **)arr + 1 ) );
#endif
    return( (unsigned_16 *)( arr + 1 ) );
}

static void turnOffFlags( void **arr, unsigned_16 flags, unsigned_16 mask )
//=========================================================================
{
    *getFlagsPtr( arr, flags ) &= ~mask;
}


static void turnOnFlags( void **arr, unsigned_16 flags, unsigned_16 mask )
//========================================================================
{
    *getFlagsPtr( arr, flags ) |= mask;
}


static  bool    Allocated( void **arr, unsigned_16 flags )
//========================================================
// Determine allocation status of array.
{
#if defined( _M_IX86 )
    if( flags & ALLOC_EXTENDED ) {
  #if defined( _M_I86 )
        return( *(void __far **)arr != NULL );
  #else
        return( FAR2NEAR( *(void __far **)arr ) != NULL );
  #endif
    } else {
#endif
        return( *arr != NULL );
#if defined( _M_IX86 )
    }
#endif
}


void    Alloc( unsigned_16 alloc_type, uint num, ... ) {
//======================================================

    void PGM * PGM *    item;
    struct string PGM * scb;
    adv_entry PGM *     adv_ent;
    int                 dim_cnt;
    unsigned_32         size;
    va_list             args;
    intstar4 PGM *      stat;
    intstar4            location;
    unsigned_32         elt_size;
    unsigned_16         alloc_flags;
#if defined( __WINDOWS_386__ )
    unsigned_16         seg;
#endif

    // Assumptions
    // 1. The first field in the scb is the pointer to the string
    // 2. The flags for an scb are right after the scb
    // 3. The flags for an array are right before the ADV

    va_start( args, num );
    if( alloc_type & (ALLOC_STAT | ALLOC_NONE) ) {
        stat = va_arg( args, intstar4 PGM * );
    } else {
        stat = NULL;
    }
    if( alloc_type & ALLOC_LOC ) {
        location = va_arg( args, intstar4 );
    } else {
        alloc_type |= ALLOC_MEM;
    }
    if( stat != NULL ) {
        *stat = STAT_OK;
    }
    while( num-- > 0 ) {
        alloc_flags = va_arg( args, unsigned_16 );
        item = va_arg( args, void PGM * PGM * );
        if( alloc_flags & ALLOC_STRING ) {
            scb = (string *)item;
            dim_cnt = 1;
            elt_size = 1;
        } else {
            adv_ent = va_arg( args, adv_entry * );
            dim_cnt = va_arg( args, int );
            elt_size = va_arg( args, unsigned_32 );
        }
        if( (alloc_flags & ALLOC_LOC) == 0 ) {
            if( Allocated( item, alloc_flags ) ) {
                if( stat != NULL ) {
                    *stat = STAT_ALREADY_ALLOC;
                    break;
                }
                RTErr( MO_STORAGE_ALLOCATED );
            }
        }
        if( alloc_type & ALLOC_LOC ) {
#if defined( _M_IX86 )
            if( alloc_flags & ALLOC_EXTENDED ) {
    #if defined( __WINDOWS_386__ )
                seg = location >> 16;
                if( ( seg == 0 ) || ( seg == 0xffff ) ) {
                    *(void __far **)item = MK_LOCAL32( (void *)location );
                } else {
                    *(void __far **)item = MK_FP32( (void *)location );
                }
    #else
                *(void __far **)item = _MK_FP( location >> 16, location & 0x0000ffff );
    #endif
            } else {
#endif
                *item = (void *)location;
#if defined( _M_IX86 )
            }
#endif
        } else {
            if( alloc_flags & ALLOC_STRING ) {
                size = scb->len;
                dim_cnt = 0;
            } else {
                size = 1;
                while( dim_cnt-- > 0 ) {
                    size *= adv_ent->num_elts;
                    ++adv_ent;
                }
            }
#if defined( _M_I86 ) && !defined( __MEDIUM__ )
            *item = (void PGM *)halloc( size, elt_size );
#else
    #if defined( _M_IX86 )
        #if defined( _M_I86 )
            if( size * elt_size > UINT_MAX ) {
                if( alloc_flags & ALLOC_EXTENDED ) {
                    *(void __far **)item = NULL;
                } else {
                    *item = NULL;
                }
            } else {
        #endif
                if( alloc_flags & ALLOC_EXTENDED ) {
        #if defined( _M_I86 )
                    *(void __far **)item = _fmalloc( size * elt_size );
        #else
                    *(void __far **)item = malloc( size * elt_size );
        #endif
                } else {
    #endif
                    *item = malloc( size * elt_size );
    #if defined( _M_IX86 )
                }
        #if defined( _M_I86 )
            }
        #endif
    #endif
#endif
            if( !Allocated( item, alloc_flags ) ) {
                if( stat != NULL ) {
                    *stat = STAT_NO_MEM;
                    break;
                }
                RTErr( MO_DYNAMIC_OUT );
            }
        }
        turnOffFlags( item, alloc_flags, ALLOC_MASK );
        turnOnFlags( item, alloc_flags, alloc_type );
    }
    va_end( args );
}


void    DeAlloc( intstar4 PGM *stat, uint num, ... ) {
//====================================================

    void PGM * PGM      *item;
    va_list             args;
    uint                istat;
    unsigned_16         alloc_flags;

    istat = STAT_OK;
    va_start( args, num );
    while( num-- > 0 ) {
        alloc_flags = va_arg( args, unsigned_16 );
        item = va_arg( args, void PGM * PGM * );
        if( !Allocated( item, alloc_flags ) ) {
            istat = STAT_NOT_ALLOCATED;
        } else {
#if defined( _M_IX86 )
            if( (alloc_flags & ALLOC_LOC) == 0 ) {
#endif
#if defined( _M_I86 ) && !defined( __MEDIUM__ )
                hfree( *item );
#else
    #if defined( _M_IX86 )
                if( alloc_flags & ALLOC_EXTENDED ) {
        #if defined( _M_I86 )
                    _ffree( *(void __far **)item );
        #else
                    free( FAR2NEAR( *(void __far **)item ) );
        #endif
                } else {
    #endif
                    free( *item );
    #if defined( _M_IX86 )
                }
    #endif
#endif
#if defined( _M_IX86 )
            }
#endif
#if defined( _M_IX86 )
            if( alloc_flags & ALLOC_EXTENDED ) {
    #if defined( _M_I86 ) && !defined( __MEDIUM__ )
                *item = NULL;
    #else
                (*(void __far **)item) = NULL;
    #endif
            } else {
#endif
                *item = NULL;
#if defined( _M_IX86 )
            }
#endif
            turnOffFlags( item, alloc_flags, ALLOC_MASK );
        }
    }
    va_end( args );
    if( ( istat != STAT_OK ) && ( stat == NULL ) ) {
        RTErr( MO_STORAGE_NOT_ALLOCATED );
    }
    if( stat != NULL ) {
        *stat = istat;
    }
}
