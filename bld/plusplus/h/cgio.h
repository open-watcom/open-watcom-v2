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


#ifndef __CGIO_H__
#define __CGIO_H__

// CGIO.H -- C++ definitions related to code-generation files
//
// 91/06/10 -- J.W.Welch        -- defined
// 92/05/29 -- J.W.Welch        -- introduced CGFILE_GEN
// 95/05/01 -- A.F.Scian        -- changed size of DISK_OFFSET

#include <unistd.h>

#include "cgiobuff.h"
#include "cgdata.h"
#include "toknlocn.h"


// STRUCTURES:

#ifndef CGFILE_DEFINED
#define CGFILE_DEFINED
typedef struct cgfile           CGFILE;         // control for a virtual file
#endif
typedef struct cgfile_gen       CGFILE_GEN;     // data while generating file

struct cgfile {                 // CGFILE -- control for a virtual file
    CGFILE      *next;          // - next virtual file
    DISK_ADDR   first;          // - first block
    DISK_OFFSET offset;         // - offset in first block
    CGIOBUFF    *buffer;        // - current buffer in memory
    SYMBOL      symbol;         // - controlling symbol-table definition
    CGINTER     *cursor;        // - cursor moving through the block
    CGFILE_GEN  *gen;           // - data while generating file
    TOKEN_LOCN  defined;        // - location of definition
    unsigned    cond_flags;     // - # conditional flags req'd in state table
  union {
   unsigned     flags;          // - overlays flags for .pch
   struct {
    unsigned    delayed      :1;// - TRUE ==> delayed for code generation
    unsigned    refed        :1;// - TRUE ==> referenced during code gener.
    unsigned    stgen        :1;// - TRUE ==> code generated static init
    unsigned    oe_inl       :1;// - TRUE ==> can be inlined when /oe
    unsigned    state_table  :1;// - TRUE ==> has a state table
    unsigned    once_inl     :1;// - TRUE ==> should be inlined only once
    unsigned    done         :1;// - TRUE ==> IC_EOF has been written
    unsigned    thunk        :1;// - TRUE ==> is addressibility thunk
    unsigned    thunk_gen    :1;// - TRUE ==> generate addressibility thunk
    unsigned    not_inline   :1;// - TRUE ==> do not inline
    unsigned    calls_inline :1;// - TRUE ==> calls another inline
    unsigned    can_throw    :1;// - TRUE ==> could throw an exception
    unsigned    stab_gen     :1;// - TRUE ==> state table needs generation
    unsigned    ctor_test    :1;// - TRUE ==> might have ctor test
    unsigned    write_to_pch :1;// - TRUE ==> write out to PCH
    unsigned    opt_retn_val :1;// - TRUE ==> optimize struct return: value
    unsigned    opt_retn_ref :1;// - TRUE ==> optimize struct return: reference
          } s;  // of struct
        };      // of union
    SYMBOL      opt_retn;       // - symbol translated for optimized return
};

struct cgfile_gen {             // CGFILE_GEN -- data while generating file
    unsigned    cs_label;       // - current CS (control-structure) label index
    unsigned    cs_allocated;   // - number of CS labels pre-allocated
    unsigned    goto_label;     // - current GOTO label index
    SYMBOL      init_sym;       // - SYMBOL for IC_INIT_BEG
    TYPE        curr_type;      // - current type
    TYPE        emit_type;      // - emitted type
    TOKEN_LOCN  emit_line_no;   // - emitted line #
    unsigned    emit_init_beg :1;// - IC_INIT_BEG to be generated
    unsigned    emit_init_end :1;// - IC_INIT_END to be generated
};

// MACROS
#define CgioRewriteRecord CgioBuffZap

#define CgioLastRead( cg ) CgioBuffLastRead( (cg)->buffer, (cg)->cursor )
#define CgioLastWrite( cg ) CgioBuffLastWrite( (cg)->buffer )


// PROTOTYPES:

void CgioCloseInputFile(        // CLOSE VIRTUAL FILE AFTER INPUT PHASE
    CGFILE *ctl )               // - control for file
;
void CgioCloseOutputFile(       // CLOSE VIRTUAL FILE AFTER OUTPUT PHASE
    CGFILE *ctl )               // - control for file
;
CGFILE *CgioCreateFile(         // CREATE A CG VIRTUAL FILE
    SYMBOL symbol )             // - controlling symbol-table entry
;
void CgioFreeFile(              // FREE A VIRTUAL FILE
    CGFILE *ctl )               // - control for file
;
CGFILE* CgioLocateAnyFile(      // LOCATE LIVE OR REMOVED FILE FOR A SYMBOL
    SYMBOL sym )                // - the symbol
;
CGFILE *CgioLocateFile(         // LOCATE FILE FOR A SYMBOL
    SYMBOL sym )                // - symbol for file
;
void CgioOpenInput(             // OPEN VIRTUAL FILE FOR INPUT
    CGFILE *ctl )               // - control for file
;
CGINTER *CgioReadIC(            // READ IC RECORD (LOCATE MODE)
    CGFILE *ctl )               // - control for the file
;
CGINTER *CgioReadICUntilOpcode( // READ IC RECORD UNTIL OPCODE IS FOUND
    CGFILE *ctl,                // - control for the file
    unsigned opcode )           // - opcode to find
;
CGINTER *CgioReadICMask(        // READ IC RECORD UNTIL OPCODE IN SET IS FOUND
    CGFILE *ctl,                // - control for the file
    unsigned mask )             // - control mask for opcodes
;
CGINTER *CgioReadICMaskCount(   // READ IC RECORD UNTIL OPCODE IN SET IS FOUND
    CGFILE *ctl,                // - control for the file
    unsigned mask,              // - control mask for opcodes to return
    unsigned count_mask,        // - control mask for opcodes to count
    unsigned *count )           // - counter to update
;
void CgioThunkAddrTaken(        // INDICATE ADDR TAKEN OF THUNK BY GEN'ED CODE
    SYMBOL thunk )              // - a thunk
;
void CgioThunkMarkGen(          // MARK THUNK TO BE GENERATED
    CGFILE* thunk )             // - CGFILE for thunk symbol
;
void CgioThunkStash(            // STASH AWAY A THUNK TO BE GEN'ED
    CGFILE* thunk )             // - that thunk
;
void CgioWalkFiles(             // WALK THRU FILES
    void (*rtn)                 // - traversal routine
        ( CGFILE *curr ) )      // - - passed current file
;
void CgioWalkThunks(            // WALK THRU STASHED THUNKS
    void (*rtn)                 // - traversal routine
        ( CGFILE *curr ) )      // - - passed current file
;
void CgioWriteIC(               // WRITE IC RECORD TO VIRTUAL FILE
    CGFILE *ctl,                // - control for the file
    CGINTER *ins )              // - instruction
;
CGINTER *CgioSeek(              // SEEK TO POSITION IN VIRTUAL FILE
    CGFILE *ctl,                // - control for the file
    CGFILE_INS *posn )          // - position to seek to
;
#ifndef NDEBUG

void DbgCgioEndCallGr(          // CALLED AT END OF CALL-GRAPH PROCESSING
    void )
;
void DbgCgioEndFront(           // CALLED AT END OF FRONT END
    void )
;

#else

#define DbgCgioEndCallGr()
#define DbgCgioEndFront()

#endif

CGFILE *CGFileMapIndex( CGFILE * );
CGFILE *CGFileGetIndex( CGFILE * );

CGVALUE CgioGetIndex( unsigned, CGVALUE );
CGVALUE CgioMapIndex( unsigned, CGVALUE );

#endif
