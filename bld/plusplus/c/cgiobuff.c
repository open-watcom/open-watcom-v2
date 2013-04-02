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
#include <sys/types.h>
#include "wio.h"
#include "memmgr.h"
#include "errdefns.h"
#include "cgiobuff.h"
#include "ring.h"
#include "iosupp.h"
#include "fname.h"
#include "toggle.h"
#include "dbg.h"
#include "stats.h"
#include "cgdata.h"
#include "icopmask.h"
#include "stringl.h"
#include "conpool.h"
#include "pcheader.h"

#define CGIOBUFF_CHECK  (-(sizeof(CGIOBUFF)|1))

// This must be free at bottom of a block for that block to be re-used
// for another virtual file
#define MAX_REUSE       ( sizeof( CGINTER ) * 8 + sizeof( BUFF_XFER ) )

// maximum capacity of a buffer before an IC_NEXT instruction is req'd
#define MAX_WRITE_AMT   ( TMPBLOCK_BSIZE - sizeof( BUFF_XFER ) - sizeof( CGINTER ) )

#ifndef NDEBUG
#define DICT_SIZE       4
#include "pragdefn.h"
#else
#define DICT_SIZE       128
#endif

typedef struct buff_dict BUFFDICT;
struct buff_dict {
    BUFFDICT    *next;
    CGIOBUFF    *dict[DICT_SIZE];
};

static BUFFDICT *directoryOfBufs;
static unsigned maxDictIndex;
static CGIOBUFF *allBufs;
static CGIOBUFF *reusableBufs;
static CGIOBUFF *beingWritten;          // NULL or the one being written

static icop_mask icMaskTable[] = {
    #include "ic_mask.gh"
};

ic_op_type ICOpTypes[] = {
    #define IC( code, type, mask )    ICOT_##type
    #include "ic.h"
    #undef IC
};

#define doReadOpNUL     NULL
#define doReadOpBIN     NULL
#define doReadOpCON     ConstantPoolMapIndex
#define doReadOpSYM     SymbolMapIndex
#define doReadOpSTR     StringMapIndex
#define doReadOpTYP     TypeMapIndex
#define doReadOpSCP     ScopeMapIndex
#define doReadOpSRC     SrcFileMapIndex

static CGIRELOCFN *relocReadOperand[] = {
#define ITYPE( typ )    ((CGIRELOCFN*) doReadOp##typ)
#include "itypes.h"
};

ExtraRptCtr( ctr_buffers );     // counts buffers
ExtraRptCtr( ctr_usage );       // counts bytes used in buffers


typedef struct {                // disk transfer code
    CGINTER     leap_ins;       // - IC_NEXT (value.uvalue has disk block)
    DISK_OFFSET offset;         // - disk offset
} BUFF_XFER;

#define pointXferOffset( ctl, off )     ( (void*)( (ctl)->data + (off)) )
#define pointXfer( ctl )                ( (void*)( (ctl)->data + (ctl)->free_offset) )

static BUFFDICT *newDict( void )
{
    BUFFDICT *dict;

    dict = CPermAlloc( sizeof( *dict ) );
    memset( dict, 0, sizeof( *dict ) );
    dict->next = directoryOfBufs;
    directoryOfBufs = dict;
    ++maxDictIndex;
    return( dict );
}

static void addDirectoryEntry( CGIOBUFF *e )
{
    unsigned dict_index;
    unsigned dict_offset;
    BUFFDICT *dict;

    dict_index = e->disk_addr / DICT_SIZE;
    dict_offset = e->disk_addr % DICT_SIZE;
    DbgAssert( dict_index <= (maxDictIndex + 1) );
    dict = directoryOfBufs;
    if( dict_index != maxDictIndex ) {
        dict = newDict();
    }
    dict->dict[ dict_offset ] = e;
}

static CGIOBUFF **findDirectoryEntry( DISK_ADDR disk_addr )
{
    unsigned dict_index;
    unsigned dict_offset;
    unsigned i;
    BUFFDICT *dict;

    dict_index = disk_addr / DICT_SIZE;
    dict_offset = disk_addr % DICT_SIZE;
    DbgAssert( dict_index <= maxDictIndex );
    dict = directoryOfBufs;
    i = maxDictIndex;
    for(;;) {
        if( i == dict_index ) break;
        DbgAssert( i != 0 );
        --i;
        dict = dict->next;
        DbgAssert( dict != NULL );
    }
    return( &(dict->dict[ dict_offset ]) );
}

