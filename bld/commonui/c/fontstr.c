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
* Description:  Convert LOGFONTs to and from strings.
*
****************************************************************************/


#include "precomp.h"
#include <commdlg.h>
#include <stdio.h>
#include <string.h>
#include "fontstr.h"

static char     *outFmtString = "%d %d %d %d %d %d %d %d %d %d %d %d %d \"%s\"";
static char     *inFmtString = "%d %d %d %d %d %d %d %d %d %d %d %d %d %[^\0]";

/*
 * GetLogFontFromString - get a logical font from a string
 */
int GetLogFontFromString( LOGFONT *l, char *data )
{
    int num;
    int i;
    int len;
    int lfItalic;
    int lfUnderline;
    int lfStrikeOut;
    int lfCharSet;
    int lfOutPrecision;
    int lfClipPrecision;
    int lfQuality;
    int lfPitchAndFamily;

    num = sscanf( data, inFmtString, &l->lfHeight, &l->lfWidth, &l->lfEscapement,
                  &l->lfOrientation, &l->lfWeight, &lfItalic, &lfUnderline, &lfStrikeOut,
                  &lfCharSet, &lfOutPrecision, &lfClipPrecision, &lfQuality,
                  &lfPitchAndFamily, &l->lfFaceName );
    if( num != 14 ) {
        return( FALSE );
    }
    l->lfItalic = lfItalic;
    l->lfUnderline = lfUnderline;
    l->lfStrikeOut = lfStrikeOut;
    l->lfCharSet = lfCharSet;
    l->lfOutPrecision = lfOutPrecision;
    l->lfClipPrecision = lfClipPrecision;
    l->lfQuality = lfQuality;
    l->lfPitchAndFamily = lfPitchAndFamily;
    if( l->lfFaceName[0] == '\"' ) {
        len = strlen( l->lfFaceName );
        for( i = 1; i < len; i++ ) {
            l->lfFaceName[i - 1] = l->lfFaceName[i];
        }
        l->lfFaceName[len - 2] = 0;
    }
    return( TRUE );

} /* GetLogFontFromString */

/*
 * GetFontFormatString
 */
void GetFontFormatString( LOGFONT *lf, char *str )
{
    wsprintf( str, outFmtString, lf->lfHeight, lf->lfWidth, lf->lfEscapement,
              lf->lfOrientation, lf->lfWeight, lf->lfItalic, lf->lfUnderline,
              lf->lfStrikeOut, lf->lfCharSet, lf->lfOutPrecision, lf->lfClipPrecision,
              lf->lfQuality, lf->lfPitchAndFamily, lf->lfFaceName );

} /* GetFontFormatString */
