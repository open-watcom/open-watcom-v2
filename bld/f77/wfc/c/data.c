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
* Description:  Processing for DATA statements.
*
****************************************************************************/


#include "ftnstd.h"
#include "errcod.h"
#include "opr.h"
#include "opn.h"
#include "progsw.h"
#include "stmtsw.h"
#include "namecod.h"
#include "rtconst.h"
#include "fcodes.h"
#include "global.h"
#include "segsw.h"
#include "recog.h"
#include "types.h"
#include "ferror.h"
#include "insert.h"
#include "utility.h"
#include "hexcnv.h"
#include "csloops.h"
#include "proctbl.h"
#include "data.h"
#include "downscan.h"
#include "ioiolist.h"
#include "rststruc.h"
#include "symtab.h"
#include "upscan.h"
#include "gdata.h"
#include "gio.h"

#include <ctype.h>
#include <string.h>


/* Forward declarations */
static  void    DoData( void );
static  void    DumpDataSets( int num, itnode *node );
static  void    Free2CIT( itnode *node );
static  void    VarList( void );
static  void    ConList( void );
static  void    CkFlags( void );
static  void    GetSConst( void );


void    CpData(void) {
//================

// Compile DATA statement.

    label_id    end_data;
    int         data_sets;
    bool        error;

    // so that we can issue ST_DATA_TOO_EARLY later
    SgmtSw |= SG_SEEN_DATA;
    error = FALSE;
    data_sets = 0;
    CITNode->opr = OPR_COM; // prevent call to FinishImpDo first time
    end_data = GDataProlog();
    for(;;) {
        DoData();
        error |= AError;
        ++data_sets;
        if( RecNOpn() ) {
            AdvanceITPtr();
            if( RecTrmOpr() ) break;
            ReqComma();
        }
        if( RecTrmOpr() || error ) break;
    }
    if( !error ) {
        DumpDataSets( data_sets, ITHead );
    }
    GDataEpilog( end_data );
    CITNode->opr = OPR_TRM;
}


void    DataInit( itnode *var_node ) {
//====================================

// Process data within a type declaration statement.

    label_id    end_data;

    Free2CIT( var_node );
    var_node->opr = OPR_COM;
    CITNode = var_node;
    end_data = GDataProlog();
    DoData();
    if( !AError ) {
        DumpDataSets( 1, var_node );
    }
    GDataEpilog( end_data );
    if( ReqNOpn() ) {
        AdvanceITPtr();
    }
}


static  void    Free2CIT( itnode *node ) {
//========================================

// Free all nodes between "node" and "CITNode".

    itnode      *junk;
    itnode      *chaser;

// this code makes a(3)/3*5/ from a type declaration look like
// a/3*5/ as in a DATA statement so we can call DoData().

    junk = node->link;
    node->link = CITNode;
    chaser = junk;
    while( chaser != CITNode ) {
        chaser = chaser->link;
        FreeOneNode( junk );
        junk = chaser;
    }
}


static  void    DoData( void ) {
//========================

// Process one vlist/dlist/ pair.

    STMT    save_stmtproc;

    StmtSw |= SS_DATA_INIT;
    save_stmtproc = StmtProc;
    StmtProc = PR_READ; // so I/O processing works correctly
    IOData = 0;         // ...
    AError = FALSE;
    VarList();
    if( !AError ) {
        GEndVarSet();
        ConList();
    }
    StmtProc = save_stmtproc;
    StmtSw &= ~SS_DATA_INIT;
}


static  OPR    FindSlash( itnode **itptr_ptr ) {
//===============================================

// Scan ahead for an OPN_DIV and replace it with OPN_TRM.

    int         level;
    itnode      *cit;
    OPR         opr;

    cit = CITNode;
    level = 0;
    for(;;) {
        if( RecOpenParen() ) {
            level++;
        } else if( RecCloseParen() ) {
            level--;
        }
        AdvanceITPtr();
        if( ( (RecDiv() || RecCat()) && (level == 0) ) || RecTrmOpr() ) break;
    }
    *itptr_ptr = CITNode;
    opr = CITNode->opr;
    CITNode->opr = OPR_TRM;
    CITNode = cit;
    return( opr );
}


static  void    VarList( void ) {
//=========================

// Process one variable list in a DATA statement.

    OPR         last_opr;
    OPR         opr;
    int         do_level;
    itnode      *last_node;

    do_level = 0;
    last_opr = FindSlash( &last_node );
    while( CITNode != last_node ) {
        if( AError ) break;
        if( RecTrmOpr() && ( CITNode != ITHead ) ) {
            --do_level;
            FinishImpDo();
        } else if( StartImpDo() ) {
            ++do_level;
        } else if( ReqName( NAME_VAR_OR_ARR ) ) {
            InitVar = LkSym();
            if( InitVar->u.ns.u1.s.typ == FT_STRUCTURE ) {
                // make sure structure size is calculated - normally
                // structure size is calculated by StructResolve() which
                // is not called until the first executable statement
                CalcStructSize( InitVar->u.ns.xt.sym_record );
            }
            CkFlags();
            opr = CITNode->opr;
            ProcDataExpr();
            CITNode->opr = opr;
            ListItem();
            if( !RecTrmOpr() ) {
                ReqComma();
            }
        } else {
            AdvanceITPtr();
            AError = TRUE;
            break;
        }
    }
    if( AError ) {
        while( do_level != 0 ) { // clean up hanging do entrys
            TermDo();
            --do_level;
        }
    } else {
        CITNode->opr = last_opr;
        ReqDiv();
    }
}


