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


#include "cpplib.h"
#include "rtexcept.h"
#include "rtinit.h"

#if 0


static unsigned fs_root_handler // HANDLER FOR FS ROOT
    ( FsExcRec* rec_exc         // - exception record
    , RW_DTREG*                 // - current R/W block
    , FsCtxRec*                 // - context record
    , unsigned                  // - dispatch context
    ... )
{
    unsigned retn;              // - return code
    if( rec_exc->flags & EXC_TYPE_UNWINDING ) {
        retn = EXC_HAND_CONTINUE;
    } else if( EXCREC_CODE_WATCOM != ( EXCREC_CODE_WATCOM & rec_exc->code ) ) {
        rec_exc->dispatch->system_exc = rec_exc->code;
        rec_exc->dispatch->type = DISPATCHABLE_SYS_EXC;
        retn = EXC_HAND_CATCH;
    } else {
        rec_exc->dispatch->type = DISPATCHABLE_NO_CATCH;
        retn = EXC_HAND_CATCH;
    }
    return retn;
}


// This entry is located on the FS chain before any others linked on by
// the C++ run-time system
//
static RW_DTREG_ROOT root_entry =// ROOT FS ENTRY
    {   0
    ,   &fs_root_handler
    ,   &fs_root_handler
    ,   0
    };

#endif


// assumptions: the following have occurred by the time fs_root is
//              invoked:
//
//  (1) fs has been set up by operating system
//  (2) thread-ctl stuff has been set up by CLIB initialization
//  (3) executes only in .EXE (not in .DLL)
//  (4) C++ run-time has not linked any others onto the chain
//


static void fs_root             // ESTABLISH ROOT ENTRY
    ( void )
{
    THREAD_CTL* thr = &_RWD_ThreadData;
    thr->flags.executable = true;
}


extern "C" XI( CPPLIBDATA( init_fs_root ), fs_root, INIT_PRIORITY_RUNTIME+1 )
