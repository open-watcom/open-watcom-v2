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
* Description:  CTL3D covers to avoid static linking and DLL dependency.
*
****************************************************************************/


#include "commonui.h"
#include "bool.h"
#include "watcom.h"
#include "ctl3dcvr.h"
#include "ctl3d32.h"

#if defined( __WINDOWS__ ) || defined( __NT__ ) && !defined( _WIN64 )

 #ifdef __WINDOWS_386__
    #define _DLLFAR     PASCAL
 #else
    #define _DLLFAR     FAR WINAPI
 #endif

typedef BOOL    (_DLLFAR *LPFN_Ctl3dSubclassDlg)( HWND, WORD );
typedef BOOL    (_DLLFAR *LPFN_Ctl3dSubclassDlgEx)( HWND, DWORD );
typedef WORD    (_DLLFAR *LPFN_Ctl3dGetVer)( void );
typedef BOOL    (_DLLFAR *LPFN_Ctl3dEnabled)( void );
typedef HBRUSH  (_DLLFAR *LPFN_Ctl3dCtlColor)( HDC, LONG );
typedef HBRUSH  (_DLLFAR *LPFN_Ctl3dCtlColorEx)( UINT, WPARAM, LPARAM );
typedef BOOL    (_DLLFAR *LPFN_Ctl3dColorChange)( void );
typedef BOOL    (_DLLFAR *LPFN_Ctl3dSubclassCtl)( HWND );
typedef LONG    (_DLLFAR *LPFN_Ctl3dDlgFramePaint)( HWND, UINT, WPARAM, LPARAM );
typedef BOOL    (_DLLFAR *LPFN_Ctl3dAutoSubclass)( HINSTANCE );
typedef BOOL    (_DLLFAR *LPFN_Ctl3dRegister)( HINSTANCE );
typedef BOOL    (_DLLFAR *LPFN_Ctl3dUnregister)( HINSTANCE );
typedef VOID    (_DLLFAR *LPFN_Ctl3dWinIniChange)( void );

static LPFN_Ctl3dSubclassDlg    cvrCtl3dSubclassDlg     = NULL;
static LPFN_Ctl3dSubclassDlgEx  cvrCtl3dSubclassDlgEx   = NULL;
static LPFN_Ctl3dGetVer         cvrCtl3dGetVer          = NULL;
static LPFN_Ctl3dEnabled        cvrCtl3dEnabled         = NULL;
static LPFN_Ctl3dCtlColor       cvrCtl3dCtlColor        = NULL;
static LPFN_Ctl3dCtlColorEx     cvrCtl3dCtlColorEx      = NULL;
static LPFN_Ctl3dColorChange    cvrCtl3dColorChange     = NULL;
static LPFN_Ctl3dSubclassCtl    cvrCtl3dSubclassCtl     = NULL;
static LPFN_Ctl3dDlgFramePaint  cvrCtl3dDlgFramePaint   = NULL;
static LPFN_Ctl3dAutoSubclass   cvrCtl3dAutoSubclass    = NULL;
static LPFN_Ctl3dRegister       cvrCtl3dRegister        = NULL;
static LPFN_Ctl3dUnregister     cvrCtl3dUnregister      = NULL;
static LPFN_Ctl3dWinIniChange   cvrCtl3dWinIniChange    = NULL;

 #if defined( __WINDOWS_386__ )

BOOL _DLLFAR _CB_Ctl3dUnregister( HANDLE h )
{
    return( Ctl3dUnregister( h ) );
}

BOOL _DLLFAR _CB_Ctl3dSubclassDlg( HWND h, WORD w )
{
    return( Ctl3dSubclassDlg( h, w ) );
}

BOOL _DLLFAR _CB_Ctl3dColorChange( void )
{
    return( Ctl3dColorChange() );
}

BOOL _DLLFAR _CB_Ctl3dSubclassCtl( HWND h )
{
    return( Ctl3dSubclassCtl( h ) );
}

HBRUSH _DLLFAR _CB_Ctl3dCtlColorEx(UINT wm, WPARAM wp, LPARAM lp )
{
  #if 0
    return( Ctl3dCtlColorEx( wm, wp, lp ) );
  #else
    return( NULL );
  #endif
}

 #endif

