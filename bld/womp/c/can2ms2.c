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
#include "msdbg.h"
#include "canaddr.h"
#include "cansymb.h"
#include "cantype.h"
#include "namemgr.h"
#include "genutil.h"
#include "memutil.h"
#include "myassert.h"
#include "canmisc.h"
#include "pcobj.h"
#include "fixup.h"
#include "objprs.h"
#include "objrec.h"

/*
    We keep a pointer to the obj_rec for our segdef so we can set seg_length
    and set the ledata records appropriately.
*/
STATIC obj_rec      *mySegdef;

/*
    blockIndent is used to represent the current block "indentation".  Each
    SYMB_BLOCK increments it, and each SYMB_BLOCK_END decrements it.  If
    blockIndent == 0 then we are outside all procedures. == 1 means we are in
    a procedure, but not inside any internal blocks of the procedure.
*/
STATIC uint         blockIndent;

/*
    Only procedure-style blocks have types which indicate them to be 386 or
    not.  We save that info here for inner blocks.
    block386 == MS_SYM_386_FLAG (0x80) if in a 386 block.
    block386 == 0 otherwise

    For Metaware it doesn't matter if block386 is set or not, we'll write
    everything using Metaware 32-bit extensions.  newRec() ensures the high
    bit is not set on Metaware records.
*/
STATIC uint_8       block386;

/*
    A canaddr fixup for SYMB_CHANGE_SEG can be any arbitrary style of
    fixup.  Microsoft debugging information only supports BASE fixups attached
    to their equivalent of SYMB_CHANGE_SEG.  For POINTER (and POINTER386) fixups
    we must break the fixup apart, and output an OFFSET (or OFFSET386) fixup
    for every single offset field affected.

    codeOffset is set first in symbChangeSeg to the data from the canaddr for
    the segment change.  Then it is adjusted within each block we enter and
    leave.

    offsetFixup is a copy of the split up fixup from the SYMB_CHANGE_SEG.
    if offsetFixup is NULL then we don't apply any fixup to offsets in blocks
    and procedures.
*/
STATIC uint_32      codeOffset;
STATIC fixup        *offsetFixup;

/*
    Microsoft can't do anything with parm locations or return value locations,
    so we must skip them.
*/
STATIC int          skipParms;
STATIC int          skipALocal;

/*
    myRecBuf is a 256 byte buffer for building Microsoft symbolic records.
    myRecPtr points into myRecBuf at the next write position.
    waitFixups are fixups attached to data in myRecBuf with their loc_offsets
        set to the offset from beginning of myRecBuf.
    wait386Fixups indicates there are 386-style fixups waiting
*/
#define MAX_REC     257
STATIC uint_8       *myRecBuf;
STATIC uint_8       *myRecPtr;
STATIC fixup        *waitFixups;
STATIC int          wait386Fixups;

/*
    ledataBuf is a 1024 byte buffer for building LEDATAs.
    ledataOffset is the offset of the next byte to be added to ledataBuf.
    doneFixups are fixups attached to data in ledataBuf with their loc_offsets
        set to the offset from beginning of ledataBuf.
    done386Fixups there are 386-style fixups done
    writeLEOffset is the offset field for the next LEDATA.  i.e., it is the
        current size of the $$SYMBOLS segment.
*/
#define MAX_LEDATA  1024
STATIC uint_8       *ledataBuf;
STATIC size_t       ledataOffset;
STATIC fixup        *doneFixups;
STATIC int          done386Fixups;
STATIC uint_16      writeLEOffset;

STATIC void setupObjIO( void ) {

    myRecBuf = MemAlloc( MAX_REC );
    ledataBuf = MemAlloc( MAX_LEDATA );
    ledataOffset = 0;
    writeLEOffset = 0;
    doneFixups = NULL;
    waitFixups = NULL;
    offsetFixup = NULL;
    mySegdef = Can2MsSegdef( (uint_8 *)MS_DDSYMBOLS MS_DEBSYM,
        MS_DDSYMBOLS_LEN + MS_DEBSYM_LEN );
}

