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


#include<stdarg.h>

#define MAIN_CLASS      "ResTestClass"
#define MENU_CLASS      "MenuWindowClass"

extern HANDLE           Instance;
extern HWND             MainHwnd;
extern HWND             AccelHwnd;
extern HACCEL           Accel;

BOOL __export FAR PASCAL MainWndProc( HWND hwnd, UINT msg, UINT wparam,
                                    LONG lparam );
void LBDump( HWND hwnd, char *str, WORD len );
void DisplayVerInfo( void );
void LBPrintf( HWND lb, char *str, ... );
void Error( char *heading, char *msg );
void DisplayStringTable( void );
void DisplayMenu( void );
BOOL RegisterMenuClass( void );
void DisplayDialog( void );
void DisplayData( BOOL rcdata );
void DisplayCursor( HWND hwnd );
void DisplayAccel( void );
void DisplayBitmap( void );
void DisplayIcon( HWND hwnd );
BOOL RegisterBitmapClass( void );
void DisplayMessageTable( void );
