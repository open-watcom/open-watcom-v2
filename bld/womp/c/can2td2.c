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


#include <string.h>
#include "womp.h"
#include "turbodbg.h"
#include "queue.h"
#include "cansymb.h"
#include "cantype.h"
#include "objrec.h"
#include "genutil.h"
#include "fixup.h"
#include "canaddr.h"
#include "myassert.h"

STATIC uint_32  codeOffset;
STATIC uint_16  codeSeg;
STATIC uint     blockIndent;
STATIC int      skipALocal;
STATIC int      inParms;    /* TD_LOCAL_PARM_FLAG if in parms */

STATIC void symbChangeSeg( symb_handle symb ) {

    addr_info   *seg;
    fixup       *fix;

/**/myassert( symb != NULL && symb->class == CANS_CHANGE_SEG );
    if( blockIndent > 0 ) {
        Fatal( MSG_CHANGE_SEG_IN_BLOCK );
    }
    seg = CanAFind( symb->d.cseg.seg );
    fix = seg->fixup;
/**/myassert( fix->self_relative == 0 );  /* no self-relative */
    if( fix->lr.target != T_SEGWD ) {
        Fatal( MSG_INVALID_FIXUP );
    }
    switch( fix->loc_method ) {
    case FIX_BASE:
        codeOffset = 0;
        codeSeg = fix->lr.target_datum + (int_16)ReadU16( seg->data );
        break;
    case FIX_POINTER:
        codeSeg = fix->lr.target_datum + (int_16)ReadU16( seg->data + 2 );
        codeOffset = fix->lr.target_offset + (int_32)ReadU16( seg->data );
        break;
    case FIX_POINTER386:
        codeSeg = fix->lr.target_datum + (int_16)ReadU16( seg->data + 4 );
        codeOffset = fix->lr.target_offset + (int_32)ReadU32( seg->data );
        break;
    default:
/**/    never_reach();
    }
}

STATIC void blockBegin( uint_16 seg, uint_32 offset ) {

    obj_rec *coment;

    if( offset > 0xffff ) {
        coment = Can2TDNewRec( TD_CMT_BEGIN_SCOPE_LARGE, 6 );
        ObjPutIndex( coment, seg ); /* max 2 bytes */
        ObjPut32( coment, offset ); /* plus 4 bytes = 6 bytes */
    } else {
        coment = Can2TDNewRec( TD_CMT_BEGIN_SCOPE, 4 );
        ObjPutIndex( coment, seg ); /* max 2 bytes */
        ObjPut16( coment, offset ); /* plus 2 bytes = 4 bytes */
    }
    Can2TDEndRec( coment );
}

STATIC void symbBlock( symb_handle symb ) {

    symb_handle end;
/**/myassert( symb != NULL && symb->class == CANS_BLOCK );
/*
    This is called on each block before any PUBDEFs have been processed.
    can2td0 will set these to 1 if they are referenced by a PUBDEF.
*/
    codeOffset += symb->d.block.start_offset;
    ++blockIndent;
    if( blockIndent == 1 ) {
        symb->extra = (uint_32)codeSeg;
        end = symb->d.block.end;
/**/    myassert( end != NULL );
        end->extra = codeOffset;
    } else {
        /* is an inner block, so we tell a little lie */
        symb->extra = TD_HAS_A_PUBDEF;
    }
    blockBegin( codeSeg, codeOffset );
    inParms = TD_LOCAL_PARM_FLAG;
}

