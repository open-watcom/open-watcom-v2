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


#ifndef _CARVE_H
#define _CARVE_H

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
    free_t *    free_list;
    free_t *    insert;
    blk_t *     blk_list;
    blk_t **    blk_map;
    size_t      elm_size;
    size_t      elm_count;
    size_t      blk_top;
    size_t      blk_count;
    size_t      blk_size;
    unsigned    size_chg : 1;
} cv_t, *carve_t;

extern carve_t CarveCreate( size_t elm_size, size_t how_many );
extern void CarveRestart( carve_t cv, unsigned );
extern void CarvePurge( carve_t cv );
extern void CarveVerifyAllGone( carve_t cv, char * );
extern void CarveDestroy( carve_t cv );

extern void *CarveAlloc( carve_t cv );
extern void *CarveZeroAlloc( carve_t cv );
extern void CarveFree( carve_t cv, void *elm );

#define MK_INDEX( b, o )        (((b)<<16)|(o))
#define GET_BLOCK( i )          (((i)>>16)&0x0ffff)
#define GET_OFFSET( i )         ((i)&0x0ffff)

// block 0 is reserved for special indices

enum {
    CARVE_NULL_INDEX    = MK_INDEX( 0, 0 ),
    CARVE_INVALID_INDEX = MK_INDEX( 0, 1 ),
};

extern void *   CarveGetIndex( carve_t, void * );
extern void     CarveWalkBlocks( carve_t, void (*)(carve_t, void *, void *),
                                 void * );
extern bool     CarveBlockModified( void * );
extern void     CarveBlockScan( carve_t, void *, void (*)(void *, void *),
                                void *);
extern unsigned CarveBlockSize( carve_t );
extern unsigned CarveNumElements( carve_t );
extern void *   CarveBlockData( void * );
extern void *   CarveMapIndex( carve_t, void * );
extern bool     CarveSizeChanged( carve_t );
extern void     CarveInsertFree( carve_t, void * );

typedef struct {
    unsigned    bindex;
    blk_t *     block;
    free_t **   head;
    carve_t     cv;
} cvinit_t;

extern void CarveWalkAllFree( carve_t gcv, void (*rtn)( void * ) );
extern void CarveWalkAll( carve_t, void (*)(void *, void *), void * );

#endif
