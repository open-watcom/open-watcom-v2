/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
#include "i64.h"
#include "cgswitch.h"
#include "cgprotos.h"


static obj_ptr          WarpReturn;
static label_entry      *LabelList;     // list of labels


void    InitLabels( void )
//========================
// Initialize label processing.
{
    LabelList = NULL;
}


void    FiniLabels( bool back_label )
//===================================
// Free specified class of labels.
{
    label_entry     **owner;
    label_entry     *curr;

    owner = &LabelList;
    while( (curr = *owner) != NULL ) {
        if( curr->back_label == back_label ) {
            if( (CGFlags & CG_FATAL) == 0 ) {
                if( curr->back_label ) {
                    BEFiniBack( curr->u.cgbck );
                    BEFreeBack( curr->u.cgbck );
                } else {
                    InfoError( CP_ERROR, "unfreed label" );
                    BEFiniLabel( curr->u.cglbl );
                }
            }
            *owner = curr->link;
            MemFree( curr );
        } else {
            owner = &curr->link;
        }
    }
}


static  label_entry     *FindLabel( label_id label )
//==================================================
// Search for given label.
{
    label_entry     *le;

    for( le = LabelList; le != NULL; le = le->link ) {
        if( le->label == label ) {
            break;
        }
    }
    if( le == NULL ) {
        le = MemAlloc( sizeof( label_entry ) );
        le->label = label;
        le->back_label = false;
        le->u.cglbl = NULL;
        le->link = LabelList;
        LabelList = le;
    }
    return( le );
}


label_handle    GetCgLabel( label_id label )
//==========================================
// Get a cg code label.
{
    label_entry     *le;

    le = FindLabel( label );
    if( le->u.cglbl == NULL ) {
        le->u.cglbl = BENewLabel();
    }
    return( le->u.cglbl );
}


void    FCJmpFalse( void )
//========================
{
    unsigned_16     typ_info;
    cg_type         cgtyp;
    cg_name         bool_expr;

    typ_info = GetU16();
    cgtyp = GetCGType( typ_info );
    if( IntType( typ_info ) ) {
        bool_expr = CGCompare( O_NE, XPopValue( cgtyp ),
                               CGInteger( 0, cgtyp ), cgtyp );
    } else {
        bool_expr = XPopValue( cgtyp );
    }
    CGControl( O_IF_FALSE, bool_expr, GetCgLabel( GetU16() ) );
}


void    FCJmpAlways( void )
//=========================
{
    CGControl( O_GOTO, NULL, GetCgLabel( GetU16() ) );
}


void    FCStmtJmpAlways( void )
//=============================
{
    sym_id          sn;

    sn = GetPtr();
    CGControl( O_GOTO, NULL, _GetStmtCgLabel( sn ) );
    RefStmtLabel( sn );
}


void    FCDefineLabel( void )
//===========================
{
    CGControl( O_LABEL, NULL, GetCgLabel( GetU16() ) );
}


void    FCStmtDefineLabel( void )
//===============================
{
    sym_id          sn;

    sn = GetPtr();
    CGControl( O_LABEL, NULL, _GetStmtCgLabel( sn ) );
    RefStmtLabel( sn );
}


back_handle     GetCgBckLabel( label_id label )
//===========================================
// Get a cg data label.
{
    label_entry     *le;

    le = FindLabel( label );
    if( le->u.cgbck == NULL ) {
        le->u.cgbck = BENewBack( NULL );
        le->back_label = true;
    }
    return( le->u.cgbck );
}


void    FCAssign( void )
//======================
// Process ASSIGN statement.
{
    sym_id          stmt;

    stmt = GetPtr();
    if( stmt->u.st.flags & SN_FORMAT ) {
        CGDone( CGAssign( SymAddr( GetPtr() ),
                          CGBackName( _GetStmtCgBckLabel( stmt ),
                                      TY_LOCAL_POINTER ),
                          TY_LOCAL_POINTER ) );
    } else {
        CGDone( CGAssign( SymAddr( GetPtr() ),
                          CGInteger( _GetStmtLabel( stmt ), TY_INTEGER ),
                          TY_INTEGER ) );
        RefStmtLabel( stmt );
    }
}


