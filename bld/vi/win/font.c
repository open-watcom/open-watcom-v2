/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Font selection.
*
****************************************************************************/


#include "vi.h"
#include "font.h"

#ifdef __NT__
    #define STUPIDNTINT     long
#else
    #ifdef __WINDOWS_386__
        #define STUPIDNTINT short
    #else
        #define STUPIDNTINT int
    #endif
#endif

typedef struct font {
    HFONT       handle;
    LOGFONT     lf;
    UINT        height;
    UINT        avg_width;
    UINT        max_width;
    UINT        space_width;
    bool        used            : 1;
    bool        fixed           : 1;
//    bool        isFunnyItalic   : 1; ?
} font;

LOGFONT Helvetica6 = {
    -5, 0,          /* height and weight - we want a short, normal width font */
    0, 0,           /* Escapement and Orientation - who cares? */
    FW_NORMAL - 50, /* we want a slightly lighter font - 400 is average */
    0, 0, 0,        /* nothing special */
    DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY,
    VARIABLE_PITCH | FF_SWISS,
    "Helv"
};

LOGFONT Arial10 = {
    -14, 0,         /* Nice big, readable font */
    0, 0,           /* Escapement and Orientation - who cares? */
    FW_NORMAL,      /* strictly average */
    0, 0, 0,        /* nothing special */
    DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY,
    VARIABLE_PITCH | FF_SWISS,
    "Arial"
};

LOGFONT ArialBold10 = {
    -14, 0,         /* Nice big, readable font */
    0, 0,           /* Escapement and Orientation - who cares? */
    FW_BOLD,        /* bold */
    0, 0, 0,        /* nothing special */
    DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY,
    VARIABLE_PITCH | FF_SWISS,
    "Arial"
};

LOGFONT Fixed10 = {
    -12, 0,
    0, 0,
    FW_NORMAL,      /* strictly average */
    0, 0, 0,
    DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY,
    FIXED_PITCH | FF_DONTCARE,
    "Courier New"
};

LOGFONT SansSerif = {
    -16, 0,
    0, 0,
    FW_NORMAL,      /* strictly average */
    0, 0, 0,
    DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY,
    FIXED_PITCH | FF_DONTCARE,
    "MS Sans Serif"
};

LOGFONT Courier = {
    -13, 0,
    0, 0,
    FW_NORMAL,
    0, 0, 0,
    DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY,
    FIXED_PITCH | FF_DONTCARE,
    "Courier New"
};

LOGFONT CourierBold = {
    -13, 0,
    0, 0,
    FW_BOLD,
    0, 0, 0,
    DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY,
    FIXED_PITCH | FF_DONTCARE,
    "Courier New"
};

font Fonts[MAX_FONTS];

UINT FontFixed( font_type f )
{
    return( Fonts[f].fixed );
}

HFONT FontHandle( font_type f )
{
    return( Fonts[f].handle );
}

UINT FontHeight( font_type f )
{
    return( Fonts[f].height );
}

int FontlfHeight( font_type f )
{
    return( Fonts[f].lf.lfHeight );
}

char *FontlfFaceName( font_type f )
{
    return( Fonts[f].lf.lfFaceName );
}

char FontIsFunnyItalic( font_type f )
{
    return( !(Fonts[f].lf.lfPitchAndFamily & TMPF_TRUETYPE) && Fonts[f].lf.lfItalic );
}


UINT FontMaxWidth( font_type f )
{
    return( Fonts[f].max_width );
}

UINT FontAverageWidth( font_type f )
{
    return( Fonts[f].avg_width );
}

UINT FontTabWidth( font_type f )
{
    // return( Fonts[f].space_width * HardTab );
    return( Fonts[f].avg_width * EditVars.HardTab );
}

static UINT GetTextExtentX( HDC hdc, char *str, int cnt )
{
#ifndef __NT__
    return( LOWORD( GetTextExtent( hdc, str, cnt ) ) );
#else
    SIZE        sz;
    GetTextExtentPoint( hdc, str, cnt, &sz );
    return( sz.cx );
#endif
}

