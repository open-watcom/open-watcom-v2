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


#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#include "cgsegid.h"
#include "tgtenv.h"

enum segments {
    SEG_NULL       = 0,         // no segment yet assigned
    SEG_CODE,                   // code segment
    SEG_CONST,                  // literal strings
    SEG_CONST2,                 // static r/o initialized data
    SEG_DATA,                   // static r/w initialized data
    SEG_TLS_BEG,                // thread-specific data (begin)
    SEG_TLS,                    // thread-specific data (actual)
    SEG_TLS_END,                // thread-specific data (end)
    SEG_BSS,                    // uninitialized data
    SEG_PROF_BEG,               // profiling data (begin)
    SEG_PROF_REF,               // profiling data (actual)
    SEG_PROF_END,               // profiling data (end)
    SEG_INIT_BEG,               // initialization references (begin)
    SEG_INIT_REF,               // initialization references (actual)
    SEG_INIT_END,               // initialization references (end)
    SEG_FINI_BEG,               // finalization references (begin)
    SEG_FINI_REF,               // finalization references (actual)
    SEG_FINI_END,               // finalization references (end)
    SEG_STACK,                  // used only by front end
};


// PROTOTYPES:

char *SegmentClassName(         // GET CLASS NAME OF SEGMENT (IF ANY)
    fe_seg_id id )              // - id of segment
;
#if _CPU == _AXP || COMP_CFG_COFF == 1
fe_seg_id SegmentAddComdatData  // ADD SEGMENT FOR A COMDAT DATA SYMBOL
    ( SYMBOL sym                // - the symbol
    , SEGID_CONTROL control )   // - segment control
;
#endif
fe_seg_id SegmentAddConstFar(   // SEGMENT: ADD CONST SYMBOL TO FAR SEGMENT
    target_size_t size,         // - size of symbol
    target_offset_t align )     // - alignment of symbol
;
fe_seg_id SegmentAddConstHuge(  // SEGMENT: ADD CONST SYMBOL TO HUGE SEGMENT
    target_size_t size )        // - size of symbol
;
fe_seg_id SegmentAddFar(        // SEGMENT: ADD SYMBOL TO FAR SEGMENT
    target_size_t size,         // - size of symbol
    target_offset_t align )     // - alignment of symbol
;
fe_seg_id SegmentAddHuge(       // SEGMENT: ADD SYMBOL TO HUGE SEGMENT
    target_size_t size )        // - size of symbol
;
fe_seg_id SegmentAddStringCodeFar(// SEGMENT: ADD CONST STRING TO CODE SEGMENT
    target_size_t size,         // - size of symbol
    target_offset_t align )     // - alignment of symbol
;
fe_seg_id SegmentAddStringConstFar(// SEGMENT: ADD CONST STRING TO FAR SEGMENT
    target_size_t size,         // - size of symbol
    target_offset_t align )     // - alignment of symbol
;
fe_seg_id SegmentAddSym(        // SEGMENT: ADD SYMBOL TO SPECIFIED SEGMENT
    SYMBOL sym,                 // - sym to add
    fe_seg_id id,               // - id of segment to use
    target_size_t size,         // - size of sym
    target_offset_t align )     // - alignment for sym
;
target_offset_t SegmentAdjust(  // SEGMENT: ADJUST OFFSET TO ALIGN
    fe_seg_id segid,            // - segment identifier
    target_size_t offset,       // - current offset
    target_offset_t align )     // - required aligment
;
target_offset_t SegmentAlignment(   // SEGMENT: ALIGNMENT FOR SYMBOL
    SYMBOL sym )                    // - symbol to align
;
#if _INTEL_CPU
void* SegmentBoundReg(          // GET REGISTER BOUND TO SEGMENT
    fe_seg_id seg_id )          // - segment id
;
#endif
void SegmentCgDefineCode(       // DEFINE CODE SEGMENT IF REQ'D
    fe_seg_id segid )           // - segment id
;
boolean SegmentIfBasedOK(       // GIVE ERROR IF BASED SEGMENT IS NOT A CODE SEGMENT
    SYMBOL func )               // - symbol that may be based
;
void SegmentCgInit(             // INITIALIZE SEGMENTS FOR CODE-GENERATION
    void )
;
void SegmentCode(               // SET DEFAULT CODE SEGMENT
    char *segname,              // - segment name
    char *segclass )            // - segment class name
;
void SegmentCodeCgInit(         // TURN OFF USED BIT FOR ALL CODE SEGMENTS
    void )
;
void SegmentData(               // SET DEFAULT DATA SEGMENT
    char *segname,              // - segment name
    char *segclass )            // - segment class name
;
fe_seg_id SegmentDefaultCode(   // GET CURRENT DEFAULT CODE SEGMENT
    void )
;
fe_seg_id SegmentFindBased(     // FIND SEGMENT ID FOR TF1_BASED_STRING
    TYPE base_mod )             // - base modifier
;
fe_seg_id SegmentFindNamed(     // FIND SEGMENT ENTRY FOR NAME
    char* segname )             // - segment name
;
void SegmentFini(               // SEGMENT: COMPLETION
    void )
;
fe_seg_id SegmentForDefinedFunc(// GET SEGMENT FOR A DEFINED FUNCTION
    SYMBOL func )               // - function
;
fe_seg_id SegmentImport(        // GET NEXT IMPORT SEGMENT #
    void )
;
void SegmentInit(               // SEGMENT: INITIALIZATION
    char *code_seg_name )       // - name of data segment
;
SYMBOL SegmentLabelGen(         // GENERATE SEGMENT LABEL IF REQ'D
    fe_seg_id seg_id )          // - segment id
;
SYMBOL SegmentLabelStackReset(  // RESET STACK-SEGMENT LABEL
    void )
;
SYMBOL SegmentLabelSym(         // GET LABEL IN SEGMENT
    fe_seg_id seg_id )          // - segment id
;
void SegmentMarkRoBlock(        // INDICATE R/O BLOCK TO BE GENERATED
    void )
;
void SegmentMarkUsed(           // MARK SEGMENT AS BEING USED
    fe_seg_id segid )           // - XI, YI segment id
;

#endif
