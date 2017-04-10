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
* Description:  Dynamic dialogs for Windows.
*
****************************************************************************/


#ifndef __OS2__

#include "wclbproc.h"
#include "_windlg.h"

#ifdef __WINDOWS__

#define _FARmemcpy              _fmemcpy
#define SLEN( a )               (((a)!=NULL)?strlen((a))+1:1)

#else

#define _FARmemcpy              memcpy
#define SLEN( a )               (((a)!=NULL)?strlen((a))*2+2:2)
//#define SLEN( a )               (((a)!=NULL)?2 * MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, (a), -1, NULL, 0 ):2)

#endif

extern TEMPLATE_HANDLE  _DialogTemplate( DWORD style, int x, int y, int cx, int cy,
                            const char *menuname, const char *classname, const char *captiontext,
                            WORD pointsize, const char *facename, size_t *templatelen );
extern TEMPLATE_HANDLE  _AddControl( TEMPLATE_HANDLE dlgtemplate, int x, int y, int cx, int cy, WORD id, DWORD style,
                            const char *classname, const char *captiontext, const void *infodata, BYTE infodatalen, size_t *templatelen );
extern void             _DoneAddingControls( TEMPLATE_HANDLE dlgtemplate );
extern INT_PTR          _DynamicDialogBox( DLGPROCx dlgfn, HANDLE inst, HWND hwnd, TEMPLATE_HANDLE dlgtemplate );

#endif
