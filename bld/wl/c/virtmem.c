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
* Description:  Virtual memory support for linker.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include "walloca.h"
#include "linkstd.h"
#include "newmem.h"
#include "msg.h"
#include "alloc.h"
#include "wlnkmsg.h"
#include "ostype.h"
#include "spillio.h"
#include "loadfile.h"
#include "fileio.h"
#include "virtmem.h"

/* flags used in the virtual memory structure */
typedef enum {
    VIRT_INMEM = 0x01,      // virtual memory block is in RAM
    VIRT_HUGE  = 0x02       // virtual memory block is a huge block
} virt_flags;

/* this is for allocating very large memory requests (i.e. > 1 megabyte).
   virtual memory locations 0x80000000 and above are split into 1 megabyte
   pages.

   offset into subpage         subpage #  page #       big-page indicator
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   15 bits                   | 5 bits  | 11 bits             |1|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
low-order                           high-order

each page pointer in the huge_table structure points to an array of 32
spill addresses, each of which points to a 32K-byte subpage of virtual memory.
*/

typedef struct huge_table {
    struct huge_table * next;       // next entry to swap.
    virt_flags          flags;
    unsigned_8          numthere;
    unsigned_8          numswapped;
    unsigned_16         sizelast;
    spilladdr *         page;
} huge_table;

#define HUGE_OFFSET_SHIFT   20
#define MAX_BIGNODE_SIZE    (1UL << HUGE_OFFSET_SHIFT)
#define HUGE_OFFSET_MASK    (MAX_BIGNODE_SIZE - 1)
#define HUGE_BIT_MASK       0x7FFFFFFFUL
#define HUGE_PAGE           0x80000000UL
#define HUGE_INITIAL_ALLOC  2
#define HUGE_LIMIT          2048       /* max number of huge pages */
#define HUGE_SUBPAGE_SHIFT  15
#define HUGE_SUBPAGE_SIZE   (1 << HUGE_SUBPAGE_SHIFT)
#define HUGE_SUBPAGE_MASK   (HUGE_SUBPAGE_SIZE - 1)
#define HUGE_NUM_SUBPAGES   32

#define BIGNODE( stg )  (&HugeTab[(stg & HUGE_BIT_MASK) >> HUGE_OFFSET_SHIFT])
#define SUBPAGENUM(stg)     ((stg & HUGE_OFFSET_MASK) >> HUGE_SUBPAGE_SHIFT)

#define BIGNODE_OFF( stg )  (stg & HUGE_SUBPAGE_MASK)

/* the following structures are for "normal" virtual memory allocation */

typedef struct seg_table {
    struct seg_table *      next;       // next entry to swap out.
    virt_flags              flags;
    unsigned_16             size;
    spilladdr               loc;
} seg_table;

/* the seg tables are referenced by an array of pointers, something like
SegTab --> 0 1 2 3 4 5 6 ...        <-- array elements are pointers
           | | | | | | |
           v v v v v v v
           1 1 1 1 1 1 1    <-  array elements are seg_tables
           2 2 2 2 2 2 2   these arrays are referred to as "branches",
           3 3 3 3 3 3 3   and the individual elements are called "leaves".

in other words, it is effectively a dynamically allocated 2 dimensional array.
All virtual memory locations from 0 - 0x7FFFFFFF are split into 4K pages.

a virtual memory address is split into three parts:
offset                   leaf id   branch id
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   12 bits             | 4 bits|  15 bits                    |0|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 low order word                 high order word.
the leaf id chooses which element of the branch is the correct seg_table.
 0 means that the bit is always zero.
*/

// this structure is used for picking the high order word off a long
typedef struct wordpick {
    unsigned_16 low;
    unsigned_16 high;
} wordpick;

// this is used instead of the virt_mem type inside this module, since it is
// desirable to be able to get the high order word without having to do a
// 16-bit shift. MAKE SURE THAT VIRT_MEM IS THE SAME SIZE AS THIS STRUCTURE!
typedef union {
    unsigned_32     l;
    wordpick        w;
} virt_struct;

#define OFFSET_SHIFT      12
#define MAX_NODE_SIZE     (1U << OFFSET_SHIFT)
#define MAX_LEAFS         16            // maximum # of leafs per branch
#define SEG_LIMIT         32767     // maximum # of branches (leafs * 16)

/* find the node for MEM_ADDR or FILE_ADDR */
#define NODE( stg )         (&SegTab[ stg.w.high ][ stg.w.low >> OFFSET_SHIFT ])