static void customFont( font *f, LOGFONT *lf )
{
    HDC         hdc;
    HFONT       old_font;
    TEXTMETRIC  tm;

    if( f->used ) {
        if( f->handle ) {
            DeleteObject( f->handle );
        }
    }
    f->used = true;
    f->fixed = ( (lf->lfPitchAndFamily & FIXED_PITCH) != 0 );
    f->handle = CreateFontIndirect( lf );
    hdc = GetDC( root_window_id );
    old_font = SelectObject( hdc, f->handle );
    GetTextMetrics( hdc, &tm );
    f->height = tm.tmHeight;
    f->avg_width = tm.tmAveCharWidth;
    f->max_width = tm.tmMaxCharWidth;
    f->space_width = GetTextExtentX( hdc, " ", 1 );
    SelectObject( hdc, old_font );
    ReleaseDC( root_window_id, hdc );
    memcpy( &f->lf, lf, sizeof( LOGFONT ) );
}

/*
 * EnsureUniformFonts - fonts between start & end get similar characteristics
 */
void EnsureUniformFonts( font_type start, font_type end, LOGFONT *given_lf, bool totally )
{
    LOGFONT     new_lf;
    font        *f;
    font_type   i;

    for( i = start; i <= end; i++ ) {
        f = &Fonts[i];
        memcpy( &new_lf, given_lf, sizeof( LOGFONT ) );

        if( !totally ) {
            // preserve old weight & italic settings
            new_lf.lfWeight = f->lf.lfWeight;
            new_lf.lfItalic = f->lf.lfItalic;
        }

        customFont( f, &new_lf );
    }
}

void InitFonts( void )
{
    font_type   i;
    font        *f;

    f = &Fonts[0];
    for( i = 0; i < MAX_FONTS; i++, f++ ) {
        f->used = false;
        f->handle = (HFONT)NULLHANDLE;
    }
    customFont( &Fonts[FONT_HELV], &Helvetica6 );
    customFont( &Fonts[FONT_ARIAL], &Arial10 );
    customFont( &Fonts[FONT_ARIALBOLD], &ArialBold10 );
    customFont( &Fonts[FONT_FIXED], &Fixed10 );
    customFont( &Fonts[FONT_SANSSERIF], &SansSerif );
    customFont( &Fonts[FONT_COURIER], &Courier );
    customFont( &Fonts[FONT_COURIERBOLD], &CourierBold );
}

static bool getInt( STUPIDNTINT *dest, const char **data )
{
    char        tmp[MAX_STR];

    *data = GetNextWord1( *data, tmp );
    if( *tmp == '\0' ) {
        return( false );
    }
    *dest = atoi( tmp );
    return( true );
}

static bool getByte( BYTE *dest, const char **data )
{
    char        tmp[MAX_STR];

    *data = GetNextWord1( *data, tmp );
    if( *tmp == '\0' ) {
        return( false );
    }
    *dest = (BYTE)atoi( tmp );
    return( true );
}

static bool getLogFont( LOGFONT *lf, const char **data )
{
    return( getInt( &lf->lfHeight, data ) &&
            getInt( &lf->lfWidth, data ) &&
            getInt( &lf->lfEscapement, data ) &&
            getInt( &lf->lfOrientation, data ) &&
            getInt( &lf->lfWeight, data ) &&
            getByte( &lf->lfItalic, data ) &&
            getByte( &lf->lfUnderline, data ) &&
            getByte( &lf->lfStrikeOut, data ) &&
            getByte( &lf->lfCharSet, data ) &&
            getByte( &lf->lfOutPrecision, data ) &&
            getByte( &lf->lfClipPrecision, data ) &&
            getByte( &lf->lfQuality, data ) &&
            getByte( &lf->lfPitchAndFamily, data ) &&
            GetStringWithPossibleQuote( data, &lf->lfFaceName[0] ) == ERR_NO_ERR );
}

