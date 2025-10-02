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
* Description:  Default windowing - internal API declarations.
*
****************************************************************************/


#ifndef __DEFWIN_H_INCLUDED
#define __DEFWIN_H_INCLUDED

#if defined(__NT__) || defined(__WINDOWS__) || defined(__OS2__)

#include <unistd.h>
#include <stdbool.h>


struct window_data;
typedef struct window_data _WCI86FAR *LPWDATA;

extern int      (_WCNEAR *_WindowsStdin)( LPWDATA, void *, unsigned );
extern int      (_WCNEAR *_WindowsStdout)( LPWDATA, const void *, unsigned );
extern bool     (_WCNEAR *_WindowsKbhit)( LPWDATA );
extern int      (_WCNEAR *_WindowsGetch)( LPWDATA );
extern int      (_WCNEAR *_WindowsGetche)( LPWDATA );
extern void     (_WCNEAR *_WindowsPutch)( LPWDATA, int );
extern void     (_WCNEAR *_WindowsExitRtn)( void );
extern bool     (_WCNEAR *_WindowsNewWindow)( const char *, ... );
extern bool     (_WCNEAR *_WindowsCloseWindow)( LPWDATA );
extern LPWDATA  (_WCNEAR *_WindowsIsWindowedHandle)( int );
extern void     (_WCNEAR *_WindowsRemoveWindowedHandle)( int );
extern bool     (_WCNEAR *_WindowsSetAbout)( const char *, const char * );
extern bool     (_WCNEAR *_WindowsSetAppTitle)( const char * );
extern bool     (_WCNEAR *_WindowsSetConTitle)( LPWDATA, const char * );
extern bool     (_WCNEAR *_WindowsDestroyOnClose)( LPWDATA );
extern bool     (_WCNEAR *_WindowsYieldControl)( void );
extern bool     (_WCNEAR *_WindowsShutDown)( void );

#endif
#endif
