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
* Description:  New (Whoosh) overlay loader definitions.
*
****************************************************************************/

//   !!!!!!!!!   must correspond to declaration in novlldr.inc  !!!!!!!!!

#ifndef _NOVLLDR_H_
#define _NOVLLDR_H_

#include <dos.h>
#include <stddef.h>
#include "ovlstd.h"

/*
    NOVLLDR specific routines
*/
extern  int      near __OVLFIXCALLCHAIN__( unsigned old, unsigned new );
extern  void     near __OVLBUILDRETTRAP__( unsigned old_handle, unsigned rt_seg );
extern  unsigned near __OVLSCANCALLCHAIN__( void );
extern  void     far  __OVLRETTRAP__( void );

#ifdef OVL_MULTITHREAD

extern  unsigned near __OVLUNDORETTRAP__( unsigned rt_seg,
                                                unsigned new_handle );
#else

extern  void     near __OVLUNDORETTRAP__( unsigned stack_trap, unsigned ret_offset,
                                unsigned ret_list, unsigned new_handle );
#endif

extern  void     near __NOVLLDR__( void );
extern  unsigned near __WhichArea__( unsigned seg );
extern  void     near __OVLINITAREA__( unsigned seg, unsigned size );

/*
    NOVLLDR data
*/
extern  unsigned        _CODE_BASED __OVLROVER__;
extern  unsigned        _CODE_BASED __OVLAREALIST__;
extern  unsigned_16     _CODE_BASED __OVLSTARTPARA__;


/*
    Different uses of fields in an ovltab_entry:

    Since NOVLLDR uses a flat overlay structure, the "anc" part of the
    flags_anc word is not required.  Instead, the low byte of flags_anc
    is used as a flag for the second chance cyclic page replacement
    algorithm.  This byte is accessed through the OVL_ACCESSES macro.
    (Also accessed by each munged overlay vector.)

    The upper byte contains the flags:

    FLAG_CHANGED:   Used for debugging support.  It is set by any of the
        overlay loading routines if they move, ret trap, or unload a
        section.  i.e., whenever the section is changed.

    FLAG_INMEM:     Set whenever the overlay is in memory or return trapped.

    FLAG_SELF_REF:  Set to indicate the overlay has self-referential relocs.

    FLAG_RET_TRAP:  Set whenever the section is return trapped.  When set,
        the code_handle contains the segment of the return trap.

    NOVLLDR assumes that all non-PRELOAD sections were generated with the
    same start_para.  It saves the original value of this start_para
    in __OVLSTARTPARA__.  From then on, the start_para must be zeroed.
    This is to save time in __OVLSCANCALLCHAIN__ which would otherwise
    have to zero each start_para before scanning the call chain.  It is
    set non-zero by __OVLSCANCALLCHAIN__ when the section is in the
    call chain. Note that in order to use __OvlRelocLoad__ start_para
    must be set to __OVLSTARTPARA__ and then set to 0 when done.

    14-jun-91 DJG
*/

#define OVL_ACCESSES( __o )     (*(unsigned_8 _CODE_BASED *)&__o->flags_anc)

/********************* memory management info *****************************/

/*
    The overlay loader can have more than one memory area.  An area_list
    structure appears in the first paragraph of each area.  A free_block
    with num_paras == 0 appears in the last paragraph of each area.  The
    free list is setup in between both these paragraphs.

    The beginning of the free list is accessed through area->fblk.next.

    IMPORTANT:  Blocks (free & used) in an area have a descriptor word
        associated with them that indicates whether the block is free
        (BLOCK_IS_FREE) or what overlay number the block belongs to.
        This word is stored at offset 0xE in the next lower segment.
        i.e., if a block is at 0x5005 it's descriptor is at 0x5004:0xE.
        The linker ensures that no overlay section has code/data in this word.

        This also means that the free_block and area_list structures cannot
        have data at their 0xE offset.

    09-jun-91 DJG

    These structures are also defined in novlldr.inc.
*/
typedef unsigned_16     desc;
typedef desc far        *desc_ptr;
#define BLOCK_IS_FREE 0xFFFF
#define NULL_SEG 0

#pragma pack( push, 1 )

typedef struct {
    unsigned_16 next;       /* paragraph of next free block in the list */
    unsigned_16 prev;       /* paragraph of previous free block in the list */
    unsigned_16 num_paras;  /* number of free paras in this block */
} free_block;
typedef free_block far *free_block_ptr;

typedef struct {
    free_block  fblk;           /* area has fake free_block with num_paras ==0*/
    unsigned_16 next;           /* next memory area in list */
    unsigned_16 size;           /* number of paragraphs in this area */
    unsigned_16 free_paras;     /* total size of all free blocks in area */
} area_list;
typedef area_list far *area_list_ptr;

