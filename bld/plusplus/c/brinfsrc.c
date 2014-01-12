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
#include "preproc.h"
#include "carve.h"
#include "vstk.h"
#include "ring.h"
#include "brinfoim.h"

#ifndef NDEBUG
#   include "dbg.h"
#   include "pragdefn.h"
#endif

typedef struct pp_ins PP_INS;

struct pp_ins                   // PP_INS -- preprocessor instr. in queue
{   PP_INS* next;               // - next in queue
    TOKEN_LOCN locn;            // - location
    void const * parm;          // - parm for instruction
    CGINTEROP opcode;           // - opcode
};

static PP_INS* ins_queue;       // queued instructions (a ring)
static VSTK_CTL open_src;       // stack of locations in open source's
static carve_t carvePpIns;      // carver for instructions
static TOKEN_LOCN cursor;       // cursor for queuing


static void writeIns            // WRITE AN INSTRUCTION
    ( CGINTEROP opcode          // - opcode
    , void const * parm         // - parm for instruction
    , TOKEN_LOCN const * locn ) // - location for instruction
{
    TOKEN_LOCN* curr_locn;      // - location
    SRCDEP const* sd;           // - dependency for open

    switch( opcode ) {
      case IC_BR_SRC_BEG :
        curr_locn = VstkPush( &open_src );
        TokenLocnAssign( *curr_locn, cursor );
        sd = parm;
        cursor.src_file = BrinfDepSrcfile( sd );
        cursor.line = 0;
        cursor.column = 0;
        BrinfWriteIns( opcode, parm );
        break;
      case IC_BR_SRC_END :
        locn = VstkPop( &open_src );
        if( NULL == locn ) {
            cursor.src_file = NULL;
        } else {
            TokenLocnAssign( cursor, *locn );
        }
        BrinfWriteIns( opcode, parm );
        break;
      case IC_BR_REF_MACRO :
        TokenLocnAssign( cursor, *locn );
        BrinfIcReference( opcode, parm, locn );
        break;
      default :
        TokenLocnAssign( cursor, *locn );
        BrinfWriteIns( opcode, parm );
        break;
    }
}


static void queue               // PLACE INSTRUCTION ON QUEUE
    ( CGINTEROP opcode          // - opcode
    , void const * parm         // - parm for instruction
    , TOKEN_LOCN const * locn ) // - location for instruction
{
    if( cursor.src_file == locn->src_file
     && NULL != cursor.src_file
     && opcode != IC_BR_SRC_END ) {
        writeIns( opcode, parm, locn );
    } else {
        PP_INS* ins = RingCarveAlloc( carvePpIns, &ins_queue );
        ins->opcode = opcode;
        ins->parm = parm;
        TokenLocnAssign( ins->locn, *locn );
        IfDbgToggle( browse ) {
            DbgStmt( printf( "queued %s %x\n   ", DbgIcOpcode( opcode ), parm ) );
            DbgStmt( DbgDumpTokenLocn( locn ) );
            DbgStmt( printf( "\n" ) );
        }
    }
}


static void deQueue             // TAKE (FIFO) INSTRUCTION FROM QUEUE
    ( void )
{
    PP_INS* ins;                // - queued instruction

    ins = ins_queue->next;
    DbgVerify( NULL != ins, "Empty instruction queue" );
    RingPrune( &ins_queue, ins );
    IfDbgToggle( browse ) {
        DbgStmt( printf( "dequeued %s %x\n   "
                       , DbgIcOpcode( ins->opcode )
                       , ins->parm ) );
        DbgStmt( DbgDumpTokenLocn( &ins->locn ) );
        DbgStmt( printf( "\n" ) );
    }
    writeIns( ins->opcode, ins->parm, &ins->locn );
    CarveFree( carvePpIns, ins );
}


void BrinfSrcSync               // SYNCHONIZE THE OPEN/CLOSE DIRECTIVES
    ( TOKEN_LOCN const* locn )  // - synchronizing location
{
    SRCFILE tgt = locn->src_file;
    for( ; ; ) {
        PP_INS* ins = ins_queue;
        if( NULL == ins ) break;
        if( cursor.src_file == tgt ) {
            if( ins->locn.src_file != tgt ) break;
            if( ins->opcode == IC_BR_SRC_END ) break;
        }
        deQueue();
    }
}



void BrinfSrcInc                // START OF SRC-INCLUDE
    ( void* short_name          // - short name
    , TOKEN_LOCN const* locn )  // - location
{
    queue( IC_BR_INC_SRC, short_name, locn );
}


void BrinfSrcBeg                // START OF SOURCE (DEPENDENCY)
    ( SRCFILE srcfile )         // - source file
{
    TOKEN_LOCN locn;

    locn.src_file = srcfile;
    locn.line = 0;
    locn.column = 0;
    queue( IC_BR_SRC_BEG, BrinfDepSrcBeg( srcfile ), &locn );
}


void BrinfSrcEndFile            // END OF A SOURCE FILE
    ( SRCFILE srcfile )         // - file being closed
{
    TOKEN_LOCN end_loc;         // - maximum TOKEN_LOCN for file

    DbgVerify( NULL != srcfile, "SRCFILE required" );
    end_loc.src_file = srcfile;
    end_loc.line = 0xFFFFFFFF;
    end_loc.column = 0xFFFFFFFF;
    queue( IC_BR_SRC_END, NULL, &end_loc );
}


void BrinfSrcMacDecl            // MACRO DECLARATION
    ( MACVALUE const * defn )   // - definition value
{
    queue( IC_BR_DCL_MACRO, defn, BrinfMacValueLocn( defn ) );
}


void BrinfSrcMacReference       // MACRO REFERENCE
    ( MACVALUE const * val )    // - reference value
{
    TOKEN_LOCN temporary;       // - current token location

    SrcFileGetTokenLocn( &temporary );
    queue( IC_BR_REF_MACRO, val, &temporary );
}


void BrinfSrcInit               // BrinfSrc INITIALIZATION
    ( void )
{
    carvePpIns = CarveCreate( sizeof( PP_INS ), 32 );
    VstkOpen( &open_src, sizeof( TOKEN_LOCN ), 16 );
}


static void brinfSrcDestroy     // DESTROY ALL DATA
    ( void )
{
    CarveDestroy( carvePpIns );
    VstkClose( &open_src );
    ins_queue = NULL;
    cursor.src_file = NULL;
    cursor.line = 0;
    cursor.column = 0;
}


void BrinfSrcFini               // BrinfSrc COMPLETION
    ( void )
{
    for( ; ins_queue != NULL; ) {
        deQueue();
    }
    brinfSrcDestroy();
}


void BrinfSrcRestart            // BrinfSrc RESTART FOR PCH READ
    ( void )
{
    brinfSrcDestroy();
    BrinfSrcInit();
}
