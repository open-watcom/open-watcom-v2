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


#ifndef __WIN1632_INCLUDED__
#define __WIN1632_INCLUDED__
#if defined(__NT__) || defined(TWIN32)

#define GET_HINSTANCE( hwnd ) (HANDLE) GetWindowLong( hwnd, GWL_HINSTANCE )
#define GET_ID( hwnd )  (DWORD) GetWindowLong( hwnd, GWL_ID )
#define GET_CBWNDEXTRA( hwnd )  (DWORD) GetClassLong( hwnd, GCL_CBWNDEXTRA )
#define GET_CLASS_STYLE(hwnd)   GetClassLong( hwnd, GCL_STYLE )

#define __FAR
#define HOOKPROC        PROC
#define WINMAINENTRY    WINAPI
#define GET_EM_SETSEL_MPS(iStart, iEnd) (UINT)(iStart), (LONG)(iEnd)
#define GET_WM_COMMAND_CMD(wp, lp)      HIWORD(wp)
#define GET_WM_COMMAND_HWND(wp, lp)     (HWND)(lp)
#define GET_WM_COMMAND_MPS(id, hwnd, cmd) (UINT)MAKELONG(id, cmd), (LONG)(hwnd)
#define GET_WM_ACTIVATE_FACTIVE(wp, lp)     LOWORD(wp)
#define GET_WM_ACTIVATE_FMINIMIZED(wp, lp)  (BOOL)HIWORD(wp)
#define GET_WM_ACTIVATE_HWND(wp, lp)        (HWND)(lp)
#define GET_WM_MDIACTIVATE_FACTIVATE(hwnd, wp, lp)  (lp == (LONG)hwnd)
#define GET_WM_MDIACTIVATE_HWNDACTIVATE(wp, lp)  (HWND)(wp)
#define GET_WM_MDIACTIVATE_HWNDDEACTIVATE(wp, lp)  (HWND)(lp)
#define GET_WM_VSCROLL_POS( wp, lp )    HIWORD(wp)
#define GET_WM_VSCROLL_HWND( wp, lp )   (HWND)(lp)
#define GET_WM_VSCROLL_CODE( wp, lp )   LOWORD(wp)
#define GET_WM_HSCROLL_POS( wp, lp )    HIWORD(wp)
#define GET_WM_HSCROLL_HWND( wp, lp )   (HWND)(lp)
#define GET_WM_HSCROLL_CODE( wp, lp )   LOWORD(wp)
#define SET_HBRBACKGROUND( hw, br ) (HBRUSH) SetClassLong( hw, GCL_HBRBACKGROUND, (LONG)br )
#define SET_HICON( hw, ic ) (HICON) SetClassLong( hw, GCL_HICON, (LONG)ic )
#define SET_CLASSCURSOR(hwnd, cur) (LONG)SetClassLong( hwnd, GCL_HCURSOR, (LONG)cur )
#define MAKE_POINT( p, pnt ) (p).x = (signed short)LOWORD( pnt ), (p).y = (signed short)HIWORD( pnt )
#define GET_WM_MENUSELECT_HMENU(wp,lp)  (HMENU)(lp)
#define GET_WM_MENUSELECT_ITEM(wp,lp) LOWORD(wp)
#define GET_WM_MENUSELECT_FLAGS(wp,lp) HIWORD(wp)
#define GET_WM_PARENTNOTIFY_EVENT(wp,lp) LOWORD(wp)
#define GET_WM_PARENTNOTIFY_ID(wp,lp) HIWORD(wp)
#define GET_WM_PARENTNOTIFY_HANDLE(wp,lp) (lp)
#define GET_WM_PARENTNOTIFY_POINT(wp,lp) (lp)

#else

#define GET_HINSTANCE( hwnd ) (HINSTANCE) GetWindowWord( hwnd, GWW_HINSTANCE )
#define GET_ID( hwnd )  (WORD) GetWindowWord( hwnd, GWW_ID )
#define GET_CBWNDEXTRA( hwnd )  (WORD) GetClassWord( hwnd, GCW_CBWNDEXTRA)
#define GET_CLASS_STYLE(hwnd)   GetClassWord( hwnd, GCW_STYLE )