#define NODE_OFF( stg )     ( stg.w.low & (MAX_NODE_SIZE-1) )

#define TINY_BLOCK_CUTOFF 256

static huge_table       *HugeTab;
static unsigned         NumHuge;
static unsigned         NextHuge;
static seg_table        **SegTab;
static unsigned         NumBranches;
// start with branch # 1 so an address of zero can be illegal.
static unsigned         CurrBranch;
static unsigned         NextLeaf;       // next leaf # to be allocated.
static seg_table        *NextSwap;      // next entry to swap out.
static unsigned         TinyLeft;
static virt_mem         TinyAddr;

void VirtMemInit( void )
/*****************************/
// Allocate space for the branch pointers.
{
    NumHuge = HUGE_INITIAL_ALLOC;
    NextHuge = 0;
    NumBranches = 127;
    CurrBranch = 1;
    NextLeaf = 0;
    NextSwap = NULL;
    TinyLeft = 0;
    _ChkAlloc( SegTab, NumBranches * sizeof( seg_table * ) );
    memset( SegTab, 0, NumBranches * sizeof( seg_table * ) );
    SegTab[1] = PermAlloc( sizeof( seg_table ) * MAX_LEAFS );
    memset( SegTab[1], 0, sizeof( seg_table ) * MAX_LEAFS );
    _ChkAlloc( HugeTab, NumHuge * sizeof( huge_table ) );
    memset( HugeTab, 0, NumHuge * sizeof( huge_table ) );
}

static void GetMoreBranches( void )
/*********************************/
// make a larger array to hold branch pointers in.
{
    seg_table **    branches;
    unsigned        alloc_size;

    alloc_size = NumBranches * sizeof( seg_table * );
    NumBranches = NumBranches * 2;   // double the # of pointers.
    if( NumBranches > SEG_LIMIT ) {
        LnkMsg( FTL+MSG_NO_VIRT_MEM, NULL );
    }
    _ChkAlloc( branches, alloc_size * 2 );
    memcpy( branches, SegTab, alloc_size );
    memset( (char *)branches + alloc_size, 0, alloc_size ); // null pointers
    _LnkFree( SegTab );
    SegTab = branches;
}

static virt_struct GetStg( virt_mem_size amt )
/********************************************/
{
    seg_table *             seg_entry;
    unsigned                alloc_size;
    virt_struct             vmem;

    if( NextLeaf >= MAX_LEAFS ) {
        NextLeaf = 0;
        CurrBranch++;
        if( CurrBranch >= NumBranches ) {
            GetMoreBranches();
        }
        alloc_size = sizeof( seg_table ) * MAX_LEAFS;
        seg_entry = PermAlloc( alloc_size );
        memset( seg_entry, 0, alloc_size ); //set all flags FALSE.
        SegTab[ CurrBranch ] = seg_entry;
    } else {
        seg_entry = &SegTab[ CurrBranch ][ NextLeaf ];
    }
    seg_entry->size = amt;
    seg_entry->loc.spill = 0;
    seg_entry->next = NULL;
    vmem.w.high = CurrBranch;
    vmem.w.low = NextLeaf << OFFSET_SHIFT;
    DEBUG((DBG_VIRTMEM, "virt %h amt %x", vmem.l, amt ));
    NextLeaf++;
    return( vmem );
}

static virt_struct GetBigStg( virt_mem_size size )
/************************************************/
{
    unsigned        alloc_size;
    huge_table *    newtab;
    huge_table *    huge_entry;
    virt_struct     vmem;

    if( NextHuge >= NumHuge ) {
        alloc_size = NumHuge * sizeof( huge_table );
        NumHuge *= 2;
        if( NumHuge > HUGE_LIMIT ) {
            LnkMsg( FTL+MSG_NO_VIRT_MEM, NULL );
        }
        _ChkAlloc( newtab, alloc_size * 2 );
        memcpy( newtab, HugeTab, alloc_size );
        memset( (char *)newtab + alloc_size, 0, alloc_size );
        _LnkFree( HugeTab );
        HugeTab = newtab;
    }
    huge_entry = &HugeTab[NextHuge];
    vmem.l = ((unsigned long)NextHuge << HUGE_OFFSET_SHIFT) | HUGE_PAGE;
    huge_entry->numthere = (size >> HUGE_SUBPAGE_SHIFT) + 1;
    huge_entry->numswapped = 0;
    huge_entry->sizelast = size & HUGE_SUBPAGE_MASK;
    if( huge_entry->sizelast == 0 ) {
        huge_entry->numthere--;
        huge_entry->sizelast = HUGE_SUBPAGE_SIZE;
    }
    huge_entry->page = NULL;
    huge_entry->next = NULL;
    huge_entry->flags = VIRT_HUGE;
    DEBUG((DBG_VIRTMEM, "huge virt %h amt %h", vmem.l, size ));
    NextHuge++;
    return( vmem );
}

