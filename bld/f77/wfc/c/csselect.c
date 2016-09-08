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
* Description:  processing for SELECT, CASE, OTHERWISE, and ENDSELECT
*
****************************************************************************/


#include "ftnstd.h"
#include "errcod.h"
#include "global.h"
#include "opr.h"
#include "fmemmgr.h"
#include "recog.h"
#include "types.h"
#include "ferror.h"
#include "utility.h"
#include "proctbl.h"
#include "csselect.h"
#include "csutls.h"
#include "cgmagic.h"
#include "fcodes.h"
#include "gflow.h"


/* forward declarations */
void            CpOtherwise( void );
static void     CaseHandler( void );

case_entry *NewCase( void )
{
    case_entry  *ptr;

    ptr = FMemAlloc( sizeof( case_entry ) );
    ptr->label.g_label = 0;
    ptr->low = 0;
    ptr->high = 0;
    ptr->link = NULL;
    ptr->multi_case = false;
    return( ptr );
}

void CpSelect( void )
{
// Compile a SELECT statement.

    Remember.slct = true;
    CSExtn();
    AddCSNode( CS_SELECT );
    CSHead->branch = NextLabel();
    CSHead->bottom = NextLabel();
    CSHead->cs_info.cases = NewCase();
    if( RecKeyWord( "CASE" ) ) {
        AdvanceITPtr();
    } else {
        ReqNOpn();
        AdvanceITPtr();
    }
    ReqOpenParen();
    if( !RecEOS() ) { // consider: SELECT
        SelectExpr();
        CSHead->cs_info.cases->sel_type = CITNode->typ;
    } else {
        CSHead->cs_info.cases->sel_type = FT_NO_TYPE;
    }
    InitSelect();
    AdvanceITPtr();
    ReqCloseParen();
    if( !RecKeyWord( "FROM" ) ) {
        ReqNOpn();
    }
    AdvanceITPtr();
    ColonLabel();
}

void CpCase( void )
{
// Compile a CASE statement.

    if( RecKeyWord( "DEFAULT" ) ) {
        AdvanceITPtr();
        CpOtherwise();
    } else {
        if( CSHead->typ == CS_SELECT ) {
            CaseHandler();
        } else if( CSHead->typ == CS_CASE ) {
            GBranch( CSHead->bottom );
            CaseHandler();
        } else if( CSHead->typ == CS_OTHERWISE ) {
            Error( SP_OTHERWISE_LAST );
        } else {
            Match();
        }
    }
}

static intstar4 MinCaseValue( TYPE typ )
{
// Get a value for case expression.

    if( _IsTypeInteger( typ ) ) {
        return( INT_MIN );
    } else { // if( typ = FT_CHAR ) {
        return( 0 );
    }
}


static intstar4 MaxCaseValue( TYPE typ )
{
// Get a value for case expression.

    if( _IsTypeInteger( typ ) ) {
        return( INT_MAX );
    } else { // if( typ = FT_CHAR ) {
        return( 255 );
    }
}


static intstar4 CaseValue( void )
{
// Get a value for case expression.

    if( _IsTypeInteger( CITNode->typ ) ) {
        return( ITIntValue( CITNode ) );
    } else if( _IsTypeLogical( CITNode->typ  ) ) {
        return( _LogValue( CITNode->value.logstar1 ) );
    } else { // if( CITNode->typ = FT_CHAR ) {
        return( *CITNode->value.cstring.strptr );
    }
}

static void CaseHandler( void )
{
    label_id    label;
    case_entry  *link;
    case_entry  *kase;
    intstar4    low = 0;
    intstar4    high = 0;
    bool        case_ok;
    bool        multi_case;

    CSHead->typ = CS_CASE;
    label = NextLabel();
    GLabel( label );
    if( ReqNOpn() ) { // consider: CASE 10
        AdvanceITPtr();
    }
    multi_case = false;
    ReqOpenParen();
    for(;;) {
        if( _IsTypeLogical( CSHead->cs_info.cases->sel_type ) ) {
            // no range allowed for LOGICAL select expressions
            case_ok = ConstExpr( CSHead->cs_info.cases->sel_type );
            if( case_ok ) {
                low = CaseValue();
                high = low;
            }
            AdvanceITPtr();
        } else {
            if( RecNOpn() && RecNextOpr( OPR_COL ) ) {
                low = MinCaseValue( CSHead->cs_info.cases->sel_type );
                AdvanceITPtr();
                case_ok = ConstExpr( CSHead->cs_info.cases->sel_type );
                if( case_ok ) {
                    high = CaseValue();
                }
                AdvanceITPtr();
            } else {
                case_ok = ConstExpr( CSHead->cs_info.cases->sel_type );
                if( case_ok ) {
                    low = CaseValue();
                    high = low;
                }
                AdvanceITPtr();
                if( RecColon() ) {
                    if( RecNOpn() &&
                        ( RecNextOpr( OPR_RBR ) || RecNextOpr( OPR_COM ) ) ) {
                        high = MaxCaseValue( CSHead->cs_info.cases->sel_type );
                    } else {
                        case_ok &= ConstExpr( CSHead->cs_info.cases->sel_type );
                        if( case_ok ) {
                            high = CaseValue();
                        }
                    }
                    AdvanceITPtr();
                }
            }
        }
        if( case_ok ) {
            if( high < low ) {
                Error( SP_NEVER_CASE );
            } else {
                kase = CSHead->cs_info.cases;
                while( kase->link != NULL ) {
                    if( kase->link->low > high )
                        break;
                    kase = kase->link;
                }
                if( ( kase != CSHead->cs_info.cases ) && ( kase->high >= low ) ) {
                    Error( SP_CASE_OVERLAP );
                } else {
                    link = kase->link;
                    kase->link = NewCase();
                    kase = kase->link;
                    kase->link = link;
                    kase->label.g_label = label;
                    kase->high = high;
                    kase->low = low;
                    kase->multi_case = multi_case;
                    multi_case = true;
                }
            }
        }
        if( !RecComma() ) {
            break;
        }
    }
    ReqCloseParen();
    ReqNOpn();
    AdvanceITPtr();
    ReqEOS();
    CSHead->block = ++BlockNum;
}

void CpOtherwise( void )
{
// Compile an OTHERWISE statement.

    if( (CSHead->typ == CS_CASE) || (CSHead->typ == CS_SELECT) ) {
        GBranch( CSHead->bottom );
        CSHead->typ = CS_OTHERWISE;
        CSHead->cs_info.cases->label.g_label = NextLabel();
        CSHead->cs_info.cases->multi_case = false;
        GLabel( CSHead->cs_info.cases->label.g_label );
        CSHead->block = ++BlockNum;
    } else if( CSHead->typ == CS_OTHERWISE ) {
        Error( SP_OTHERWISE_LAST );
    } else {
        Match();
    }
    CSNoMore();
}

void CpEndSelect( void )
{
// Compile an ENDSELECT statement.

    GBranch( CSHead->bottom );
    if( CSHead->typ == CS_SELECT ) {
        Error( SP_EMPTY_SELECT );
    } else if( CSHead->typ == CS_CASE ) {
        CSHead->cs_info.cases->label.g_label = CSHead->bottom;
        FiniSelect();
    } else if( CSHead->typ == CS_OTHERWISE ) {
        FiniSelect();
    } else {
        Match();
    }
    GLabel( CSHead->bottom );
    // the bottom label will be freed as the default case when no default
    // case has been specified
    if( CSHead->typ != CS_CASE ) {
        FreeLabel( CSHead->bottom );
    }
    FreeLabel( CSHead->branch );
    DelCSNode();
}
