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
* Description:  Fortran interface to symbol table manager.
*
****************************************************************************/


#include "ftnstd.h"
#include "errcod.h"
#include "global.h"
#include "extnsw.h"
#include "stmtsw.h"
#include "progsw.h"
#include "opn.h"
#include "insert.h"
#include "impltab.h"
#include "rstnlist.h"
#include "rststruc.h"
#include "symtab.h"
#include "declare.h"
#include "rstmgr.h"

extern  sym_id          STNameSearch(char *,int);

#define PROG_LEN        5
char    ProgName[ PROG_LEN+1 ] = { 'F','M','A','I','N',
                                   NULLCHAR };
#define BLKDAT_LEN      10
char    BlkData[ BLKDAT_LEN+1 ] = { '@','B','L','O','C','K','D','A','T','A',
                                    NULLCHAR };
#define CBLANK_LEN      6
char    CBlank[ CBLANK_LEN+1 ] = { '@','B','L','A','N','K',NULLCHAR };


void    STInit( void ) {
//================

// Initialize symbol table manager.

    VSTInit();
    InitImplTab();
}


sym_id  SymLookup( char *name, int length ) {
//===========================================

// Lookup a symbol in the symbol table.

    sym_id    sym;

    sym = STName( name, length );
    if( ( sym->u.ns.flags & ( SY_TYPE | SY_INTRINSIC ) ) == 0 ) {
        sym->u.ns.xt.size = ImplSize( *name );
        sym->u.ns.u1.s.typ = MapTypes( ImplType( *name ), sym->u.ns.xt.size );
    }
    return( sym );
}


sym_id  LkSym( void ) {
//===============

// Look up symbol in symbol table and set flag bits.

    sym_id    sym;

    sym = SymLookup( CITNode->opnd, CITNode->opnd_size );
    CITNode->sym_ptr = sym;
    CITNode->flags = sym->u.ns.flags;
    CITNode->size = sym->u.ns.xt.size;
    CITNode->typ = sym->u.ns.u1.s.typ;
    if( ( sym->u.ns.u2.name_len > STD_SYMLEN ) &&
        ( ( ExtnSw & XS_LONG_NAME ) == 0 ) ) {
        NameExt( VA_NAME_LEN_EXT, sym );
        ExtnSw |= XS_LONG_NAME;
    }
    return( sym );
}


sym_id  LkProgram( void ) {
//===================

// Look up default program name in symbol table. Don't set any flag bits.

    sym_id    sym;

    sym = SymLookup( ProgName, PROG_LEN );
    sym->u.ns.flags = SY_USAGE | SY_SUBPROGRAM | SY_PROGRAM | SY_PENTRY |
                    SY_UNNAMED | SY_REFERENCED;
    return( sym );
}


sym_id  LkBlkData( void ) {
//===================

// Look up default block data name in symbol table.

    sym_id    sym;

    sym = SymLookup( BlkData, BLKDAT_LEN );
    sym->u.ns.flags = SY_USAGE | SY_SUBPROGRAM | SY_BLOCK_DATA |
                    SY_PENTRY | SY_UNNAMED | SY_REFERENCED;
    return( sym );
}


sym_id  SymFind( char *name, int length ) {
//=========================================

// Determine if a symbol exists in the symbol table.

    return( STNameSearch( name, length ) );
}


static  sym_id  ComLookup( char *name, int length ) {
//===================================================

// Lookup a common block name.

    sym_id      sym;

    sym = STCommon( name, length );
    sym->u.ns.flags |= SY_USAGE | SY_COMMON;
    if( ProgSw & PS_BLOCK_DATA ) {
        sym->u.ns.flags |= SY_IN_BLOCK_DATA;
    }
    return( sym );
}


sym_id  LkBCommon( void ) {
//===================

// Look up blank common in the symbol table.

    sym_id      sym;

    sym = ComLookup( CBlank, CBLANK_LEN );
    sym->u.ns.flags |= SY_BLANK_COMMON;
    return( sym );
}


sym_id  LkCommon( void ) {
//==================

// Look up common name in the symbol table.

    return( ComLookup( CITNode->opnd, CITNode->opnd_size ) );
}


sym_id  LkField( sym_id sd ) {
//============================

    sym_id      sym;

    sym = FieldLookup( sd->u.sd.fl.sym_fields, CITNode->opnd, CITNode->opnd_size,
                       &CITNode->value.intstar4 );
    if( sym != NULL ) {
        CITNode->typ = sym->u.fd.typ;
        if( CITNode->typ == FT_STRUCTURE ) {
            CITNode->size = sym->u.fd.xt.record->size;
        } else {
            CITNode->size = sym->u.fd.xt.size;
        }
        CITNode->flags = SY_VARIABLE;
        if( sym->u.fd.dim_ext != NULL ) {
            CITNode->flags |= SY_SUBSCRIPTED;
        }
    } else {
        StructErr( SP_NO_SUCH_FIELD, sd );
    }
    CITNode->sym_ptr = sym;
    return( sym );
}


sym_id  LkNameList( void ) {
//====================

    return( STNameList( CITNode->opnd, CITNode->opnd_size ) );
}


sym_id  NameListFind( void ) {
//======================

// Determine if a symbol exists in the name list symbol table.

    sym_id      nl;

    nl = FindNameList( CITNode->opnd, CITNode->opnd_size );
    if( nl != NULL ) {
        CITNode->sym_ptr = nl;
    }
    return( nl );
}
