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
* Description:  CTL3D cover function prototypes.
*
****************************************************************************/


#ifndef _CTL3DCVR_H_INCLUDED
#define _CTL3DCVR_H_INCLUDED

#if defined( __WINDOWS__ ) || defined( __NT__ ) && !defined( _WIN64 )

#include <ctl3d.h>

#define C3D_EXPORT  WINAPI

extern int C3D_EXPORT       CvrCtl3DInit( HINSTANCE );
extern void C3D_EXPORT      CvrCtl3DFini( HINSTANCE );
extern bool C3D_EXPORT      CvrCtl3dSubclassDlg( HWND, WORD );
extern bool C3D_EXPORT      CvrCtl3dSubclassDlgAll( HWND );
extern bool C3D_EXPORT      CvrCtl3dSubclassDlgEx( HWND, DWORD );
extern bool C3D_EXPORT      CvrCtl3dSubclassDlgExAll( HWND );
extern WORD C3D_EXPORT      CvrCtl3dGetVer( void );
extern bool C3D_EXPORT      CvrCtl3dEnabled( void );
extern HBRUSH C3D_EXPORT    CvrCtl3dCtlColor( HDC, LONG );
extern HBRUSH C3D_EXPORT    CvrCtl3dCtlColorEx( UINT, WPARAM, LPARAM );
extern bool C3D_EXPORT      CvrCtl3dColorChange( void );
extern bool C3D_EXPORT      CvrCtl3dSubclassCtl( HWND );
extern LONG C3D_EXPORT      CvrCtl3dDlgFramePaint( HWND, UINT, WPARAM, LPARAM );
extern bool C3D_EXPORT      CvrCtl3dAutoSubclass( HINSTANCE );
extern bool C3D_EXPORT      CvrCtl3dRegister( HINSTANCE );
extern bool C3D_EXPORT      CvrCtl3dUnregister( HINSTANCE );
extern VOID C3D_EXPORT      CvrCtl3dWinIniChange( void );

#else

#define CvrCtl3DInit(i)                     false
#define CvrCtl3DFini(i)
#define CvrCtl3dSubclassDlg(hw,w)           false
#define CvrCtl3dSubclassDlgAll(hw)          false
#define CvrCtl3dSubclassDlgEx(hw,dw)        false
#define CvrCtl3dSubclassDlgExAll(hw)        false
#define CvrCtl3dGetVer()                    0
#define CvrCtl3dEnabled()                   false
#define CvrCtl3dCtlColor(dc,l)              ((HBRUSH)NULL)
#define CvrCtl3dCtlColorEx(u,wp,lp)         ((HBRUSH)NULL)
#define CvrCtl3dColorChange()               false
#define CvrCtl3dSubclassCtl(hw)             false
#define CvrCtl3dDlgFramePaint(hw,u,wp,lp)   0
#define CvrCtl3dAutoSubclass(i)             false
#define CvrCtl3dRegister(i)                 false
#define CvrCtl3dUnregister(i)               false
#define CvrCtl3dWinIniChange()

#endif

#endif /* _CTL3DCVR_H_INCLUDED */
