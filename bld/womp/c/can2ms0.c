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
* Description:  Filters and support routines for converting canonical
*               to Microsoft.
*
****************************************************************************/


#include "watcom.h"
#include "womp.h"
#include "genutil.h"
#include "msdbg.h"
#include "objprs.h"
#include "objrec.h"
#include "pcobj.h"
#include "canmisc.h"
#include "cansymb.h"
#include "cantype.h"
#include "myassert.h"
#include "genmscmt.h"
#include "queue.h"

int             Can2MSMetaware;

STATIC uint_16  nullLname;
STATIC qdesc    dataRecs;
STATIC uint_8   cmtRecBuf[3];
STATIC int      alreadyConverted;
STATIC int      forOS2;

obj_rec *Can2MsRec( uint_8 command ) {
/**********************************/
    obj_rec *objr;

    objr = ObjNewRec( command );
    QEnqueue( &dataRecs, objr );
    return( objr );
}

obj_rec *Can2MsSegdef( uint_8 *lname_data, uint_16 lname_len ) {
/************************************************************/
/*  lname_data must be static data... see can2ms1 for an example */

    obj_rec *lnames;
    obj_rec *segdef;
    uint_16 first_idx;

    lnames = PObjNewLnames( 2 );  /* need 2 lnames */
    ObjAttachData( lnames, lname_data, lname_len );
    ObjRSeek( lnames, lname_len );
    ObjTruncRec( lnames );
    first_idx = lnames->d.lnames.first_idx;

    segdef = PObjNewSegdef();
    segdef->d.segdef.align          = SEGDEF_ALIGN_BYTE;
    segdef->d.segdef.combine        = COMB_INVALID;
    segdef->d.segdef.use_32         = Can2MSMetaware != 0;
    segdef->d.segdef.access_valid   = 0;
    segdef->d.segdef.seg_name_idx   = first_idx;
    segdef->d.segdef.class_name_idx = first_idx + 1;
    segdef->d.segdef.ovl_name_idx   = nullLname;

    return( segdef );
}

STATIC int readTheadr( obj_rec *objr, pobj_state *state ) {

/**/myassert( objr != NULL && objr->command == CMD_THEADR );
/**/myassert( state != NULL && state->pass == POBJ_READ_PASS );
    objr = objr;
    state = state;
    nullLname = 0;
    QInit( &dataRecs );
    alreadyConverted = 0;
    return( 0 );
}

STATIC int readLnames( obj_rec *objr, pobj_state *state ) {

    uint_8  len;
    uint    count;
    uint_16 posn;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LNAMES || objr->command == CMD_LLNAMES );
/**/myassert( state != NULL && state->pass == POBJ_READ_PASS );
    state = state;
    if( nullLname > 0 ) {
        return( 0 );
    }
    count = 0;
    posn = ObjRTell( objr );
    ObjRSeek( objr, 0 );
    while( !ObjEOR( objr ) ) {
        len = ObjGet8( objr );
        if( len == 0 ) {
            nullLname = objr->d.lnames.first_idx + count;
            break;
        } else {
            ObjGet( objr, len );
        }
    }
    ObjRSeek( objr, posn );
    return( 0 );    /* we don't do a thing with them */
}

STATIC void doConversion( void ) {
    obj_rec *lnames;

    /* now we convert types and symbols */
    alreadyConverted = 1;
    if( nullLname == 0 ) {
        lnames = PObjNewLnames( 1 );
        ObjAttachData( lnames, (uint_8 *)"", 1 );
        ObjRSeek( lnames, 1 );
        nullLname = lnames->d.lnames.first_idx;
    }
    if( CanMisc.processor < 3 ) {
        Can2MSMetaware = 0;
    }
    /* these guys should have PObjEnqueue'd their segdefs and lnames */
    Can2MsT();
    Can2MsS();
}

STATIC int writePubdef( obj_rec *objr, pobj_state *state ) {

    cantype     *type;
    pubdef_data *pubdata;
    pubdef_data *pubstop;

/**/myassert( objr != NULL &&
        ( objr->command == CMD_PUBDEF || objr->command == CMD_STATIC_PUBDEF ) );
/**/myassert( state != NULL && state->pass == POBJ_WRITE_PASS );
    state = state;
    if( alreadyConverted == 0 ) {
        doConversion();
    }
    /* we must place the proper MS Type index onto each PUBDEF */
    pubdata = objr->d.pubdef.pubs;
    if( pubdata != NULL ) {
        pubstop = pubdata + objr->d.pubdef.num_pubs;
        while( pubdata < pubstop ) {
            if( pubdata->type.hdl != CANS_NULL ) {
                type = CanTFind( pubdata->type.hdl->d.nat.type_hdl );
                if( type != NULL ) {
                    pubdata->type.idx = (uint_16)type->extra;
                } else {
                    pubdata->type.idx = 0;
                }
            } else {
                pubdata->type.idx = 0;
            }
            ++pubdata;
        }
    }
    return( 0 );
}

