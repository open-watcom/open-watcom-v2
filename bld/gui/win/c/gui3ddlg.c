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


#include "guiwind.h"
#include "guixhook.h"
#undef __NT__ // for now to disable 3d dialogs
#ifdef __WINDOWS__
    #include "ctl3d.h"
    #ifndef __WINDOWS_386__
        #pragma library( "ctl3d" )
    #endif
#endif
#ifdef __NT__
    #include "ctl3d.h"
    #pragma library( "ctl3d32" )
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
    Ctl3dRegister( GUIMainHInst );
    Ctl3dAutoSubclass( GUIMainHInst );

    #ifdef __WINDOWS_386__
        GUISetCtl3dUnregister( &_CB_Ctl3dUnregister );
        GUISetCtl3dSubclassDlg( &_CB_Ctl3dSubclassDlg );
        GUISetCtl3dColorChange( &_CB_Ctl3dColorChange );
        GUISetCtl3dSubclassCtl( &_CB_Ctl3dSubclassCtl );
        GUISetCtl3dCtlColorEx( &_CB_Ctl3dCtlColorEx );
    #else
        GUISetCtl3dUnregister( &Ctl3dUnregister );
        GUISetCtl3dSubclassDlg( &Ctl3dSubclassDlg );
        GUISetCtl3dColorChange( &Ctl3dColorChange );
        GUISetCtl3dSubclassCtl( &Ctl3dSubclassCtl );
        GUISetCtl3dCtlColorEx( &Ctl3dCtlColorEx );
    #endif
    return( TRUE );
#else
    return( FALSE );
#endif
}
