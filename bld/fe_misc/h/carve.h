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
* Description:  Carver public interface.
*
****************************************************************************/


#ifndef _CARVE_H
#define _CARVE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

/*
  The carver module will allocate larger chunks of memory and handle reuse
  of equally sized chunks.  A module "creates" a handler passes the handle
  to CarveAlloc and CarveFree to malloc and free the elements.  At the end
  of execution, CarveDestroy can be used to free up the memory for the
  chunks.
*/

typedef struct blk blk_t;
typedef struct free_t free_t;

typedef struct {
    free_t      *free_list;
    blk_t       *blk_list;
    blk_t       **blk_map;
    unsigned    elm_size;
    unsigned    elm_count;
    unsigned    blk_top;
    unsigned    blk_count;
#ifndef NDEBUG
    free_t      *zapped_free_list;
#endif
} cv_t, *carve_t;

void * StackCarveAlloc(         // CARVER ALLOC AND PUSH STACK
    carve_t carver,             // - carving control
    void *stack_hdr )           // - addr[ stack hdr ]
;

extern carve_t CarveCreate( unsigned elm_size, unsigned how_many );
extern void CarveDestroy( carve_t cv );

extern void *CarveAlloc( carve_t cv );
extern void CarveFree( carve_t cv, void *elm );

#ifndef NDEBUG
    extern void CarveVerifyFreeElement( carve_t cv, void *elm );
    extern void CarveVerifyAllGone( carve_t cv, char const * );
#else
#   define CarveVerifyFreeElement( cv, elm );
#   define CarveVerifyAllGone( cv, msg );
#endif

#ifdef CARVEPCH

#if defined( LONG_IS_64BIT ) || defined( _WIN64 )
#define CV_SHIFT		17
#else
#define CV_SHIFT		16
#endif

#define MK_INDEX( b, o )        (((b)<<CV_SHIFT)|((o)&((1<<CV_SHIFT)-1)))
#define GET_BLOCK( i )          (((i)>>CV_SHIFT))
#define GET_OFFSET( i )         ((i)&CV_MASK)

typedef unsigned_32 cv_index; // type to use for carve indices

// block 0 is reserved for special indices

enum {
    CARVE_NULL_INDEX    = MK_INDEX( 0, 0 ),
    CARVE_ERROR_INDEX   = MK_INDEX( 0, 1 ),
};

extern void *CarveGetIndex( carve_t cv, void * );
extern void *CarveMapIndex( carve_t cv, void * );

typedef struct {
    unsigned    bindex;
    blk_t       *block;
    free_t      **head;
    carve_t     cv;
} cvinit_t;

extern carve_t CarveRestart( carve_t cv );
extern void CarveInitStart( carve_t cv, cvinit_t * );
extern void *CarveInitElement( cvinit_t *, cv_index );

extern void CarveMapOptimize( carve_t, cv_index );
extern void CarveMapUnoptimize( carve_t );
extern cv_index CarveLastValidIndex( carve_t );

extern void CarveWalkAllFree( carve_t gcv, void (*rtn)( void * ) );

typedef struct {
    cv_index    index;
    void        *extra;
} carve_walk_base;

extern void CarveWalkAll( carve_t gcv, void (*rtn)( void *, carve_walk_base * ), carve_walk_base *data );

#endif // CARVEPCH

#ifdef __cplusplus
};
#endif

#endif