void    FCIfArith( void )
//=======================
// Set up control structure for arithmetic if.
{
    cg_name         if_expr;
    sym_id          lt;
    sym_id          eq;
    sym_id          gt;
    cg_type         cgtyp;

    cgtyp = GetCGType( GetU16() );
    if_expr = XPopValue( cgtyp );
    lt = GetPtr();
    eq = GetPtr();
    gt = GetPtr();
    if( lt == gt ) {
        CGControl( O_IF_TRUE,
                   CGCompare( O_EQ, if_expr, CGInteger( 0, cgtyp ), cgtyp ),
                   _GetStmtCgLabel( eq ) );
        CGControl( O_GOTO, NULL, _GetStmtCgLabel( lt ) );
    } else if( lt == eq ) {
        CGControl( O_IF_TRUE,
                   CGCompare( O_GT, if_expr, CGInteger( 0, cgtyp ), cgtyp ),
                   _GetStmtCgLabel( gt ) );
        CGControl( O_GOTO, NULL, _GetStmtCgLabel( eq ) );
    } else if( eq == gt ) {
        CGControl( O_IF_TRUE,
                   CGCompare( O_LT, if_expr, CGInteger( 0, cgtyp ), cgtyp ),
                   _GetStmtCgLabel( lt ) );
        CGControl( O_GOTO, NULL, _GetStmtCgLabel( eq ) );
    } else {
        CG3WayControl( if_expr, _GetStmtCgLabel( lt ), _GetStmtCgLabel( eq ),
                       _GetStmtCgLabel( gt ) );
    }
    RefStmtLabel( lt );
    RefStmtLabel( eq );
    RefStmtLabel( gt );
}


void    FCAssignedGOTOList( void )
//================================
// Perform assigned GOTO with list.
{
    sel_handle      s;
    label_handle    cglbl;
    sym_id          sn;
    sym_id          var;
    obj_ptr         curr_obj;
    signed_64       tmp;

    var = GetPtr();
    curr_obj = FCodeTell( 0 );
    s = CGSelInit();
    while( (sn = GetPtr()) != NULL ) {
        if( (sn->u.st.flags & SN_IN_GOTO_LIST) == 0 ) {
            sn->u.st.flags |= SN_IN_GOTO_LIST;
            cglbl = _GetStmtCgLabel( sn );
            Set64ValU32( tmp, _GetStmtLabel( sn ) );
            CGSelCase( s, cglbl, tmp );
        }
    }
    cglbl = BENewLabel();
    CGSelOther( s, cglbl );
    CGSelect( s, CGUnary( O_POINTS, CGFEName( var, TY_INTEGER ), TY_INTEGER ) );
    CGControl( O_LABEL, NULL, cglbl );
    BEFiniLabel( cglbl );
    FCodeSeek( curr_obj );
    while( (sn = GetPtr()) != NULL ) {
        sn->u.st.flags &= ~SN_IN_GOTO_LIST;
        RefStmtLabel( sn );
    }
}


void    FCComputedGOTO( void )
//============================
// Perform computed GOTO.
{
    DoSelect( FC_COMPUTED_GOTO );
}


static  void    RBReferenced( sym_id rb )
//=======================================
// REMOTE BLOCK has been referenced.
{
    rb->u.ns.si.rb.ref_count--;
    if( rb->u.ns.si.rb.ref_count == 0 ) {
        DoneLabel( rb->u.ns.si.rb.entry );
    }
}


void    FCStartRB( void )
//=======================
// Start a REMOTE BLOCK.
{
    sym_id          rb;

    rb = GetPtr();
    CGControl( O_LABEL, NULL, GetCgLabel( rb->u.ns.si.rb.entry ) );
    RBReferenced( rb );
}


void    FCWarp( void )
//====================
// Process WARP F-Code.
{
    sym_id          arr;
    warp_label      init_label;

    arr = GetPtr();
    init_label = arr->u.ns.si.va.u.dim_ext->l.init_label;
    WarpReturn = FCodeSeek( init_label );
}


void    FCWarpReturn( void )
//==========================
// Return from a warp (array initialization).
{
    FCodeSeek( WarpReturn );
}


void    FCExecute( void )
//=======================
// Process EXECUTE F-Code (call remote block).
{
    sym_id          rb;

    rb = GetPtr();
    CGControl( O_INVOKE_LABEL, NULL, GetCgLabel( rb->u.ns.si.rb.entry ) );
    RBReferenced( rb );
}


void    FCEndRB( void )
//=====================
// Terminate a REMOTE BLOCK.
{
    CGControl( O_LABEL_RETURN, NULL, NULL );
}


static  void    RefStmtFunc( sym_id sf )
//======================================
// A statement function has been referenced.
{
    sf->u.ns.si.sf.header->ref_count--;
}


