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
* Description:  Windows editor global include.
*
****************************************************************************/


#ifndef WINVI_INCLUDED
#define WINVI_INCLUDED

#include "winhdr.h"
#include "winrtns.h"

#if defined( __WINDOWS_386__ )
    #define WATCOM_ABOUT_EDITOR "Open Watcom Text Editor for Windows (32-bit)"
    #define WINEXP              FAR PASCAL
    #define MAKEPTR( a )        ((void far *)MK_FP32( (void *) a ))
    #define __FAR__             __far
    #define MEMCPY              _fmemcpy
    #define NULLHANDLE          ((HANDLE)0)
#elif defined( __WINDOWS__ )
    #define WATCOM_ABOUT_EDITOR "Open Watcom Text Editor for Windows"
    #define WINEXP              __export FAR PASCAL
    #define MAKEPTR( a )        ((LPVOID) a)
    #define __FAR__
    #define MEMCPY              memcpy
    #define NULLHANDLE          ((HANDLE)0)
#elif defined( __NT__ )
    #define WATCOM_ABOUT_EDITOR "Open Watcom Text Editor for Windows"
    #define WINEXP              __export __stdcall
    #define MAKEPTR( a )        ((LPVOID) a)
    #define __FAR__
    #define MEMCPY              memcpy
    #define NULLHANDLE          ((HANDLE)0)
#elif defined( __OS2__ )
    #define WATCOM_ABOUT_EDITOR "Open Watcom Text Editor for OS/2 PM"
    #define WINEXP              __export _System
    #define MAKEPTR( a )        ((LPVOID) a)
    #define __FAR__
    #define MEMCPY              memcpy
#endif

typedef LPVOID  *LPLPVOID;
typedef LPSTR   *LPLPSTR;
typedef UINT    *LPUINT;

#define HNULL   ((HANDLE)0)

typedef struct colour {
    HBRUSH      brush;
    HPEN        pen;
    long        rgb;
} colour;

typedef BOOL (*win_func)( struct window *, void * );

/*
 * This structure is mostly an artifact without any real use now...
 */
typedef struct window {
    window_info *info;
    RECT        area;
    win_func    init, fini;
} window;

typedef struct window_data {
    RECT        extra;          // the bit at the bottom which is not used
    info        *info;
} window_data;

typedef enum window_extra {
    WIN_WINDOW = 0,
    WIN_DATA,
    WIN_MDI_DATA,
    WIN_LAST                    // so we can tell how big this is
} window_extra;

#define MAGIC_SIZE      4       // we will always be using SetWindowLong

#ifdef DBG
    #define BAD_ID( id )    ((id) == NULL || (id) == (window_id)-1 || !IsWindow( id ))
#else
    #define BAD_ID( id )    ((id) == (window_id)-1)
#endif
#define WINDOW_FROM_ID( x ) ((window *)GetWindowLong( x, WIN_WINDOW * MAGIC_SIZE ))
#define DATA_FROM_ID( x )   ((window_data *)GetWindowLong( x, WIN_DATA * MAGIC_SIZE ))
#define WIN_STYLE( w )      (&((w)->info->text))
#define WIN_HILIGHT( w )    (&((w)->info->hilight))
#define WIN_FONT( w )       ((w)->info->text.font)
#define WIN_TEXTCOLOR( w )  ((w)->info->text.foreground)
#define WIN_BACKCOLOR( w )  ((w)->info->text.background)

extern window_id        EditContainer;
extern window_id        Root;
extern HINSTANCE        InstanceHandle;
extern BOOL             AllowDisplay;
extern window           StatusBar;
extern window           EditWindow;
extern window           CommandWindow;
extern window           MessageBar;
extern window           FileCompleteWindow;
extern window           RepeatCountWindow;
extern RECT             ToolBarFloatRect;
extern RECT             RootRect;
extern int              RootState;
extern char             near EditorName[];
extern window_id        CommandId;

#endif
