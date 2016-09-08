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
* Description:  simple control structure F-Code processor
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "fcgbls.h"
#include "wf77defs.h"
#include "wf77cg.h"
#include "tmpdefs.h"
#include "wf77labe.h"
#include "cgflags.h"
#include "fcodes.h"
#include "cpopt.h"
#include "fmemmgr.h"
#include "emitobj.h"
#include "inout.h"
#include "errcod.h"
#include "ferror.h"
#include "fctypes.h"
#include "tcmplx.h"
#include "rststmt.h"
#include "fccmplx.h"
#include "fcjmptab.h"
#include "fcflow.h"
#include "fcsel.h"
#include "fcstring.h"
#include "fctemp.h"
#include "fcdata.h"
#include "fcgmain.h"
#include "fcstack.h"
#include "cgswitch.h"
#include "cgprotos.h"


extern  cgflags_t       CGFlags;

static  obj_ptr         WarpReturn;


/* Forward declarations */
static  void    RBReferenced( sym_id rb );
static  void    RefStmtFunc( sym_id sf );


void    InitLabels( void ) {
//====================

// Initialize label processing.

    LabelList = NULL;
}


void    FiniLabels( int label_type ) {
//====================================

// Free specified class of labels.

    label_entry **owner;
    label_entry *curr;

    owner = (label_entry **)&LabelList;
    for(;;) {
        curr = *owner;
        if( curr == NULL ) break;
        if( (curr->label & FORMAT_LABEL) == label_type ) {
            if( (CGFlags & CG_FATAL) == 0 ) {
                if( curr->label & FORMAT_LABEL ) {
                    BEFiniBack( curr->handle );
                    BEFreeBack( curr->handle );
                } else {
                    InfoError( CP_ERROR, "unfreed label" );
                    BEFiniLabel( curr->handle );
                }
            }
            *owner = curr->link;
            FMemFree( curr );
        } else {
            owner = &curr->link;
        }
    }
}


static  label_entry     *FindLabel( int label ) {
//===============================================

// Search for given label.

    label_entry *le;

    le = LabelList;
    for(;;) {
        if( le == NULL ) break;
        if( (le->label & ~FORMAT_LABEL) == label ) break;
        le = le->link;
    }
    if( le == NULL ) {
        le = FMemAlloc( sizeof( label_entry ) );
        le->label = label;
        le->handle = NULL;
        le->link = LabelList;
        LabelList = le;
    }
    return( le );
}


label_handle    GetLabel( label_id label ) {
//==========================================

// Get a label.

    label_entry *le;

    le = FindLabel( label );
    if( le->handle == NULL ) {
        le->handle = BENewLabel();
    }
    return( le->handle  );
}


void    FCJmpFalse( void ) {
//====================

    unsigned_16 typ_info;
    cg_type     typ;
    cg_name     bool_expr;

    typ_info = GetU16();
    typ = GetType( typ_info );
    if( IntType( typ_info ) ) {
        bool_expr = CGCompare( O_NE, XPopValue( typ ),
                               CGInteger( 0, typ ), typ );
    } else {
        bool_expr = XPopValue( typ );
    }
    CGControl( O_IF_FALSE, bool_expr, GetLabel( GetU16() ) );
}


void    FCJmpAlways( void ) {
//=====================

    CGControl( O_GOTO, NULL, GetLabel( GetU16() ) );
}


label_handle    GetStmtLabel( sym_id sn ) {
//=========================================

// Get a statement label.

    return( GetLabel( sn->u.st.address ) );
}


void    FCStmtJmpAlways( void ) {
//=========================

    sym_id      sn;

    sn = GetPtr();
    CGControl( O_GOTO, NULL, GetStmtLabel( sn ) );
    RefStmtLabel( sn );
}


void    FCDefineLabel( void ) {
//=======================

    CGControl( O_LABEL, NULL, GetLabel( GetU16() ) );
}