/************************** return trapping info **************************/

/*
    Return traps are used to throw code out of the call chain.  The lowest
    return into a section is trapped, and the rest are linked together.

    Suppose a section is in memory at 0x5abc; and suppose there are far
    return addrs to this section on the stack at offsets 0x780, 0x700, and
    0x640 (near ret addrs can be ignored). example:

        SS:0x780        0x5abc          <-- segment of section
           0x77e        0x1234          <-- return offset
           0x77c        0x0881          <-- saved bp (with far bit set)
           ...
           0x700        0x5abc          <-- segment of section
           0x6fe        0x2112          <-- return offset
           0x6fc        0x077d          <-- saved bp (with far bit set)
           ...
           0x640        0x5abc          <-- segment of section
           0x63e        0x0021          <-- return offset
           0x63c        0x065d          <-- saved bp (with far bit set)
           ...

    To toss this section out of memory we need to trap the lowest return
    addr (it is always used before higher ones -- except during a longjmp
    which is handled specially).  Suppose the overlay loader creates the
    trap at paragraph 0x5005.  This is what it would look like:

        0x5004:E        xxxx            <-- section number (memory handler)
        0x5005:0        call far ret_trap_entry
        0x5005:6        0x0021          <-- saved return from lowest on stack
  +---< 0x5005:8        0x077c          <-- ptr to head of linked list
  |     0x5005:A        0x063c          <-- offset of trap on stack
  |
  | And the stack would be modified like so:
  |
  |     SS:0x780        0x06fc >-+      <-- next ret addr offset
  |        0x77e        0x1234   |      <-- return offset
  +--->    0x77c        0x0880   |      <-- saved bp (with far bit not set)
           ...                   |
           0x700        0x0000   |      <-- end of linked list
           0x6fe        0x2112   |      <-- return offset
           0x6fc        0x077c <-+      <-- saved bp (with far bit not set)
           ...
           0x640        0x5005          <-- segment of return trap
           0x63e        0x0000          <-- return trap offset
           0x63c        0x065d          <-- saved bp (with far bit set)
           ...

    Note the heavy reliance on the 64k limitation on sections.  This
    basically gives us two free bytes in every far pointer.

    Also note that the bp chain has been modified so that the ret addrs
    don't appear to be far ret addrs any more.  This way the overlay loader
    won't get confused while examining the stack at a later time.

    When the return trap is invoked, the section is loaded into memory.  Then
    we walk up the linked list on the stack replacing the links with the
    new load address of the section; and setting the bp far bits.

    09-jun-91 DJG
*/
/*  Multithreaded applications can require more than one set of
    (stack_trap,ret_list,offset)
    Follow it by a 0 for stack_trap

    28-jun-83 DPY
*/

#ifdef OVL_MULTITHREAD

typedef struct {
    unsigned_16 stack_trap;     /* offset of bp for this trap on stack */
    unsigned_16 ret_list;       /* beginning of linked list */
    unsigned_16 ret_offset;     /* offset of original ret addr */
    unsigned_16 context;        /* offset of context for this trap */
} trap_entry;

/* List of tasks for multithreading
    28-jun-93   DPY
   Must reflect code in engine RNKERNEL.C
*/

typedef struct {
    unsigned_16 saved_bp;       // is 0 for active task (us BPChain instead)
    unsigned_16 saved_sp;
    unsigned_16 next;
} task_list;

extern task_list        *Context_list;

#define MAX_THREAD      5

typedef struct {
    /* any changes to structure should also be done in novlldr.inc */
    unsigned_8  call_far;
    dos_addr    rt_entry;       /* entry into return trap handler */
    unsigned_8  pad;
    unsigned_16 old_code_handle; /* for the debugger */
    trap_entry  traps[MAX_THREAD];/* set of return traps */
    unsigned_16 end_of_list;    /* End of list if all threads used */
} ret_trap;

#define RET_TRAP_PARA   ((sizeof(ret_trap)+sizeof(unsigned_16)+15)/16)

#else

typedef struct {
    /* any changes to structure should also be done in novlldr.inc */
    unsigned_8  call_far;
    dos_addr    rt_entry;       /* entry into return trap handler */
    unsigned_8  pad;
    unsigned_16 ret_offset;     /* offset of original ret addr */
    unsigned_16 old_code_handle;/* for the debugger */
    unsigned_16 ret_list;       /* beginning of linked list */
    unsigned_16 stack_trap;     /* offset of bp for this trap on stack */
    /* cannot be greater than 14 bytes! see memory notes above for reason */
} ret_trap;
#endif

typedef ret_trap far *ret_trap_ptr;

#define CALL_FAR_INSTRUCTION    0x9a

#pragma pack( pop )

#endif
