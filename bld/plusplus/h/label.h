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


#ifndef __LABEL_H
#define __LABEL_H

// LABEL.H -- definitions for labels in code
//
// 91/12/02 -- J.W.Welch        -- defined

#include "cgio.h"

                                    // TYPEDEF's:
typedef struct lab_ref  LAB_REF;    // - source of goto (label reference)
typedef struct lab_def  LAB_DEF;    // - target of goto (label definition)
typedef struct lab_posn LAB_POSN;   // - label position in scope
typedef struct lab_mem  LAB_MEM;    // - memory for label.c

#define BLOCK_LAB_DEF  16           // - # elements per carve block
#define BLOCK_LAB_REF  16           // - # elements per carve block
#define BLOCK_BLK_INIT 16           // - # elements per carve block

struct lab_posn                 // LAB_POSN -- position for a label
{   SCOPE scope;                // - scope
    SYMBOL sym;                 // - last var requiring DTOR
    unsigned var_no;            // - last initialization #
};

struct lab_ref                  // LAB_REF -- for each label reference
{   LAB_REF *next;              // - next reference for this definition
    LAB_DEF *defn;              // - label definition
    LAB_POSN posn;              // - position for reference
    CGFILE_INS ins_exit;        // - IC_BLOCK_EXIT instruction
    CGFILE_INS ins_enter;       // - IC_BLOCK_ENTER instruction
};

struct lab_def                  // LAB_DEF -- for each label definition
{   LAB_POSN posn;              // - position for definition
    LAB_REF *forward;           // - forward references to label
    TOKEN_LOCN locn;            // - source location for label
};

struct lab_mem                  // LAB_MEM -- currency information for a func.
{   void *carve;                // - carve information
    void *blk_hdr;              // - header for BLK_INIT information
};

// PROTOTYPES:

LAB_DEF *LabelAllocLabDef(      // ALLOCATE A LABEL DEFINITION
    void )
;
LAB_REF *LabelAllocLabRef(      // ALLOCATE A LABEL REFERENCE
    LAB_DEF *def )              // - definition for label
;
void LabelBlkCatch(             // INDICATE CATCH BLOCK
    TOKEN_LOCN* posn,           // - position of "{"
    void* try_id )              // - try id
;
void LabelBlkTry(               // INDICATE TRY BLOCK
    TOKEN_LOCN* posn,           // - position of "{"
    SYMBOL try_var )            // - try variable
;
void LabelBlockClose(           // CLOSE CURRENT BLOCK SCOPE
    bool dead_code )            // - true ==> in dead-code state
;
void LabelBlockOpen(            // EMIT OPENING OF CURRENT SCOPE
    bool dead_code )            // - true ==> in dead-code state
;
void LabelTryComplete(          // TRY HAS BEEN COMPLETED
    SYMBOL try_var )            // - try variable
;
void LabelCondEnd(              // END OF CONDITIONAL BLOCK
    void )
;
void LabelCondFalse(            // START false PART OF CONDITIONAL BLOCK
    void )
;
void LabelCondTrue(             // START true PART OF CONDITIONAL BLOCK
    void )
;
void LabelDeclInited(           // SIGNAL NEXT INITIALIZATION IN BLOCK
    SYMBOL sym )
;
void LabelDeclInitedFileScope(  // SIGNAL NEXT INITIALIZATION IN FILE SCOPE
    SYMBOL sym )                // - symbol requiring DTORing
;
void LabelDefine(               // DEFINE A LABEL
    LAB_DEF *def )              // - label definition
;
void LabelExprBegin(            // START OF REGION FOR TEMPORARIES
    void )
;
void LabelExprEnd(              // END OF REGION FOR TEMPORARIES
    void )
;
void LabelExprNewCtor(          // MARK NEW-CTOR REFERENCE
    void )
;
void LabelExprSetjmp(           // MARK SETJMP REFERENCE
    void )
;
void LabelFiniFunc(             // COMPLETION OF LABELS (FUNCTION)
    LAB_MEM *label_mem )        // - used to stack memory
;
void LabelGotoBwd(              // CHECK A GOTO (BACKWARDS)
    LAB_REF *ref )              // - reference for goto (defined) label
;
void LabelGotoFwd(              // CHECK A GOTO (FOREWARDS)
    LAB_REF *ref )              // - reference for goto (defined) label
;
void LabelInitFunc(             // INITIALIZE LABELS (FUNCTION)
    LAB_MEM *label_mem )        // - used to stack memory
;
void LabelReturn(               // RETURN STATEMENT PROCESSING
    void )
;
void LabelSwitch(               // CHECK A CASE/DEFAULT LABEL
    SCOPE sw )                  // - scope containing switch
;
void LabelSwitchBeg(            // START OF A SWITCH STATEMENT
    void )
;
void LabelSwitchEnd(            // COMPLETION OF A SWITCH STATEMENT
    void )
;
void LabelSwitchFunc(           // SWITCH FUNCTION'S LABEL MEMORY
    LAB_MEM *label_mem )        // - used to stack memory
;
void LabelSwitchLabel(          // PROCESSING FOR A BLOCK OF SWITCH LABELS
    SCOPE defn,                 // - scope for switch
    bool deadcode )             // - true==> state is dead-code
;
void LabelTempDtored(           // ENSURE DTOR OF TEMP IS OK
    PTREE expr,                 // - expression for errors
    SYMBOL temp )               // - a temporary symbol
;
void LabelDefFree(              // FREE A LABEL DEFN
    LAB_DEF *def )              // - the definition
;
void LabelRefFree(              // FREE A LABEL REFERENCE
    LAB_REF *ref )              // - the reference
;

extern void *LabelBlockOpenFindZap( LAB_MEM *, CGFILE_INS * );
extern CGFILE_INS *LabelBlockOpenAdjustZap( LAB_MEM *, void * );
extern void LabelPCHWrite( LAB_MEM * );
extern void LabelPCHRead( LAB_MEM * );

#endif
