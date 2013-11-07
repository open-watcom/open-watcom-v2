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
* Description:  New (Whoosh) overlay loader.
*
****************************************************************************/


#include "novlldr.h"

/*
 * Here's a nice fast inline memcpy that is used if a 386 or better is
 * detected. This is passed the length to copy in paragraphs.
*/

extern  void DoFastCopyPara( unsigned dst, unsigned src, unsigned len_in_paras );

#pragma aux DoFastCopyPara = \
        ".386" \
        "xor si,si" \
        "xor di,di" \
        "test byte ptr CS:__OVLFLAGS__, 1 " \
        "jz   cpu86" \
        "shl cx,2" \
        "rep movsd" \
        "jmp short end" \
"cpu86:  shl cx,1" \
        "shl cx,1" \
        "shl cx,1" \
        "rep movsw" \
"end:   " \
        parm [es] [ds] [cx]               \
        modify exact [si di cx];


/**************************************************************************/

#ifdef OVL_MULTITHREAD
// Extra flag used only for the Multi-threaded overlay manager

#define FLAG_ACTIVE_TRAP        1       // Assumes flags start at 0x8000

#endif

static void __near AllocSeg( unsigned seg, unsigned amount, unsigned area_seg )
/***************************************************************************/
/* Allocate amount paras from seg in area_seg */
{
    area_list_ptr       area;
    free_block_ptr      memptr;
    free_block_ptr      newptr;
    free_block_ptr      nextptr;
    free_block_ptr      prevptr;

    area = MK_FP( area_seg, 0 );
    area->free_paras -= amount;
    memptr = MK_FP( seg - 1, 0x10 );
    if( memptr->num_paras == amount ) {     /* took up entire free block */
        nextptr = MK_FP( memptr->next, 0 ); /* unlink from ring */
        nextptr->prev = memptr->prev;
        prevptr = MK_FP( memptr->prev, 0 );
        prevptr->next = memptr->next;
    } else { /* make new free block with remaining paras from this block */
        newptr = MK_FP( FP_SEG( memptr ) + amount, 0x10 );
        newptr->next = memptr->next;
        newptr->prev = memptr->prev;
        newptr->num_paras = memptr->num_paras - amount;
        *((desc_ptr)newptr - 1) = BLOCK_IS_FREE;
        nextptr = MK_FP( newptr->next, 0 );
        nextptr->prev = FP_SEG( newptr ) + 1;
        prevptr = MK_FP( newptr->prev, 0 );
        prevptr->next = FP_SEG( newptr ) + 1;
    }
}


static unsigned __near AllocFromArea( unsigned amount, unsigned area_seg )
/**********************************************************************/
/* Allocate amount paras from area. */
{
    free_block_ptr      memptr;
    unsigned            seg;
    area_list_ptr       area;

    area = MK_FP( area_seg, 0 );
    for( memptr = MK_FP( area->fblk.next, 0 ); memptr->num_paras != 0 && memptr->num_paras < amount; ) {
        memptr = MK_FP( memptr->next, 0 );
    }
    seg = NULL_SEG;
    if( memptr->num_paras != 0 ) {
        seg = FP_SEG( memptr );
        AllocSeg( seg, amount, FP_SEG( area ) );
    }
    return( seg );
}


static unsigned __near Allocate( unsigned amount )
/**********************************************/
/* Allocate amount paragraphs from any area */
{
    unsigned            seg;
    area_list_ptr       area;

    for( area = MK_FP( __OVLAREALIST__, 0 ); area != NULL; area = MK_FP( area->next, 0 ) ) {
        if( area->free_paras >= amount ) {
            seg = AllocFromArea( amount, FP_SEG( area ) );
            if( seg != NULL_SEG ) {
                return( seg );
            }
        }
    }
    return( NULL_SEG );
}


static void __near FreeWithNext( unsigned seg, unsigned num_paras,
                                        unsigned area_seg, unsigned next_seg )
