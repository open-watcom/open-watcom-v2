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
* Description:  Covers for several base Windows API functions.
*
****************************************************************************/


#ifdef DLL32
typedef LPVOID __far *LPLPVOID;
#else
typedef LPVOID *LPLPVOID;
#endif

extern void     GetAlias( LPLPVOID name );
extern void     ReleaseAlias( LPVOID orig, LPVOID ptr );
extern BOOL     FAR PASCAL __GetMessage( LPMSG msg, HWND a, WORD b, WORD c );
extern BOOL     FAR PASCAL __PeekMessage( LPMSG msg, HWND a, WORD b, WORD c, WORD d );
#if 0
extern BOOL     FAR PASCAL __RegisterClass(LPWNDCLASS wc);
#endif
extern BOOL     FAR PASCAL __ModifyMenu(HMENU a, WORD b, WORD fl, WORD d, LPSTR z);
extern BOOL     FAR PASCAL __InsertMenu(HMENU a, WORD b, WORD fl, WORD d, LPSTR z);
extern BOOL     FAR PASCAL __AppendMenu(HMENU a, WORD fl, WORD c, LPSTR z);
extern int      FAR PASCAL __Escape(HDC a, int b, int c, LPSTR d, LPSTR e);
extern int      FAR PASCAL __GetInstanceData( HANDLE a, DWORD offset, int len );
extern LPSTR    FAR PASCAL __AnsiPrev(LPSTR a, LPSTR b);
extern LPSTR    FAR PASCAL __AnsiNext(LPSTR a);
extern int      FAR PASCAL __StartDoc( HDC hdc, DOCINFO FAR *di);
extern BOOL     FAR PASCAL __WinHelp( HWND hwnd, LPCSTR hfile, UINT cmd, DWORD data );
