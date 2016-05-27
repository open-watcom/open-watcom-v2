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
// GDO       : code generation for DO-loops
//

#include "ftnstd.h"
#include "fcodes.h"
#include "global.h"
#include "stmtsw.h"
#include "opn.h"
#include "cpopt.h"
#include "recog.h"
#include "emitobj.h"
#include "types.h"
#include "utility.h"
#include "cgmagic.h"
#include "gdo.h"


/* Forward declarations */
static  void    DataDo( TYPE do_type );
static  void    DataDoEnd( void );
static  void    DoLoop( TYPE do_type );
static  void    DoExpr( void );
static  void    DoLoopEnd( void );


void    GDoInit( TYPE do_type ) {
//==============================

// Initialize a DO or implied-DO.
// Process "DO I=e1,e2,e3", where e1, e2 and e3 are numeric expressions.

    if( StmtSw & SS_DATA_INIT ) {
        DataDo( do_type );
    } else {
        DoLoop( do_type );
    }
}


static intstar4 GetIntValue( itnode *node ) {
//===========================================

    if( node->typ == FT_REAL ) {
        return( node->value.single );
    } else if( node->typ == FT_DOUBLE ) {
        return( node->value.dble );
    } else if( node->typ == FT_EXTENDED ) {
        return( node->value.extended );
    } else {
        return( ITIntValue( node ) );
    }
}


static bool NeedIncrement( intstar4 limit, intstar4 incr, TYPE do_type ) {
//=======================================================================

    switch( do_type ) {
    case FT_INTEGER_1:
        if( incr > 0 ) {
            if( limit > SCHAR_MAX - incr ) {
                return( true );
            }
        } else {
            if( limit < SCHAR_MIN - incr ) {
                return( true );
            }
        }
        break;
    case FT_INTEGER_2:
        if( incr > 0 ) {
            if( limit > SHRT_MAX - incr ) {
                return( true );
            }
        } else {
            if( limit < SHRT_MIN - incr ) {
                return( true );
            }
        }
        break;
    case FT_INTEGER:
        if( incr > 0 ) {
            if( limit > INT_MAX - incr ) {
                return( true );
            }
        } else {
            if( limit < INT_MIN - incr ) {
                return( true );
            }
        }
        break;
    }
    return( false );
}


