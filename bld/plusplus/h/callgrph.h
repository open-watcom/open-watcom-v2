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


#ifndef __CALLGRPH_H__
#define __CALLGRPH_H__

// CALLGRPH.H -- call graph processing

#include "dirgraph.h"
#include "vstk.h"
#include "carve.h"
#include "typesig.h"

typedef struct callgraph CALLGRAPH;
typedef struct calledge  CALLEDGE;
typedef struct callnode  CALLNODE;
typedef struct inlinee   INLINEE;
#ifndef CGFILE_DEFINED
#define CGFILE_DEFINED
typedef struct cgfile    CGFILE;
#endif

typedef enum                    // SCOPE_STATE
{   STS_CALL        = 0x01      // - "can-throw" call
,   STS_DTOR_TMP_NT = 0x02      // - dtor temporary (non-throw)
,   STS_DTOR_TMP_CT = 0x04      // - dtor temporary (can-throw)
,   STS_DTOR_BLK_NT = 0x20      // - dtor scope (non-throw)
,   STS_DTOR_BLK_CT = 0x40      // - dtor scope (can-throw)
,   STS_GEN         = 0x80      // - bookkeeping required for scope
,   STS_NONE        = 0         // - nothing

                                // derived values

,   STS_DTORABLE = 0            // - scope has dtorable object already
              | STS_DTOR_TMP_NT
              | STS_DTOR_TMP_CT
              | STS_DTOR_BLK_NT
              | STS_DTOR_BLK_CT
,   STS_SCOPE = STS_CALL        // - scope states
              | STS_DTOR_BLK_NT
              | STS_DTOR_BLK_CT
,   STS_STMNT = STS_CALL        // - statement states
              | STS_DTOR_TMP_NT
              | STS_DTOR_TMP_CT
} SCOPE_STATE;

typedef struct call_info CALL_INFO;
struct call_info                // CALL_INFO -- SF_LONGJUMP resolution info
{   CALL_INFO* next;            // - - next in list
    CALLNODE* caller;           // - - calling node
    SYMBOL callee;              // - - called function
};

struct callgraph                // CALLGRAPH INFORMATION
{
    DIRGRAPH_CTL base;          // - directed-graph information
    VSTK_CTL calls;             // - used to stack calls
    carve_t carve_nodes;        // - carving for nodes
    carve_t carve_edges;        // - carving for call edges
    unsigned incr_refs;         // - amount to increment refs
    unsigned incr_addrs;        // - amount to increment addrs
    unsigned depth;             // - current call depth
    unsigned pruned     :1;     // - TRUE ==> has been pruned
    unsigned assumed_longjump:1;// - TRUE ==> incomplete SF_LONGJUMP info
    unsigned does_longjump  :1; // - TRUE ==> does a longjump
    unsigned scope_call_opt :1; // - TRUE ==> scope-call optimization enabled
    unsigned push_unresolved :1;// - TRUE ==> push unresolved function
    unsigned stmt_scope      :1;// - TRUE == statement scope open
#ifndef NDEBUG
    unsigned doing_rescan    :1;// - TRUE ==> (debug only) rescanning
#endif
    CALLNODE* curr_node;        // - current node being processed
    VSTK_CTL scopes;            // - used to stack scopes in routine
    VSTK_CTL dtors;             // - used to stack dtors in a scope
    VSTK_CTL rescan;            // - used to stack functions for re-scanning
    carve_t call_nodes;         // - carving for CALL_INFO
    CALLNODE* scanning;         // - node being scanned
    CALL_INFO* unresolved;      // - unresolved calls
};

typedef struct unr_usage        UNR_USAGE;          // unresolved usage

