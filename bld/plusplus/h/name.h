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


#ifndef _NAME_H

typedef uint_16                 name_hash_t;
typedef unsigned long           name_dummy_index_t;

#define NAME_HASH               (0x01000)

/*
  All internal names start with NAME_DUMMY_PREFIX_0 and all
  counter generated names have the extra NAME_DUMMY_PREFIX_1
  as a prefix
*/
#define NAME_DUMMY_PREFIX_0     '.'
#define NAME_DUMMY_PREFIX_1     '#'

extern char *NameCreateNoLen( char *name );
extern char *NameCreateLen( char *name, unsigned len );
extern char *NameDummy( void );
extern name_dummy_index_t NameNextDummyIndex( void );
extern boolean IsNameDummy( char *name );

// hash returned is 0..4095 i.e., mod NAME_RAW_HASH
extern unsigned NameCalcHashLen( char const *, size_t );

#define NameHash( n )   ((unsigned)(((name_hash_t *)(n))[-1]))

extern char *NameGetIndex( char *name );
extern char *NameMapIndex( char *index );

extern int NameMemCmp( const char *, const char *, unsigned );

extern unsigned const NameCmpMask[5];
#define NAME_MAX_MASK_INDEX     ( ARRAY_SIZE( NameCmpMask ) - 1 )

#define _NAME_H
#endif
