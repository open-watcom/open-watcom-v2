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
#include "cgfront.h"
#include "cgback.h"
#include "memmgr.h"
#include "errdefns.h"
#include "codegen.h"
#include "cgbackut.h"
#include "ring.h"
#include "toggle.h"
#include "vstk.h"
#include "carve.h"
#include "initdefs.h"

typedef struct call_stk CALL_STK;
struct call_stk                 // CALL_STK -- call stack
{   call_handle handle;         // - handle for call
    SYMBOL func;                // - NULL or symbol for scheduled inline call
    target_offset_t adj_this;   // - vfun: "this" adjustment
    target_offset_t adj_retn;   // - vfun: return adjustment
    cg_type retn_type;          // - cg type of call
};

static VSTK_CTL stack_calls;    // stack: calls (executed)
static SYMBOL ind_call_stack;   // stack: indirect-calls (allocated)
static SYMBOL ind_call_free;    // stack: indirect-calls (popped)


void CallStackPush(             // PUSH CALL STACK
    SYMBOL func,                // - NULL, or inlined function
    call_handle handle,         // - handle for call
    cg_type cg_retn )           // - cg type of call
{
    CALL_STK* curr;             // - entry on call stack

    curr = VstkPush( &stack_calls );
    curr->func = func;
    curr->handle = handle;
    curr->adj_this = 0;
    curr->adj_retn = 0;
    curr->retn_type = cg_retn;
}


call_handle CallStackPop(       // POP CALL STACK
    void )
{
    return ((CALL_STK*)VstkPop( &stack_calls ))->handle;
}


boolean CallStackTopInlined(    // TEST IF TOP OF CALL STACK IS INLINED
    void )
{
    return NULL != ((CALL_STK*)VstkTop( &stack_calls ))->func;
}


call_handle CallStackTopHandle( // GET HANDLE FOR TOP OF CALL STACK
    void )
{
    return ((CALL_STK*)VstkTop( &stack_calls ))->handle;
}


SYMBOL CallStackTopFunction(    // GET FUNCTION FOR TOP OF CALL STACK
    void )
{
    return ((CALL_STK*)VstkTop( &stack_calls ))->func;
}


target_offset_t CallStackThisAdj( // GET "THIS" ADJUSTMENT FOR VIRTUAL CALL
    void )
{
    return ((CALL_STK*)VstkTop( &stack_calls ))->adj_this;
}


target_offset_t CallStackRetnAdj( // GET RETURN ADJUSTMENT FOR VIRTUAL CALL
    void )
{
    return ((CALL_STK*)VstkTop( &stack_calls ))->adj_retn;
}


target_offset_t CallStackRetnType( // GET RETURN TYPE FOR CALL
    void )
{
    return ((CALL_STK*)VstkTop( &stack_calls ))->retn_type;
}


SYMBOL CallIndirectPush(        // PUSH SYMBOL FOR INDIRECT CALL
    TYPE type )                 // - expression type
{
    SYMBOL sym;                 // - symbol

    sym = AllocSymbol();
    sym->thread = ind_call_stack;
    ind_call_stack = sym;
    sym->sym_type = type;
    return sym;
}


void CallIndirectVirtual(       // MARK INDIRECT CALL AS VIRTUAL
    SYMBOL vfunc,               // - the virtual function
    boolean is_virtual,         // - TRUE ==> an actual virtual call
    target_offset_t adj_this,   // - adjustment for "this"
    target_offset_t adj_retn )  // - adjustment for return
{
    CALL_STK* cstk;             // - top of call stack
    SYMBOL virt_fun;            // - dummy symbol for virtual call

    vfunc = vfunc;
    if( is_virtual ) {
        virt_fun = ind_call_stack;
        virt_fun->id = SC_VIRTUAL_FUNCTION;
        virt_fun->u.virt_fun = vfunc;
    } else {
        cstk = VstkTop( &stack_calls );
        DbgVerify( cstk != NULL, "CallIndirectVirtual -- no call stack" );
        cstk->adj_this = adj_this;
        cstk->adj_retn = adj_retn;
    }
}


void CallIndirectPop(           // POP AN INDIRECT CALL ENTRY
    void )
{
    SYMBOL sym;                 // - top of indirect-call stack

    sym = ind_call_stack;
    ind_call_stack = sym->thread;
    sym->thread = ind_call_free;
    ind_call_free = sym;
}


void CgBackFreeIndCall(         // FREE INDIRECT-CALL SYMBOL
    SYMBOL sym )                // - symbol to be freed
{
#if 1
    sym = sym;
#else
    SYMBOL curr;                // - current symbol in free list
    SYMBOL prev;                // - previous symbol

    for( prev = NULL, curr = ind_call_free
       ; curr != sym
       ; prev = curr, curr = curr->thread ) {
        DbgVerify( curr != NULL, "CgBackFreeIndCall: can't find symbol" );
    }
    curr = curr->thread;
    if( prev == NULL ) {
        ind_call_free = curr;
    } else {
        prev->thread = curr;
    }
    FreeSymbol( sym );
#endif
}


void CgBackFreeIndCalls(        // FREE ALL INDIRECT-CALL SYMBOLS
    void )
{
    SYMBOL curr;                // - current symbol
    SYMBOL next;                // - next symbol

    for( curr = ind_call_free; curr != NULL; curr = next ) {
        next = curr->thread;
        FreeSymbol( curr );
    }
    ind_call_free = NULL;
}


static void callStackInit(      // INITIALIZE CGBKCSTK
    INITFINI* defn )            // - definition
{
    defn = defn;
    ind_call_stack = NULL;
    ind_call_free = NULL;
    VstkOpen( &stack_calls, sizeof( CALL_STK ), 4 );
}


static void callStackFini(      // COMPLETE CGBKCSTK
    INITFINI* defn )            // - definition
{
    defn = defn;
    VstkClose( &stack_calls );
}


INITDEFN( call_stack, callStackInit, callStackFini )