#ifndef APIENTRY
#define APIENTRY        FAR PASCAL
#endif
#ifndef __FAR
#define __FAR           __far
#endif
#define WINMAINENTRY    PASCAL
#define GET_EM_SETSEL_MPS(iStart, iEnd) 0, MAKELONG(iStart, iEnd)
#define GET_WM_COMMAND_CMD(wp, lp)      HIWORD(lp)
#define GET_WM_COMMAND_HWND(wp, lp)     (HWND)LOWORD(lp)
#define GET_WM_COMMAND_MPS(id, hwnd, cmd) (UINT)(id), MAKELONG(hwnd, cmd)
#define GET_WM_ACTIVATE_FACTIVE(wp, lp)     (wp)
#define GET_WM_ACTIVATE_FMINIMIZED(wp, lp)  (BOOL)HIWORD(lp)
#define GET_WM_ACTIVATE_HWND(wp, lp)        (HWND)LOWORD(lp)
#define GET_WM_MDIACTIVATE_FACTIVATE(hwnd, wp, lp)  (BOOL)(wp)
#define GET_WM_MDIACTIVATE_HWNDACTIVATE(wp, lp)  (HWND)HIWORD(lp)
#define GET_WM_MDIACTIVATE_HWNDDEACTIVATE(wp, lp)  (HWND)LOWORD(lp)
#define GET_WM_VSCROLL_POS( wp, lp )    LOWORD(lp)
#define GET_WM_VSCROLL_HWND( wp, lp )   HIWORD(lp)
#define GET_WM_VSCROLL_CODE( wp, lp )   (wp)
#define GET_WM_HSCROLL_POS( wp, lp )    LOWORD(lp)
#define GET_WM_HSCROLL_HWND( wp, lp )   HIWORD(lp)
#define GET_WM_HSCROLL_CODE( wp, lp )   (wp)
#define SET_HBRBACKGROUND( hw, br ) (HBRUSH) SetClassWord( hw, GCW_HBRBACKGROUND, br )
#define SET_HICON( hw, ic ) (HICON) SetClassWord( hw, GCW_HICON, ic )
#define SET_CLASSCURSOR(hwnd, cur) (WORD)SetClassWord( hwnd, GCW_HCURSOR, cur )
#define MAKE_POINT( p, pnt ) p = MAKEPOINT( pnt )
#define GET_WM_MENUSELECT_HMENU(wp,lp) (HMENU)HIWORD(lp)
#define GET_WM_MENUSELECT_ITEM(wp,lp) (wp)
#define GET_WM_MENUSELECT_FLAGS(wp,lp) LOWORD(lp)
#define GET_WM_PARENTNOTIFY_EVENT(wp,lp) LOWORD(wp)
#define GET_WM_PARENTNOTIFY_ID(wp,lp) HIWORD(lp)
#define GET_WM_PARENTNOTIFY_HANDLE(wp,lp) LOWORD(lp)
#define GET_WM_PARENTNOTIFY_POINT(wp,lp) (lp)

#endif

#if defined(__386__) || defined(__AXP__) || defined(__PPC__)
#define FARstricmp stricmp
#define FARstrcpy strcpy
#define FARmemcpy memcpy
#else
#define FARstricmp _fstricmp
#define FARstrcpy _fstrcpy
#define FARmemcpy _fmemcpy
#endif

#undef CALLBACK
#if defined( __WINDOWS_386__ )
#define CALLBACK        APIENTRY
#elif defined( UNIX )
#define CALLBACK
#else
#define CALLBACK        __export APIENTRY
#endif

#ifndef DECLSPEC
#if defined(__AXP__) || defined(__PPC__)
#define DECLSPEC        __declspec(dllexport)
#else
#define DECLSPEC
#endif
#endif

#endif
