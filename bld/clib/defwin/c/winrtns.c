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


unsigned long (*_WindowsIsWindowedHandle)() = { 0 };
void (*_WindowsRemoveWindowedHandle)() = { 0 };
int (*_WindowsNewWindow)() = { 0 };
int (*_WindowsCloseWindow)() = { 0 };
int (*_WindowsSetAbout)() = { 0 };
int (*_WindowsSetAppTitle)() = { 0 };
int (*_WindowsSetConTitle)() = { 0 };
int (*_WindowsDestroyOnClose)() = { 0 };
int (*_WindowsYieldControl)() = { 0 };
int (*_WindowsShutDown)() = { 0 };
unsigned (*_WindowsStdin)() = { 0 };
unsigned (*_WindowsStdout)() = { 0 };
unsigned (*_WindowsKbhit)() = { 0 };
unsigned (*_WindowsGetch)() = { 0 };
unsigned (*_WindowsGetche)() = { 0 };
void (*_WindowsPutch)() = { 0 };
void (*_WindowExitRtn)() = { 0 };
