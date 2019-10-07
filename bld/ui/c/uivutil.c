/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
#if defined(HAVE_FAR)
#include <i86.h>
#endif


bool intern isdialogue( VSCREEN *vs )
/***********************************/
{
    return( ( vs != NULL ) && ( (vs->flags & V_DIALOGUE) != 0 ) );
}


bool intern isscreen( BUFFER * bptr )
/***********************************/
{
#if defined( HAVE_FAR )
    // Short cut when using far pointers. Just check segment:

    return( _FP_SEG( bptr->origin ) == _FP_SEG( UIData->screen.origin ) );

#else
    // No far pointers. Check range:

    return( bptr->origin >= UIData->screen.origin
        &&  bptr->origin <  UIData->screen.origin + UIData->width * UIData->height );
#endif
}


void UIAPI uisetsnow( bool snow )
/********************************/
{
    UIData->no_snow = !snow;
}
