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
* Description:  Constant list
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "fmemmgr.h"
#include "fhash.h"

#include <string.h>

hash_entry              ConstHashTable[HASH_PRIME];


sym_id  STConst( void *ptr, TYPE typ, uint size ) {
//=================================================

// Search the symbol table for a constant. If the constant is not in the
// symbol table, add it to the symbol table.

    unsigned    hash_value;
    sym_id      head;
    sym_id      tail;
    ftn_type    *c_ptr = ptr;

    if( _IsTypeLogical( typ ) ) {
        c_ptr->logstar4 = c_ptr->logstar1;
    }
    hash_value = CalcHash( ptr, size );
    head = ConstHashTable[ hash_value ].h_head;
    if( head != NULL ) {
        tail = ConstHashTable[ hash_value ].h_tail;
        for(;;) {
            if( head->u.cn.typ == typ ) {
                if( memcmp( c_ptr, &head->u.cn.value, size ) == 0 ) {
                    return( head );
                }
            }
            if( head == tail ) break;
            head = head->u.cn.link;
        }
    }
    head = FMemAlloc( sizeof( constant ) - sizeof( ftn_type ) + size );
    memcpy( &head->u.cn.value, c_ptr, size );
    head->u.cn.typ = typ;
    head->u.cn.size = size;
    head->u.cn.address = NULL;
    HashInsert( ConstHashTable, hash_value, &CList, head );
    return( head );
}
