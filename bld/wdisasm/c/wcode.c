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
* Description:  Weitek coprocessor disassembly.
*
****************************************************************************/


#include <string.h>
#include "disasm.h"

#define DO_ENUM
#include "wcode.h"
#undef  DO_ENUM
#include "wcode.h"

uint_32         WtkAddr;
char            WTLBASEStr[] = "WTLBASE";
char            WTLSEGStr[] = "WTLSEG";
char            WtlsegPresent = FALSE;
static int      WcodeErr;
static char     didAnyWtk = FALSE;
static operand  LocalOp[ 3 ];

/*
 * Tells whether CurrIns is Weitek
 * Note: This routine should set the global WtkAddr to be the ABACUS offset
 *       in order for DoWtk to work correctly. The reasons that this should
 *       be done here are two folds:
 *       1. The way in finding the ABACUS offset is project specific.
 *       2. In the course of testing whether curr_ins is Weitek, it is just
 *          one step further in finding the offset.
 */
#if defined( O2A )
int IsWtk( instruction * curr_ins )
//=================================
{
    operand *   op  = &curr_ins->op[ curr_ins->mem_ref_op ];
    char *      sym = FindSymbol( InsAddr + op->offset );
    int         retn = FALSE;

    if( /* Pass == 2  &&   so that didAnyWtk can be set */
        ( Options & FORM_DO_WTK ) ) {
        if( strstr( sym, WTLBASEStr ) != NULL ) {
            fixup *     fix = FindFixup( InsAddr + op->offset, Segment );
            if( fix != NULL ) {
                WtkAddr = fix->imp_address + ( fix->seg_address << 4 );
                retn = TRUE;
            }
        } else if( WtlsegPresent  &&  ( curr_ins->pref & PREF_FS )  &&
                   curr_ins->seg_used == FS_REG  &&  sym == NULL ) {
            WtkAddr = op->disp;
            retn = TRUE;
        }
    }
    if( retn == TRUE ) {
        didAnyWtk = TRUE;
    }
    if( Pass == 2 ) {
        return( retn );
    } else {
        return( FALSE );
    }
}
#endif


static void setoperand( int set_num, wop_type op_type, int src_num )
//==================================================================
{
    uint_16     where;

    if( src_num == 0 ) {   /* "TO" */
        where = ( WtkAddr & 0x7f ) >> 2;
    } else {  /* "FROM" */
        where = ( ( WtkAddr & 0x380 ) >> 5 ) | ( WtkAddr & 0x3 );
    }
    if( ( where & 1 )  && ( op_type != ANY_SINGLE  &&  op_type != WST ) ) {
        WcodeErr = TRUE;
        return;
    }
    if( where != 0 ) {  /* 0 means to/from data bus */
        if( op_type == EREG  ||  op_type >= REAL_ANY ) {
            WcodeErr = TRUE;
        } else {
            operand * op;
            memcpy( &LocalOp[ set_num ], &CurrIns.op[ src_num ], sizeof( operand ) );
            op = &LocalOp[ set_num ];
            op->mode = ADDR_WTK;
            op->size = (uint_8) op_type;
            op->disp = where;
        }
    } else if( op_type == ANY_SINGLE  ||  op_type == ANY_DOUBLE  ||
               op_type == EREG ) {
        LocalOp[ set_num ] = CurrIns.op[ src_num ];
    } else if( op_type >= REAL_ANY ) {
        wop_type    tmp_typ = ( op_type - REAL_ANY ) & 0xf;
        int         tmp_src = op_type >> 7;
        if( tmp_typ == WST  ||  tmp_typ == WDT  ||
            ( tmp_typ == EREG  && CurrIns.op[ tmp_src ].mode != ADDR_REG ) ) {
            WcodeErr = TRUE;
        } else {
            LocalOp[ set_num ] = CurrIns.op[ tmp_src ];
        }
    } else {
        WcodeErr = TRUE;
    }
}


char * GetWtkInsName( uint_16 wopcode )
//=====================================
{
    char    instr_type = wopcode >> 5;

    wopcode &= 0x1f;    /* throw away bit 5 */
    if( wopcode > WFIX ) {  /* non-common wopcode */
        wopcode &= 0xf;
        if( instr_type ) {  /* double specific */
            return( DoubleWcode[ wopcode ] );
        } else {
            return( SingleWcode[ wopcode ] );
        }
    }
    return( CommonWcode[ wopcode ] );
}


