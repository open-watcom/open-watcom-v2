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


#include "wic.h"
#include <string.h>
#define STR_HASH_TABLE_SIZE     (4096*4l)

typedef struct {
    char *string;
    unsigned long count: 31;
    unsigned long freeStr: 1;
} StrEntry, *pStrEntry;

static pHTable strTable = NULL;

static unsigned _strEntryHashFunc(pStrEntry entry, unsigned size) {
    assert(entry != NULL);
    return stringHashFunc(entry->string, size);
}

static int _strEntryHashComp(pStrEntry entry1, pStrEntry entry2) {
    if (entry1 == entry2) {
        return 0;
    } else {
        return strcmp(entry1->string, entry2->string);
    }
}

char *registerString(char *str, int freeStr) {
    StrEntry entry;
    pStrEntry tabEntry;
    char *retVal = str;

    if (str == NULL) {
        return NULL;
    }
    if (strTable == NULL) {
        strTable = createHTable(STR_HASH_TABLE_SIZE, _strEntryHashFunc,
            _strEntryHashComp);
    }

    entry.string = str;

    tabEntry = findHTableElem(strTable, &entry);
    if (tabEntry == NULL) {
        tabEntry = wicMalloc(sizeof *tabEntry);
        tabEntry->string = str;
        tabEntry->freeStr = freeStr;
        tabEntry->count = 1;
        if (!addHTableElem(strTable, tabEntry)) {
            reportError(FATAL_INTERNAL, "in registerString");
        }
    } else {
        if (freeStr) {
            wicFree(str);
        }
        tabEntry->count++;
        retVal = tabEntry->string;
    }
    return retVal;
}


void zapString(char *str) {
    StrEntry entry;
    pStrEntry tabEntry;

    if (str == NULL) {
        return;
    }
    assert(strTable != NULL);

    entry.string = str;

    tabEntry = findHTableElem(strTable, &entry);
    assert(tabEntry != NULL);
    if (tabEntry->count > 0) {
        tabEntry->count--;
    } else {
        assert(0);
    }
}

static void _zapStrEntry(pStrEntry entry) {
    if (entry->count != 0) {
//      printf(" STR_NON_FREED: %s ", entry->string);
    }
    if (entry->freeStr) {
        wicFree(entry->string);
    }
    wicFree(entry);
}

void zapStringTable(void) {
    int longestChainLen, numElems;
    getHTableStats(strTable, &numElems, &longestChainLen);
    #ifndef NDEBUG
        printf("Number of elements: %d;  Longest chain length: %d\n",
                        numElems, longestChainLen);
    #endif
    zapHTable(strTable, _zapStrEntry);
    strTable = NULL;
}
