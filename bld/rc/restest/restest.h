/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Resource test internal interface.
*
****************************************************************************/


#include <stdarg.h>
#include "bool.h"

#define MAIN_CLASS      "ResTestClass"
#define MENU_CLASS      "MenuWindowClass"

extern HINSTANCE        Instance;
extern HWND             MainHwnd;
extern HWND             AccelHwnd;
extern HACCEL           Accel;

LRESULT CALLBACK MainWndProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
void LBDump( HWND hwnd, char *str, WORD len );
void DisplayVerInfo( void );
void LBPrintf( HWND lb, char *str, ... );
void Error( char *heading, char *msg );
void DisplayStringTable( void );
void DisplayMenu( void );
BOOL RegisterMenuClass( void );
void DisplayDialog( void );
void DisplayData( bool rcdata );
void DisplayCursor( HWND hwnd );
void DisplayAccel( void );
void DisplayBitmap( void );
void DisplayIcon( HWND hwnd );
BOOL RegisterBitmapClass( void );
void DisplayMessageTable( void );
INT_PTR CALLBACK VerInfoDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
void DisplayStrings( HWND hwnd );
INT_PTR CALLBACK StrTableDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
LRESULT CALLBACK MenuWndProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
INT_PTR CALLBACK GetMenuNameDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
INT_PTR CALLBACK GetDialogNameDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
INT_PTR CALLBACK DispDialogDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
INT_PTR CALLBACK GetDataTypeDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
INT_PTR CALLBACK GetDataNameDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
INT_PTR CALLBACK DataDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
INT_PTR CALLBACK GetCursorNameDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
INT_PTR CALLBACK GetIconNameDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
INT_PTR CALLBACK GetAccelNameDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
LRESULT CALLBACK BitmapWndProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
INT_PTR CALLBACK GetBitmapNameDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
INT_PTR CALLBACK GetMsgNumDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
