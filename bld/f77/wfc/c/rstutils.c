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
* Description:  resident symbol table utilities
*
****************************************************************************/


#include "ftnstd.h"
#include "segsw.h"
#include "global.h"
#include "cpopt.h"
#include "errcod.h"
#include "fmemmgr.h"
#include "insert.h"

#include <string.h>

extern  void            FreeChain(void *);
extern  void            *FreeLink(void *);


char    *STExtractName( sym_id sym, char *buff ) {
//================================================

// Get the name of a symbol.

    memcpy( buff, &sym->ns.name, sym->ns.name_len );
    buff += sym->ns.name_len;
    *buff = NULLCHAR;
    return( buff );
}


char    *STGetName( sym_id sym, char *buff ) {
//============================================

// Get the name of a symbol (check for shadowed symbols).

    if( ( ( sym->ns.flags & SY_CLASS ) == SY_VARIABLE ) &&
        ( sym->ns.flags & SY_SPECIAL_PARM ) ) {
        sym = sym->ns.si.ms.sym;
    }
    return( STExtractName( sym, buff ) );
}


uint    AllocName( uint length ) {
//================================

// Determine how much space to allocate for the name of a symbol.

    if( length <= STD_SYMLEN ) return( 0 );
    return( length - STD_SYMLEN );
}


sym_id  STAdd( char *name, int length ) {
//=======================================

// Add a symbol table entry to the symbol table. Return a pointer to the
// new symbol table entry.

    sym_id    sym;

    sym = FMemAlloc( sizeof( symbol ) + AllocName( length ) );
    sym->ns.name_len = length;
    memcpy( &sym->ns.name, name, length );
    sym->ns.flags = 0;
    sym->ns.xflags = 0;
    if( !(Options & OPT_REFERENCE) ) {
        sym->ns.xflags |= SY_FAKE_REFERENCE;
    }
    return( sym );
}


sym_id  STFree( sym_id sym ) {
//============================

// Free a symbol table entry and return its link field (must not be a
// common block).

    return( FreeLink( sym ) );
}


void    FreeSFHeader( sym_id sym ) {
//==================================

// Free statement function header.

    if( sym->ns.si.sf.header != NULL ) {
        FreeChain( &sym->ns.si.sf.header->parm_list );
        FMemFree( sym->ns.si.sf.header );
        sym->ns.si.sf.header = NULL;
    }
}


void    CkSymDeclared( sym_id sym ) {
//===================================

// Make sure type has been explicitly declared.

    if( (SgmtSw & SG_IMPLICIT_NONE) || (Options & OPT_EXPLICIT) ) {
        if( !(sym->ns.flags & SY_TYPE) ) {
            NameErr( TY_UNDECLARED, sym );
        }
    }
}
