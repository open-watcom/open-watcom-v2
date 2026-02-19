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
* Description:  Alternate WPI font routines.
*
****************************************************************************/


#define INCL_PM
#define INCL_DOSNLS
#include <os2.h>
#include "wpi.h"

/***** _WPI_F_* Font Functions *****/
/* These functions are a set of replacement WPI font functions
   which should work. All the other functions are crap; this
   is an attempt to do it right. DJP */

#define FONT_SET_ID     1       // font index in the PRES which we use

void _wpi_f_setfontfacename( WPI_F_FONT *font, PSZ name )
/*******************************************************/

{
    font->attr.szFacename[0] = '\0';
    if( name != NULL ) {
        if( strcmp( name, "MS Sans Serif" ) == 0 ) {
            strcpy( font->attr.szFacename, "Helvetica" );
        } else if( strcmp( name, "MS Serif" ) == 0 ) {
            strcpy( font->attr.szFacename, "Times New Roman" );
        } else if( strcmp( name, "Arial" ) == 0 ) {
            strcpy( font->attr.szFacename, "Helvetica" );
        } else if( strcmp( name, "Times" ) == 0 ) {
            strcpy( font->attr.szFacename, "Times New Roman" );
        } else {
            strcpy( font->attr.szFacename, name );
        }
    }
}

HFONT _wpi_f_createfont( WPI_F_FONT *font )
/*****************************************/

{
    /* In Windows, the CreateFont function takes a LOGFONT and builds
       a HFONT. With our WPI_F_FONT structure in WPI OS/2, we don't
       really need this. But, somebody may build a WPI_F_FONT,
       call this function, then return the result. Hence,
       we do have to allocates and copy it */

    WPI_F_FONT                  *copy_font;

    _wpi_malloc2( copy_font, 1 );

    memcpy( copy_font, font, sizeof( *font ) );

    return( (LONG)copy_font );
}

void _wpi_f_getoldfont( WPI_PRES pres, HFONT ofont )
/**************************************************/

{
    WPI_F_FONT                  *font;

    font = (void *) ofont;
    GpiSetCharSet( pres, LCID_DEFAULT );
    GpiDeleteSetId( pres, FONT_SET_ID );        // delete any leftover first
    GpiCreateLogFont( pres, NULL, FONT_SET_ID, &font->attr );
    GpiSetCharSet( pres, FONT_SET_ID );
    if( font->attr.fsFontUse & FATTR_FONTUSE_OUTLINE ) {
        /* set character attrs too */
        GpiSetAttrs( pres, PRIM_CHAR, CBB_BOX | CBB_ANGLE | CBB_SHEAR,
                                                        0, &font->bundle );
    }

    _wpi_free( font );
}

static WPI_F_FONT *get_f_attrs( WPI_PRES pres, WPI_F_FONT *font )
/***************************************************************/
/* get the current font information in the pres into the font structure */

{
    FONTMETRICS                 fm;
    HDC                         dc;
    LONG                        vert_res;

    GpiQueryFontMetrics( pres, sizeof(FONTMETRICS), &fm );

    font->attr.usRecordLength = sizeof( FATTRS );

    font->attr.fsSelection = fm.fsSelection;
    font->attr.lMatch = fm.lMatch;
    strcpy( font->attr.szFacename, fm.szFacename );
    font->attr.idRegistry = fm.idRegistry;
    font->attr.lMaxBaselineExt = fm.lMaxBaselineExt;
    font->attr.lAveCharWidth = fm.lAveCharWidth;
    if( fm.fsDefn & FM_DEFN_OUTLINE ) {
        font->attr.fsFontUse = FATTR_FONTUSE_OUTLINE;
        GpiQueryCharShear( pres, &font->bundle.ptlShear );
        GpiQueryCharAngle( pres, &font->bundle.ptlAngle );
        GpiQueryCharBox( pres, &font->bundle.sizfxCell );

        /* compute point size */
        dc = GpiQueryDevice( pres );
        DevQueryCaps( dc, CAPS_VERTICAL_FONT_RES, 1L, &vert_res );

        font->pt_size = font->bundle.sizfxCell.cy * 72 / vert_res;
    } else {
        font->pt_size = fm.sNominalPointSize / 10;
    }

    font->retrieved = TRUE;

    return( font );
}

static BOOL find_font( WPI_PRES pres, FATTRS *attr, WPI_F_FONT *font )
/********************************************************************/