struct callnode                 // CALLNODE -- node in call graph
{
    DIRGRAPH_NODE base;         // - node in directed graph
    CGFILE * cgfile;            // - CGFILE for node
    unsigned refs;              // - # references
    unsigned addrs;             // - number of times address taken
    unsigned depth;             // - minimum call depth when inline
    unsigned opcodes;           // - # of opcodes in file
    unsigned cond_flags;        // - # conditional flags
    UNR_USAGE* unresolved;      // - unresolved usages of function
    SCOPE_STATE stmt_state;     // - state for "statement" scope
    DT_METHOD dtor_method;      // - specified dtor method for function
    uint_8 state_table      :1; // - TRUE ==> state table for function
    uint_8 calls_done       :1; // - TRUE ==> fn has been scanned for calls
    uint_8 is_vft           :1; // - TRUE ==> represents VFT
    uint_8 inline_fun       :1; // - TRUE ==> is inline function
    uint_8 inlineable       :1; // - TRUE ==> able to be inlined
    uint_8 inlineable_oe    :1; // - TRUE ==> able to be -oe inlined
    uint_8 rescan           :1; // - TRUE ==> has to be re-scanned
    uint_8 stab_gen         :1; // - TRUE ==> need to generate state table
    uint_8 flowed_recurse   :1; // - TRUE ==> flowed a recursive call
    PAD_UNSIGNED
};

struct calledge                 // CALLEDGE -- edge in call graph
{
    DIRGRAPH_EDGE base;         // - edge in directed graph
    unsigned refs;              // - number of calls from function
    unsigned addrs;             // - number of times address taken
};


struct inlinee                  // INLINEE -- stacked inline reference
{
    CALLNODE *callee;           // - function called
    unsigned expanded   :1;     // - expanded the function
};

// PROTOTYPES

void CgrfDump(                  // DUMP CALL GRAPH
    CALLGRAPH *ctl )            // - call graph information
;
void CgrfFini(                  // FINALIZE FOR CALL-GRAPHING
    CALLGRAPH *ctl )            // - call graph information
;
void CgrfInit(                  // INITIALIZE FOR CALL-GRAPHING
    CALLGRAPH *ctl )            // - call graph information
;
void CgrfAddAddrOf(             // ADD AN ADDRESS OF
    CALLGRAPH *ctl,             // - call graph information
    CALLNODE *node_src,         // - node for caller
    CALLNODE *node_tgt )        // - node for callee
;
void CgrfAddCall(               // ADD A CALL
    CALLGRAPH *ctl,             // - call graph information
    CALLNODE *node_src,         // - node for caller
    CALLNODE *node_tgt )        // - node for callee
;
CALLNODE *CgrfAddFunction(      // ADD A FUNCTION
    CALLGRAPH *ctl,             // - call graph information
    SYMBOL func )               // - function
;
CALLNODE* CgrfCallNode(         // GET CALLNODE FOR FUNCTION
    SYMBOL fun )                // - function
;
CALLNODE* CgrfDtorAddr(         // DTOR ADDR-OF HAS BEEN ESTABLISHED
    CALLNODE* owner,            // - owner
    SYMBOL dtor )               // - dtor called
;
CALLNODE* CgrfDtorCall(         // DTOR CALL HAS BEEN ESTABLISHED
    CALLNODE* owner,            // - owner
    SYMBOL dtor )               // - dtor called
;
void CgrfMarkNodeGen(           // MARK NODE AS GEN-ED, IF REQ'D
    CALLNODE* cnode )           // - the node
;
void CgrfPruneFunction(         // PRUNE FUNCTION (AND CALLS) FROM GRAPH
    CALLGRAPH *ctl,             // - call graph information
    CALLNODE *node )            // - node for function
;
boolean CgrfWalkCalls(          // WALK CALLS FROM NODE IN GRAPH
    CALLGRAPH *ctl,             // - call graph information
    CALLNODE *node,             // - source node
    boolean (*walker)           // - walking routine
        ( CALLGRAPH *           // - - control information
        , CALLEDGE * ) )        // - - edge
;
void CgrfWalkFunctions(         // WALK FUNCTIONS IN GRAPH
    CALLGRAPH *ctl,             // - call graph information
    boolean (*walker)           // - walking routine
        ( CALLGRAPH *           // - - control information
        , CALLNODE * ) )        // - - function
;

#endif
