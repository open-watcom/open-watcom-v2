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


#ifndef __CGBACK_H
#define __CGBACK_H

// PROTOTYPES FOR BACK-END
//
// 91/09/26 -- J.W.Welch    -- defined

#include "cgiobuff.h"
#include "cgio.h"
#include "module.h"
#include "segment.h"

#define DEFAULT_INLINE_DEPTH    3
#define MAX_INLINE_DEPTH        255

void CgBackDtorAutoOffset(      // SAVE OFFSET OF DTOR SYMBOL
    SYMBOL sym,                 // - symbol
    unsigned offset )           // - offset
;
void CgBackEnd(                 // BACK-END CONTROLLER
    void )
;
void CgBackFreeFileHandles(     // FREE HANDLES FOR FILE SCOPE
    void )
;
void CgBackFreeHandle(          // FREE A BACK HANDLE FOR A SYMBOL
    SYMBOL sym )                // - the symbol
;
void CgBackFreeIndCall(         // FREE INDIRECT-CALL SYMBOL
    SYMBOL sym )                // - symbol to be freed
;
void CgBackFreeIndCalls(        // FREE ALL INDIRECT-CALL SYMBOLS
    void )
;
bool CgBackFuncInlined(         // DETERMINE IF FUNCTION INVOCATION INLINED
    SYMBOL sym )                // - function symbol
;
void CgBackGenLabel(            // GENERATE A LABEL
    SYMBOL sym )                // - symbol defining label
;
void CgBackGenLabelInternal(    // GENERATE A LABEL FOR INTERNAL STRUCTURE
    SYMBOL sym )                // - symbol defining label
;
void CgBackSetInlineDepth(      // SET MAXIMUM INLINE DEPTH
    unsigned max_depth )        // - the maximum
;
unsigned CgBackGetInlineDepth(  // GET MAXIMUM INLINE DEPTH
    void )
;
void CgBackSetInlineRecursion(  // SET INLINE RECURSION
    bool allowed )              // - TRUE ==> inline recursion allowed
;
bool CgBackGetInlineRecursion(  // GET INLINE RECURSION
    void )
;
void CgBackPopBlocks(           // POP BLOCKS
    unsigned count )            // - number of blocks to pop
;
void CgBackSetOeSize(           // SET SIZE FOR INLINING STATICS
    unsigned value )            // - size specified with -oe
;
void CgBackStatHandleAdd(       // ADD SYMBOL TO STATIC HANDLES
    SYMBOL sym )                // - symbol to be added
;
void CgBackStatHandlesFini(     // INITIALIZE FOR STATIC HANDLES
    void )
;
void CgBackStatHandlesInit(     // INITIALIZE FOR STATIC HANDLES
    void )
;
target_size_t CgCodePtrSize(    // SIZE OF DEFAULT CODE POINTER
    void )
;
target_size_t CgDataPtrSize(    // SIZE OF DEFAULT DATA POINTER
    void )
;
cg_type CgTypeOffset(          // GET CODEGEN TYPE FOR AN OFFSET
    void )
;
SYMBOL DtorFindCg(              // FIND DTOR, DURING CODE-GEN
    TYPE type )                 // - a class or array class type
;

#endif
