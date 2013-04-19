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


#include "wdeglbl.h"
#include "wdectl3d.h"
#include "wdemsgbx.h"
#include "rcstr.gh"
#include "wrdll.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define WDE_USE_3D
#define CTL3D_VER 0x0205

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
#if defined( WDE_USE_3D )
static Bool     CorrectVersion = FALSE;
#endif

Bool WdeCtl3DInit( HINSTANCE inst )
{
#if !defined( WDE_USE_3D )
    _wde_touch( inst );
    return ( TRUE );
#else
    WORD        ver;
    Bool        ok;

    ver = WRCtl3dGetVer();
    if( ver >= CTL3D_VER ) {
        CorrectVersion = TRUE;
    }

    ok = WRCtl3dRegister( inst );
    if( ok ) {
        ok = WRCtl3dAutoSubclass( inst );
    }

    return( ok );
#endif
}

void WdeCtl3DFini( HINSTANCE inst )
{
#if !defined( WDE_USE_3D )
    _wde_touch( inst );
#else
    WRCtl3dUnregister( inst );
#endif
}

void WdeCtl3dColorChange( void )
{
#if defined( WDE_USE_3D )
    WRCtl3dColorChange();
#endif
}

void WdeCtl3dSubclassDlg( HWND win, WORD w )
{
#if !defined( WDE_USE_3D )
    _wde_touch( win );
    _wde_touch( w );
#else
    if( CorrectVersion ) {
        WRCtl3dSubclassDlgEx( win, w );
    } else {
        WRCtl3dSubclassDlg( win, w );
    }
#endif
}

void WdeCtl3dSubclassCtl( HWND win )
{
#if !defined( WDE_USE_3D )
    _wde_touch( win );
#else
    WRCtl3dSubclassCtl( win );
#endif
}

LONG WdeCtl3dDlgFramePaint( HWND win, UINT msg, WPARAM wp, LPARAM lp )
{
#if !defined( WDE_USE_3D )
    _wde_touch( win );
    _wde_touch( msg );
    _wde_touch( wp );
    _wde_touch( lp );
    return( 0 );
#else
    return( WRCtl3dDlgFramePaint( win, msg, wp, lp ) );
#endif
}

HBRUSH WdeCtl3dCtlColorEx( UINT msg, WPARAM wp, LPARAM lp )
{
#if !defined( WDE_USE_3D )
    _wde_touch( msg );
    _wde_touch( wp );
    _wde_touch( lp );
    return( (HBRUSH)NULL );
#else
    return( WRCtl3dCtlColorEx( msg, wp, lp ) );
#endif
}
