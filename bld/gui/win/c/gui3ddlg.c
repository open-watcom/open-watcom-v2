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
* Description:  Initialize 3D controls support.
*
****************************************************************************/


#include "guiwind.h"
#include "guixhook.h"
#undef __NT__ // for now to disable 3d dialogs
#ifdef __WINDOWS_386__
    #include "ctl3d.h"
#endif
#ifdef __NT__
    #include "ctl3d.h"
    #pragma library( "ctl3d32.lib" )
#endif

#if defined( __WINDOWS__ ) && !defined( __WINDOWS_386__ )
typedef BOOL    ( WINAPI *LPFN_Ctl3dRegister )( HANDLE );
typedef BOOL    ( WINAPI *LPFN_Ctl3dUnregister )( HANDLE );
typedef BOOL    ( WINAPI *LPFN_Ctl3dAutoSubclass )( HANDLE );
typedef BOOL    ( WINAPI *LPFN_Ctl3dSubclassDlg )( HWND, WORD );
typedef BOOL    ( WINAPI *LPFN_Ctl3dSubclassCtl )( HWND );
typedef BOOL    ( WINAPI *LPFN_Ctl3dColorChange )( void );
typedef HBRUSH  ( WINAPI *LPFN_Ctl3dCtlColorEx )( UINT, WPARAM, LPARAM );

static HINSTANCE    ctlDLLLib = NULL;
#endif

#ifdef __WINDOWS_386__
BOOL _DLLFAR PASCAL _CB_Ctl3dUnregister( HANDLE h )
{
    return( Ctl3dUnregister( h ) );
}

BOOL _DLLFAR PASCAL _CB_Ctl3dSubclassDlg( HWND h, WORD w )
{
    return( Ctl3dSubclassDlg( h, w ) );
}

BOOL _DLLFAR PASCAL _CB_Ctl3dColorChange( void )
{
    return( Ctl3dColorChange() );
}

BOOL _DLLFAR PASCAL _CB_Ctl3dSubclassCtl( HWND h )
{
    return( Ctl3dSubclassCtl( h ) );
}

HBRUSH _DLLFAR PASCAL _CB_Ctl3dCtlColorEx(UINT wm, WPARAM wp, LPARAM lp )
{
#if 0
    return( Ctl3dCtlColorEx( wm, wp, lp ) );
#else
    return( NULL );
#endif
}
#endif

extern  WPI_INST        GUIMainHInst;

bool GUI3DDialogInit( void )
{
#ifdef __WINDOWS_386__
    static bool dll32Ctl3dOpen = FALSE;

    if( !dll32Ctl3dOpen ) {
        if( Init32Ctl3d() ) {
            dll32Ctl3dOpen = TRUE;
        } else {
            return( FALSE );
        }
    }
#endif

#if defined( __WINDOWS__ ) || defined( __NT__ )

    /* Only use CTL3D on old versions of Windows. */
    if( LOBYTE(LOWORD(GetVersion())) < 4 ) {
    #ifdef __WINDOWS_386__
        GUISetCtl3dUnregister( &_CB_Ctl3dUnregister );
        GUISetCtl3dSubclassDlg( &_CB_Ctl3dSubclassDlg );
        GUISetCtl3dSubclassCtl( &_CB_Ctl3dSubclassCtl );
        GUISetCtl3dColorChange( &_CB_Ctl3dColorChange );
        GUISetCtl3dCtlColorEx( &_CB_Ctl3dCtlColorEx );
    #else
        LPFN_Ctl3dRegister          pfnCtl3dRegister;
        LPFN_Ctl3dUnregister        pfnCtl3dUnregister;
        LPFN_Ctl3dAutoSubclass      pfnCtl3dAutoSubclass;
        LPFN_Ctl3dSubclassDlg       pfnCtl3dSubclassDlg;
        LPFN_Ctl3dSubclassCtl       pfnCtl3dSubclassCtl;
        LPFN_Ctl3dColorChange       pfnCtl3dColorChange;
        LPFN_Ctl3dCtlColorEx        pfnCtl3dCtlColorEx;
        UINT                        uErrMode;

        /* Use SetErrorMode to prevent annoying error popups. */
        uErrMode = SetErrorMode( SEM_NOOPENFILEERRORBOX );
        ctlDLLLib = LoadLibrary( "CTL3DV2.DLL" );
        SetErrorMode( uErrMode );

        /* If CTL3D DLL couldn't be loaded, just fail this function. */
        if( ctlDLLLib == (HINSTANCE)NULL ) {
            return( FALSE );
        }

        /* Get the addresses of the CTL3D functions we need. */
        pfnCtl3dRegister        = (void *)GetProcAddress( ctlDLLLib, (LPCSTR)12 );
        pfnCtl3dUnregister      = (void *)GetProcAddress( ctlDLLLib, (LPCSTR)13 );
        pfnCtl3dAutoSubclass    = (void *)GetProcAddress( ctlDLLLib, (LPCSTR)16 );
        pfnCtl3dSubclassDlg     = (void *)GetProcAddress( ctlDLLLib, (LPCSTR)2 );
        pfnCtl3dSubclassCtl     = (void *)GetProcAddress( ctlDLLLib, (LPCSTR)3 );
        pfnCtl3dColorChange     = (void *)GetProcAddress( ctlDLLLib, (LPCSTR)6 );
        pfnCtl3dCtlColorEx      = (void *)GetProcAddress( ctlDLLLib, (LPCSTR)18 );

        if( ( pfnCtl3dRegister      == NULL ) ||
            ( pfnCtl3dUnregister    == NULL ) ||
            ( pfnCtl3dAutoSubclass  == NULL ) ||
            ( pfnCtl3dSubclassDlg   == NULL ) ||
            ( pfnCtl3dSubclassCtl   == NULL ) ||
            ( pfnCtl3dColorChange   == NULL ) ||
            ( pfnCtl3dCtlColorEx    == NULL ) ) {

            FreeLibrary( ctlDLLLib );
            ctlDLLLib = (HINSTANCE)NULL;
            return( FALSE );
        }

        pfnCtl3dRegister( GUIMainHInst );
        pfnCtl3dAutoSubclass( GUIMainHInst );

        GUISetCtl3dUnregister( pfnCtl3dUnregister );
        GUISetCtl3dSubclassDlg( pfnCtl3dSubclassDlg );
        GUISetCtl3dSubclassCtl( pfnCtl3dSubclassCtl );
        GUISetCtl3dColorChange( pfnCtl3dColorChange );
        GUISetCtl3dCtlColorEx( pfnCtl3dCtlColorEx );
    #endif
    }
    return( TRUE );
#else
    return( FALSE );
#endif
}
