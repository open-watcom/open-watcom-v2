/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


#include "wreglbl.h"
#include "wremain.h"
#include "wreseted.h"
#include "wregcres.h"
#include "wrenames.h"
#include "wrdll.h"
#include "wrmemf.h"
#include "wresall.h"
#include "wrememf.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

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

bool WREChangeMemFlags( void )
{
    WRECurrentResInfo   curr;
    HWND                parent;
    char                *type_name;
    uint_16             type_id;
    uint_16             mflags;
    HELPFUNC            hcb;
    bool                ok;

    hcb = NULL;
    type_name = NULL;
    mflags = 0;
    type_id = 0;

    ok = WREGetCurrentResource( &curr );

    if( ok )  {
        if( !curr.type->Info.TypeName.IsName ) {
            type_id = curr.type->Info.TypeName.ID.Num;
        }
        type_name = WREGetResName( curr.res, type_id );
        parent = WREGetMainWindowHandle();
    }

    if( ok ) {
        hcb = MakeProcInstance_HELP( WREHelpRoutine, WREGetAppInstance() );
        ok = (hcb != NULL);
    }

    if( ok ) {
        mflags = curr.lang->Info.MemoryFlags;
        ok = WRChangeMemFlags( parent, type_name, &mflags, hcb );
    }

    if( ok ) {
        curr.lang->Info.MemoryFlags = mflags;
    }

    if( hcb != NULL ) {
        FreeProcInstance_HELP( hcb );
    }

    if( type_name != NULL ) {
        WRMemFree( type_name );
    }

    return( ok );
}