/****************************************************************************/
/* Put seg of size num_paras onto the free list in area_seg.  next_seg must be
 * the segment of the next free block in the area. */
{
    area_list_ptr       area;
    free_block_ptr      prevptr;
    free_block_ptr      nextptr;
    free_block_ptr      memptr;

    /* add block to area */
    area = MK_FP( area_seg, 0 );
    area->free_paras += num_paras;
    memptr = MK_FP( seg, 0 );
    memptr->num_paras = num_paras;
    memptr->next = next_seg;
    nextptr = MK_FP( next_seg, 0 );
    memptr->prev = nextptr->prev;
    nextptr->prev = FP_SEG( memptr );
    prevptr = MK_FP( memptr->prev, 0 );
    prevptr->next = FP_SEG( memptr );
    /* try to coalesce forward */
    if( nextptr->num_paras != 0 &&
                FP_SEG( memptr ) + memptr->num_paras == FP_SEG( nextptr ) ) {
        memptr->num_paras += nextptr->num_paras;
        memptr->next = nextptr->next;
        nextptr = MK_FP( nextptr->next, 0 ); /* advance nextptr */
        nextptr->prev = FP_SEG( memptr );
    }
    /* try to coalesce backward */
    if( prevptr->num_paras != 0 &&
                FP_SEG( prevptr ) + prevptr->num_paras == FP_SEG( memptr ) ) {
        /* coalesce backward */
        prevptr->next = memptr->next;
        nextptr->prev = FP_SEG( prevptr );
        prevptr->num_paras += memptr->num_paras;
        memptr = prevptr;
    }
    /* set up the descriptor */
    *(desc_ptr)MK_FP( FP_SEG( memptr ) - 1, 0xE ) = BLOCK_IS_FREE;
}


static void __near FreeSeg( unsigned seg, unsigned num_paras, unsigned area_seg )
/*****************************************************************************/
/* Add the seg of size num_paras to the free list in area_seg. */
{
    area_list_ptr       area;
    free_block_ptr      nextptr;
    free_block_ptr      memptr;

    area = MK_FP( area_seg, 0 );
    memptr = MK_FP( seg, 0 );
    /* Note: Since there is a dummy block at end of list, we don't have to
     * explicitly test for it here. */
    for( nextptr = MK_FP( area->fblk.next, 0 ); FP_SEG( nextptr ) < FP_SEG( memptr ); ) {
        nextptr = MK_FP( nextptr->next, 0 );
    }
    FreeWithNext( seg, num_paras, area_seg, FP_SEG( nextptr ) );
}


static void __near DeMungeVectors( unsigned tab_off, unsigned sec_num,
                                                                unsigned seg )
/****************************************************************************/
/* overlay not in memory any more, so demunge vectors.
 * NOTE: this assumes that __NOVLLDR__ and the vectors are in the same segment!
 */
{
    lvector_ptr vect;
    unsigned    loader;

    loader = FP_OFF( __NOVLLDR__ ) - FP_OFF( &__OVLSTARTVEC__->u.v.ldr_addr ) - sizeof( __OVLSTARTVEC__->u.v.ldr_addr );
    WALK_ALL_VECT( vect ) {
        if(( vect->u.i.cs_over == OVV_CS_OVERRIDE ) && ( vect->u.i.tab_addr == tab_off )) {
            vect->u.v.call_op = CALL_INSTRUCTION;
            vect->u.v.ldr_addr = loader;
            vect->u.v.sec_num = sec_num;
            vect->target.seg -= seg;
        }
        loader -= sizeof( lvector );
    }
}


static unsigned __near UnloadSection( ovltab_entry_ptr ovl, unsigned ovl_num )
/**************************************************************************/
/* Unload a section.  Assumes that __OVLSCANCALLCHAIN__ has been called.
 * Returns the area that the section was in. */
{
    unsigned            area_seg;
    unsigned            rt_seg;

    area_seg = __WhichArea__( ovl->code_handle );
    FreeSeg( ovl->code_handle, ovl->num_paras, area_seg );
    ovl->flags_anc |= FLAG_CHANGED;
    ovl->flags_anc &= ~FLAG_INMEM;
    DeMungeVectors( FP_OFF( ovl ), ovl_num, ovl->code_handle );
    if( ovl->start_para != 0 ) {
        /* was in call chain - build a ret trap */
#ifdef OVL_MULTITHREAD
        rt_seg = Allocate( RET_TRAP_PARA );
#else
        rt_seg = Allocate( 1 );
#endif
        __OVLBUILDRETTRAP__( ovl->code_handle, rt_seg );
        ovl->code_handle = rt_seg;
        ovl->flags_anc |= FLAG_RET_TRAP;
        *(desc_ptr)MK_FP( rt_seg - 1, 0xE ) = ovl_num;
#ifdef OVL_DEBUG
        __OvlMsg__( OVL_SECTION );
        __OvlNum__( ovl_num );
        __OvlMsg__( OVL_RET_TRAPPED );
    } else {
        __OvlMsg__( OVL_SECTION );
        __OvlNum__( ovl_num );
        __OvlMsg__( OVL_UNLOADED );
#endif
    }
    return( area_seg );
}