static  bool    HexConst(void) {
//==========================

// Check for a hexadecimal constant specifier.

    char        *hex_data;
    int         hex_len;
    sym_id      sym;

    hex_data = CITNode->opnd;
    hex_len = CITNode->opnd_size;
    if( CITNode->opn.ds != DSOPN_HEX ) {
        if( !RecName() ) return( FALSE );
        if( *hex_data != 'Z' ) return( FALSE );
        sym = SymFind( hex_data, hex_len );
        if( sym != NULL ) {
            if( ( sym->u.ns.flags & SY_CLASS ) == SY_PARAMETER ) return( FALSE );
        }
        ++hex_data;
    }
    --hex_len;
    hex_len = MkHexConst( hex_data, CITNode->opnd, hex_len );
    if( hex_len == 0 ) return( FALSE );
    CITNode->opnd_size = hex_len;
    CITNode->opn.ds = DSOPN_LIT;
    GetConst();
    AddConst( CITNode );
    CITNode->typ = FT_HEX;
    Extension( DA_HEX_CONST );
    return( TRUE );
}


static  void    ConList( void ) {
//=========================

// Collect constants for data initialization.

    OPR         opr;
    itnode      *last_node;

    opr = FindSlash( &last_node );
    for(;;) {
        if( RecNextOpr( OPR_MUL ) ) {
            ProcDataRepExpr();
            if( ITIntValue( CITNode ) <= 0 ) {
                Error( DA_BAD_RPT_SPEC );
            }
            AddConst( CITNode );
            AdvanceITPtr();
        }
        if( !HexConst() ) {
            GetSConst();
            AddConst( CITNode );
        }
        AdvanceITPtr();
        if( CITNode == last_node ) break;
        ReqComma();
        if( AError ) break;
    }
    CITNode->opr = opr;
    ReqDiv();
}


static  void    DumpDataSets( int num, itnode *node ) {
//=====================================================

// Dump the constants for data initialization.

    itnode      *rpt;

    GStopIO();
    CITNode = node;
    GBegDList();
    while( --num >= 0 ) {
        for(;;) {
            AdvanceITPtr();
            if( RecDiv() ) break;
        }
        for(;;) {
            if( RecNextOpr( OPR_MUL ) ) {
                rpt = CITNode;
                AdvanceITPtr();
            } else {
                rpt = NULL;
            }
            if( RecNOpn() ) { // jump over optional sign.
                AdvanceITPtr();
            }
            GDataItem( rpt );
            AdvanceITPtr();
            if( RecDiv() ) break;
        }
        GEndDSet();
    }
    GEndDList();
}


static  void    GetSConst( void ) {
//===========================

// Signed constant converting without downscan-upscan process.

    int         sign;

    if( RecNOpn() ) {
        sign = 1;
        if( RecNextOpr( OPR_MIN ) ) {
            sign = -1;
        } else if( !RecNextOpr( OPR_PLS ) ||
                   ( CITNode->link->opn.ds < DSOPN_INT ) ) {
            ProcDataIExpr();
            return;
        }
        AdvanceITPtr();
        ProcDataIExpr();
        switch( CITNode->typ ) {
        case FT_INTEGER_1:
            CITNode->value.intstar1 *= sign;
            break;
        case FT_INTEGER_2:
            CITNode->value.intstar2 *= sign;
            break;
        case FT_INTEGER:
            CITNode->value.intstar4 *= sign;
            break;
        case FT_REAL:
            CITNode->value.single *= sign;
            break;
        case FT_DOUBLE:
            CITNode->value.dble *= sign;
            break;
        case FT_TRUE_EXTENDED:
            CITNode->value.extended *= sign;
            break;
        }
    } else {
        ProcDataIExpr();
    }
}


int  MkHexConst( char *hex_data, char *dst, int hex_len ) {
//====================================================================

    uint        len;

    len = HSToB( hex_data, hex_len, dst );
    if( len != (hex_len+1)/2 ) return( 0 );
    return( len );
}


static  void    CkFlags( void ) {
//=========================

    if( ( InitVar->u.ns.flags & SY_CLASS ) != SY_VARIABLE ) {
        ClassNameErr( DA_ILL_NAME, InitVar );
    } else if( ( InitVar->u.ns.flags & SY_SUB_PARM ) != 0 ) {
        ClassNameErr( DA_ILL_NAME, InitVar );
    } else if((InitVar->u.ns.flags & SY_SUBSCRIPTED) && _Allocatable( InitVar )) {
        IllName( InitVar );
    } else {
        // Don't set SY_TYPE otherwise we won't be able to detect whether
        // the type has been explicitly declared when we call ProcDataExpr().
        // SY_TYPE will be set by DSName() when we call ProcDataExpr().
        InitVar->u.ns.flags |= SY_USAGE | SY_DATA_INIT;
    }
}
