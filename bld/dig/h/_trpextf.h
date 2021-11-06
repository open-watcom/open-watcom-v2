/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  OS specific interface functions prototypes
*
****************************************************************************/


/*
 * Trap OS specific interface functions prototype used only for local debug
 */
/*        name              ret     parms                       loc ret loc parms   */
#if defined( __OS2__ )
  #if defined( _M_I86 )
    pick( TellHandles,      void, (void __far *,void __far *),  bool,   (void __far *, void __far *) )
  #else
    pick( TellHandles,      void, (HAB, HWND),                  bool,   (HAB, HWND) )
  #endif
    pick( TellHardMode,     char, (char),                       char,   (char) )
#elif defined( __NT__ )
    pick( InfoFunction,     void, (HWND),                       bool,   (HWND) )
    pick( InterruptProgram, void, (void),                       bool,   (void) )
    pick( Terminate,        bool, (void),                       bool,   (void) )
#elif defined( __WINDOWS__ )
    pick( InfoFunction,     void, (HWND),                       bool,   (HWND) )
    pick( GetHwndFunc,      HWND, (void),                       HWND,   (void) )
    pick( InputHook,        void, (event_hook_fn *),            void,   (event_hook_fn *) )
    pick( HardModeCheck,    bool, (void),                       void,   (void) )
    pick( SetHardMode,      void, (bool),                       void,   (bool) )
    pick( UnLockInput,      void, (void),                       void,   (void) )
#endif
