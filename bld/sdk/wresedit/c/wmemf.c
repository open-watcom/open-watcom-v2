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
#include "watcom.h"
#include "wresall.h"
#include "wglbl.h"
#include "wrdll.h"
#include "wmem.h"
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

Bool WChangeMemFlags( HWND parent, uint_16 *mflags, WResID *res_name,
                      HINSTANCE inst, HELP_CALLBACK *hcb )
{
    char        *name;
    Bool        ok;
    FARPROC     cb;

    _wtouch( inst );

    name = NULL;
    cb = NULL;

    ok = (mflags != NULL && res_name != NULL);

    if( ok )  {
        if( res_name != NULL ) {
            name = WResIDToStr( res_name );
            ok = (name != NULL);
        }
    }

    if( ok ) {
        cb = MakeProcInstance( (FARPROC)hcb, inst );
        ok = (cb != (FARPROC)NULL);
    }

    if( ok ) {
        ok = WRChangeMemFlags( parent, name, mflags, cb );
    }

    if( cb != (FARPROC)NULL ) {
        FreeProcInstance( (FARPROC)cb );
    }

    if( name != NULL ) {
        WMemFree( name );
    }

    return( ok );
}
