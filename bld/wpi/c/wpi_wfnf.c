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
* Description:  WPI Windows font routines that require floating-point.
*
****************************************************************************/


#include <stdlib.h>

#include <windows.h>
#include "wpi.h"


int _wpi_getmetricpointsize( WPI_PRES pres, WPI_TEXTMETRIC *tm,
                                            int *pix_size, int *match_num )
/*************************************************************************/
{
    int                 logpixelsy;
    int                 pointsize;

    *pix_size = _wpi_metricheight( *tm ) - _wpi_metricileading( *tm );
    *match_num = 0;

    logpixelsy = GetDeviceCaps( pres, LOGPIXELSY );
    pointsize = (int)((float)( (float)( (*pix_size) * 72.0 ) /
                                                (float)logpixelsy ) + .5 );
    return( pointsize );
} /* _wpi_getmetricpointsize */

