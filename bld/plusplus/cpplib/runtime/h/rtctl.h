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
* Description:  Run-time control information
*
****************************************************************************/

#ifndef __RTCTL_H__
#define __RTCTL_H__

struct _RTCTL                   // RUN-TIME CONTROL
{   THREAD_CTL* thr;            // - thread pointer
    RW_DTREG* rw;               // - R/W block
    RO_DTREG* ro;               // - R/O block

    _RTCTL()                    // - default constructor
        : thr( PgmThread() )
    {
    }

    _RTCTL( THREAD_CTL* thrp )  // - constructor( thread-ptr )
        : thr( thrp )
    {
    }

    _RTCTL* setRwRo             // - set RW, RO fields
        ( RW_DTREG* rwp )       // - - initial RW ptr.
    {
        rw = rwp;
        ro = rwp->base.ro;
        return this;
    }
};


#endif
