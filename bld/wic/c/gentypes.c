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
* Description:  Generic data structures (linked list, stack).
*
****************************************************************************/


#include <stdlib.h>
#include "wic.h"


/* Allocate space for list AND initialize it */
pSLList createSLList(void) {
    pSLList newList = wicMalloc(sizeof *newList);
    newList->head = wicMalloc(sizeof *newList->head);
    newList->tail = newList->head;
    rewindCurrSLListPos(newList);
    return newList;
}

int isEmptySLList(pSLList pList) {
    return (pList->head == pList->tail);
}

static _SLList* _lastElemCopied; /* Needed for addBegSLListSLList */

/* copy pList, returning newly created copy (or NULL in case of error) */
pSLList dupSLList(pSLList pList, void* (*action)(void *)) {
    pSLList newList;
    void *elem;
    _SLList *savCurrPos;

    if (pList == NULL) {
        return NULL;
    }
    savCurrPos = pList->currPos;
    newList = createSLList();
    rewindCurrSLListPos(pList);

    while (getCurrSLListPosElem(pList, &elem)) {
        _lastElemCopied = newList->tail; /* Needed for addBegSLListSLList */
        addSLListElem(newList, action(elem));
        incCurrSLListPos(pList);
    }

    pList->currPos = savCurrPos;
    return newList;
}

/* Add an element to the end of a singly-linked list */
void addSLListElem(pSLList pList, void *elem) {
    pList->tail->next = wicMalloc(sizeof(_SLList));
    pList->tail->userData = elem;
    pList->tail = pList->tail->next;
}

/* Add elem to the beginning of pList */
void addBegSLListElem(pSLList pList, void *elem) {

    _SLList *newList;
    newList = wicMalloc(sizeof(_SLList));
    newList->userData = elem;
    newList->next = pList->head;
    pList->head = newList;
}

/* Add listAdd to the beginning of pList */
void addBegSLListSLList(pSLList pList, pSLList listAdd, void* (*action)(void *)) {
    pSLList newList;
    if (isEmptySLList(listAdd)) {
        return;
    }
    newList = dupSLList(listAdd, action);
    wicFree(_lastElemCopied->next); _lastElemCopied->next = NULL;
    _lastElemCopied->next = pList->head;
    pList->head = newList->head;
    wicFree(newList); newList = NULL;
}

pSLList combine2SLList(pSLList l1, pSLList l2) {
    if (l1 == NULL) {
        if (l2 == NULL) {
            return NULL;
        } else {
            return l2;
        }
    } else {
        if (l2 == NULL) {
            return l1;
        } else {
            assert(l1 != l2);
            l1->tail->next = l2->head->next;
            l1->tail->userData = l2->head->userData;
            l1->tail = l2->tail;
            wicFree(l2);
            return l1;
        }
    }
}

void insertSLListAfterCurrPosElem(pSLList pList, void *elem) {
    if (pList->currPos == pList->tail) {
        addSLListElem(pList, elem);
    } else {
        _SLList *temp = pList->currPos->next;
        pList->currPos->next = wicMalloc(sizeof(_SLList));
        pList->currPos->next->userData = elem;
        pList->currPos->next->next = temp;
    }
}

void setCurrSLListPosElem(pSLList pList, void *elem) {
    assert(!isEmptySLList(pList));
    assert(pList->currPos != pList->tail);
    pList->currPos->userData = elem;
}

/* Get current position element inside a list */
int getCurrSLListPosElem(pSLList pList, void *_pElem) {
    void    **pElem = _pElem;

    if (pList->currPos == pList->tail) {
        *pElem = NULL;
        return 0;
    } else {
        *pElem = pList->currPos->userData;
        return 1;
    }
}


/* Increment current position */
void incCurrSLListPos(pSLList pList) {
    if (pList->currPos != pList->tail) {
        pList->currPos = pList->currPos->next;
    }
}

/* Rewind current position to the beginning of the list */
void rewindCurrSLListPos(pSLList pList) {
    pList->currPos = pList->head;
}

/* Call 'func(elem)' for every element 'elem' in the list */
void forAllInSLList(pSLList pList, void (func)(void**)) {
    _SLList *temp;
    if (func == NULL) {
        return;
    }
    temp = pList->head;
    while (temp != pList->tail) {
        func(&(temp->userData));
        temp = temp->next;
    }
}

/* Delete a list, deallocating all of the space */
void zapSLList(pSLList pList, void (zapElem)(void*)) {
    _SLList *temp;
    if (pList == NULL) {
        return;
    }

    while (!isEmptySLList(pList)) {
        temp = pList->head;
        pList->head = pList->head->next;
        if (zapElem != NULL) {
            zapElem(temp->userData); temp->userData = NULL;
        }
        wicFree(temp); temp = NULL;
    }
    wicFree(pList->head); pList->head = NULL;
    wicFree(pList);
}

/* Unqueue element from the beginning of list.  Return 0 iff list is empty. */
int unqueSLListElem(pSLList pList, void *_pElem) {

    void    **pElem = _pElem;
    _SLList *temp;

    if (isEmptySLList(pList)) {
        *pElem = NULL;
        return 0;
    } else {
        if (pList->currPos == pList->head) {
            incCurrSLListPos(pList);
        }
        temp = pList->head;
        pList->head = pList->head->next;
        *pElem = temp->userData;
        wicFree(temp); temp = NULL;
        return 1;
    }
}

/*------------------------ Wic Stack -------------------------------------*/

void pushWicStackElem(pWicStack stack, void *elem) {
    pushStackElem(stack, elem);
}

pWicStack createWicStack() {
    return createStack();
}

/*----------------------- String Hash Table ----------------------------*/
