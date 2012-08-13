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


extern "C"
THREAD_CTL* CPPLIB( base_register )( // REGISTRATION OF BASE
    RW_DTREG* rw,               // - R/W element
    RO_DTREG* ro,               // - R/O element
    RT_STATE_VAR state_var )    // - initial state variable
{
    THREAD_CTL *thr;            // - thread control

    rw->base.ro = ro;
    rw->base.state_var = state_var;
    thr = PgmThread();
#ifdef RW_REGISTRATION
    rw->base.handler = & CPPLIB( fs_handler_rtn );
#endif
    return RwRegister( thr, rw );
}


extern "C"
_WPRTLINK
#if 0
THREAD_CTL* CPPLIB( base_deregister )( // DE-REGISTRATION OF BASE
#else
void CPPLIB( base_deregister )( // DE-REGISTRATION OF BASE
#endif
    void )
{
#ifdef FS_REGISTRATION
    FsPop();
#else
    THREAD_CTL *thr;            // - thread control

    thr = PgmThread();
    (void) (RwDeregister( thr ));
#endif
}
