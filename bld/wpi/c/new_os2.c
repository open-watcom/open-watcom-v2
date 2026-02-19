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
#include <string.h>

#define INCL_PM
#define INCL_DOSPROCESS
#define INCL_SPL
#define INCL_SPLFSE
#define INCL_SPLDOSPRINT
#define INCL_BSE
#define INCL_DOSMODULEMGR
#define INCL_DOSRESOURCES
#define INCL_DOSSIGNALS
#define INCL_DOSSEMAPHORES
#define INCL_DOSMISC
#define INCL_DOSERRORS
#define INCL_DOSDEVICES
#include <os2.h>
#include <pmspl.h>
#include <bsedos.h>
#include <bseerr.h>
#include "wpi.h"

#if 0
WPI_FONT _wpi_createwpifont( WPI_LOGFONT *fontmetrics )
/****************************************************************/
/* The wpi_font is more similar to the windows fonts created    */
/* in the sense that it doesn't require a presentation space    */
/* handle which most PM apps do                                 */
{
    WPI_FONT    wpifont;

    wpifont = _wpi_malloc( sizeof(FATTRS) );
    _wpi_getfontattrs( fontmetrics, wpifont );
    wpifont->fsFontUse = FATTR_FONTUSE_OUTLINE;
    wpifont->fsType = 0;
    wpifont->fsSelection = 0;
    return( wpifont );
} /* _wpi_createwpifont */

#endif

