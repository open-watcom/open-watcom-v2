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


#include "vi.h"

#ifdef _M_I86

#define DO_DEFINE_OLEGUID
#include "ole2def.h"

bool    UseOLE2;

/*
 * OLE2Init - set up to do OLE 2.0
 */
bool OLE2Init( void )
{
    HRESULT     hresult;

    if( UseOLE2 ) {
        return( TRUE );
    }
    hresult = OleInitialize( NULL );
    if( hresult != NOERROR ) {
        return( FALSE );
    }
    if( !OLE2ClassFactoryInit() ) {
        OleUninitialize();
        return( FALSE );
    }

    UseOLE2 = TRUE;
    return( TRUE );

} /* OLE2Init */

/*
 * OLE2Fini - clean up dde related things
 */
void OLE2Fini( void )
{
    if( !UseOLE2 ) {
        return;
    }
    OLE2ClassFactoryFini();
    OleUninitialize();
    UseOLE2 = FALSE;

} /* OLE2Fini */
#else
bool OLE2Init( void ) { return( FALSE ); }
void OLE2Fini( void ) {}
#endif
