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


#ifndef _DEFWIN_H_INCLUDED
#define _DEFWIN_H_INCLUDED

#include "variety.h"

#if defined(__NT__) || defined(__WINDOWS__) || defined(__OS2__)

#include <unistd.h>

#define DEFAULT_WINDOWING

struct window_data;
typedef struct window_data _WCI86FAR *LPWDATA;

extern unsigned (*_WindowsStdin)( LPWDATA, void *, unsigned );
extern unsigned (*_WindowsStdout)( LPWDATA, const void *, unsigned );
extern unsigned (*_WindowsKbhit)( LPWDATA );
extern unsigned (*_WindowsGetch)( LPWDATA );
extern unsigned (*_WindowsGetche)( LPWDATA );
extern void     (*_WindowsPutch)( LPWDATA, unsigned );
extern void     (*_WindowExitRtn)( void );
extern unsigned (*_WindowsNewWindow)( char *, ... );
extern int      (*_WindowsCloseWindow)( LPWDATA );
extern LPWDATA  (*_WindowsIsWindowedHandle)( int );
extern void     (*_WindowsRemoveWindowedHandle)( int );
extern int      (*_WindowsSetAbout)( char *, char * );
extern int      (*_WindowsSetAppTitle)( char * );
extern int      (*_WindowsSetConTitle)( LPWDATA, char * );
extern int      (*_WindowsDestroyOnClose)( LPWDATA );
extern int      (*_WindowsYieldControl)( void );
extern int      (*_WindowsShutDown)( void );

#endif
#endif
