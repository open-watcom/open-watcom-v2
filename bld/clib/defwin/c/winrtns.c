/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
#include <stddef.h>
#include "_defwin.h"


LPWDATA (*_WindowsIsWindowedHandle)( int h ) = { NULL };
void (*_WindowsRemoveWindowedHandle)( int h ) = { NULL };
bool (*_WindowsNewWindow)( const char *s, ... ) = { NULL };
bool (*_WindowsCloseWindow)( LPWDATA lp ) = { NULL };
bool (*_WindowsSetAbout)( const char *p1, const char *p2 ) = { NULL };
bool (*_WindowsSetAppTitle)( const char *p ) = { NULL };
bool (*_WindowsSetConTitle)( LPWDATA lp, const char *s ) = { NULL };
bool (*_WindowsDestroyOnClose)( LPWDATA lp ) = { NULL };
bool (*_WindowsYieldControl)( void ) = { NULL };
bool (*_WindowsShutDown)( void ) = { NULL };
int (*_WindowsStdin)( LPWDATA lp, void *p, unsigned u ) = { NULL };
int (*_WindowsStdout)( LPWDATA lp, const void *p, unsigned u ) = { NULL };
bool (*_WindowsKbhit)( LPWDATA lp ) = { NULL };
int (*_WindowsGetch)( LPWDATA lp ) = { NULL };
int (*_WindowsGetche)( LPWDATA lp ) = { NULL };
void (*_WindowsPutch)( LPWDATA lp, int c ) = { NULL };
void (*_WindowsExitRtn)( void ) = { NULL };
