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
#include <ctype.h>
#include "wic.h"


/*---------- Symbol table ------------*/

static symbol_table symTab;

static symTabCmp(pSymTabEntry entry1, pSymTabEntry entry2) {
    return strcmp(entry1->name, entry2->name);
}

void initSymbolTable(void) {
    symTab = SymInit(symTabCmp);
    if (symTab == NULL) {
        outOfMemory();
    }
}

void addSymbol(pSymTabEntry pEntry) {
    if (SymFind(symTab, pEntry) != NULL) {
        reportError(CERR_REDEFN_IGNORED, pEntry->name);
    } else {
        if (!SymAdd(symTab, pEntry)) {
            outOfMemory();
        }
        g_numSymbols++;  // Number of symbols in symbol table
    }
}

pSymTabEntry findSymbol(char *name) {
    SymTabEntry nameEntry;
    nameEntry.name = name;
    return SymFind(symTab, &nameEntry);
}

int zapSymbolTableElem(pSymTabEntry entry, void *dummy) {
    dummy = dummy;
    switch (entry->type) {
        case SYMT_MACRO:
            zapSLList(entry->repr.macroCode, zapToken);
            break;
        case SYMT_TYPEDEF:
            zapDeclInfo(entry->repr.typeCode);
            break;
        default:
            assert(0);
    }
    wicFree(entry->name);
    wicFree(entry);
    return 0;
}

void zapSymbolTable(void) {
    SymWalk(symTab, NULL, zapSymbolTableElem);
    SymFini(symTab);
}

pSymTabEntry createTabEntry(char *name, SymType type, void *data) {
    pSymTabEntry newEntry;

    newEntry = wicMalloc(sizeof(SymTabEntry));
    newEntry->name = wicStrdup(name);
    if (newEntry->name == NULL) {
        outOfMemory();
    }
    newEntry->type = type;
    newEntry->repr.data = data;

    return newEntry;
}

int printSymTabElement(pSymTabEntry entry, void *dummy) {
    dummy = dummy;
    switch (entry->type) {
        case SYMT_MACRO:
            printf("MACRO %s:   %s", entry->name,
                    getTokListString(entry->repr.macroCode));
            break;

        case SYMT_TYPEDEF:
            printf("TYPEDEF %s:   %s", entry->name,  "Can't print decl");
            break;

        default:
            assert(0);
    }

    printf("\n");
    return 0;
}

void printSymTab(void) {
    printf("=============== Symbol Table Contents ==============\n");
    SymWalk(symTab, NULL, printSymTabElement);
    printf("====================================================\n");
}

/*------------------ Hash Table -----------------------------*/

pHTable createHTable(int size, pHashFunc hashFunc, pHashElemCmp compareFunc) {
    pHTable table;
    table = wicMalloc(sizeof *table);
    table->tbl = wicMalloc(sizeof table[0] * size);
    memset(table->tbl, 0, sizeof table[0] * size);
    table->size = size;
    table->hashFunc = hashFunc;
    table->compareFunc = compareFunc;
    table->stats.numElems = 0;
    table->stats.longestChainLen = 0;
    return table;
}

int addHTableElem(pHTable table, void *elem) {
    unsigned key;
    int chainLen = 0;
    _pHTElem tblElem;
    pHashElemCmp cmp = table->compareFunc;

    assert(elem != NULL);
    key = table->hashFunc(elem, table->size);
    if (key >= table->size) {
        printf("ERROR: in hashFunc = %p, key = %u\n", table->hashFunc, key);
        return 0;
    }

    for (tblElem = table->tbl[key]; tblElem != NULL; tblElem = tblElem->next) {
        chainLen++;
        if (!cmp(elem, tblElem->userData)) {
            return 0;
        }
    }
    tblElem = wicMalloc(sizeof *tblElem);
    tblElem->userData = elem;
    tblElem->next = table->tbl[key];
    table->tbl[key] = tblElem;
    chainLen++;

    table->stats.numElems++;
    table->stats.longestChainLen = max(table->stats.longestChainLen, chainLen);
    return 1;
}

void* findHTableElem(pHTable table, void *elem) {
    int key;
    _pHTElem tblElem;
    pHashElemCmp cmp = table->compareFunc;

    assert(elem != NULL);
    key = table->hashFunc(elem, table->size);
    if (key >= table->size) {
        printf("ERROR: in hashFunc = %p, key = %u\n", table->hashFunc, key);
        return NULL;
    }
    for (tblElem = table->tbl[key]; tblElem != NULL; tblElem = tblElem->next) {
        if (!cmp(elem, tblElem->userData)) {
            return tblElem->userData;
        }
    }
    return NULL;
}

void walkHTable(pHTable table, void* (action)(void *)) {
    int i;
    _pHTElem *tblPtr = table->tbl;
    _pHTElem tblElem;
    if (action == NULL) {
        return;
    }
    for (i = 0; i < table->size; i++) {
        for (tblElem = tblPtr[i]; tblElem != NULL; tblElem = tblElem->next) {
            action(tblElem->userData);
        }
    }
}

void zapHTable(pHTable table, void* (zapElemAction)(void *)) {
    int i;
    _pHTElem *tblPtr;
    _pHTElem tblElem, temp;

    if (table == NULL) {
        return;
    }

    tblPtr = table->tbl;

    for (i = 0; i < table->size; i++) {
        for (tblElem = tblPtr[i]; tblElem != NULL; tblElem = temp) {
            if (zapElemAction != NULL) {
                zapElemAction(tblElem->userData);
            }
            temp = tblElem->next;
            wicFree(tblElem);
        }
    }
    wicFree(table->tbl);
    wicFree(table);
}

void getHTableStats(pHTable table, int *numElems, int *longestChainLen) {
    *numElems = table->stats.numElems;
    *longestChainLen = table->stats.longestChainLen;
}

unsigned stringHashFunc(char *s, unsigned size) {
    enum { b = 101 };
    unsigned long key = 0;
    int i;
    for (i = 0; s[i] != 0; i++) {
        key += s[i];
        key *= b;
    }
    key = key & (size-1);

    return key;
}

unsigned stringiHashFunc(char *s, unsigned size) {
    enum { b = 101 };
    unsigned long key = 0;
    int i;
    for (i = 0; s[i] != 0; i++) {
        key += toupper(s[i]);
        key *= b;
    }
    key = key & (size-1);

    return key;
}

unsigned computeKey(char *s, unsigned size) {
    enum { b = 101 };
    unsigned long key = 0;
    int i;
    assert(size == 64);
    for (i = 0; s[i] != 0; i++) {
        key += s[i];
        key *= b;
    }
    key = key & (size-1);

    return key;
}

