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


#include "commonui.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "watcom.h"
#include "cguimem.h"
#include "rcstr.grh"
#include "uistr.grh"
#include "ldstr.h"


#define RCSTR_MAX_LEN           128

typedef struct {
    HWND        hwnd;
    HDC         memdc;
    HBITMAP     old_bitmap;
    HBITMAP     screen;
}ScreenBitmap;

typedef struct {
    int                 magnif;
    POINT               wndpos;
    POINT               wndsize;
    POINT               magsize;
    POINT               magpos;
    WORD                caption_hite;
    ScreenBitmap        *screen;
    BOOL                looking;        /* currently zooming */
    BOOL                new_look;       /* doing a zoom but the magnifier has
                                           not been drawn yet */
    BOOL                sizing;         /* sizing the magnifier */
    BOOL                scrolling;      /* magnifier present for scrolling */
    HWND                hscroll;
    HWND                vscroll;
    HWND                zoomin;
    HWND                zoomout;
    int                 zoomincrement;
    HPEN                magnifpen;
}MainWndInfo;

typedef struct {
    BOOL                stickymagnifier;
    BOOL                topmost;
    BOOL                autorefresh;
    WORD                refresh_interval;
    POINT               maxwndsize;
}ZoomConfigInfo;

extern HANDLE           Instance;
extern HWND             MainWin;
extern ZoomConfigInfo   ConfigInfo;
extern char             *AppName;

/* zmproc.c */
WINEXPORT extern LRESULT CALLBACK ZOOMMainWndProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
extern void DoAutoRefresh( HWND mainhwnd, BOOL on );

/* zmscreen.c */
extern ScreenBitmap *InitScreenBitmap( HWND display );
extern void GetScreen( ScreenBitmap *info, POINT *pos, POINT *size, BOOL trim );
extern void DrawScreen( ScreenBitmap *info, HDC dc, POINT *dstpos, POINT *dstsize, POINT *srcpos, POINT *srcsize );
extern void CopyToClipBoard( MainWndInfo *info );
extern void FiniScreenBitmap( ScreenBitmap *info );

/* zmconfig.c */
extern void LoadConfig( void );
extern void SaveConfig( void );
extern void DoConfig( HWND hwnd );