static void setDirectoryEntry( DISK_ADDR disk_addr, CGIOBUFF *e )
{
    CGIOBUFF **dict;

    dict = findDirectoryEntry( disk_addr );
    *dict = e;
}


static void addToReuseList(     // ADD BUFFER TO REUSE LIST (IF IT CAN BE REUSED)
    CGIOBUFF *ctl )             // - buffer control
{
    CGIOBUFF *head;
    CGIOBUFF *head_next;

    if( ctl->reuse_next != NULL ) {
        return;
    }
    if( ctl->writing ) {
        return;
    }
    if( ctl->free_offset > ( TMPBLOCK_BSIZE - MAX_REUSE ) ) {
        return;
    }
    if( reusableBufs != NULL ) {
        head = reusableBufs;
        head_next = head->reuse_next;
        // insert between 'head' and 'head_next'
        ctl->reuse_prev = head;
        ctl->reuse_next = head_next;
        head_next->reuse_prev = ctl;
        head->reuse_next = ctl;
    } else {
        ctl->reuse_prev = ctl;
        ctl->reuse_next = ctl;
        reusableBufs = ctl;
    }
}


static void removeFromReuseList(// DETACH BUFFER FROM REUSE LIST
    CGIOBUFF *ctl )             // - buffer control
{
    CGIOBUFF *prev;
    CGIOBUFF *next;

    DbgAssert( ctl->reuse_next != NULL );
    DbgAssert( ctl->reuse_prev != NULL );
    prev = ctl->reuse_prev;
    next = ctl->reuse_next;
    prev->reuse_next = next;
    next->reuse_prev = prev;
    if( reusableBufs == ctl ) {
        if( prev == next ) {
            reusableBufs = NULL;
        } else {
            reusableBufs = prev;
        }
    }
    // must be here in case 'prev' == 'next'
    ctl->reuse_prev = NULL;
    ctl->reuse_next = NULL;
}


static CGIOBUFF *allocateBuffer(// ALLOCATE A NEW BUFFER
    DISK_ADDR block )           // - block number
{
    CGIOBUFF *ctl;              // - new buffer

    ctl = CMemAlloc( sizeof( *ctl ) );
    ctl->next = NULL;
    ctl->reuse_prev = NULL;
    ctl->reuse_next = NULL;
    ctl->disk_addr = block;
    ctl->free_offset = 0;
    ctl->reading = 0;
    ctl->written = FALSE;
    ctl->writing = FALSE;
    ctl->active = FALSE;
    DbgStmt( ctl->check = CGIOBUFF_CHECK );
    RingAppend( &allBufs, ctl );
    ExtraRptIncrementCtr( ctr_buffers );
    return ctl;
}


static CGIOBUFF *findWrBuffer(  // FIND A BUFFER FOR WRITING
    void )
{
    CGIOBUFF *ctl;              // - buffer control
    DISK_ADDR block;            // - block number

    ctl = reusableBufs;
    if( ctl != NULL ) {
        removeFromReuseList( ctl );
    } else {
        block = IoSuppTempNextBlock( 1 );
        ctl = allocateBuffer( block );
        addDirectoryEntry( ctl );
    }
    ctl->active = TRUE;
    ctl->writing = TRUE;
    ctl->written = FALSE;
    DbgAssert( ctl->check == CGIOBUFF_CHECK );
    return( ctl );
}


static CGIOBUFF *findRdBuffer(  // FIND A BUFFER FOR READING
    DISK_ADDR block  )          // - disk block
{
    CGIOBUFF *ctl;
    CGIOBUFF **dict;

    dict = findDirectoryEntry( block );
    ctl = *dict;
    if( ctl == NULL ) {
        ctl = allocateBuffer( block );
        setDirectoryEntry( block, ctl );
        IoSuppTempRead( block, TMPBLOCK_BSIZE, ctl->data );
        ctl->written = TRUE;
        ctl->free_offset = TMPBLOCK_BSIZE;
    }
    ctl->active = TRUE;
    ++ctl->reading;
    DbgAssert( ctl->check == CGIOBUFF_CHECK );
    return ctl;
}


static void finishWrBuffer(     // COMPLETE WRITE-USE OF A BUFFER
    CGIOBUFF *ctl )             // - buffer control
{
    DbgAssert( ctl->check == CGIOBUFF_CHECK );
    ctl->writing = FALSE;
    if( ctl->reading == 0 ) {
        ctl->active = FALSE;
    }
    addToReuseList( ctl );
}


