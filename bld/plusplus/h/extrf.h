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


#ifndef __EXTRF_H__
#define __EXTRF_H__

#include "carve.h"
#include "vstk.h"

// EXTRF.H -- definitions use to resolve external references
//
// 93/02/04 -- J.W.Welch        -- defined for PC

// Summary of attributes:
//   WEAK -- can be undef'd and won't search libs
//   LAZY -- can be undef'd but will search libs for it
//   HARD -- must be def'd and will search libs for it

typedef enum {                  // EXTRF_TYPE -- type of reference
    EXTRF_DATA_WEAK,            // - can be undef'd (will not search libs)
    EXTRF_FN_WEAK,              // - can be undef'd (will not search libs)
    EXTRF_FN_LAZY,              // - can be undef'd (will search libs)
    EXTRF_VFN_CONDITIONAL,      // - virtual conditional (normal)
    EXTRF_PURE_VFN_CONDITIONAL, // - virtual conditional (pure)
    EXTRF_UNKNOWN
} EXTRF_TYPE;

typedef struct {                // EXTRF -- data for reference resolution
    void *syms;                 // - symbols when EXTRF_VIRTUAL_...
    SYMBOL sym;                 // - original symbol
    EXTRF_TYPE type;            // - type of resolution
} EXTRF;


// PROTOTYPES : PC

void *ExtrefImportType(         // GET NEXT IMPORT TYPE FOR A SYMBOL
    EXTRF *rinfo )              // - resolution information
;
void *ExtrefResolve(            // DETERMINE RESOLUTION FOR A SYMBOL
    SYMBOL sym,                 // - symbol
    EXTRF *rinfo )              // - resolution information
;
void *ExtrefVfunInfo(           // GET INFORMATION FOR VIRTUAL FUN. REFERENCE
    SYMBOL sym )                // - symbol used in indirect call
;
void *ExtrefVfunSym(            // GET ORIGNATING FUN. FOR VIRTUAL CALL
    void *funs )                // - originating functions
;
void *ExtrefNextVfunSym(         // MOVE TO NEXT ORIGNATING FUN. FOR VIRTUAL CALL
    void *funs )                // - originating functions
;
void *ExtrefVirtualSymbol(      // GET NEXT DEPENDENCY SYMBOL
    EXTRF *rinfo )              // - resolution information
;


#endif
