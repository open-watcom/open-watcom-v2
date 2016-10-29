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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "cgdefs.h"
#include "procdef.h"
#include "score.h"
#include "zerobits.h"
#include "makeins.h"
#include "namelist.h"
#include "regset.h"
#include "expand.h"
#include "split.h"
#include "insutil.h"
#include "insdead.h"


static  bool    CheckIns( instruction **pins ) {
/**********************************************/

    bool                ok;
    opcode_entry        *try;
    instruction         *ins;
    bool                dummy;
    int                 i;

    ins = *pins;
    ok = true;
    if( ins != NULL ) {
        for( i = ins->num_operands; i-- > 0; ) {
            if( ins->operands[i]->n.class == N_REGISTER &&
                ins->operands[i]->r.reg_index == -1 ) {
                ok = false;
            }
        }
        if( ok && ins->result != NULL ) {
            if( ins->result->n.class == N_REGISTER &&
                ins->result->r.reg_index == -1 ) {
                ok = false;
            }
        }
        if( ok ) {
            try = FindGenEntry( ins, &dummy );
            if( try == NULL ) {
                ok = false;
            } else if( try->generate >= G_UNKNOWN ) {
                ok = false;
            } else {
                ins->table = try;
                ins->u.gen_table = try;
            }
        }
    }
    if( !ok ) {
        ins->head.next = ins;
        ins->head.prev = ins;
        FreeIns( ins );
        *pins = NULL;
    }
    return( ok );
}


static  name    *NonZeroPart( name *op, zero_bits z, type_class_def class ) {
/******************************************************************/

    if( z & LO_HALF ) {
        return( HighPart( op, HalfClass[class] ) );
    } else if( z & HI_HALF ) {
        return( LowPart( op, HalfClass[class] ) );
    } else {
        return( op );
    }
}


static  zero_bits       AZeroHalf( zero_bits l, zero_bits r ) {
/*************************************************************/

    zero_bits   zhalf;

    zhalf = l & LO_HALF;
    if( zhalf ) return( zhalf );
    zhalf = l & HI_HALF;
    if( zhalf ) return( zhalf );
    zhalf = r & LO_HALF;
    if( zhalf ) return( zhalf );
    zhalf = r & HI_HALF;
    return( zhalf );
}

static  instruction     *MakeClear( name *res, type_class_def class ) {
/*********************************************************************/

    if( res->n.class == N_REGISTER ) { /* since there's a R_MAKEXORRR*/
        return( MakeBinary( OP_XOR, res, res, res, class ) );
    } else {
        return( MakeMove( AllocIntConst( 0 ), res, class ) );
    }
}