static void finishRdBuffer(     // COMPLETE READ-USE OF A BUFFER
    CGIOBUFF *ctl )             // - buffer control
{
    DbgAssert( ctl->check == CGIOBUFF_CHECK );
    -- ctl->reading;
    if( ctl->reading == 0 && ! ctl->writing ) {
        ctl->active = FALSE;
        addToReuseList( ctl );
    }
}


CGIOBUFF *CgioBuffWrOpen(       // GET BUFFER FOR WRITING
    void )
{
    return findWrBuffer();
}

static CGIOBUFF *switchToNextBuffer( CGIOBUFF *ctl )
{
    CGIOBUFF *next;             // - next buffer control
    BUFF_XFER *xfer;            // - transfer record

    next = findWrBuffer();
    xfer = pointXfer( ctl );
    xfer->leap_ins.opcode = IC_NEXT;
    xfer->leap_ins.value.uvalue = next->disk_addr;
    xfer->offset = next->free_offset;
    ctl->free_offset += sizeof( BUFF_XFER );
    finishWrBuffer( ctl );
    return( next );
}

CGINTER *CgioBuffPCHRead(       // READ FROM PCH AND WRITE INTO BUFFER
    CGINTER *buff,              // - alternate buffer for PCHReadLoc
    CGIOBUFF **pctl )           // - addr( buffer control )
{
    CGIRELOCFN *relocate;       // - relocator function
    CGINTER *p_instr;           // - IC to process
    CGINTER *s_instr;           // - past last IC to process
    CGIOBUFF *ctl;              // - buffer control
    CGINTER *start;             // - first destination for IC
    CGINTER *dest;              // - destination for IC
    CGINTER *stop;              // - last destination for IC
    unsigned opcode;            // - IC opcode

    #define CBP_LIMIT ( MAX_WRITE_AMT - CGINTER_BLOCKING * sizeof(CGINTER) )
    ctl = *pctl;
    start = pointXfer( ctl );
    dest = start;
    stop = pointXferOffset( ctl, CBP_LIMIT );
    for(;;) {
        if( dest >= stop ) {
            ctl->free_offset += ((char*)dest) - (char*)start;
            ctl = switchToNextBuffer( ctl );
            start = pointXfer( ctl );
            dest = start;
            stop = pointXferOffset( ctl, CBP_LIMIT );
        }
        PCHReadLocSize( p_instr, buff, CGINTER_BLOCKING * sizeof( *p_instr ) );
        s_instr = &p_instr[ CGINTER_BLOCKING ];
        do {
            opcode = p_instr->opcode;
            if( icMaskTable[ opcode ] & ICOPM_PCHREAD ) {
                ctl->free_offset += ((char*)dest) - (char*)start;
                *pctl = ctl;
                // rest of block can be ignored
#ifndef NDEBUG
                {
                    CGINTER *c = p_instr + 1;
                    DbgAssert( c <= s_instr );
                    DbgAssert(( s_instr - c ) < CGINTER_BLOCKING );
                    while( c < s_instr ) {
                        DbgAssert( c->opcode == IC_PCH_PAD );
                        ++c;
                    }
                }
#endif
                return( p_instr );
            }
            relocate = relocReadOperand[ ICOpTypes[ opcode ] ];
            if( relocate != NULL ) {
                dest->value.pvalue = (*relocate)( p_instr->value.pvalue );
            } else {
                dest->value = p_instr->value;
            }
            dest->opcode = opcode;
            ++dest;
            ++p_instr;
        } while( p_instr < s_instr );
    }
    #undef CBP_LIMIT
}


CGVALUE CgioMapIndex( unsigned opcode, CGVALUE value )
/****************************************************/
{
    ic_op_type op_class;
    CGIRELOCFN *reloc;

    op_class = ICOpTypes[ opcode ];
    reloc = relocReadOperand[ op_class ];
    if( reloc != NULL ) {
        value.pvalue = reloc( value.pvalue );
    }
    return( value );
}


