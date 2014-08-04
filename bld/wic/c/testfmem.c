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


#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "fmem.h"
#include "time.h"

long g_maxMemUsage = 0;
long g_memUsed = 0;
long g_numFreeCalls = 0;
long g_numAllocCalls = 0;

OutOfMemory() {
    printf("Out of memory!!! Max usage: %ld.  Current usage: %ld\n",
            g_maxMemUsage, g_memUsed);
    exit(-1);
}

void *BasicAlloc(size_t size) {
    void *newBlock;
    newBlock = malloc(size);
    if (newBlock == NULL) if (size != 0) {
        // We are fried.
        OutOfMemory();
    }
    if (size != 0) {
        g_memUsed += _msize(newBlock);
    }
    if( g_maxMemUsage < g_memUsed )
        g_maxMemUsage = g_memUsed;
    g_numAllocCalls++;
    return newBlock;
}

void BasicFree(void *block) {
    if (block == NULL) {
        return;
    }
    g_memUsed -= _msize(block);
    free(block);
    g_numFreeCalls++;
    assert(g_memUsed >= 0);
}

void printMemStats() {
    printf("Mem usage: %ld;  Max usage: %ld;  'Alloc' calls: %ld;"
           " 'Free' calls: %ld\n", g_memUsed, g_maxMemUsage,
           g_numAllocCalls, g_numFreeCalls);
}

//============================================================================

#define MAX 1000
struct {
    int size;
    char *ptr;
} a[MAX];
long numInA = 0;

#define FAlloc BasicAlloc
#define FFree BasicFree


void AddElem(unsigned num, int size) {
    assert(numInA < MAX);
    assert(num < MAX);
    assert(a[num].ptr == NULL);
    a[num].size = size;
    a[num].ptr = FAlloc(size);
    numInA++;
    memset(a[num].ptr, size, size);
}

void FreeElem(unsigned num) {
    int i;
    int size;
    char *p;
    assert(num < MAX);
    assert(a[num].ptr != NULL);
    assert(numInA > 0);
    p = a[num].ptr;
    size = a[num].size;
    for (i = 0; i < size; i++) {
        assert(p[i] == (char) size);
    }
    FFree(p);
    numInA--;
    a[num].ptr = NULL;
    a[num].size = 0;
}

void AddRandElem(int size) {
    int i = rand() % MAX;
    int j = i;
    while (a[j].ptr != NULL) {
        j = (j + 1) % MAX;
        assert(j != i);
    }
    AddElem(j, size);
}

void FreeRandElem(void) {
    int i = rand() % MAX;
    int j = i;
    while (a[j].ptr == NULL) {
        j = (j + 1) % MAX;
        assert(j != i);
    }
    FreeElem(j);
}

void FreeAllElems() {
    int i;
    for (i = 0; i < MAX; i++) {
        if (a[i].ptr != NULL) {
            FreeElem(i);
        }
    }
}


void test1(void) {
    int i;
    assert(MAX >= 100);
    InitFMem(BasicAlloc, BasicFree, NULL, NULL);
    memset(a, 0, sizeof(a));
    FAlloc(0);
    FFree(0);
    for (i = 0; i < MAX-50; i++) {
        AddElem(i, rand() % 50 + 1);
    }
    for (i = MAX-50; i < MAX; i++) {
        AddElem(i, rand() % 10 + 253);
    }

    for (i = 0; i < MAX; i++) {
        FreeElem(i);
    }

    for (i = 0; i < 10000; i++) {
        if (rand() % 2 && numInA < MAX) {
            AddRandElem(rand() % 20 + 1);
        } else if (numInA > 0) {
            FreeRandElem();
        }
    }
    FreeAllElems();
    AddRandElem(20);
    AddRandElem(40);
    AddRandElem(255);
    AddRandElem(256);
    FiniFMem(FMEM_CHECK);
    assert(g_memUsed == 0);
    printf("Test 1 -- Success.\n");
    printMemStats();
}

void test2(void) {
    clock_t timer = clock();
    int top = 0;
    int i;
    for (i = 0; i < 500000; i++) {
        if (rand() % 2) {
            if (top < MAX) {
                a[top++].ptr = FAlloc(rand() % 20 + 1);
            }
        } else if (top > 0) {
            FFree(a[--top].ptr);
        }
    }
    FiniFMem(FMEM_NO_CHECK);
    timer = clock() - timer;
    printf("Time spent: %ld\n", timer);
    printf("Test 2 -- Success.\n");
}

void main() {
    test1();
    InitFMem(BasicAlloc, BasicFree, NULL, NULL);
    test2();
}



