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
#include "progsw.h"
#include "errcod.h"
#include "global.h"
#include "stmtsw.h"
#include "fhash.h"
#include "types.h"

#include <string.h>

extern  sym_id          STSearch(char *,int,sym_id);
extern  sym_id          STNameSearch(char *,int);
extern  sym_id          TmpVar(TYPE,uint);
extern  sym_id          StaticAlloc(uint,TYPE);
extern  sym_id          STAdd(char *,int);


static  void    InitHashTable( hash_entry *hash_table, int size ) {
//=================================================================

    int         i;

    for( i = 0; i < size; ++i ) {
        hash_table[i].h_head = 0;
        hash_table[i].h_tail = 0;
    }
}


void    OpenSymTab( void ) {
//====================

// Initialize the symbol table.

    if( ( ProgSw & PS_DONT_GENERATE ) == 0 ) return;
    GList = NULL;
    InitHashTable( GHashTable, HASH_PRIME + 1 );
}


void    CloseSymTab( void ) {
//=====================

}


void    VSTInit( void ) {
//=================

// Initialize symbol table.

    NList = NULL;
    InitHashTable( HashTable, HASH_PRIME );
    CList = NULL;
    InitHashTable( ConstHashTable, HASH_PRIME );
    LList = NULL;
    SList = NULL;
    MList = NULL;
    BList = NULL;
    RList = NULL;
    NmList = NULL;
    IFList = NULL;
}


sym_id  FindShadow( sym_id sym ) {
//================================

// Find symbol's shadow.

    sym_id      shadow;

    shadow = MList;
    for(;;) {
        if( shadow->ns.si.ms.sym == sym ) return( shadow );
        shadow = shadow->ns.link;
    }
}



sym_id    STAdvShadow( sym_id sym ) {
//===================================

// Shadow the array ( for automatic adv's ) argument.

    sym_id       shadow;

    shadow = StaticAlloc( sizeof( inttarg ), FT_INTEGER_TARG );
    shadow->ns.flags |= SY_SPECIAL_PARM | SY_SUBSCRIPTED;
    shadow->ns.si.ms.sym = sym;
    return( shadow );
}


sym_id  FindAdvShadow( sym_id sym ) {
//===================================

// Find ADV shadow.

    sym_id      shadow;

    for( shadow = MList; shadow != NULL; shadow = shadow->ns.link ) {
        if( shadow->ns.si.ms.sym == sym ) {
            if( shadow->ns.flags & SY_SUBSCRIPTED ) {
                break;
            }
        }
    }
    return( shadow );
}


sym_id    STArgShadow( sym_id sym ) {
//===================================

// Shadow a character argument.

    sym_id       shadow;

    shadow = StaticAlloc( sizeof( inttarg ), FT_INTEGER_TARG );
    shadow->ns.flags |= SY_SPECIAL_PARM | SY_VALUE_PARM;
    shadow->ns.si.ms.sym = sym;
    return( shadow );
}


sym_id  FindArgShadow( sym_id sym ) {
//===================================

// Find character argument's shadow.

    sym_id      shadow;

    shadow = MList;
    for(;;) {
        if( shadow->ns.si.ms.sym == sym ) {
            if( shadow->ns.flags & SY_VALUE_PARM ) {
                return( shadow );
            }
        }
        shadow = shadow->ns.link;
    }
}


sym_id    STEqSetShadow( sym_id sym ) {
//=====================================

// Shadow leader of equivalence set.

    sym_id       shadow;

    shadow = StaticAlloc( sym->ns.xt.size, sym->ns.u1.s.typ );
    shadow->ns.flags |= SY_SPECIAL_PARM | SY_IN_EQUIV;
    shadow->ns.si.ms.sym = sym;
    return( shadow );
}


sym_id  FindEqSetShadow( sym_id sym ) {
//=====================================

// Find shadow of leader of equivalence set.

    sym_id      shadow;

    for( shadow = MList; shadow != NULL; shadow = shadow->ns.link ) {
        if( shadow->ns.si.ms.sym == sym ) {
            if( shadow->ns.flags & SY_IN_EQUIV ) {
                break;
            }
        }
    }
    return( shadow );
}


sym_id    STFnShadow( sym_id sym ) {
//==================================

// Shadow a function entry point.

    sym_id       shadow;

    shadow = StaticAlloc( sym->ns.xt.size, sym->ns.u1.s.typ );
    shadow->ns.flags |= SY_SPECIAL_PARM | SY_PS_ENTRY;
    shadow->ns.si.ms.sym = sym;
    return( shadow );
}


sym_id    STShadow( sym_id sym ) {
//================================

// Shadow the given symbol.

    sym_id       shadow;

    if( StmtSw & SS_DATA_INIT ) { // implied do parm
        shadow = TmpVar( FT_INTEGER, TypeSize( FT_INTEGER ) );
    } else {
        shadow = StaticAlloc( sym->ns.xt.size, sym->ns.u1.s.typ );
    }
    shadow->ns.flags |= SY_SPECIAL_PARM;
    sym->ns.flags |= SY_SPECIAL_PARM;
    shadow->ns.si.ms.sym = sym;
    return( shadow );
}


void    STUnShadow( sym_id sym ) {
//================================

// Unshadow the specified symbol.

    sym_id      shadow;

    sym->ns.flags &= ~SY_SPECIAL_PARM;
    shadow = FindShadow( sym );
    shadow->ns.flags &= ~SY_SPECIAL_PARM;
    shadow->ns.si.ms.sym = NULL;
}


void    HashInsert( hash_entry *hash_table, unsigned hash_value,
                    sym_id *list, sym_id sym ) {
//==============================================

    if( hash_table[ hash_value ].h_head == NULL ) { // empty list
        hash_table[ hash_value ].h_head = sym;
        sym->ns.link = *list;
        *list = sym;
    } else {
        sym->ns.link = hash_table[ hash_value ].h_tail->ns.link;
        hash_table[ hash_value ].h_tail->ns.link = sym;
    }
    hash_table[ hash_value ].h_tail = sym;
}


sym_id  STName( char *name, int length ) {
//========================================

// Lookup the specified name in the symbol table.

    sym_id    sym;

    if( length > MAX_SYMLEN ) {
        length = MAX_SYMLEN;
    }
    sym = STNameSearch( name, length );
    if( sym == NULL ) {
        sym = STAdd( name, length );
        sym->ns.si.va.vi.ec_ext = NULL;
        sym->ns.u3.address = NULL;
        HashInsert( HashTable, HashValue, &NList, sym );
    } else if( ( ( sym->ns.flags & SY_CLASS ) == SY_VARIABLE ) &&
               ( sym->ns.flags & SY_SPECIAL_PARM ) ) {
        sym = FindShadow( sym ); // Shadowed variable
    }
    return( sym );
}


sym_id  STCommon( char *name, int length ) {
//==========================================

// Lookup the specified name in the common list.

    sym_id      sym_ptr;

    if( length > MAX_SYMLEN ) {
        length = MAX_SYMLEN;
    }
    sym_ptr = STSearch( name, length, BList );
    if( sym_ptr == NULL ) {
        sym_ptr = STAdd( name, length );
        sym_ptr->ns.link = BList;
        sym_ptr->ns.si.cb.first = NULL;    // indicate an empty common block
        sym_ptr->ns.u3.address = NULL;
        BList = sym_ptr;
    }
    return( sym_ptr );
}
