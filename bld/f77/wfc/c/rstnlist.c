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
* Description:  resident symbol table manager
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "fmemmgr.h"

#include <string.h>

extern  uint                    AllocName(uint);
extern  label_id                NextLabel(void);


static  sym_id  AddNameList( char *name, int length ) {
//=====================================================

// Add a symbol table entry to the symbol table. Return a pointer to the
// new symbol table entry.

    sym_id      sym;

    sym = FMemAlloc( sizeof( name_list ) + AllocName( length ) );
    sym->nl.name_len = length;
    sym->nl.address = NULL;
    sym->nl.dbh = 0;
    memcpy( &sym->nl.name, name, length );
    return( sym );
}


sym_id  FindNameList( char *name, int len ) {
//===========================================

// Search symbol table for given name.

    sym_id      head;

    head = NmList;
    for(;;) {
        if( head == NULL ) return( NULL );
        if( head->nl.name_len == len ) {
            if( memcmp( name, &head->nl.name, len ) == 0 ) return( head );
        }
        head = head->nl.link;
    }
}


sym_id  STNameList( char *name, int length ) {
//============================================

// Lookup the specified name list name in the symbol table.

    sym_id      sym;

    if( length > MAX_SYMLEN ) {
        length = MAX_SYMLEN;
    }
    sym = FindNameList( name, length );
    if( sym == NULL ) {
        sym = AddNameList( name, length );
        sym->nl.link = NmList;
        sym->nl.group_list = NULL;
        NmList = sym;
    }
    return( sym );
}


char    *STNmListName( sym_id sym, char *buff ) {
//===============================================

// Get name list name.

    memcpy( buff, &sym->nl.name, sym->nl.name_len );
    buff += sym->nl.name_len;
    *buff = NULLCHAR;
    return( buff );

}