CGIOBUFF *CgioBuffWriteIC(      // WRITE AN IC RECORD
    CGIOBUFF *ctl,              // - buffer control
    CGINTER *ins )              // - IC to write
{
    CGINTER *dest;              // - destination for IC

    if( ctl->free_offset > MAX_WRITE_AMT ) {
        ctl = switchToNextBuffer( ctl );
    }
#ifdef CRIPPLE
    {
        static unsigned count;

        ++count;
        if( count > CGIO_CRIPPLE_LIMIT ) {
            CErr1( ERR_OUT_OF_MEMORY );
            CSuicide();
        }
    }
#endif
#ifndef NDEBUG
    DbgAssert( ins->opcode != IC_EOF );
    if( icMaskTable[ ins->opcode ] & ICOPM_BRINFO ) {
        if( PragDbgToggle.browse_emit ) {
            DumpCgFront( "BiEm", ctl->disk_addr, ctl->free_offset, ins );
        }
    } else {
        if( PragDbgToggle.dump_emit_ic ) {
            DumpCgFront( "Emit", ctl->disk_addr, ctl->free_offset, ins );
        }
        if( ICOpTypes[ ins->opcode ] == ICOT_SYM ) {
            SYMBOL sym = ins->value.pvalue;
            if( sym != NULL && !SymIsCatchAlias( sym ) && SymIsAlias( sym ) ) {
                if( ErrCount == 0 ) {
                    CFatal( "alias symbol in IC stream" );
                }
            }
        }
    }
#endif
    dest = pointXfer( ctl );
    dest->opcode = ins->opcode;
    dest->value = ins->value;
    ctl->free_offset += sizeof( CGINTER );
    // we will always be able to write an IC_NEXT so this is OK
    DbgStmt(( ++dest, dest->opcode = IC_NEXT, dest->value.ivalue = -1 ));
    return ctl;
}


void CgioBuffWrClose(           // RELEASE BUFFER AFTER WRITING
    CGIOBUFF *ctl )             // - buffer control
{
    CGINTER *p;

    p = pointXfer( ctl );
    p->opcode = IC_EOF;
    p->value.uvalue = 0;
    ctl->free_offset += sizeof( CGINTER );
    finishWrBuffer( ctl );
}


CGIOBUFF *CgioBuffRdOpen(       // GET BUFFER FOR READING
    DISK_ADDR block )           // - disk address
{
    return findRdBuffer( block );
}

#ifndef NDEBUG
static void dumpRead            // DBG: TRACE AN INSTRUCTION READ
    ( CGIOBUFF *ctl             // - buffer control
    , CGINTER *curr             // - current instruction
    , char *exec                // - execution string
    , char *scan )              // - scan string
{
    char * prefix = NULL;       // - NULL or prefix when tracing

    if( IC_EOF != curr->opcode
     && icMaskTable[ curr->opcode ] & ICOPM_BRINFO ) {
        if( PragDbgToggle.browse_read ) {
            prefix = "BiRd";
        }
    } else if( PragDbgToggle.dump_exec_ic ) {
        prefix = exec;
    } else if( PragDbgToggle.callgraph_scan ) {
        prefix = scan;
    }
    if( NULL != prefix ) {
        unsigned offset = ((char*)curr) - ctl->data;
        DumpCgFront( prefix, ctl->disk_addr, offset, curr );
    }
}
#else
#define dumpRead( ctl, curr, exec, scan )
#endif

#define _NEXT_BLOCK( __ctl, __curr ) \
    { \
        BUFF_XFER *xfer = (BUFF_XFER *) __curr; \
        CGIOBUFF *next = findRdBuffer( xfer->leap_ins.value.uvalue ); \
        __curr = (CGINTER *)( next->data + xfer->offset ); \
        finishRdBuffer( __ctl ); \
        __ctl = next; \
    }


CGIOBUFF *CgioBuffReadIC(       // READ A RECORD
    CGIOBUFF *ctl,              // - buffer control
    CGINTER **ins )             // - cursor to update
{
    CGINTER *curr;

    curr = (*ins) + 1;
    if( curr->opcode == IC_NEXT ) {
        _NEXT_BLOCK( ctl, curr );
    }
    *ins = curr;
    dumpRead( ctl, curr, "Exec", "Scan" );
    return ctl;
}


CGIOBUFF *CgioBuffReadICUntilOpcode(       // READ A RECORD UNTIL OPCODE IS FOUND
    CGIOBUFF *ctl,              // - buffer control
    CGINTER **ins,              // - cursor to update
    unsigned opcode )           // - opcode to find
{
    unsigned check_opcode;
    CGINTER *curr;

    curr = *ins;
    ++curr;
    for(;;) {
        check_opcode = curr->opcode;
        if( check_opcode == opcode ) break;
        if( check_opcode == IC_NEXT ) {
            _NEXT_BLOCK( ctl, curr );
        } else {
            ++curr;
        }
    }
    *ins = curr;
    dumpRead( ctl, curr, "Find", "Find" );
    return ctl;
}


