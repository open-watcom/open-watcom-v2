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


/* Generic singly linked linked list, Generic queue. */

typedef struct _SLList {
    struct _SLList* next;
    void *userData;
} _SLList;

typedef struct SLList {
    _SLList *head;  /* 1-st element in the list */
    _SLList *tail;  /* Last (most recently added) element in the list */
    _SLList *currPos;
} *pSLList;

/* Allocate space for list and initialize it */
pSLList createSLList(void);

/* Return true iff pList is empty */
int isEmptySLList(pSLList pList);

/* Copy pList, returning newly created copy. */
pSLList dupSLList(pSLList pList, void* (*action)(void*));

/* Combine l1 and l2 into one list and return it.
Note that l1 and l2 may be freed so their usage is invalid after this call. */
pSLList combine2SLList(pSLList l1, pSLList l2);

/* Add an element to the end of a singly-linked list */
void addSLListElem(pSLList pList, void *elem);

/* Add elem to the beginning of pList */
void addBegSLListElem(pSLList pList, void *elem);

/* Add listAdd to the beginning of pList */
void addBegSLListSLList(pSLList pList, pSLList listAdd, void (*action)(void *));

/* Insert an element after current position in the list */
insertSLListAfterCurrPosElem(pSLList pList, void *elem);

/* Set user data at current position to a specified value */
void setCurrSLListPosElem(pSLList pList, void *elem);

/* Get current position element of a list. Return 0 iff end of list is reached*/
int getCurrSLListPosElem(pSLList pList, void **pElem);

/* Unqueue (delete) element from the beginning of list. Place the deleted
element into element. Return 0 iff list is empty. */
int unqueSLListElem(pSLList pList, void **element);

/* Increment current position */
void incCurrSLListPos(pSLList pList);

/* Rewind current position to the beginning of the list */
void rewindCurrSLListPos(pSLList pList);

/* Call 'func(elem)' for every element 'elem' in the list. 'func' must
return (new) userData from list. */
void forAllInSLList(pSLList pList, void (func)(void**));

/* Delete a list, deallocating all of the space. */
void zapSLList(pSLList pList, void* (zapElem)(void*));



/* Generic stack */

typedef pSLList pStack;

#define createStack  createSLList
#define pushStackElem(stack, elem)  addBegSLListElem(stack, elem)
#define popStackElem(stack, ppElem) unqueSLListElem(stack, ppElem)

/*------------------------ Wic Stack -------------------------------------*/

typedef pStack pWicStack;

pWicStack createWicStack();
void pushWicStackElem(pWicStack stack, void *elem);
#define popWicStackElem  popStackElem