STATIC void flushLedata( void ) {

    uint_16 length;
    obj_rec *ledata;
    obj_rec *fixups;

    if( ledataOffset > 0 ) {
        ledata = Can2MsRec( CMD_LEDATA );
        ledata->d.ledata.idx = mySegdef->d.segdef.idx;
        ledata->d.ledata.offset = writeLEOffset;
        length = ledataOffset;
        ObjAllocData( ledata, length );
        ObjPut( ledata, ledataBuf, length );
        ObjTruncRec( ledata );
        ObjRSeek( ledata, 0 );  /* so filters can find the data */

            /* update state */
        ledataOffset = 0;
        writeLEOffset += length;

        if( doneFixups != NULL ) {
            fixups = Can2MsRec( CMD_FIXUP );
            fixups->d.fixup.data_rec = ledata;
            fixups->d.fixup.fixup = doneFixups;
            fixups->is_32 = done386Fixups;
            doneFixups = NULL;
            done386Fixups = 0;
        }
    }
}

STATIC void finishObjIO( void ) {

    flushLedata();
/**/myassert( waitFixups == NULL );
/**/myassert( doneFixups == NULL );
    mySegdef->d.segdef.seg_length = writeLEOffset;
    MemFree( ledataBuf );
    MemFree( myRecBuf );
    if( offsetFixup != NULL ) {
        FixKill( offsetFixup );
    }
}


/*
    newRec, put8, put16, putName, putAddr, and endRec are used
    to create, build, and finish a Microsoft symbolic record.
*/

STATIC void newRec( uint_8 typ ) {

    uint_8  *p;

    p = myRecBuf;
    *p++ = 0;       /* len - will be corrected in endRec */
    if( Can2MSMetaware ) {
        *p++ = typ & ~0x80;
    } else {
        *p++ = typ;     /* typ */
    }
    myRecPtr = p;
}

STATIC void put8( uint_8 byte ) {

    *myRecPtr++ = byte;
/**/myassert( myRecPtr - myRecBuf < MAX_REC );
}

STATIC void put16( uint_16 word ) {

    WriteU16( myRecPtr, word );
    myRecPtr += 2;
/**/myassert( myRecPtr - myRecBuf < MAX_REC );
}

STATIC void put32( uint_32 dword ) {

    WriteU32( myRecPtr, dword );
    myRecPtr += 4;
/**/myassert( myRecPtr - myRecBuf < MAX_REC );
}

STATIC void putEither( uint_32 dword ) {

    if( block386 || Can2MSMetaware ) {
        put32( dword );
    } else {
        put16( dword );
    }
}

STATIC void putMetaware( uint_32 dword ) {

    if( Can2MSMetaware ) {
        put32( dword );
    } else {
        put16( dword );
    }
}

STATIC void putName( name_handle hdl ) {

    const char  *p;
    size_t      len;

    if( hdl == NAME_NULL ) {
        put8( 0 );
    } else {
        p = NameGet( hdl );
        len = strlen( p );
        if( len > MAX_REC - 1 - ( myRecPtr - myRecBuf ) ) {
            len = MAX_REC - 1 - ( myRecPtr - myRecBuf );
            PrtMsg( WRN | MSG_NAME_TOO_LONG, p );
        }
/**/    myassert( len <= 255 );
        *myRecPtr++ = len;
        memcpy( myRecPtr, p, len );
        myRecPtr += len;
/**/    myassert( myRecPtr - myRecBuf < MAX_REC );
    }
}

STATIC void putAddr( fixup *fix, uint_8 *data, size_t len ) {

/**/myassert( data != NULL && fix != NULL );
    fix->loc_offset = myRecPtr - myRecBuf;
    fix->next = waitFixups;
    waitFixups = fix;
    switch( fix->loc_method ) {
    case FIX_POINTER386:
    case FIX_OFFSET386:
        wait386Fixups = 1;
        break;
    }
    memcpy( myRecPtr, data, len );
    myRecPtr += len;
/**/myassert( myRecPtr - myRecBuf < MAX_REC );
}