void    FCStmtDefineLabel( void ) {
//===========================

    sym_id      sn;

    sn = GetPtr();
    CGControl( O_LABEL, NULL, GetStmtLabel( sn ) );
    RefStmtLabel( sn );
}


back_handle     GetFmtLabel( label_id label ) {
//=============================================

// Get a format label.

    label_entry *le;

    le = FindLabel( label );
    if( le->handle == NULL ) {
        le->handle = BENewBack( NULL );
        le->label |= FORMAT_LABEL;
    }
    return( le->handle  );
}


void    FCAssign( void ) {
//==================

// Process ASSIGN statement.

    sym_id      stmt;

    stmt = GetPtr();
    if( stmt->u.st.flags & SN_FORMAT ) {
        CGDone( CGAssign( SymAddr( GetPtr() ),
                          CGBackName( GetFmtLabel( stmt->u.st.address ),
                                      TY_LOCAL_POINTER ),
                          TY_LOCAL_POINTER ) );
    } else {
        CGDone( CGAssign( SymAddr( GetPtr() ),
                          CGInteger( stmt->u.st.address, TY_INTEGER ),
                          TY_INTEGER ) );
        RefStmtLabel( stmt );
    }
}


void    FCIfArith( void ) {
//===================

// Set up control structure for arithmetic if.

    cg_name     if_expr;
    sym_id      lt;
    sym_id      eq;
    sym_id      gt;
    cg_type     typ;

    typ = GetType( GetU16() );
    if_expr = XPopValue( typ );
    lt = GetPtr();
    eq = GetPtr();
    gt = GetPtr();
    if( lt == gt ) {
        CGControl( O_IF_TRUE,
                   CGCompare( O_EQ, if_expr, CGInteger( 0, typ ), typ ),
                   GetStmtLabel( eq ) );
        CGControl( O_GOTO, NULL, GetStmtLabel( lt ) );
    } else if( lt == eq ) {
        CGControl( O_IF_TRUE,
                   CGCompare( O_GT, if_expr, CGInteger( 0, typ ), typ ),
                   GetStmtLabel( gt ) );
        CGControl( O_GOTO, NULL, GetStmtLabel( eq ) );
    } else if( eq == gt ) {
        CGControl( O_IF_TRUE,
                   CGCompare( O_LT, if_expr, CGInteger( 0, typ ), typ ),
                   GetStmtLabel( lt ) );
        CGControl( O_GOTO, NULL, GetStmtLabel( eq ) );
    } else {
        CG3WayControl( if_expr, GetStmtLabel( lt ), GetStmtLabel( eq ),
                       GetStmtLabel( gt ) );
    }
    RefStmtLabel( lt );
    RefStmtLabel( eq );
    RefStmtLabel( gt );
}


void    FCAssignedGOTOList( void ) {
//============================

// Perform assigned GOTO with list.

    sel_handle          s;
    label_handle        label;
    sym_id              sn;
    sym_id              var;
    obj_ptr             curr_obj;

    var = GetPtr();
    curr_obj = FCodeTell( 0 );
    s = CGSelInit();
    for(;;) {
        sn = GetPtr();
        if( sn == NULL ) break;
        if( (sn->u.st.flags & SN_IN_GOTO_LIST) == 0 ) {
            sn->u.st.flags |= SN_IN_GOTO_LIST;
            label = GetStmtLabel( sn );
            CGSelCase( s, label, sn->u.st.address );
        }
    }
    label = BENewLabel();
    CGSelOther( s, label );
    CGSelect( s, CGUnary( O_POINTS, CGFEName( var, TY_INTEGER ), TY_INTEGER ) );
    CGControl( O_LABEL, NULL, label );
    BEFiniLabel( label );
    FCodeSeek( curr_obj );
    for(;;) {
        sn = GetPtr();
        if( sn == NULL ) break;
        sn->u.st.flags &= ~SN_IN_GOTO_LIST;
        RefStmtLabel( sn );
    }
}


void    FCComputedGOTO( void ) {
//========================

// Perform computed GOTO.

    DoSelect( FC_COMPUTED_GOTO );
}


