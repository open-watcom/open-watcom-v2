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
#include "objrec.h"
#include "pcobj.h"
#include "queue.h"
#include "myassert.h"

STATIC qdesc    libQueue;

STATIC int readTheadr( obj_rec *objr, pobj_state *state ) {

/**/myassert( objr != NULL && objr->command == CMD_THEADR );
/**/myassert( state != NULL && state->pass == POBJ_READ_PASS );
    objr = objr;
    state = state;
    QInit( &libQueue );
    return( 0 );
}

STATIC int readComent( obj_rec *objr, pobj_state *state ) {

/**/myassert( objr != NULL && objr->command == CMD_COMENT );
/**/myassert( state != NULL && state->pass == POBJ_READ_PASS );
    state = state;
    if( objr->d.coment.class == CMT_DEFAULT_LIBRARY ) {
        QEnqueue( &libQueue, objr );
        return( -1 );
    }
    return( 0 );
}

STATIC int writeTheadr( obj_rec *objr, pobj_state *state ) {

/**/myassert( objr != NULL && objr->command == CMD_THEADR );
/**/myassert( state != NULL && state->pass == POBJ_WRITE_PASS );
    state = state;
    objr = objr;
    if( libQueue.head != NULL ) {
        PObjJoinQueue( &libQueue );
    }
    return( 0 );
}

STATIC const pobj_list myFuncs[] = {
    { CMD_THEADR,       POBJ_READ_PASS,     readTheadr },
    { CMD_COMENT,       POBJ_READ_PASS,     readComent },
    { CMD_THEADR,       POBJ_WRITE_PASS,    writeTheadr }
};
#define NUM_FUNCS   ( sizeof( myFuncs ) / sizeof( pobj_list ) )

void DefLibInit( void ) {

    PObjRegList( myFuncs, NUM_FUNCS );
}

void DefLibFini( void ) {

    PObjUnRegList( myFuncs, NUM_FUNCS );
}

