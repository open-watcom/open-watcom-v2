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
#include "objprs.h"
#include "msdbg.h"
#include "genmscmt.h"
#include "myassert.h"

STATIC int      registered;

STATIC int doDefn( obj_rec *objr, pobj_state *state ) {

    obj_rec *lnames;
    obj_rec *grpdef;
    obj_rec *coment;

/**/myassert( objr != NULL );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    state = state;
    objr = objr;
    if( Can2MsOS2Flat() && ObjFLATIndex == 0 ) {
        /* add FLAT null GRPDEF for OS/2 2.0 */
        lnames = PObjNewLnames( 1 );
        ObjAttachData( lnames, (uint_8 *) MS_FLAT, MS_FLAT_LEN );
        ObjRSeek( lnames, MS_FLAT_LEN );
        ObjTruncRec( lnames );
        grpdef = PObjNewGrpdef();
        ObjFLATIndex = grpdef->d.grpdef.idx;
        ObjAllocData( grpdef, 4 );
        ObjRSeek( grpdef, 0 );
        ObjPutIndex( grpdef, lnames->d.lnames.first_idx );
        ObjTruncRec( grpdef );
        PObjEnqueue( lnames );
        PObjEnqueue( grpdef );
    }
    coment = ObjNewRec( CMD_COMENT );
    coment->d.coment.attr = 0x00;
    coment->d.coment.class = CMT_MS_END_PASS_1;
    ObjAttachData( coment, (uint_8 *) "\x1", 1 );
    ObjRSeek( coment, 0 );
    PObjEnqueue( coment );
    return( 0 );
}

STATIC int doTheadr( obj_rec *objr, pobj_state *state ) {

    obj_rec *coment;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_THEADR );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    state = state;
    objr = objr;
    coment = ObjNewRec( CMD_COMENT );
    coment->d.coment.attr = 0x00;
    coment->d.coment.class = CMT_MS_OMF;
    ObjAttachData( coment, (uint_8 *) "\x1" "CV", 3 );
    ObjRSeek( coment, 0 );
    PObjEnqueue( coment );
    return( 0 );
}

STATIC const pobj_list myFilters[] = {
    { CMD_LAST_DEFN,    POBJ_WRITE_PASS, doDefn },
    { CMD_THEADR,       POBJ_WRITE_PASS, doTheadr }
};
#define NUM_FILTERS ( sizeof( myFilters ) / sizeof( pobj_list ) )

void GenMSCmtInit( void ) {
/***********************/
    if( !registered ) {
        registered = 1;
        PObjRegList( myFilters, NUM_FILTERS );
    }
}

void GenMSCmtFini( void ) {
/***********************/
    if( registered ) {
        registered = 0;
        PObjUnRegList( myFilters, NUM_FILTERS );
    }
}