void    FCStartRB( void ) {
//===================

// Start a REMOTE BLOCK.

    sym_id      rb;

    rb = GetPtr();
    CGControl( O_LABEL, NULL, GetLabel( rb->u.ns.si.rb.entry ) );
    RBReferenced( rb );
}


void    FCWarp( void ) {
//================

// Process WARP F-Code.

    sym_id      arr;
    warp_label  init_label;

    arr = GetPtr();
    init_label = arr->u.ns.si.va.u.dim_ext->l.init_label;
    WarpReturn = FCodeSeek( init_label );
}


void    FCWarpReturn( void ) {
//======================

// Return from a warp (array initialization).

    FCodeSeek( WarpReturn );
}


void    FCExecute( void ) {
//===================

// Process EXECUTE F-Code (call remote block).

    sym_id      rb;

    rb = GetPtr();
    CGControl( O_INVOKE_LABEL, NULL, GetLabel( rb->u.ns.si.rb.entry ) );
    RBReferenced( rb );
}


static  void    RBReferenced( sym_id rb ) {
//=========================================

// REMOTE BLOCK has been referenced.

    rb->u.ns.si.rb.ref_count--;
    if( rb->u.ns.si.rb.ref_count == 0 ) {
        DoneLabel( rb->u.ns.si.rb.entry );
    }
}


void    FCEndRB( void ) {
//=================

// Terminate a REMOTE BLOCK.

    CGControl( O_LABEL_RETURN, NULL, NULL );
}


void    FCSFCall( void ) {
//==================

// Call a statement function.

    sym_id      sf;
    sym_id      sf_arg;
    sym_id      tmp;
    cg_type     sf_type;
    cg_name     arg_list;
    cg_name     value;
    cg_cmplx    z;
    obj_ptr     curr_obj;

    sf = GetPtr();
    arg_list = NULL;
    value = NULL;
    sf_type = 0;
    for(;;) {
        sf_arg = GetPtr();
        if( sf_arg == NULL ) break;
        if( sf_arg->u.ns.u1.s.typ == FT_CHAR ) {
            value = Concat( 1, CGFEName( sf_arg, TY_CHAR ) );
        } else {
            sf_type = F77ToCGType( sf_arg );
            if( TypeCmplx( sf_arg->u.ns.u1.s.typ ) ) {
                XPopCmplx( &z, sf_type );
                sf_type = CmplxBaseType( sf_type );
                value = ImagPtr( SymAddr( sf_arg ), sf_type );
                CGTrash( CGAssign( value, z.imagpart, sf_type ) );
                value = CGFEName( sf_arg, sf_type );
                value = CGAssign( value, z.realpart, sf_type );
            } else {
                value = CGFEName( sf_arg, sf_type );
                value = CGAssign( value, XPopValue( sf_type ), sf_type );
            }
        }
        if( arg_list == NULL ) {
            arg_list = value;
        } else {
            arg_list = CGBinary( O_COMMA, arg_list, value, TY_DEFAULT );
        }
    }
    if( sf->u.ns.u1.s.typ == FT_CHAR ) {
        tmp = GetPtr();
        value = CGUnary( O_POINTS, CGFEName( tmp, TY_CHAR ), TY_CHAR );
        value = CGAssign( CGFEName( sf, TY_CHAR ), value, TY_CHAR );
        if( arg_list == NULL ) {
            arg_list = value;
        } else {
            arg_list = CGBinary( O_COMMA, arg_list, value, TY_DEFAULT );
        }
        value = CGFEName( tmp, TY_CHAR );
    } else {
        sf_type = F77ToCGType( sf );
        if( (OZOpts & OZOPT_O_INLINE) == 0 ) {
            value = CGUnary( O_POINTS, CGFEName( sf, sf_type ), sf_type );
        }
    }
    if( OZOpts & OZOPT_O_INLINE ) {
        if( arg_list != NULL ) {
            CGTrash( arg_list );
        }
        curr_obj = FCodeSeek( sf->u.ns.si.sf.u.sequence );
        GetObjPtr();
        FCodeSequence();
        FCodeSeek( curr_obj );
        if( sf->u.ns.u1.s.typ == FT_CHAR ) {
            CGTrash( XPop() );
            XPush( value );
        } else if( TypeCmplx( sf->u.ns.u1.s.typ ) ) {
            XPopCmplx( &z, sf_type );
            sf_type = CmplxBaseType( sf_type );
            XPush( TmpVal( MkTmp( z.imagpart, sf_type ), sf_type ) );
            XPush( TmpVal( MkTmp( z.realpart, sf_type ), sf_type ) );
        } else {
            XPush( TmpVal( MkTmp( XPopValue( sf_type ), sf_type ), sf_type ) );
        }
    } else {
        value = CGWarp( arg_list, GetLabel( sf->u.ns.si.sf.u.location ), value );
        // consider: y = f( a, f( b, c, d ), e )
        // make sure that inner reference to f gets evaluated before we assign
        // arguments for outer reference
        value = CGEval( value );
        if( TypeCmplx( sf->u.ns.u1.s.typ ) ) {
            SplitCmplx( TmpPtr( MkTmp( value, sf_type ), sf_type ), sf_type );
        } else {
            XPush( value );
        }
        RefStmtFunc( sf );
    }
}