{
    HDC                         dc;
    LONG                        horz_res;
    LONG                        vert_res;
    LONG                        num_fonts;
    FONTMETRICS                 *fonts;
    int                         i;
    int                         best_outline;
    int                         best_outline_diff;
    SHORT                       size;
    int                         size_diff;
    SIZEF                       sizef;
    BOOL                        found_match;

    dc = GpiQueryDevice( pres );

    DevQueryCaps( dc, CAPS_HORIZONTAL_FONT_RES, 1L, &horz_res );
    DevQueryCaps( dc, CAPS_VERTICAL_FONT_RES,   1L, &vert_res );

    memset( attr, 0, sizeof( *attr ) );
    attr->usRecordLength = sizeof( FATTRS );
    if( font->attr.szFacename[0] == '\0' ) {
        /* default font requested. The books claim that we can just
           set the facename to the null string, and it will select
           the default font for the DC */

        return( TRUE );
    }

    /* First, see if we can match the font with an image
       font based on the definition resolution and the size */

    num_fonts = 0;
    num_fonts = GpiQueryFonts( pres, QF_PUBLIC, font->attr.szFacename,
                                   &num_fonts, 0L, NULL);
    if( num_fonts == 0 ) {
        return( FALSE );
    }

    fonts = _wpi_malloc( num_fonts * sizeof( FONTMETRICS ) );
    GpiQueryFonts( pres, QF_PUBLIC, font->attr.szFacename,
                    &num_fonts, (LONG) sizeof( FONTMETRICS ), fonts );

    found_match = FALSE;
    best_outline = -1;
    size = font->pt_size * 10;
    for( i = 0; i < num_fonts; i++ ) {
        size_diff = abs( size - fonts[i].sNominalPointSize );
        if( fonts[i].fsDefn & FM_DEFN_OUTLINE ) {
            /* outline font. Record it if it is a good fit */
            if( best_outline == -1 ||
                ( best_outline_diff < size_diff &&
                ( font->attr.usCodePage == 0 ||
                fonts[i].usCodePage == 0 ||
                ( font->attr.usCodePage == fonts[i].usCodePage ) ) ) ) {
                best_outline = i;
                best_outline_diff = size_diff;
            }
        } else {
            if( fonts[i].sXDeviceRes == (SHORT) horz_res &&
                fonts[i].sYDeviceRes == (SHORT) vert_res &&
                fonts[i].sNominalPointSize == (SHORT) size &&
                ( font->attr.usCodePage == 0 ||
                fonts[i].usCodePage == 0 ||
                ( font->attr.usCodePage == fonts[i].usCodePage ) ) ) {
                /* this is a good image font match. Select it */
                break;
            }
        }
    }

    size = font->pt_size;
    if( i == num_fonts ) {
        if( best_outline != -1 ) {
            /* use an outline font */
            found_match = TRUE;
            i = best_outline;
            attr->fsFontUse |= FATTR_FONTUSE_OUTLINE;
            attr->lMatch = 0;

            /* the size is unusual for OS/2. The conversion to
               logical units has to be by you, not the OS.
               So this is what that does */
            sizef.cx = MAKEFIXED( size * horz_res / 72, 0 );
            sizef.cy = MAKEFIXED( size * vert_res / 72, 0 );
            font->bundle.sizfxCell = sizef;
            attr->lMaxBaselineExt = 0;
            attr->lAveCharWidth = 0;
        }
    } else {
        found_match = TRUE;
        attr->fsFontUse &= ~FATTR_FONTUSE_OUTLINE;
        attr->lMatch = fonts[i].lMatch;
        attr->lMaxBaselineExt = fonts[i].lMaxBaselineExt;
        attr->lAveCharWidth = fonts[i].lAveCharWidth;
    }

    if( found_match ) {
        strcpy( attr->szFacename, fonts[i].szFacename );
        attr->fsSelection = font->attr.fsSelection;
        attr->idRegistry = fonts[i].idRegistry;
        attr->usCodePage = fonts[i].usCodePage;
        //attr->fsType ... // doesn't need to be set
    }

    _wpi_free( fonts );

    return( found_match );
}


HFONT _wpi_f_selectfont( WPI_PRES pres, HFONT f )
/***********************************************/
/* this code mimics the behaviour of the OS/2 font palette program.
   Hence, corresponding fonts in the palette program and
   from this code will appear the same. */

