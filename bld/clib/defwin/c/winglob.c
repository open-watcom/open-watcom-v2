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


#include "variety.h"
#include "win.h"

#if defined( __OS2__ )
HAB     _AnchorBlock;
HWND    _MainFrameWindow;
#else
#if 0
/* these literal strings were placed inline in the calls in winmain.c */
char _MenuFlush[] = "&Clear";
char _MenuWrite[] = "&Save As ...";
char _MenuSetClearInt[] = "Set &Lines Between Auto-Clears ...";
char _MenuExit[] = "E&xit";
char _MenuFile[] = "&File";
char _MenuCopy[] = "&Copy";
char _MenuEdit[] = "&Edit";
char _MenuHelp[] = "&Help";
char _MenuWindows[] = "&Windows";
char _MenuAbout[] = "&About...";
#endif
HFONT _FixedFont;
HMENU _MainMenu,_SubMenuEdit,_SubMenuWindows;

#define IL 0x54
#define IM 0xA8
#define IH 0xFF

DWORD _ColorMap[16] = {
RGB(0,0,0),
RGB(0,0,IM),
RGB(0,IM,0),
RGB(0,IM,IM),
RGB(IM,0,0),
RGB(IM,0,IM),
RGB(IM,IL,0),
RGB(IM,IM,IM),
RGB(IL,IL,IL),
RGB(IL,IL,IH),
RGB(IL,IH,IL),
RGB(IL,IH,IH),
RGB(IH,IL,IL),
RGB(IH,IL,IH),
RGB(IH,IH,IL),
RGB(IH,IH,IH)
};
#endif
HWND            _MainWindow;
int             _MainWindowDestroyed = 0;
LPMWDATA        _MainWindowData;
char            *_ClassName;
DWORD           _AutoClearLines = DEFAULT_CLEAR_INTERVAL;
BOOL            _GotEOF=FALSE;