static unsigned __near UnloadNonChained( unsigned amount, unsigned start_ovl )
/**************************************************************************/
/* Toss overlays that are not in call chain.  Assumes __OVLSCANCALLCHAIN__
 * has been called.  Returns NULL_SEG if it was unable to make enough room in
 * any area.  Otherwise it returns the area seg.
 */
{
    unsigned            i;
    ovltab_entry_ptr    ovl;
    area_list_ptr       area;

    ovl = __OVLTAB__.entries + start_ovl - 1;
    for( i = 0; i < __OVLTABEND__ - __OVLTAB__.entries; ++i ) {
        ++ovl;
        if( !OVLTAB_OK( ovl ) ) {
            ovl = __OVLTAB__.entries;
        }
        if( ( ovl->flags_anc & FLAG_INMEM ) && ( ovl->start_para == 0 ) ) {
            if( OVL_ACCESSES( ovl ) != 0 ) {
#ifdef OVL_MULTITHREAD
                OVL_ACCESSES( ovl ) -= 1;
#else
                OVL_ACCESSES( ovl ) = 0;
#endif
            } else {
                area = MK_FP( UnloadSection( ovl, OVLNUM( ovl ) ), 0 );
                if( area->free_paras >= amount ) {
                    __OVLROVER__ = OVLNUM( ovl );
                    return( FP_SEG( area ) );
                }
            }
        }
    }
    __OVLROVER__ = start_ovl;
    return( NULL_SEG );
}


static int __near UnloadChained( unsigned amount, unsigned_16 __far *call_chain )
/****************************************************************************/
/* Unload sections that are in the call chain.  We also unlink the sections
 * from the call_chain linked list.  Returns NULL_SEG if it didn't make enough
 * room in any area; otherwise returns an area seg with enough room. */
{
    ovltab_entry_ptr    ovl;
    unsigned            ovl_num;
    area_list_ptr       area;

    while( *call_chain != 0xFFFF ) {
        ovl_num = *call_chain >> 4;
        ovl = __OVLTAB__.entries + ovl_num - 1;
        if( OVL_ACCESSES( ovl ) != 0 ) {
#ifdef OVL_MULTITHREAD
            OVL_ACCESSES( ovl ) -= 1;
#else
            OVL_ACCESSES( ovl ) = 0;
#endif
            call_chain = &ovl->start_para;      /* advance pointer */
        } else {
            *call_chain = ovl->start_para;      /* unlink from call chain */
            if( ( ovl->flags_anc & FLAG_RET_TRAP ) == 0 ) {
                area = MK_FP( UnloadSection( ovl, ovl_num ), 0 );
                ovl->start_para = 0;
                if( area->free_paras >= amount ) {
                    return( FP_SEG( area ) );
                }
            }
            ovl->start_para = 0;
        }
    }
    return( NULL_SEG );
}


static void __near redoRelocs( ovltab_entry_ptr ovl, unsigned startseg )
/********************************************************************/
/* Relocate self-referential code. Assumes that ovl->code_handle points
 * to new location of code. */
{
    tiny_ret_t          status;
    tiny_handle_t       fp;
    unsigned            save_delta;
    unsigned            save_start_para;

    status = __OpenOvl__( ovl->fname );
    if( TINY_ERROR( status ) )
        __OvlExit__( OVL_OPEN_ERR );
    fp = TINY_INFO( status );
    if( TINY_ERROR( __OvlSeek__( fp, ovl->disk_addr + ( (unsigned_32)ovl->num_paras << 4 ) ) ) )
        __OvlExit__( OVL_IO_ERR );
    /*
        Fool __OvlRelocLoad__ into relocating section for us by making it
        look like the linker linked things at the current load address.
    */
    save_delta = __OVLTAB__.prolog.delta;
    save_start_para = ovl->start_para;
    ovl->start_para = startseg;
    __OVLTAB__.prolog.delta = 0;
    __OvlRelocLoad__( ovl, fp );
    __OVLTAB__.prolog.delta = save_delta;
    ovl->start_para = save_start_para;
}


