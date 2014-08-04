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
* Description:  Carver memory allocator.
*
****************************************************************************/


#include "carver.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

enum { MAX_NUM_ELEM_IN_BLOCK  = 1000 };


struct _TagCarverBlk {
    pCarverBlk next;
    char data[1];  // An array of BlkData
};  // pCarverBlk is a forward def'n in carver.h

struct _TagBlkElem {
    CarverElemPrefix info;
    union {
        pBlkElem nextFreeElem;
        char data[1];
    };
};
#define BLK_ELEM_SIZE(dataSize)   (CARVER_ELEM_PREFIX_SIZE+ (dataSize))

static int guessCarverBlockSize = 0;

void GuessCarverBlockSize(void) {
    guessCarverBlockSize = 1;
}

static int sprintBlkElem(char *str, pBlkElem blkElem) {
/* Format:
Data &12345678: aa bb cc dd; Size 123 chk a5
*/
    int len = 0;
    int temp;
    int i;
    int len1;

    if (blkElem == NULL) {
        len += sprintf(str+len, "Data could not be accessed");
    } else {
        len += sprintf(str+len, "Data &%X:", blkElem->data);
    }
    len1 = blkElem->info.size;
    if( len1 > 4 )
        len1 = 4;
    for( i = 0; i < len1; i++ ) {
        len += sprintf(str+len, " %2X", blkElem->data[i]);
    }
    len += sprintf(str+len, "; Size %d", blkElem->info.size);
    temp = CARVER_ELEM_PREFIX_SIZE - sizeof(CarverElemSize);
    if (temp != 0) {
        char *p = (char*) &(blkElem->info.size) + sizeof blkElem->info.size;
        len += sprintf(str+len, " chk");
        for (i = 0; i < temp; i++) {
            len += sprintf(str+len, " %2X", p[i]);
        }
    }
    return len;
}


static void CarverMsg(CarverMsgType type, pCarver carver, pBlkElem blkElem)
{
    CarverMsgType carverSupressMsgs;
    if (carver == NULL) {
        carverSupressMsgs = CARVER_NONE;
    } else {
        carverSupressMsgs = carver->supressMsgs;
    }
    if (!(carverSupressMsgs & type)) {
        static char errStr[200];
        int errStrLen = 0;
        errStrLen += sprintf(errStr, "CARVER &%X: ", carver);
        switch(type) {
        case CARVER_CORRUPTED:
            errStrLen += sprintf(errStr+errStrLen, "Corrupted!      ");
            errStrLen += sprintBlkElem(errStr+errStrLen, blkElem);
            break;

        case CARVER_ELEMENT_WAS_FREED:
            errStrLen += sprintf(errStr+errStrLen, "Data was freed! ");
            errStrLen += sprintBlkElem(errStr+errStrLen, blkElem);
            break;

        case CARVER_UNFREED:
            errStrLen += sprintf(errStr+errStrLen, "Data not freed. ");
            errStrLen += sprintBlkElem(errStr+errStrLen, blkElem);
            break;

        case CARVER_FREED_NULL:
            errStrLen += sprintf(errStr+errStrLen, "Tried to free NULL.");
            break;

        case CARVER_ALLOC_0:
            errStrLen += sprintf(errStr+errStrLen, "Tried to allocate 0 bytes.");
            break;

        default:
            printf("Internal error in CarverMsg!\n");
        }

        if (carver == NULL) {
            CarverDefaultPrintMsg(type, errStr);
        } else {
            carver->printMsgFunc(type, errStr);
        }
    }
}


void InitCarver(pCarver carver,
                void* (*mallocFunc)(size_t size),
                void  (*freeFunc)(void* ptr),
                pCarverPrintMsgFunc printMsgFunc,
                CarverMsgType supressMsgs,
                short unsigned blkSize,
                CarverElemSize elemSize)
{
    carver->mallocFunc = mallocFunc;
    carver->freeFunc = freeFunc;
    if (printMsgFunc == NULL) {
        printMsgFunc = CarverDefaultPrintMsg;
    }
    carver->printMsgFunc = printMsgFunc;
    carver->supressMsgs = supressMsgs;
    if (guessCarverBlockSize) {
        blkSize = 0;
    }
    if (elemSize < sizeof (void *)) {
        if (elemSize == 0) {
            CarverMsg(CARVER_ALLOC_0, carver, NULL);
        }
        elemSize = sizeof(void*);
    }
    carver->elemSize = elemSize;
    carver->blkSize = blkSize;
    carver->numBlks = 0;
    carver->tail = NULL;
    carver->totalNumElems = 0;
    carver->freeList = NULL;
}

void* CreateCarver(
                void* (*mallocFunc)(size_t size),
                void  (*freeFunc)(void* ptr),
                pCarverPrintMsgFunc printMsgFunc,
                CarverMsgType supressMsgs,
                short unsigned blkSize,
                CarverElemSize elemSize)
{
    pCarver newCarver = mallocFunc(sizeof *newCarver);
    InitCarver(newCarver,
               mallocFunc,
               freeFunc,
               printMsgFunc,
               supressMsgs,
               blkSize,
               elemSize);

    return newCarver;
}


