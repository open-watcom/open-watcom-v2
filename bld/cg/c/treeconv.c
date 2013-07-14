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
* Description:  Type conversion folding, tree demotion.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "cgdefs.h"
#include "addrname.h"
#include "tree.h"
#include "treeconv.h"
#include "zoiks.h"
#include "cfloat.h"
#include "utils.h"

extern  void            BurnTree(tn);
extern  tn              TGConst(pointer,type_def*);
extern  bool            NeedPtrConvert(an,type_def*);
extern  type_class_def  TypeClass(type_def*);
extern  cfloat *        CnvCFToType( cfloat *cf, type_def *tipe );


static  bool    DemoteTree( tn name, type_def *tipe, bool just_test ) {
/*********************************************************************/

    type_def    *frum;
    bool        demote_this_node;
    bool        can_demote;

    frum = name->tipe;
    can_demote = FALSE;
    demote_this_node = FALSE;
    if( TypeClass( frum ) <= I4 ) {
        switch( name->class ) {
        case TN_UNARY: /* go left*/
            switch( name->op ) {
            case O_UMINUS:
            case O_COMPLEMENT:
            case O_CONVERT:
            case O_ROUND:
                can_demote = DemoteTree( name->u.left, tipe, just_test );
                demote_this_node = TRUE;
                break;
            case O_POINTS:
#if _TARG_MEMORY == _TARG_LOW_FIRST
                can_demote = TRUE;
                demote_this_node = TRUE;
#endif
                break;
            }
            break;
        case TN_BINARY: /* go left, right*/
            switch( name->op ) {
#if _TARGET & ( _TARG_80386 | _TARG_IAPX86 )
            case O_CONVERT:
                 /* Based pointer junk */
                 break;
            case O_DIV:
            case O_MOD:
                if( name->u.left->tipe->length > tipe->length ||
                    name->rite->tipe->length > tipe->length ) break;
#endif
            case O_TIMES:
            case O_AND:
            case O_OR:
            case O_XOR:
            case O_LSHIFT:
            case O_PLUS:
            case O_MINUS:
                if( name->tipe->refno == TY_HUGE_POINTER ) break;
                if( name->u.left->tipe->refno == TY_HUGE_POINTER ) break;
                if( name->rite->tipe->refno == TY_HUGE_POINTER ) break;
                can_demote = DemoteTree( name->u.left, tipe, just_test );
                if( can_demote ) {
                    can_demote = DemoteTree( name->rite, tipe, just_test );
                }
                demote_this_node = TRUE;
                break;
            }
            break;
        case TN_COMMA:
            can_demote = DemoteTree( name->rite, tipe, just_test );
            break;
        case TN_SIDE_EFFECT:
            can_demote = DemoteTree( name->u.left, tipe, just_test );
            break;
        case TN_FLOW_OUT:
        case TN_CONS:
            can_demote = TRUE;
            demote_this_node = TRUE;
            break;
        }
        if( !just_test && demote_this_node && frum->length > tipe->length ) {
            name->flags |= TF_DEMOTED;
            name->tipe = tipe;
        }
    }
    return( can_demote );
}


extern  void    TGDemote( tn name, type_def *tipe ) {
/***************************************************/

    if( DemoteTree( name, tipe, TRUE ) ) {
        DemoteTree( name, tipe, FALSE );
    }
}


extern  tn      FoldCnvRnd( cg_op op, tn name, type_def *to_tipe ) {
/***************************************************************/

    tn          new;
    cfloat      *cf;
    cfloat      *junk;

    if( name->class == TN_CONS ) {
        if( name->tipe->refno == TY_DEFAULT ) {
            cf = CFCopy( name->u.name->c.value );
        } else {
            cf = CnvCFToType( name->u.name->c.value, name->tipe );
        }
        if( to_tipe->attr & TYPE_FLOAT ) {
            new = TGConst( cf, to_tipe );
        } else if( op == O_CONVERT ) {
            junk = cf;
            cf = CFTrunc( cf );
            CFFree( junk );
            if( to_tipe->refno != TY_DEFAULT ) {
                junk = cf;
                cf = CnvCFToType( cf, to_tipe );
                CFFree( junk );
            }
            new = TGConst( cf, to_tipe );
        } else if( op == O_ROUND ) {
            junk = cf;
            cf = CFRound( cf );
            CFFree( junk );
            if( to_tipe->refno != TY_DEFAULT ) {
                junk = cf;
                cf = CnvCFToType( cf, to_tipe );
                CFFree( junk );
            }
            new = TGConst( cf, to_tipe );
        } else {
            new = NULL;
            Zoiks( ZOIKS_078 ); /* Not supposed to get here, ever */
        }
        BurnTree( name );
    } else {
        TGDemote( name, to_tipe );
        new = NULL;
    }
    return( new );
}
