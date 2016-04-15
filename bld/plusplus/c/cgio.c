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
#include "cgio.h"
#include "memmgr.h"
#include "ring.h"
#include "cgiobuff.h"
#include "carve.h"
#include "initdefs.h"
#include "stringl.h"
#include "conpool.h"
#include "pcheader.h"
#include "ioptypes.h"
#include "stats.h"
#include "module.h"
#include "dumpapi.h"


static CGFILE *cg_file_ring;    // ring of virtual files (live)
static CGFILE *cg_thunk_ring;   // ring of virtual thunks (live)
static CGFILE *cg_file_removed; // ring of virtual files (removed)
static carve_t carveCGFILE;
static carve_t carveCGFILE_GEN;

ExtraRptCtr( cgio_write_ins );
ExtraRptCtr( cgio_write_nul );
ExtraRptCtr( cgio_files );
ExtraRptCtr( cgio_locates_front );      // front end
ExtraRptCtr( cgio_comps_front );
ExtraRptCtr( cgio_locates_callgr );     // call graph
ExtraRptCtr( cgio_comps_callgr );
ExtraRptCtr( cgio_locates );            // back end
ExtraRptCtr( cgio_comps );
ExtraRptCtr( cgio_locates_thunk );


#ifndef NDEBUG

    #include "dbg.h"
    #include "pragdefn.h"

    static void _dump( CGFILE* cgfile, const char* prefix )
    {
        if( PragDbgToggle.callgraph ) {
            VBUF vbuf;
            printf( "Cgio: %p %s - %s\n"
                  , cgfile
                  , prefix
                  , DbgSymNameFull( cgfile->symbol, &vbuf ) );
            VbufFree( &vbuf );
        }
    }

    static void _dumpRing( CGFILE* ring, char const * msg )
    {
        if( NULL != ring ) {
            CGFILE* curr;
            VBUF vbuf;
            printf( "\n%s\n", msg );
            VbufInit( &vbuf );
            RingIterBeg( ring, curr ) {
                printf( "\nCGFILE[%p] %s\n"
                        "     first[%lx] offset[%x] buffer[%p] cursor[%p]"
                        " cond_flags(%d)\n"
                        "     delayed(%d) refed(%d) stgen(%d)"
                        " oe_inl(%d) state_table(%d) once_inl(%d)\n"
                        "     done(%d) thunk(%d) thunk_gen(%d) not_inline(%d)"
                        " calls_inline(%d) can_throw(%d)\n"
                        "     stab_gen(%d) ctor_test(%d) write_to_pch(%d)\n"
                      , curr
                      , DbgSymNameFull( curr->symbol, &vbuf )
                      , curr->first
                      , curr->offset
                      , curr->buffer
                      , curr->cursor
                      , curr->cond_flags
                      , curr->u.s.delayed
                      , curr->u.s.refed
                      , curr->u.s.stgen
                      , curr->u.s.oe_inl
                      , curr->u.s.state_table
                      , curr->u.s.once_inl
                      , curr->u.s.done
                      , curr->u.s.thunk
                      , curr->u.s.thunk_gen
                      , curr->u.s.not_inline
                      , curr->u.s.calls_inline
                      , curr->u.s.can_throw
                      , curr->u.s.stab_gen
                      , curr->u.s.ctor_test
                      , curr->u.s.write_to_pch
                      );
            } RingIterEnd( curr )
            VbufFree( &vbuf );
        }
    }

    void CgFileDump( void )
    {
        _dumpRing( cg_file_ring, "Live Virtual Files" );
        _dumpRing( cg_thunk_ring, "Live Virtual Thunks" );
        _dumpRing( cg_file_removed, "Removed Virtual Files" );
    }

#else

    #define _dump(a,b)

#endif


