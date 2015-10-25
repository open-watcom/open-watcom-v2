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
* Description:  Mark dialog interface.
*
****************************************************************************/


#ifndef _MARK_H_INCLUDED
#define _MARK_H_INCLUDED

#define MARK_LEN        100

/* Constants for mark dialog */
#define MARK_CANCEL                     IDCANCEL
#define MARK_OK                         IDOK
#define MARK_EDIT                       103
#define MARK_TEXT                       104
#define MARK_PLAIN                      105
#define MARK_BOXED                      106
#define MARK_STYLE_GRP                  107

void    ProcessMark( HWND owner, HANDLE instance, void (*fn)( char * ) );

WINEXPORT INT_PTR CALLBACK MarkDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

#endif /* _MARK_H_INCLUDED */
