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



/*---------- Symbol table for storing constants and macros ------------*/

typedef enum {
    SYMT_MACRO,
    SYMT_TYPEDEF
} SymType;

typedef struct SymTabEntry {
    char *name;
    SymType type;
    union {
        pSLList macroCode;
        pDeclInfo typeCode;
        void *data;
    } repr;
} SymTabEntry;

void initSymbolTable(void);
void zapSymbolTable(void);
void addSymbol(pSymTabEntry pEntry);
pSymTabEntry findSymbol(char *name);
void zapSymbolTable(void);
pSymTabEntry createTabEntry(char *name, SymType type, void *data);
int printSymTabElement(pSymTabEntry entry, void *dummy);
void printSymTab(void);


/*----------------- Generic Hash Table and string hash function ---------*/

typedef unsigned HashFunc(void *elem, unsigned size);
typedef HashFunc *pHashFunc;
typedef int HashElemCmp(void *elem1, void *elem2);  // Return 0 iff elem1 == elem2
typedef HashElemCmp *pHashElemCmp;

typedef struct _HTElem {
    void *userData;
    struct _HTElem* next;
} *_pHTElem;

typedef struct {
    _pHTElem *tbl;
    unsigned size;
    pHashFunc hashFunc;
    pHashElemCmp compareFunc; // Take two elements of the table;
                          // Return 0 iff elem1 == elem2
    struct {
        long numElems;
        int longestChainLen;
    } stats;
} *pHTable;

/*
    createHTable -- Create hash table
    size -- The size of a table, may be smaller than the number of elements
            that will be stored in it.
    keyFunc -- The key function which returns a key for an element.  The
               value returned by calling this function must be within [0, size-1]
    compareFunc -- Take 2 elements and return 0 iff elem1 == elem2.
    EXAMPLE:   table = createHTable(128,  stringHashFunc, strcmp);
*/
pHTable createHTable(int size, pHashFunc hashFunc, pHashElemCmp compareFunc);

/* addHTable  -- Add element to a hash table.
   If elem is already in hash table or if error occurs, return 0.
   Otherwise return non-zero. The check of whether the element is the same
   is done thorugh compareFunc (which was passed to createHTable)
*/
int addHTableElem(pHTable table, void *elem);

/*  findHTableElem -- Find an element in hash table.
    Return NULL if the element is not found, otherwise return a pointer
    to that element.
*/
void* findHTableElem(pHTable table, void *elem);

/*  walkHTable -- Perform 'action' for every element in hash table
*/
void walkHTable(pHTable table, void* (action)(void *));

/*  zapHTable -- Deallocate all of the elements of table using zapElemAction,
    then remove the table from memory.  zapElemAction may be NULL, in
    which case it is not called.
*/
void zapHTable(pHTable table, void* (zapElemAction)(void *));

/*  getHTableStats -- Get statistics about the hash table.  Useful for
    evaluating performance of a hash table to determine if a larger
    size table or better hashing function is needed
*/
void getHTableStats(pHTable table, int *numElem, int *longestChainLen);

/*  stringHashFunc -- Hash function for null-terminated strings.
    IMPORTANT: This function assumes that 'size' parameter is a power of 2
    (for example, 64, 128, but NOT 100)
*/
unsigned stringHashFunc(char *s, unsigned size);

/*  stringiHashFunc -- Hash function for null-terminated strings.
    The same as above, but ignore the case
*/
unsigned stringiHashFunc(char *s, unsigned size);