CGIOBUFF *CgioBuffReadICMask(   // READ A RECORD UNTIL OPCODE IN SET IS FOUND
    CGIOBUFF *ctl,              // - buffer control
    CGINTER **ins,              // - cursor to update
    unsigned mask )             // - control mask for opcodes
{
    CGINTER *curr;
    unsigned opcode;

    curr = *ins;
    ++curr;
    for(;;) {
        opcode = curr->opcode;
        if( icMaskTable[ opcode ] & mask ) break;
        if( opcode == IC_NEXT ) {
            _NEXT_BLOCK( ctl, curr );
        } else {
            ++curr;
        }
    }
    *ins = curr;
    dumpRead( ctl, curr, "Mask", "Mask" );
    return ctl;
}


CGIOBUFF *CgioBuffReadICMaskCount(      // READ A RECORD UNTIL OPCODE IN SET IS FOUND
    CGIOBUFF *ctl,              // - buffer control
    CGINTER **ins,              // - cursor to update
    unsigned mask,              // - control mask for opcodes to return
    unsigned count_mask,        // - control mask for opcodes to count
    unsigned *count )           // - counter to update
{
    unsigned opcode;
    unsigned extra;
    unsigned op_mask;
    CGINTER *curr;

    extra = 0;
    curr = *ins;
    ++curr;
    for(;;) {
        opcode = curr->opcode;
        op_mask = icMaskTable[ opcode ];
        if( op_mask & count_mask ) {
            ++extra;
        }
        if( op_mask & mask ) break;
        if( opcode == IC_NEXT ) {
            _NEXT_BLOCK( ctl, curr );
        } else {
            ++curr;
        }
    }
    *count += extra;
    *ins = curr;
    dumpRead( ctl, curr, "Mask", "Mask" );
    return ctl;
}


CGIOBUFF *CgioBuffSeek(         // SEEK TO POSITION
    CGIOBUFF *ctl,              // - buffer control
    CGFILE_INS *posn,           // - position to seek to
    CGINTER **ins )             // - cursor to update
{
    CGINTER *curr;

    if( posn->block != ctl->disk_addr ) {
        CGIOBUFF *next = findRdBuffer( posn->block );
        finishRdBuffer( ctl );
        ctl = next;
    }
    DbgAssert( ctl->check == CGIOBUFF_CHECK );
    curr = (CGINTER *)( ctl->data + posn->offset );
    *ins = curr;
    dumpRead( ctl, curr, "Seek", "Seek" );
    return( ctl );
}


CGFILE_INS CgioBuffLastRead(    // RETURN POSITION OF LAST READ
    CGIOBUFF *ctl,              // - buffer control
    CGINTER *ins )              // - current cursor
{
    CGFILE_INS posn;

    // depends on CgioBuffReads leaving the cursor set to last read posn
    DbgAssert( ctl->check == CGIOBUFF_CHECK );
    posn.offset = (char*)(ins) - ctl->data;
    posn.block = ctl->disk_addr;
    return( posn );
}


CGFILE_INS CgioBuffLastWrite(   // RETURN POSITION OF LAST WRITE
    CGIOBUFF *ctl  )            // - buffer control
{
    CGFILE_INS posn;

    // depends on CgioBuffWrite leaving the cursor set to just after write
    // (no IC_NEXT problems since IC_NEXT is always generated BEFORE an
    //  IC code is written)
    DbgAssert( ctl->check == CGIOBUFF_CHECK );
    posn.block = ctl->disk_addr;
    posn.offset = ctl->free_offset - sizeof( CGINTER );
    return( posn );
}


void CgioBuffRdClose(           // RELEASE BUFFER AFTER READING
    CGIOBUFF *ctl )             // - buffer control
{
    finishRdBuffer( ctl );
}


