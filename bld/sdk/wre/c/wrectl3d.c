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


#include "precomp.h"
#include "wreglbl.h"
#include "wrectl3d.h"
#include "wrdll.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define WRE_USE_3D

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

Bool WRECtl3DInit ( HINSTANCE inst )
{
#if !defined( WRE_USE_3D )
    _wre_touch( inst );
    return( TRUE );
#else
    Bool ok;

    ok = WRCtl3dRegister( inst );
    if( ok ) {
        ok = WRCtl3dAutoSubclass( inst );
    }

    return( ok );
#endif
}

void WRECtl3DFini( HINSTANCE inst )
{
#if !defined( WRE_USE_3D )
    _wre_touch( inst );
#else
    WRCtl3dUnregister( inst );
#endif
}

void WRECtl3dColorChange( void )
{
#if defined( WRE_USE_3D )
    WRCtl3dColorChange();
#endif
}

void WRECtl3dSubclassDlg( HWND win, WORD w )
{
#if !defined( WRE_USE_3D)
    _wre_touch( win );
    _wre_touch( w );
#else
    WRCtl3dSubclassDlg( win, w );
#endif
}