static void cgioInit(           // INITIALIZE FOR CG-IO
    INITFINI* defn )
{
    defn = defn;
    cg_file_ring = NULL;
    cg_thunk_ring = NULL;
    cg_file_removed = NULL;
    CgioBuffInit();
    carveCGFILE = CarveCreate( sizeof( CGFILE ), 64 );
    carveCGFILE_GEN = CarveCreate( sizeof( CGFILE_GEN ), 32 );
    ExtraRptRegisterCtr( &cgio_files, "intermediate code files" );
    ExtraRptRegisterCtr( &cgio_locates_front
                       , "IC file locates (front-end)" );
    ExtraRptRegisterCtr( &cgio_comps_front
                       , "IC file locate comparisons (front-end)" );
    ExtraRptRegisterAvg( &cgio_comps_front
                       , &cgio_locates_front
                       , "average # comparisons per IC file locate (front-end)" );
    ExtraRptRegisterCtr( &cgio_locates_callgr
                       , "IC file locates (call-graph)" );
    ExtraRptRegisterCtr( &cgio_comps_callgr
                       , "IC file locate comparisons (call-graph)" );
    ExtraRptRegisterAvg( &cgio_comps_callgr
                       , &cgio_locates_callgr
                       , "average # comparisons per IC file locate (call-graph)" );
    ExtraRptRegisterCtr( &cgio_locates
                       , "IC file locates (back-end)" );
    ExtraRptRegisterCtr( &cgio_comps
                       , "IC file locate comparisons (back-end)" );
    ExtraRptRegisterAvg( &cgio_comps
                       , &cgio_locates
                       , "average # comparisons per IC file locate (back-end)" );
    ExtraRptRegisterCtr( &cgio_locates_thunk
                       , "IC file locates (back-end) thunks" );
    ExtraRptRegisterCtr( &cgio_write_ins, "# ICs written" );
    ExtraRptRegisterCtr( &cgio_write_nul, "# ICs written with NUL operands" );
}


static void cgioFini(           // FINALIZE FOR CG-IO
    INITFINI* defn )
{
    defn = defn;
    CgioBuffFini();
    CarveDestroy( carveCGFILE );
    CarveDestroy( carveCGFILE_GEN );
}


INITDEFN( cgio, cgioInit, cgioFini )


static CGFILE* lookupFile(      // DO A FILE LOOKUP FOR A SYMBOL, RING
    SYMBOL sym,                 // - symbol
    CGFILE* ring )              // - ring of symbols
{
    CGFILE *curr;               // - current CGFILE

    RingIterBeg( ring, curr ) {
        ExtraRptIncrementCtr( cgio_comps );
        if( sym == curr->symbol ) {
            return curr;
        }
    } RingIterEnd( curr );
    return NULL;
}

#ifndef NDEBUG


void DbgCgioEndFront(           // CALLED AT END OF FRONT END
    void )
{
    cgio_comps_front = cgio_comps;
    cgio_locates_front = cgio_locates;
    cgio_comps = 0;
    cgio_locates = 0;
}


void DbgCgioEndCallGr(          // CALLED AT END OF CALL-GRAPH PROCESSING
    void )
{
    cgio_comps_callgr = cgio_comps;
    cgio_locates_callgr = cgio_locates;
    cgio_comps = 0;
    cgio_locates = 0;
}


#endif


