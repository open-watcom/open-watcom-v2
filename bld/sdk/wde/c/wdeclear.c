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
#include "wdeglbl.h"
#include "wderesin.h"
#include "wderes.h"
#include "wdemsgbx.h"
#include "rcstr.gh"
#include "wdemain.h"
#include "wdeactn.h"
#include "wdefdiag.h"

static Bool WdeQueryClearRes( void )
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
        return( TRUE );
    }

    if( title ) {
        WdeFreeRCString( title );
    }
    if( text ) {
        WdeFreeRCString( text );
    }

    return( FALSE );
}

Bool WdeDeleteAllObjects( void )
{
    OBJPTR      base;
    OBJPTR      child;

    if( !WdeGetNumRes() ) {
        return( FALSE );
    }

    child = NULL;
    base = GetMainObject();
    if( base == NULL ) {
        return( FALSE );
    }

    Forward( base, GET_FIRST_CHILD, &child, NULL );
    while( child != NULL ) {
        MakeObjectCurrent( child );
        Destroy( child, FALSE );
        Forward( base, GET_FIRST_CHILD, &child, NULL );
    }

    MakeObjectCurrent( base );

    return( TRUE );
}

Bool WdeClearCurrentResource( void )
{
    Bool        ok;
    Bool        is32bit;

    ok = WdeQueryClearRes();

    if( ok ) {
        ok = WdeDeleteAllObjects();
    }

    if( ok && WdeIsDDE() ) {
#ifdef __NT__
        is32bit = TRUE;
#else
        is32bit = FALSE;
#endif
        ok = (WdeCreateNewDialog( NULL, is32bit ) != NULL);
    }

    return( ok );
}