STATIC int writeComent( obj_rec *objr, pobj_state *state ) {

/**/myassert( objr != NULL && objr->command == CMD_COMENT );
/**/myassert( state != NULL && state->pass == POBJ_WRITE_PASS );
    state = state;
    if( objr->d.coment.class == CMT_MS_END_PASS_1 ) {
        if( alreadyConverted == 0 ) {
            doConversion();
        }
        if( dataRecs.head != NULL ) {
            PObjJoinQueue( &dataRecs );
        }
    }
    return( 0 );
}

STATIC const uint_8 memModels[] = {
/*CAN_MODEL_SMALL   */  'S',
/*CAN_MODEL_MEDIUM  */  'M',
/*CAN_MODEL_COMPACT */  'C',
/*CAN_MODEL_LARGE   */  'L',
/*CAN_MODEL_HUGE    */  'H',
/*CAN_MODEL_FLAT    */  'F',
/*CAN_MODEL_TINY    */  'T'
};

STATIC int writeTheadr( obj_rec *objr, pobj_state *state ) {
/*
    output some coment records after THEADR
*/
    obj_rec *work;

/**/myassert( objr != NULL && objr->command == CMD_THEADR );
/**/myassert( state != NULL && state->pass == POBJ_WRITE_PASS );
    state = state;
    objr = objr;

        /* we output the compile information record */
    work = ObjNewRec( CMD_COMENT );
    work->d.coment.attr = 0x00;
    work->d.coment.class = CMT_MS_PROC_MODEL;
    cmtRecBuf[0] = '0' + CanMisc.processor;
    if( CanMisc.memory_model > CAN_MODEL_TINY ) {
        cmtRecBuf[1] = 'S';
        PrtMsg( WRN|MSG_UNS_MEMORY_MODEL );
    } else {
        cmtRecBuf[1] = memModels[ CanMisc.memory_model ];
    }
    ObjAttachData( work, cmtRecBuf, 3 );
    if( CanMisc.optimized ) {
        cmtRecBuf[2] = 'O';
        ObjRSeek( work, 3 );
    } else {
        ObjRSeek( work, 2 );
    }
    ObjTruncRec( work );
    ObjRSeek( work, 0 );
    PObjEnqueue( work );

    return( 0 );
}

STATIC const pobj_list myFuncs[] = {
    { CMD_THEADR,       POBJ_READ_PASS,     readTheadr },
    { CMD_LNAMES,       POBJ_READ_PASS,     readLnames },
    { CMD_THEADR,       POBJ_WRITE_PASS,    writeTheadr },
    { CMD_PUBDEF,       POBJ_WRITE_PASS,    writePubdef },
    { CMD_STATIC_PUBDEF,POBJ_WRITE_PASS,    writePubdef },
    { CMD_COMENT,       POBJ_WRITE_PASS,    writeComent },
    { CMD_LLNAMES,      POBJ_READ_PASS,     readLnames }
};
#define NUM_FUNCS       ( sizeof( myFuncs ) / sizeof( pobj_list ) )

void Can2MsInit( int metaware, int os2_specific ) {
/***********************************************/

    Can2MSMetaware = metaware;
    forOS2 = os2_specific;
    GenMSCmtInit(); /* must come after us */
    PObjRegList( myFuncs, NUM_FUNCS );
}

int Can2MsOS2Flat( void )
/***********************/
{
    if( forOS2 == 0 ) {
        return( 0 );
    }
    if( CanMisc.processor < 3 ) {
        return( 0 );
    }
    if( CanMisc.memory_model == CAN_MODEL_FLAT ) {
        return( 1 );
    }
    /* req'd because of bug in C8.0; remove before C8.5 release! */
    if( CanMisc.memory_model == CAN_MODEL_SMALL ) {
        return( 1 );
    }
    return( 0 );
}

void Can2MsFini( void ) {
/*********************/

    PObjUnRegList( myFuncs, NUM_FUNCS );
    GenMSCmtFini();
}
