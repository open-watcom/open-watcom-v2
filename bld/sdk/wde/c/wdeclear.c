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


#include "wdeglbl.h"
#include "wderesin.h"
#include "wderes.h"
#include "wdemsgbx.h"
#include "rcstr.grh"
#include "wdemain.h"
#include "wdeactn.h"
#include "wdefdiag.h"
#include "wdeclear.h"


static bool WdeQueryClearRes( void )
{
    int         ret;
    UINT        style;
    char        *title;
    char        *text;

    style = MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION;
    title = WdeAllocRCString( WDE_CLEARTITLE );
    text = WdeAllocRCString( WDE_CLEARMSG );

    ret = MessageBox( WdeGetMainWindowHandle(), text, title, style );
    if( ret == IDYES ) {
        return( true );
    }

    if( title ) {
        WdeFreeRCString( title );
    }
    if( text ) {
        WdeFreeRCString( text );
    }

    return( false );
}

bool WdeDeleteAllObjects( void )
{
    OBJPTR      base;
    OBJPTR      child;

    if( WdeGetNumRes() == 0 ) {
        return( false );
    }

    child = NULL;
    base = GetMainObject();
    if( base == NULL ) {
        return( false );
    }

    Forward( base, GET_FIRST_CHILD, &child, NULL );
    while( child != NULL ) {
        MakeObjectCurrent( child );
        Destroy( child, false );
        Forward( base, GET_FIRST_CHILD, &child, NULL );
    }

    MakeObjectCurrent( base );

    return( true );
}

bool WdeClearCurrentResource( void )
{
    bool        ok;
    bool        is32bit;

    ok = WdeQueryClearRes();

    if( ok ) {
        ok = WdeDeleteAllObjects();
    }

    if( ok && WdeIsDDE() ) {
#ifdef __NT__
        is32bit = true;
#else
        is32bit = false;
#endif
        ok = (WdeCreateNewDialog( NULL, is32bit ) != NULL);
    }

    return( ok );
}
