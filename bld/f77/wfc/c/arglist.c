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
* Description:  process argument lists.
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "prmcodes.h"
#include "types.h"
#include "errcod.h"
#include "fmemmgr.h"
#include "insert.h"

extern  TYPE            ImplType(char);
extern  uint            ImplSize(char);
extern  void            GArgList(entry_pt *,uint,PTYPE);
extern  void            GArgInfo(sym_id,PTYPE,PTYPE);
extern  sym_id          FindShadow(sym_id);


static  void    GetImplType( sym_id sym ) {
//=========================================

// Get the implicit type of a symbol.
// The following is to dump the proper function type if:
//    FUNCTION F()        - Type is REAL
//    IMPLICIT REAL*8 (F) - Type is now DOUBLE PRECISION
//    RETURN              - When we dump ARGLIST here, we better
//    END                   update the type
// and to dump the proper dummy argument type if:
//    FUNCTION F(I)       - Type if I is REAL
//    IMPLICIT REAL*8 (I) - Type is now DOUBLE PRECISION
//    RETURN              - When we dump ARGLIST here, we better
//    END                   update the type

    if( ( sym->u.ns.flags & SY_TYPE ) == 0 ) {
        sym->u.ns.flags |= SY_TYPE;
        sym->u.ns.u1.s.typ = ImplType( sym->u.ns.name[ 0 ] );
        sym->u.ns.xt.size = ImplSize( sym->u.ns.name[ 0 ] );
    }
}


static  void    ChkEntryType( sym_id sym, sym_id entry ) {
//========================================================

    // when we compile ENTRY statement, we make sure that its class
    // matches the class of the main entry
    if( ( sym->u.ns.flags & SY_SUBPROG_TYPE ) == SY_SUBROUTINE ) return;
    if( (entry->u.ns.u1.s.typ == FT_CHAR) || (entry->u.ns.u1.s.typ == FT_STRUCTURE) ) {
        if( sym->u.ns.u1.s.typ != entry->u.ns.u1.s.typ ) {
            NamNamErr( EY_TYPE_MISMATCH, entry, sym );
        } else {
            if( entry->u.ns.u1.s.typ == FT_STRUCTURE ) {
                if( entry->u.ns.xt.record != sym->u.ns.xt.record ) {
                    NamNamErr( EY_TYPE_MISMATCH, entry, sym );
                }
            } else {
                if( sym->u.ns.xt.size != entry->u.ns.xt.size ) {
                    NamNamErr( EY_SIZE_MISMATCH, entry, sym );
                }
            }
        }
    } else if( (sym->u.ns.u1.s.typ == FT_CHAR) || (sym->u.ns.u1.s.typ == FT_STRUCTURE) ) {
        NamNamErr( EY_TYPE_MISMATCH, sym, SubProgId );
    }
}


void    DumpEntries(void) {
//=====================

// Dump argument lists.

    parameter   *curr_parm;
    entry_pt    *dum_lst;

    uint        args_cnt;
    PTYPE       code;
    PTYPE       typ;
    unsigned_16 flags;
    sym_id      sym;
    sym_id      fn_shadow;

    dum_lst = Entries;
    while( dum_lst != NULL ) {
        args_cnt = 0;
        curr_parm = dum_lst->parms;
        while( curr_parm != NULL ) {
            ++args_cnt;
            curr_parm = curr_parm->link;
        }
        sym = dum_lst->id;
        GetImplType( sym );
        ChkEntryType( sym, SubProgId );
        typ = PT_NOTYPE;
        if( ( sym->u.ns.flags & SY_SUBPROG_TYPE ) == SY_FUNCTION ) {
            fn_shadow = FindShadow( sym );
            fn_shadow->u.ns.xt.size = sym->u.ns.xt.size;
            fn_shadow->u.ns.u1.s.typ = sym->u.ns.u1.s.typ;
            typ = ParmType( sym->u.ns.u1.s.typ, sym->u.ns.xt.size );
            if( ( typ == PT_CHAR ) && ( sym->u.ns.xt.size == 0 ) ) {
                typ |= VAR_LEN_CHAR;
            }
        }
        if( ( ProgSw & PS_ERROR ) == 0 ) {
            GArgList( dum_lst, args_cnt, typ );
        }
        curr_parm = dum_lst->parms;
        while( curr_parm != NULL ) {
            if( curr_parm->flags & ARG_STMTNO ) {
                typ = PT_NOTYPE;
                code = PC_STATEMENT;
            } else {
                sym = curr_parm->id;
                GetImplType( sym );
                typ = ParmType( sym->u.ns.u1.s.typ, sym->u.ns.xt.size );
                flags = sym->u.ns.flags;
                if( ( flags & SY_CLASS ) == SY_SUBPROGRAM ) {
                    code = PC_FN_OR_SUB;
                    if( ( flags & SY_SUBPROG_TYPE ) != SY_FN_OR_SUB ) {
                        code = PC_PROCEDURE;
                        if( ( flags & SY_SUBPROG_TYPE ) != SY_FUNCTION ) {
                            typ = PT_NOTYPE;
                        }
                    }
                } else {
                    code = PC_VARIABLE;
                    if( ( flags & SY_SUBSCRIPTED ) != 0 ) {
                        code = PC_ARRAY_NAME;
                    }
                    if( ( typ == PT_CHAR ) && ( sym->u.ns.xt.size == 0 ) ) {
                        typ |= VAR_LEN_CHAR;
                    }
                }
            }
            if( ( ProgSw & PS_ERROR ) == 0 ) {
                GArgInfo( sym, code, typ );
            }
            curr_parm = curr_parm->link;
        }
        dum_lst = dum_lst->link;
    }
}


void    EnPurge(void) {
//=================

// Free up all the entry list information.

    parameter   *curr_parm;
    entry_pt    *dum_lst;
    pointer     next;

    dum_lst = Entries;
    while( dum_lst != NULL ) {
        curr_parm = dum_lst->parms;
        while( curr_parm != NULL ) {
            next = curr_parm->link;
            FMemFree( curr_parm );
            curr_parm = next;
        }
        next = dum_lst->link;
        FMemFree( dum_lst );
        dum_lst = next;
    }
    Entries = NULL;
}
