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
* Description:  Signal table for DOS and Windows 3.x.
*
****************************************************************************/


#include "variety.h"
#include <signal.h>
#include "sigfunc.h"


__sig_func _HUGEDATA _SignalTable[] = {
    SIG_IGN,        /* unused  */
    SIG_DFL,        /* SIGABRT */
    SIG_DFL,        /* SIGFPE  */
    SIG_DFL,        /* SIGILL  */
    SIG_DFL,        /* SIGINT  */
    SIG_DFL,        /* SIGSEGV */
    SIG_DFL,        /* SIGTERM */
    SIG_DFL,        /* SIGBREAK */
    SIG_IGN,        /* SIGUSR1 */
    SIG_IGN,        /* SIGUSR2 */
    SIG_IGN,        /* SIGUSR3 */
    SIG_DFL,        /* SIGIDIVZ */
    SIG_DFL         /* SIGIOVFL */
};
