/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#if defined( __OS2__ )

#include "_pmdlg.h"

#define LPBYTE          PBYTE
#define _FARmemcpy      memcpy

#define SLEN( a )       (((a)!=NULL)?strlen((a)):0)

/*
 * OS/2 PM defaults,
 */
#define CODE_PAGE       850
#define TEMP_TYPE       0
#define RESERVED        0

extern TEMPLATE_HANDLE  _DialogTemplate( USHORT temptype, USHORT codepage, USHORT focus );
extern TEMPLATE_HANDLE  _AddControl( TEMPLATE_HANDLE dlgtemplate, ULONG style,
                            int x, int y, int cx, int cy, USHORT id, USHORT children,
                            const char FAR *classname, const char *captiontext, const char *presparms,
                            const void *ctldata, ULONG ctldatalen );
extern TEMPLATE_HANDLE  _DoneAddingControls( TEMPLATE_HANDLE dlgtemplate );
extern int              _DynamicDialogBox( PFNWP fn, HWND hwnd, TEMPLATE_HANDLE dlgtemplate );

#endif
