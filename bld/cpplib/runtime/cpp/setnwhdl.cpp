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


// OPNEW.CPP -- operator ::new default definition
//           -- set operator new handler, this routine is used to set the
//              handler to deal with allocation failure during operator new
//           -- the default new handler is NULL
//
// 92/05/15  -- G.R.Bentz        -- defined

#include <cpplib.h>
#include <stddef.h>
#include <malloc.h>
#include <new>


_WPRTLINK
PFV set_new_handler(            // SET HANDLER FOR NEW FAILURE
    PFV handler )               // - new handler to be used
{
    THREAD_CTL *thr;            // - thread control
    PFV previous_handler;

    thr = &_RWD_ThreadData;
    previous_handler = thr->new_handler;
    thr->new_handler = handler;
    return( previous_handler );
}
