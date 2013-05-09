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


#define MSG_ABOUT       100
#define MSG_EXIT        101
#define MSG_PORT        102
#define MSG_SWITCHTOPGM 103
#define MSG_CLEAR       104
#define RADIO_1         200
#define RADIO_2         201
#define RADIO_3         202
#define RADIO_4         203
#define EDIT_1          300
#define SELECT_1        301
#define MAX_BAUD        5
#define BAUD_1200       401
#define BAUD_2400       402
#define BAUD_4800       403
#define BAUD_9600       404
#define BAUD_19200      405

#define MAX_PPORTS      3
#define MAX_SERVER_NAME 80

WINEXPORT LRESULT CALLBACK MainDriver( HWND, UINT, WPARAM, LPARAM );
void WinMessage( char *, ... );

extern HANDLE CProcHandle;
extern HWND DebugWindow;
extern HWND MainWindowHandle;
extern HWND PortChildren[MAX_PPORTS];
extern HWND EditChild;
extern FARPROC OldEditProc;
extern int FakeKeyPress;
extern int FakeKey;
extern int SessionStarted;
extern int RemoteLinkObtained;
extern int CurrentPort;