static  void    DoLoop( TYPE do_type ) {
//=====================================

// Generate code for DO statement or implied-DO.

    do_entry    *doptr;
    uint        do_size;
    intstar4    incr;
    intstar4    limit;
    sym_id      loop_ctrl;
    TYPE        e1_type;
    uint        e1_size;
    itnode      *e2_node;
    itnode      *e3_node;
    bool        e2_const;

    doptr = CSHead->cs_info.do_parms;
    do_size = CITNode->sym_ptr->u.ns.xt.size;
    doptr->do_parm = CITNode->sym_ptr;          // save ptr to do variable
    AdvanceITPtr();                             // bump past the '='
    EatDoParm();                                // process e1
    PushOpn( CITNode );
    e1_type = CITNode->typ;
    e1_size = CITNode->size;
    AdvanceITPtr();
    if( ReqComma() ) {
        EatDoParm();                            // process e2
        e2_const = CITNode->opn.us == USOPN_CON;
        PushOpn( CITNode );
        e2_node = CITNode;
        AdvanceITPtr();
        e3_node = NULL;
        if( RecComma() ) {
            EatDoParm();                        // process e3
            e3_node = CITNode;
            if( !AError ) {
                if( (CITNode->opn.us == USOPN_CON) && _IsTypeInteger( do_type ) ) {
                    incr = GetIntValue( CITNode );
                    doptr->incr_value = incr;
                    doptr->increment = NULL;
                    if( !(OZOpts & OZOPT_O_FASTDO) ) {
                        if( e2_const ) {
                            limit = GetIntValue( e2_node );
                            if( NeedIncrement( limit, incr, do_type ) ) {
                                PushOpn( CITNode );
                                doptr->increment = StaticAlloc( do_size, do_type );
                            }
                        } else {
                            PushOpn( CITNode );
                            doptr->increment = StaticAlloc( do_size, do_type );
                        }
                    }
                } else {
                    PushOpn( CITNode );
                    doptr->increment = StaticAlloc( do_size, do_type );
                }
                AdvanceITPtr();
            }
        } else {
            if( _IsTypeInteger( do_type ) ) {
                doptr->increment = NULL;
                doptr->incr_value = 1;
                if( !(OZOpts & OZOPT_O_FASTDO) ) {
                    if( e2_const ) {
                        limit = GetIntValue( e2_node );
                        if( NeedIncrement( limit, 1, do_type ) ) {
                            PushConst( 1 );
                            doptr->increment = StaticAlloc( do_size, do_type );
                        }
                    } else {
                        PushConst( 1 );
                        doptr->increment = StaticAlloc( do_size, do_type );
                    }
                }
            } else {
                PushConst( 1 );
                doptr->increment = StaticAlloc( do_size, do_type );
            }
        }
        EmitOp( FC_DO_BEGIN );
        OutPtr( doptr->do_parm );
        OutPtr( doptr->increment );
        if( doptr->increment == NULL ) { // INTEGER do-loop with constant incr
            loop_ctrl = StaticAlloc( do_size, do_type );
            OutConst32( doptr->incr_value );
            OutPtr( loop_ctrl );
        } else {
            if( _IsTypeInteger( do_type ) ) {
                loop_ctrl = StaticAlloc( do_size, do_type );
            } else {
                loop_ctrl = StaticAlloc( sizeof( intstar4 ), FT_INTEGER );
            }
            doptr->iteration = loop_ctrl;
            OutPtr( loop_ctrl );
            if( e3_node == NULL ) {
                DumpType( FT_INTEGER, TypeSize( FT_INTEGER ) );
            } else {
                GenType( e3_node );
            }
        }
        GenType( e2_node );
        DumpType( e1_type, e1_size );
        OutU16( CSHead->branch );
        OutU16( CSHead->bottom );
    }
}


static  void    DataDo( TYPE do_type ) {
//=====================================

// Process an implied-DO for DATA statements.

    sym_id      do_var;

    do_type = do_type;
    do_var = CITNode->sym_ptr;
    AdvanceITPtr();
    DoExpr();                           // process e1
    if( ReqComma() ) {
        DoExpr();                       // process e2
        if( RecComma() ) {
            DoExpr();                   // process e3
        } else {
            PushConst( 1 );             // indicate unit incrementation
        }
    }
    EmitOp( FC_DATA_DO_LOOP );
    OutPtr( do_var );
}


static  void    DoExpr( void ) {
//========================

// Evaluate a DO expression (e1, e2 or e3 ).

    EatDoParm();
    PushOpn( CITNode );
    AdvanceITPtr();
}


void    GDoEnd( void ) {
//================

// Terminate a DO or an implied-DO.

    if( StmtSw & SS_DATA_INIT ) {
        DataDoEnd();
    } else {
        DoLoopEnd();
    }
}


static  void    DoLoopEnd( void ) {
//===========================

// Generate code for end of DO-loops or implied-DO.

    do_entry    *doptr;

    doptr = CSHead->cs_info.do_parms;
    EmitOp( FC_DO_END );
    OutPtr( doptr->do_parm );
    OutPtr( doptr->increment );
    if( doptr->increment == NULL ) {
        OutConst32( doptr->incr_value );
    } else {
        OutPtr( doptr->iteration );
    }
    OutU16( CSHead->branch );
}


static  void    DataDoEnd( void ) {
//===========================

// Process end of implied-DO for DATA statement.

    EmitOp( FC_END_OF_SEQUENCE );
}
