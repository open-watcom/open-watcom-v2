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


#include "wglbl.h"
#include "wresall.h"
#include "wrdll.h"
#include "wmemf.h"
#include "wrmemf.h"

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

bool WChangeMemFlags( HWND parent, uint_16 *mflags, WResID *res_name,
                      HINSTANCE inst, HELPFUNC help_callback )
{
    char            *name;
    bool            ok;
    HELPFUNC        hcb;

    /* unused parameters */ (void)inst;

    name = NULL;
    hcb = NULL;

    ok = (mflags != NULL && res_name != NULL);

    if( ok )  {
        if( res_name != NULL ) {
            name = WResIDToStr( res_name );
            ok = (name != NULL);
        }
    }

    if( ok ) {
        hcb = MakeProcInstance_HELP( help_callback, inst );
        ok = ( hcb != NULL );
    }

    if( ok ) {
        ok = WRChangeMemFlags( parent, name, mflags, hcb );
    }

    if( hcb != NULL ) {
        FreeProcInstance_HELP( hcb );
    }

    if( name != NULL ) {
        WRMemFree( name );
    }

    return( ok );
}