STATIC void endRec( void ) {

    size_t  len;
    uint_16 fudge;
    fixup   *cur;
    fixup   *next;

    len = myRecPtr - myRecBuf;
/**/myassert( len <= 256 );
    if( ledataOffset + len > MAX_LEDATA ) {
        flushLedata();
    }
/**/myassert( ledataOffset + len <= MAX_LEDATA );
    myRecBuf[ 0 ] = len - 1;
    fudge = ledataOffset;
    cur = waitFixups;
    while( cur ) {
        next = cur->next;
        cur->loc_offset += fudge;
/**/    myassert( cur->loc_offset < 1024 );
        cur->next = doneFixups;
        doneFixups = cur;
        cur = next;
    }
    memcpy( ledataBuf + ledataOffset, myRecBuf, len );
    ledataOffset += len;
    waitFixups = NULL;
    done386Fixups = done386Fixups || wait386Fixups;
    wait386Fixups = 0;
}

STATIC void symbChangeSeg( symb_handle cur ) {

    addr_info *seg;
    fixup   *oldfix;
    fixup   *newfix;

/**/myassert( cur != NULL && cur->class == CANS_CHANGE_SEG );
    if( blockIndent > 0 ) {
        Fatal( MSG_CHANGE_SEG_IN_BLOCK );
    }
    seg = CanAFind( cur->d.cseg.seg );
    oldfix = seg->fixup;
/**/myassert( oldfix->self_relative == 0 ); /* no self-relative */
    /* if we had a saved fixup, we nuke it now */
    if( offsetFixup != NULL ) {
        FixKill( offsetFixup );
    }
    /* now we can begin writing the record */
    newfix = FixDup( oldfix );
    newRec( MS_SYM_CHANGE_DEF_SEG );
    switch( oldfix->loc_method ) {
    case FIX_BASE:
        /* great! a simple base fixup, we're done */
        codeOffset = 0UL;
        putAddr( newfix, seg->data, 2 );
        offsetFixup = NULL;
        break;

    case FIX_POINTER:
        /* not so great, this is a pointer fixup and we can only do seg
           fixups */
        codeOffset = (uint_32)ReadU16( seg->data );
        offsetFixup = FixDup( newfix );
        offsetFixup->loc_method = FIX_OFFSET;
        newfix->loc_method = FIX_BASE;
        putAddr( newfix, seg->data + 2, 2 );
        break;

    case FIX_POINTER386:
        /* not so great, this is a pointer fixup and we can only do seg
           fixups */
        codeOffset = ReadU32( seg->data );
        offsetFixup = FixDup( newfix );
        offsetFixup->loc_method = FIX_OFFSET386;
        newfix->loc_method = FIX_BASE;
        putAddr( newfix, seg->data + 4, 2 );
        break;

    default:
/**/    never_reach();
    }
    put16( 0 ); /* reserved stuff */
    endRec();
}

