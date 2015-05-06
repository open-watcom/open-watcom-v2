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
* Description:  multi-threaded internal functions and data declarations
*
****************************************************************************/


#ifndef _MTHREAD_H_INCLUDED
#define _MTHREAD_H_INCLUDED

    extern void         *__InitThreadProcessing( void );
    extern void         __FiniThreadProcessing( void );
#if defined( _M_I86 )
  #if defined( __OS2__ )
    extern void         __SetupThreadProcessing( int );
  #endif
#else
    extern thread_data  *__FirstThreadData;

    extern thread_data  *__AllocInitThreadData( thread_data *tdata );
    extern void         __FreeInitThreadData( thread_data *tdata );
    extern thread_data  *__AllocThreadData( void );
    extern void         __InitThreadData( thread_data * );

    extern void         __InitMultipleThread( void );

    extern void         __AccessTDList( void );
    extern void         __ReleaseTDList( void );
  #if defined( __NT__ )
    extern int          __NTThreadInit( void );
    extern int          __NTAddThread( thread_data * );
    extern void         __NTRemoveThread( int );
  #elif defined( _NETWARE_LIBC )
    #include "nw_libc.h"
    extern int          __LibCThreadInit( void );
    extern void         __LibCThreadFini( void );
    extern int          __LibCAddThread( thread_data * );
    extern void         __LibCRemoveThread( int );
  #elif defined( __OS2__ )
    extern int          __OS2AddThread( TID, thread_data * );
    extern void         __OS2RemoveThread( void );
  #elif defined( __QNX__ )
    extern thread_data  *__QNXAddThread( thread_data *tdata );
    extern void         __QNXRemoveThread( void );
  #elif defined( __LINUX__ )
    extern thread_data  *__LinuxAddThread( thread_data *tdata );
    extern void         __LinuxRemoveThread( void );
  #elif defined( __RDOS__ ) /* || defined( __RDOSDEV__ ) */
    extern int          __RdosThreadInit( void );
    extern int          __RdosAddThread( thread_data * );
    extern void         __RdosRemoveThread( void );
  #endif

    extern void         (*_AccessTDList)( void );
    extern void         (*_ReleaseTDList)( void );
    extern void         (*_ThreadExitRtn)( void );
#endif

#endif
