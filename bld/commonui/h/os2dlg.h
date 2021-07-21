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


#include "_pmdlg.h"

#ifdef _M_I86
#define _FARmemcpy      _fmemcpy
#else
#define _FARmemcpy      memcpy
#endif

#define SLEN( a )       (((a)!=NULL)?strlen((a)):0)

/*
 * OS/2 PM defaults,
 */
#define CODE_PAGE       437
#define TEMPLATE_TYPE   0
#define RESERVED        0

/*
 * I am not about to change this in ALL the code, and since I am not including
 * windows.h, I have defined it as such
 */

extern TEMPLATE_HANDLE  DialogTemplate( DWORD dtStyle, int x, int y, int cx, int cy,
                                const char *menuname, const char *classname, const char *captiontext,
                                WORD pointsize, const char *facename, size_t *templatelen );
extern TEMPLATE_HANDLE  AddControl( TEMPLATE_HANDLE dlgtemplate, int x, int y, int cx, int cy, WORD id, DWORD style,
                                const char FAR *classname, const char *captiontext,
                                const void *infodata, BYTE infodatalen, size_t *templatelen );
extern TEMPLATE_HANDLE  DoneAddingControls ( TEMPLATE_HANDLE dlgtemplate );
extern int              DynamicDialogBox( PFNWP fn, WPI_INST inst, HWND hwnd, TEMPLATE_HANDLE dlgtemplate, MPARAM lparam );