STATIC void symbBlock( symb_handle cur ) {

    cantype     *type;
    symb_handle walk;
    fixup       *myfix;
    uint_8      buf[ 4 ];

/**/myassert( cur != NULL && cur->class == CANS_BLOCK );
    codeOffset += cur->d.block.start_offset;
    if( offsetFixup != NULL ) {
        myfix = FixDup( offsetFixup );
        WriteU32( buf, codeOffset );
    } else {
        myfix = NULL;
    }
    skipParms = 1;
    ++blockIndent;
    if( blockIndent == 1 ) {    /* this is a procedure block */
        type = CanTFind( cur->d.nat.type_hdl );
        if( type == NULL ) {
            block386 = 0;
        } else {
/**/        myassert( type->class == CANT_PROCEDURE );
            block386 = ( type->d.proc.class & CANT_PROC_386 ) ?
                MS_SYM_386_FLAG : 0;
        }
        newRec( block386 | MS_SYM_PROCEDURE_START );
        if( myfix != NULL ) {
            putAddr( myfix, buf, ( block386 | Can2MSMetaware ) ? 4 : 2 );
        } else {
            putEither( codeOffset );
        }
        if( type == NULL ) {
            put16( 0 );
        } else {
            put16( type->extra );
        }
        /* FIXME we should warn about block sizes too large */
        putMetaware( cur->d.block.size );
        walk = cur;
        while( walk->class != CANS_PROLOG ) {
            walk = CanSFwd( walk );
/**/        myassert( walk != NULL );
        }
/**/    myassert( walk->class == CANS_PROLOG );
        put16( walk->d.prolog.size );       /* debug start */
        walk = cur->d.block.end;
/**/    myassert( walk != NULL );
        walk = CanSBwd( walk );
/**/    myassert( walk != NULL && walk->class == CANS_EPILOG );
            /* debug end */
        putMetaware( cur->d.block.size - walk->d.epilog.size );
        put16( 0 ); /* reserved */
        if( type == NULL ) {
            switch( CanMisc.memory_model ) {
            case CAN_MODEL_SMALL:
            case CAN_MODEL_COMPACT:
            case CAN_MODEL_FLAT:
            case CAN_MODEL_TINY:
                put8( MS_SYM_NEAR );    /* default to NEAR */
                break;
            default:
                put8( MS_SYM_FAR );     /* default to FAR */
            }
        } else {
            if( type->d.proc.class & CANT_PROC_FAR ) {
                put8( MS_SYM_FAR );
            } else {
                put8( MS_SYM_NEAR );
            }
        }
        putName( cur->d.nat.name_hdl );
        endRec();
        if( Can2MSMetaware ) {
            /* metaware outputs an extra block for each function scope */
            newRec( MS_SYM_BLOCK_START );
            if( myfix != NULL ) {
                putAddr( FixDup( myfix ), buf,
                    ( block386 | Can2MSMetaware ) ? 4 : 2 );
            } else {
                putEither( codeOffset );
            }
            putMetaware( cur->d.block.size );
            endRec();
        }
    } else {        /* this is an inner code block */
        newRec( block386 | MS_SYM_BLOCK_START );
        if( myfix != NULL ) {
            putAddr( myfix, buf, block386 ? 4 : 2 );
        } else {
            putEither( codeOffset );
        }
        putMetaware( cur->d.block.size );
        endRec();
    }
}

STATIC void symbBPOffset( symb_handle cur ) {

    cantype *type;

/**/myassert( cur != NULL && cur->class == CANS_BP_OFFSET );
    if( skipParms ) {
        return;
    }
    if( skipALocal ) {
        skipALocal = 0;
        return;
    }
    newRec( block386 | MS_SYM_BP_RELATIVE );
    putEither( cur->d.bpoff.offset );
    type = CanTFind( cur->d.nat.type_hdl );
    if( type == NULL ) {
        put16( 0 );
    } else {
        put16( type->extra );
    }
    putName( cur->d.nat.name_hdl );
    endRec();
}

STATIC void symbMemLoc( symb_handle cur ) {

    cantype     *type;
    fixup       *newfix;
    addr_info   *mem_loc;
    uint_8      do32;

/**/myassert( cur != NULL && cur->class == CANS_MEM_LOC );
    if( skipParms ) {
        return;
    }
    if( skipALocal ) {
        skipALocal = 0;
        return;
    }
    mem_loc = CanAFind( cur->d.memloc.mem_hdl );
    do32 = ( mem_loc->data_len == 6 ) ? MS_SYM_386_FLAG : 0;
/**/myassert( do32 || mem_loc->data_len == 4 );
    newRec( do32 | MS_SYM_LOCAL_SYM );
    newfix = FixDup( mem_loc->fixup );
/**/myassert( newfix->loc_method == FIX_POINTER ||
                newfix->loc_method == FIX_POINTER386 );
    putAddr( newfix, mem_loc->data, do32 ? 6 : 4 );
    type = CanTFind( cur->d.nat.type_hdl );
    if( type == NULL ) {
        put16( 0 );
    } else {
        put16( type->extra );
    }
    putName( cur->d.nat.name_hdl );
    endRec();
}