{
    WPI_F_FONT                  *font;
    FATTRS                      attr;
    ULONG                       mask;
    char                        old_face[FACESIZE];
    char                        tmp_face[500];
    WPI_F_FONT                  *old_font;
    BOOL                        find_normal;
    CHARBUNDLE                  bundle;

    font = (void *)f;

    _wpi_malloc2( old_font, 1 );
    get_f_attrs( pres, old_font );

    /* The OS/2 font palette will choose a non-synthesized bold/italic
       font by title, if you ask for a 'bold' one */
    find_normal = TRUE;
    if( font->attr.fsSelection & FATTR_SEL_ITALIC ||
                                font->attr.fsSelection & FATTR_SEL_BOLD ) {
        strcpy( old_face, font->attr.szFacename );
        strcpy( tmp_face, font->attr.szFacename );
        if( font->attr.fsSelection & FATTR_SEL_BOLD ) {
            strcat( tmp_face, " Bold" );
        }
        if( font->attr.fsSelection & FATTR_SEL_ITALIC ) {
            strcat( tmp_face, " Italic" );
        }
        if( strlen( tmp_face ) < FACESIZE ) {
            strcpy( font->attr.szFacename, tmp_face );
            if( find_font( pres, &attr, font ) ) {
                find_normal = FALSE;
                attr.fsSelection &= ~FATTR_SEL_BOLD;
                attr.fsSelection &= ~FATTR_SEL_ITALIC;
            }
            strcpy( font->attr.szFacename, old_face );
        }
    }
    if( find_normal ) {
        find_font( pres, &attr, font );
    }

    /* select the font in the PS */
    GpiSetCharSet( pres, LCID_DEFAULT );
    GpiDeleteSetId( pres, FONT_SET_ID );        // delete any leftover first
    GpiCreateLogFont( pres, (PSTR8) NULL, FONT_SET_ID, &attr );
    GpiSetCharSet( pres, FONT_SET_ID );

    /* set the charbundle features */
    if( attr.fsFontUse & FATTR_FONTUSE_OUTLINE ) {
        mask = CBB_BOX;
        if( font->bundle.ptlAngle.x != 0 || font->bundle.ptlAngle.y != 0 ) {
            mask |= CBB_ANGLE;
        }
        if( font->bundle.ptlShear.x != 0 || font->bundle.ptlShear.y != 0 ) {
            mask |= CBB_SHEAR;
        }
        GpiSetAttrs( pres, PRIM_CHAR, mask, 0, &font->bundle );
    } else {
        /* we stil must set the attrs for image fonts. AND, DON'T TRY
           and set these to 'defaults'; the defaults are different for
           different devices! */
        bundle.ptlAngle.x = 1;
        bundle.ptlAngle.y = 0;
        bundle.ptlShear.x = 0;
        bundle.ptlShear.y = 1;
        GpiSetAttrs( pres, PRIM_CHAR,
                        CBB_ANGLE | CBB_SHEAR | CBB_BOX, CBB_BOX, &bundle );
    }

    return( (HFONT) old_font );
}

void _wpi_f_getsystemfont( WPI_PRES in_pres, WPI_F_FONT *font )
/*************************************************************/

{
    FATTRS                      attr;
    WPI_F_FONT                  old_font;
    WPI_PRES                    pres;

    if( in_pres == (WPI_PRES)NULL ) {
        /* assume screen */
        pres = _wpi_getpres( HWND_DESKTOP );
    } else {
        pres = in_pres;
    }
    /* select a default font into the presentation space */
    memset( &attr, 0, sizeof( attr ) );
    attr.usRecordLength = sizeof( FATTRS );

    get_f_attrs( pres, &old_font );

    GpiSetCharSet( pres, LCID_DEFAULT );
    GpiDeleteSetId( pres, FONT_SET_ID );        // delete any leftover first
    GpiCreateLogFont( pres, (PSTR8) NULL, FONT_SET_ID, &attr );
    GpiSetCharSet( pres, FONT_SET_ID );

    get_f_attrs( pres, font );

    _wpi_f_deletefont( (WPI_F_FONT *)_wpi_f_selectfont( pres,
                                                (HFONT)&old_font ) );

    if( in_pres == (WPI_PRES)NULL ) {
        _wpi_releasepres( HWND_DESKTOP, pres );
    }
}

void _wpi_f_default( WPI_F_FONT *font )
/*************************************/
/* this is here, rather than wpi_os2.h, so that people don't have
   to recompile the world when we change WPI_F_FONT */

{
    memset( font, 0, sizeof( *font ) );
}

LONG _wpi_f_getfontsize( WPI_F_FONT *font )
/*****************************************/

{
    return( font->pt_size );
}

/***** end of _WPI_F_* Font Functions *****/