static CGFILE *initCGFILE(      // INITIALIZE A CG VIRTUAL FILE
    CGFILE *file_element,       // - allocated file element
    SYMBOL symbol )             // - controlling symbol-table entry
{
    CGIOBUFF *buff_element;     // - buffer element
    CGFILE_GEN *gen;            // - data while being generated

    buff_element = CgioBuffWrOpen();
    file_element->symbol = symbol;
    file_element->buffer = buff_element;
    file_element->offset = buff_element->free_offset;
    file_element->cursor = (CGINTER*) (buff_element->data + buff_element->free_offset);
    file_element->first = buff_element->disk_addr;
    file_element->u.s.delayed = false;
    file_element->u.s.refed = false;
    file_element->u.s.stgen = false;
    file_element->u.s.oe_inl = false;
    file_element->u.s.once_inl = false;
    file_element->u.s.done = false;
    file_element->u.s.thunk = false;
    file_element->u.s.thunk_gen = false;
    file_element->u.s.not_inline = false;
    file_element->u.s.calls_inline = false;
    file_element->u.s.can_throw = false;
    file_element->u.s.stab_gen = false;
    file_element->u.s.ctor_test = false;
    file_element->u.s.write_to_pch = false;
    file_element->u.s.state_table = 0;
    file_element->defined.src_file = NULL;
    file_element->defined.line = 0;
    file_element->defined.column = 0;
    file_element->cond_flags = 0;
    file_element->u.s.opt_retn_val = false;
    file_element->u.s.opt_retn_ref = false;
    file_element->opt_retn = NULL;
    gen = CarveAlloc( carveCGFILE_GEN );
    file_element->gen = gen;
    gen->cs_label = 0;
    gen->cs_allocated = 0;
    gen->goto_label = 0;
    gen->emit_init_beg = false;
    gen->emit_init_end = false;
    gen->emit_line_no.src_file = NULL;
    gen->curr_type = NULL;
    gen->emit_type = NULL;
    gen->init_sym = NULL;
    return( file_element );
}

CGFILE *CgioCreateFile(         // CREATE A CG VIRTUAL FILE
    SYMBOL symbol )             // - controlling symbol-table entry
{
    CGFILE *new_cgfile;

    ExtraRptIncrementCtr( cgio_files );
    new_cgfile = RingCarveAlloc( carveCGFILE, &cg_file_ring );
    return( initCGFILE( new_cgfile, symbol ) );
}


void CgioWriteIC(               // WRITE IC RECORD TO VIRTUAL FILE
    CGFILE *ctl,                // - control for the file
    CGINTER *ins )              // - instruction
{
#ifndef NDEBUG
    ExtraRptIncrementCtr( cgio_write_ins );
    if( ICOpTypes[ ins->opcode ] == ICOT_NUL ) {
        ExtraRptIncrementCtr( cgio_write_nul );
    }
#endif
    ctl->buffer = CgioBuffWriteIC( ctl->buffer, ins );
}


static void freeGenData(        // FREE GENERATION DATA
    CGFILE *ctl )               // - control for file
{
    CarveFree( carveCGFILE_GEN, ctl->gen );
    ctl->gen = NULL;
}


static bool saveGenData(        // TEST IF GENERATION DATA REQ'D AFTER CLOSE
    CGFILE *ctl )               // - control for file
{
    SYMBOL func;                // - symbol for file

    func = ctl->symbol;
    return func == NULL
        || func == BRINF_SYMBOL
        || func->id == SC_DEFAULT;
}


void CgioCloseOutputFile(       // CLOSE VIRTUAL FILE AFTER OUTPUT PHASE
    CGFILE *ctl )               // - control for file
{
    CgioBuffWrClose( ctl->buffer );
    ctl->u.s.done = true;
    ctl->buffer = NULL;
    if( ! saveGenData( ctl ) ) {
        freeGenData( ctl );
    }
}


void CgioOpenInput(             // OPEN VIRTUAL FILE FOR INPUT
    CGFILE *ctl )               // - control for file
{
    ctl->buffer = CgioBuffRdOpen( ctl->first );
    ctl->cursor = (CGINTER*) (( ctl->buffer->data + ctl->offset ) - sizeof( CGINTER ));
}


CGINTER *CgioSeek(              // SEEK TO POSITION IN VIRTUAL FILE
    CGFILE *ctl,                // - control for the file
    CGFILE_INS *posn )          // - position to seek to
{
    DbgAssert( ctl->cursor != NULL );
    ctl->buffer = CgioBuffSeek( ctl->buffer, posn, &ctl->cursor );
    return ctl->cursor;
}