void    FCSFCall( void )
//======================
// Call a statement function.
{
    sym_id          sf;
    sym_id          sf_arg;
    sym_id          tmp;
    cg_type         sf_cgtyp;
    cg_name         arg_list;
    cg_name         value;
    cg_cmplx        z;
    obj_ptr         curr_obj;

    sf = GetPtr();
    arg_list = NULL;
    value = NULL;
    sf_cgtyp = 0;
    while( (sf_arg = GetPtr()) != NULL ) {
        if( sf_arg->u.ns.u1.s.typ == FT_CHAR ) {
            value = Concat( 1, CGFEName( sf_arg, TY_CHAR ) );
        } else {
            sf_cgtyp = F77ToCGType( sf_arg );
            if( TypeCmplx( sf_arg->u.ns.u1.s.typ ) ) {
                XPopCmplx( &z, sf_cgtyp );
                sf_cgtyp = CmplxBaseType( sf_cgtyp );
                value = ImagPtr( SymAddr( sf_arg ), sf_cgtyp );
                CGTrash( CGAssign( value, z.imagpart, sf_cgtyp ) );
                value = CGFEName( sf_arg, sf_cgtyp );
                value = CGAssign( value, z.realpart, sf_cgtyp );
            } else {
                value = CGFEName( sf_arg, sf_cgtyp );
                value = CGAssign( value, XPopValue( sf_cgtyp ), sf_cgtyp );
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
        sf_cgtyp = F77ToCGType( sf );
        if( (OZOpts & OZOPT_O_INLINE) == 0 ) {
            value = CGUnary( O_POINTS, CGFEName( sf, sf_cgtyp ), sf_cgtyp );
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
            XPopCmplx( &z, sf_cgtyp );
            sf_cgtyp = CmplxBaseType( sf_cgtyp );
            XPush( TmpVal( MkTmp( z.imagpart, sf_cgtyp ), sf_cgtyp ) );
            XPush( TmpVal( MkTmp( z.realpart, sf_cgtyp ), sf_cgtyp ) );
        } else {
            XPush( TmpVal( MkTmp( XPopValue( sf_cgtyp ), sf_cgtyp ), sf_cgtyp ) );
        }
    } else {
        value = CGWarp( arg_list, GetCgLabel( sf->u.ns.si.sf.u.location ), value );
        // consider: y = f( a, f( b, c, d ), e )
        // make sure that inner reference to f gets evaluated before we assign
        // arguments for outer reference
        value = CGEval( value );
        if( TypeCmplx( sf->u.ns.u1.s.typ ) ) {
            SplitCmplx( TmpPtr( MkTmp( value, sf_cgtyp ), sf_cgtyp ), sf_cgtyp );
        } else {
            XPush( value );
        }
        RefStmtFunc( sf );
    }
}


void            FCStartSF( void )
//===============================
// Start definition of a statement function.
{
    sym_id          sf;

    if( OZOpts & OZOPT_O_INLINE ) {
        // skip the statement function
        FCodeSeek( GetObjPtr() );
    } else {
        sf = GetPtr();
        SFEndLabel = GetU16();
        CGControl( O_GOTO, NULL, GetCgLabel( SFEndLabel ) );
        CGControl( O_LABEL, NULL, GetCgLabel( sf->u.ns.si.sf.u.location ) );
    }
}


void            FCEndSF( void )
//=============================
// End definition of a statement function.
// This F-CODE is only generated if we are NOT generating statement
// functions in-line.
{
    sym_id          sf;

    sf = GetPtr();
    CGControl( O_LABEL_RETURN, NULL, NULL );
    CGControl( O_LABEL, NULL, GetCgLabel( SFEndLabel ) );
    DoneLabel( SFEndLabel );
    RefStmtFunc( sf );
}


void            FCSFReferenced( void )
//====================================
// Statement function has been referenced; check if its label can be freed.
{
    sym_id          sf;

    for( sf = SFSymId; sf != NULL; sf = sf->u.ns.si.sf.header->link ) {
        if( sf->u.ns.si.sf.header->ref_count == 0 ) {
            if( sf->u.ns.si.sf.u.location != 0 ) {
                DoneLabel( sf->u.ns.si.sf.u.location );
                sf->u.ns.si.sf.u.location = 0;
            }
        }
    }
}


void    DoneLabel( label_id label )
//=================================
// Free specified label since it will no longer be referenced.
{
    label_entry     **owner;
    label_entry     *curr;

    owner = &LabelList;
    for( ;; ) {
        curr = *owner;
        if( curr->label == label ) {
            break;
        }
        owner = &curr->link;
    }
    *owner = curr->link;
    BEFiniLabel( curr->u.cglbl );
    MemFree( curr );
}


void    FCFreeLabel( void )
//=========================
// Free specified label since it will no longer be referenced.
{
    DoneLabel( GetU16() );
}


void    RefStmtLabel( sym_id sn )
//===============================
// Statement number has been referenced.
{
    if( sn->u.st.ref_count == 0 ) {
        InfoError( CP_ERROR, "unaccounted referenced to label" );
    } else {
        sn->u.st.ref_count--;
        if( sn->u.st.ref_count == 0 ) {
            DoneLabel( _GetStmtLabel( sn ) );
        }
    }
}
