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


#ifndef _LOCK_H_INCLUDED
#ifdef __MT__

#if defined( __cplusplus )
extern "C" {
#endif
#pragma warning 604 9
#pragma warning 594 9
#include "osthread.h"
#pragma warning 604 1
#pragma warning 594 1
#if defined( __cplusplus )
};
#endif
//#include <stddef.h>
#define  __lock_first( __a, __b )  ((__a) < (__b) ? (__a) : (__b))
#define  __lock_second( __a, __b ) ((__a) > (__b) ? (__a) : (__b))

#if defined( __cplusplus )
class _WPRTLINK __lock {
public:
    __lock();
    ~__lock();
    void p( void );
    void v( void );
private:
    semaphore_object locksem;
};

class _WPRTLINK __fn_lock {
public:
    __fn_lock( __lock * );
    ~__fn_lock();
private:
    __lock *fn_lock;
    __fn_lock( __fn_lock const & );
    void operator =( __fn_lock const & );
};
#endif

#endif
#define _LOCK_H_INCLUDED
#endif