STATIC void symbBPOffset( symb_handle symb ) {

    cantype     *type;
    obj_rec     *coment;
    const char  *name;
    size_t      name_len;
    uint_16     type_idx;
    int         pasvar;

/**/myassert( symb != NULL && symb->class == CANS_BP_OFFSET );
    if( skipALocal ) {
        skipALocal = 0;
        return;
    }
    name = NameGet( symb->d.nat.name_hdl );
    name_len = strlen( name );
/**/myassert( name_len < 256 );
    pasvar = 0;
    type = CanTFind( symb->d.nat.type_hdl );
    if( type == NULL ) {
        type_idx = 0;
    } else if( type->class == CANT_POINTER &&
        ( type->d.pointr.class & CANT_PTR_DEREF ) ) {
            /* generate DEREFs as PASVAR locals */
        pasvar = 1;
        type = CanTFind( type->d.pointr.base_type );
        if( type == NULL ) {
            type_idx = 0;
        } else {
            type_idx = type->extra;
        }
    } else {
        type_idx = type->extra;
    }
    if( symb->d.bpoff.offset > 0xffff ) {
        /* 1 for name_len byte, 2 for type idx, 1 for class byte,
            4 for offset word */
        coment = Can2TDNewRec( TD_CMT_LOCAL_DEFN_LARGE, 1 + name_len + (2+1+4) );
    } else {
        /* 1 for name_len byte, 2 for type idx, 1 for class byte,
            2 for offset word */
        coment = Can2TDNewRec( TD_CMT_LOCAL_DEFN, 1 + name_len + (2+1+2) );
    }
    ObjPutName( coment, name, name_len );
    ObjPutIndex( coment, type_idx );
    ObjPut8( coment, inParms | ( pasvar ? TD_LOCAL_PASVAR : TD_LOCAL_AUTO ) );
    if( symb->d.bpoff.offset > 0xffff ) {
        ObjPut32( coment, symb->d.bpoff.offset );
    } else {
        ObjPut16( coment, symb->d.bpoff.offset );
    }
    Can2TDEndRec( coment );
}

STATIC void symbMemLoc( symb_handle symb ) {

    cantype     *type;
    uint_16     type_idx;
    size_t      name_len;
    const char  *name;
    fixup       *fix;
    addr_info   *mem_loc;
    obj_rec     *coment;
    uint_32     offset;
    uint_16     grp_idx;

/**/myassert( symb != NULL && symb->class == CANS_MEM_LOC );
    if( skipALocal ) {
        skipALocal = 0;
        return;
    }
    mem_loc = CanAFind( symb->d.memloc.mem_hdl );
    fix = mem_loc->fixup;
    switch( fix->lr.frame ) {
    case F_GRP:
        grp_idx = fix->lr.frame_datum;
        break;
    default:
        /* FIXME this heuristic is not quite accurate... it's ok for F_TARG */
        grp_idx = 0;
        break;
    }
    if( fix->lr.target != T_SEGWD ) {
        Fatal( MSG_INVALID_FIXUP );
    }
    type = CanTFind( symb->d.nat.type_hdl );
    if( type == NULL ) {
        type_idx = 0;
    } else {
        type_idx = type->extra;
    }
    name = NameGet( symb->d.nat.name_hdl );
    name_len = strlen( name );
/**/myassert( name_len < 256 );
    offset = fix->lr.target_offset;
    switch( fix->loc_method ) {
    case FIX_POINTER:
    case FIX_OFFSET:
        offset += (int_32)ReadS16( mem_loc->data );
        break;
    case FIX_POINTER386:
    case FIX_OFFSET386:
        offset += (int_32)ReadS32( mem_loc->data );
        break;
    }
    if( offset > 0xffff ) {
        /* 1 for name_len byte, 2 for type_idx, 1 for class byte, 2 for grp_idx,
            2 for seg_idx, 4 for offset word */
        coment = Can2TDNewRec( TD_CMT_LOCAL_DEFN_LARGE, name_len+(1+2+1+2+2+4));
    } else {
        /* 1 for name_len byte, 2 for type_idx, 1 for class byte, 2 for grp_idx,
            2 for seg_idx, 2 for offset word */
        coment = Can2TDNewRec( TD_CMT_LOCAL_DEFN, name_len + (1+2+1+2+2+2));
    }
    ObjPutName( coment, name, name_len );
    ObjPutIndex( coment, type_idx );
    ObjPut8( coment, inParms | TD_LOCAL_STATIC );
    ObjPutIndex( coment, grp_idx );
    ObjPutIndex( coment, fix->lr.target_datum );
    if( offset > 0xffff ) {
        ObjPut32( coment, offset );
    } else {
        ObjPut16( coment, offset );
    }
    Can2TDEndRec( coment );
}