CGINTER *CgioReadIC(            // READ IC RECORD (LOCATE MODE)
    CGFILE *ctl )               // - control for the file
{
    DbgAssert( ctl->cursor != NULL );
    ctl->buffer = CgioBuffReadIC( ctl->buffer, &ctl->cursor );
    return ctl->cursor;
}


CGINTER *CgioReadICUntilOpcode( // READ IC RECORD UNTIL OPCODE IS FOUND
    CGFILE *ctl,                // - control for the file
    CGINTEROP opcode )          // - opcode to find
{
    DbgAssert( ctl->cursor != NULL );
    ctl->buffer = CgioBuffReadICUntilOpcode( ctl->buffer, &ctl->cursor, opcode );
    return ctl->cursor;
}


CGINTER *CgioReadICMask(        // READ IC RECORD UNTIL OPCODE IN SET IS FOUND
    CGFILE *ctl,                // - control for the file
    unsigned mask )             // - control mask for opcodes
{
    DbgAssert( ctl->cursor != NULL );
    ctl->buffer = CgioBuffReadICMask( ctl->buffer, &ctl->cursor, mask );
    return ctl->cursor;
}


CGINTER *CgioReadICMaskCount(   // READ IC RECORD UNTIL OPCODE IN SET IS FOUND
    CGFILE *ctl,                // - control for the file
    unsigned mask,              // - control mask for opcodes to return
    unsigned count_mask,        // - control mask for opcodes to count
    unsigned *count )           // - counter to update
{
    DbgAssert( ctl->cursor != NULL );
    ctl->buffer = CgioBuffReadICMaskCount( ctl->buffer, &ctl->cursor, mask, count_mask, count );
    return ctl->cursor;
}


void CgioCloseInputFile(        // CLOSE VIRTUAL FILE AFTER INPUT PHASE
    CGFILE *ctl )               // - control for file
{
    CgioBuffRdClose( ctl->buffer );
    ctl->buffer = NULL;
}


static bool changeRing(         // CHANGE RING FOR A CGFILE IN cg_file_ring
    CGFILE** a_new,             // - addr[ new ring header ]
    CGFILE* element,            // - element
    CGFILE** a_old )            // - addr[ old ring header ]
{
    bool retn;                  // - true ==> changed the ring
    CGFILE* prev;               // - preceding element

    prev = RingPred( *a_old, element );
    if( prev == NULL ) {
        retn = false;
    } else {
        RingPruneWithPrev( a_old, element, prev );
        RingAppend( a_new, element );
        retn = true;
    }
    return retn;
}


void CgioFreeFile(              // FREE A VIRTUAL FILE
    CGFILE *ctl )               // - control for file
{
    _dump( ctl, "free file" );
    if( saveGenData( ctl ) ) {
        freeGenData( ctl );
    }
#if 0
    RingPrune( &cg_file_ring, ctl );
    CarveFree( carveCGFILE, ctl );
#else
    if( ! changeRing( &cg_file_removed, ctl, &cg_file_ring ) ) {
  #ifndef NDEBUG
        int changed =
  #endif
        changeRing( &cg_file_removed, ctl, &cg_thunk_ring );
        DbgVerify( changed, "CgioFreeFile -- could not free" );
    }
#endif
}


void CgioWalkFiles(             // WALK THRU FILES
    void (*rtn)                 // - traversal routine
        ( CGFILE *curr ) )      // - - passed current file
{
    CGFILE *curr;

    RingIterBegSafe( cg_file_ring, curr ) {
        (*rtn)( curr );
    } RingIterEndSafe( curr )
}


void CgioThunkStash(            // STASH AWAY A THUNK TO BE GEN'ED
    CGFILE* thunk )             // - that thunk
{
#ifndef NDEBUG
    int changed =
#endif
    changeRing( &cg_thunk_ring, thunk, &cg_file_ring );
    DbgVerify( changed, "CgioThunkStash -- could not stash" );
    _dump( thunk, "stash thunk" );
}


