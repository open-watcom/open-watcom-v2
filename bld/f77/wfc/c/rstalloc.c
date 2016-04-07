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
* Description:  Resident symbol table memory manager.
*
****************************************************************************/


#include "ftnstd.h"
#include "errcod.h"
#include "global.h"
#include "fmemmgr.h"
#include "chain.h"
#include "rstutils.h"
#include "rstalloc.h"

#include <string.h>


/* Forward declarations */
static  void    FreeList( sym_id sym_ptr );
static  void    FreeMList( sym_id sym_ptr );


grp_entry       *STGroupEntry( void ) {
//===============================

// Allocate a group entry extension.

    grp_entry   *ge;

    ge = FMemAlloc( sizeof( grp_entry ) );
    ge->link = NULL;
    return( ge );
}


com_eq  *STComEq( void ) {
//==================

// Allocate a common/equivalence extension.

    com_eq      *ce_ext;

    ce_ext = FMemAlloc( sizeof( com_eq ) );
    ce_ext->ec_flags = 0;
    ce_ext->low = 0;
    ce_ext->high = 0;
    ce_ext->offset = 0;
    ce_ext->link_eqv = NULL;
    ce_ext->link_com = NULL;
    return( ce_ext );
}


act_dim_list    *STSubsList( act_dim_list *subs_ptr ) {
//======================================================

// Allocate a dimension extension.

    int             size;
    act_dim_list    *ste_ptr;

    size = sizeof( dim_list ) + ( 2 * sizeof( intstar4 ) ) * _DimCount( subs_ptr->dim_flags );
    ste_ptr = FMemAlloc( size );
    memcpy( ste_ptr, subs_ptr, size );
    return( ste_ptr );
}


void    STPurge( void ) {
//=================

// Free up the entire symbol table.

    FreeNameList( GList );
    GList = NULL;
    FreeLocalLists();
    // SubProgId is used by Error() to report subprogram name. In case
    // we are purging as a result of "out of memory" error, set SubProgId
    // to NULL.
    SubProgId = NULL;
}


void    FreeLocalLists( void ) {
//========================

// Free symbol tables associated with subprograms (local).

    FreeNameList( NList );
    FreeNameList( BList );
    FreeList( CList );
    FreeList( SList );
    FreeList( LList );
    FreeMList( MList );
    FreeRList( RList );
    FreeNmList( NmList );
    FreeNameList( IFList );
    BList = NULL;
    SList = NULL;
    NList = NULL;
    LList = NULL;
    CList = NULL;
    MList = NULL;
    RList = NULL;
    NmList = NULL;
    IFList = NULL;
}


void    FreeNmList( sym_id sym ) {
//================================

// Free a symbol table list of name lists.

    while( sym != NULL ) {
        FreeChain( &sym->u.nl.group_list );
        sym = FreeLink( (void **)sym );
    }
}


sym_id  FreeREntry( sym_id sym ) {
//================================

    sym_id      fd;

    fd = sym->u.sd.fl.sym_fields;
    while( fd != NULL ) {
        if( fd->u.fd.typ == FT_UNION ) {
            FreeRList( fd->u.fd.xt.sym_record );
        } else {
            if( fd->u.fd.dim_ext != NULL ) {
                FMemFree( fd->u.fd.dim_ext );
            }
        }
        fd = FreeLink( (void **)fd );
    }
    return( FreeLink( (void **)sym ) );
}


void    FreeRList( sym_id sym ) {
//===============================

// Free a symbol table list of structure names.

    while( sym != NULL ) {
        sym = FreeREntry( sym );
    }
}


static  sym_id    STFreeMSTE( sym_id sym_ptr ) {
//==============================================

// Free a "magic" symbol table entry.

    sym_ptr = STFree( sym_ptr );
    return( sym_ptr );
}


static  void    FreeMList( sym_id sym_ptr ) {
//===========================================

// Free a symbol table list of names.

    while( sym_ptr != NULL ) {
        sym_ptr = STFreeMSTE( sym_ptr );
    }
}


sym_id        STFreeName( sym_id sym_ptr ) {
//==========================================

// Free a symbol name.

    if( ( sym_ptr->u.ns.flags & SY_CLASS ) == SY_COMMON ) {
        sym_ptr = FreeLink( (void **)sym_ptr );
    } else {
        if( ( sym_ptr->u.ns.flags & SY_CLASS ) == SY_VARIABLE ) {
            if( sym_ptr->u.ns.flags & SY_SUBSCRIPTED ) {
                if( sym_ptr->u.ns.si.va.u.dim_ext != NULL ) {
                    FMemFree( sym_ptr->u.ns.si.va.u.dim_ext );
                }
            }
            if( sym_ptr->u.ns.flags & SY_IN_EC ) {
                if( sym_ptr->u.ns.si.va.vi.ec_ext != NULL ) {
                    FMemFree( sym_ptr->u.ns.si.va.vi.ec_ext );
                }
            }
        } else if( ( sym_ptr->u.ns.flags & SY_CLASS ) == SY_SUBPROGRAM ) {
            if( ( sym_ptr->u.ns.flags & SY_SUBPROG_TYPE ) == SY_STMT_FUNC ) {
                FreeSFHeader( sym_ptr );
            }
        }
        sym_ptr = STFree( sym_ptr );
    }
    return( sym_ptr );
}


void    FreeNameList( sym_id sym_ptr ) {
//======================================

// Free a symbol table list of names.

    while( sym_ptr != NULL ) {
        sym_ptr = STFreeName( sym_ptr );
    }
}


static  void    FreeList( sym_id sym_ptr ) {
//==========================================

// Free a symbol table (CList, LList or SList ).

    while( sym_ptr != NULL ) {
        sym_ptr = STFree( sym_ptr );
    }
}
