/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Dump scores.
*
****************************************************************************/


#include "_cgstd.h"
#include "coderep.h"
#include "score.h"
#include "regset.h"
#include "confldef.h"
#include "conflict.h"
#include "dumpio.h"
#include "dumpconf.h"
#include "dumpins.h"
#include "dumpsc.h"
#include "feprotos.h"


static  void    DumpScList( score_list *curr )
/********************************************/
{
    DumpLiteral( "    " );
    switch( curr->info.class ) {
    case SC_N_CONSTANT:
        DumpChar( '&' );
        DumpLong( curr->info.offset );
        break;
    case SC_N_TEMP:
        DumpChar( 't' );
        DumpInt( curr->info.symbol.t->v.id );
        DumpLiteral( " offset " );
        DumpLong( curr->info.offset );
        break;
    case SC_N_MEMORY:
        DumpXString( FEName( curr->info.symbol.p ) );
        DumpLiteral( " offset " );
        DumpLong( curr->info.offset );
        break;
    case SC_N_INDEXED:
        if( curr->info.base == NULL ) {
            ;
        } else if( curr->info.base->n.class == N_TEMP ) {
            DumpChar( 't' );
            DumpInt( curr->info.base->t.v.id );
            DumpChar( '+' );
        } else {
            DumpXString( FEName( curr->info.base->v.symbol ) );
            DumpChar( '+' );
        }
        DumpLong( curr->info.offset );
        DumpChar( '[' );
        DumpRegName( ScoreList[curr->info.index_reg]->reg );
        DumpChar( ']' );
        break;
    case SC_N_INITIAL:
        DumpLiteral( "INITIAL(" );
        DumpLong( curr->info.offset );
        DumpChar( ')' );
        break;
    case SC_N_VOLATILE:
        DumpLiteral( "VOLATILE - Oh No!" );
        break;
    case SC_N_ADDRESS:
        DumpLiteral( "ADDRESS(" );
        DumpOperand(curr->info.symbol.p);
        DumpChar( ')' );
        break;
    }
    DumpNL();
}


void    DumpSc( score *scoreboard )
/*********************************/
{
    score_reg   *entry;
    score_list  *curr;
    score       *reg;
    int         i;

    for( i = 0; i < ScoreCount; ++i ) {
        entry = ScoreList[i];
        DumpInt( i );
        DumpLiteral( " reg " );
        DumpRegName( entry->reg );
        DumpLiteral( " low " );
        DumpInt( entry->low );
        DumpLiteral( " high " );
        DumpInt( entry->high );
        for( reg = scoreboard[i].next_reg; reg != &scoreboard[i]; reg = reg->next_reg ) {
            DumpLiteral( "==" );
            DumpRegName( ScoreList[reg->index]->reg );
        }
        DumpLiteral( " generation " );
        DumpInt( scoreboard[i].generation );
        DumpNL();
        for( curr = *scoreboard[i].list; curr != NULL; curr = curr->next ) {
            DumpScList( curr );
        }
    }
}