void DoWtk( void )
//================
{
    uint_16     instr;
    char        instr_type;
    int         numop = 2;

    WcodeErr = FALSE;
    instr = WtkAddr >> 10;
    instr_type = ( instr >> 5 );
    instr &= 0x1F;          /* throw away bit 5 */
    if( instr > WFSUB ) {   /* unknown Weitek instr */
        return;
    }
    if( instr > WFIX ) {
        if( instr_type ) {  /* instr specific for double */
            switch( instr ) {
              case WFLDCTX:
                setoperand( 0, EREG, 1 );
                numop = 1;
                break;
              case WFSTCTX:
                setoperand( 0, EREG, 0 );
                if( CurrIns.opcode == I_MOV ) {
                    numop = 1;
                } else {
                    LocalOp[ 1 ].mode = ADDR_WTK_OPCODE;
                    LocalOp[ 1 ].disp = CurrIns.opcode;
                }
                break;
              case WFMACD:
                setoperand( 0, WST, 0 );
                setoperand( 1, ANY_SINGLE, 1 );
                break;
              case WFLDD:   /* can be WFSTD too */
                setoperand( 0, EREG + REAL_ANY, 1 );
                memcpy( DoubleWcode[ WFLDD & 0xf ] + 2,
                        WcodeErr ? "ld" : "st", 2 );
                if( WcodeErr ) {    /* it's a WFLDD */
                    WcodeErr = FALSE;
                    setoperand( 0, WDT, 0 );
                    setoperand( 1, WDT, 1 );
                    if( WcodeErr == TRUE ) {
                        WcodeErr = FALSE;
                        setoperand( 1, ( EREG + REAL_ANY ) | 0x80, 1 );
                    }
                } else {
                    setoperand( 1, WDT, 0 );
                }
                break;
              case WFSQRT:
                setoperand( 0, WDT, 0 );
                setoperand( 1, WDT, 1 );
                break;
              case WFSUB:
                setoperand( 0, WDT, 0 );
                setoperand( 1, ANY_DOUBLE, 1 );
                break;
            }
        } else {            /* instr specific for single */
            switch( instr ) {
              case WFCVT:   /* there're two of these */
                setoperand( 0, WST, 0 );
                setoperand( 1, ANY_DOUBLE, 1 );
                break;
              case ( WFCVT + 1 ):
              case WFMACDD:
                setoperand( 0, WDT, 0 );
                setoperand( 1, ANY_SINGLE, 1 );
                break;
              case WFSQRT:
                setoperand( 0, WST, 0 );
                setoperand( 1, WST, 1 );
                break;
              case WFMAC:
              case WFSUB:
                setoperand( 0, WST, 0 );
                setoperand( 1, ANY_SINGLE, 1 );
            }
        }
    } else {    /* common single/double instr */
        switch( instr ) { /* look for special cases */
          case WFLD:
            setoperand( 0, WST, 0 );
            if( CurrIns.opcode == I_POP ) {
                WtkAddr = WFPOP << 10;
                numop = 1;
            } else {
                setoperand( 1, ANY_SINGLE, 1 );
            }
            break;
          case WFST:
            if( CurrIns.opcode == I_PUSH ) {
                setoperand( 0, WST, 0);
                WtkAddr = WFPUSH << 10;
                numop = 1;
            } else {
                setoperand( 0, EREG + REAL_ANY, 1 );
                setoperand( 1, WST, 0 );
                if( CurrIns.opcode != I_MOV ) {
                    LocalOp[ 2 ].mode = ADDR_WTK_OPCODE;
                    LocalOp[ 2 ].disp = CurrIns.opcode;
                    numop = 3;
                }
            }
            break;
          case WFTSTT:  /* single Wxt operand */
          case WFTST:   /* single Wxt operand */
            setoperand( 0, instr_type ? WDT : WST, 1 );
            if( !WcodeErr ) {
                setoperand( 1, WST, 0 );
                if( WcodeErr ) {
                    WcodeErr = FALSE;
                    numop = 1;
                } else if( LocalOp[ 1 ].disp != 1 ) {   /* error if not ws1 */
                    WcodeErr = TRUE;
                }
            }
            break;
          case WFIX:
            setoperand( 0, WST, 0 );
            setoperand( 1, instr_type ? WDT : WST, 1 );
            break;
          default:      /* plain:   oper    Wxt,any */
            setoperand( 0, instr_type ? WDT : WST, 0 );
            setoperand( 1, instr_type ? ANY_DOUBLE : ANY_SINGLE, 1 );
            break;
        }
    }
    if( !WcodeErr ) {
        CurrIns.modifier = MOD_NONE;   /* prevent printing of modifier */
        CurrIns.seg_used = NULL_REG;   /* prevent printing of prefix */
        CurrIns.num_oper = numop;
        CurrIns.opcode = ( WtkAddr >> 10 ) + FIRST_WTK_INS;
        memcpy( CurrIns.op, LocalOp, sizeof( LocalOp ) );
        // mov the new operands
    }
}

int HaveWtk( void )
/*****************/
{
    return( didAnyWtk );
}