void            FCStartSF( void ) {
//===========================

// Start definition of a statement function.

    sym_id      sf;

    if( OZOpts & OZOPT_O_INLINE ) {
        // skip the statement function
        FCodeSeek( GetObjPtr() );
    } else {
        sf = GetPtr();
        SFEndLabel = GetU16();
        CGControl( O_GOTO, NULL, GetLabel( SFEndLabel ) );
        CGControl( O_LABEL, NULL, GetLabel( sf->u.ns.si.sf.u.location ) );
    }
}


void            FCEndSF( void ) {
//=========================

// End definition of a statement function.
// This F-CODE is only generated if we are NOT generating statement
// functions in-line.

    sym_id      sf;

    sf = GetPtr();
    CGControl( O_LABEL_RETURN, NULL, NULL );
    CGControl( O_LABEL, NULL, GetLabel( SFEndLabel ) );
    DoneLabel( SFEndLabel );
    RefStmtFunc( sf );
}


void            FCSFReferenced( void ) {
//================================

// Statement function has been referenced; check if its label can be freed.

    sym_id      sf;

    sf = SFSymId;
    for(;;) {
        if( sf == NULL ) break;
        if( sf->u.ns.si.sf.header->ref_count == 0 ) {
            if( sf->u.ns.si.sf.u.location != 0 ) {
                DoneLabel( sf->u.ns.si.sf.u.location );
                sf->u.ns.si.sf.u.location = 0;
            }
        }
        sf = sf->u.ns.si.sf.header->link;
    }
}


static  void    RefStmtFunc( sym_id sf ) {
//========================================

// A statement function has been referenced.

    sf->u.ns.si.sf.header->ref_count--;
}


void    DoneLabel( label_id label ) {
//===================================

// Free specified label since it will no longer be referenced.

    label_entry **owner;
    label_entry *curr;

    owner = (label_entry **)&LabelList;
    for(;;) {
        curr = *owner;
        if( curr->label == label ) break;
        owner = &curr->link;
    }
    *owner = curr->link;
    BEFiniLabel( curr->handle );
    FMemFree( curr );
}


void    FCFreeLabel( void ) {
//=====================

// Free specified label since it will no longer be referenced.

    DoneLabel( GetU16() );
}


void    RefStmtLabel( sym_id sn ) {
//=================================

// Statement number has been referenced.

    if( sn->u.st.ref_count == 0 ) {
        InfoError( CP_ERROR, "unaccounted referenced to label" );
    } else {
        sn->u.st.ref_count--;
        if( sn->u.st.ref_count == 0 ) {
            DoneLabel( sn->u.st.address );
        }
    }
}
