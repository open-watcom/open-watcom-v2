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


#include "uidef.h"
#ifdef __UNIX__
#include "uivirt.h"
#endif
#include "uiforce.h"
#include "uigchar.h"


bool UIAPI uistart( void )
/*************************/
{
    UIMemOpen();
    if( initbios() ) {
        DBCSCharacterMap();
        /* need for LUI and DUI apps to avoid divide by zero    */
        /* when no mouse is found                               */
        UIData->mouse_yscale = 1;
        UIData->mouse_xscale = 1;
        uiattrs();
        openbackground();
        forceevinit();
        initeventlists();
        return( true );
    }
    return( false );
}

bool UIAPI uiinit( int install )
/******************************/
{
    bool    initialized;

    initialized = uistart();

    if( initialized ) {
#ifdef __UNIX__
        _initmouse( install );
#else
        initmouse( install );
#endif
    }
    return( initialized );
}

void UIAPI uistop( void )
{
    forceevfini();
    closebackground();
    finibios();
    UIMemClose();
}

void UIAPI uifini( void )
/************************/
{
#ifdef __UNIX__
    _finimouse();
#else
    finimouse();
#endif
    uistop();
}


void UIAPI uiswap( void )
/************************/
{
    uiswapcursor();
    uiswapmouse();
}