static void __near MoveSection( unsigned destseg, ovltab_entry_ptr ovl )
/********************************************************************/
/* this moves a section to the address in destseg, and fixes the vectors.
 * note this assumes that destseg is less than the current address.
*/
{
    unsigned            startseg;
    lvector_ptr         vect;

    ovl->flags_anc |= FLAG_CHANGED;
    startseg = ovl->code_handle;
    ovl->code_handle = destseg;
    *(desc_ptr)MK_FP( destseg - 1, 0xE ) = *(desc_ptr)MK_FP( startseg-1, 0xE );
    __OVLFIXCALLCHAIN__( startseg, destseg );
    /* copy the code */
    DoFastCopyPara( destseg, startseg, ovl->num_paras );
    /* possibly relocate */
    if( ovl->flags_anc & FLAG_SELF_REF ) {
        redoRelocs( ovl, startseg );
    }
    /* modify the vectors */
    WALK_ALL_VECT( vect ) {
        if( vect->u.i.cs_over == OVV_CS_OVERRIDE && vect->u.i.tab_addr == FP_OFF( ovl ) ) {
            vect->target.seg = destseg;
        }
    }
}


static void __near MoveRetTrap( unsigned to_seg, ovltab_entry_ptr ovl )
/*******************************************************************/
{
    unsigned_16                 __far *stkptr;
    ret_trap_ptr                rt;
    ret_trap_ptr                rt_new;
#ifdef OVL_MULTITHREAD
    unsigned                    i;
#endif

    rt = MK_FP( ovl->code_handle - 1, 0x10 );
    rt_new = MK_FP( to_seg - 1, 0x10 );
    *((desc_ptr)rt_new - 1) = *((desc_ptr)rt - 1);      /* copy ovl_num */
    rt_new->call_far = CALL_FAR_INSTRUCTION;            /* standard stuff */
    rt_new->rt_entry.off = FP_OFF( __OVLRETTRAP__ );
    rt_new->rt_entry.seg = FP_SEG( __OVLRETTRAP__ );
#ifdef OVL_MULTITHREAD
    rt_new->old_code_handle = rt->old_code_handle;
    for( i = 0; rt->traps[i].stack_trap != 0; ++i ) {
        rt_new->traps[i].ret_offset = rt->traps[i].ret_offset;
        rt_new->traps[i].ret_list = rt->traps[i].ret_list;
        rt_new->traps[i].stack_trap = rt->traps[i].stack_trap;
        rt_new->traps[i].context = rt->traps[i].context;
        if( !(ovl->flags_anc & FLAG_ACTIVE_TRAP) ) {
            stkptr = MK_FP( FP_SEG( &stkptr ), rt->traps[i].stack_trap + 4 );
            /* fix the stack up */
            *stkptr = to_seg;
        }
    }
    rt_new->traps[i].stack_trap = 0;
#else
    rt_new->ret_offset = rt->ret_offset;                /* copy the trap */
    rt_new->ret_list = rt->ret_list;
    rt_new->stack_trap = rt->stack_trap;
    stkptr = MK_FP( FP_SEG( &stkptr ), rt->stack_trap + 4 );/* fix the stack up */
    *stkptr = to_seg;
#endif
    ovl->code_handle = to_seg;
    ovl->flags_anc |= FLAG_CHANGED;
}


