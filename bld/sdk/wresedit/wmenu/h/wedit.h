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

#include "w_menu.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern void  WInitEditWindows           ( HINSTANCE );
extern void  WFiniEditWindows           ( void );
extern void  WInitEditDlg               ( HINSTANCE inst, HWND parent );
extern Bool  WCreateMenuEditWindow      ( WMenuEditInfo *, HINSTANCE );
extern Bool  WResizeMenuEditWindow      ( WMenuEditInfo *, RECT * );
extern Bool  WSetEditWinResName         ( WMenuEditInfo * );
extern Bool  WInitEditWindowListBox     ( WMenuEditInfo * );
extern void  WHandleSelChange           ( WMenuEditInfo * );
extern void  WDoHandleSelChange         ( WMenuEditInfo *, Bool, Bool );
extern Bool  WResetEditWindowFlags      ( HWND dlg );

extern void  WSetEditWindowControls     ( WMenuEditInfo *, WMenuEntry * );
extern Bool  WSetEditWindowMenuEntry    ( WMenuEditInfo *, WMenuEntry * );
extern Bool  WGetEditWindowMenuEntry    ( WMenuEditInfo *, WMenuEntry *, Bool, Bool * );

extern Bool  WSetEditWindowText         ( HWND, MenuFlags, char * );
extern Bool  WSetEditWindowID           ( HWND, uint_16, Bool, char * );
extern Bool  WSetEditWindowFlags        ( HWND, MenuFlags, Bool );

extern Bool  WGetEditWindowText         ( HWND, char ** );
extern Bool  WGetEditWindowID           ( HWND, char **, uint_16 *, WRHashTable *, Bool );
extern Bool  WGetEditWindowFlags        ( HWND, MenuFlags * );
extern Bool  WClipMenuItem              ( WMenuEditInfo *einfo, Bool cut );
extern Bool  WPasteMenuItem             ( WMenuEditInfo *einfo );

#endif
