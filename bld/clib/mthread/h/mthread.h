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


#ifndef _MTHREAD_H_INCLUDED
#define _MTHREAD_H_INCLUDED

#if defined(__386__) || defined(__AXP__) || defined(__PPC__)
  thread_data *__AllocInitThreadData( thread_data *tdata );
  thread_data   *__AllocThreadData( void );
  void          __InitThreadData( thread_data * );
  void          __AccessTDList( void );
  void          __ReleaseTDList( void );
  #if defined(__NT__)
    BOOL        __NTThreadInit( void );
    void        __NTThreadFini( void );
    BOOL        __NTAddThread( thread_data * );
    void        __NTRemoveThread( int );
  #endif
  #if defined(__OS2__)
    int         __OS2AddThread( TID, thread_data * );
    void        __OS2RemoveThread( void );
  #endif
#endif

#endif
