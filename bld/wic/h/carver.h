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


/*========================================================================

Carver is a data structure that provides fast memory allocation for elements
of the same type.  It does it by allocating big blocks of memory at a time.
Carver has an additional advantage that it does not use 'free()' to free
the elements, but stores them in a list.  The only time carver uses 'free'
is to free the whole carver itself.  The only time it uses 'alloc' is
to allocate a big block.

This implementation compbines a regular carver and an 'adaptive' carver
which can be used when it is not known in advance how many elements
will be used.  In 'adaptive' carver, the size of n-th block need not
be constant.  For example it may be 2^(n-1) elements.

========================================================================*/

#ifndef CARVER_H_INCLUDED
#define CARVER_H_INCLUDED

#include <stddef.h>

/*---------- Carver element header ----------------------------*/

/* To allow elements of size > 255, change CarverElemSize to "short" and
   recompile carver.c
*/
typedef unsigned char CarverElemSize;

/* CARVER_ELEM_PREFIX_SIZE specifies the size of element's prefix.
   It must be >= sizeof (CarverElemSize)
   It can be used for aligment as well as for debugging purposes.
   CARVER_ELEM_PREFIX_SIZE includes the size of CarverElemSize.
   It is user-defined but recompile of carver.c is required if it is modified.
*/
enum { CARVER_ELEM_PREFIX_SIZE = sizeof (CarverElemSize) };

/* This is exactly how prefix to an element looks like.
   NOTE:  size is 0 iff  element was not allocated
*/
typedef union {
    CarverElemSize size;
    char align[CARVER_ELEM_PREFIX_SIZE];
} CarverElemPrefix, *pCarverElemPrefix;

/*------------------ User-defined functions -------------------------*/

typedef void *AllocFunc(size_t size);
typedef AllocFunc *  pAllocFunc;
typedef void FreeFunc(void *elem);
typedef FreeFunc *  pFreeFunc;
typedef void CarverPrintMsgFunc(size_t size, char *str);
typedef CarverPrintMsgFunc *  pCarverPrintMsgFunc;

/*--------------- Carver information reporting ---------------------------*/

typedef enum {
    CARVER_NONE = 0,  // Not a message

    // Serious problems
    CARVER_CORRUPTED = 1,         // Corruption of memory has occured
    CARVER_ELEMENT_WAS_FREED = 2, // Attempted to free element that was already freed.

    // Allowable situations
    CARVER_UNFREED = 4,      // While freeing carver, found an unfreed element
    CARVER_FREED_NULL = 8,    // Null pointer was passed to FreeCarverElem
    CARVER_ALLOC_0 = 16      // Attempted to allocate memory of size 0
} CarverMsgType;

/* Default carver printing message function (used if NULL is specified for
   printMsgFunc in InitCarver)
*/
CarverPrintMsgFunc CarverDefaultPrintMsg;

/*------------------- Carver structure (private) ---------------------*/

typedef struct _TagCarverBlk *pCarverBlk;
typedef struct _TagBlkElem *pBlkElem;
typedef struct {
    pAllocFunc          mallocFunc;  // user supplied malloc
    pFreeFunc           freeFunc;  // user supplied free
    pCarverPrintMsgFunc printMsgFunc;
    CarverMsgType       supressMsgs;
    short unsigned      blkSize;  // Size of block (in number of elements)
    CarverElemSize      elemSize;
    short unsigned      numBlks;
    pBlkElem            topElem;
    pCarverBlk          tail;
    long                totalNumElems;
    pBlkElem            freeList;
} Carver, *pCarver;

/*==================== Carver functions =================================*/

/* InitCarver -- Initialize carver.
   Parameters:
   -- mallocFunc, freeFunc:  Functions to use to free/allocate memory.
   It is assumed that mallocFunc never returns NULL.
   -- printMsgFunc: When an error occurs,  printFunc is called. printFunc accepts
   errType which indicates which error occured and str which describes
   what happened.  str is a null-terminated string.  Use NULL to use
   'CarverDefaultprintMsg'
   -- supressMsgs: What messages to supress (ORed together)
   -- blkSize:  Size of each block (in number of elements).
   If blkSize == GUESS_CARVER_BLOCK_SIZE,
   the block size will be guessed and may vary from block to block.
   blkSize of 0 can be useful if it is not known in advance how many elements
   will be allocated.  As an experiment, try using GuessCarverBlockSize()
   before any carver is allocated to see if the memory usage of a program
   improves.
   -- elemSize:  Size of each element.  If elemSize < sizeof (void*),
   elemSize is set to sizeof(void*).  This will be then reflected on
   GetCarverElemSize().

   Example:
   typedef struct { float x_coord, y_coord } Point;
   InitCarver(&carver, malloc, free, NULL, CARVER_FREE_NULL | CARVER_ALLOC_0,
              GUESS_CARVER_BLOCK_SIZE, sizeof(Point));
*/
enum { GUESS_CARVER_BLOCK_SIZE = 0 };
void InitCarver(pCarver carver,
                void* (*mallocFunc)(size_t size),
                void  (*freeFunc)(void* ptr),
                pCarverPrintMsgFunc printMsgFunc,
                CarverMsgType supressMsgs,
                short unsigned blkSize,
                CarverElemSize elemSize);

/* CreateCarver -- Allocate memory for carver using mallocFunc
                   and call InitCarver.
*/
void *CreateCarver(
                void* (*mallocFunc)(size_t size),
                void  (*freeFunc)(void* ptr),
                pCarverPrintMsgFunc printMsgFunc,
                CarverMsgType supressMsgs,
                short unsigned blkSize,
                CarverElemSize elemSize);


/* GuessCarverBlockSize -- Use GUESS_CARVER_BLOCK_SIZE as blkSize
                           when initializing a carver (see InitCarver).
*/
void GuessCarverBlockSize(void);

/* AllocCarverElem -- Allocate memory.
   Parameters:
   -- carver:  Specifies which carver to allocate memory from.  'carver'
   must have been initialized before this call.  The size of element to be
   allocated is specified by elemSize parameter in InitCarver.

   This function never returns NULL.
*/
void* AllocCarverElem(pCarver carver);

/* FreeCarverElem -- Free elem (that was previously allocated from carver)
*/
void FreeCarverElem(pCarver carver, void *elem);

/* FiniCarver -- Free carver internals.
   Parameters:
   -- carver:  Carver to use
   -- checkNonFreedBlocks <CARVER_NO_CHECK or CARVER_CHECK>:
   if CARVER_CHECK, all of the blocks will be checked if they are free,
   and also to see if memory corruption has occured. However, this will take
   time in amount proportional to how many elements are in the block.
   Use CARVER_NO_CHECK for speed.
*/
enum {
    CARVER_NO_CHECK = 0,
    CARVER_CHECK
};
void FiniCarver(pCarver carver, int checkMem);
#define ZapCarver(carver, check)  FiniCarver((carver), (check)); \
                                  carver->freeFunc(carver);

/* GET_CARVER_ELEM_SIZE -- Get the size of element allocated on carver.
*/
#define GET_CARVER_ELEM_SIZE(elem) \
    (((pCarverElemPrefix)((char*)(elem)-CARVER_ELEM_PREFIX_SIZE))->size)

/*========================== END ===========================================*/

#endif
