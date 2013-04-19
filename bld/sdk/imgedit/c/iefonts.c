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


#include "imgedit.h"
#include <stdarg.h>
#include <mbstring.h>
#include "iemem.h"

#define STATUS_FONTNAME         "Helv"
#define STATUS_POINTSIZE        8

/*
 * CreateStatusFont - create the font used in the status window
 */
void CreateStatusFont( void )
{
    LOGFONT             lf;
    HDC                 dc;
    char                *status_font;
    char                *cp;
    int                 point_size;
    BOOL                use_default;

    memset( &lf, 0, sizeof( LOGFONT ) );
    dc = GetDC( (HWND)NULL );
    lf.lfWeight = FW_NORMAL;
    use_default = TRUE;

    status_font = IEAllocRCString( WIE_STATUSFONT );
    if( status_font != NULL ) {
        cp = (char *)_mbschr( (unsigned char *)status_font, '.' );
        if( cp != NULL ) {
            *cp = '\0';
            strcpy( lf.lfFaceName, status_font );
            cp++;
            point_size = atoi( cp );
            use_default = FALSE;
        }
        IEFreeRCString( status_font );
    }

    if( use_default ) {
        strcpy( lf.lfFaceName, STATUS_FONTNAME );
        point_size = STATUS_POINTSIZE;
    }

    lf.lfHeight = -MulDiv( point_size, GetDeviceCaps( dc, LOGPIXELSY ), 72 );
    SmallFont = CreateFontIndirect( &lf );
    ReleaseDC( (HWND)NULL, dc );

} /* CreateStatusFont */
