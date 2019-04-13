/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


// these functions get overridden by the OW Debugger if it's doing its own
// hardware cursor on a monochrome screen, but the debugger needs
// to call the original version if it's on the main screen, so I've changed
// the original versions into _uixxx().  In a normal UI app, these stubs kick
// in so the world is back to normal

extern void     UIHOOK uiinitcursor( void );
extern void     UIHOOK uifinicursor( void );
extern void     UIHOOK uioncursor( void );
extern void     UIHOOK uioffcursor( void );
extern void     UIHOOK uigetcursor( CURSORORD *crow, CURSORORD *ccol, CURSOR_TYPE *ctype, CATTR *cattr );
extern void     UIHOOK uisetcursor( CURSORORD crow, CURSORORD ccol, CURSOR_TYPE ctype, CATTR cattr );
extern void     UIHOOK uiswapcursor( void );

#ifndef UIVIRTS

extern void     UIHOOK _uiinitcursor( void );
extern void     UIHOOK _uifinicursor( void );
extern void     UIHOOK _uioncursor( void );
extern void     UIHOOK _uioffcursor( void );
extern void     UIHOOK _uigetcursor( CURSORORD *crow, CURSORORD *ccol, CURSOR_TYPE *ctype, CATTR *cattr );
extern void     UIHOOK _uisetcursor( CURSORORD crow, CURSORORD ccol, CURSOR_TYPE ctype, CATTR cattr );
extern void     UIHOOK _uiswapcursor( void );

#endif