static unsigned __near DefragmentMem( unsigned amount, unsigned area_seg )
/**********************************************************************/
/* Try to defragment an area of memory.  We assume that the area has
 * free_paras >= amount. If this assumption is met, this routine is
 * guaranteed to return a non-NULL_SEG segment of size amount. */
{
    area_list_ptr       area;
    unsigned_16         to_seg;
    free_block_ptr      to_block; /* free block to be filled in */
    ovltab_entry_ptr    ovl;
    desc_ptr            descptr;
    unsigned_16         to_block_paras;
    unsigned            to_block_next;
    unsigned            num_paras;

    /* check if we can just allocate a large enough block */
    to_seg = AllocFromArea( amount, area_seg );
    if( to_seg != NULL_SEG ) {
        return( to_seg );
    }
    area = MK_FP( area_seg, 0 );
    /* there is enough room, so we move used blocks until we get a
     * free block large enough */
    to_seg = area->fblk.next;
    for(;;) {
        to_block = MK_FP( to_seg, 0 );
        to_block_paras = to_block->num_paras;
        to_block_next = to_block->next;
        AllocSeg( to_seg, to_block_paras, FP_SEG( area ) );
        if( to_block_paras >= amount ) { /* can quit early */
            if( to_block_paras != amount ) {
                    /* put back what we don't need */
                FreeWithNext( to_seg + amount, to_block_paras - amount,
                        FP_SEG( area ), to_block_next );
            }
            return( to_seg );
        }
        /* note we cannot be at last free block since we know there is
         * enough free space in this area, and we abort before now when
         * we collect a large enough free block. */
        descptr = MK_FP( to_seg - 1 + to_block_paras, 0xE );
#ifdef OVL_DEBUG
        __OvlMsg__( OVL_SECTION );
        __OvlNum__( *descptr );
        __OvlMsg__( OVL_MOVED );
#endif
        ovl = __OVLTAB__.entries + *descptr - 1;
        if( ovl->flags_anc & FLAG_RET_TRAP ) {
            MoveRetTrap( to_seg, ovl );
#ifdef OVL_MULTITHREAD
            num_paras = RET_TRAP_PARA;
#else
            num_paras = 1;
#endif
        } else {
            MoveSection( to_seg, ovl );
            num_paras = ovl->num_paras;
        }
        to_seg += num_paras;
        FreeWithNext( to_seg, to_block_paras, FP_SEG( area ), to_block_next );
        /* next free block is the one we have just added */
    }
/*  return( NULL_SEG ); unreachable */
}


static unsigned __near ForceAllocate( unsigned amount )
/***************************************************/
/* Always returns a segment of size amount... throws out anything necessary
 * to do so. */
{
    unsigned            seg;
    unsigned_16         call_chain;
    unsigned            rover_save;
    unsigned            area_seg;
    ovltab_entry_ptr    ovl;

    seg = Allocate( amount );
    if( seg != NULL_SEG )
        return( seg );
    /*
        Since the linker ensures that our dynamic area has as least as
        many paragraphs as (sizeof largest section) + (number of sections) + 1.
        This way every section can be ret trapped and we can still load the
        largest section into memory.  Therefore, this loop will terminate... :>

        Oh yeah, we rely on UnloadNonChained & UnloadChained to set the
        OVL_ACCESSES field to zero for every overlay scanned.
    */
    call_chain = __OVLSCANCALLCHAIN__();
    rover_save = __OVLROVER__;
    for(;;) {
        area_seg = UnloadNonChained( amount, rover_save );
        if( area_seg != NULL_SEG ) {
            seg = DefragmentMem( amount, area_seg );
            break;
        }
        area_seg = UnloadChained( amount, &call_chain );
        if( area_seg != NULL_SEG ) {
            seg = DefragmentMem( amount, area_seg );
            break;
        }
    }
    /* we are responsible for zeroing the start_paras */
    while( call_chain != 0xFFFF ) {
        ovl = __OVLTAB__.entries + ( call_chain >> 4 ) - 1;
        call_chain = ovl->start_para;
        ovl->start_para = 0;
    }
    return( seg );
}


void __near __LoadSectionCode__( ovltab_entry_ptr ovl )
/**************************************************/
/* Load the code & do the relocations for a specified overlay.  Loads code
 * into ovl->code_handle.  Caller is responsible for putting the overlay
 * number into the descriptor byte. */
{
    tiny_ret_t          status;
    tiny_handle_t       fp;
    desc_ptr            descptr;
    desc                tmp;

    status = __OpenOvl__( ovl->fname );
    if( TINY_ERROR( status ) )
        __OvlExit__( OVL_OPEN_ERR );
    fp = TINY_INFO( status );
    if( TINY_ERROR( __OvlSeek__( fp, ovl->disk_addr ) ) )
        __OvlExit__( OVL_IO_ERR );
    descptr = MK_FP( ovl->code_handle + ovl->num_paras - 1, 0xE );
    tmp = *descptr;             /* save the descriptor across call */
    __OvlCodeLoad__( ovl, fp );
    *descptr = tmp;
    if( __OvlRelocLoad__( ovl, fp ) ) {
        ovl->flags_anc |= FLAG_SELF_REF;
    }
    ovl->flags_anc |= FLAG_INMEM;
}


