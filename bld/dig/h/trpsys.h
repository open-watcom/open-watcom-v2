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
* Description:  TRAP file OS extension global variable/function declaration
*
****************************************************************************/


#if defined( __OS2__ )
extern bool     IsTrapFilePumpingMessageQueue( void );
extern char     TellHardMode( char hard );
  #if defined( _M_I86 )
extern void     TellHandles( void __far *hab, void __far *hwnd );
  #else
extern void     TellHandles( HAB hab, HWND hwnd );
  #endif
#elif defined( __DOS__ )
  #if defined( _M_I86 )
  #else
extern unsigned char    DPMICheck;

extern void     SaveOrigVectors( void );
extern void     RestoreOrigVectors( void );
  #endif
#elif defined( __NT__ )
extern void     TellHWND( HWND hwnd );
#elif defined( __WINDOWS__ )
extern int      HardModeRequired;

extern void     (TRAPENTRY*HookFunc)(LPVOID);
extern void     (TRAPENTRY*InfoFunction)(HWND);
extern void     (TRAPENTRY*UnLockInput)(void);
extern void     (TRAPENTRY*SetHardMode)(char);
extern void     DoHardModeCheck( void );
#else
#endif
