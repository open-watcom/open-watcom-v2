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

#include "ctl3d.h"
#include "wrglbl.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern int      WR_EXPORT    WRCtl3DInit( HINSTANCE );
extern void     WR_EXPORT   WRCtl3DFini( HINSTANCE );

extern BOOL     WR_EXPORT WRCtl3dSubclassDlg( HWND, WORD );
extern BOOL     WR_EXPORT WRCtl3dSubclassDlgEx( HWND, DWORD );
extern WORD     WR_EXPORT WRCtl3dGetVer( void );
extern BOOL     WR_EXPORT WRCtl3dEnabled( void );
extern HBRUSH   WR_EXPORT WRCtl3dCtlColor( HDC, LONG );
extern HBRUSH   WR_EXPORT WRCtl3dCtlColorEx( UINT wm, WPARAM wParam, LPARAM lParam );
extern BOOL     WR_EXPORT WRCtl3dColorChange( void );
extern BOOL     WR_EXPORT WRCtl3dSubclassCtl( HWND );
extern LONG     WR_EXPORT WRCtl3dDlgFramePaint( HWND, UINT, WPARAM, LPARAM );
extern BOOL     WR_EXPORT WRCtl3dAutoSubclass( HANDLE );
extern BOOL     WR_EXPORT WRCtl3dRegister( HANDLE );
extern BOOL     WR_EXPORT WRCtl3dUnregister( HANDLE );
extern VOID     WR_EXPORT WRCtl3dWinIniChange( void );

#endif
