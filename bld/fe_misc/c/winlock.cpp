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



#include "winlock.h"

namespace iothread {

__lock::__lock() {
/*****************/
    InitializeCriticalSection(&locksec);
}

__lock::~__lock() {
/*****************/
    DeleteCriticalSection(&locksec);
}

void __lock::p( void ) {
/**********************/
    EnterCriticalSection(&locksec);
}

void __lock::v( void ) {
/**********************/
    LeaveCriticalSection(&locksec);
}

/***************************************************/

__semaphore::__semaphore(int initSize, int maxSize) {
    _sem =  CreateSemaphore(NULL,initSize,maxSize,NULL);
    _level = initSize;
}

__semaphore::~__semaphore() {
    CloseHandle(_sem);
}

int __semaphore::decrease(int timeOut) {
    DWORD result = WaitForSingleObject(_sem, timeOut);
    _level--;
    return(result);
}

int __semaphore::decrease() {
    DWORD result =WaitForSingleObject(_sem, INFINITE);
    _level--;
    return(result);
}

int __semaphore::increase() {
    DWORD result = ReleaseSemaphore(_sem, 1, NULL);
    _level++;
    return(result);
}

int __semaphore::increase(int increaseBy) {
    DWORD result = ReleaseSemaphore(_sem, increaseBy, NULL);
    _level+=increaseBy;
    return(result);
}

} // end of namespace iothread