#if defined( __WINDOWS_386__ )
static bool                 dll32Ctl3dOpen = false;
#else
static HINSTANCE            ctlDLLLib = NULL;
#endif

/*
 * CvrCtl3DDLLFini
 */
static void CvrCtl3DDLLFini( void )
{
 #if defined( __WINDOWS_386__ )
    cvrCtl3dUnregister      = NULL;
    cvrCtl3dSubclassDlg     = NULL;
    cvrCtl3dSubclassCtl     = NULL;
    cvrCtl3dColorChange     = NULL;
    cvrCtl3dCtlColorEx      = NULL;
 #else
    cvrCtl3dSubclassDlg     = NULL;
    cvrCtl3dSubclassDlgEx   = NULL;
    cvrCtl3dGetVer          = NULL;
    cvrCtl3dEnabled         = NULL;
    cvrCtl3dCtlColor        = NULL;
    cvrCtl3dCtlColorEx      = NULL;
    cvrCtl3dColorChange     = NULL;
    cvrCtl3dSubclassCtl     = NULL;
    cvrCtl3dDlgFramePaint   = NULL;
    cvrCtl3dAutoSubclass    = NULL;
    cvrCtl3dRegister        = NULL;
    cvrCtl3dUnregister      = NULL;
    cvrCtl3dWinIniChange    = NULL;
    if( ctlDLLLib != (HINSTANCE)NULL ) {
        FreeLibrary( ctlDLLLib );
    }
 #endif

} /* CvrCtl3DDLLFini */

/*
 * CvrCtl3DDLLInit
 */
static int CvrCtl3DDLLInit( void )
{
 #if defined( __WINDOWS_386__ )
    cvrCtl3dUnregister  = &_CB_Ctl3dUnregister;
    cvrCtl3dSubclassDlg = &_CB_Ctl3dSubclassDlg;
    cvrCtl3dSubclassCtl = &_CB_Ctl3dSubclassCtl;
    cvrCtl3dColorChange = &_CB_Ctl3dColorChange;
    cvrCtl3dCtlColorEx  = &_CB_Ctl3dCtlColorEx;
 #else
  #ifdef __NT__
    ctlDLLLib = LoadLibrary( "CTL3D32.DLL" );
  #else
    UINT    uErrMode;

    /* Use SetErrorMode to prevent annoying error popups. */
    uErrMode = SetErrorMode( SEM_NOOPENFILEERRORBOX );
    ctlDLLLib = LoadLibrary( "CTL3DV2.DLL" );
    SetErrorMode( uErrMode );
  #endif

    if( ctlDLLLib == (HINSTANCE)NULL ) {
        return( FALSE );
    }

    cvrCtl3dSubclassDlg     = (LPFN_Ctl3dSubclassDlg)GetProcAddress( ctlDLLLib, (LPCSTR)(pointer_int)2 );
    cvrCtl3dSubclassDlgEx   = (LPFN_Ctl3dSubclassDlgEx)GetProcAddress( ctlDLLLib, (LPCSTR)(pointer_int)21 );
    cvrCtl3dGetVer          = (LPFN_Ctl3dGetVer)GetProcAddress( ctlDLLLib, (LPCSTR)(pointer_int)1 );
    cvrCtl3dEnabled         = (LPFN_Ctl3dEnabled)GetProcAddress( ctlDLLLib, (LPCSTR)(pointer_int)5 );
    cvrCtl3dCtlColor        = (LPFN_Ctl3dCtlColor)GetProcAddress( ctlDLLLib, (LPCSTR)(pointer_int)4 );
    cvrCtl3dCtlColorEx      = (LPFN_Ctl3dCtlColorEx)GetProcAddress( ctlDLLLib, (LPCSTR)(pointer_int)18 );
    cvrCtl3dColorChange     = (LPFN_Ctl3dColorChange)GetProcAddress( ctlDLLLib, (LPCSTR)(pointer_int)6 );
    cvrCtl3dSubclassCtl     = (LPFN_Ctl3dSubclassCtl)GetProcAddress( ctlDLLLib, (LPCSTR)(pointer_int)3 );
    cvrCtl3dDlgFramePaint   = (LPFN_Ctl3dDlgFramePaint)GetProcAddress( ctlDLLLib, (LPCSTR)(pointer_int)20 );
    cvrCtl3dAutoSubclass    = (LPFN_Ctl3dAutoSubclass)GetProcAddress( ctlDLLLib, (LPCSTR)(pointer_int)16 );
    cvrCtl3dRegister        = (LPFN_Ctl3dRegister)GetProcAddress( ctlDLLLib, (LPCSTR)(pointer_int)12 );
    cvrCtl3dUnregister      = (LPFN_Ctl3dUnregister)GetProcAddress( ctlDLLLib, (LPCSTR)(pointer_int)13 );
    cvrCtl3dWinIniChange    = (LPFN_Ctl3dWinIniChange)GetProcAddress( ctlDLLLib, (LPCSTR)(pointer_int)22 );

    if( cvrCtl3dSubclassDlg == NULL || cvrCtl3dSubclassDlgEx == NULL ||
        cvrCtl3dGetVer == NULL || cvrCtl3dEnabled == NULL || cvrCtl3dCtlColor == NULL ||
        cvrCtl3dCtlColorEx == NULL || cvrCtl3dColorChange == NULL ||
        cvrCtl3dSubclassCtl == NULL || cvrCtl3dDlgFramePaint == NULL ||
        cvrCtl3dAutoSubclass == NULL || cvrCtl3dRegister == NULL ||
        cvrCtl3dUnregister == NULL || cvrCtl3dWinIniChange == NULL ) {
        CvrCtl3DDLLFini();
        return( FALSE );
    }
 #endif

    return( TRUE );

} /* CvrCtl3DDLLInit */