static void cgioBuffReleaseMemory(      // RELEASE MEMORY
    void )
{
#if 0
//
// This algorithm is bad because it builds a temporary list (first loop )
// which becomes a memory leak if IoSuppTempWrite suicides.
//
    CGIOBUFF *prev;
    CGIOBUFF *curr;
    CGIOBUFF *list;

    list = NULL;
    prev = NULL;
    RingIterBegSafe( allBufs, curr ) {
        if( ! curr->active
         || CompFlags.compile_failed ) {
            DbgAssert( curr->reading == 0 && ! curr->writing );
            if( curr->reuse_next != NULL ) {
                removeFromReuseList( curr );
            }
            RingPruneWithPrev( &allBufs, curr, prev );
            RingAppend( &list, curr );
            // 'prev' stays the same
        } else {
            DbgAssert( curr->reading != 0 || curr->writing );
            prev = curr;
        }
    } RingIterEndSafe( curr )
    RingIterBegSafe( list, curr ) {
        if( ! curr->written
         && ! CompFlags.compile_failed ) {
            IoSuppTempWrite( curr->disk_addr, TMPBLOCK_BSIZE, curr->data );
        }
        DbgAssert( *findDirectoryEntry( curr->disk_addr ) == curr );
        setDirectoryEntry( curr->disk_addr, NULL );
        DbgAssert( *findDirectoryEntry( curr->disk_addr ) == NULL );
        CMemFree( curr );
    } RingIterEndSafe( curr )
#else
    CGIOBUFF *prev;
    CGIOBUFF *curr;

    prev = NULL;
    RingIterBegSafe( allBufs, curr ) {
        if( ! curr->active
         || CompFlags.compile_failed ) {
            DbgAssert( curr->reading == 0 && ! curr->writing );
            if( curr->reuse_next != NULL ) {
                removeFromReuseList( curr );
            }
            RingPruneWithPrev( &allBufs, curr, prev );
            if( ! curr->written
             && ! CompFlags.compile_failed ) {
                beingWritten = curr;
                IoSuppTempWrite( curr->disk_addr
                               , TMPBLOCK_BSIZE
                               , curr->data );
                beingWritten = NULL;
            }
            DbgAssert( *findDirectoryEntry( curr->disk_addr ) == curr );
            setDirectoryEntry( curr->disk_addr, NULL );
            DbgAssert( *findDirectoryEntry( curr->disk_addr ) == NULL );
            CMemFree( curr );
            // 'prev' stays the same
        } else {
            DbgAssert( curr->reading != 0 || curr->writing );
            prev = curr;
        }
    } RingIterEndSafe( curr )
#endif
}

void CgioBuffInit(              // BUFFERING INITIALIZATION
    void )
{
    beingWritten = NULL;
    allBufs = NULL;
    reusableBufs = NULL;
    directoryOfBufs = NULL;
    maxDictIndex = -1;
    newDict();
    CMemRegisterCleanup( cgioBuffReleaseMemory );
    ExtraRptRegisterCtr( &ctr_buffers, "buffers for IC file" );
    ExtraRptRegisterCtr( &ctr_usage, "bytes used for IC files" );
    ExtraRptRegisterAvg( &ctr_usage, &ctr_buffers, "average bytes used in IC files" );
}

void CgioBuffFini(              // BUFFERING COMPLETION
    void )
{
#ifndef NDEBUG
    {
        CGIOBUFF *curr;

        RingIterBegSafe( allBufs, curr ) {
            ExtraRptAddtoCtr( ctr_usage, curr->free_offset );
        } RingIterEndSafe( curr )
    }
#endif
    if( NULL != beingWritten ) {
        CMemFree( beingWritten );
        beingWritten = NULL;
    }
    RingFree( &allBufs );
}


void CgioBuffZap(               // ZAP A WRITTEN AREA OF A BUFFER
    CGFILE_INS zap,             // - zap location
    CGINTER *ins )              // - instr to be written
{
    CGINTER *dest;              // - zap location
    CGIOBUFF *ctl;              // - buffer control

    ctl = findRdBuffer( zap.block );
#ifndef NDEBUG
    if( icMaskTable[ ins->opcode ] & ICOPM_BRINFO ) {
        if( PragDbgToggle.browse_emit ) {
            DumpCgFront( "ZAP ", zap.block, zap.offset, ins );
        }
    } else {
        if( PragDbgToggle.dump_emit_ic ) {
            DumpCgFront( "ZAP ", zap.block, zap.offset, ins );
        }
    }
#endif
    DbgAssert( ctl->check == CGIOBUFF_CHECK );
    dest = (CGINTER *)( ctl->data + zap.offset );
    dest->opcode = ins->opcode;
    dest->value = ins->value;
    ctl->written = FALSE;
    finishRdBuffer( ctl );
}
