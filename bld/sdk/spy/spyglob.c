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


#include <stdio.h>
#include "spy.h"

WndConfigInfo   SpyMainWndInfo;
char            *SpyName;
char            near SpyPickClass[]="WatSpyPickClass";
char            *TitleBar;
char            *TitleBarULine;
int             TitleBarLen;
HWND            SpyListBox;
HWND            SpyListBoxTitle;
HWND            SpyMainWindow;
WORD            WindowCount;
HWND            *WindowList;
HANDLE          MyTask;
BOOL            SpyMessagesPaused;
BOOL            SpyMessagesAutoScroll=TRUE;
BOOL            AutoSaveConfig;
spystate        SpyState=NEITHER;
HANDLE          Instance;
HANDLE          ResInstance;
LPVOID          HandleMessageInst;
HMENU           SpyMenu;
statwnd         *StatusHdl;
filters         Filters =
                { {
                    { 1, 0, MC_CLIPBRD },
                    { 1, 0, MC_DDE },
                    { 1, 0, MC_IME },
                    { 1, 0, MC_INIT },
                    { 1, 0, MC_INPUT },
                    { 1, 0, MC_MDI },
                    { 1, 0, MC_MOUSE },
                    { 1, 0, MC_NC_MISC },
                    { 1, 0, MC_NC_MOUSE },
                    { 1, 0, MC_OTHER },
                    { 1, 0, MC_SYSTEM },
                    { 1, 0, MC_UNKNOWN },
                    { 1, 0, MC_USER },
                    { 1, 0, MC_WINDOW }
                } };