static virt_mem DoAllocStg( virt_struct (*allocfn)(virt_mem_size),
                                   virt_mem_size size, virt_mem_size limit )
/**************************************************************************/
{
    virt_struct ret;
    bool        gotaddr;

    ret.l = 0;
    gotaddr = FALSE;
    for( ; size > limit; size -= limit ) {
        if( !gotaddr ) {
            gotaddr = TRUE;
            ret = allocfn( limit );
        } else {
            allocfn( limit );
        }
    }
    if( !gotaddr ) {
        ret = allocfn( size );
    } else {
        allocfn( size );
    }
    return( ret.l );
}

static virt_mem AllocTinyStg( unsigned size )
/*******************************************/
{
    virt_mem    retval;

    if( TinyLeft < size ) {
        TinyAddr = DoAllocStg( GetStg, MAX_NODE_SIZE, MAX_NODE_SIZE );
        TinyLeft = MAX_NODE_SIZE;
    }
    retval = TinyAddr;
    TinyAddr += size;
    TinyLeft -= size;
    return( retval );
}

virt_mem AllocStg( virt_mem_size size )
/*************************************/
{
    if( size == 0 )
        return 0;
    size = MAKE_EVEN( size );
    if( size <= TINY_BLOCK_CUTOFF ) {
        return( AllocTinyStg( size ) );
    } else if( size >= MAX_BIGNODE_SIZE ) {
        return( DoAllocStg( GetBigStg, size, MAX_BIGNODE_SIZE ) );
    } else {
        return( DoAllocStg( GetStg, size, MAX_NODE_SIZE ) );
    }
}

void ReleaseInfo( virt_mem stg )
/*************************************/
// can't prematurely release, but no big deal
{
}

bool SwapOutVirt( void )
/*****************************/
// NOTE - this routine assumes that once something has been swapped out, it
// will never be read back in again.
{
    spilladdr       *spillmem;
    void            *mem;
    seg_table       *seg_entry;
    huge_table      *huge_entry;
    unsigned        size;

    while( NextSwap != NULL ) {
        seg_entry = NextSwap;
        NextSwap = NextSwap->next;
        if( seg_entry->flags & VIRT_INMEM ) {
            if( seg_entry->flags & VIRT_HUGE ) {
                huge_entry = (huge_table *)seg_entry;
                spillmem = &huge_entry->page[huge_entry->numswapped];
                mem = (*spillmem).addr;
                huge_entry->numswapped++;
                if( huge_entry->numthere == huge_entry->numswapped ) {
                    seg_entry->flags &= ~VIRT_INMEM;
                    size = huge_entry->sizelast;
                } else {
                    size = HUGE_SUBPAGE_SIZE;
                }
                (*spillmem).spill = SpillAlloc( size );
                SpillWrite( (*spillmem).spill, 0, mem, size );
                _LnkFree( mem );
            } else {
                seg_entry->flags &= ~VIRT_INMEM;
                mem = seg_entry->loc.addr;
                seg_entry->loc.spill = SpillAlloc( seg_entry->size );
                SpillWrite( seg_entry->loc.spill, 0, mem, seg_entry->size );
                _LnkFree( mem );
            }
            DEBUG((DBG_VIRTMEM, "swapping out %h to %h", mem, seg_entry->loc.spill ));
            return( TRUE );
        }
    }
    return( FALSE );
}

