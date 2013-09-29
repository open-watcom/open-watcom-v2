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
* Description:  Dump invariants.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "indvars.h"
#include "dump.h"
#include "data.h"

extern  induction       *IndVarList;

extern  void            DumpOperand(name*);
extern  void            DumpInt(int);
extern  void            DumpLong(signed_32);
extern  void            DumpBlkId(block*);
extern  void            DumpPtr(pointer);
extern  void            DumpIns(instruction*);
extern  void            DumpNL();

extern  void    DumpIV( induction *var ) {
/****************************************/

    induction   *alias;
    invariant   *invar;

    DumpPtr( var );
    DumpLiteral( "  " );
    DumpOperand( var->name );
    DumpLiteral( " = ( " );
    if( _IsV( var, IV_BASIC ) ) {
        DumpLiteral( "**basic**" );
    } else {
        if( var->ivtimes != NULL ) {
            DumpOperand( var->ivtimes );
            DumpLiteral( " * " );
        }
        if( var->times != 1 ) {
            DumpLong( var->times );
            DumpLiteral( " * " );
        }
        DumpOperand( var->basic->name );
    }
    DumpLiteral( " )" );
    if( var->plus != 0 ) {
        DumpLiteral( " + ( " );
        DumpLong( var->plus );
        DumpLiteral( " )" );
    }
    if( var->ivtimes != NULL && var->plus2 != 0 ) {
        DumpLiteral( " + ( " );
        DumpOperand( var->ivtimes );
        DumpLiteral( " * " );
        DumpLong( var->plus2 );
        DumpLiteral( " )" );
    }
    invar = var->invar;
    while( invar != NULL ) {
        DumpLiteral( " + ( " );
        if( var->lasttimes >= invar->id ) {
            DumpOperand( var->ivtimes );
            DumpLiteral( " * " );
        }
        if( invar->times != 1 ) {
            DumpLong( invar->times );
            DumpLiteral( " * " );
        }
        DumpOperand( invar->name );
        DumpLiteral( " )" );
        invar = invar->next;
    }
    DumpNL();
    DumpLiteral( "        ins=" );
    DumpPtr( var->ins );
    DumpLiteral( " prev=" );
    DumpPtr( var->prev );
    DumpLiteral( " basic=" );
    DumpOperand( var->basic->name );
    if( _IsV( var, IV_ALIAS ) ) {
        DumpLiteral( " alias=" );
        alias = var->alias;
        while( _IsV( alias, IV_ALIAS ) ) {
            alias = alias->alias;
        }
        DumpPtr( alias );
    }
    DumpNL();
    DumpLiteral( "        Uses: " );
    DumpInt( var->use_count );
    DumpLiteral( " " );
    if( _IsV( var, IV_BASIC ) ) {
        DumpLiteral( " Basic" );
    }
    if( _IsV( var, IV_DEAD ) ) {
        DumpLiteral( " Dead" );
    }
    if( _IsV( var, IV_SURVIVED ) ) {
        DumpLiteral( " Survived" );
    }
    if( _IsV( var, IV_TOP ) ) {
        DumpLiteral( " Top" );
    }
    if( _IsV( var, IV_INTRODUCED ) ) {
        DumpLiteral( " Introduced" );
    }
    if( _IsV( var, IV_ALIAS ) ) {
        DumpLiteral( " Alias" );
    }
    if( _IsV( var, IV_INDEXED ) ) {
        DumpLiteral( " Indexed" );
    }
    if( _IsV( var, IV_USED ) ) {
        DumpLiteral( " Used" );
    }
    DumpNL();
}

extern  void    DumpIVList() {
/****************************/

    induction   *var;

    var = IndVarList;
    while( var != NULL ) {
        DumpIV( var );
        var = var->next;
    }
}

static  void    DumpInv( name *name ) {
/***************************************/

    while( name != NULL ) {
        if( name->v.block_usage & VU_INVARIANT ) {
            DumpOperand( name );
            DumpLiteral( " " );
        }
        name = name->n.next_name;
    }
    DumpNL();
}

extern  void    DumpInvariants() {
/********************************/

    DumpLiteral( "Temps: " );
    DumpInv( Names[ N_TEMP ] );
    DumpLiteral( "Memory: " );
    DumpInv( Names[ N_MEMORY ] );
}

extern  void    DumpCurrLoop() {
/******************************/

    block               *blk;
    block               *header;
    interval_depth      depth;

    DumpLiteral( "The Loop Is:" );
    DumpNL();
    DumpLiteral( "============" );
    DumpNL();
    blk = HeadBlock;
    depth = MAX_INTERVAL_DEPTH;
    header = NULL;
    while( blk != NULL ) {
        if( blk->class & IN_LOOP ) {
            if( blk->depth < depth ) {
                header = blk;
                depth = header->depth;
            }
        }
        blk = blk->next_block;
    }
    blk = HeadBlock;
    while( blk != NULL ) {
        DumpBlkId( blk );
        if( blk->class & IN_LOOP ) {
            if( blk == header ) {
                DumpLiteral( " - Header" );
            } else {
                DumpLiteral( " - In for(;;) {" );
            }
            if( blk->class & LOOP_EXIT ) {
                DumpLiteral( " Exits" );
            }
        }
        DumpNL();
        blk = blk->next_block;
    }
}
