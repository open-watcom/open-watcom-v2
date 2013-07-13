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
#include "cgback.h"
#include "codegen.h"
#include "cgbackut.h"
#include "vstk.h"
#include "context.h"
#include "toggle.h"
#include "initdefs.h"

#ifndef NDEBUG
    #include <stdio.h>
    #include "dbg.h"
#endif


static VSTK_CTL stack_files;        // stack: files
static unsigned cond_flags_offset;  // offset to be added to conditional flags


FN_CTL *FnCtlPush(              // PUSH FILE CONTROL
    call_handle handle,         // - handle for IBRP substitution
    CGFILE *cgfile )            // - fn's CGFILE
{
    FN_CTL *fctl;               // - file control
    unsigned cd_arg;            // - integral cdtor arg
    boolean has_cdtor;          // - TRUE ==> has inlined CDTOR argument

    fctl = VstkTop( &stack_files );
    if( fctl == NULL ) {
        has_cdtor = FALSE;
    } else {
        cond_flags_offset += fctl->cond_flags;
        has_cdtor = CallStabCdArgGet( handle, &cd_arg );
    }
    fctl = VstkPush( &stack_files );
    // CgBackFnCtlInit inits:
    // fctl->base_goto_near
    // fctl->base_labs_cs
    // fctl->sym_trans_id
    fctl = CgBackFnCtlInit( fctl );
    fctl->cond_flags     = 0;
    // fctl->cdtor_val   (protected by has_cdtor_val)
    fctl->cond_next      = 0;
    fctl->try_depth      = 0;
    fctl->return_symbol  = NULL;
    fctl->this_sym       = NULL;
    fctl->cdtor_sym      = NULL;
    fctl->func           = NULL;
    fctl->new_ctor_ptr   = NULL;
    fctl->return_label   = UNDEFINED_LABEL;
    fctl->cdarg_lab      = UNDEFINED_LABEL;
    fctl->try_label      = UNDEFINED_LABEL;
    fctl->handle         = handle;
    fctl->prof_data      = NULL;
    fctl->state_table_bound = NULL;
    fctl->pre_init       = NULL;
    fctl->fun_sv         = NULL;
    fctl->ctor_components = NULL;
    fctl->dtor_components = NULL;
    fctl->ctored_obj     = NULL;
    fctl->marked_at_start = FstabMarkedPosn();
    fctl->obj_registration = NULL;
    fctl->expr_calls     = NULL;
    fctl->cgfile         = cgfile;
    fctl->dtor_method    = DTM_DIRECT;
    fctl->func_dtor_method = DTM_DIRECT;

    fctl->deregistered   = FALSE;
    fctl->has_fn_exc     = FALSE;
    fctl->is_ctor        = FALSE;
    fctl->is_dtor        = FALSE;
    fctl->ctor_complete  = FALSE;
    fctl->coded_return   = FALSE;
    fctl->has_ctor_test  = FALSE;
    fctl->has_cdtor_val  = FALSE;
    fctl->temp_dtoring   = FALSE;
    fctl->ctor_test      = FALSE;
    fctl->dtor_reg_reqd  = FALSE;
    fctl->debug_info     = ( 0 == CgBackInlinedDepth() )
                         && ( GenSwitches
                            & ( NUMBERS | DBG_TYPES | DBG_LOCALS ) );
    if( has_cdtor ) {
        fctl->cdtor_val = cd_arg;
        fctl->has_cdtor_val = TRUE;
    }
    return( fctl );
}


void FnCtlPop(                  // POP FILE CONTROL
    void )
{
    FN_CTL *fctl;               // - file control
    FN_CTL *prev;               // - previous file control

    fctl = VstkPop( &stack_files );
    fctl = CgBackFnCtlFini( fctl );
    prev = FnCtlTop();
    if( prev != NULL ) {
        CtxFunction( prev->func );
        cond_flags_offset -= prev->cond_flags;
    }
}


FN_CTL* FnCtlTop(               // GET TOP FN_CTL ITEM
    void )
{
    return VstkTop( &stack_files );
}


FN_CTL* FnCtlPrev(              // GET PREVIOUS FN_CTL ITEM
    FN_CTL* curr )              // - current file control
{
    return VstkNext( &stack_files, curr );
}


unsigned FnCtlCondFlagExpr(     // START FLAGS OFFSET FOR EXPRESSION
    FN_CTL *fctl )              // - current function information
{
    unsigned start;             // - start flag # for expression

    start = cond_flags_offset;
    if( fctl->has_ctor_test ) {
        ++ start;
    }
    fctl->cond_next = start;
    DbgVerify( start <= cond_flags_offset + fctl->cond_flags
             , "FnCtlCondFlagExpr -- flags overflow" );
    return start;
}


