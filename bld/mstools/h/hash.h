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


#ifndef _HASH_H
#define _HASH_H

#include <stddef.h>


/*
 * Define some types.
 */
typedef void *          HashTable;
typedef unsigned        (*HashFunction)( const void *item );
typedef int             (*HashCompareFunction)( const void *item1,
                                                const void *item2 );
typedef int             (*HashWalkCallback)( const void *item, void *cookie );


/*
 * Function prototypes.
 */
extern HashTable    InitHash( unsigned numbuckets, HashFunction hashfunc,
                              HashCompareFunction comparefunc );
extern void         FiniHash( HashTable _table, int freeitems );

extern void         InsertHash( HashTable *table, const void *item );
extern void         WalkBucketHash( HashTable *table, const void *item,
                                    HashWalkCallback callback, void *cookie );


#endif
