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


#include "standard.h"
#include "coderep.h"
#include "score.h"
#include "dump.h"
#include "feprotos.h"

extern  void            DumpInt(int);
extern  void            DumpLong(signed_32);
extern  void            DumpRegName(hw_reg_set);
extern  void            DumpNL();
extern  void            DumpOperand(name *);

extern  int             ScoreCount;
extern  score_reg       **ScoreList;

extern  void    DumpSc( score *sc ) {
/***********************************/

    score_reg   *entry;
    score_list  *curr;
    score       *reg;
    int         i;

    i = 0;
    while( i < ScoreCount ) {
        entry = ScoreList[ i ];
        DumpInt( i );
        DumpLiteral( " reg " );
        DumpRegName( entry->reg );
        DumpLiteral( " low " );
        DumpInt( entry->low );
        DumpLiteral( " high " );
        DumpInt( entry->high );
        reg = sc->next_reg;
        while( reg != sc ) {
            DumpLiteral( "==" );
            DumpRegName( ScoreList[ reg->index ]->reg );
            reg = reg->next_reg;
        }
        DumpLiteral( " generation " );
        DumpInt( sc->generation );
        DumpNL();
        curr = *sc->list;
        while( curr != NULL ) {
            DumpScList( curr );
            curr = curr->next;
        }
        ++ sc;
        ++ i;
    }
}

static  void    DumpScList( score_list *curr ) {
/**********************************************/

    DumpLiteral( "    " );
    switch( curr->info.class ) {
    case N_CONSTANT:
        DumpLiteral( "&" );
        DumpLong( curr->info.offset );
        break;
    case N_TEMP:
        DumpLiteral( "t" );
        DumpInt( curr->info.symbol.t->v.id );
        DumpLiteral( " offset " );
        DumpLong( curr->info.offset );
        break;
    case N_MEMORY:
        DumpXString( FEName( curr->info.symbol.p ) );
        DumpLiteral( " offset " );
        DumpLong( curr->info.offset );
        break;
    case N_INDEXED:
        if( curr->info.base == NULL ) {
            ;
        } else if( curr->info.base->n.class == N_TEMP ) {
            DumpLiteral( "t" );
            DumpInt( curr->info.base->t.v.id );
            DumpLiteral( "+" );
        } else {
            DumpXString( FEName( curr->info.base->v.symbol ) );
            DumpLiteral( "+" );
        }
        DumpLong( curr->info.offset );
        DumpLiteral( "[" );
        DumpRegName( ScoreList[ curr->info.index_reg ]->reg );
        DumpLiteral( "]" );
        break;
    case N_INITIAL:
        DumpLiteral( "INITIAL(" );
        DumpLong( curr->info.offset );
        DumpLiteral( ")" );
        break;
    case N_VOLATILE:
        DumpLiteral( "VOLATILE - Oh No!" );
        break;
    case N_ADDRESS:
        DumpLiteral( "ADDRESS(" );
        DumpOperand(curr->info.symbol.p);
        DumpLiteral( ")" );
        break;
    }
    DumpNL();
}