static bool userPickFont( LOGFONT *lf, HWND parent )
{
    CHOOSEFONT  cf;

    memset( &cf, 0, sizeof( CHOOSEFONT ) );

    cf.lStructSize = sizeof( CHOOSEFONT );
    cf.hwndOwner = parent;
    cf.lpLogFont = lf;
    cf.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
    cf.rgbColors = RGB( 0, 0, 0 );
    cf.nFontType = SCREEN_FONTTYPE;

    return( ChooseFont( &cf ) );
}


/*
 * SetUpFont - set up a font, once it has been selected
 */
void SetUpFont( LOGFONT *lf, font_type index )
{
    info        *cinfo;
    font        *f;

    f = &Fonts[index];

    customFont( f, lf );
    for( cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
        DCResize( cinfo );
    }
    ResizeRoot();
    ResetExtraRects();
    if( EditFlags.WindowsStarted ) {
        ReDisplayScreen();
    }

} /* SetUpFont */

/*
 * initFont - init for font selection
 */
static void initFont( font_type index, LOGFONT *lf )
{
    if( Fonts[index].used ) {
        memcpy( lf, &Fonts[index].lf, sizeof( LOGFONT ) );
    } else {
        memset( lf, 0, sizeof( LOGFONT ) );
    }

} /* initFont */

/*
 * PickFont - pick a new font with the font dialog
 */
void PickFont( font_type index, HWND parent )
{
    LOGFONT     lf;

    initFont( index, &lf );
    if( !userPickFont( &lf, parent ) ) {
        return;
    }
    SetUpFont( &lf, index );

} /* PickFont */

/*
 * SetFont - process a set font command
 */
vi_rc SetFont( const char *data )
{
    LOGFONT     lf;
    STUPIDNTINT index;

    if( !getInt( &index, &data ) ) {
        return( ERR_INVALID_FONT );
    }
    if( index >= MAX_FONTS || index < 0 ) {
        return( ERR_INVALID_FONT );
    }
    initFont( index, &lf );
    /*
     * Either the user can specify 'setfont x' and choose a font
     * using the common dialog - or he/she can do the full
     * 'setfont x n n n n n n... ad nauseum to define a font.
     */
    while( isspace( *data ) ) {
        data++;
    }
    if( *data == '\0' ) {
        if( !userPickFont( &lf, root_window_id ) ) {
            return( ERR_NO_ERR );
        }
    } else {
        if( !getLogFont( &lf, &data ) ) {
            return( ERR_INVALID_FONT );
        }
    }
    SetUpFont( &lf, index );
    return( ERR_NO_ERR );

} /* SetFont */

void BarfFontData( FILE *file )
{
    font_type   i;
    font        *f;
    LOGFONT     *lf;

    f = &Fonts[0];
    for( i = 0; i < MAX_FONTS; i++, f++ ) {
        if( f->used ) {
            lf = &f->lf;
            /* ick... */
            MyFprintf( file,
                       "setfont %d %d %d %d %d %d %d %d %d %d %d %d %d %d \"%s\"\n", i,
                       lf->lfHeight, lf->lfWidth, lf->lfEscapement, lf->lfOrientation,
                       lf->lfWeight, (int)lf->lfItalic, (int)lf->lfUnderline,
                       (int)lf->lfStrikeOut, (int)lf->lfCharSet, (int)lf->lfOutPrecision,
                       (int)lf->lfClipPrecision, (int)lf->lfQuality,
                       (int)lf->lfPitchAndFamily, &lf->lfFaceName[0] );
        }
    }
}

void FiniFonts( void )
{
    font_type   i;
    font        *f;

    f = &Fonts[0];
    for( i = 0; i < MAX_FONTS; i++, f++ ) {
        if( f->used ) {
            DeleteObject( f->handle );
        }
    }
}
