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
#include "wrdll.h"

char            const PaletteClass[]    = "PalletClass";
char            const DrawAreaClassB[]  = "DrawAreaWindowB";
char            const DrawAreaClassI[]  = "DrawAreaWindowI";
char            const DrawAreaClassC[]  = "DrawAreaWindowC";
char            const ViewWinClass[]    = "ViewWindow";
char            const BitmapPickClass[] = "ImgedPickClass";
WPI_FONT        SmallFont;
HWND            HMainWindow;
HWND            ClientWindow;
HWND            HColorPalette;
WPI_INST        Instance;
int             ColorPlanes;
int             BitsPerPixel;
config_info     ImgedConfigInfo;
BOOL            ImgedIsDDE = FALSE;
char            *IEAppTitle             = NULL;
char            *IEImageFilter          = NULL;
char            *IEPaletteFilter        = NULL;
char            *IEImageUntitled        = NULL;
char            *IESaveImageTitle       = NULL;
char            *IEOpenImageTitle       = NULL;
char            *IESavePaletteTitle     = NULL;
char            *IEOpenPaletteTitle     = NULL;

/*
 * IEMassageFilter
 */
static void IEMassageFilter( char *filter )
{
    WRMassageFilter( filter );

} /* IEMassageFilter */

/*
 * IEFiniGlobalStrings
 */
void IEFiniGlobalStrings( void )
{
    if( IEAppTitle != NULL ) {
        IEFreeRCString( IEAppTitle );
    }
    if( IEImageFilter != NULL ) {
        IEFreeRCString( IEImageFilter );
    }
    if( IEPaletteFilter != NULL ) {
        IEFreeRCString( IEPaletteFilter );
    }
    if( IEImageUntitled != NULL ) {
        IEFreeRCString( IEImageUntitled );
    }
    if( IESaveImageTitle != NULL ) {
        IEFreeRCString( IESaveImageTitle );
    }
    if( IEOpenImageTitle != NULL ) {
        IEFreeRCString( IEOpenImageTitle );
    }
    if( IESavePaletteTitle != NULL ) {
        IEFreeRCString( IESavePaletteTitle );
    }
    if( IEOpenPaletteTitle != NULL ) {
        IEFreeRCString( IEOpenPaletteTitle );
    }

} /* IEFiniGlobalStrings */

/*
 * IEInitGlobalStrings
 */
BOOL IEInitGlobalStrings( void )
{
    BOOL        ok;

    IEAppTitle = IEAllocRCString( WIE_APPNAME );
    ok = (IEAppTitle != NULL);

    if( ok ) {
        IEImageFilter = IEAllocRCString( WIE_IMAGEFILTER );
        ok = (IEImageFilter != NULL);
        if( ok ) {
            IEMassageFilter( IEImageFilter );
        }
    }

    if( ok ) {
        IEPaletteFilter = IEAllocRCString( WIE_PALETTEFILTER );
        ok = (IEPaletteFilter != NULL);
        if( ok ) {
            IEMassageFilter( IEPaletteFilter );
        }
    }

    if( ok ) {
        IEImageUntitled = IEAllocRCString( WIE_IMAGEUNTITLED );
        ok = (IEImageUntitled != NULL);
    }

    if( ok ) {
        IESaveImageTitle = IEAllocRCString( WIE_SAVEIMAGETITLE );
        ok = (IESaveImageTitle != NULL);
    }

    if( ok ) {
        IEOpenImageTitle = IEAllocRCString( WIE_OPENIMAGETITLE );
        ok = (IEOpenImageTitle != NULL);
    }

    if( ok ) {
        IESavePaletteTitle = IEAllocRCString( WIE_SAVEPALETTETITLE );
        ok = (IESavePaletteTitle != NULL);
    }

    if( ok ) {
        IEOpenPaletteTitle = IEAllocRCString( WIE_OPENPALETTETITLE );
        ok = (IEOpenPaletteTitle != NULL);
    }

    if( !ok ) {
        IEFiniGlobalStrings();
    }

    return( ok );

} /* IEInitGlobalStrings */