unsigned __near __LoadNewOverlay__( unsigned ovl_num )
/**************************************************/
/* Load the overlay into memory.  Removes the return trap if one exists. */
{
    ovltab_entry_ptr    ovl;
    unsigned            segment;
#ifdef OVL_MULTITHREAD
    unsigned            rt_seg;
#else
    unsigned            stack_trap;
    unsigned            ret_offset;
    unsigned            ret_list;
    ret_trap_ptr        rt;
#endif

    ovl = __OVLTAB__.entries + ovl_num - 1;
    if( !(ovl->flags_anc & FLAG_INMEM) ) {
        if( ovl->flags_anc & FLAG_RET_TRAP ) {
#ifdef OVL_MULTITHREAD
            // Prevent fixups of the return trap for this overlay
            ovl->flags_anc |= FLAG_ACTIVE_TRAP;
#else
            rt = MK_FP( ovl->code_handle, 0 );
            stack_trap = rt->stack_trap;
            ret_offset = rt->ret_offset;
            ret_list = rt->ret_list;
            FreeSeg( FP_SEG( rt ), 1, __WhichArea__( FP_SEG( rt ) ) );
            /* Since we have left the FLAG_RET_TRAP set we don't have to
               worry about ForceAllocate trying to remove this section
               from memory.  i.e., we still may have the trap on the
               stack; so we don't want any of our routines to try to
               move it or throw it out :> */
        } else {
            stack_trap = 0;
#endif
        }
        segment = ForceAllocate( ovl->num_paras );
#ifdef OVL_MULTITHREAD
        if( ovl->flags_anc & FLAG_RET_TRAP ) {
            rt_seg = ovl->code_handle;
            FreeSeg( rt_seg, RET_TRAP_PARA, __WhichArea__( rt_seg ) );
            ovl->flags_anc &= ~(FLAG_RET_TRAP|FLAG_ACTIVE_TRAP);
            __OVLUNDORETTRAP__( rt_seg, segment );
            OVL_ACCESSES( ovl ) = 1; /* this overlay has been accessed */
        }
#else
        if( stack_trap != 0 ) {
            ovl->flags_anc &= ~FLAG_RET_TRAP;
            __OVLUNDORETTRAP__( stack_trap, ret_offset, ret_list, segment );
            OVL_ACCESSES( ovl ) = 1; /* this overlay has been accessed */
        }
#endif
        ovl->code_handle = segment;
        *(desc_ptr)MK_FP( segment - 1, 0xE ) = ovl_num;
        ovl->flags_anc |= FLAG_CHANGED;
        ovl->start_para = __OVLSTARTPARA__; /* restore start_para */
        __LoadSectionCode__( ovl );
        ovl->start_para = 0;
#ifdef OVL_DEBUG
        __OvlMsg__( OVL_SECTION );
        __OvlNum__( ovl_num );
        __OvlMsg__( OVL_LOADED );
#endif
    } else {
        OVL_ACCESSES( ovl ) = 1; /* this overlay has been accessed */
        segment = ovl->code_handle;
#ifdef OVL_DEBUG
        __OvlMsg__( OVL_SECTION );
        __OvlNum__( ovl_num );
        __OvlMsg__( OVL_RESIDENT );
#endif
    }
    return( segment );
}


unsigned __near __WOVLLDR__( lvector_ptr vect )
/******************************************/
// Load overlay.
{
    unsigned        retval;
    unsigned        ovl_num;

    ovl_num = vect->u.v.sec_num;            // get the overlay number
    retval = __LoadNewOverlay__( ovl_num );     // load the overlay
    vect->target.seg += retval;                 // now munge the vector.
    vect->u.i.tab_addr = FP_OFF( __OVLTAB__.entries + ovl_num - 1 );
    vect->u.i.cs_over = OVV_CS_OVERRIDE;
    vect->u.i.inc_op = OVV_INC_OPCODE;
    __NDBG_HOOK__( ovl_num, 0, __OVLCAUSE__ );
    return( retval );
}


void __near __OVLINITAREA__( unsigned start, unsigned size )
/*********************************************************/
/* initialize an overlay area */
{
    area_list_ptr       area;
    free_block_ptr      freelist;

    area = MK_FP( start, 0 );
    area->fblk.next = start + 1;
    area->fblk.prev = start + size - 1;
    area->fblk.num_paras = 0;
    area->next = NULL_SEG;
    area->size = size;
    area->free_paras = size - 2;
    /* construct initial free list */
    freelist = MK_FP( start + 1, 0 );
    freelist->prev = start;
    freelist->next = start + size - 1;
    freelist->num_paras = size - 2;
    /* construct dummy blk at end of list */
    freelist = MK_FP( start + size - 1, 0 );
    freelist->prev = start + 1;
    freelist->next = start;
    freelist->num_paras = 0;
}


