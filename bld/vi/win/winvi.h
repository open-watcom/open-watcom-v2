/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
    #define NULLHANDLE          ((HANDLE)0)
#elif defined( __WINDOWS__ )
    #define NULLHANDLE          ((HANDLE)0)
#elif defined( __NT__ )
    #define NULLHANDLE          ((HANDLE)0)
#elif defined( __OS2__ )
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

/*
 * This structure is mostly an artifact without any real use now...
 */
typedef struct window {
    window_info *info;
    RECT        area;
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

// we will always be using SetWindowLongPtr
#define MAGIC_SIZE          sizeof( LONG_PTR )
#define EXTRA_WIN_DATA      (WIN_LAST * MAGIC_SIZE)
#define WINDOW_FROM_ID( x ) ((window *)GET_WNDLONGPTR( x, WIN_WINDOW * MAGIC_SIZE ))
#define DATA_FROM_ID( x )   ((window_data *)GET_WNDLONGPTR( x, WIN_DATA * MAGIC_SIZE ))
#define WINDOW_TO_ID( x, d )((window *)SET_WNDLONGPTR( x, WIN_WINDOW * MAGIC_SIZE, d ))
#define DATA_TO_ID( x, d )  ((window_data *)SET_WNDLONGPTR( x, WIN_DATA * MAGIC_SIZE, d ))

#define WIN_TEXT_STYLE(w)       (&((w)->info->text_style))
#define WIN_HILIGHT_STYLE(w)    (&((w)->info->hilight_style))
#define WIN_TEXT_FONT(w)        ((w)->info->text_style.font)
#define WIN_TEXT_COLOR(w)       ((w)->info->text_style.foreground)
#define WIN_TEXT_BACKCOLOR(w)   ((w)->info->text_style.background)

extern window_id        EditContainer;
extern window_id        Root;
extern HINSTANCE        InstanceHandle;
extern bool             AllowDisplay;
extern window           StatusBar;
extern window           EditWindow;
extern window           CommandWindow;
extern window           MessageBar;
extern window           FileCompleteWindow;
extern window           RepeatCountWindow;
extern RECT             ToolBarFloatRect;
extern RECT             RootRect;
extern int              RootState;
extern char             _NEAR EditorName[];
extern window_id        CommandId;

#endif
