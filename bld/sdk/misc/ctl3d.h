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


/*-----------------------------------------------------------------------
|       CTL3D.DLL
|
|       Adds 3d effects to Windows controls
|
|       See ctl3d.doc for info
|
-----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

#if defined( __WINDOWS_386__ )

BOOL Init32Ctl3d( void );
void Fini32Ctl3d( void );
extern HINDIR DLL32Ctl3dAutoSubclass;
extern HINDIR DLL32Ctl3dSubclassDlg;
extern HINDIR DLL32Ctl3dRegister;
extern HINDIR DLL32Ctl3dUnregister;
extern HINDIR DLL32Ctl3dColorChange;
extern HINDIR DLL32Ctl3dSubclassCtl;
extern HINDIR DLL32Ctl3dCtlColorEx;
#define Ctl3dAutoSubclass( a ) InvokeIndirectFunction( DLL32Ctl3dAutoSubclass, a )
#define Ctl3dSubclassDlg( a, b ) InvokeIndirectFunction( DLL32Ctl3dSubclassDlg, a, b )
#define Ctl3dRegister( a ) InvokeIndirectFunction( DLL32Ctl3dRegister, a )
#define Ctl3dUnregister( a ) InvokeIndirectFunction( DLL32Ctl3dUnregister, a )
#define Ctl3dColorChange( ) InvokeIndirectFunction( DLL32Ctl3dColorChange )
#define Ctl3dSubclassCtl( a ) InvokeIndirectFunction( DLL32Ctl3dSubclassCtl, a )
#define Ctl3dCtlColorEx( a, b, c ) InvokeIndirectFunction( DLL32Ctl3dCtlColorEx, a, b, c )

#elif defined( __WINDOWS__ ) || defined( __NT__ )

BOOL WINAPI Ctl3dSubclassDlg(HWND, WORD);
BOOL WINAPI Ctl3dSubclassDlgEx(HWND, DWORD);
WORD WINAPI Ctl3dGetVer(void);
BOOL WINAPI Ctl3dEnabled(void);
HBRUSH WINAPI Ctl3dCtlColor(HDC, LONG); // ARCHAIC, use Ctl3dCtlColorEx
HBRUSH WINAPI Ctl3dCtlColorEx(UINT wm, WPARAM wParam, LPARAM lParam);
BOOL WINAPI Ctl3dColorChange(void);
BOOL WINAPI Ctl3dSubclassCtl(HWND);
LONG WINAPI Ctl3dDlgFramePaint(HWND, UINT, WPARAM, LPARAM);

BOOL WINAPI Ctl3dAutoSubclass(HANDLE);

BOOL WINAPI Ctl3dRegister(HANDLE);
BOOL WINAPI Ctl3dUnregister(HANDLE);

//begin DBCS: far east short cut key support
VOID WINAPI Ctl3dWinIniChange(void);
//end DBCS

#else
#define Ctl3dSubclassDlg(a, b)
#define Ctl3dSubclassDlgEx(a, b)
#define Ctl3dGetVer()
#define Ctl3dEnabled()
#define Ctl3dCtlColor( a, b )
#define Ctl3dCtlColorEx( a, b, c )
#define Ctl3dColorChange()
#define Ctl3dSubclassCtl( a )
#define Ctl3dAutoSubclass( a )
#define Ctl3dRegister( a )
#define Ctl3dUnregister( a )
#endif

/* Ctl3dSubclassDlg3d flags */
#define CTL3D_BUTTONS           0x0001
#define CTL3D_LISTBOXES         0x0002
#define CTL3D_EDITS                     0x0004
#define CTL3D_COMBOS                    0x0008
#define CTL3D_STATICTEXTS       0x0010
#define CTL3D_STATICFRAMES      0x0020

#define CTL3D_NODLGWINDOW       0x00010000
#define CTL3D_ALL                               0xffff

#define WM_DLGBORDER (WM_USER+3567)
/* WM_DLGBORDER *(int FAR *)lParam return codes */
#define CTL3D_NOBORDER          0
#define CTL3D_BORDER                    1

#define WM_DLGSUBCLASS (WM_USER+3568)
/* WM_DLGSUBCLASS *(int FAR *)lParam return codes */
#define CTL3D_NOSUBCLASS        0
#define CTL3D_SUBCLASS          1

/* Resource ID for 3dcheck.bmp (for .lib version of ctl3d) */
#define CTL3D_3DCHECK 26567


#ifdef __cplusplus
}
#endif

