/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Internal debugger trap file OS specific implementation
*               header file.
*
****************************************************************************/


#ifndef _TRPIMP_H_INCLUDED
#define _TRPIMP_H_INCLUDED

#include "trpall.h"

#define pick(sym,dumbfunc,stdfunc)          extern trap_retval TRAP_CORE( stdfunc ) (void);
#include "_trpreq.h"
#undef pick

#ifdef WANT_FILE_INFO
    #define REQ_FILE_INFO_DEF(sym,func)     extern trap_retval TRAP_FILE_INFO( func ) (void);
    REQ_FILE_INFO_DEFS()
    #undef REQ_FILE_INFO_DEF
#endif

#ifdef WANT_ENV
    #define REQ_ENV_DEF(sym,func)           extern trap_retval TRAP_ENV( func ) (void);
    REQ_ENV_DEFS()
    #undef REQ_ENV_DEF
#endif

#ifdef WANT_ASYNC
    #define REQ_ASYNC_DEF(sym,func)         extern trap_retval TRAP_ASYNC( func ) (void);
    REQ_ASYNC_DEFS()
    #undef REQ_ASYNC_DEF
#endif

#ifdef WANT_FILE
    #define REQ_FILE_DEF(sym,func)          extern trap_retval TRAP_FILE( func ) (void);
    REQ_FILE_DEFS()
    #undef REQ_FILE_DEF
#endif

#ifdef WANT_OVL
    #define REQ_OVL_DEF(sym,func)           extern trap_retval TRAP_OVERLAY( func ) (void);
    REQ_OVL_DEFS()
    #undef REQ_OVL_DEF
#endif

#ifdef WANT_THREAD
    #define REQ_THREAD_DEF(sym,func)        extern trap_retval TRAP_THREAD( func ) (void);
    REQ_THREAD_DEFS()
    #undef REQ_THREAD_DEF
#endif

#ifdef WANT_RUN_THREAD
    #define REQ_RUN_THREAD_DEF(sym,func)    extern trap_retval TRAP_RUN_THREAD( func ) (void);
    REQ_RUN_THREAD_DEFS()
    #undef REQ_RUN_THREAD_DEF
#endif

#ifdef WANT_RFX
    #define REQ_RFX_DEF(sym,func)           extern trap_retval TRAP_RFX( func ) (void);
    REQ_RFX_DEFS()
    #undef REQ_RFX_DEF
#endif

#ifdef WANT_CAPABILITIES
    #define REQ_CAPABILITIES_DEF(sym,func)  extern trap_retval TRAP_CAPABILITIES( func ) (void);
    REQ_CAPABILITIES_DEFS()
    #undef REQ_CAPABILITIES_DEF
#endif

#endif