void CgioThunkAddrTaken(        // INDICATE ADDR TAKEN OF THUNK BY GEN'ED CODE
    SYMBOL thunk )              // - a thunk
{
    CGFILE* thunk_file;         // - file for the thunk

    ExtraRptIncrementCtr( cgio_locates_thunk );
    thunk_file = CgioLocateFile( thunk );
    if( thunk_file == NULL ) {
        thunk_file = lookupFile( thunk, cg_thunk_ring );
    }
    if( thunk_file != NULL
     && thunk_file->u.s.thunk ) {
        _dump( thunk_file, "thunk addr taken" );
        CgioThunkMarkGen( thunk_file );
    }
}


void CgioThunkMarkGen(          // MARK THUNK TO BE GENERATED
    CGFILE* thunk )             // - CGFILE for thunk symbol
{
    _dump( thunk, "thunk marked to generate" );
    DbgVerify( thunk != NULL, "CgioThunkMarkGen -- no thunk" );
    thunk->u.s.refed = true;
}


void CgioWalkThunks(            // WALK THRU STASHED THUNKS
    void (*rtn)                 // - traversal routine
        ( CGFILE *curr ) )      // - - passed current file
{
    CGFILE *curr;

    RingIterBegSafe( cg_thunk_ring, curr ) {
        (*rtn)( curr );
    } RingIterEndSafe( curr )
}


CGFILE *CgioLocateFile(         // LOCATE LIVE FILE FOR A SYMBOL
    SYMBOL sym )                // - symbol for file
{
    ExtraRptIncrementCtr( cgio_locates );
    return lookupFile( sym, cg_file_ring );
}


CGFILE* CgioLocateAnyFile(      // LOCATE LIVE OR REMOVED FILE FOR A SYMBOL
    SYMBOL sym )                // - the symbol
{
    CGFILE* retn;               // - the CGFILE for the symbol

    retn = CgioLocateFile( sym );
    if( retn == NULL ) {
        retn = lookupFile( sym, cg_thunk_ring );
        if( retn == NULL ) {
            retn = lookupFile( sym, cg_file_removed );
        }
    }
    return retn;
}

#define doWriteOpNUL    NULL
#define doWriteOpBIN    NULL
#define doWriteOpCON    ConstantPoolGetIndex
#define doWriteOpSYM    SymbolGetIndex
#define doWriteOpSTR    StringGetIndex
#define doWriteOpTYP    TypeGetIndex
#define doWriteOpSCP    ScopeGetIndex
#define doWriteOpSRC    SrcFileGetIndex

static CGIRELOCFN *relocWriteOperand[] = {
#define ITYPE( typ )    ((CGIRELOCFN*) doWriteOp##typ)
#include "itypes.h"
};


CGVALUE CgioGetIndex( CGINTEROP opcode, CGVALUE value )
/*****************************************************/
{
    ic_op_type op_class;
    CGIRELOCFN *reloc;

    op_class = ICOpTypes[ opcode ];
    reloc = relocWriteOperand[ op_class ];
    if( reloc != NULL ) {
        value.pvalue = reloc( value.pvalue );
    }
    return( value );
}

CGFILE *CGFileGetIndex( CGFILE *e )
{
    return( CarveGetIndex( carveCGFILE, e ) );
}

CGFILE *CGFileMapIndex( CGFILE *e )
{
    return( CarveMapIndex( carveCGFILE, e ) );
}

static void getCGFileLocn( CGIOBUFF *h, void *cursor, CGFILE_INS *p )
{
    p->block = h->disk_addr;
    p->offset = ((char*)cursor) - h->data;
}

static unsigned padOutICBlock( unsigned ics )
{
    auto CGINTER pad_ic;

    if(( ics % CGINTER_BLOCKING ) != 0 ) {
        pad_ic.opcode = IC_PCH_PAD;
        do {
            ++ics;
            pad_ic.value.uvalue = ics;
            PCHWriteUnaligned( &pad_ic, sizeof( pad_ic ) );
        } while(( ics % CGINTER_BLOCKING ) != 0 );
    }
    return( ics );
}

