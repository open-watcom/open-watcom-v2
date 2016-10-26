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
* Description:  utilities for processing control structures
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "errcod.h"
#include "opr.h"
#include "fmemmgr.h"
#include "recog.h"
#include "ferror.h"
#include "insert.h"
#include "utility.h"
#include "proctbl.h"
#include "csutls.h"
#include "csdep.h"
#include "fcodes.h"
#include "gflow.h"

#include <string.h>


extern  char                *StmtKeywords[];

static  const STMT __FAR    CSWords[] = {
        0,
        PR_IF,
        PR_ELSEIF,
        PR_ELSE,
        PR_GUESS,
        PR_ADMIT,
        PR_SELECT,
        PR_CASE,
        PR_OTHERWISE,
        PR_ATEND,
        PR_REMBLK,
        PR_LOOP,
        PR_WHILE,
        PR_DO,
        PR_GOTO,
        PR_DO       // DO WHILE
};


csnode  *NewCSNode( int label_len )
{
// Allocate a new "csnode".

    csnode  *csptr;

    csptr = FMemAlloc( sizeof( csnode ) + label_len );
    csptr->link = NULL;
    csptr->typ = CS_EMPTY_LIST;
    csptr->cs_info.do_parms = NULL;
    csptr->cs_info.cases = NULL;
    csptr->cs_info.rb = NULL;
    csptr->branch = 0;
    csptr->bottom = 0;
    csptr->block = 0;
    csptr->label = NULLCHAR;
    return( csptr );
}

void InitCSList(void)
{
    BlockNum = 0;
    CSHead = NewCSNode( 0 );
}

void CSPurge(void)
{
    if( CSHead != NULL ) {
        while( CSHead->typ != CS_EMPTY_LIST ) {
            DelCSNode();
        }
        FMemFree( CSHead );
        CSHead = NULL;
    }
}

itnode *GetBlockLabel(void)
{
    itnode      *citnode;

    citnode = CITNode;
    while(  citnode->link->opr != OPR_TRM ) {
        citnode = citnode->link;
    }
    if( citnode->opr != OPR_COL ) {
        citnode = citnode->link;
    }
    return( citnode );
}

void    AddCSNode( byte typ )
{
    csnode      *new_cs_node;
    itnode      *label;
    char        *label_ptr;
    int         label_len;

    if( typ == CS_REMOTEBLOCK ) {
        label = CITNode;
    } else {
        label = GetBlockLabel();
    }
    label_len = label->opnd_size;
    new_cs_node = NewCSNode( label_len );
    new_cs_node->link = CSHead;
    CSHead = new_cs_node;
    CSHead->typ = typ;
    CSHead->block = ++BlockNum;
    label_ptr = &CSHead->label;
    memcpy( label_ptr, label->opnd, label_len );
    label_ptr[ label_len ] = NULLCHAR;
}

void DelCSNode(void)
{
    csnode      *old;
    case_entry  *currcase;
    case_entry  *newcase;

    if( CSHead->typ != CS_EMPTY_LIST ) {
        old = CSHead;
        CSHead = CSHead->link;
        if( ( old->typ == CS_SELECT ) || ( old->typ == CS_CASE ) ||
            ( old->typ == CS_OTHERWISE ) || ( old->typ == CS_COMPUTED_GOTO ) ) {
            currcase = old->cs_info.cases;
            while( currcase != NULL ) {
                newcase = currcase->link;
                if( old->typ != CS_COMPUTED_GOTO ) {
                    // Consider:    CASE( 1, 2 )
                    // don't free the label more than once
                    if( !currcase->multi_case ) {
                        FreeLabel( currcase->label.g_label );
                    }
                }
                FMemFree( currcase );
                currcase = newcase;
            }
        } else if( old->typ == CS_DO ) {
            FMemFree( old->cs_info.do_parms );
        }
        FMemFree( old );
    }
}

void CSNoMore(void)
{
    if( RecNOpn() ) {
        AdvanceITPtr();
    }
    ReqEOS();
}

void ColonLabel(void)
{
    if( RecColon() ) {
        if( !RecName() ) {
            Error( SP_BAD_LABEL );
        }
        AdvanceITPtr();
    }
    ReqEOS();
}

void BlockLabel(void)
{
    if( RecNOpn() ) {
        AdvanceITPtr();
        ColonLabel();
    } else {
        Error( SX_NO_EOS_OR_COLON );
    }
}

void Match(void)
{
    if( CSHead->typ == CS_EMPTY_LIST ) {
        StmtErr( SP_INCOMPLETE );
    } else {
        StmtPtrErr( SP_UNMATCHED, StmtKeywords[ CSWords[ CSHead->typ ] ] );
    }
}

void CSExtn(void)
{
    StmtExtension( SP_STRUCTURED_EXT );
}

bool CheckCSList( byte typ )
{
    byte        head_typ;

    for(;;) {
        head_typ = CSHead->typ;
        if( head_typ == typ ) break;
        if( head_typ == CS_EMPTY_LIST ) break;
        Error( SP_UNFINISHED, StmtKeywords[ CSWords[ head_typ ] ] );
        DelCSNode();
    }
    return( head_typ == typ );
}

bool EmptyCSList(void)
{
    return( CSHead->typ == CS_EMPTY_LIST );
}

void CSCond( label_id label )
{
// Process a control structure condition (e.g. WHILE( cond )DO).

    if( RecNOpn() ) {
        AdvanceITPtr();
        if( ReqOpenParen() ) {
            GBoolExpr();
            GBrFalse( label );
            AdvanceITPtr();
            ReqCloseParen();
        }
    } else {
        Error( PC_NO_OPENPAREN );
    }
}
