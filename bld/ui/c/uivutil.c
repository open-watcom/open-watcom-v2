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


#include <stdlib.h>
#include "uidef.h"
#ifndef __UNIX__
#include <conio.h>
#if defined(HAVE_FAR)
#include <i86.h>
#endif
#endif


bool intern isdialogue( VSCREEN *vptr )
/*************************************/
{
    return( ( vptr != NULL ) && ( ( vptr->flags & V_DIALOGUE ) != 0 ) );
}


bool intern isscreen( BUFFER * bptr )
/***********************************/
{
#if defined( HAVE_FAR ) && !defined( __UNIX__ )
    // Short cut when using far pointers. Just check segment:

    return( FP_SEG( bptr->origin ) == FP_SEG( UIData->screen.origin ) );

#else
    // No far pointers. Check range:

    return( bptr->origin >= UIData->screen.origin
        &&  bptr->origin <  UIData->screen.origin + UIData->width * UIData->height );
#endif
}


void global uisetsnow( bool snow )
/********************************/
{
    UIData->no_snow = ~snow;
}
