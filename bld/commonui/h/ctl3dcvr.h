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
#endif

#if defined( __WINDOWS__ ) || defined( __NT__ ) && !defined( _WIN64 )
#define C3D_EXPORT  WINAPI
#else
#define C3D_EXPORT
#endif

extern int C3D_EXPORT       CvrCtl3DInit( WPI_INST );
extern void C3D_EXPORT      CvrCtl3DFini( WPI_INST );
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
extern bool C3D_EXPORT      CvrCtl3dAutoSubclass( WPI_INST );
extern bool C3D_EXPORT      CvrCtl3dRegister( WPI_INST );
extern bool C3D_EXPORT      CvrCtl3dUnregister( WPI_INST );
extern VOID C3D_EXPORT      CvrCtl3dWinIniChange( void );

#endif /* _CTL3DCVR_H_INCLUDED */