unsigned FnCtlCondFlagCtor(     // GET FLAG # FOR CTOR-TEST
    FN_CTL *fctl )              // - current function information
{
    fctl = fctl;
    DbgVerify( fctl->has_ctor_test, "FnCtlCondFlagCtor -- ! has_ctor_test" );
    return cond_flags_offset;
}


unsigned FnCtlCondFlagNext(     // GET NEXT FLAG OFFSET FOR EXPRESSION
    FN_CTL *fctl )              // - current function information
{
    DbgVerify( fctl->cond_next < cond_flags_offset + fctl->cond_flags
             , "FnCtlCondFlagNext -- flags overflow" );
    return fctl->cond_next++;
}


SYMBOL FnCtlNewCtorPtr(         // GET SYMBOL FOR NEW-CTORED PTR.
    FN_CTL *fctl )              // - current function information
{
    SYMBOL new_ctor_ptr = fctl->new_ctor_ptr;
    if( new_ctor_ptr == NULL ) {
        new_ctor_ptr = CgVarRw( CgbkInfo.size_data_ptr, SC_AUTO );
        new_ctor_ptr->sym_type = TypePtrToVoid();
        fctl->new_ctor_ptr = new_ctor_ptr;
    }
    return new_ctor_ptr;
}


static void fnCtlInit(          // INITIALIZATION FOR CGBKFCTL
    INITFINI* defn )            // - definition
{
    defn = defn;
    VstkOpen( &stack_files, sizeof( FN_CTL ), 4 );
}


static void fnCtlFini(          // COMPLETION FOR CGBKFCTL
    INITFINI* defn )            // - definition
{
    defn = defn;
    VstkClose( &stack_files );
}


INITDEFN( fn_ctl, fnCtlInit, fnCtlFini )


#ifndef NDEBUG

#include "module.h"

void FnctlDump()                // DEBUGGING -- dump stack
{
    FN_CTL *fctl;               // - current function information

    for( fctl = VstkTop( &stack_files )
       ; fctl != NULL
       ; fctl = VstkNext( &stack_files, fctl ) ) {
        VBUF vbuf;
        SYMBOL func = fctl->func;
        if( func == NULL ) {
            func = ModuleInitFuncSym();
        }
        printf( "FN_CTL[%x] %s\n"
                "  bases: labs_cs(%u) goto_near(%u)\n"
                "  try_depth(%u) try_label(%p)\n"
                "  sym_trans_id(%d) symbols: return(%x) this(%x) cdtor(%x)\n"
                "  handle(%x) state_table_bound(%x) pre_init(%x) cond_flags(%x)\n"
                "  fun_sv(%x) expr_calls(%x) dtor_method(%d) cdtor_val(%x) ctor_components(%x)\n"
                "  cond_next(%d) ctored_obj(%x) new_ctor_ptr(%x)\n"
                "  cdarg_lab(%x) func_dtor_method(%d)\n"
                "flags:\n"
                "  deregistered(%d) has_fn_exc(%d) is_ctor(%d) is_dtor(%d) ctor_complet(%d)\n"
                "  coded_return(%d) has_cdtor_val(%d) temp_dtoring(%d) ctor_test(%d)\n"
                "\n"
              , fctl
              , DbgSymNameFull( func, &vbuf )
              , fctl->base_labs_cs
              , fctl->base_goto_near
              , fctl->try_depth
              , fctl->try_label
              , fctl->sym_trans_id
              , fctl->return_symbol
              , fctl->this_sym
              , fctl->cdtor_sym
              , fctl->handle
              , fctl->state_table_bound
              , fctl->pre_init
              , fctl->cond_flags
              , fctl->fun_sv
              , fctl->expr_calls
              , fctl->dtor_method
              , fctl->cdtor_val
              , fctl->ctor_components
              , fctl->cond_next
              , fctl->ctored_obj
              , fctl->new_ctor_ptr
              , fctl->cdarg_lab
              , fctl->func_dtor_method
              , fctl->deregistered
              , fctl->has_fn_exc
              , fctl->is_ctor
              , fctl->is_dtor
              , fctl->ctor_complete
              , fctl->coded_return
              , fctl->has_cdtor_val
              , fctl->temp_dtoring
              , fctl->ctor_test
              );
        VbufFree( &vbuf );
    }
}

#endif