STATIC const uint_8 msRegMap[] = {
/*CANS_REG_AL   */  MS_REG_AL,
/*CANS_REG_AH   */  MS_REG_AH,
/*CANS_REG_BL   */  MS_REG_BL,
/*CANS_REG_BH   */  MS_REG_BH,
/*CANS_REG_CL   */  MS_REG_CL,
/*CANS_REG_CH   */  MS_REG_CH,
/*CANS_REG_DL   */  MS_REG_DL,
/*CANS_REG_DH   */  MS_REG_DH,
/*CANS_REG_AX   */  MS_REG_AX,
/*CANS_REG_BX   */  MS_REG_BX,
/*CANS_REG_CX   */  MS_REG_CX,
/*CANS_REG_DX   */  MS_REG_DX,
/*CANS_REG_SI   */  MS_REG_SI,
/*CANS_REG_DI   */  MS_REG_DI,
/*CANS_REG_BP   */  MS_REG_BP,
/*CANS_REG_SP   */  MS_REG_SP,
/*CANS_REG_CS   */  MS_REG_CS,
/*CANS_REG_SS   */  MS_REG_SS,
/*CANS_REG_DS   */  MS_REG_DS,
/*CANS_REG_ES   */  MS_REG_ES,
/*CANS_REG_ST0  */  MS_REG_ST0,
/*CANS_REG_ST1  */  MS_REG_ST1,
/*CANS_REG_ST2  */  MS_REG_ST2,
/*CANS_REG_ST3  */  MS_REG_ST3,
/*CANS_REG_ST4  */  MS_REG_ST4,
/*CANS_REG_ST5  */  MS_REG_ST5,
/*CANS_REG_ST6  */  MS_REG_ST6,
/*CANS_REG_ST7  */  MS_REG_ST7,
/*CANS_REG_EAX  */  MS_REG_EAX,
/*CANS_REG_EBX  */  MS_REG_EBX,
/*CANS_REG_ECX  */  MS_REG_ECX,
/*CANS_REG_EDX  */  MS_REG_EDX,
/*CANS_REG_ESI  */  MS_REG_ESI,
/*CANS_REG_EDI  */  MS_REG_EDI,
/*CANS_REG_EBP  */  MS_REG_EBP,
/*CANS_REG_ESP  */  MS_REG_ESP,
/*CANS_REG_FS   */  MS_REG_FS,
/*CANS_REG_GS   */  MS_REG_GS,
/*CANS_REG_FLAGS*/  MS_REG_FLAGS
};

STATIC uint_8 canReg2MsReg( register_type can ) {

    if( can > CANS_REG_FLAGS ) {
        PrtMsg( WRN|MSG_UNK_REGISTER );
        return( MS_REG_AX );
    }
    if( Can2MSMetaware && can >= CANS_REG_EAX && can <= CANS_REG_ESP ) {
        can -= CANS_REG_EAX - CANS_REG_AX;
    }
    return( msRegMap[ can ] );
}

STATIC void symbRegister( symb_handle cur ) {

    cantype         *type;
    uint_8          ms_reg;
    register_type   *reg;

/**/myassert( cur != NULL && cur->class == CANS_REGISTER );
    if( skipParms ) {
        return;
    }
    if( skipALocal ) {
        skipALocal = 0;
        return;
    }
    reg = cur->d.reg.reg;
    if( cur->d.reg.num_regs == 2 ) {
        if( reg[ 0 ] == CANS_REG_AX && reg[ 1 ] == CANS_REG_DX ) {
            ms_reg = MS_REG_DX_AX;
        } else if( reg[ 0 ] == CANS_REG_ES && reg[ 1 ] == CANS_REG_BX ) {
            ms_reg = MS_REG_ES_BX;
        } else {
            PrtMsg( WRN|MSG_NO_MULTI_REG );
            ms_reg = MS_REG_DX_AX;
        }
    } else if( cur->d.reg.num_regs > 1 ) {
        PrtMsg( WRN|MSG_NO_MULTI_REG );
        ms_reg = MS_REG_DX_AX;
    } else {
        ms_reg = canReg2MsReg( reg[ 0 ] );
    }
    newRec( MS_SYM_REGISTER_SYM );
    type = CanTFind( cur->d.nat.type_hdl );
    if( type == NULL ) {
        put16( 0 );
    } else {
        put16( type->extra );
    }
    put8( ms_reg );
    putName( cur->d.nat.name_hdl );
    endRec();
}