/*
 * CvrCtl3dSubclassDlg
 */
bool C3D_EXPORT CvrCtl3dSubclassDlg( HWND hwnd, WORD w )
{
    if( cvrCtl3dSubclassDlg != NULL ) {
        return( cvrCtl3dSubclassDlg( hwnd, w ) != 0 );
    }
    return( false );

} /* CvrCtl3dSubclassDlg */

/*
 * CvrCtl3dSubclassDlgAll
 */
bool C3D_EXPORT CvrCtl3dSubclassDlgAll( HWND hwnd )
{
    if( cvrCtl3dSubclassDlg != NULL ) {
        return( cvrCtl3dSubclassDlg( hwnd, CTL3D_ALL ) != 0 );
    }
    return( false );

} /* CvrCtl3dSubclassDlgAll */

/*
 * CvrCtl3dSubclassDlgEx
 */
bool C3D_EXPORT CvrCtl3dSubclassDlgEx( HWND hwnd, DWORD dw )
{
    if( cvrCtl3dSubclassDlgEx != NULL ) {
        return( cvrCtl3dSubclassDlgEx( hwnd, dw ) != 0 );
    }
    return( false );

} /* CvrCtl3dSubclassDlgEx */

/*
 * CvrCtl3dSubclassDlgExAll
 */
bool C3D_EXPORT CvrCtl3dSubclassDlgExAll( HWND hwnd )
{
    if( cvrCtl3dSubclassDlgEx != NULL ) {
        return( cvrCtl3dSubclassDlgEx( hwnd, CTL3D_ALL ) != 0 );
    }
    return( false );

} /* CvrCtl3dSubclassDlgExAll */

/*
 * CvrCtl3dGetVer
 */
WORD C3D_EXPORT CvrCtl3dGetVer( void )
{
    if( cvrCtl3dGetVer != NULL ) {
        return( cvrCtl3dGetVer() );
    }
    return( 0 );

} /* CvrCtl3dGetVer */

/*
 * CvrCtl3dEnabled
 */
bool C3D_EXPORT CvrCtl3dEnabled( void )
{
    if( cvrCtl3dEnabled != NULL ) {
        return( cvrCtl3dEnabled() != 0 );
    }
    return( false );

} /* CvrCtl3dEnabled */

/*
 * CvrCtl3dCtlColor
 */
