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
// UPCAT        : UPSCAN concatenation sequences
//

#include "ftnstd.h"
#include "opr.h"
#include "opn.h"
#include "errcod.h"
#include "global.h"
#include "iflookup.h"
#include "recog.h"
#include "emitobj.h"
#include "insert.h"
#include "utility.h"
#include "arutls.h"
#include "upscan.h"
#include "usconst.h"
#include "gstring.h"
#include "upcat.h"


static  void    FoldCatSequence( itnode *cit ) {
//==============================================

// Fold a sequnece of character constants.

    uint        size;
    uint        num;
    itnode      *save;

    save = CITNode;
    CITNode = cit;
    num = 0;
    size = 0;
    for(;;) {
        if( CITNode->opn.us != USOPN_CON ) break;
        num++;
        if( CITNode->typ != FT_CHAR ) {
            TypeErr( MD_ILL_OPR, CITNode->typ );
        } else {
            size += CITNode->value.cstring.len;
        }
        AdvanceITPtr();
        if( CITNode->opr != OPR_CAT ) break;
    }
    if( !AError ) {
        CITNode = cit;
        if( num > 1 ) {
            ConstCat( size );
        } else if( num == 1 ) {
            AddConst( CITNode );
        }
    }
    CITNode = save;
}


static  void    ChkConstCatOpn( itnode *cat_opn ) {
//=================================================

    if( cat_opn->opn.us == USOPN_CON ) {
        FoldCatSequence( cat_opn );
        if( !AError ) {
            PushOpn( cat_opn );
        }
    }
}


static  void    GenCatOpn( void ) {
//===========================

    if( CITNode->opn.us != USOPN_CON ) {
        ChkConstCatOpn( CITNode->link );
        PushOpn( CITNode );
    }
}


static  void    FoldCat( void ) {
//=========================

    GenCatOpn();
    ChkConstCatOpn( CITNode );
}

void            CatOpn( void ) {
//========================

// Process a concatenation operand.

    GenCatOpn();
    BackTrack();
}

static  int     ScanCat( int *size_ptr ) {
//========================================

// Scan for strings to be concatenated.

    uint        cat_size;
    itnode      *itptr;
    uint        num_cats;

    itptr = CITNode;
    cat_size = 0;
    num_cats = 0;
    for(;;) {
        if( CITNode->opn.ds == DSOPN_PHI ) {
            // no operand (A = B // // C)
            TypeErr( SX_WRONG_TYPE, FT_CHAR );
        } else if( CITNode->typ != FT_CHAR ) {
            TypeTypeErr( MD_MIXED, FT_CHAR, CITNode->typ );
        } else if( ( CITNode->size == 0 ) && ( size_ptr != NULL ) ) {
            // NULL 'size_ptr' means we are concatenating into a character
            // variable so character*(*) variables are allowed.
            OpndErr( CV_BAD_LEN );
        } else {
            cat_size += CITNode->size;
        }
        CITNode = CITNode->link;
        num_cats++;
        if( CITNode->opr != OPR_CAT ) break;
    }
    CITNode = itptr;
    if( size_ptr != NULL ) {
        *size_ptr = cat_size;
    }
    return( num_cats );
}


void            FiniCat( void ) {
//=========================

// Finish concatenation.

    int         num;
    sym_id      result;
    int         size;

    // Make sure we don't PushOpn() a constant expression
    // in case it's for a PARAMETER constant
    if( CITNode->opn.us == USOPN_CON ) {
        FoldCatSequence( CITNode );
        if( AError ) return;
    } else {
        GenCatOpn();
    }
    num = ScanCat( &size );
    if( num != 1 ) {
        PushOpn( CITNode );
        result = GStartCat( num, size );
        CatArgs( num );
        CITNode->size = size;
        GStopCat( num, result );
    }
}


int             AsgnCat( void ) {
//=========================

// Get character operand to assign.

    return( ScanCat( NULL ) );
}


void            CatBack( void ) {
//=========================

// Scan back on = // sequence if RHS is a char expression.
// All parens and lists must have been removed already.
// Consider:    l = a//b .eq. c//d
//                  vs
//              c = x//y//z


    itnode      *itptr;

    itptr = CITNode->link->link; // point one operator past "//"
    for(;;) {
        if( itptr->opr == OPR_TRM ) break;
        if( itptr->opr != OPR_CAT ) {
            FiniCat();
            return;
        }
        itptr = itptr->link;
    }
    FoldCat();
    BackTrack(); // eg : a = b//c//d//e
}


