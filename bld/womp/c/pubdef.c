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


#include "womp.h"
#include "myassert.h"
#include "cantype.h"
#include "cansymb.h"
#include "canaddr.h"
#include "canmisc.h"
#include "objrec.h"
#include "objprs.h"
#include "fixup.h"
#include "queue.h"
#include "pubdef.h"

/*
    This module tries to determine the symb_handle for each pubdef.
*/

STATIC qdesc    *pubdefs;

STATIC int scanRecOffset( obj_rec *objr, int_32 offset, symb_handle symb ) {

    pubdef_data *pubdata;
    pubdef_data *pubstop;
    uint_16     num_pubs;

    num_pubs = objr->d.pubdef.num_pubs;
    pubdata = objr->d.pubdef.pubs;
    if( pubdata == NULL ) return( 0 );
    pubstop = pubdata + num_pubs;
    while( pubdata < pubstop ) {
        if( pubdata->offset == offset ) {
            pubdata->type.hdl = symb;
            switch( symb->class ) {
            case CANS_MEM_LOC:
                symb->d.memloc.is_static = 0;
                break;
            }
            return( 1 );
        }
        ++pubdata;
    }
    return( 0 );
}

STATIC void doAnAddr( addr_info *addr, symb_handle symb ) {
/*
    If this addr_handle references a pubdef, then give that pubdef the proper
    symb_handle.
*/
    uint_16     seg_idx;
    int_32      offset;
    fixup       *addr_fixup;
    obj_rec     *objr;

    addr_fixup = addr->fixup;
    if( addr_fixup->self_relative ) return; /* can't handle these */
    switch( addr_fixup->lr.frame ) {
    case F_TARG:
    case F_NONE:
    case F_GRP:
        break;
    default:
        return;     /* can't handle any other form */
    }
    switch( addr_fixup->lr.target ) {
    case T_SEGWD:
        seg_idx = addr_fixup->lr.target_datum;
        break;
    default:
        return;     /* can't handle any other form */
    }
    offset = addr_fixup->lr.target_offset;
    switch( addr_fixup->loc_method ) {
    case FIX_LO_BYTE:
    case FIX_HI_BYTE:
        return;     /* can't handle any of these */
    case FIX_OFFSET:
    case FIX_POINTER:
        offset += (int_32)ReadS16( addr->data );
        break;
    case FIX_BASE:
        break;
    case FIX_OFFSET386:
    case FIX_POINTER386:
        offset += ReadS32( addr->data );
        break;
    default:
/**/    never_reach();
    }
    /* now offset contains the offset we are looking for */
    objr = pubdefs->head;
    while( objr != NULL ) {
/**/    myassert( objr->command == CMD_PUBDEF ||
            objr->command == CMD_STATIC_PUBDEF );
        if( objr->d.pubdef.base.seg_idx == seg_idx ) {
            if( scanRecOffset( objr, offset, symb ) ) {
                return;
            }
        }
        objr = objr->next;
    }
}

STATIC void initPubs( void ) {

    obj_rec     *objr;
    pubdef_data *pub;
    pubdef_data *pub_stop;

    pubdefs = PObjPubdefQueue();
    objr = pubdefs->head;
    while( objr != NULL ) {
/**/    myassert( objr->command == CMD_PUBDEF ||
            objr->command == CMD_STATIC_PUBDEF );
        pub = objr->d.pubdef.pubs;
        pub_stop = pub + objr->d.pubdef.num_pubs;
        while( pub < pub_stop ) {
            pub->type.hdl = CANS_NULL;
            ++pub;
        }
        objr = objr->next;
    }
}

void TypePubdefs( void ) {
/**********************/

    symb_handle head;
    symb_handle cur;
    addr_info   *seg;
    addr_info   tmp;
    unsigned    block_level;
    fixup       *myfix;
    uint_32     code_offset;

    initPubs();
    if( CanMisc.symbs_present == 0 ) {
        return;
    }
    head = CanSGetHead();
    if( head == NULL ) {
        return;
    }
    seg = CANA_NULL;
    block_level = 0;
    myfix = FixNew();
    tmp.fixup = myfix;
    code_offset = 0;
    cur = head;
    do {
        switch( cur->class ) {

        case CANS_BLOCK_END:
/**/        myassert( block_level > 0 );
            --block_level;
            break;

        case CANS_CHANGE_SEG:
            seg = CanAFind( cur->d.cseg.seg );
            switch( seg->fixup->loc_method ) {
            case FIX_BASE:
                code_offset = 0UL;
                break;
            case FIX_POINTER:
                code_offset = (uint_32)ReadU16( seg->data );
                break;
            case FIX_POINTER386:
                code_offset = ReadU32( seg->data );
                break;
            default:
/**/            never_reach();
            }
            break;

        case CANS_BLOCK:
            ++block_level;
            if( block_level == 1 && seg != CANA_NULL ) {
                     /* procedure entry */
                *myfix = *seg->fixup;
                WriteU32( tmp.data, code_offset + cur->d.block.start_offset );
/**/            myassert( tmp.fixup == myfix );
                doAnAddr( &tmp, cur );
            }
            break;

        case CANS_MEM_LOC:
            doAnAddr( CanAFind( cur->d.memloc.mem_hdl ), cur );
            break;
        }
        cur = CanSFwd( cur );
    } while( cur != head );
    FixKill( myfix );
}