STATIC const uint_8 tdRegs[] = {
/*CANS_REG_AL   */  TD_REG_AL,
/*CANS_REG_AH   */  TD_REG_AH,
/*CANS_REG_BL   */  TD_REG_BL,
/*CANS_REG_BH   */  TD_REG_BH,
/*CANS_REG_CL   */  TD_REG_CL,
/*CANS_REG_CH   */  TD_REG_CH,
/*CANS_REG_DL   */  TD_REG_DL,
/*CANS_REG_DH   */  TD_REG_DH,
/*CANS_REG_AX   */  TD_REG_AX,
/*CANS_REG_BX   */  TD_REG_BX,
/*CANS_REG_CX   */  TD_REG_CX,
/*CANS_REG_DX   */  TD_REG_DX,
/*CANS_REG_SI   */  TD_REG_SI,
/*CANS_REG_DI   */  TD_REG_DI,
/*CANS_REG_BP   */  TD_REG_BP,
/*CANS_REG_SP   */  TD_REG_SP,
/*CANS_REG_CS   */  TD_REG_CS,
/*CANS_REG_SS   */  TD_REG_SS,
/*CANS_REG_DS   */  TD_REG_DS,
/*CANS_REG_ES   */  TD_REG_ES,
/*CANS_REG_ST0  */  TD_REG_ST0,
/*CANS_REG_ST1  */  TD_REG_ST1,
/*CANS_REG_ST2  */  TD_REG_ST2,
/*CANS_REG_ST3  */  TD_REG_ST3,
/*CANS_REG_ST4  */  TD_REG_ST4,
/*CANS_REG_ST5  */  TD_REG_ST5,
/*CANS_REG_ST6  */  TD_REG_ST6,
/*CANS_REG_ST7  */  TD_REG_ST7,
/*CANS_REG_EAX  */  TD_REG_EAX,
/*CANS_REG_EBX  */  TD_REG_EBX,
/*CANS_REG_ECX  */  TD_REG_ECX,
/*CANS_REG_EDX  */  TD_REG_EDX,
/*CANS_REG_ESI  */  TD_REG_ESI,
/*CANS_REG_EDI  */  TD_REG_EDI,
/*CANS_REG_EBP  */  TD_REG_EBP,
/*CANS_REG_ESP  */  TD_REG_ESP,
/*CANS_REG_FS   */  TD_REG_FS,
/*CANS_REG_GS   */  TD_REG_GS
};

STATIC uint_8 multiReg( register_type canreg ) {

    switch( canreg ) {
    case CANS_REG_AX:   return( TD_REG_MULTI_AX );
    case CANS_REG_CX:   return( TD_REG_MULTI_CX );
    case CANS_REG_DX:   return( TD_REG_MULTI_DX );
    case CANS_REG_BX:   return( TD_REG_MULTI_BX );
    case CANS_REG_SI:   return( TD_REG_MULTI_SI );
    case CANS_REG_DI:   return( TD_REG_MULTI_DI );
    case CANS_REG_ES:   return( TD_REG_MULTI_ES );
    case CANS_REG_DS:   return( TD_REG_MULTI_DS );
    }
    PrtMsg( WRN|MSG_UNK_REGISTER );
    return( TD_REG_AX );
}

