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
* Description:  Dwarf segment information data structures and functions.
*
****************************************************************************/


#define    PTRDIFF( a, b )   ( (char * )(a) -  (char *)(b) )

enum {
    SEG_PER_BLK = 256,
};

enum {
    OFF_PER_BLK = 256,
};

typedef struct{
    addr_seg    real;
    unsigned_16 count;    /*#entries collected by head */
}seg_entry;

struct seg_blk_head{
    struct seg_blk_head *next;  /*next block of seg_info */
    seg_entry           *info;  /*ptr to array of seg_info of .item_size size */
};

typedef
    seg_blk_head    *(*seg_alloc)( void );
typedef
    void            (*seg_init)( void *new );
typedef struct {
    seg_alloc   alloc;
    seg_init    init;
}seg_ctl;

typedef
    int         (*SEGWLK)( void *, void * );
extern int  SegWalk( seg_list *ctl, SEGWLK wlk, void * d );
extern seg_entry *AddMapSeg( seg_list *list, seg_ctl *ctl, word seg );
extern seg_entry *FindMapSeg( seg_list *list, word seg );
extern seg_entry *FindRealSeg( seg_list *ctl, word seg );
extern void AddSegReal( seg_list  *ctl, void *d );
extern void SortSegReal( seg_list *ctl );
extern void InitSegList( seg_list *ctl, unsigned_16 item_size );
extern void FiniSegList( seg_list *ctl );