extern  bool    ScoreZero( score *sc, instruction **pins ) {
/***********************************************************/

    instruction         *ins;
    bool                change;
    type_class_def      class;
    type_class_def      hc;
    zero_bits           op1zpart;
    zero_bits           op2zpart;
    zero_bits           zeropart;
    zero_bits           nonzeropart;
    name                *op1;
    name                *op2;
    name                *res;
    instruction         *ins1;
    instruction         *ins2;
    name                *tmp1;
    name                *tmp2;
    name                *tmp3;
    name                *tmp4;


#define NonZeroOP2Part( x )  NonZeroPart( op2, x, hc )
#define NonZeroOP1Part( x )  NonZeroPart( op1, x, hc )
#define NonZeroRESPart( x )  NonZeroPart( res, x, hc )

    ins = *pins;
    if( ins->num_operands != 2 ) return( false );
    if( ins->result == NULL ) return( false );
    if( ( ins->head.opcode != OP_ADD && ins->head.opcode != OP_AND
       && ins->head.opcode != OP_OR && ins->head.opcode != OP_XOR
       && ins->head.opcode != OP_SUB ) ) return( false );
    if( ins->ins_flags & INS_CC_USED ) return( false );
    if( VolatileIns( ins ) ) return( false );
    op1 = ins->operands[0];
    op2 = ins->operands[1];
    res = ins->result;
    if( op1->n.size != op2->n.size ) return( false );
    if( res->n.size != op2->n.size ) return( false );
    class = ins->type_class;
    op1zpart = HasZero( sc, op1 );
    op2zpart = HasZero( sc, op2 );
    hc = HalfClass[class];
    ins1 = NULL;
    ins2 = NULL;
    change = false;
    switch( ins->head.opcode ) {
    case OP_ADD:
        if( _IsZero( op1zpart ) && _IsZero( op2zpart ) ) {
            ins1 = MakeClear( res, class );
        } else if( _IsZero( op1zpart ) ) {
            ins1 = MakeMove( op2, res, class );
        } else if( _IsZero( op2zpart ) ) {
            ins1 = MakeMove( op1, res, class );
        } else if( _OpposZero( op1zpart, op2zpart ) ) {
            tmp1 = NonZeroOP1Part( op1zpart );
            tmp2 = NonZeroRESPart( op1zpart );
            tmp3 = NonZeroOP2Part( op2zpart );
            tmp4 = NonZeroRESPart( op2zpart );
            if( tmp1 == 0 || tmp2 == 0 || tmp3 == 0 || tmp4 == 0 )
                return( false );
            ins1 = MakeMove( tmp1, tmp2, hc );
            ins2 = MakeMove( tmp3, tmp4, hc );
        } else {
            zeropart = _ZeroHalf( op1zpart, op2zpart );
            if( _LoZero( zeropart ) ) {
                nonzeropart = _OtherHalf( zeropart );
                tmp1 = NonZeroRESPart( nonzeropart );
                tmp2 = NonZeroOP1Part( zeropart );
                tmp3 = NonZeroOP2Part( zeropart );
                tmp4 = NonZeroRESPart( zeropart );
                if( tmp1 == 0 || tmp2 == 0 || tmp3 == 0 || tmp4 == 0 )
                    return( false );
                ins2 = MakeClear( tmp1, hc );
                ins1 = MakeBinary( OP_ADD, tmp2, tmp3, tmp4, hc );
            }
        }
        break;
    case OP_SUB:
        if( _IsZero( op1zpart ) && _IsZero( op2zpart ) ) {
            ins1 = MakeClear( res, class );
        } else if( _IsZero( op2zpart ) ) {
            ins1 = MakeMove( op1, res, class );
        } else if( _IsZero( op1zpart ) ) {
            ins1 = MakeUnary( OP_NEGATE, op2, res, class );
        }
        break;
    case OP_AND:
        if( _IsZero( op1zpart ) || _IsZero( op2zpart ) || _OpposZero( op1zpart,op2zpart ) ) {
            ins1 = MakeClear( res, class );
        } else {
            zeropart = AZeroHalf( op1zpart, op2zpart );
            if( zeropart ) {
                nonzeropart = _OtherHalf( zeropart );
                tmp1 = NonZeroRESPart( nonzeropart );
                tmp2 = NonZeroOP1Part( zeropart );
                tmp3 = NonZeroOP2Part( zeropart );
                tmp4 = NonZeroRESPart( zeropart );
                if( tmp1 == 0 || tmp2 == 0 || tmp3 == 0 || tmp4 == 0 )
                    return( false );
                ins1 = MakeClear( tmp1, hc );
                ins2 = MakeBinary( OP_AND, tmp2, tmp3, tmp4, hc );
            }
        }
        break;
    case OP_OR:
    case OP_XOR:
        if( _IsZero( op1zpart ) && _IsZero( op2zpart ) ) {
            if( op1 == res || op2 == res ) { /* result already has zero*/
                *pins = ins->head.next;
                FreeIns( ins );
                change = true;
            } else {
                ins1 = MakeClear( res, class );
            }
        } else if( _IsZero( op1zpart ) ) {
            ins1 = MakeMove( op2, res, class );
        } else if( _IsZero( op2zpart ) ) {
            ins1 = MakeMove( op1, res, class );
        } else if( _OpposZero( op1zpart, op2zpart ) ) {
            tmp1 = NonZeroOP1Part( op1zpart );
            tmp2 = NonZeroRESPart( op1zpart );
            tmp3 = NonZeroOP2Part( op2zpart );
            tmp4 = NonZeroRESPart( op2zpart );
            if( tmp1 == 0 || tmp2 == 0 || tmp3 == 0 || tmp4 == 0 )
                return( false );
            ins1 = MakeMove( tmp1, tmp2, hc );
            ins2 = MakeMove( tmp3, tmp4, hc );
        } else if( _MatchZero( op1zpart, op2zpart ) ) {
            zeropart = _ZeroHalf( op1zpart, op2zpart );
            nonzeropart = _OtherHalf( zeropart );
            tmp1 = NonZeroRESPart( nonzeropart );
            tmp2 = NonZeroOP1Part( zeropart );
            tmp3 = NonZeroOP2Part( zeropart );
            tmp4 = NonZeroRESPart( zeropart );
            if( tmp1 == 0 || tmp2 == 0 || tmp3 == 0 || tmp4 == 0 )
                return( false );
            ins2 = MakeClear( tmp1, hc );
            ins1 = MakeBinary( ins->head.opcode, tmp2, tmp3, tmp4, hc );
        }
        break;
    }
    if( CheckIns( &ins1 ) == false && ins2 != NULL ) {
        ins2->head.next = ins2;
        ins2->head.prev = ins2;
        FreeIns( ins2 );
        ins2 = NULL;
    }
    if( CheckIns( &ins2 ) == false && ins1 != NULL ) {
        ins1->head.next = ins1;
        ins1->head.prev = ins1;
        FreeIns( ins1 );
        ins1 = NULL;
    }
    if( ins1 == NULL && ins2 == NULL ) return( change );
    if( ins2 != NULL ) {
        SuffixIns( ins, ins2 );
    }
    if( ins1 != NULL ) {
        SuffixIns( ins, ins1 );
    }
    FreeIns( ins );
    *pins = ins1;
    return( true );
}
