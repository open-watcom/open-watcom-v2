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
* Description:  WOMP utility routines, these are used by WOMP
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>

#include "asmerr.h"
#include "objprs.h"
#include "myassert.h"
#include "womputil.h"
#include "errout.h"


#define JUMP_OFFSET(cmd)    ((cmd)-CMD_POBJ_MIN_CMD)

static pobj_filter      jumpTable[ CMD_MAX_CMD - CMD_POBJ_MIN_CMD + 1 ];

/* these routines are part of the interface to the WOMP routines */

void PObjRegList( const pobj_list *list, size_t len )
/***************************************************/
{

    size_t  i;

    for( i = 0; i < len; ++i ) {
        jumpTable[ JUMP_OFFSET( list[i].command ) ] = list[i].func;
    }
}

void PObjUnRegList( const pobj_list *list, size_t len )
/*****************************************************/
{

    list = list;
    len = len;
}

#ifndef NDEBUG
int InternalError( const char *file, unsigned line )
/**************************************************/
// it is used by WOMP myassert function in debug version
{
    char msgbuf[MAX_MESSAGE_SIZE];

    MsgGet( MSG_INTERNAL_ERROR, msgbuf );
    fprintf( errout, msgbuf, file, line );
    fflush( errout );
    exit( EXIT_FAILURE );
    return( 0 );
}
#endif

void ObjWriteError( void )
/************************/
{
    MsgPrintf( OBJECT_WRITE_ERROR );
    exit( EXIT_FAILURE );
}

void write_record( obj_rec *objr, bool kill )
/*******************************************/
{
    /**/myassert( objr != NULL );
    ObjRSeek( objr, 0 );
    jumpTable[ JUMP_OFFSET(objr->command) ] ( objr, &pobjState );
    if( kill ) {
        ObjKillRec( objr );
    }
}
