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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


//
// COMDUMP   : COMMON block resolution
//

#include "ftnstd.h"
#include "global.h"
#include "errcod.h"
#include "ecflags.h"
#include "brseinfo.h"
#include "ferror.h"
#include "insert.h"

extern  void            SetComBlkSize(sym_id,intstar4);
extern  void            BIStartComBlock( sym_id ste_ptr );
extern  void            BIOutComSymbol( sym_id ste_ptr );
extern  void            BIEndComBlock( void );

#define CT_MIXED        0
#define CT_CHAR         1
#define CT_NOT_CHAR     2

static  int    ClassifyType( TYPE typ )
{
// Classify the specified type into CHARACTER or NON-CHARACTER.

    if( typ == FT_CHAR ) return( CT_CHAR );
    return( CT_NOT_CHAR );
}

void    STComDump( void )
{
// Resolve common block offsets and check for errors.

    sym_id      sym;
    int         common_type;
    sym_id      name_in_com;
    sym_id      extend_beg;
    sym_id      com_blk;
    com_eq      *eq_ext;
    signed_32   offset;
    unsigned_32 common_size;
    unsigned_32 end_common;
    unsigned_32 size;

    com_blk = BList;
    while( com_blk != NULL ) {
        if( ( com_blk->ns.si.cb.first == NULL ) &&
            ( com_blk->ns.flags & SY_SAVED ) ) {
            NameErr( SA_COMBLK_EMPTY, com_blk );
        } else {
            extend_beg = NULL;
            common_size = 0;
            end_common = 0;
            name_in_com = com_blk->ns.si.cb.first;
            BIStartComBlock( com_blk );
            // in case common block empty because of errors
            if( name_in_com == NULL ) break;
            common_type = ClassifyType( name_in_com->ns.u1.s.typ );
            for(;;) {
                sym = name_in_com;
                if( ( common_type != CT_MIXED ) &&
                    ( common_type != ClassifyType( sym->ns.u1.s.typ ) ) ) {
                    Extension( CM_MIXED_COMMON );
                    common_type = CT_MIXED;
                }
                size = _SymSize( sym );
                if( sym->ns.flags & SY_SUBSCRIPTED ) {
                    size *= sym->ns.si.va.u.dim_ext->num_elts;
                    sym->ns.si.va.u.dim_ext->dim_flags &= ~DIM_PVD;
                }
                if( sym->ns.flags & SY_IN_EQUIV ) {
                    com_blk->ns.flags |= SY_EQUIVED_NAME;
                    offset = 0;
                    for(;;) {
                        eq_ext = sym->ns.si.va.vi.ec_ext;
                        if( eq_ext->ec_flags & LEADER ) break;
                        offset += eq_ext->offset;
                        sym = eq_ext->link_eqv;
                    }
                    eq_ext->com_blk = com_blk;
                    if( offset - eq_ext->low > common_size ) {
                        extend_beg = sym;
                    } else {
                        if( eq_ext->ec_flags & HAVE_COMMON_OFFSET ) {
                            if( eq_ext->offset != common_size - offset ) {
                                NameErr( EV_DIFF_REL_POS, sym );
                            }
                        }
                        eq_ext->offset = common_size - offset;
                        eq_ext->ec_flags |= HAVE_COMMON_OFFSET;
                    }
                    offset = eq_ext->high - offset;
                    if( offset > end_common ) {
                        end_common = common_size + offset;
                    }
                } else {
                    sym->ns.si.va.vi.ec_ext->offset = common_size;
                }
                common_size += size;
                if( common_size > end_common ) {
                    end_common = common_size;
                }
                BIOutComSymbol( name_in_com );
                eq_ext = name_in_com->ns.si.va.vi.ec_ext;
                if( eq_ext->ec_flags & LAST_IN_COMMON ) break;
                name_in_com = eq_ext->link_com;
            }
            BIEndComBlock();
            if( extend_beg != NULL ) {
                NamNamErr( EC_PAST_BEG, extend_beg, com_blk );
            }
            SetComBlkSize( com_blk, end_common );
        }
        com_blk = com_blk->ns.link;
    }
}

