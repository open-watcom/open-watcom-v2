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
#include "genutil.h"
#include "memutil.h"
#include "turbodbg.h"
#include "objprs.h"
#include "objrec.h"
#include "pcobj.h"
#include "queue.h"
#include "canmisc.h"
#include "cantype.h"
#include "cansymb.h"
#include "myassert.h"
#include "carve.h"

STATIC qdesc    dataRecs;
STATIC uint_8   compileParmsBuf[2];

obj_rec *Can2TDNewRec( uint_8 cmt_class, uint_16 size ) {
/*****************************************************/
    obj_rec *coment;

    coment = ObjNewRec( CMD_COMENT );
    QEnqueue( &dataRecs, coment );
    coment->d.coment.attr = TD_CMT_ATTR;
    coment->d.coment.class = cmt_class;
    if( size > 0 ) {
        ObjAllocData( coment, size );
        ObjRSeek( coment, 0 );
    }
    return( coment );
}

void Can2TDEndRec( obj_rec *coment ) {
/**********************************/

/**/myassert( coment != NULL && coment->command == CMD_COMENT );
    ObjTruncRec( coment );
    ObjRSeek( coment, 0 );
}

STATIC int writeTheadr( obj_rec *objr, pobj_state *state ) {

    obj_rec *coment;
    uint_8  name_len;
    char    *name;
    uint_16 save;

/**/myassert( objr != NULL && objr->command == CMD_THEADR );
/**/myassert( state != NULL && state->pass == POBJ_WRITE_PASS );
    state = state;

    /* we output the default source file information */
    save = ObjRTell( objr );
    name_len = ObjGet8( objr );
    name = (char *)ObjGet( objr, name_len );
    ObjRSeek( objr, save );
    coment = ObjNewRec( CMD_COMENT );
    coment->d.coment.attr = TD_CMT_ATTR;
    coment->d.coment.class = TD_CMT_SELECT_SOURCE;
    /* 1 byte for name_len, 1 for index, 4 for dword date/time stamp */
    ObjAllocData( coment, name_len + ( 1 + 1 + 4 ) );
    ObjRSeek( coment, 0 );
    ObjPut8( coment, 0 );   /* an index of 0 */
    ObjPutName( coment, name, name_len );
    ObjPut32( coment, 0 );      /* FIXME should get a proper date/time stamp! */
    ObjTruncRec( coment );
    ObjRSeek( coment, 0 );
    PObjEnqueue( coment );

    /* There is no Fortran style for Turbo... */
    if( CanMisc.src_language == CAN_LANG_FORTRAN ) {
        CanMisc.src_language = CAN_LANG_ASM;
    }

    /* now we do the debug conversion */
    QInit( &dataRecs );
    Can2TDT();      /* save the records for later */
    Can2TDS();

    return( 0 );
}

STATIC int writeLastDefn( obj_rec *objr, pobj_state *state ) {

    uint_8  lang;
    uint_8  bits;
    obj_rec *coment;

/**/myassert( state != NULL && state->pass == POBJ_WRITE_PASS );
    objr = objr;
    state = state;
    /* now we output the src language information */
    switch( CanMisc.src_language ) {
    case CAN_LANG_ASM:
        lang = TD_CP_LANG_ASSEMBLY;
        bits = 0; /* FIXME need a better way of setting UNDERBARS_USED */
        break;
    case CAN_LANG_C:
        lang = TD_CP_LANG_C;
        bits = TD_CP_UNDERBARS_USED;
        break;
#if 0
    case CAN_LANG_FORTRAN:  /* no need to do this cause of writeTheadr */
        break;
#endif
    case CAN_LANG_PASCAL:
        lang = TD_CP_LANG_PASCAL;
        bits = TD_CP_UNDERBARS_USED;
        break;
    case CAN_LANG_BASIC:
        lang = TD_CP_LANG_BASIC;
        bits = TD_CP_UNDERBARS_USED;
        break;
    default:
/**/    never_reach();
    }
    if( CanMisc.processor < CAN_PROC_80386 ) {
        switch( CanMisc.memory_model ) {
        case CAN_MODEL_TINY:    bits |= TD_CP_MEMORY_TINY;      break;
        case CAN_MODEL_SMALL:   bits |= TD_CP_MEMORY_SMALL;     break;
        case CAN_MODEL_MEDIUM:  bits |= TD_CP_MEMORY_MEDIUM;    break;
        case CAN_MODEL_COMPACT: bits |= TD_CP_MEMORY_COMPACT;   break;
        case CAN_MODEL_LARGE:   bits |= TD_CP_MEMORY_LARGE;     break;
        case CAN_MODEL_HUGE:    bits |= TD_CP_MEMORY_HUGE;      break;
        default:
            PrtMsg( WRN|MSG_UNS_MEMORY_MODEL );
            bits |= TD_CP_MEMORY_SMALL;
            break;
        }
    } else {
        switch( CanMisc.memory_model ) {
        case CAN_MODEL_SMALL:   bits |= TD_CP_MEMORY_386_SMALL;     break;
        case CAN_MODEL_MEDIUM:  bits |= TD_CP_MEMORY_386_MEDIUM;    break;
        case CAN_MODEL_COMPACT: bits |= TD_CP_MEMORY_386_COMPACT;   break;
        case CAN_MODEL_LARGE:   bits |= TD_CP_MEMORY_386_LARGE;     break;
        default:
            PrtMsg( WRN|MSG_UNS_MEMORY_MODEL );
            bits |= TD_CP_MEMORY_386_SMALL;
            break;
        }
    }
    compileParmsBuf[ 0 ] = lang;
    compileParmsBuf[ 1 ] = bits;
    coment = ObjNewRec( CMD_COMENT );
    coment->d.coment.attr = TD_CMT_ATTR;
    coment->d.coment.class = TD_CMT_COMPILE_PARMS;
    ObjAttachData( coment, compileParmsBuf, 2 );
    ObjRSeek( coment, 0 );
    PObjEnqueue( coment );

    return( 0 );
}


