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
#include <string.h>
#include "watcom.h"
#include "wreglbl.h"
#include "wreresin.h"
#include "wremain.h"

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static void WREDestroyMDIWindow( HWND );

void WREFreeResInfo( WREResInfo *info )
{
    if( info != NULL ) {
        if( info->info_win != NULL && IsWindow( info->info_win ) ) {
            DestroyWindow( info->info_win );
        }
        if( info->res_win != NULL && IsWindow( info->res_win ) ) {
            WREDestroyMDIWindow( info->res_win );
        }
        if( info->info != NULL ) {
            WRFreeWRInfo( info->info );
        }
        if( info->symbol_table != NULL ) {
            WRFreeHashTable( info->symbol_table );
        }
        if( info->symbol_file != NULL ) {
            WRMemFree( info->symbol_file );
        }
        WRMemFree( info );
    }
}

WREResInfo *WREAllocResInfo( void )
{
    WREResInfo  *info;

    info = (WREResInfo *)WRMemAlloc( sizeof( WREResInfo ) );

    if( info != NULL ) {
        memset( info, 0, sizeof( WREResInfo ) );
    }

    return( info );
}

Bool WREIsResModified( WREResInfo *info )
{
    return( info == NULL || info->modified );
}

void WRESetResModified( WREResInfo *info, Bool mod )
{
    if( info != NULL ) {
        info->modified = mod;
    }
}

void WREDestroyMDIWindow( HWND win )
{
    SendMessage( WREGetMDIWindowHandle(), WM_MDIDESTROY, (WPARAM)win, 0 );
}
