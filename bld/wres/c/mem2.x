/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Routines to memory allocation/usage.
*
*       ResAddLLItemAtEnd
*       ResInsertLLItemAfter
*       ResInsertLLItemBefore
*       ResDeleteLLItem
*       ResReplaceLLItem
*
****************************************************************************/


#include <stdlib.h>
#include "mem2.h"


typedef struct ss {
    struct ss *next,*prev;
} ss;

/*
 * ResAddLLItemAtEnd - create a new item at the tail of a linked list
 */
void ResAddLLItemAtEnd( ss **head, ss **tail, ss *item )
{
    if( *head == NULL ) {
        *head = *tail = item;
        item->next = NULL;
        item->prev = NULL;
    } else {
        item->prev = *tail;
        item->next = NULL;
        (*tail)->next = item;
        *tail = item;
    } /* if */

} /* ResAddLLItemAtEnd */

/*
 * ResInsertLLItemAfter - insert an item into a linked list after an item
 */
void ResInsertLLItemAfter( ss **head, ss **tail, ss *who, ss *item )
{
    ss *after_who;

    after_who = who->next;
    who->next = item;
    item->prev = who;
    item->next = after_who;

    if( after_who == NULL ) {
        *tail = item;
    } else {
        after_who->prev = item;
    } /* if */

} /* ResInsertLLItemAfter */

/*
 * ResInsertLLItemBefore - insert an item into a linked list after an item
 */
void ResInsertLLItemBefore( ss **head, ss **tail, ss *who, ss *item )
{
    ss *before_who;

    before_who = who->prev;
    who->prev = item;
    item->next = who;
    item->prev = before_who;

    if( before_who == NULL ) {
        *head = item;
    } else {
        before_who->next = item;
    } /* if */

} /* ResInsertLLItemBefore */

/*
 * ResDeleteLLItem - delete an item from a linked list
 */
void *ResDeleteLLItem( ss **head, ss **tail, ss *item )
{
#if 0
    if( item == *head
      && item == *tail ) {
        *head = *tail = NULL;
    } else if( item == *head ) {
        *head = item->next;
        (*head)->prev = NULL;
    } else if( item == *tail ) {
        *tail = item->prev;
        (*tail)->next = NULL;
    } else {
        item->prev->next = item->next;
        item->next->prev = item->prev;
    } /* if */
#else
    if( item == *head )
        *head = item->next;
    if( item == *tail )
 *tail = item->prev;
    if( item->prev != NULL )
        item->prev->next = item->next;
    if( item->next != NULL )
        item->next->prev = item->prev;
#endif
    return( item );

} /* ResDeleteLLItem */

/*
 * ResReplaceLLItem - drop a replacement item into a linked list
 */
void ResReplaceLLItem( ss **head, ss **tail, ss *item, ss *new )
{
    if( item == *head )
        *head = new;
    if( item == *tail )
        *tail = new;
   
    new->prev = item->prev;
    new->next = item->next;

    if( item->prev != NULL )
        item->prev->next = new;
    if( item->next != NULL )
        item->next->prev = new;
 
} /* ResReplaceLLItem */
