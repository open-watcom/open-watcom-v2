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


extern void DUIUpdate( update_list flags );
extern void DUIStatusText( char *text );
extern void DUIMsgBox( char *text );
extern bool DUIDlgTxt( char *text );
extern void DUIInfoBox( char *text );
extern void DUIStop();
extern void DUIFini();
extern void DUIInitHistory();
extern void DUIFiniHistory();
extern bool DUIClose();
extern void DUIInit();
extern void DUIFreshAll();
extern bool DUIStopRefresh( bool ok );
extern void DUIShow();
extern void DUIWndUser();
extern void DUIWndDebug();
extern void DUIShowLogWindow();
extern void DUIRedrawRegisters();
extern int DUIGetMonitorType();
extern int DUIScreenSizeX();
extern int DUIScreenSizeY();
extern void DUIRedrawSources();
extern void DUIErrorBox( char *buff );
extern void DUIArrowCursor();
extern char *DUILoadString( int i );
extern bool DUIAskIfAsynchOk();
extern void DUIFlushKeys();
extern void DUIPlayDead( bool );
extern void DUISysStart();
extern void DUISysEnd( bool pause );
extern void DUIRingBell();
extern void DUIProcPendingPaint(void);
extern bool DUIInfoRelease(void);
extern void *DUIHourGlass( void *x );
extern void DUIEnterCriticalSection();
extern void DUIExitCriticalSection();
extern void DUIInitLiterals();
extern void DUIFiniLiterals();
extern bool DUIIsDBCS();
extern int DUIEnvLkup(char *,char *, int);
extern void DUIDirty();
extern void DUISrcOrAsmInspect( address );
extern void DUIAddrInspect( address );
extern bool DUICopyCancelled( void * );
extern void DUICopySize( void *, long );
extern void DUICopyCopied( void *, long );

