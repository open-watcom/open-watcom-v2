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
* Description:  Global symbol table routines.
*
****************************************************************************/


#include "ftnstd.h"
#include "ecflags.h"
#include "errcod.h"
#include "undef.h"
#include "global.h"
#include "progsw.h"
#include "cpopt.h"
#include "fmemmgr.h"
#include "fhash.h"
#include "iflookup.h"
#include "ferror.h"
#include "insert.h"
#include "cbsize.h"
#include "rstutils.h"
#include "rstglist.h"
#include "rstmgr.h"

#include <string.h>


// Local variables are only added to the GList for relocation purposes.
// They are never searched for; so we will store them in the bucket
// at HASH_PRIME.  (that's the reason for the +1)

hash_entry              GHashTable[HASH_PRIME + 1];


static  sym_id  LnkNewGlobal( sym_id local ) {
//============================================

// Allocate a global symbol and link it into the global list.

    sym_id      global;
    int         len;

    len = sizeof( symbol ) + AllocName( local->u.ns.u2.name_len );
    global = FMemAlloc( len );
    memcpy( global, local, len );
    HashInsert( GHashTable, HashValue, &GList, global );
    return( global );
}



sym_id        SearchGList( sym_id local ) {
//=========================================

// Search the global list for a symbol.

    sym_id      head;
    sym_id      tail;
    int         name_len;

    name_len = local->u.ns.u2.name_len;
    HashValue = CalcHash( local->u.ns.name, name_len );
    head = GHashTable[ HashValue ].h_head;
    if( head == NULL ) return( NULL );
    tail = GHashTable[ HashValue ].h_tail;
    for(;;) {
        if( ( head->u.ns.u2.name_len == name_len ) &&
            ( memcmp( &local->u.ns.name, &head->u.ns.name, name_len ) == 0 ) &&
            ( IsIntrinsic(head->u.ns.flags) == IsIntrinsic(local->u.ns.flags) ) ) {
             return( head );
        }
        if( head == tail ) return( NULL );
        head = head->u.ns.link;
    }
}


sym_id      AddSP2GList( sym_id ste_ptr ) {
//=========================================

// Add a subprogram to the global list.

    sym_id      gbl;
    unsigned_16 flags;
    unsigned_16 subprog;
    unsigned_16 gsubprog;

    flags = ste_ptr->u.ns.flags;
    subprog = flags & SY_SUBPROG_TYPE;
    gbl = SearchGList( ste_ptr );
    if( gbl == NULL ) {
        gbl = LnkNewGlobal( ste_ptr );
        gbl->u.ns.flags &= ~SY_REFERENCED;
    } else if( ( gbl->u.ns.flags & SY_CLASS ) != SY_SUBPROGRAM ) {
        PrevDef( gbl );
        return( gbl );
    } else {
        gsubprog = gbl->u.ns.flags & SY_SUBPROG_TYPE;
        if( gsubprog == SY_FN_OR_SUB ) {
            // We don't know what global symbol is - it could be a
            // function, subroutine or block data subprogram.
            // If we know what the local symbol is then the global symbol
            // becomes what the local symbol is.
            gbl->u.ns.flags &= ~SY_FN_OR_SUB;
            gbl->u.ns.flags |= subprog;
        } else if( (gsubprog != subprog) && (subprog != SY_FN_OR_SUB) ) {
            PrevDef( gbl );
            return( gbl );
        }
    }
    if( ( flags & SY_PS_ENTRY ) || ( subprog == SY_BLOCK_DATA ) ) {
        if( gbl->u.ns.flags & SY_ADDR_ASSIGNED ) {
            if( ( ( subprog != SY_PROGRAM ) && ( subprog != SY_BLOCK_DATA ) ) ||
                ( ( flags & SY_UNNAMED ) == 0 ) ) {
                PrevDef( gbl );
            } else {
                ClassErr( SR_TWO_UNNAMED, gbl );
            }
        } else {
            gbl->u.ns.flags |= SY_ADDR_ASSIGNED;
        }
    }
    return( gbl );
}


void    CkComSize( sym_id sym_ptr, unsigned_32 size ) {
//=====================================================

// Check for matching sizes of common blocks.

    char        buff[MAX_SYMLEN+1];
    intstar4    com_size;

    com_size = GetComBlkSize( sym_ptr );
    if( size != com_size ) {
        if( size > com_size ) {
            SetComBlkSize( sym_ptr, size );
        }
        if( ( sym_ptr->u.ns.flags & SY_COMSIZE_WARN ) == 0 ) {
            // It's nice to give a warning message when the blank common
            // block appears as different sizes even though the standard
            // permits it.
            if( sym_ptr->u.ns.flags & SY_BLANK_COMMON ) {
                Warning( CM_BLANK_DIFF_SIZE );
            } else {
                STGetName( sym_ptr, buff );
                Warning( CM_NAMED_DIFF_SIZE, buff );
            }
            sym_ptr->u.ns.flags |= SY_COMSIZE_WARN;
        }
    }
}

sym_id  AddCB2GList( sym_id ste_ptr ) {
//=====================================

// Add a common block to the global list.

    sym_id      gbl;
    unsigned_16 flags;

    flags = ste_ptr->u.ns.flags;
    gbl = SearchGList( ste_ptr );
    if( gbl == NULL ) {
        gbl = LnkNewGlobal( ste_ptr );
    } else if( ( gbl->u.ns.flags & SY_CLASS ) != SY_COMMON ) {
        PrevDef( gbl );
    } else {
        if( ( gbl->u.ns.flags & SY_SAVED ) != ( flags & SY_SAVED ) ) {
            gbl->u.ns.flags |= SY_SAVED;
            if( ( flags & SY_COMMON_LOAD ) == 0 ) {
                NameWarn( SA_COMMON_NOT_SAVED, ste_ptr );
            }
        }
        CkComSize( gbl, GetComBlkSize( ste_ptr ) );
        if( flags & gbl->u.ns.flags & SY_IN_BLOCK_DATA ) {
            NameErr( CM_BLKDATA_ALREADY, gbl );
        }
        gbl->u.ns.flags |= flags & ( SY_COMMON_INIT | SY_EQUIVED_NAME );
    }
    return( gbl );
}


