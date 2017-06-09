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
* Description:  Virtual pointer stacking.
*
****************************************************************************/


#ifndef __PSTK_H__
#define __PSTK_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pstk_ctl     PSTK_CTL;       // control element for PSTK
typedef struct pstk_blk     PSTK_BLK;       // block of items
typedef struct pstk_iter    PSTK_ITER;      // iterator

typedef void *elem_type;

struct pstk_ctl {               // PSTK_CTL -- controller for a pointer stack
    elem_type *top_item;        // - top occupied slot
    PSTK_BLK *top_blk;          // - top of blocks
    PSTK_BLK *popped_blk;       // - last popped block
};

struct pstk_iter {              // PSTK_ITER -- iterator for PSTK
    PSTK_CTL const *pstk;       // - pstack being iterated
    PSTK_BLK const *block;      // - current block
    const elem_type *item;      // - last item returned
    const elem_type *top;       // - addr[ top element in stack ]
    const elem_type *bot;       // - addr[ bottom element in stack ]
};

extern void         PstkPush( PSTK_CTL *, elem_type );
extern elem_type    *PstkPop( PSTK_CTL * );
extern elem_type    PstkPopElement( PSTK_CTL * );
extern void         PstkOpen( PSTK_CTL * );
extern void         PstkClose( PSTK_CTL * );
extern void         PstkPopAll( PSTK_CTL * );
extern elem_type    PstkTopElement( PSTK_CTL * );
extern int          PstkContainsElement( PSTK_CTL *, elem_type );

extern int          PstkIterDnOpen( PSTK_ITER*, PSTK_CTL const * );
extern elem_type    PstkIterDnNext( PSTK_ITER* );

extern int          PstkIterUpOpen( PSTK_ITER*, PSTK_CTL const * );
extern elem_type    PstkIterUpNext( PSTK_ITER* );

#define PstkTop( addr_ctl )     ((addr_ctl)->top_item)

#ifdef __cplusplus
};
#endif

#endif
