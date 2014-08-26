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


#ifndef WRCTL3D_INCLUDED
#define WRCTL3D_INCLUDED

#if defined( __WINDOWS__ ) || defined( __NT__ ) && !defined( _WIN64 )
#include <ctl3d.h>
#endif

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
WRDLLENTRY extern int      WRAPI WRCtl3DInit( HINSTANCE );
WRDLLENTRY extern void     WRAPI WRCtl3DFini( HINSTANCE );

WRDLLENTRY extern bool     WRAPI WRCtl3dSubclassDlg( HWND, WORD );
WRDLLENTRY extern bool     WRAPI WRCtl3dSubclassDlgAll( HWND );
WRDLLENTRY extern bool     WRAPI WRCtl3dSubclassDlgEx( HWND, DWORD );
WRDLLENTRY extern bool     WRAPI WRCtl3dSubclassDlgExAll( HWND );
WRDLLENTRY extern WORD     WRAPI WRCtl3dGetVer( void );
WRDLLENTRY extern bool     WRAPI WRCtl3dEnabled( void );
WRDLLENTRY extern HBRUSH   WRAPI WRCtl3dCtlColor( HDC, LONG );
WRDLLENTRY extern HBRUSH   WRAPI WRCtl3dCtlColorEx( UINT wm, WPARAM wParam, LPARAM lParam );
WRDLLENTRY extern bool     WRAPI WRCtl3dColorChange( void );
WRDLLENTRY extern bool     WRAPI WRCtl3dSubclassCtl( HWND );
WRDLLENTRY extern LONG     WRAPI WRCtl3dDlgFramePaint( HWND, UINT, WPARAM, LPARAM );
WRDLLENTRY extern bool     WRAPI WRCtl3dAutoSubclass( HANDLE );
WRDLLENTRY extern bool     WRAPI WRCtl3dRegister( HANDLE );
WRDLLENTRY extern bool     WRAPI WRCtl3dUnregister( HANDLE );
WRDLLENTRY extern VOID     WRAPI WRCtl3dWinIniChange( void );

#endif
