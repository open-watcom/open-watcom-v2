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


#ifndef __VSTK_H__
#define __VSTK_H__

// VSTK.H -- virtual stacking
//
// 92/03/02 -- J.W.Welch        -- defined

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PAD_UNSIGNED
#   define PAD_UNSIGNED unsigned :0;
#endif
                                        // STRUCTURES:
typedef struct vstk_ctl     VSTK_CTL;   // - control element for VSTK
typedef struct vstk_blk     VSTK_BLK;   // - block of items


struct vstk_ctl                 // VSTK_CTL -- controller for a virtual stack
{   void *current;              // - current item
    VSTK_BLK *top;              // - top of blocks
    VSTK_BLK *freed;            // - freed list of blocks
    uint_16 per_block;          // - number of items per block
    uint_16 size;               // - size of one element
    PAD_UNSIGNED
};

struct vstk_blk {               // VSTK_BLK -- block of stack items
    VSTK_BLK *last;             // - last stacked block
    char data[1];               // - stacked elements
};


// PROTOTYPES:

void *VstkBase(                 // GET BASE ELEMENT
    VSTK_CTL *stack,            // - stack to be based
    int base )                  // - the base index
;
void VstkClose(                 // CLOSE THE VIRTUAL STACK
    VSTK_CTL *stack )           // - stack to be closed
;
int VstkDimension(              // GET DIMENSION OF VIRTUAL STACK
    VSTK_CTL const *stack )     // - stack
;
void *VstkIndex(                // INDEX INTO A VIRTUAL STACK
    VSTK_CTL *stack,            // - stack to be indexed
    int index )                 // - the index
;
void *VstkNext(                 // GET NEXT ITEM IN STACK
    VSTK_CTL const *stack,      // - stack being walked
    void *cur )                 // - current item
;
void VstkOpen(                  // OPEN THE VIRTUAL STACK
    VSTK_CTL *stack,            // - stack to be opened
    size_t size,                // - size of an element
    size_t count )              // - number per block
;
void *VstkPop(                  // POP THE STACK
    VSTK_CTL *stack )           // - stack to be used
;
void *VstkPush(                 // PUSH THE STACK
    VSTK_CTL *stack )           // - stack to be pushed
;
void *VstkTop(                  // POINT AT TOP OF STACK
    VSTK_CTL const *stack )     // - stack to be used
;
void VstkTruncate(              // TRUNCATE A VSTK
    VSTK_CTL *stack,            // - stack to be truncated
    int base )                  // - the truncation index
;

// MACROS

#define VstkIterBeg( stk, it ) \
    for( it=VstkTop(stk); it!=NULL; it=VstkNext(stk,it) )

#define VstkIterEnd( it )

#ifdef __cplusplus
};
#endif

#endif