STATIC int writeComent( obj_rec *objr, pobj_state *state ) {

/**/myassert( objr != NULL && objr->command == CMD_COMENT );
/**/myassert( state != NULL && state->pass == POBJ_WRITE_PASS );
    state = state;
    if( objr->d.coment.class == TD_CMT_COMPILE_PARMS ) {
        Can2TDTypeDef();
        Can2TDStatic();
        if( dataRecs.head != NULL ) {
            PObjJoinQueue( &dataRecs );
        }
    }
    return( 0 );
}

uint_8 Can2TDBPOffset( symb_handle symb ) {
/***************************************/

    symb_handle walk;

/**/myassert( symb != NULL && symb->class == CANS_BLOCK );
    walk = CanSFwd( symb );
/**/myassert( walk != NULL );
    while( walk->class != CANS_PROLOG ) {
        walk = CanSFwd( walk );
/**/    myassert( walk != NULL );
    }
    if( walk->d.prolog.has_ret_addr ) {
        return( TD_PUB_VALID_BP |
            ( ( walk->d.prolog.ret_addr_offset >> 1 ) <<
                TD_PUB_RET_ADDR_SHIFT ) );
    }
    return( 0 );
}


STATIC int writePubdef( obj_rec *objr, pobj_state *state ) {
/*
    We will break up pubdefs into individual records and spit out the
    COMENT records with the turbo type idx.  We don't bother breaking up
    the pubdef_data array in objr->d.pubdef.pubs... we just set the
    don't free pubs flag in the records that reference the old array.
*/
    obj_rec     *coment;
    cantype     *type;
    symb_handle symb;
    pubdef_data *pub;
    pubdef_data *pub_stop;
    uint_16     tidx;
    uint_8      tbyte;
    uint_8      command;
    struct base_info base;

/**/myassert( objr != NULL && (
        objr->command == CMD_PUBDEF || objr->command == CMD_STATIC_PUBDEF ) );
/**/myassert( state != NULL && state->pass == POBJ_WRITE_PASS );
    state = state;
    if( objr->d.pubdef.processed ) return( 0 );
    command = objr->command;
    base = objr->d.pubdef.base;
    pub_stop = objr->d.pubdef.pubs;
    if( objr->d.pubdef.num_pubs == 0 ) return( -1 );    /* huh?? no pubs? */
    pub = pub_stop + objr->d.pubdef.num_pubs - 1;
    for(;;) {
        symb = pub->type.hdl;
        if( symb != CANS_NULL ) {
            type = CanTFind( symb->d.nat.type_hdl );
            tidx = ( type != NULL ) ? type->extra : 0;
            switch( symb->class ) {
            case CANS_BLOCK:
            /*
                This is used by Can2TDStatic to determine whether a fcn is
                static or not.
            */
                symb->extra |= TD_HAS_A_PUBDEF;
                tbyte = Can2TDBPOffset( symb );
                break;
            default:
                tbyte = 0;
                break;
            }
            coment = ObjNewRec( CMD_COMENT );
            coment->d.coment.attr = TD_CMT_ATTR;
            coment->d.coment.class = TD_CMT_PUBDEF;
            ObjAllocData( coment, 3 );
            ObjRSeek( coment, 0 );
            ObjPutIndex( coment, tidx );
            ObjPut8( coment, tbyte );
            ObjTruncRec( coment );
            ObjRSeek( coment, 0 );
            PObjEnqueue( coment );
        }
        objr->d.pubdef.num_pubs = 1;
        objr->d.pubdef.pubs = pub;
        objr->d.pubdef.free_pubs = 0;   /* can't free the pubs */
        objr->d.pubdef.processed = 1;
        pub->type.idx = 0;
        if( pub == pub_stop ) break;
        objr = ObjNewRec( command );    /* for next time around loop */
        objr->d.pubdef.base = base;     /* setup proper base */
        PObjEnqueue( objr );
        --pub;
    }
    objr->d.pubdef.free_pubs = 1; /* last one along can free the pubs */
    return( 0 );
}

STATIC const pobj_list myFuncs[] = {
    { CMD_THEADR,       POBJ_WRITE_PASS, writeTheadr },
    { CMD_LAST_DEFN,    POBJ_WRITE_PASS, writeLastDefn },
    { CMD_COMENT,       POBJ_WRITE_PASS, writeComent },
    { CMD_PUBDEF,       POBJ_WRITE_PASS, writePubdef },
    { CMD_STATIC_PUBDEF,POBJ_WRITE_PASS, writePubdef }
};
#define NUM_FUNCS       ( sizeof( myFuncs ) / sizeof( pobj_list ) )

void Can2TDInit( void ) {
/*********************/
    PObjRegList( myFuncs, NUM_FUNCS );
}

void Can2TDFini( void ) {
/*********************/
    PObjUnRegList( myFuncs, NUM_FUNCS );
}


