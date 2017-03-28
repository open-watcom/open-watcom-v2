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
* Description:  Dynamic dialog internals for Windows.
*
****************************************************************************/


#if !defined( __OS2__ )

#include "wprocmap.h"
#include "_windlg.h"

#if defined( __WINDOWS_386__ )
#define GetPtrGlobalLock(data) MK_FP32( GlobalLock( data ) )
#else
#define GetPtrGlobalLock(data) GlobalLock( data )
#endif

#ifdef __WINDOWS__

#define SLEN( a )       (strlen((a))+1)
#define _ISFAR          __far
#define _FARmemcpy      _fmemcpy

#else

#define _FARmemcpy      memcpy
#define _ISFAR
#define SLEN( a )       (2 * MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, (a), -1, NULL, 0 ))

#endif

extern TEMPLATE_HANDLE  DialogTemplate( DWORD style, int x, int y, int cx, int cy,
                            const char *menuname, const char *classname, const char *captiontext,
                            WORD pointsize, const char *typeface, size_t *datalen );
extern TEMPLATE_HANDLE  AddControl( TEMPLATE_HANDLE data, int x, int y, int cx, int cy, WORD id, DWORD style,
                            const char *class, const char *text, BYTE infolen, const char *infodata, size_t *datalen );
extern TEMPLATE_HANDLE  DoneAddingControls( TEMPLATE_HANDLE data );
extern INT_PTR          DynamicDialogBox( DLGPROCx fn, HINSTANCE inst, HWND hwnd, TEMPLATE_HANDLE data, LPARAM lparam );

#endif
