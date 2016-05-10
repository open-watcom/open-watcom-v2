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


#include "plusplus.h"
#include "cgfront.h"
#include "cgback.h"
// #include "memmgr.h"
#include "codegen.h"
#include "cgbackut.h"
#include "ring.h"
#include "toggle.h"
#include "vstk.h"
#include "initdefs.h"
#include "dumpapi.h"


struct blk_posn                 // BLK_POSN -- current block position
{   SCOPE scope;                // - defining scope
    SE* posn;                   // - position
    SE* temp_beg;               // - temp DTOR: starting entry
    SE* temp_end;               // - temp DTOR: ending entry
    SE* starting;               // - position at start
    DT_METHOD last_method;      // - last method used
    unsigned :0;                // - alignment
};

static VSTK_CTL stack_blk_posn;     // stack: current block position

#ifdef NDEBUG
    #define DbgDumpBlkPosn(a,b)
#else
    #include "pragdefn.h"


static void DbgDumpBlkPosn(     // DUMP A BLK_POSN
    BLK_POSN* bpos,             // - entry
    const char* text )          // - text
{
    if( PragDbgToggle.dump_blk_posn ) {
        printf( "%s BLK_POSN[%p] scope(%p) posn(%p)\n"
                "    last_method(%d) starting(%p) temp_beg(%p) temp_end(%p)\n"
              , text
              , bpos
              , bpos->scope
              , bpos->posn
              , bpos->last_method
              , bpos->starting
              , bpos->temp_beg
              , bpos->temp_end );
    }
    if( PragDbgToggle.dump_blk_posn || PragDbgToggle.dump_exec_ic ) {
        FN_CTL* fctl = FnCtlTop();
        printf( "** function dtm(%d) scope dtm(%d)\n"
              , fctl->func_dtor_method
              , fctl->dtor_method );
    }
}

void DumpBlkPosns(              // DUMP ALL BLK_POSN'S
    void )
{
    BLK_POSN* curr;             // - current position

    curr = VstkTop( &stack_blk_posn );
    if( NULL == curr ) {
        printf( "No BLK_POSN entries active\n" );
    } else {
        for( ; curr != NULL; curr = VstkNext( &stack_blk_posn, curr ) ) {
            DbgDumpBlkPosn( curr, "\n*" );
        }
    }
}
#endif


void BlkPosnPush(               // PUSH A NEW BLOCK POSITION
    SCOPE scope )               // - defining scope
{
    BLK_POSN* bpos;             // - current position
    FN_CTL* fctl;               // - top function
    DT_METHOD dtm;              // - function dtor method

    fctl = FnCtlTop();
    bpos = VstkPush( &stack_blk_posn );
    bpos->scope = scope;
    bpos->posn = FstabCurrPosn();
    bpos->temp_beg = NULL;
    bpos->temp_end = NULL;
    bpos->starting = FstabActualPosn();
    dtm = fctl->func_dtor_method;
    bpos->last_method = fctl->dtor_method;
    if( NULL == scope ) {
        fctl->dtor_method = DTM_DIRECT;
    } else {
        if( scope->u.s.cg_stab ) {
            fctl->dtor_method = dtm;
        } else {
            fctl->dtor_method = DtmDirect( dtm );
        }
    }
    DbgDumpBlkPosn( bpos, "Push" );
}


SE* BlkPosnUpdate(              // UPDATE POSITION IN CURRENT BLOCK
    SE* se )                    // - state entry for current position
{
    BLK_POSN* bpos;             // - current position

    bpos = VstkTop( &stack_blk_posn );
    bpos->posn = se;
    DbgDumpBlkPosn( bpos, "Update" );
    return se;
}


SE* BlkPosnCurr(                // GET CURRENT BLOCK POSITION
    void )
{
    BLK_POSN* bpos;             // - current BLK_POSN position
    SE* posn;                   // - current SE position

    bpos = VstkTop( &stack_blk_posn );
    if( NULL == bpos ) {
        posn = NULL;
    } else {
        posn = bpos->posn;
    }
    return posn;
}


SE* BlkPosnEnclosing(           // GET CURRENT POSITION OF ENCLOSING BLOCK
    void )
{
    BLK_POSN* bpos;             // - current position
    SE* posn;                   // - current SE position

    bpos = VstkTop( &stack_blk_posn );
    DbgVerify( bpos != NULL, "BlkPosnEnclosing -- no block" );
    bpos = VstkNext( &stack_blk_posn, bpos );
    if( NULL == bpos ) {
        posn = NULL;
    } else {
        posn = bpos->posn;
    }
    return posn;
}


