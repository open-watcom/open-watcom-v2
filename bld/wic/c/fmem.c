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


#include "fmem.h"
#include "assert.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

static struct {
    pAllocFunc allocFunc;
    pFreeFunc freeFunc;
    pCarverPrintMsgFunc cPrintMsgFunc;
    CarverMsgType supressMsgs;
    pCarver  carver[MAX_ELEM_SIZE_INTO_CARVER+1];
    int numBigElems;
} fmem;

typedef struct {
    #ifndef NDEBUG
        size_t bigSize;
        char check;
    #endif
    CarverElemPrefix smallSize;
    char data[1];
} FMemBigElem, *pFMemBigElem;

enum { BIG_ELEM_PREFIX_SIZE = sizeof (FMemBigElem) - 1 };

typedef enum {
    FMEM_BIG_UNFREED,
    FMEM_BIG_CORRUPTED,
    FMEM_FREED_NULL,
    FMEM_ALLOC_0
} FMemMsgType;

void FMemMsg(FMemMsgType type, pFMemBigElem bigElem) {
    static char errStr[150];
    switch (type) {
    case FMEM_BIG_UNFREED:
        if (fmem.supressMsgs & CARVER_UNFREED) {
            return;
        }
        sprintf(errStr, "FMEM: %d big elements left unfreed", fmem.numBigElems);
        fmem.cPrintMsgFunc(CARVER_CORRUPTED, errStr);
        break;

    case FMEM_BIG_CORRUPTED:
        #ifndef NDEBUG
            sprintf(errStr, "FMEM: Big elem corrupted. Size %d; chkA5: %Xh; "
                            "chk0: %Xh;  data &%Xh",
                            bigElem->bigSize, bigElem->check,
                            bigElem->smallSize.size, bigElem->data);
        #else
            sprintf(errStr, "FMEM: Big elem corrupted. chk0: %Xh;  data &%Xh",
                            bigElem->smallSize.size, bigElem->data);
        #endif
        fmem.cPrintMsgFunc(CARVER_CORRUPTED, errStr);
        break;

    case FMEM_FREED_NULL:
        if (fmem.supressMsgs & CARVER_ALLOC_0) {
            return;
        }
        fmem.cPrintMsgFunc(CARVER_ALLOC_0, "FMEM: Attempted to allocate 0 bytes");

    case FMEM_ALLOC_0:
        if (fmem.supressMsgs & CARVER_FREED_NULL) {
            return;
        }
        fmem.cPrintMsgFunc(CARVER_FREED_NULL, "FMEM: Attempted to free NULL");
    }
}

void InitFMem(pAllocFunc allocFunc,
              pFreeFunc freeFunc,
              pCarverPrintMsgFunc cPrintMsgFunc,
              CarverMsgType supressMsgs) {
    fmem.allocFunc = allocFunc;
    fmem.freeFunc = freeFunc;
    if (cPrintMsgFunc == NULL) {
        cPrintMsgFunc = CarverDefaultPrintMsg;
    }
    fmem.cPrintMsgFunc = cPrintMsgFunc;
    fmem.supressMsgs = supressMsgs;
    memset(&(fmem.carver), 0, sizeof fmem.carver);
    fmem.numBigElems = 0;
}

void FiniFMem(int checkMem) {
    int i;

    for (i = 0; i < MAX_ELEM_SIZE_INTO_CARVER+1; i++) {
        if (fmem.carver[i] != NULL) {
            ZapCarver(fmem.carver[i], checkMem);
        }
    }
    if (fmem.numBigElems != 0) {
        FMemMsg(FMEM_BIG_UNFREED, NULL);
    }
}


int adjustSize(size_t size) {
    if (size > MAX_ELEM_SIZE_INTO_CARVER) {
        return -1;
    } else if (size > 0) {
        // Try to align on sizeof(void *) - byte boundary
        return min(MAX_ELEM_SIZE_INTO_CARVER,
                     size + sizeof(void*) - (size % sizeof(void*)) );
    } else {
        assert(0);
        return 0;
    }
}


void *FAlloc(size_t size) {
    size_t adjSize;
    void *retVal;
    if (size == 0) {
        retVal = NULL;
        FMemMsg(FMEM_ALLOC_0, NULL);
        goto Return;
    }
    adjSize = adjustSize(size);
    if (adjSize != -1) {
        pCarver *cvr = fmem.carver + adjSize;
        if (*cvr == NULL) {
            *cvr = CreateCarver(
                fmem.allocFunc,
                fmem.freeFunc,
                fmem.cPrintMsgFunc,
                fmem.supressMsgs,
                GUESS_CARVER_BLOCK_SIZE,
                adjSize
            );
        }
        retVal = AllocCarverElem(*cvr);
        assert(GET_CARVER_ELEM_SIZE(retVal) == adjSize);
    } else {
        pFMemBigElem bigElem = fmem.allocFunc(sizeof *bigElem - 1 + size );
        bigElem->smallSize.size = 0;
        #ifndef NDEBUG
            bigElem->bigSize = size;
            bigElem->check = 0xa5;
        #endif
        retVal = bigElem->data;
        fmem.numBigElems++;
    }

    Return: return retVal;
}

void FFree(void *elem) {
    size_t size;

    if (elem == NULL) {
        FMemMsg(FMEM_FREED_NULL, NULL);
        return;
    }
    size = GET_CARVER_ELEM_SIZE(elem);
    if (size != 0) {
        FreeCarverElem(fmem.carver[size], elem);
    } else {
        pFMemBigElem bigElem = (pFMemBigElem)
                            ((char*) elem - BIG_ELEM_PREFIX_SIZE);

        #ifndef NDEBUG
        if (bigElem->check != 0xa5 || adjustSize(bigElem->bigSize) != -1) {
            FMemMsg(FMEM_BIG_CORRUPTED, bigElem);
            return;
        }
        bigElem->check = 0;
        #endif
        fmem.freeFunc(bigElem);
        fmem.numBigElems--;
    }
}















