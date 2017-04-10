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


#if !defined( __OS2__ )

#include "_windlg.h"
#include "wclbproc.h"


#if defined( __WINDOWS_386__ )
#define GetPtrGlobalLock(data) MK_FP32( GlobalLock( data ) )
#else
#define GetPtrGlobalLock(data) GlobalLock( data )
#endif

#ifdef __WINDOWS__
  #define _FARmemcpy    _fmemcpy
  #define SLEN( a )     ((a != NULL) ? strlen( a ) + 1 : 1)
#else
  #define _FARmemcpy    memcpy
  #define SLEN( a )     ((a != NULL) ? strlen( a ) * 2 + 2 : 2)
#endif

extern TEMPLATE_HANDLE  DialogTemplate( DWORD dtStyle, int x, int y, int cx, int cy, const char *menuname, const char *classname, const char *captiontext, WORD font_pointsize, const char *font_facename, size_t *templatelen );
extern void             DoneAddingControls( TEMPLATE_HANDLE dlgtemplate );
extern TEMPLATE_HANDLE  AddControl( TEMPLATE_HANDLE dlgtemplate, int x, int y, int cx, int cy, WORD id, DWORD style, const char *classname, const char *captiontext, const void *infodata, BYTE infodatalen, size_t *templatelen );
extern INT_PTR          DynamicDialogBox( DLGPROCx fn, HANDLE inst, HWND hwnd, TEMPLATE_HANDLE dlgtemplate );

#endif