STATIC void symbProlog( symb_handle cur ) {

/**/myassert( cur != NULL && cur->class == CANS_PROLOG );
    skipParms = 0;
    skipALocal = cur->d.prolog.has_ret_val;
}

STATIC void symbEpilog( symb_handle cur ) {

/**/myassert( cur != NULL && cur->class == CANS_EPILOG );
    cur = cur;
}

STATIC void symbBlockEnd( symb_handle cur ) {

/**/myassert( cur != NULL && cur->class == CANS_BLOCK_END );
/**/myassert( blockIndent > 0 );
    cur = cur->d.end.start;
/**/myassert( cur != NULL && cur->class == CANS_BLOCK );
    codeOffset -= cur->d.block.start_offset;
    --blockIndent;
    if( blockIndent == 0 && Can2MSMetaware ) {
        /* there's an extra scope on every function */
        newRec( MS_SYM_END_RECORD );
        endRec();
    }
    newRec( block386 | MS_SYM_END_RECORD );
    endRec();
}

STATIC void symbIndReg( symb_handle cur ) {

/**/myassert( cur != NULL && cur->class == CANS_IND_REG );
    cur = cur;
    PrtMsg( WRN|MSG_UNS_IND_REG );
}

STATIC void putInitialRecs( void ) {
/********************************/

    fixup *newfix;
    uint_16 seg_fixup;

/**/myassert( ObjDGROUPIndex != 0 );
    newfix = FixNew();
    newfix->loc_method = FIX_BASE;
    newfix->self_relative = 0;
    newfix->loader_resolved = 0;
    newfix->lr.frame = FRAME_TARG;
    newfix->lr.target = TARGET_GRP;
    newfix->lr.frame_datum = 0;
    newfix->lr.target_datum = ObjDGROUPIndex;
    newfix->lr.is_secondary = 1;
    newfix->lr.target_offset = 0;

    newRec( MS_SYM_CHANGE_DEF_SEG );
    seg_fixup = 0;
    putAddr( newfix, (void *) &seg_fixup, 2 );
    put16( 0 ); /* reserved stuff */
    endRec();
}

void Can2MsS( void ) {
/******************/

    symb_handle head;
    symb_handle cur;

    head = CanSGetHead();
    if( head != NULL ) {
        setupObjIO();
        blockIndent = 0;
        block386 = CanMisc.processor >= CAN_PROC_80386 ? MS_SYM_386_FLAG : 0;
        codeOffset = 0;
        putInitialRecs();
        cur = head;
        do {
            switch( cur->class ) {
            case CANS_CHANGE_SEG:   symbChangeSeg( cur );   break;
            case CANS_BLOCK:        symbBlock( cur );       break;
            case CANS_BP_OFFSET:    symbBPOffset( cur );    break;
            case CANS_MEM_LOC:
                if( cur->d.memloc.is_static ) {
                    symbMemLoc( cur );
                }
                break;
            case CANS_REGISTER:     symbRegister( cur );    break;
            case CANS_PROLOG:       symbProlog( cur );      break;
            case CANS_EPILOG:       symbEpilog( cur );      break;
            case CANS_BLOCK_END:    symbBlockEnd( cur );    break;
            case CANS_IND_REG:      symbIndReg( cur );      break;
            default:
    /**/        never_reach();
            }
            cur = CanSFwd( cur );
        } while( cur != head );

        finishObjIO();
    }
}