HBRUSH C3D_EXPORT CvrCtl3dCtlColor( HDC dc, LONG l )
{
    if( cvrCtl3dCtlColor != NULL ) {
        return( cvrCtl3dCtlColor( dc, l ) );
    }
    return( (HBRUSH)NULL );

} /* CvrCtl3dCtlColor */

/*
 * CvrCtl3dCtlColorEx
 */
HBRUSH C3D_EXPORT CvrCtl3dCtlColorEx( UINT wm, WPARAM wParam, LPARAM lParam )
{
    if( cvrCtl3dCtlColorEx != NULL ) {
        return( cvrCtl3dCtlColorEx( wm, wParam, lParam ) );
    }
    return( (HBRUSH)NULL );

} /* CvrCtl3dCtlColorEx */

/*
 * CvrCtl3dColorChange
 */
bool C3D_EXPORT CvrCtl3dColorChange( void )
{
    if( cvrCtl3dColorChange != NULL ) {
        return( cvrCtl3dColorChange() != 0 );
    }
    return( false );

} /* CvrCtl3dColorChange */

/*
 * CvrCtl3dSubclassCtl
 */
bool C3D_EXPORT CvrCtl3dSubclassCtl( HWND hwnd )
{
    if( cvrCtl3dSubclassCtl != NULL ) {
        return( cvrCtl3dSubclassCtl( hwnd ) != 0 );
    }
    return( false );

} /* CvrCtl3dSubclassCtl */

/*
 * CvrCtl3dDlgFramePaint
 */
LONG C3D_EXPORT CvrCtl3dDlgFramePaint( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
    if( cvrCtl3dDlgFramePaint != NULL ) {
        return( cvrCtl3dDlgFramePaint( hwnd, msg, wp, lp ) );
    }
    return( 0L );

} /* CvrCtl3dDlgFramePaint */

/*
 * CvrCtl3dAutoSubclass
 */
bool C3D_EXPORT CvrCtl3dAutoSubclass( HINSTANCE hndl )
{
    if( cvrCtl3dAutoSubclass != NULL ) {
        return( cvrCtl3dAutoSubclass( hndl ) != 0 );
    }
    return( false );

} /* CvrCtl3dAutoSubclass */

/*
 * CvrCtl3dRegister
 */
bool C3D_EXPORT CvrCtl3dRegister( HINSTANCE hndl )
{
    if( cvrCtl3dRegister != NULL ) {
        return( cvrCtl3dRegister( hndl ) != 0 );
    }
    return( false );

} /* CvrCtl3dRegister */

/*
 * CvrCtl3dUnregister
 */
bool C3D_EXPORT CvrCtl3dUnregister( HINSTANCE inst )
{
    if( cvrCtl3dUnregister != NULL ) {
        return( cvrCtl3dUnregister( inst ) != 0 );
    }
    return( false );

} /* CvrCtl3dUnregister */

/*
 * CvrCtl3dWinIniChange
 */
void C3D_EXPORT CvrCtl3dWinIniChange( void )
{
    if( cvrCtl3dWinIniChange != NULL ) {
        cvrCtl3dWinIniChange();
    }

} /* CvrCtl3dWinIniChange */

/*
 * CvrCtl3DInit
 */
int C3D_EXPORT CvrCtl3DInit( HINSTANCE inst )
{
 #if defined( __WINDOWS_386__ )
    if( !dll32Ctl3dOpen ) {
        if( Init32Ctl3d() ) {
            dll32Ctl3dOpen = true;
        } else {
            return( FALSE );
        }
    }
 #else
    DWORD       ver;
    BYTE        vm;

    ver = GetVersion();
    vm = (BYTE)(ver & 0x000000FF);
    if( vm >= 0x04 ) {
        return( TRUE );
    }
 #endif
    inst = inst;
    CvrCtl3DDLLInit();
    return( TRUE );

} /* CvrCtl3DInit */

/*
 * CvrCtl3DFini
 */
void C3D_EXPORT CvrCtl3DFini( HINSTANCE inst )
{
    inst = inst;
    CvrCtl3DDLLFini();
 #if defined( __WINDOWS_386__ )
    if( dll32Ctl3dOpen ) {
        Fini32Ctl3d();
        dll32Ctl3dOpen = false;
    }
 #endif

} /* CvrCtl3DFini */

#endif