static long getCurrBlockSize(pCarver carver) {
    int temp;
    assert(carver != NULL);
    assert(carver->numBlks != 0);
    if (carver->blkSize == 0) {
        int temp2 = carver->numBlks;
        if( temp2 > 15 )
            temp2 = 15;
        temp = 1 << temp2 - 1;
        if( temp > MAX_NUM_ELEM_IN_BLOCK ) {
            temp = MAX_NUM_ELEM_IN_BLOCK;
        }
    } else {
        temp = carver->blkSize;
    }
    return temp * BLK_ELEM_SIZE(carver->elemSize);
}

static void CreateNewCarverBlk(pCarver carver) {

    pCarverBlk   newBlk;
    long         nextBlockSize;

    assert(carver != NULL);
    carver->numBlks++;
    nextBlockSize = getCurrBlockSize(carver);
    newBlk = carver->mallocFunc(sizeof *newBlk - sizeof newBlk->data +
                      nextBlockSize);

    newBlk->next = carver->tail;
    carver->tail = newBlk;
    carver->topElem = (pBlkElem) (newBlk->data + nextBlockSize);
    #ifndef NDEBUG
    {
        char* ptr;
        for (ptr = newBlk->data;
             ptr != (char*) carver->topElem;
             ptr += BLK_ELEM_SIZE(carver->elemSize))
        {
            memset(ptr, 0xa5, CARVER_ELEM_PREFIX_SIZE);
            memset(ptr+CARVER_ELEM_PREFIX_SIZE, 0, carver->elemSize);
        }
    }
    #endif

}

void* AllocCarverElem(pCarver carver) {
    pBlkElem newElem;

    #define SETUP_NEW_ELEMENT \
            newElem->info.size = carver->elemSize; carver->totalNumElems++;

    assert(carver != NULL);
    if (carver->freeList != NULL) {
        newElem = carver->freeList;
        carver->freeList = newElem->nextFreeElem;
        if (newElem->info.size != 0) {
            CarverMsg(CARVER_CORRUPTED, carver, newElem);
        }
        SETUP_NEW_ELEMENT
        return newElem->data;
    }

    if (carver->tail == NULL) {
        CreateNewCarverBlk(carver);
    }
    if (carver->topElem == (pBlkElem) carver->tail->data) {
        CreateNewCarverBlk(carver);
    }

    carver->topElem =  (pBlkElem)
        ( (char*) carver->topElem - BLK_ELEM_SIZE(carver->elemSize) );
    newElem = carver->topElem;
    SETUP_NEW_ELEMENT

    return newElem->data;
    #undef SETUP_NEW_ELEMENT
}

void FreeCarverElem(pCarver carver, void *elem) {
    pBlkElem blkElem;
    assert(carver != NULL);
    if (elem == NULL) {
        CarverMsg(CARVER_FREED_NULL, carver, NULL);
        return;
    }
    blkElem = (pBlkElem)((char*)elem - CARVER_ELEM_PREFIX_SIZE);
    if (blkElem->info.size != carver->elemSize) {
        if (blkElem->info.size == 0) {
            CarverMsg(CARVER_ELEMENT_WAS_FREED, carver, blkElem);
        } else {
            CarverMsg(CARVER_CORRUPTED, carver, blkElem);
        }
    } else {
        blkElem->info.size = 0;
        blkElem->nextFreeElem = carver->freeList;
        carver->freeList = blkElem;
        carver->totalNumElems--;
    }
}

void FiniCarver(pCarver carver, int checkMem) {
    pCarverBlk temp, *tail;

    assert(carver != NULL);
    tail = &(carver->tail);
    while(*tail != NULL) {

        if (checkMem == CARVER_CHECK) {
            pBlkElem lastElem = (pBlkElem)
                        ((*tail)->data + getCurrBlockSize(carver));
            pBlkElem blkElem;
            CarverElemSize elemSize = carver->elemSize;
            CarverElemSize currSize;

            for (blkElem = carver->topElem; blkElem != lastElem;
                blkElem = (pBlkElem)
                    ( (char*)blkElem + BLK_ELEM_SIZE(elemSize) ) )
            {
                currSize = blkElem->info.size;
                if (currSize != 0) {
                    if (currSize != elemSize) {
                        CarverMsg(CARVER_CORRUPTED, carver, blkElem);
                    } else {
                        CarverMsg(CARVER_UNFREED, carver, blkElem);
                    }
                }
            }
        }

        temp = (*tail)->next;
        carver->freeFunc(*tail);
        *tail = temp;
        carver->numBlks--;
        carver->topElem = (pBlkElem) ((*tail)->data);
    }
}

void CarverDefaultPrintMsg(size_t msgType, char *str) {
    msgType = msgType;
    printf("%s\n", str);
}