void            CatAxeParens( void ) {
//==============================

// Remove LBR on ( // sequence.
//
// Before:                            |   After:
//                ---------------     |                 ---------------
//                | opr1 | PHI  |     |                    released**
//                ---------------     |                 ---------------
//       CIT ==>  | (    | opn2 |     |        CIT ==>  | opr1 | opn2 |
//                ---------------     |                 ---------------
//                | //   | opn3 |     |                 | //   | opn3 |
//                ---------------     |                 ---------------
//                      ...           |                       ...
//
//  ** see KillOpnOpr() for case where first node is start-node of expr

    BackTrack();
    ReqNOpn();
    MoveDown();
    if( CITNode->is_catparen ) {
        KillOpnOpr();
    } else {
        FiniCat();
    }
}


static  itnode  *findMatch( bool *ok_to_axe, bool *all_const_opns ) {
//===================================================================

    itnode      *cit;
    int         num;

    num = 1;
    cit = BkLink;
    *ok_to_axe = true;
    if( all_const_opns != NULL ) {
        *all_const_opns = true;
    }
    for(;;) {
        if( all_const_opns != NULL ) {
            if( (cit->opn.ds != DSOPN_PHI) && (cit->opn.us != USOPN_CON) ) {
                *all_const_opns = false;
            }
        }
        if( ( cit->opr == OPR_LBR ) || ( cit->opr == OPR_FBR ) ) {
            // if it is a left parenthesis of a concatenation expression, we
            // simply ignore it since the right parenthesis was already
            // removed prior to calling this function
            if( !cit->is_catparen ) {
                num--;
            }
        } else if( cit->opr == OPR_RBR ) {
            num++;
        } else if( ( cit->opr != OPR_CAT ) && ( num == 1 ) ) {
            // consider:
            //      l = ( name .eq. 'abc'//'def' )
            // and:
            //      a(5)(2:3)//'def'
            *ok_to_axe = false;
        }
        if( num == 0 ) break;
        cit = cit->link;
        if( cit == NULL ) break;
    }
    return( cit );
}


void            ParenCat( void ) {
//==========================

// Check if ) matches ( as opposed to [.
// called on ) // sequence

    itnode      *cit;
    bool        ok_to_axe;
    bool        all_const_opns;

    cit = findMatch( &ok_to_axe, &all_const_opns );
    if( cit != NULL ) {
        // consider:    a(1)(2:3)//c
        if( ( cit->opr == OPR_LBR ) && ok_to_axe ) {
            ReqNOpn();
            cit->is_catparen = 1;
            cit = CITNode;
            AdvanceITPtr();
            FreeOneNode( cit );
        // check for CHAR(73) - CHAR is allowed in constant expressions
        } else if( (cit->opr != OPR_FBR) || !all_const_opns ||
                   ((cit->link->flags & SY_CLASS ) != SY_SUBPROGRAM) ||
                   ((cit->link->flags & SY_INTRINSIC) == 0) ||
                   (cit->link->sym_ptr->u.ns.si.fi.index != IF_CHAR) ) {
            ChkConstCatOpn( CITNode->link );
        }
    }
    BackTrack();
}


void            CatParen( void ) {
//==========================

// Check if ) matches ( as opposed to [.
// called on // ) sequence

    itnode      *cit;
    bool        ok_to_axe;

    cit = findMatch( &ok_to_axe, NULL );
    if( cit != NULL ) {
        if( ( cit->opr == OPR_LBR ) && ok_to_axe ) {
            cit->is_catparen = 1;
            cit = CITNode;
            AdvanceITPtr();
            ReqNOpn();
            cit->link = CITNode->link;
            FreeOneNode( CITNode );
            CITNode = cit;
        } else {
            CatOpn();
        }
    }
}


void            ChkCatOpn( void ) {
//===========================

// Check if ) is the start of a concatenation operand.
// Called on ) rel sequence since only relational operators are allowed with
// character arguments.
// Consider:
//      if( a(1)//a(2) .eq. 'ab' )then
// We want to evaluate 'ab' first. Otherwise, a(2) would get evaluated,
// followed by 'ab' and finally a(1) -- which is incorrect.

    itnode      *cit;
    bool        ok_to_axe;

    cit = findMatch( &ok_to_axe, NULL );
    if( cit != NULL ) {
        if( cit->opr == OPR_FBR ) {
            if( cit->link->opr == OPR_CAT ) {
                CatOpn();
                return;
            }
        }
    }
    BackTrack();
}


void            CatArgs( int num ) {
//==================================

// Generate code for concatenation arguments.

    itnode      *itptr;
    itnode      *junk;
    int         count;

    itptr = CITNode;
    count = num;
    for(;;) {
        // Don't call CatArg() if no operand or not of type character.
        // This covers the case where invalid operands are specified.
        if( ( itptr->opn.ds != DSOPN_PHI ) && ( itptr->typ == FT_CHAR ) ) {
            GCatArg( itptr );
        }
        if( --count <= 0 ) break;
        itptr = itptr->link;
    }
    if( CITNode != itptr ) {
        junk = CITNode->link;
        CITNode->link = itptr->link;
        itptr->link = NULL;
        FreeITNodes( junk );
    }
}
