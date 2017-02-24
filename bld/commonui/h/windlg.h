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

#if defined( __WINDOWS_386__ )
#define GetPtrGlobalLock(data) MK_FP32( GlobalLock( data ) )
#else
#define GetPtrGlobalLock(data) GlobalLock( data )
#endif

#define _AdjustUp( size, word ) ( ((size)+((word)-1)) & ~((word)-1) )

typedef GLOBALHANDLE    TEMPLATE_HANDLE;

#if defined(__NT__)

#define ADJUST_ITEMLEN( a )     (a) = _AdjustUp( a, 8 )
#define ADJUST_BLOCKLEN( a )    (a) = _AdjustUp( a, 4 )
#define ADJUST_CLASSLEN( a )    (a) = _AdjustUp( a, 2 )
#define _FARmemcpy              memcpy
#define _ISFAR
//#define SLEN( a )               (strlen((a))*2+2)
// fixed to handle DBCS strings properly - rnk 3/1/96
#define SLEN( a )               (2 * MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, (a), -1, NULL, 0 ))
typedef WORD INFOTYPE;

#else

#define SLEN( a )               (strlen((a))+1)
#define ADJUST_ITEMLEN( a )
#define ADJUST_BLOCKLEN( a )
#define ADJUST_CLASSLEN( a )
#define _ISFAR                  __far
#define _FARmemcpy              _fmemcpy
typedef BYTE INFOTYPE;

#endif

#include "_windlg.h"

extern TEMPLATE_HANDLE DialogTemplate( LONG dtStyle, int dtx, int dty,
                                       int dtcx, int dtcy, const char *menuname,
                                       const char *classname, const char *captiontext,
                                       int pointsize, const char *typeface, size_t *datalen );
extern TEMPLATE_HANDLE DoneAddingControls( TEMPLATE_HANDLE data );
extern TEMPLATE_HANDLE AddControl( TEMPLATE_HANDLE data, int dtilx,
                                       int dtily, int dtilcx, int dtilcy,
                                       int id, long style, const char *class,
                                       const char *text, BYTE infolen,
                                       const char *infodata, size_t *datalen );
INT_PTR DynamicDialogBox( DLGPROCx fn, HINSTANCE inst, HWND hwnd,
                                       TEMPLATE_HANDLE data, LPARAM lparam );

#endif
