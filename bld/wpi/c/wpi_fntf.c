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
* Description:  WPI OS/2 font routines that require floating-point.
*
****************************************************************************/


#include <math.h>

#define INCL_PM
#include <os2.h>
#include "wpi.h"

#define PI 3.14159265

void _wpi_f_setfontescapement( WPI_F_FONT *font, LONG angle )
/***********************************************************/
/* In Windows, the escapement is in tenths of degrees */

{
    /* The '50's below are important. On a Laserwriter PS printer,
       using 200 causes outline fonts to come out on an angle
       (i.e. yet another OS/2 bug). */
    font->bundle.ptlShear.x = 50 * sin( 2 * PI * angle / 3600 );
    font->bundle.ptlShear.y = 50 * cos( 2 * PI * angle / 3600 );
}


void _wpi_f_setfontorientation( WPI_F_FONT *font, LONG angle )
/************************************************************/
/* In Windows, the orientation is in tenths of degrees */

{
    /* The '50's below are important. On a Laserwriter PS printer,
       using 200 causes outline fonts to come out on an angle
       (i.e. yet another OS/2 bug). */
    font->bundle.ptlAngle.x = 50 * cos( 2 * PI * angle / 3600 );
    font->bundle.ptlAngle.y = 50 * sin( 2 * PI * angle / 3600 );
}