dos_addr __near __NOVLTINIT__( void )
/*********************************/
// Overlay initialization.
{
    ovltab_entry_ptr    ovl;

    if( __OVLTAB__.prolog.major != OVL_MAJOR_VERSION
        || __OVLTAB__.prolog.minor > OVL_MINOR_VERSION ) {
        __OvlExit__( OVL_BAD_VERSION );
    }
    __OVLFILEPREV__ = 0xFFFF;
    /* We assume that the first overlay table entry is NOT a PRELOAD-type
     * overlay... thus its start_para is the start_para of all the dynamic
     * sections. */
    __OVLSTARTPARA__ = __OVLTAB__.entries[0].start_para; /* save for later */
    __OVLAREALIST__ = __OVLSTARTPARA__ + __OVLTAB__.prolog.delta;
    __OVLROVER__ = 1;

    WALK_ALL_OVL( ovl ) {
        if( ovl->flags_anc & OVE_FLAG_PRELOAD ) {
            ovl->code_handle = ovl->start_para + __OVLTAB__.prolog.delta;
#ifdef OVL_DEBUG
            __OvlMsg__( OVL_SECTION );
            __OvlNum__( OVLNUM( ovl ) );
            __OvlMsg__( OVL_LOADED );
#endif
            __LoadSectionCode__( ovl );
            ovl->num_paras = 0;     /* don't count in in largest size calcs*/
        }
 /* set default flags to zero, set reference count to zero, and make sure that
  * preload sections aren't unloaded (because FLAG_IN_MEM is not set)
  * (all in one statement!) */
        ovl->flags_anc = 0;
        ovl->start_para = 0;    /* required by ForceAllocate code */
        ovl->code_handle = 0;   /* required by debugging support */
    }
    return( __OVLTAB__.prolog.start );
}


/* these two routines are for the C setjmp/longjmp support */

extern unsigned long __far __FINDOVLADDR__( unsigned unused, unsigned segment )
/***************************************************************************/
/* find the overlay number corresponding to the given segment, and turn the
 * segment into a relative offset from the beginning of the section */
{
    unsigned            ovl_num;

    unused = unused;    /* to prevent a warning - will be optimized away */
    if( segment < __OVLTAB__.prolog.delta + __OVLSTARTPARA__ ) {
        ovl_num = 0;  /* in the root */
    } else {
        ovl_num = *(desc_ptr)MK_FP( segment - 1, 0xE ); /* in overlay */
    }
    return( ( (unsigned_32)segment << 16 ) | ovl_num );
}


#ifdef OVL_MULTITHREAD
extern unsigned_32 __near __OVLLONGJMP__( unsigned ovl_num, unsigned segment )
#else
extern unsigned_32 __near __OVLLONGJMP__( unsigned ovl_num, unsigned segment,
                                                            unsigned bp_chain )
#endif
/*****************************************************************************/
/* Ensure that ovl_num is loaded into memory.  Return the segment of ovl_num
 * in DX.  Check all return traps; and move them to higher stack locations
 * if required. This function is wrapped by longjmp_wrap in novlmain.asm */
{
    ovltab_entry_ptr    ovl;
#ifdef OVL_MULTITHREAD
    unsigned_16         rt_seg;
#else
    ret_trap_ptr        rt;
#endif

    /* check return traps */
    WALK_ALL_OVL( ovl ) {
        if( (ovl->flags_anc & FLAG_RET_TRAP) == 0 )
            continue;
#ifdef OVL_MULTITHREAD
        rt_seg = ovl->code_handle;
#else
        rt = MK_FP( ovl->code_handle, 0 );
        if( rt->stack_trap >= bp_chain )
            continue; /* trap safe */
        if( rt->ret_list < bp_chain ) {
            ovl->flags_anc &= ~FLAG_RET_TRAP;
            FreeSeg( FP_SEG( rt ), 1, __WhichArea__( FP_SEG( rt ) ) );
            continue;  /* trap removed */
        }
#endif
        /*
            __OVLUNDORETTRAP__ only undoes things down to the head of the bp
            chain.  We know from above conditions that there must be
            at least one occurance above the bp_chain head.  This could
            be faster; but it gets more complicated.
        */
#ifdef OVL_MULTITHREAD
        if( __OVLUNDORETTRAP__( rt_seg, rt_seg ) == 0 ) {
            ovl->flags_anc &= ~FLAG_RET_TRAP;
            FreeSeg( rt_seg, RET_TRAP_PARA, __WhichArea__( rt_seg ) );
            /* trap removed */
        } else {
            __OVLBUILDRETTRAP__( rt_seg, rt_seg );
        }
#else
        __OVLUNDORETTRAP__( rt->stack_trap, 0, rt->ret_list, FP_SEG( rt ) );
        __OVLBUILDRETTRAP__( FP_SEG( rt ), FP_SEG( rt ) );
#endif
    }
    if( ovl_num )
        segment = __LoadNewOverlay__( ovl_num );
    return( (unsigned_32)segment << 16 );
}


