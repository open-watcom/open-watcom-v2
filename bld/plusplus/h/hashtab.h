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


#ifndef _HASHTAB_H
#define _HASHTAB_H

#if defined( LONG_IS_64BIT ) || defined( _WIN64 )
#define SYMBOL_NAME_SHIFT	5
#else
#define SYMBOL_NAME_SHIFT	4
#endif

#ifndef NDEBUG
#define MIN_HASHTAB_SIZE        (1)
#else
#define MIN_HASHTAB_SIZE        (5)
#endif
#define MAX_HASHTAB_SIZE        (CV_SHIFT - SYMBOL_NAME_SHIFT)

typedef struct hash_tab *HASHTAB;

extern void HashPostInit( SCOPE );      // called after scopes are initialized
extern HASHTAB HashCreate( unsigned init_table_size);
extern void HashDestroy( HASHTAB );
extern HASHTAB HashMakeMax( HASHTAB );
extern bool HashEmpty( HASHTAB );
extern SYMBOL_NAME HashLookup( HASHTAB, NAME );
extern void HashInsert( HASHTAB, SYMBOL_NAME, NAME );
extern void HashWalk( HASHTAB, void (*)( SYMBOL_NAME ) );
extern void HashWalkData( HASHTAB, void (*)( SYMBOL_NAME, void * ), void * );

HASHTAB HashGetIndex( HASHTAB );
HASHTAB HashMapIndex( HASHTAB );

#ifndef NDEBUG
void StatsHASHTAB( HASHTAB hash );
#endif

#endif