void FreeVirtMem( void )
/*****************************/
{
    unsigned        index;
    unsigned        inner;
    unsigned        branch;
    unsigned        leaf;
    seg_table *     seg_entry;
    huge_table *    huge_entry;
    spilladdr *     page;

    if( SegTab == NULL )
        return;
    for( branch = 0; branch < NumBranches; branch++ ) {
        seg_entry = SegTab[ branch ];
        if( seg_entry != NULL ) {
            for( leaf = 0; leaf < MAX_LEAFS; leaf++ ) {
                if( seg_entry->flags & VIRT_INMEM ) {
                    _LnkFree( seg_entry->loc.addr );
                }
                seg_entry++;
            }
        }
    }
    _LnkFree( SegTab );
    for( index = 0; index < NumHuge; index++ ) {
        huge_entry = &HugeTab[ index ];
        if( huge_entry->page != NULL ) {
            page = huge_entry->page;
            for( inner = huge_entry->numswapped; inner < huge_entry->numthere; inner++ ) {
                _LnkFree( page[inner].addr );
            }
        }
    }
    _LnkFree( HugeTab );
}

static void AllocNode( seg_table *node )
/**************************************/
/* allocate a regular 4K page */
{
    void        *mem;

    _LnkAlloc( mem, node->size );
    if( mem == NULL ) {
        node->loc.spill = SpillAlloc( node->size );
        node->flags &= ~VIRT_INMEM;
    } else {
        node->loc.addr = mem;
        node->flags |= VIRT_INMEM;
    }
    LinkList( &NextSwap, node );
}

static void AllocHugeNode( huge_table *node )
/*******************************************/
/* allocate a huge page */
{
    void *      mem;
    int         index;
    spilladdr * page;
    bool        nomem;

    node->page = PermAlloc( HUGE_NUM_SUBPAGES * sizeof( spilladdr ) );
    page = node->page;
    _LnkAlloc( mem, node->sizelast );  /* first try to allocate the last bit*/
    if( mem == NULL ) {                 /* on the end of the page. */
        node->flags &= ~VIRT_INMEM;
        page[node->numthere - 1].spill = SpillAlloc( node->sizelast );
        nomem = TRUE;
        node->numswapped = node->numthere;
    } else {
        page[node->numthere - 1].addr = mem;
        nomem = FALSE;
    }
    /* now allocate all of the subpages, starting at the end and working backwards
     * so that they can be swapped from the start forwards. */
    for( index = node->numthere - 2; index >= 0; index-- ) {
        if( nomem ) {
            page[ index ].spill = SpillAlloc( HUGE_SUBPAGE_SIZE );
        } else {
            _LnkAlloc( mem, HUGE_SUBPAGE_SIZE );
            if( mem == NULL ) {
                nomem = TRUE;
                node->numswapped = index + 1;
                page[ index ].spill = SpillAlloc( HUGE_SUBPAGE_SIZE );
            } else {
                page[ index ].addr = mem;
            }
        }
    }
}

static bool ScanNodes( virt_mem mem, void *info, virt_mem_size len,
                    bool (*rtn)( void *, spilladdr, unsigned, unsigned, bool))
/*****************************************************************************/
/* go through the virtual memory nodes, reading or writing data */
{
    virt_mem_size   end_off;
    unsigned        off;
    seg_table       *node;
    huge_table      *bignode;
    spilladdr       *page;
    unsigned        currlen;
    unsigned        amt;
    unsigned        subpage;
    virt_struct     stg;
    bool            retval;

    stg.l = mem;
    if( stg.l & HUGE_PAGE ) {  /* it is a huge page */
        off = BIGNODE_OFF( stg.l );
        for( ;; ) {
            bignode = BIGNODE( stg.l );
            page = bignode->page;
            subpage = SUBPAGENUM( stg.l );
            if( subpage == bignode->numthere - 1 ) {
                currlen = bignode->sizelast;
            } else {
                currlen = HUGE_SUBPAGE_SIZE;
            }
            end_off = off + len;
            if( page == NULL ) {
                AllocHugeNode( bignode );
                page = bignode->page;
            }
            if( end_off <= currlen )
                break;
            amt = currlen - off;
            if( !rtn( info, page[subpage], off, amt, subpage >= bignode->numswapped ) )
                return FALSE;
            len -= amt;
            stg.l += amt;
            info = (char *)info + amt;
            off = 0;
        }
        retval = rtn( info, page[subpage], off, len, subpage >= bignode->numswapped );
    } else {
        off = NODE_OFF( stg );
        for( ;; ) {
            node = NODE( stg );
            end_off = off + len;
            if( node->loc.spill == 0 ) {
                AllocNode( node );
            }
            if( end_off <= node->size )
                break;
            amt = node->size - off;
            if( !rtn( info, node->loc, off, amt, node->flags & VIRT_INMEM ) ) {
                return FALSE;
            }
            len -= amt;
            stg.l += amt;
            info = (char *)info + amt;
            off = 0;
        }
        retval = rtn( info, node->loc, off, len, node->flags & VIRT_INMEM );
    }
    return retval;
}

