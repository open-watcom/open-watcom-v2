/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  symbol table search
*
****************************************************************************/


#include "ftnstd.h"
#include "symbol.h"
#include "fhash.h"
#include "rstsrch.h"


unsigned                HashValue;
hash_entry              HashTable[HASH_PRIME];


sym_id STSearch( const char *name, size_t len, sym_id head )
//==========================================================
// Search symbol table for given name.
{
    for(;;) {
        if( head == NULL )
            break;
        if( head->u.ns.u2.name_len == len ) {
            if( memcmp( name, &head->u.ns.name, len ) == 0 ) {
                break;
            }
        }
        head = head->u.ns.link;
    }
    return( head );
}


sym_id STNameSearch( const char *name, size_t len )
//=================================================
// Search symbol table for given name.
{
    sym_id      head;
    sym_id      tail;

    HashValue = CalcHash( name, len );
    head = HashTable[ HashValue ].h_head;
    if( head == NULL )
        return( NULL );
    tail = HashTable[ HashValue ].h_tail;
    for(;;) {
        if( head->u.ns.u2.name_len == len ) {
            if( memcmp( name, &head->u.ns.name, len ) == 0 ) {
                return( head );
            }
        }
        if( head == tail )
            return( NULL );
        head = head->u.ns.link;
    }
}
