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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "watcom.h"
#include "womp.h"
#include "myassert.h"
#include "asmalloc.h"
#include "asmerr.h"
#include "memutil.h"

#include "objprs.h"
#define JUMP_OFFSET(cmd)    ((cmd)-CMD_POBJ_MIN_CMD)

pobj_filter     jumpTable[ CMD_MAX_CMD - CMD_POBJ_MIN_CMD + 1 ];

extern void             MsgPrintf( int resourceid );
extern int              MsgGet( int resourceid, char *buffer );

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

int InternalError( const char *file, unsigned line ) {
/***************************************************/

    char msgbuf[80];

///**/    printf( "Internal error in %s(%u)\n", file, line );
    MsgGet( MSG_INTERNAL_ERROR, msgbuf );
    printf( msgbuf, file, line );
    exit( EXIT_FAILURE );
    return( 0 );
}

int BeQuiet( void )
/*****************/
{
    return 1;
}

void ObjWriteError( void )
{
//    printf( "OBJECT WRITE ERROR !!\n" );
    MsgPrintf( OBJECT_WRITE_ERROR );
    exit( EXIT_FAILURE );
};
