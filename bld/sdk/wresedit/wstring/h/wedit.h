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


#ifndef WEDIT_INCLUDED
#define WEDIT_INCLUDED

#include "wstr.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern void WInitEditWindows( HINSTANCE );
extern void WFiniEditWindows( void );
extern void WInitEditDlg( HINSTANCE inst, HWND parent );
extern bool WCreateStringEditWindow( WStringEditInfo *, HINSTANCE );
extern bool WResizeStringEditWindow( WStringEditInfo *, RECT * );
extern bool WInitEditWindowListBox( WStringEditInfo * );
extern void WHandleSelChange( WStringEditInfo * );
extern void WDoHandleSelChange( WStringEditInfo *, bool, bool );

extern bool WSetEditWindowStringData( WStringEditInfo *, WStringBlock *, uint_16 );
extern bool WGetEditWindowStringData( WStringEditInfo *, char **, char **, uint_16 * );

extern bool WGetEditWindowText( HWND, char ** );
extern bool WGetEditWindowID( HWND, char **, uint_16 *, WRHashTable *, bool );

extern bool WSetEditWindowText( HWND, char * );
extern bool WSetEditWindowID( HWND, uint_16, char * );

extern void WResetEditWindow( WStringEditInfo *einfo );
extern bool WClipStringItem( WStringEditInfo *einfo, bool cut );
extern bool WPasteStringItem( WStringEditInfo *einfo );

#endif