static bool LoadInfo( void * info, spilladdr loc, unsigned off, unsigned len, bool inmem )
/****************************************************************************************/
/* copy data to info from the memory or spillfile referenced by node & off */
{
    if( len == 0 )
        return TRUE;
    if( inmem ) {
        memcpy( info, (char *)loc.addr + off, len );
    } else {
        SpillRead( loc.spill, off, info, len );
    }
    return TRUE;
}

void ReadInfo( virt_mem stg, void *buf, virt_mem_size len )
/***********************************************************/
/* copy data into info from the memory or spillfile referenced by stg */
{
    ScanNodes( stg, buf, len, LoadInfo );
}

static bool SaveInfo( void * info, spilladdr loc, unsigned off, unsigned len, bool inmem )
/****************************************************************************************/
/* copy data at info to the memory or spillfile referenced by node & off */
{
    if( len == 0 ) return TRUE;
    DEBUG((DBG_VIRTMEM, "saving %d bytes (offset %x) to %d.%h", len, off, inmem, loc.spill ));
    if( inmem ) {
        memcpy( (char *)loc.addr + off, info, len );
    } else {
        SpillWrite( loc.spill, off, info, len );
    }
    return TRUE;
}

void PutInfo( virt_mem stg, void * info, virt_mem_size len )
/************************************************************/
/* copy data at info to the memory or spillfile referenced by stg */
{
    ScanNodes( stg, info, len, SaveInfo );
}

void CopyInfo( virt_mem a, virt_mem b, unsigned len )
/***************************************************/
{
    void *      buf;

    _ChkAlloc( buf, len );
    ReadInfo( b, buf, len );
    PutInfo( a, buf, len );
    _LnkFree( buf );
}

static bool CompareBlock( void * info, spilladdr loc, unsigned off, unsigned len, bool inmem )
/********************************************************************************************/
/* compare data at info to the memory or spillfile referenced by node & off */
{
    void *      buf;

    if( len == 0 )
        return TRUE;
    if( inmem ) {
        buf = (char *)loc.addr + off;
    } else {
        buf = alloca( len );
        SpillRead( loc.spill, off, buf, len );
    }
    return memcmp( buf, info, len ) == 0;
}

bool CompareInfo( virt_mem stg, void *info, virt_mem_size len )
/***************************************************************/
{
    return ScanNodes( stg, info, len, CompareBlock );
}

static bool OutInfo( void * dummy, spilladdr loc, unsigned off, unsigned len, bool inmem )
/****************************************************************************************/
/* copy data in memory or spillfile referenced by node & off to LoadFile */
{
    unsigned    amt;

    dummy = dummy;   /* to avoid a warning: will be optimized away. */
    if( len == 0 )
        return TRUE;
    DEBUG((DBG_VIRTMEM, "writing %d bytes (offset %x) to %d.%h", len, off, inmem, loc.spill ));
    if( inmem ) {
        WriteLoad( (char *)loc.addr + off, len );
    } else {
        for( ; len != 0; len -= amt ) {
            amt = len;
            if( amt > TokSize )
                amt = TokSize;
            SpillRead( loc.spill, off, TokBuff, amt );
            WriteLoad( TokBuff, amt );
            off += amt;
        }
    }
    return TRUE;
}

void WriteInfoLoad( virt_mem stg, virt_mem_size len )
/******************************************************/
/* copy data in memory or spillfile referenced by stg to LoadFile */
{
    ScanNodes( stg, NULL, len, OutInfo );
}

static bool NullInfo( void *dummy, spilladdr loc, unsigned off, unsigned len, bool inmem )
/****************************************************************************************/
// write nulls to the location referenced by node and off.
{
    dummy = dummy;   /* to avoid a warning: will be optimized away. */
    if( len == 0 )
        return TRUE;
    if( inmem ) {
        memset( (char *)(loc.addr) + off, 0, len );
    } else {
        SpillNull( loc.spill, off, len );
    }
    return TRUE;
}

void PutInfoNulls( virt_mem stg, virt_mem_size len )
/**************************************************/
/* copy NULLS in memory or spillfile referenced by stg */
{
    ScanNodes( stg, NULL, len, NullInfo );
}
