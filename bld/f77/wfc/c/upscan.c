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
* Description:  Upscan phase of expression handling.
*
****************************************************************************/


#include "ftnstd.h"
#include "opr.h"
#include "optr.h"
#include "opn.h"
#include "errcod.h"
#include "astype.h"
#include "iflookup.h"
#include "global.h"
#include "cpopt.h"
#include "prmcodes.h"
#include "recog.h"
#include "emitobj.h"
#include "types.h"
#include "ferror.h"
#include "insert.h"
#include "utility.h"
#include "convert.h"

extern  void            DetSubList(void);
extern  void            DetCallList(void);
extern  void            DetSStr(void);
extern  void            MarkIFUsed(IFF);
extern  void            GMakeCplx(void);
extern  void            GMakeDCplx(void);
extern  void            GMakeXCplx(void);
extern  void            GArg(void);
extern  void            GILCnvTo(TYPE,uint);
extern  void            UpdateNode(itnode *,itnode *);
extern  void            BackTrack(void);
extern  void            MoveDown(void);
extern  void            KillOpnOpr(void);
extern  void            AdvError(int);
extern  void            EndExpr(void);
extern  sym_id          STConst(void *,TYPE,uint);
extern  sym_id          STLit(byte *,int);
extern  byte            ParmCode(itnode *);
extern  void            FiniCat(void);
extern  void            CatBack(void);
extern  void            CatAxeParens(void);
extern  void            CatOpn(void);
extern  void            ChkCatOpn(void);
extern  void            CatParen(void);
extern  void            ParenCat(void);
extern  void            ProcList(itnode *);
extern  void            GIChar(void);
extern  void            GModulus(void);
extern  void            GCharLen(void);
extern  void            GImag(void);
extern  void            GMax(int);
extern  void            GMin(int);
extern  void            GConjg(void);
extern  void            GDProd(void);
extern  void            GXProd(void);
extern  void            GSign(void);
extern  void            GBitTest(void);
extern  void            GBitSet(void);
extern  void            GBitClear(void);
extern  void            GBitOr(void);
extern  void            GBitAnd(void);
extern  void            GBitNot(void);
extern  void            GBitExclOr(void);
extern  void            GBitChange(void);
extern  void            GBitLShift(void);
extern  void            GBitRShift(void);
extern  void            GMod(void);
extern  void            GAbs(void);
extern  void            GASin(void);
extern  void            GACos(void);
extern  void            GATan(void);
extern  void            GATan2(void);
extern  void            GLog(void);
extern  void            GLog10(void);
extern  void            GCos(void);
extern  void            GSin(void);
extern  void            GTan(void);
extern  void            GSinh(void);
extern  void            GCosh(void);
extern  void            GTanh(void);
extern  void            GSqrt(void);
extern  void            GExp(void);
extern  void            GVolatile(void);
extern  void            GLoc(void);
extern  void            GAllocated(void);
extern  sym_id          FindStruct(char *,int);
extern  void            SetDefinedStatus(void);

extern  void            (* const __FAR GenOprTable[])(TYPE, TYPE, OPTR);
extern  void            (* const __FAR ConstTable[])(TYPE, TYPE, OPTR);

/* Forward declarations */
static  void    InlineCnvt( void );
static  void    IFPrmChk( void );
static  void    EvalList( void );
static  void    FixList( void );
static  void    PrepArg( void );
static  void    LowColon( void );
static  void    AddSS( int number );

void    AddConst( itnode *node );



static const OPTR __FAR OprNum[] = {
    #define pick(id,opr_index,proc_index) proc_index,
    #include "oprdefn.h"
    #undef pick
};

// Constants below used in UPSCAN for operator sequence table and are
// modified (along with UPSCAN) if operators are added/deleted/modified

//  dimensions of operator sequence table

#define OPR_CLASSES             18
#define OPR_SEQ_MAT_SIZE        (OPR_CLASSES*OPR_CLASSES-1)     //  base 0
#define OPR_SEQ_MAT_COLS        OPR_CLASSES

enum {
    OPRI_PHI,         //  null operator
    OPRI_TRM,         //  start/terminate symbol
    OPRI_FBR,         //  function/array bracket
    OPRI_LBR,         //  (
    OPRI_COM,         //  ,
    OPRI_COL,         //  :
    OPRI_EQU,         //  =
    OPRI_EQV_NEQV,    //  .EQV., .NEQV.
    OPRI_OR,          //  .OR.
    OPRI_AND,         //  .AND.
    OPRI_NOT,         //  .NOT.
    OPRI_REL,         //  .EQ.,.NE.,.LT.,.GE.,.LE.,.GT.
    OPRI_PLS_MIN,     //  +,-
    OPRI_MUL_DIV,     //  *,/
    OPRI_EXP,         //  **
    OPRI_CAT,         //  //
    OPRI_RBR,         //  )
    OPRI_FLD          //  %,.
};

typedef enum {
    #define pick(id,proc) id,
    #include "rtntable.h"
    #undef pick
} move;

