/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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


#include <stddef.h>
#include "asmglob.h"
#include "omfqueue.h"
#include "myassert.h"

void QInit( qdesc *q )
/********************/
{
    q->head = NULL;
    q->tail = NULL;
}

void QEnqueue( qdesc *q, void *item )
/***********************************/
{
    if( q->head == NULL ) {
        q->head = item;
    } else {
/**/    myassert( q->tail != NULL );
        *(void **)(q->tail) = item;
    }
    q->tail = item;
    *(void **)item = NULL;
}

void QJoinQueue( qdesc *dest, qdesc *src )
/****************************************/
{
    if( dest->head == NULL ) {
        dest->head = src->head;
    } else if( src->head == NULL ) {
        return;
    } else {
/**/    myassert( dest->tail != NULL );
        *(void **)(dest->tail) = src->head;
    }
    dest->tail = src->tail;
    *(void **)(src->tail) = NULL;
}

void *QDequeue( qdesc *q )
/************************/
{
    void *item;

    if( q->head == NULL ) {
        return( NULL );
    }
    item = q->head;
    q->head = *(void **)item;
    if( q->head == NULL ) {
        q->tail = NULL;
    }
    return( item );
}