SE* BlkPosnScope(               // GET BLOCK POSITION FOR A SCOPE
    SCOPE scope )               // - scope in question
{
    SE* posn;                   // - state entry for position
    BLK_POSN* bpos;             // - current position

    if( NULL == scope ) {
        posn = NULL;
    } else {
        for( bpos = VstkTop( &stack_blk_posn )
           ;
           ; bpos = VstkNext( &stack_blk_posn, bpos ) ) {
            DbgVerify( bpos != NULL, "BlkPosnScope -- no scope stacked" );
            if( scope == bpos->scope ) break;
        }
        posn = bpos->posn;
    }
    return posn;
}


void BlkPosnPop(                // POP A BLOCK POSITION
    void )
{
    BLK_POSN* bpos;             // - popped element
    FN_CTL* fctl;               // - top function
    SE* next_posn;              // - next position

    next_posn = BlkPosnEnclosing();
    next_posn = FstabSetSvSe( next_posn );
//  FstabMarkedPosnSet( next_posn );
    bpos = VstkPop( &stack_blk_posn );
    fctl = FnCtlTop();
    fctl->dtor_method = bpos->last_method;
    DbgDumpBlkPosn( bpos, "Popped" );
    FstabPrune( bpos->starting );
}


void BlkPosnTrash(              // TRASH TOP BLOCK POSITION
    void )
{
#ifndef NDEBUG
    BLK_POSN *bpos;             // - popped element

    bpos = VstkPop( &stack_blk_posn );
    DbgDumpBlkPosn( bpos, "Trashed" );
#else
    VstkPop( &stack_blk_posn );
#endif
}


SE* BlkPosnTempBegSet(          // SET STARTING POS'N FOR TEMP DTOR'ING
    SE* se )                    // - starting position
{
    BLK_POSN* bpos;             // - current position

    bpos = VstkTop( &stack_blk_posn );
    DbgVerify( bpos != NULL, "BlkPosnTempBegSet -- no block" );
    bpos->temp_beg = se;
    DbgDumpBlkPosn( bpos, "BegSet" );
    return se;
}


SE* BlkPosnTempBeg(             // GET STARTING POS'N FOR TEMP DTOR'ING
    void )
{
    BLK_POSN* bpos;             // - current position

    bpos = VstkTop( &stack_blk_posn );
    DbgVerify( bpos != NULL, "BlkPosnTempBeg -- no block" );
    return bpos->temp_beg;
}


SE* BlkPosnTempEndSet(          // SET ENDING POS'N FOR TEMP DTOR'ING
    SE* se )                    // - ending position
{
    BLK_POSN* bpos;             // - current position

    bpos = VstkTop( &stack_blk_posn );
    DbgVerify( bpos != NULL, "BlkPosnTempEndSet -- no block" );
    bpos->temp_end = se;
    DbgDumpBlkPosn( bpos, "EndSet" );
    return se;
}


SE* BlkPosnTempEnd(             // GET ENDING POS'N FOR TEMP DTOR'ING
    void )
{
    BLK_POSN* bpos;             // - current position

    bpos = VstkTop( &stack_blk_posn );
    DbgVerify( bpos != NULL, "BlkPosnTempEnd -- no block" );
    return bpos->temp_end;
}


bool BlkPosnUseStab(            // TEST IF REALLY USING STATE TABLE IN SCOPE
    void )
{
    bool rc;                    // - true ==> gen state table code
    BLK_POSN* bpos;             // - current position

    bpos = VstkTop( &stack_blk_posn );
    DbgVerify( bpos != NULL, "BlkPosnStabGen -- no block" );
    if( NULL == bpos->scope ) {
        rc = false;
    } else {
        rc = bpos->scope->u.s.cg_stab;
    }
    return rc;
}


static void blkPosnInit(        // INITIALIZATION FOR BLK_POSN MODULE
    INITFINI* defn )            // - definition
{
    defn = defn;
    VstkOpen( &stack_blk_posn, sizeof( BLK_POSN ), 32 );
}


static void blkPosnFini(        // COMPLETION FOR BLK_POSN MODULE
    INITFINI* defn )            // - definition
{
    defn = defn;
    VstkClose( &stack_blk_posn );
}


INITDEFN( blk_posn, blkPosnInit, blkPosnFini )
