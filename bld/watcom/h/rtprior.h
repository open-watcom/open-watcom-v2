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
* Description:  Run-time initialization/finalization priority constants.
*
****************************************************************************/


#ifndef __RTPRIOR_H__
#define __RTPRIOR_H__

/*
    ..._PRIORITY_... constant must correspond with definition in xinit.inc file
*/
enum {
    INIT_PRIORITY_THREAD    =  1, // priority for thread data init
    INIT_PRIORITY_FPU       =  2, // priority for FPU/EMU init
    INIT_PRIORITY_RUNTIME   = 10, // priority for run/time initialization
    INIT_PRIORITY_EXIT      = 16,
    INIT_PRIORITY_IOSTREAM  = 20, // priority for IOSTREAM
    INIT_PRIORITY_LIBRARY   = 32, // default library-initialization priority
    INIT_PRIORITY_PROGRAM   = 64, // default program-initialization priority
    FINI_PRIORITY_DTOR      = 40, // priority for module DTOR
    DTOR_PRIORITY           = 40, // priority for module DTOR
    FINI_PRIORITY_EXIT      = 16  // when exit() is called, functions between
                                  // 255 and this are called, the rest of the
                                  // fini routines are called from __exit().
};

#endif
