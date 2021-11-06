/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "wglbl.h"
#include <limits.h>
#include "wacc.h"
#include "wlist.h"
#include "wisedit.h"
#include "whndl.h"

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    WAccelHandle    hndl;
    WAccelEditInfo  *info;
} WHndlInfo;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WHndlInfo    *WFindHndlInfo( WAccelHandle );
static WHndlInfo    *WFindHndlInfoR( WAccelEditInfo *info );
static void         WFreeHndlInfo( WHndlInfo * );
static WHndlInfo    *WAllocHndlInfo( void );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static WAccelHandle WNextHndl = 1;
static LIST         *WHndlList = NULL;

WAccelHandle WRegisterEditSession( WAccelEditInfo *info )
{
    WHndlInfo   *hinfo;

    if( info == NULL || WNextHndl == INT_MAX || (hinfo = WAllocHndlInfo()) == (WHndlInfo *)NULL ) {
        return( 0 );
    }

    hinfo->hndl = WNextHndl;
    hinfo->info = info;

    WInsertObject( &WHndlList, hinfo );

    WNextHndl++;

    return( hinfo->hndl );
}

int WUnRegisterEditSession( WAccelHandle hndl )
{
    WHndlInfo *hinfo;

    if( hndl == 0 || (hinfo = WFindHndlInfo( hndl )) == (WHndlInfo *)NULL ) {
        return( FALSE );
    }

    ListRemoveElt( &WHndlList, hinfo );

    WFreeHndlInfo( hinfo );

    return( TRUE );
}

WAccelEditInfo *WGetEditSessionInfo( WAccelHandle hndl )
{
    WHndlInfo *hinfo;

    if( hndl == 0 || (hinfo = WFindHndlInfo( hndl )) == (WHndlInfo *)NULL ) {
        return( NULL );
    }

    return( hinfo->info );
}

WAccelHandle WGetEditSessionHandle( WAccelEditInfo *info )
{
    WHndlInfo *hinfo;

    if( info == NULL || (hinfo = WFindHndlInfoR( info )) == (WHndlInfo *)NULL ) {
        return( 0 );
    }

    return( hinfo->hndl );
}

int WIsValidHandle( WAccelHandle hndl )
{
    return( hndl != 0 && WFindHndlInfo( hndl ) != (WHndlInfo *)NULL );
}

bool WIsAccelDialogMessage( MSG *msg, HACCEL accel_table )
{
    WHndlInfo   *hinfo;
    LIST        *l;
    bool        ok;
    HWND        active;

    ok = false;
    active = GetActiveWindow();

    for( l = WHndlList; l != NULL && !ok; l = ListNext( l ) ) {
        hinfo = ListElement( l );
        if( hinfo != NULL && hinfo->info->win == active ) {
            if( WDoesEditHaveFocus() ) {
                if( hinfo->info->edit_dlg != (HWND)NULL ) {
                    ok = ( IsDialogMessage( hinfo->info->edit_dlg, msg ) != 0 );
                }
                if( !ok ) {
                    ok = ( TranslateAccelerator( hinfo->info->win, accel_table, msg ) != 0 );
                }
            } else {
                ok = ( TranslateAccelerator( hinfo->info->win, accel_table, msg ) != 0 );
                if( !ok && hinfo->info->edit_dlg != (HWND)NULL ) {
                    ok = ( IsDialogMessage( hinfo->info->edit_dlg, msg ) != 0 );
                }
            }
        }
    }

    return( ok );
}

WHndlInfo *WFindHndlInfo( WAccelHandle hndl )
{
    WHndlInfo   *hinfo;
    LIST        *l;

    for( l = WHndlList; l != NULL; l = ListNext( l ) ) {
        hinfo = ListElement( l );
        if( hinfo->hndl == hndl ) {
            return( hinfo );
        }
    }

    return( NULL );
}

WHndlInfo *WFindHndlInfoR( WAccelEditInfo *info )
{
    WHndlInfo   *hinfo;
    LIST        *l;

    for( l = WHndlList; l != NULL; l = ListNext( l ) ) {
        hinfo = ListElement( l );
        if( hinfo->info == info ) {
            return( hinfo );
        }
    }

    return( NULL );
}

WHndlInfo *WAllocHndlInfo( void )
{
    WHndlInfo *info;

    info = (WHndlInfo *)WRMemAlloc( sizeof( WHndlInfo ) );

    memset( info, 0, sizeof( WHndlInfo ) );

    return( info );
}

void WFreeHndlInfo( WHndlInfo *info )
{
    if( info != NULL ) {
        WRMemFree( info );
    }
}
