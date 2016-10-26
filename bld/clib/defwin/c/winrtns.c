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
* Description:  Default Windowing - API function pointers definition.
*
****************************************************************************/

#include "variety.h"
#include "_defwin.h"

LPWDATA (*_WindowsIsWindowedHandle)( int h ) = { 0 };
void (*_WindowsRemoveWindowedHandle)( int h ) = { 0 };
unsigned (*_WindowsNewWindow)( char *s, ... ) = { 0 };
int (*_WindowsCloseWindow)( LPWDATA lp ) = { 0 };
int (*_WindowsSetAbout)( char *p1, char *p2 ) = { 0 };
int (*_WindowsSetAppTitle)( char *p ) = { 0 };
int (*_WindowsSetConTitle)( LPWDATA lp, char *s ) = { 0 };
int (*_WindowsDestroyOnClose)( LPWDATA lp ) = { 0 };
int (*_WindowsYieldControl)( void ) = { 0 };
int (*_WindowsShutDown)( void ) = { 0 };
unsigned (*_WindowsStdin)( LPWDATA lp, void *p, unsigned u ) = { 0 };
unsigned (*_WindowsStdout)( LPWDATA lp, const void *p, unsigned u ) = { 0 };
unsigned (*_WindowsKbhit)( LPWDATA lp ) = { 0 };
unsigned (*_WindowsGetch)( LPWDATA lp ) = { 0 };
unsigned (*_WindowsGetche)( LPWDATA lp ) = { 0 };
void (*_WindowsPutch)( LPWDATA lp, unsigned u ) = { 0 };
void (*_WindowsExitRtn)( void ) = { 0 };