static void saveCGFILE( void *e, carve_walk_base *d )
{
    CGFILE *file = e;
    CGIOBUFF *h;
    CGIOBUFF *stop_buffer;
    CGINTER *cursor;
    CGINTER *stop_cursor;
    ic_op_type op_class;
    CGINTEROP opcode;
    unsigned ics;
    void *zap_reloc;
    CGIRELOCFN *reloc;
    auto CGINTER terminator;
    auto CGINTER zap_ref;
    auto CGFILE_INS zap_locn;

    if( !file->u.s.write_to_pch || file->symbol == BRINF_SYMBOL ) {
        return;
    }
    PCHWriteCVIndex( d->index );
    SymbolPCHWrite( file->symbol );
    SymbolPCHWrite( file->opt_retn );
    PCHWriteUInt( file->u.flags );
    h = CgioBuffRdOpen( file->first );
    cursor = (CGINTER *)( h->data + file->offset );
    ics = 0;
    if( file->u.s.done ) {
        // CGFILE contains a finished function
        for(;;) {
            opcode = cursor->opcode;
            op_class = ICOpTypes[ opcode ];
            reloc = relocWriteOperand[ op_class ];
            if( reloc != NULL ) {
                void *save = cursor->value.pvalue;
                cursor->value.pvalue = reloc( save );
                PCHWriteUnaligned( cursor, sizeof( *cursor ) );
                cursor->value.pvalue = save;
            } else {
                PCHWriteUnaligned( cursor, sizeof( *cursor ) );
            }
            ++ics;
            if( opcode == IC_EOF ) break;
            h = CgioBuffReadIC( h, &cursor );
        }
    } else {
        // CGFILE contains a function in progress
        stop_buffer = file->buffer;
        stop_cursor = (CGINTER *) ( stop_buffer->data + stop_buffer->free_offset );
        // Ivan's bug fix simplified:
        // if stop_cursor randomly contains an IC_NEXT opcode, the final
        // CgioBuffRead may leap off into never-never land;
        // since we can always write one more instruction, we simply
        // make sure we have a benign opcode so that the following loop
        // terminates properly (many thanks to Ivan for tracking this down!)
        // Note: this is the only case where we are reading a CGFILE that
        // is not terminated by an IC_EOF but the same precaution must be
        // taken if we ever do it in some other case
        stop_cursor->opcode = IC_NO_OP;
        DbgAssert( stop_cursor->opcode != IC_NEXT );
        for(;;) {
            if( h == stop_buffer && cursor == stop_cursor ) break;
            opcode = cursor->opcode;
            // might be the destination of a CgioZap
            zap_ref.opcode = IC_NO_OP;
            switch( opcode ) {
            case IC_BLOCK_OPEN:
            case IC_BLOCK_DEAD:
                getCGFileLocn( h, cursor, &zap_locn );
                zap_reloc = ModuleIsZap2( &zap_locn );
                if( zap_reloc != NULL ) {
                    zap_ref.opcode = IC_ZAP2_REF;
                    zap_ref.value.pvalue = zap_reloc;
                }
                break;
            case IC_NO_OP:
                getCGFileLocn( h, cursor, &zap_locn );
                if( ModuleIsZap1( &zap_locn ) ) {
                    zap_ref.opcode = IC_ZAP1_REF;
                    zap_ref.value.ivalue = 0;
                }
                break;
            }
            op_class = ICOpTypes[ opcode ];
            reloc = relocWriteOperand[ op_class ];
            if( reloc != NULL ) {
                void *save = cursor->value.pvalue;
                cursor->value.pvalue = reloc( save );
                PCHWriteUnaligned( cursor, sizeof( *cursor ) );
                cursor->value.pvalue = save;
            } else {
                PCHWriteUnaligned( cursor, sizeof( *cursor ) );
            }
            ++ics;
            if( zap_ref.opcode != IC_NO_OP ) {
                PCHWriteUnaligned( &zap_ref, sizeof( zap_ref ) );
                ++ics;
                ics = padOutICBlock( ics );
            }
            h = CgioBuffReadIC( h, &cursor );
        }
        terminator.opcode = IC_PCH_STOP;
        PCHWriteUnaligned( &terminator, sizeof( terminator ) );
        ++ics;
    }
    padOutICBlock( ics );
    CgioBuffRdClose( h );
}

