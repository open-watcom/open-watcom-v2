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


#include "precomp.h"
#include "ctl3dcvr.h"

typedef BOOL    (WINAPI *LPFN_Ctl3dSubclassDlg)( HWND, WORD );
typedef BOOL    (WINAPI *LPFN_Ctl3dSubclassDlgEx)( HWND, DWORD );
typedef WORD    (WINAPI *LPFN_Ctl3dGetVer)( void );
typedef BOOL    (WINAPI *LPFN_Ctl3dEnabled)( void );
typedef HBRUSH  (WINAPI *LPFN_Ctl3dCtlColor)( HDC, LONG );
typedef HBRUSH  (WINAPI *LPFN_Ctl3dCtlColorEx)( UINT, WPARAM, LPARAM );
typedef BOOL    (WINAPI *LPFN_Ctl3dColorChange)( void );
typedef BOOL    (WINAPI *LPFN_Ctl3dSubclassCtl)( HWND );
typedef LONG    (WINAPI *LPFN_Ctl3dDlgFramePaint)( HWND, UINT, WPARAM, LPARAM );
typedef BOOL    (WINAPI *LPFN_Ctl3dAutoSubclass)( HANDLE );
typedef BOOL    (WINAPI *LPFN_Ctl3dRegister)( HANDLE );
typedef BOOL    (WINAPI *LPFN_Ctl3dUnregister)( HANDLE );
typedef VOID    (WINAPI *LPFN_Ctl3dWinIniChange)( void );

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

static HINSTANCE                ctlDLLLib = NULL;

/*
 * CvrCtl3DDLLFini
 */
void CvrCtl3DDLLFini( void )
{
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

} /* CvrCtl3DDLLFini */

/*
 * CvrCtl3DDLLInit
 */
static int CvrCtl3DDLLInit( void )
{
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

    cvrCtl3dSubclassDlg     = (LPFN_Ctl3dSubclassDlg)GetProcAddress( ctlDLLLib, (LPCSTR)2 );
    cvrCtl3dSubclassDlgEx   = (LPFN_Ctl3dSubclassDlgEx)GetProcAddress( ctlDLLLib, (LPCSTR)21 );
    cvrCtl3dGetVer          = (LPFN_Ctl3dGetVer)GetProcAddress( ctlDLLLib, (LPCSTR)1 );
    cvrCtl3dEnabled         = (LPFN_Ctl3dEnabled)GetProcAddress( ctlDLLLib, (LPCSTR)5 );
    cvrCtl3dCtlColor        = (LPFN_Ctl3dCtlColor)GetProcAddress( ctlDLLLib, (LPCSTR)4 );
    cvrCtl3dCtlColorEx      = (LPFN_Ctl3dCtlColorEx)GetProcAddress( ctlDLLLib, (LPCSTR)18 );
    cvrCtl3dColorChange     = (LPFN_Ctl3dColorChange)GetProcAddress( ctlDLLLib, (LPCSTR)6 );
    cvrCtl3dSubclassCtl     = (LPFN_Ctl3dSubclassCtl)GetProcAddress( ctlDLLLib, (LPCSTR)3 );
    cvrCtl3dDlgFramePaint   = (LPFN_Ctl3dDlgFramePaint)GetProcAddress( ctlDLLLib, (LPCSTR)20 );
    cvrCtl3dAutoSubclass    = (LPFN_Ctl3dAutoSubclass)GetProcAddress( ctlDLLLib, (LPCSTR)16 );
    cvrCtl3dRegister        = (LPFN_Ctl3dRegister)GetProcAddress( ctlDLLLib, (LPCSTR)12 );
    cvrCtl3dUnregister      = (LPFN_Ctl3dUnregister)GetProcAddress( ctlDLLLib, (LPCSTR)13 );
    cvrCtl3dWinIniChange    = (LPFN_Ctl3dWinIniChange)GetProcAddress( ctlDLLLib, (LPCSTR)22 );

    if( cvrCtl3dSubclassDlg == NULL || cvrCtl3dSubclassDlgEx == NULL ||
        cvrCtl3dGetVer == NULL || cvrCtl3dEnabled == NULL || cvrCtl3dCtlColor == NULL ||
        cvrCtl3dCtlColorEx == NULL || cvrCtl3dColorChange == NULL ||
        cvrCtl3dSubclassCtl == NULL || cvrCtl3dDlgFramePaint == NULL ||
        cvrCtl3dAutoSubclass == NULL || cvrCtl3dRegister == NULL ||
        cvrCtl3dUnregister == NULL || cvrCtl3dWinIniChange == NULL ) {
        CvrCtl3DDLLFini();
        return( FALSE );
    }

    return( TRUE );

} /* CvrCtl3DDLLInit */

/*
 * CvrCtl3DInit
 */
int C3D_EXPORT CvrCtl3DInit( HINSTANCE inst )
{
    DWORD       ver;
    BYTE        vm;

    inst = inst;
    ver = GetVersion();
    vm = (BYTE)(ver & 0x000000FF);
    if( vm >= 0x04 ) {
        return( TRUE );
    }

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

} /* CvrCtl3DFini */

/*
 * CvrCtl3dSubclassDlg
 */
BOOL C3D_EXPORT CvrCtl3dSubclassDlg( HWND hwnd, WORD w )
{
    if( cvrCtl3dSubclassDlg != NULL ) {
        return( cvrCtl3dSubclassDlg( hwnd, w ) );
    }
    return( FALSE );

} /* CvrCtl3dSubclassDlg */

/*
 * CvrCtl3dSubclassDlgEx
 */
BOOL C3D_EXPORT CvrCtl3dSubclassDlgEx( HWND hwnd, DWORD dw )
{
    if( cvrCtl3dSubclassDlgEx != NULL ) {
        return( cvrCtl3dSubclassDlgEx( hwnd, dw ) );
    }
    return( FALSE );

} /* CvrCtl3dSubclassDlgEx */

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
BOOL C3D_EXPORT CvrCtl3dEnabled( void )
{
    if( cvrCtl3dEnabled != NULL ) {
        return( cvrCtl3dEnabled() );
    }
    return( FALSE );

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
BOOL C3D_EXPORT CvrCtl3dColorChange( void )
{
    if( cvrCtl3dColorChange != NULL ) {
        return( cvrCtl3dColorChange() );
    }
    return( FALSE );

} /* CvrCtl3dColorChange */

/*
 * CvrCtl3dSubclassCtl
 */
BOOL C3D_EXPORT CvrCtl3dSubclassCtl( HWND hwnd )
{
    if( cvrCtl3dSubclassCtl != NULL ) {
        return( cvrCtl3dSubclassCtl( hwnd ) );
    }
    return( FALSE );

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
BOOL C3D_EXPORT CvrCtl3dAutoSubclass( HANDLE hndl )
{
    if( cvrCtl3dAutoSubclass != NULL ) {
        return( cvrCtl3dAutoSubclass( hndl ) );
    }
    return( FALSE );

} /* CvrCtl3dAutoSubclass */

/*
 * CvrCtl3dRegister
 */
BOOL C3D_EXPORT CvrCtl3dRegister( HANDLE hndl )
{
    if( cvrCtl3dRegister != NULL ) {
        return( cvrCtl3dRegister( hndl ) );
    }
    return( FALSE );

} /* CvrCtl3dRegister */

/*
 * CvrCtl3dUnregister
 */
BOOL C3D_EXPORT CvrCtl3dUnregister( HANDLE inst )
{
    if( cvrCtl3dUnregister != NULL ) {
        return( cvrCtl3dUnregister( inst ) );
    }
    return( FALSE );

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
