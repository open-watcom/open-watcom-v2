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
* Description:  Button library interface.
*
****************************************************************************/


extern void add_button( HWND parent, int top, int left, int id, int FAR * pwidth, int FAR * pheight );

#if defined(__OS2__)

extern ULONG measure_button( HWND parent, MPARAM mp1, MPARAM mp2 );

extern void draw_button( MPARAM mp1, MPARAM mp2 );

#else   // Win16 or Win32

extern void measure_button( HWND parent, int button_id, MEASUREITEMSTRUCT FAR * measure );

extern void draw_button( int button_id, DRAWITEMSTRUCT FAR * draw );

#endif