STATIC void symbRegister( symb_handle symb ) {

    obj_rec         *coment;
    cantype         *type;
    uint_16         type_idx;
    const char      *name;
    size_t          name_len;
    uint_8          treg;
    register_type   *reg;

/**/myassert( symb != NULL && symb->class == CANS_REGISTER );
    if( skipALocal ) {
        skipALocal = 0;
        return;
    }
    reg = symb->d.reg.reg;
    if( symb->d.reg.num_regs == 4 ) {
        if( reg[ 0 ] == CANS_REG_AX &&
            reg[ 1 ] == CANS_REG_BX &&
            reg[ 2 ] == CANS_REG_CX &&
            reg[ 3 ] == CANS_REG_DX ) {
            treg = TD_REG_MULTI_AXBXCXDX;
        } else {
            Fatal( MSG_IMPOSSIBLE_REG );
        }
    } else if( symb->d.reg.num_regs == 2 ) {
        treg = TD_REG_MULTI
            | ( multiReg( reg[ 0 ] ) << TD_REG_MULTI_LO_SHIFT )
            | ( multiReg( reg[ 1 ] ) << TD_REG_MULTI_HI_SHIFT );
    } else if( symb->d.reg.num_regs > 1 ) {
        Fatal( MSG_IMPOSSIBLE_REG );
    } else if( reg[ 0 ] == CANS_REG_FLAGS ) {
        Fatal( MSG_IMPOSSIBLE_REG );
    } else {
        treg = tdRegs[ reg[ 0 ] ];
    }
    type = CanTFind( symb->d.nat.type_hdl );
    if( type == NULL ) {
        type_idx = 0;
    } else {
        type_idx = type->extra;
    }
    name = NameGet( symb->d.nat.name_hdl );
    name_len = strlen( name );
/**/myassert( name_len < 256 );
    /* 1 for name_len, 2 for type_idx, 1 for class byte, 1 for treg byte */
    coment = Can2TDNewRec( TD_CMT_LOCAL_DEFN, name_len + ( 1 + 2 + 1 + 1 ) );
    ObjPutName( coment, name, name_len );
    ObjPutIndex( coment, type_idx );
    ObjPut8( coment, inParms | TD_LOCAL_REGISTER );
    ObjPut8( coment, treg );
    Can2TDEndRec( coment );
}

STATIC void symbProlog( symb_handle symb ) {

/**/myassert( symb != NULL && symb->class == CANS_PROLOG );
/**/myassert( blockIndent > 0 );
    skipALocal = symb->d.prolog.has_ret_val;
    if( blockIndent > 1 ) {
        return;   /* not a procedure block */
    }
    blockBegin( codeSeg, codeOffset + symb->d.prolog.size );
    inParms = 0;
}

STATIC void blockEnd( uint_32 offset ) {
    obj_rec *coment;

    if( offset > 0xffff ) {
        coment = Can2TDNewRec( TD_CMT_LARGE_END_SCOPE, 4 );
        ObjPut32( coment, offset );
    } else {
        coment = Can2TDNewRec( TD_CMT_END_SCOPE, 2 );
        ObjPut16( coment, offset );
    }
    Can2TDEndRec( coment );
}

STATIC void symbEpilog( symb_handle symb ) {

    symb_handle block_end;
    symb_handle block_start;

/**/myassert( symb != NULL && symb->class == CANS_EPILOG );
/**/myassert( blockIndent > 0 );
    if( blockIndent > 1 ) {
        return;     /* not a procedure epilog */
    }
    block_end = CanSFwd( symb );
/**/myassert( block_end->class == CANS_BLOCK_END );
    block_start = block_end->d.end.start;
    blockEnd( codeOffset + block_start->d.block.size - symb->d.epilog.size );
}

STATIC void symbBlockEnd( symb_handle symb ) {

    symb_handle start;

/**/myassert( symb != NULL && symb->class == CANS_BLOCK_END );
/**/myassert( blockIndent > 0 );
    start = symb->d.end.start;
    blockEnd( codeOffset + start->d.block.size );
    codeOffset -= start->d.block.start_offset;
    --blockIndent;
}

STATIC void symbIndReg( symb_handle symb ) {

/**/myassert( symb != NULL && symb->class == CANS_IND_REG );
    symb = symb;
    PrtMsg( WRN|MSG_UNS_IND_REG );
}