#ifdef OVL_DEBUG
#include <conio.h>

#pragma aux cprintf modify [es ds];

#define CRLF "\r\n"

extern void __far __NOVLDUMP__( void )
/**********************************/
{
    ovltab_entry_ptr    ovl;
    unsigned_16         fn_off;

    cprintf( "ovltab_prolog" CRLF );
    cprintf( "  major=%u, minor=%u, start=%04xh:%04xh," CRLF,
        __OVLTAB__.prolog.major, __OVLTAB__.prolog.minor,
        __OVLTAB__.prolog.start.seg, __OVLTAB__.prolog.start.off );
    cprintf( "  delta=%04xh, ovl_size=%04xh" CRLF CRLF, __OVLTAB__.prolog.delta,
        __OVLTAB__.prolog.ovl_size );

    WALK_ALL_OVL( ovl ) {
        cprintf( "overlay %u:" CRLF "  flags_anc=%04xh", OVLNUM( ovl ),
            ovl->flags_anc );
        if( ovl->flags_anc & FLAG_CHANGED ) {
            cprintf( " FLAG_CHANGED" );
        }
        if( ovl->flags_anc & FLAG_INMEM ) {
            cprintf( " FLAG_INMEM" );
        }
        if( ovl->flags_anc & FLAG_SELF_REF ) {
            cprintf( " FLAG_SELF_REF" );
        }
        if( ovl->flags_anc & FLAG_RET_TRAP ) {
            cprintf( " FLAG_RET_TRAP" );
        }
        cprintf( CRLF "  relocs=%04xh, start_para=%04xh, code_handle=%04xh"CRLF,
            ovl->relocs, ovl->start_para, ovl->code_handle );
        fn_off = ovl->fname & ~OVE_EXE_FILENAME;
        cprintf( "  num_paras=%04xh, fname=%04xh(%s), disk_addr=%08lxh" CRLF,
            ovl->num_paras, ovl->fname, (char __far *)&__OVLTAB__ + fn_off,
            ovl->disk_addr );
        if( ovl->flags_anc & FLAG_RET_TRAP ) {
            ret_trap_ptr        rt;
            unsigned_16         ret;
            unsigned_16 __far   *stk_ptr;
#ifdef OVL_MULTITHREAD
            int                 i;
#endif

            rt = MK_FP( ovl->code_handle, 0 );
            cprintf( "  return trap:" CRLF );
#ifdef OVL_MULTITHREAD
            for( i = 0; rt->traps[i].stack_trap != 0; ++i ) {
                cprintf( "  Thread=%d", i );
                cprintf( "    ret_offset=%04xh, stack_trap=%04xh" CRLF,
                    rt->traps[i].ret_offset, rt->traps[i].stack_trap );
                cprintf( "    ret_list=%04xh", rt->traps[i].ret_list );
                for( ret = rt->traps[i].ret_list; ret != 0; ret = *stk_ptr ) {
                    stk_ptr = MK_FP( FP_SEG( &stk_ptr ), ret + 4 );
                    cprintf( ", %04xh", *stk_ptr );
                }
            }
#else
            cprintf( "    ret_offset=%04xh, stack_trap=%04xh" CRLF,
                rt->ret_offset, rt->stack_trap );
            cprintf( "    ret_list=%04xh", rt->ret_list );
            for( ret = rt->ret_list; ret != 0; ret = *stk_ptr ) {
                stk_ptr = MK_FP( FP_SEG( &stk_ptr ), ret + 4 );
                cprintf( ", %04xh", *stk_ptr );
            }
#endif
            cprintf( CRLF );
        }
        cprintf( CRLF );
    }
}
#endif