static  const move    __FAR OprSeqMat[] = {
//                                                                       |o   /
//                                                                       |p  /
//                                                                       |r /
//                                                                       |2/
// phi |- [   (   ,   :   =   eqv OR  AND NOT rel +-  */  **  //  )   %  |/opr1
//                                                                       *-----
  MO, BT, BT, BT, BT, BT, BT, BT, BT, BT, BT, BT, BT, BT, BT, BT, BT, GO, // phi
  MO, EE, CA, RP, BS, BS, GO, GO, GO, GO, GO, GO, GO, GO, GO, FC, BB, GO, // |-
  MO, BB, CA, RP, PA, LC, BE, GO, GO, GO, GO, GO, GO, GO, GO, FC, PA, GO, // [
  MO, BB, CA, RP, BT, GC, BE, GO, GO, GO, GO, GO, GO, GO, GO, CR, PE, GO, // (
  MO, BS, CA, RP, PA, BS, BE, GO, GO, GO, GO, GO, GO, GO, GO, FC, PA, GO, // ,
  MO, BS, CA, RP, BS, BS, BE, BC, BC, BC, BC, BC, GO, GO, GO, BC, HC, GO, // :
  MO, EV, CA, RP, BS, BS, GO, GO, GO, GO, GO, GO, GO, GO, GO, CB, BB, GO, // =
  MO, BT, CA, RP, BT, BC, BE, BT, GO, GO, GO, GO, GO, GO, GO, FC, BT, GO, // eqv
  MO, BT, CA, RP, BT, BC, BE, BT, GO, GO, GO, GO, GO, GO, GO, FC, BT, GO, // OR
  MO, BT, CA, RP, BT, BC, BE, BT, BT, GO, GO, GO, GO, GO, GO, FC, BT, GO, // AND
  MO, BT, CA, RP, BT, BC, BE, BT, BT, BT, BS, GO, GO, GO, GO, FC, BT, GO, // NOT
  MO, BT, CA, RP, BT, BC, BE, BT, BT, BT, BS, BR, GO, GO, GO, FC, BT, GO, // rel
  MO, BT, CA, RP, BT, BT, BE, BT, BT, BT, BS, BT, BT, GO, GO, CO, BT, GO, // +-
  MO, BT, CA, RP, BT, BT, BE, BT, BT, BT, BS, BT, BT, BT, GO, CO, BT, GO, // */
  MO, BT, CA, RP, BT, BT, BE, BT, BT, BT, BS, BT, BT, BT, GO, CO, BT, GO, // **
  MO, CO, CA, RP, CO, BC, BE, CO, CO, CO, CO, CO, GO, GO, GO, CO, CP, GO, // //
  MO, BT, BS, BS, BT, BT, BT, BT, BT, BT, BS, KO, BT, BT, BT, PC, BT, BT, // )
  MO, EV, CA, EV, EV, EV, EV, EV, EV, EV, EV, EV, EV, EV, EV, CO, EV, GO  // %
};

// Notes:
//        |-      -- start/terminal operator
//        [       -- function/array bracket
//        eqv     -- .EQV.,  .NEQV.
//        rel     -- .LT., .GT., .EQ., .NE., .LE., .GE.
//
//        operator1 is in row, operator2 is in column  ( e.g. op1 * op2 )

//
// LegalOprs -- specifies which operators are legal given the operand
//
// --------------------------------------------------------------------+
//   15  14  13  12  11  10   9   8   7   6   5   4   3   2   1   0    |
//                                                                     |
//   //  **   /   *   -   +   r   .   P   =   %   n   a   o   n   e    |
//                            e       H           o   n   r   e   q    |
//                            l       I           t   d       q   v    |
//                                                            v        |
// --------------------------------------------------------------------+
// NOTES : PHI -- not used
//       : rel -- relational operators

#define CT      0x8000          // //
#define RL      0x0200          // .EQ. .NE. .GT. .LT. .GE. .LE.
#define LG      0x000F          // .AND. .OR. .EQV. .NEQV. .XOR.
#define AR      0x7C40          // +, -, *, /, **, =
#define EQ      0x0040          // =
#define NONE    0x0000          // none allowed
#define NOT     0x0010          // .NOT.
#define PLMIN   0x0C00          // +, -
#define IPLMIN  0x0C10          // integer +, -, .NOT.
#define CHAR    (CT+RL+EQ)      // legal character-character ops
#define LOG     (LG+EQ)         // legal logical-logical ops
#define NUMOP   (AR+RL)         // legal number-number ops
#define INUMOP  (LG+AR+RL)      // legal integer-integer ops
#define FLDOP   0x0120          // field selection
#define FLDEQ   0x0160          // field selection, .EQ.

#define LEGALOPR_TAB_ROWS       13
#define LEGALOPR_TAB_COLS       13
#define LEGALOPR_TAB_SIZE       169