void Can2TDS( void ) {
/******************/

    symb_handle head;
    symb_handle cur;

    head = CanSGetHead();
    if( head != NULL ) {
        codeOffset = 0;
        codeSeg = 0;
        blockIndent = 0;
        skipALocal = 0;
        inParms = 0;
        /*
            Slight gotcha in the Turbo debugging format... static locals and
            static functions must appear together after all the scope records
            and so on... Well, I'm unsure if they must appear after all or if
            they must just appear together.

            We simply scan ahead over the locals and pretend our head is in
            a different spot.  (Rings are fun :)
        */
        cur = head;
        do {
            if( cur->class == CANS_BLOCK ) break;
            if( cur->class == CANS_CHANGE_SEG ) {
                symbChangeSeg( cur );
            }
            cur = CanSFwd( cur );
        } while( cur != head ); /* in case it is a module with no functions */
        head = cur;
        do {
            switch( cur->class ) {
            case CANS_CHANGE_SEG:   symbChangeSeg( cur );   break;
            case CANS_BLOCK:        symbBlock( cur );       break;
            case CANS_BP_OFFSET:    symbBPOffset( cur );    break;
            case CANS_MEM_LOC:      symbMemLoc( cur );      break;
            case CANS_REGISTER:     symbRegister( cur );    break;
            case CANS_PROLOG:       symbProlog( cur );      break;
            case CANS_EPILOG:       symbEpilog( cur );      break;
            case CANS_BLOCK_END:    symbBlockEnd( cur );    break;
            case CANS_IND_REG:      symbIndReg( cur );      break;
            default:
/**/            never_reach();
            }
            cur = CanSFwd( cur );
        } while( cur != head );
    }
}


void Can2TDStatic( void ) {
/***********************/

    symb_handle head;
    symb_handle cur;
    symb_handle end;
    obj_rec     *coment;
    uint_8      tbyte;
    size_t      name_len;
    const char  *name;
    uint_32     offset;
    cantype     *type;

    head = CanSGetHead();
    if( head != NULL ) {
        cur = head;
        do {
            switch( cur->class ) {
            case CANS_BLOCK:
                if( cur->extra & TD_HAS_A_PUBDEF ) {
                    break;  /* has a PUBDEF for it already */
                }
                /* otherwise it is a static fcn, so we generate a local for it*/
                tbyte = Can2TDBPOffset( cur );
                type = CanTFind( cur->d.nat.type_hdl );
                name = NameGet( cur->d.nat.name_hdl );
                name_len = strlen( name );
/**/            myassert( name_len < 256 );
                end = cur->d.block.end;
/**/            myassert( end != NULL );
                offset = end->extra;        /* saved here by symbBlock() */
                if( offset > 0xffff ) {
                    coment = Can2TDNewRec( TD_CMT_LOCAL_DEFN_LARGE,
                        name_len + ( 1 + 2 + 1 + 1 + 2 + 4 ) );
                } else {
                    coment = Can2TDNewRec( TD_CMT_LOCAL_DEFN,
                        name_len + ( 1 + 2 + 1 + 1 + 2 + 2 ) );
                }
                ObjPutName( coment, name, name_len );
                if( type != NULL ) {
                    ObjPutIndex( coment, type->extra );
                } else {
                    ObjPutIndex( coment, 0 );
                }
                if( tbyte != 0 ) {
                    ObjPut8( coment, tbyte );
                }
                ObjPut8( coment, TD_LOCAL_STATIC );
                /* ObjPutIndex( coment, 0 );   grp_idx not reqd?? */
                ObjPutIndex( coment, (uint_16)cur->extra );
                if( offset > 0xffff ) {
                    ObjPut32( coment, offset );
                } else {
                    ObjPut16( coment, offset );
                }
                Can2TDEndRec( coment );
                break;
            }
            cur = CanSFwd( cur );
        } while( cur != head );
    }
}
