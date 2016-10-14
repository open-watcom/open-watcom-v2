/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Insert FWAIT instructions as necessary.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "data.h"
#include "insutil.h"
#include "optab.h"
#include "optimize.h"


extern  instruction     *MakeWait(void);

static  bool    OpsCollide( name *op_87, name *op_86 ) {
/****************************************************/

    if( op_87->n.class == N_INDEXED ) {
        if( op_86 == NULL ) return( false );
        if( op_86->n.class == N_INDEXED ) {
            if( op_87->i.base == NULL ) return( true );
            if( op_86->i.base == NULL ) return( true );
            return( SameThing( op_87->i.base, op_86->i.base ) );
        }
        if( op_86->n.class != N_MEMORY && op_86->n.class != N_TEMP ) {
            return( false );
        }
        if( op_86->v.usage & ( NEEDS_MEMORY | USE_ADDRESS ) ) {
            if( op_87->i.base != NULL ) {
                return( SameThing( op_87->i.base, op_86 ) );
            }
            return( true );
        }
    } else if( SameThing( op_87, op_86 ) ) {
        return( true );
    }
    return( false );
}


static  bool    CallNeedsWait( name *op, name *res ) {
/****************************************************/

    if( op == NULL ) {
        op = res;
    }
    if( op == NULL ) return( false );
    if( op->n.class == N_REGISTER ) return( false );
    if( op->n.class == N_CONSTANT ) return( false );
    if( op->n.class == N_INDEXED ) return( true );
    if( op->v.usage & (USE_ADDRESS | NEEDS_MEMORY) ) return( true );
    return( false );
}


static  bool    NeedWait( name *op, name *res, instruction *ins_86 ) {
/********************************************************************/

    bool        need_wait;
    int         i;

    if( op != NULL ) {
        need_wait = OpsCollide( op, ins_86->result );
    } else {
        if( !DoesSomething( ins_86 ) ) return( false );
        need_wait = OpsCollide( res, ins_86->result );
        for( i = ins_86->num_operands; i-- > 0; ) {
            need_wait |= OpsCollide( res, ins_86->operands[i] );
        }
    }
    return( need_wait );
}


extern  void    Wait8087( void ) {
/**************************/

    block       *blk;
    instruction *ins;
    instruction *last_non_fpins = NULL;
    instruction *last_fpins;
    gentype     gen;
    name        *last_fpop = NULL;
    name        *last_fpres = NULL;
    bool        past_jump;

    /* Prior to 386, FWAITs are always needed. On 386 and up, only when
     * user explicitly asked for them.
     */
    if( _CPULevel( CPU_386 ) && _IsntTargetModel( GEN_FWAIT_386 ) ) return;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        last_fpins = NULL;
        past_jump = false;
        for( ins = blk->ins.hd.next; ; ins = ins->head.next ) {
            for( ; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
                if( _OpIsCall( ins->head.opcode ) ) break;
                gen = G( ins );
                if( _GenIs8087( gen ) ) {
                    last_non_fpins = NULL;
                    switch( gen ) {
                    case G_MFLD:
                    case G_MNFBIN:
                    case G_MRFBIN:
                        last_fpop = ins->operands[0];
                        last_fpres = NULL;
                        last_fpins = ins;
                        break;
                    case G_MFSTNP:
                    case G_MFST:
                    case G_MFSTRND:
                        last_fpins = ins;
                        last_fpres = ins->result;
                        last_fpop = NULL;
                        break;
                    default:
                        last_fpins = NULL;
                        break;
                    }
                } else {
                    if( past_jump || _OpIsJump( ins->head.opcode ) ) {
                        past_jump = true;
                    } else {
                        last_non_fpins = ins;
                        if( last_fpins != NULL
                          && NeedWait( last_fpop, last_fpres, ins ) ) {
                            PrefixIns( ins, MakeWait() );
                            last_fpins = NULL;
                        }
                    }
                }
            }
            if( ins->head.opcode == OP_BLOCK ) break;
            if( last_fpins != NULL && CallNeedsWait( last_fpop, last_fpres ) ) {
                PrefixIns( ins, MakeWait() );
                last_fpins = NULL;
            }
        }
        if( last_fpins != NULL
          && ( ( last_fpres != NULL ) || ( last_fpop != NULL ) ) ) {
            if( last_non_fpins != NULL ) {
                SuffixIns( last_non_fpins, MakeWait() );
            } else {
                SuffixIns( last_fpins, MakeWait() );
            }
        }
    }
}