static  const unsigned_16     __FAR LegalOprsB[] = { /*                                |
                                                                                       |
 opnd1  (binary)                                                                       | opnd2
                                                                                       |
log1 log4  int1   int2   int4   real   dble   xtnd   cmplx  dcmplx xcmplex char struct |
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
                                                                                               
LOG,  LOG,  NONE,  NONE,  NONE,  NONE,  NONE,  NONE,  NONE,  NONE,  NONE,  NONE, FLDOP, // log*1
LOG,  LOG,  NONE,  NONE,  NONE,  NONE,  NONE,  NONE,  NONE,  NONE,  NONE,  NONE, FLDOP, // log*4
NONE, NONE, INUMOP,INUMOP,INUMOP,NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NONE, FLDOP, // int*1
NONE, NONE, INUMOP,INUMOP,INUMOP,NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NONE, FLDOP, // int*2
NONE, NONE, INUMOP,INUMOP,INUMOP,NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NONE, FLDOP, // int*4
NONE, NONE, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NONE, FLDOP, // real
NONE, NONE, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NONE, FLDOP, // double
NONE, NONE, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NONE, FLDOP, // extend
NONE, NONE, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NONE, FLDOP, // complex
NONE, NONE, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NONE, FLDOP, // dcomplex
NONE, NONE, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NUMOP, NONE, FLDOP, // xcomplex
NONE, NONE, NONE,  NONE,  NONE,  NONE,  NONE,  NONE,  NONE,  NONE,  NONE,  CHAR, FLDOP, // character
NONE, NONE, NONE,  NONE,  NONE,  NONE,  NONE,  NONE,  NONE,  NONE,  NONE,  NONE, FLDEQ  // structure
};


static  const unsigned_16     __FAR LegalOprsU[] = { /*

 opnd2 (unary)

log1 log4  int1   int2   int4   real   dble   xtnd   cmplx  dcmplx xcmplex char struct
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

NOT, NOT, IPLMIN,IPLMIN,IPLMIN, PLMIN, PLMIN, PLMIN, PLMIN, PLMIN, PLMIN,  NONE,  NONE
};


static const byte __FAR OprIndex[] = {
    #define pick(id,opr_index,proc_index) opr_index,
    #include "oprdefn.h"
    #undef pick
};


static  bool    SimpleScript( itnode *op ) {
//==========================================

    if( ( op->opn.us & USOPN_WHERE ) != 0 )
        return( FALSE );
    switch( op->opn.us & USOPN_WHAT ) {
    case USOPN_NNL:
    case USOPN_CON:
    case USOPN_NONE:
        return( TRUE );
    }
    return( FALSE );
}


static  int     SameScripts( itnode *op1, itnode *op2 ) {
//=======================================================

    if( !SimpleScript( op1 ) ) return( 0 );
    if( ( op1->opn.us & USOPN_WHAT ) == USOPN_NONE ) {
        if( ( op2->opn.us & USOPN_WHAT ) == USOPN_CON ) {
            return( ITIntValue( op2 ) );
        } else {
            return( 0 );
        }
    }
    if( ( op1->opn.us & USOPN_WHAT ) != ( op2->opn.us & USOPN_WHAT ) ) {
        return( 0 );
    }
    if( ( op1->opn.us & USOPN_WHAT ) == USOPN_NNL ) {
        if( op1->sym_ptr == op2->sym_ptr ) {
            return( 1 );
        }
    } else if( ( op1->opn.us & USOPN_WHAT ) == USOPN_CON ) {
        return( ITIntValue( op2 ) - ITIntValue( op1 ) + 1 );
    }
    return( 0 );
}


bool    OptimalChSize( uint size ) {
//==================================

    return( ( size == 1 ) || ( size == 2 ) || ( size == 4 ) );
}


static  void    EvalOpn( void ) {
//===============================

// Evaluate operand.

    if( CITNode->opn.us == USOPN_CON ) {
        AddConst( CITNode );
    }
    PushOpn( CITNode );
}


static  void    ParenExpr( void ) {
//=================================

// Finish off evaluation of a parenthesized expression.

    // don't evaluate constants enclosed in parentheses
    // so that they can be folded. Consider: (3+4)+5
    if( CITNode->opn.us != USOPN_CON ) {
        // Consider: CHARACTER A
        //           IF( ('9') .NE. (A) ) CONTINUE
        // make sure that we can optimize the character operation
        if( CITNode->opn.us == USOPN_NNL ) {
            if( CITNode->typ == FT_CHAR ) {
                int     ch_size;

                ch_size = CITNode->size;
                if( OptimalChSize( ch_size ) ) {
                    CITNode->value.st.ss_size = ch_size;
                    CITNode->opn.us |= USOPN_SS1;
                }
            }
        }
        PushOpn( CITNode );
        GParenExpr();
    }
    BackTrack();
}


static  void    ProcOpn( void ) {
//===============================

// Process operand and then scan backwards.
// Currently called for = -| sequence, and most % opr sequences.

    EvalOpn();
    BackTrack();
}


static  void    RemoveParen( void ) {
//===================================

// Upscan routine when second operator is '('.
//
//  Before:                           |   After:
//                 --------------     |                 --------------
//       CIT ==>   | opr1 | PHI |     |                    released**
//                 --------------     |                 --------------
//                 | (    | opn |     |        CIT ==>  | opr1 | opn |
//                 --------------     |                 --------------
//                 | )    | PHI |     |                    released
//                 --------------     |                 --------------
//                 | opr2 |     |     |                 | opr2 |     |
//                 --------------     |                 --------------
//
//  ** see KillOpnOpr() for case where first node is start-node of expr

    itnode      *cit;

    if( CITNode->opn.ds != DSOPN_PHI ) {
        AdvError( SX_NO_OPR );
    }
    MoveDown();
    KillOpnOpr();
    cit = CITNode;
    if( CITNode->opn.ds == DSOPN_PHI ) {
        Error( PC_SURP_PAREN );
    }
    CITNode = CITNode->link;
    if( CITNode->opn.ds != DSOPN_PHI ) {
        Error( SX_NO_OPR );
    }
    cit->link = CITNode->link;
    FreeOneNode( CITNode );
    CITNode = cit;
}


static  void    GrabColon( void ) {
//=================================

// Upscan routine for handling substring indexing expression.
// The operator sequence is OPR_LBR - OPR_COL.
//
// We expect that when this routine is called, there is an array element
// which has a substring expression after it.
//
// Note: Arith() does not process a dim'n declarator containing a ":"
//       since the operator sequences "comma,colon" and "colon,comma"
//       must be disallowed in OprSeqTab ( by syntax of substring expr )

// Consider PRINT *, '1234'(2:3)
// It is illegal to substring constants. Note: If the constant were a
// variable name, DSName() would be called and the OPR_LBR would be
// changed to OPR_FBR and we won't get here.

    if( BkLink->opn.ds != DSOPN_PHI ) {
        Error( SS_ONLY_IF_CHAR );
    } else {
        KillOpnOpr();
        // Consider PRINT *,(F:80)TEST
        if( CITNode->opr != OPR_TRM ) {
            CITNode->opr = OPR_COM;   // concatenate substring expression
            LowColon();               // onto end of subscript list
        } else {
            Error( SX_BAD_OPR_SEQ );
        }
    }
}


static  void    LowColon( void ) {
//================================

    // we are looking at the low bound
    if( CITNode->opn.ds == DSOPN_PHI ) {
        AddSS( 1 );
    }
    PrepArg();
}


static  void    HighColon( void ) {
//=================================

    // must be high bound
    if( CITNode->opn.ds == DSOPN_PHI ) {
        // we don't know have access to the symbol table
        // entry so we can't compute the size
        CITNode->opn.us = USOPN_SSR;
        CITNode->typ = FT_INTEGER;
        CITNode->size = TypeSize( FT_INTEGER );
    } else if( SimpleScript( CITNode ) &&
               ( (BkLink->opr == OPR_FBR) || (BkLink->opr == OPR_LBR) ) ) {
        int     ch_size;

        ch_size = SameScripts( BkLink, CITNode );
        if( ch_size > 0 ) { // The size no longer has to be optimal, only constant
            CITNode->value.st.ss_size = ch_size;
            CITNode->opn.us |= USOPN_SS1;
        }
    }
    PrepArg();
}


static  void    MkConst( intstar4 number ) {
//==========================================

    CITNode->value.intstar4 = number;
    CITNode->typ  = FT_INTEGER;
    CITNode->size = TypeSize( FT_INTEGER );
    CITNode->opn.us  = USOPN_CON;
}


static  void    AddSS( int number ) {
//===================================

    MkConst( number );
    AddConst( CITNode );
}


static  void    BadSequence( void ) {
//===================================

// Upscan routine for bad sequence of operators.

    AdvError( SX_BAD_OPR_SEQ );
}


static  void    Missing( void ) {
//===============================

// Upscan routine for missing operator.

    AdvError( SX_NO_OPR );
}


static  void    BadBracket( void ) {
//==================================

// Upscan routine for odd/unexpected parenthesis sequence.

    Error( PC_SURP_PAREN );
}


static void    BadEqual( void ) {
//===============================

// Upscan routine for illegal quantity on left side of equal sign

    Error( EQ_BAD_TARGET );
}


static  void    BadRelOpn( void ) {
//=================================

// Upscan routine for relop with log opnd (relop,relop).

    Error( MD_BAD_REL_OPN );
}


static  void    BadColonOpn( void ) {
//===================================

// Upscan routine for ":" operator with invalid operand(s).

    Error( SX_BAD_OPR_SEQ );
}


sym_id    CkAssignOk( void ) {
//============================

// Check if operand is allowed to be assigned a value.

    sym_id      sym;

    switch( CITNode->opn.us & USOPN_WHAT ) {
    case USOPN_NNL:
    case USOPN_ASS:
    case USOPN_NWL:
    case USOPN_ARR:
        if( ClassIs( SY_VARIABLE ) ) {
            if( BitOn( SY_DO_PARM ) ) {
                Error( DO_PARM_REDEFINED );
                return( NULL );
            }
            sym = CITNode->sym_ptr;
            // Consider: READ *, CH(I:J)
            // GFiniSS() sets the symbol table entry in the I.T. node
            // to the temporary SCB so we need to get the actual symbol
            // we are substringing elsewhere
            if( CITNode->opn.us & USOPN_ASY ) {
                sym = CITNode->value.st.ss_id;
            }
            sym->ns.u1.s.xflags |= SY_DEFINED;
            return( sym );
        } else {
            ClassErr( EQ_CANNOT_ASSIGN, CITNode->sym_ptr );
            return( NULL );
        }
        break;
    default:
        Error( EQ_BAD_TARGET );
        return( NULL );
    }
}


static  void    USCleanUp( void ) {
//=================================

// Clean up text list after expression error has occurred
// releasing all nodes on the way, leaving:
//                                               +------------------+
//                                       CIT ==> | OPR_TRM |        |
//                                               +------------------+
//                                               | OPR_TRM |        |
//                                               +------------------+
// NOTE : CITNode must not be pointing at the end of expression terminal

    itnode      *junk;
    itnode      *first;

    while( CITNode->opr != OPR_TRM ) {
        BackTrack();
    }
    first = CITNode;
    switch( first->opn.us & USOPN_WHAT ) {
    case USOPN_NWL:
    case USOPN_ASS:
        if( first->list != NULL ) {
            FreeITNodes( first->list );
        }
    }
    CITNode = CITNode->link;
    while( CITNode->opr != OPR_TRM ) {
        junk = CITNode;
        CITNode = CITNode->link;
        FreeOneNode( junk );
    }
    first->link = CITNode;
    CITNode = first;
    CITNode->typ = FT_NO_TYPE;
    CITNode->opn.ds = DSOPN_PHI;
}


static  bool    DoGenerate( TYPE typ1, TYPE typ2, uint *res_size ) {
//================================================================

    if( CITNode->link->opr == OPR_EQU ) {
        ResultType = typ1;
        *res_size = CITNode->size;
        if( ( ASType & AST_ASF ) || CkAssignOk() ) return( TRUE );
        return( FALSE );
    } else {
        if( ( ( typ1 == FT_DOUBLE ) && ( typ2 == FT_COMPLEX ) ) ||
            ( ( typ2 == FT_DOUBLE ) && ( typ1 == FT_COMPLEX ) ) ) {
            ResultType = FT_DCOMPLEX;
            *res_size = TypeSize( FT_DCOMPLEX );
            Extension( MD_DBLE_WITH_CMPLX );
        } else if( ( ( typ1 == FT_TRUE_EXTENDED ) && ( typ2 == FT_COMPLEX ) )
            ||     ( ( typ2 == FT_TRUE_EXTENDED ) && ( typ1 == FT_COMPLEX ) )
            ||     ( ( typ1 == FT_TRUE_EXTENDED ) && ( typ2 == FT_DCOMPLEX ) )
            ||     ( ( typ2 == FT_TRUE_EXTENDED ) && ( typ1 == FT_DCOMPLEX ) ) ) {
            ResultType = FT_XCOMPLEX;
            *res_size = TypeSize( FT_XCOMPLEX );
            Extension( MD_DBLE_WITH_CMPLX );
        } else if( ( typ2 > typ1 ) || ( typ1 == FT_STRUCTURE ) || ( typ1 == FT_NO_TYPE ) ) {
            ResultType = typ2;
            *res_size = TypeSize( typ2 );
        } else {
            ResultType = typ1;
            if( _IsTypeInteger( ResultType ) ) {
                *res_size = CITNode->size;
                if( *res_size < CITNode->link->size ) {
                    *res_size = CITNode->link->size;
                }
            } else
                *res_size = TypeSize( typ1 );
        }
        return( TRUE );
    }
}

static  void    FixFldNode( void ) {
//==================================

// Fix CITNode after a field-op has been generated.

    itnode      *next;

    next = CITNode->link;
    if( CITNode->opn.us & USOPN_FLD ) { // sub-field
        CITNode->sym_ptr = next->sym_ptr;
        CITNode->opn = next->opn;
        if( next->opn.us & USOPN_SS1 ) {
            CITNode->value.st.ss_size = next->value.st.ss_size;
        }
    } else {
        if( ( next->opn.us & USOPN_WHAT ) == USOPN_ARR ) {
            CITNode->opn.us &= ~USOPN_WHAT;
            CITNode->opn.us |= USOPN_ARR;
            // pass on structure name
            // Consider:        PRINT *, X.Y
            // where Y is an array. We need the field name of
            // array for i/o.
            CITNode->value.st.field_id = next->sym_ptr;
        } else if( next->typ == FT_STRUCTURE ) {
            // pass on structure name
            // Consider:        PRINT *, X.Y(1)
            // where Y is an array of structures. The structure we
            // want to display is for Y, not X.
            CITNode->value.st.field_id = next->sym_ptr;
        }
        if( next->opn.us & USOPN_SS1 ) {
            CITNode->opn.us |= USOPN_SS1;
            CITNode->value.st.ss_size = next->value.st.ss_size;
        }
        // so we can tell the difference between an array of
        // structures and a field of a structure that is an
        // array
        CITNode->opn.us |= USOPN_FLD;
    }
}


static  void    Generate( void ) {
//================================

// Generate code.

    TYPE        typ1;
    TYPE        typ2;
    OPTR        op;
    OPR         opr;
    itnode      *next;
    unsigned_16 mask;
    uint        res_size;

    next = CITNode->link;
    if( next->opn.ds == DSOPN_PHI ) {
        BadSequence();
    } else {
        typ1 = CITNode->typ;
        typ2 = next->typ;
        opr = next->opr;
        if( RecNOpn() ) {
            typ1 = FT_NO_TYPE;
            CITNode->size = next->size;
            if( (opr != OPR_PLS) && (opr != OPR_MIN) && (opr != OPR_NOT) ) {
                BadSequence();
                return;
            }
        }
        op = OprNum[ opr ];
        if( typ1 == FT_NO_TYPE ) {
            mask = LegalOprsU[ typ2 - FT_FIRST ];
        } else {
            mask = LegalOprsB[ ( typ2 - FT_FIRST ) * LEGALOPR_TAB_COLS + typ1 - FT_FIRST ];
        }
        if( ( ( mask >> ( op - OPTR_FIRST ) ) & 1 ) == 0 ) {
            // illegal combination
            MoveDown();
            if( typ1 == FT_NO_TYPE ) {
                TypeErr( MD_UNARY_OP, typ2 );
            } else if( typ1 == typ2 ) {
                TypeErr( MD_ILL_OPR, typ1 );
            } else {
                TypeTypeErr( MD_MIXED, typ1, typ2 );
            }
            BackTrack();
        } else if( DoGenerate( typ1, typ2, &res_size ) ) {
            if( ( opr >= OPR_FIRST_RELOP ) && ( opr <= OPR_LAST_RELOP ) &&
                ( (ResultType == FT_COMPLEX) || (ResultType == FT_DCOMPLEX) ||
                (ResultType == FT_XCOMPLEX) ) &&
                ( opr != OPR_EQ ) && ( opr != OPR_NE ) ) {
                // can only compare complex with .EQ. and .NE.
                Error( MD_RELOP_OPND_COMPLEX );
            } else {
                if( ( next->opn.us == USOPN_CON ) &&
                    ( ( CITNode->opn.us == USOPN_CON ) || ( typ1 == FT_NO_TYPE ) ) ) {
                    // we can do some constant folding
                    ConstTable[ op ]( typ1, typ2, op );
                } else {
                    // we have to generate code
                    if( CITNode->opn.us == USOPN_CON ) {
                        AddConst( CITNode );
                    } else if( next->opn.us == USOPN_CON ) {
                        AddConst( next );
                    }
                    GenOprTable[ op ]( typ1, typ2, op );
                }
            }
            switch( opr ) {
            case OPR_EQV:
            case OPR_NEQV:
            case OPR_OR:
            case OPR_AND:
            case OPR_NOT:
                if( _IsTypeInteger( typ1 ) ) {
                    Extension( MD_LOGOPR_INTOPN );
                }
                break;
            case OPR_EQ:        // relational operators
            case OPR_NE:
            case OPR_LT:
            case OPR_GE:
            case OPR_LE:
            case OPR_GT:
                ResultType = FT_LOGICAL;
                res_size = TypeSize( ResultType );
                break;
            case OPR_FLD:
            case OPR_DPT:
                // set result size to size of field
                res_size = next->size;
                FixFldNode();
                break;
            }
            CITNode->size = res_size;
            CITNode->typ = ResultType;
            FixList();
        }
    }
}


void    AddConst( itnode *node ) {
//================================

// Add constant to symbol table.

    cstring     *val_ptr;

    val_ptr = &node->value.cstring;
    if( node->typ != FT_CHAR ) {
        node->sym_ptr = STConst( val_ptr, node->typ, node->size );
    } else {
        if( node->value.cstring.len == 0 ) {
            Error( CN_ZERO_LEN );
        }
        node->sym_ptr = STLit( (byte *)val_ptr->strptr, val_ptr->len );
    }
}


static  TYPE    IFPromote( TYPE typ ) {
//=====================================
// if the promote switch is activated we promote certain integer intrinsics
// arguments

    if( ( Options & OPT_PROMOTE ) && _IsTypeInteger( typ ) ) {
        typ = FT_INTEGER;
    }
    return( typ );
}


static  void    Call( void ) {
//============================

// Upscan routine for either (1) calling a function or subroutine
//                           (2) detaching a subscript list
//                           (3) detaching a substring expression
//

    IFF     func;

    if( Subscripted() ) {
        DetSubList();
        EvalList();
    } else if( ClassIs( SY_SUBPROGRAM ) ) {
        if( BitOn( SY_INTRINSIC ) ) {
            if( CITNode->link->opn.ds != DSOPN_PHI ) {  // consider IFIX()
                if( IFSpecific( IFPromote( CITNode->link->typ ) ) == MAGIC ) {
                    DetCallList();
                    IFPrmChk();     // must do before InLineCnvt and ...
                                    // ... after IFSpecific, DetCallList
                    func = CITNode->sym_ptr->ns.si.fi.index;
                    if( ( IsIFMax( func ) || IsIFMin( func ) ) &&
                        ( CITNode->sym_ptr->ns.si.fi.u.num_args > 2 ) )
                        EvalList();
                    else
                        InlineCnvt();
                } else {
                    DetCallList();
                    IFPrmChk();
                    EvalList();
                }
            } else {
                Error( LI_NO_PARM );
            }
        } else {
            DetCallList();
            EvalList();
        }
    } else {
        DetSStr();
        EvalList();
    }
}


static  void    EvalList( void ) {
//================================

    if( !AError ) {
        if( RecNextOpr( OPR_EQU ) && !( CITNode->opn.us & USOPN_FLD ) ) {
            SetDefinedStatus();
        }
        ProcList( CITNode );
    }
}


static  void    IFPrmChk( void ) {
//================================

// Check that argument types agree with those intrinsic func expects.
//     (1) check that argument codes are acceptable ( e.g. ~ array )
//     (2) check correct number of arguments has been passed
//

    sym_id      sym;
    itnode      *oldcit;
    IFF         func;
    int         parm_cnt;
    TYPE        parm_typ;
    PCODE       parm_code;

    sym = CITNode->sym_ptr;
    func = sym->ns.si.fi.index;
    parm_typ = IFArgType( func );
    oldcit = CITNode;
    CITNode = oldcit->list;
    parm_cnt = 0;
    for(;;) {
        if( RecColon() )         // substring the i.f.
            break;
        if( RecCloseParen() )    // end of list
            break;
        parm_code = ParmCode( CITNode );
        switch( func ) {
        case IF_ALLOCATED: {
            sym_id      sym = CITNode->sym_ptr;

            if( (parm_code == PC_ARRAY_NAME) && _Allocatable( sym ) )
                break;
            if( (parm_code == PC_VARIABLE) && (sym->ns.u1.s.typ == FT_CHAR) &&
                (sym->ns.xt.size == 0) && !(sym->ns.flags & SY_SUB_PARM) ) {
                sym->ns.u1.s.xflags |= SY_ALLOCATABLE;
                break;
            }
            Error( LI_ARG_ALLOCATED );
            break;
        }
        case IF_ISIZEOF:
            switch( CITNode->opn.us ) {
            case USOPN_NNL:
                MkConst( _SymSize( CITNode->sym_ptr ) );
                break;
            case USOPN_CON:
                if( CITNode->typ == FT_CHAR ) {
                    MkConst( CITNode->value.cstring.len );
                } else if( CITNode->typ == FT_STRUCTURE ) {
                    MkConst( CITNode->value.intstar4 );
                } else {
                    MkConst( CITNode->size );
                }
                break;
            case USOPN_ARR:
                MkConst( CITNode->sym_ptr->ns.si.va.u.dim_ext->num_elts *
                         _SymSize( CITNode->sym_ptr ) );
                break;
            default:
                Error( LI_ARG_ISIZEOF );
                break;
            }
            break;
        case IF_LOC:
        case IF_VOLATILE:
            break;
        default:
            if( CITNode->typ != parm_typ ) {
                if( ( (CITNode->opn.us & USOPN_WHAT) == USOPN_CON) && TypeIs( parm_typ ) ) {
                    // we don't want an error in the following case:
                    //          INTEGER*2 I
                    //          PRINT *, MOD( I, 3 )
                    // I is INTEGER*2 and 3 is INTEGER*4
                    CnvTo( CITNode, parm_typ, TypeSize( parm_typ ) );
                } else if( ( Options & OPT_PROMOTE ) && ( parm_typ == FT_INTEGER ) &&
                           TypeIs( parm_typ ) ) {
                    // check if we should allow
                    //  INTEGER*1 I
                    //  I = 13
                    //  PRINT *, IABS( I )
                    // if the users turns on the switch we allow it
                    break;
                } else {
                    switch( func ) {
                    case IF_INT:
                    case IF_REAL:
                    case IF_DBLE:
                    case IF_QEXT:
                    case IF_CMPLX:
                    case IF_DCMPLX:
                    case IF_QCMPLX:
                        break;
                    case IF_FLOAT:
                    case IF_DFLOAT:
                    case IF_CHAR:
                        if( TypeIs( parm_typ ) ) {
                            break;
                        }
                        // else drop through to error
                    default:
                        // consider:
                        //          I = 5
                        //          PRINT *, ANINT(I)
                        // we don't want to issue 2 error msgs
                        if( !AError ) {
                            TypeTypeErr( LI_PT_MISMATCH, parm_typ, CITNode->typ );
                        }
                        break;
                    }
                }
            }
            switch( parm_code ) {
            case PC_CONST:
            case PC_VARIABLE:
            case PC_ARRAY_ELT:
            case PC_SS_ARRAY:
                break;
            default:
                PrmCodeErr( LI_ILL_PARM_CODE, parm_code );
                break;
            }
            break;
        }
        AdvanceITPtr();
        ++parm_cnt;
    }
    // for intrinsic functions that take variable # of args (e.g. MAX, MIN)
    sym->ns.si.fi.u.num_args = parm_cnt;
    CITNode = oldcit;
    IFCntPrms( func, parm_cnt );
}


static  bool    IFAsOperator( void ) {
//====================================

    if( CITNode->opr != OPR_FBR )
        return( FALSE );
    if( ( BkLink->flags & SY_CLASS ) != SY_SUBPROGRAM )
        return( FALSE );
    if( !(BkLink->flags & SY_INTRINSIC) )
        return( FALSE );
    switch( BkLink->sym_ptr->ns.si.fi.index ) {
    case IF_ISIZEOF:
    case IF_ALLOCATED:
    case IF_VOLATILE:
    case IF_CHAR:
        return( TRUE );
    }
    return( FALSE );
}


static  void    PrepArg( void ) {
//===============================

// Upscan routine to prepare an item in a function or subscript list.

    IFF         if_index;

    if( ClassIs( SY_SUBPROGRAM ) && BitOn( SY_INTRINSIC ) ) {
        if( CITNode->opn.us == USOPN_NNL ) {
            if_index = CITNode->sym_ptr->ns.si.fi.index;
            if( IFAsArg( if_index ) == 0 ) {
                Error( LI_NOT_PARM );
            } else {
                MarkIFUsed( if_index );
                CITNode->sym_ptr->ns.flags |= SY_IF_ARGUMENT;
            }
        }
    }
    if( ( CITNode->opn.us & USOPN_WHAT ) == USOPN_STN ) {
        if( !(ASType & AST_ALT) ) {
            PrmCodeErr( SR_ILL_PARM, PC_STATEMENT );
        }
    } else {
        if( !IFAsOperator() ) {
            if( CITNode->opn.us == USOPN_CON ) {
                AddConst( CITNode );
            }
            if( ( CITNode->opn.us & USOPN_SS1 ) == 0 )
            GArg();
        }
    }
    BackTrack();
}


static  void    FixList( void ) {
//===============================

// Remove the second operand itnode after code generation
// and update variable type or result.

    itnode      *junk;

    junk = CITNode->link;
    CITNode->link = junk->link;
    FreeOneNode( junk );
}


static  void    InlineCnvt( void ) {
//==================================

// Do conversion routines inline (no function call).

    itnode      *cit;
    IFF         func;
    TYPE        typ;
    TYPE        func_type;

    cit = CITNode;
    func = CITNode->sym_ptr->ns.si.fi.index;
    func_type = CITNode->typ;
    CITNode = CITNode->list;
    typ = CITNode->typ;
    if( func == IF_ALLOCATED ) {
        GAllocated();
    } else if( func == IF_LOC ) {
        GLoc();
    } else if( func == IF_VOLATILE ) {
        func_type = typ;
        GVolatile();
    } else if( func == IF_CHAR ) {
        if( CITNode->opn.us == USOPN_CON ) {
            intstar4    arg;

            arg = ITIntValue( CITNode );
            // see comments in IFCHAR
            if( ( arg < -128 ) || ( arg > 255 ) ) {
                Error( LI_CHAR_BOUND );
                arg = '?';
            }
            cit->value.cstring.data = arg;
            cit->value.cstring.strptr = &cit->value.cstring.data;
            cit->value.cstring.len = 1;
            cit->opn.us = USOPN_CON;
            cit->flags = 0;
        } else {
            GArg();
            GILCnvTo( FT_INTEGER, TypeSize( FT_INTEGER ) );
            ProcList( cit );
            MarkIFUsed( IF_CHAR );
            CITNode = cit;
            return;
        }
    } else if( func == IF_ISIZEOF ) {
        cit->value.intstar4 = CITNode->value.intstar4;
        cit->opn.us = USOPN_CON;
        cit->flags = 0;
    } else if( ( typ >= FT_INTEGER_1 ) && ( typ <= FT_XCOMPLEX ) ) {
        // this switch statement replaces a huge if() statement for speed
        switch( func ) {
        case IF_CMPLX:
            switch( typ ) {
            case( FT_DOUBLE ):
                func = IF_DCMPLX;
                func_type = FT_DCOMPLEX;
                break;
            case( FT_TRUE_EXTENDED ):
                func = IF_QCMPLX;
                func_type = FT_XCOMPLEX;
                break;
            }
        case IF_DCMPLX:
        case IF_QCMPLX:
            if( RecNextOpr( OPR_COM ) ) {
                if( CITNode->link->typ == typ ) {
                    if( typ <= FT_EXTENDED ) {
                        if( func == IF_CMPLX ) {
                            GMakeCplx();
                        } else if ( func == IF_DCMPLX ) {
                            GMakeDCplx();
                        } else {
                            GMakeXCplx();
                        }
                    } else {
                        Error( LI_2_CMPLX, IFName( func ) );
                        AdvanceITPtr();
                    }
                } else {
                    Error( LI_CMPLX_TYPES, IFName( func ) );
                    AdvanceITPtr();
                }
            } else {
                GILCnvTo( func_type, TypeSize( func_type ) );
            }
            break;
        case IF_HFIX:
            GILCnvTo( func_type, sizeof( intstar2 ) );
            break;
        case IF_I1MOD:
        case IF_I2MOD:
        case IF_MOD:            GModulus();     break;
        case IF_AIMAG:
        case IF_QIMAG:
        case IF_DIMAG:          GImag();        break;
        case IF_CONJG:
        case IF_QCONJG:
        case IF_DCONJG:         GConjg();       break;
        case IF_DPROD:          GDProd();       break;
        case IF_QPROD:          GXProd();       break;
        case IF_I1BTEST:
        case IF_I2BTEST:
        case IF_BTEST:          GBitTest();     break;
        case IF_I1BSET:
        case IF_I2BSET:
        case IF_IBSET:          GBitSet();      break;
        case IF_I1BCLR:
        case IF_I2BCLR:
        case IF_IBCLR:          GBitClear();    break;
        case IF_I1OR:
        case IF_I2OR:
        case IF_IOR:            GBitOr();       break;
        case IF_I1AND:
        case IF_I2AND:
        case IF_IAND:           GBitAnd();      break;
        case IF_I1NOT:
        case IF_I2NOT:
        case IF_NOT:            GBitNot();      break;
        case IF_I1EOR:
        case IF_I2EOR:
        case IF_IEOR:           GBitExclOr();   break;
        case IF_I1BCHNG:
        case IF_I2BCHNG:
        case IF_IBCHNG:         GBitChange();   break;
        case IF_AMOD:
        case IF_QMOD:
        case IF_DMOD:           GMod();         break;
        case IF_ABS:
        case IF_IABS:
        case IF_I1ABS:
        case IF_I2ABS:
        case IF_QABS:
        case IF_DABS:           GAbs();         break;
        case IF_ASIN:
        case IF_QASIN:
        case IF_DASIN:          GASin();        break;
        case IF_ACOS:
        case IF_QACOS:
        case IF_DACOS:          GACos();        break;
        case IF_ATAN:
        case IF_QATAN:
        case IF_DATAN:          GATan();        break;
        case IF_ATAN2:
        case IF_QATAN2:
        case IF_DATAN2:         GATan2();       break;
        case IF_ALOG:
        case IF_QLOG:
        case IF_DLOG:           GLog();         break;
        case IF_ALOG10:
        case IF_QLOG10:
        case IF_DLOG10:         GLog10();       break;
        case IF_COS:
        case IF_QCOS:
        case IF_DCOS:           GCos();         break;
        case IF_SIN:
        case IF_QSIN:
        case IF_DSIN:           GSin();         break;
        case IF_TAN:
        case IF_QTAN:
        case IF_DTAN:           GTan();         break;
        case IF_SINH:
        case IF_QSINH:
        case IF_DSINH:          GSinh();        break;
        case IF_COSH:
        case IF_QCOSH:
        case IF_DCOSH:          GCosh();        break;
        case IF_TANH:
        case IF_QTANH:
        case IF_DTANH:          GTanh();        break;
        case IF_SQRT:
        case IF_QSQRT:
        case IF_DSQRT:          GSqrt();        break;
        case IF_EXP:
        case IF_QEXP:
        case IF_DEXP:           GExp();         break;
        case IF_SIGN:
        case IF_DSIGN:
        case IF_ISIGN:
        case IF_I1SIGN:
        case IF_I2SIGN:
        case IF_QSIGN:          GSign();        break;
        case IF_I1LSHIFT:
        case IF_I2LSHIFT:
        case IF_LSHIFT:         GBitLShift();   break;
        case IF_I1RSHIFT:
        case IF_I2RSHIFT:
        case IF_RSHIFT:         GBitRShift();   break;
        // IsIFMax() stuff... be sure to keep this up to date with iflookup.c
        case IF_AMAX0:
        case IF_AMAX1:
        case IF_DMAX1:
        case IF_QMAX1:
        case IF_I1MAX0:
        case IF_I2MAX0:
        case IF_MAX0:
        case IF_MAX1:           GMax( func_type );      break;
        // IsIfMin() stuff... be sure to keep this up to date with iflookup.c
        case IF_AMIN0:
        case IF_AMIN1:
        case IF_DMIN1:
        case IF_QMIN1:
        case IF_I1MIN0:
        case IF_I2MIN0:
        case IF_MIN0:
        case IF_MIN1:           GMin( func_type );      break;
        case IF_REAL:           // Make sure that D<-REAL(Z) && X<-REAL(Q)
            switch( typ ) {
            case( FT_DCOMPLEX ):
                func_type = FT_DOUBLE;
                break;
            case( FT_TRUE_XCOMPLEX ):
                func_type = FT_EXTENDED;
                break;
            break;
            }                   // Fall through to default:
        default:
            GILCnvTo( func_type, TypeSize( func_type ) );
            break;
        }
    } else if( IFIsGeneric( func ) ) {
        TypeErr( LI_EXP_CNV_TYPE, typ );      // typ is what we got
    } else if( func == IF_LEN ) {
        GCharLen();
    } else { // ichar
        GIChar();
    }
    AdvanceITPtr();
    ReqCloseParen();
    CITNode = cit;
    cit = CITNode->list;
    CITNode->typ = func_type;
    CITNode->size = TypeSize( func_type );
    SetOpn( CITNode, cit->opn.us & USOPN_WHERE );
    CITNode->list = NULL;
    UpdateNode( CITNode, cit );
    FreeITNodes( cit );
}


static  const void (* const __FAR RtnTable[])(void) = {
    #define pick(id,proc) proc,
    #include "rtntable.h"
    #undef pick
};

void    UpScan( void ) {
//======================

// Upscan phase of expression processor.
// On entry, CITNode is OPR_TRM at end of expression
// Action routines move CITNode appropriately each time through loop.

    int         index;

    BackTrack();
    for(;;) {
        if( AError ) {
            USCleanUp();
            break;
        }
        if( ( CITNode->opr == OPR_TRM ) &&
            ( CITNode->link->opr == OPR_TRM ) )
            break;
        index = OprIndex[ CITNode->link->opr ];
        index += OprIndex[ CITNode->opr ] * OPR_SEQ_MAT_COLS;
        RtnTable[ OprSeqMat[ index ] ]();
    }
    EndExpr();
}
