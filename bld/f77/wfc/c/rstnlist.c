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
* Description:  resident symbol table manager
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "fmemmgr.h"
#include "rstutils.h"
#include "rstnlist.h"
#include "cgmagic.h"


static  sym_id  AddNameList( const char *name, size_t length )
//============================================================
// Add a symbol table entry to the symbol table. Return a pointer to the
// new symbol table entry.
{
    sym_id      sym;

    sym = FMemAlloc( sizeof( name_list ) + length );
    sym->u.nl.name_len = length;
    sym->u.nl.address = NULL;
    sym->u.nl.dbh = 0;
    memcpy( &sym->u.nl.name, name, length );
    sym->u.nl.name[length] = NULLCHAR;
    return( sym );
}


sym_id  FindNameList( const char *name, size_t len )
//==================================================
// Search symbol table for given name.
{
    sym_id      head;

    head = NmList;
    for(;;) {
        if( head == NULL )
            return( NULL );
        if( head->u.nl.name_len == len ) {
            if( memcmp( name, &head->u.nl.name, len ) == 0 ) {
                return( head );
            }
        }
        head = head->u.nl.link;
    }
}


sym_id  STNameList( const char *name, size_t length )
//===================================================
// Lookup the specified name list name in the symbol table.
{
    sym_id      sym;

    if( length > MAX_SYMLEN ) {
        length = MAX_SYMLEN;
    }
    sym = FindNameList( name, length );
    if( sym == NULL ) {
        sym = AddNameList( name, length );
        sym->u.nl.link = NmList;
        sym->u.nl.group_list = NULL;
        NmList = sym;
    }
    return( sym );
}


char    *STNmListName( sym_id sym, char *buff ) {
//===============================================

// Get name list name.

    memcpy( buff, &sym->u.nl.name, sym->u.nl.name_len );
    buff += sym->u.nl.name_len;
    *buff = NULLCHAR;
    return( buff );

}
