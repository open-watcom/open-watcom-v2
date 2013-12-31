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


#ifndef __PTREEDEC_H__
#define __PTREEDEC_H__

// PtreeDec.h -- PTREE decoration
//
// A PTREE node can have a ring of PTD entries linked from it.
//
// The entries are used to specify additional information for the node.
// The order is not important.
//
// 94/11/10 -- J.W.Welch        -- defined

typedef union ptd PTD;

// PROTOTYPES

PTREE PtdCompCtored             // DECORATE INDICATING COMPONENT CTORED
    ( PTREE expr                // - expression
    , target_offset_t offset    // - offset
    , DTC_KIND kind )           // - kind of component
;
PTREE PtdCtoredComponent        // CTORED COMPONENT (BASE OR ELEMENT)
    ( PTREE expr                // - expression
    , TYPE type )               // - type of component
;
PTREE PtdCtoredExprType         // DECORATE TEMP AFTER CTORING
    ( PTREE expr                // - expression
    , SYMBOL ctor               // - ctor used
    , TYPE cl_type )            // - class type
;
PTREE PtdCtoredScopeType        // DECORATE AUTO AFTER CTORING
    ( PTREE expr                // - expression
    , SYMBOL ctor               // - ctor used
    , TYPE cl_type )            // - class type
;
PTREE PtdDltDtorArr             // DECORATE FOR DELETE OF DTORABLE-ARRAY
    ( PTREE expr                // - expression
    , SYMBOL del )              // - operator delete
;
PTREE PtdDltDtorElm             // DECORATE FOR DELETE OF DTORABLE-ELEMENT
    ( PTREE expr                // - expression
    , SYMBOL del )              // - operator delete
;
PTREE PtdDltDtorEnd             // DECORATE FOR END OF DTORABLE-ELEMENT ON DEL
    ( PTREE expr )              // - expression
;
PTREE PtdDltDtorSize            // DECORATE FOR SIZE OF DTORABLE-ELEMENT
    ( PTREE expr                // - expression
    , target_size_t size )      // - size
;
#if 0
PTREE PtdDtorRefType            // DECORATE WITH DTOR REFERENCE (TYPE)
    ( PTREE expr                // - expression
    , TYPE type )               // - dtor type
;
#endif
PTREE PtdDtorRefSym             // DECORATE WITH DTOR REFERENCE (SYMBOL)
    ( PTREE expr                // - expression
    , SYMBOL dtor )             // - destructor symbol
;
PTREE PtdDtorScopeSym           // DTOR OF SCOPE (SYMBOL)
    ( PTREE expr                // - expression
    , SYMBOL dtor )             // - destructor
;
PTREE PtdDtorScopeType          // DTOR OF SCOPE (TYPE)
    ( PTREE expr                // - expression
    , TYPE type )               // - type dtored
;
PTREE PtdDtorUseSym             // DECORATE WITH DTOR USAGE (SYMBOL)
    ( PTREE expr                // - expression
    , SYMBOL dtor )             // - destructor symbol
;
PTD* PtdDuplicate               // DUPLICATE DECORATION
    ( PTREE src )               // - source PTREE
;
PTD* PtdDuplicateReloc          // DUPLICATE DECORATION, RELOC SYMBOLS
    ( PTREE src                 // - source PTREE
    , RELOC_LIST *reloc_list )  // - relocation list
;
PTREE PtdExprConst              // DECORATE A CONSTANT EXPRESSION
    ( PTREE expr )              // - expression
;
PTREE PtdFree                   // FREE ENTRIES FOR A PARSE TREE NODE
    ( PTREE expr )              // - parse tree node
;
void PtdGenAfter                // GENERATE AFTER NODE PROCESSED
    ( PTD* ring )               // - ring of entries
;
void PtdGenBefore               // GENERATE BEFORE NODE PROCESSED
    ( PTD* ring )               // - ring of entries
;
target_offset_t PtdGetVbExact   // GET VBASE EXACT DECORATION
    ( PTREE expr )              // - expression
;
target_offset_t PtdGetVbOffset  // GET VBASE OFFSET DECORATION
    ( PTREE expr )              // - expression
;
PTREE PtdInitSymEnd             // DECORATE FOR END OF SYMBOL INITIALIZATION
    ( PTREE expr                // - expression
    , SYMBOL sym )              // - symbol
;
PTREE PtdInsert                 // INSERT A DECORATION RING
    ( PTREE expr                // - expression
    , PTD* ring )               // - ring of decoration
;
PTREE PtdNewAlloc               // DECORATE NEW ALLOCATION
    ( PTREE expr )              // - expression
;
PTREE PtdNewCtor                // DECORATE FOR CTORING NEW'ED EXPRESSION
    ( PTREE expr                // - expression
    , TYPE type )               // - class type
;
PTREE PtdObjPop                 // DECORATE FOR END OF DTORABLE OBJECT
    ( PTREE expr )              // - expression
;
PTREE PtdObjPush                // DECORATE FOR START OF DTORABLE OBJECT
    ( PTREE expr                // - expression
    , TYPE type                 // - type of object
    , SYMBOL sym                // - NULL or symbol
    , target_offset_t offset )  // - offset
;
PTREE PtdRetnOptEnd             // DECORATE WITH IC_RETNOPT_END
    ( PTREE expr )              // - expression
;
PTREE PtdRetnOptVar             // DECORATE WITH IC_RETNOPT_VAR
    ( PTREE expr                // - expression
    , SYMBOL var )              // - symbol which could be optimized away
;
PTREE PtdScopeCall              // SCOPE-CALL RECORDING
    ( PTREE expr                // - expression
    , SYMBOL fun )              // - function/ctor called
;
PTREE PtdThrow                  // DECORATE TO INDICATE THROW (OR EQUIVALENT)
    ( PTREE expr )              // - expression
;
PTREE PtdOffsetofExpr           // DECORATE FOR OFFSETOF EXPR
    ( PTREE expr                // - expression
    , PTREE tree )              // - operator delete
;
PTREE PtdVbaseFetch             // FETCH OF VIRTUAL BASE
    ( PTREE expr                // - expression
    , target_offset_t vb_offset // - virtual base offset
    , vindex vb_index           // - virtual base index
    , target_offset_t vb_delta  // - virtual base delta
    , target_offset_t vb_exact )// - virtual base exact offset
;
PTREE PtdVfunAccess             // ACCESS VIRTUAL FUNCTION
    ( PTREE expr                // - expression
    , vindex vf_index           // - index into VF table
    , target_offset_t vf_offset // - offset to VF table ptr
    , SYMBOL baser )            // - basing "this" symbol
;
PTREE PtdGetOffsetofExpr(       // GET OFFSETOF EXPR
    PTREE expr )                // - expr
;

extern PTD *PtdGetIndex( PTD *e );
extern PTD *PtdMapIndex( PTD *e );

#ifndef NDEBUG

// PROTOTYPES (Debugging only)

void PtdPrint(                  // DEBUG: print decoration for a node
    int numb,                   // - number for node
    PTREE node )                // - the node
;


#endif
#endif