static void markFreeCGFILE( void *p )
{
    CGFILE *c = p;

    c->u.s.write_to_pch = false;
}

pch_status PCHWriteCGFiles( void )
{
    CGFILE *curr;
    auto carve_walk_base data;

    CarveWalkAllFree( carveCGFILE, markFreeCGFILE );
    RingIterBeg( cg_file_ring, curr ) {
        curr->u.s.write_to_pch = true;
    } RingIterEnd( curr )
    CarveWalkAll( carveCGFILE, saveCGFILE, &data );
    PCHWriteCVIndexTerm();
    return( PCHCB_OK );
}

pch_status PCHReadCGFiles( void )
{
    CGINTEROP opcode;
    CGFILE *curr;
    CGINTER last;
    auto cvinit_t data;

    CarveInitStart( carveCGFILE, &data );
    for( ; (curr = PCHReadCVIndexElement( &data )) != NULL; ) {
        RingAppend( &cg_file_ring, curr );
        initCGFILE( curr, SymbolPCHRead() );
        curr->opt_retn = SymbolPCHRead();
        curr->u.flags = PCHReadUInt();
        for(;;) {
            // The following comment is a trigger for the ICMASK program to
            // start scanning for case IC_* patterns.
            // ICMASK BEGIN PCHREAD (do not remove)
            /* fake case labels for ICMASK program (do not remove)
                case IC_PCH_STOP:
                case IC_ZAP1_REF:
                case IC_ZAP2_REF:
                case IC_EOF:
            */
            // ICMASK END (do not remove)
            last = CgioBuffPCHRead( &(curr->buffer) );
            opcode = last.opcode;
            if( opcode == IC_PCH_STOP )
                break;
            if( opcode == IC_EOF ) {
                DbgAssert( ICOpTypes[ opcode ] == ICOT_NUL );
                // this writes the IC_EOF into the buffer
                CgioCloseOutputFile( curr );
                break;
            }
            switch( opcode ) {
            case IC_ZAP1_REF:
                ModuleAdjustZap1( curr );
                break;
            case IC_ZAP2_REF:
                ModuleAdjustZap2( curr, last.value.pvalue );
                break;
            DbgDefault( "unexpected IC opcode during PCH read" );
            }
        }
    }
    return( PCHCB_OK );
}

pch_status PCHInitCGFiles( bool writing )
{
    if( writing ) {
        PCHWriteCVIndex( CarveLastValidIndex( carveCGFILE ) );
        PCHWriteCVIndex( CarveLastValidIndex( carveCGFILE_GEN ) );
    } else {
        CGFILE *curr;

        // shut down all pending functions so that the CGIOBUFF can stabilize
        // to a point where there are no outstanding writes in progress
        RingIterBeg( cg_file_ring, curr ) {
            if( ! curr->u.s.done ) {
                CgioCloseOutputFile( curr );
            }
        } RingIterEnd( curr )
        cg_file_ring = NULL;
        carveCGFILE = CarveRestart( carveCGFILE );
        CarveMapOptimize( carveCGFILE, PCHReadCVIndex() );
        carveCGFILE_GEN = CarveRestart( carveCGFILE_GEN );
        CarveMapOptimize( carveCGFILE_GEN, PCHReadCVIndex() );
    }
    return( PCHCB_OK );
}

pch_status PCHFiniCGFiles( bool writing )
{
    if( ! writing ) {
        CarveMapUnoptimize( carveCGFILE );
        CarveMapUnoptimize( carveCGFILE_GEN );
    }
    return( PCHCB_OK );
}
