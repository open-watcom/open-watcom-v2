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
* Description:  Font file support (Windows .fon files, bitmap or vector).
*
****************************************************************************/


#include <string.h>
#include "gdefn.h"
#include "fontsupp.h"
#if !defined( _DEFAULT_WINDOWS )


typedef _Packed struct font_entry {
    short                   type;       // 0 == bitmap, 1 == vector
    short                   ascent;     // distance from top to baseline (in pixels)
    short                   width;      // character width in pixels, 0 == proportional
    short                   height;     // character height in pixels
    short                   avgwidth;   // average character width
    short                   firstchar;
    short                   lastchar;
    char                    filename[ 81 ];
    char                    facename[ 32 ];
    char                    filler;
    short                   version;
    char _WCI86FAR          *glyph_table;
    char _WCI86FAR          *bitmap_table;
    long                    start_offset;
    long                    glyph_offset;
    long                    bitmap_offset;
    unsigned short          bitmap_size;
    struct font_entry _WCI86FAR  *link;
} FONT_ENTRY;


typedef _Packed struct windows_font {
    short               dfVersion;
    long                dfSize;
    char                dfCopyright[ 60 ];
    short               dfType;     // 0 == bitmap, 1 == vector
    short               dfPoints;
    short               dfVertRes;
    short               dfHorizRes;
    short               dfAscent;
    short               dfInternalLeading;
    short               dfExternalLeading;
    char                dfItalic;
    char                dfUnderline;
    char                dfStrikeOut;
    short               dfWeight;
    char                dfCharSet;
    short               dfPixWidth;
    short               dfPixHeight;
    char                dfPitchAndFamily;
    short               dfAvgWidth;
    short               dfMaxWidth;
    unsigned char       dfFirstChar;
    unsigned char       dfLastChar;
    char                dfDefaultChar;
    char                dfBreakChar;
    short               dfWidthBytes;
    long                dfDevice;
    long                dfFace;
    long                dfBitsPointer;
    long                dfBitsOffset;
    // additional fields have been omitted
} WINDOWS_FONT;
#endif


#define _UNDEFINED      (-1)

#define _BITMAP         0
#define _STROKE         1

static short            _XVecDir = 1;      // text vector direction
static short            _YVecDir = 0;

#if defined( _DEFAULT_WINDOWS )
  #if defined( __WINDOWS__ )
    static short        StockFont = TRUE;
  #else
    // Font ID for OS2
    #include< limits.h >
    #define _STDFONTID  250
  #endif
    static int YVec2Degs( short YDir );
#else
    #define _PROPORTIONAL   0
    #define _FIXED          1

    extern FONT_ENTRY _WCI86FAR  _8x8Font;
    static FONT_ENTRY _WCI86FAR  *_CurFont = &_8x8Font;
    static FONT_ENTRY _WCI86FAR  *_FontList = NULL;
    static float            _XVecScale = 1;    // magnification factor for
    static float            _YVecScale = 1;    // stroke fonts
#endif

#if defined( __386__ )
    #define MemCpy( dst, src, len )     memcpy( dst, src, len )
    #define MemSet( s, c, len )         memset( s, c, len )
    #define StrNICmp( s1, s2, len )     strnicmp( s1, s2, len )
    #define StrCpy( dst, src )          strcpy( dst, src )
    #define StrCmp( dst, src )          strcmp( dst, src )
    #define StrLen( s )                 strlen( s )
#else
    #define MemCpy( dst, src, len )     _fmemcpy( dst, src, len )
    #define MemSet( s, c, len )         _fmemset( s, c, len )
    #define StrNICmp( s1, s2, len )     _fstrnicmp( s1, s2, len )
    #define StrCpy( dst, src )          _fstrcpy( dst, src )
    #define StrCmp( dst, src )          _fstrcmp( dst, src )
    #define StrLen( s )                 _fstrlen( s )
#endif

#if defined( __QNX__ )
  #include <dirent.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <malloc.h>
  #if !defined( __386__ )
    #include <sys/slib16.h>
  #endif
  #define tiny_ret_t                    int
  #define tiny_handle_t                 int
  #define TINY_ERROR( rc )              ( rc < 0 )
  #define TINY_OK( rc )                 ( rc >= 0 )
  #define TINY_INFO( rc )               ( rc )
  #define TinyOpen( f, m )              __open_slib( f, O_RDONLY, 0 )
  #define FontSeekSet( f, o )           ( ( lseek( f, o, SEEK_SET ) == -1 ) ? -1 : 0 )
  #define TinyRead( f, b, l )           read( f, b, l )
  #define MyTinyFarRead( f, b, l )      read( f, b, l )
  #define TinyClose( f )                close( f )
#else
  #include "tinyio.h"
  #if defined( __386__ )
    #define MyTinyFarRead( h, b, l )    TinyRead( h, b, l )
  #else
    #define MyTinyFarRead( h, b, l )    TinyFarRead( h, b, l )
  #endif
  #define FontSeekSet( f, o )           TinySeek( f, o, TIO_SEEK_START )
#endif


#if !defined( _DEFAULT_WINDOWS )

static void _WCI86FAR *Alloc( unsigned short size )
//=================================================

{
#if defined( __QNX__ )
  #if defined( __386__ )
    return( malloc( size ) );
  #else
    return( MK_FP( qnx_segment_alloc( size ), 0 ) );
  #endif
#else
  #if defined( __386__ )
    return( malloc( size ) );
  #else
    tiny_ret_t          rc;

    rc = TinyAllocBlock( ( size + 15 ) / 16 );
    if( TINY_ERROR( rc ) ) {
        return( NULL );
    } else {
        return( MK_FP( TINY_INFO( rc ), 0 ) );
    }
  #endif
#endif
}


static void Free( void _WCI86FAR *p )
//==============================

{
#if defined( __QNX__ )
  #if defined( __386__ )
    free( p );
  #else
    qnx_segment_free( FP_SEG( p ) );
  #endif
#else
  #if defined( __386__ )
    free( p );
  #else
    TinyFreeBlock( FP_SEG( p ) );
  #endif
#endif
}


static short seek_and_read( tiny_handle_t handle, long offset,
                     void _WCI86FAR *buf, unsigned short len )
/*===========================================================*/
{
    tiny_ret_t          rc;
    short               rlen;

    if( TINY_ERROR( FontSeekSet( handle, offset ) ) ) {
        _ErrorStatus = _GRINVALIDFONTFILE;
        TinyClose( handle );
        return( 0 );
    }
    rc = MyTinyFarRead( handle, buf, len );
    if( TINY_ERROR( rc ) ) {
        _ErrorStatus = _GRINVALIDFONTFILE;
        TinyClose( handle );
        return( 0 );
    }
    rlen = TINY_INFO( rc );
    if( rlen != len ) {
        _ErrorStatus = _GRINVALIDFONTFILE;
        TinyClose( handle );
        return( 0 );
    }
    return( 1 );
}


#define RS_HEADER   ( 4 * sizeof( short ) )
#define RS_DESC     ( 6 * sizeof( short ) )


static short addfont( long offset, tiny_handle_t handle, char *font_file )
//========================================================================

{
    tiny_ret_t          rc;
    WINDOWS_FONT        w_font;
    FONT_ENTRY _WCI86FAR     *curr;
    char                facename[ 32 ];

//  printf( "found font at %lx\n", offset );
    if( seek_and_read( handle, offset, &w_font, sizeof( WINDOWS_FONT ) ) == 0 ) {
        return( 0 );
    }
    // read facename, can't use seek_and_read, since it might be at end of file
    if( TINY_ERROR( FontSeekSet( handle, offset + w_font.dfFace ) ) ) {
        _ErrorStatus = _GRINVALIDFONTFILE;
        TinyClose( handle );
        return( 0 );
    }
    rc = TinyRead( handle, facename, 32 );
    if( TINY_ERROR( rc ) ) {        // only check for error, not length
        _ErrorStatus = _GRINVALIDFONTFILE;
        TinyClose( handle );
        return( 0 );
    }
    facename[ 31 ] = '\0';
    curr = Alloc( sizeof( FONT_ENTRY ) );
    if( curr == NULL ) {
        _ErrorStatus = _GRINSUFFICIENTMEMORY;
        TinyClose( handle );
        return( 0 );
    }
    MemSet( curr, 0, sizeof( FONT_ENTRY ) );
    curr->type      = w_font.dfType & 1;
    curr->ascent    = w_font.dfAscent;
    curr->width     = w_font.dfPixWidth;
    curr->height    = w_font.dfPixHeight;
    curr->avgwidth  = w_font.dfAvgWidth;
    curr->firstchar = w_font.dfFirstChar;
    curr->lastchar  = w_font.dfLastChar;
    curr->version   = w_font.dfVersion;
    StrCpy( curr->filename, font_file );
    StrCpy( curr->facename, facename );
    curr->start_offset = offset;
    offset += sizeof( WINDOWS_FONT );
    if( curr->type == _BITMAP && ( offset & 1 ) ) {
        ++offset;
    }
    if( w_font.dfVersion == 0x0300 ) {
        offset += 30;   // size of extra stuff in V3.0 files
    }
    curr->glyph_offset = offset;
    curr->bitmap_offset = w_font.dfBitsOffset;
    curr->bitmap_size = w_font.dfSize - w_font.dfBitsOffset;
    curr->link = _FontList;
    _FontList = curr;
    return( 1 );
}


static short readfontfile( char *font_file )
//==========================================

{
    char                sig;
    long                ne_offset;
    short               rs_offset;
    short               shift_count;
    unsigned short      type;
    short               count;
    short               i;
    tiny_ret_t          rc;
    tiny_handle_t       handle;
    short               table[ RS_DESC ];

//  printf( "Found file '%s'\n", font_file );
    rc = TinyOpen( font_file, TIO_READ );
    if( TINY_ERROR( rc ) ) {
        _ErrorStatus = _GRFONTFILENOTFOUND;
        return( 0 );
    }
    handle = TINY_INFO( rc );
    // check for signature of 0x40 at location 0x18
    if( seek_and_read( handle, 0x18, &sig, sizeof( char ) ) == 0 ) {
        return( 0 );
    }
    if( sig != 0x40 ) {
        _ErrorStatus = _GRINVALIDFONTFILE;
        TinyClose( handle );
        return( 0 );
    }
    // get offset of NE header
    if( seek_and_read( handle, 0x3c, &ne_offset, sizeof( long ) ) == 0 ) {
        return( 0 );
    }
    // find start of resource table
    if( seek_and_read( handle, ne_offset + 0x24, &rs_offset, sizeof( short ) ) == 0 ) {
        return( 0 );
    }
    // get shift_count at start of resource table
    ne_offset += rs_offset;
    if( seek_and_read( handle, ne_offset, &shift_count, sizeof( short ) ) == 0 ) {
        return( 0 );
    }
    // read entries in resource table, looking for font definitions
    ne_offset += sizeof( short );   // skip over shift_count
    for( ;; ) {
        if( seek_and_read( handle, ne_offset, &table, RS_HEADER ) == 0 ) {
            return( 0 );
        }
        type = table[ 0 ];
        count = table[ 1 ];
        if( type == 0 ) {
            break;
        } else if( type == 0x8008 ) {   // font
            for( i = 0; i < count; ++i ) {
                if( seek_and_read( handle, ne_offset + RS_HEADER + i * RS_DESC, &table, RS_DESC ) == 0 ) {
                    return( 0 );
                }
                if( addfont( (long) table[ 0 ] << shift_count, handle, font_file ) == 0 ) {
                    return( 0 );
                }
            }
        }
        ne_offset += RS_HEADER + count * RS_DESC;
    }
    TinyClose( handle );
    return( 1 );
}


static short GlyphWidth( FONT_ENTRY _WCI86FAR *curr )
//==============================================

// The format of the glyph table is as follows
//   For _BITMAP fonts, there are two words: width, offset
//   For _STROKE fonts, there are two words: offset, width
//   Note: V3.0 _BITMAP fonts have a long offset, and
//         fixed width _STROKE fonts do not include the width.

{
    short               width;

    if( curr->type == _BITMAP ) {
        if( curr->version == 0x200 ) {
            width = 2 * sizeof( short );
        } else {
            width = sizeof( short ) + sizeof( long );
        }
    } else {
        if( curr->width == 0 ) {    // proportional
            width = 2 * sizeof( short );
        } else {
            width = sizeof( short );
        }
    }
    return( width );
}


_WCRTLINK short _WCI86FAR _CGRAPH _registerfonts( char _WCI86FAR *font_path )
//=======================================================

{
#if defined( __QNX__ )
    DIR _WCI86FAR            *dirp;
    struct dirent _WCI86FAR  *direntp;
#else
    tiny_ret_t          rc;
    tiny_find_t         fileinfo;
    char _WCI86FAR           *p;
#endif
    short               count;
    FONT_ENTRY _WCI86FAR     *curr;
    short               len;
    char                curr_file[ _MAX_PATH ];

    _ErrorStatus = _GROK;
    _unregisterfonts();     // free previous fonts, if any
    StrCpy( curr_file, font_path );     // copy into near buffer
#if defined( __QNX__ )
    #if !defined( __386__ )     // shared library returns far pointers
      #define _dir _dir __far
    #endif
    dirp = opendir( curr_file );
    #if !defined( __386__ )
      #undef _dir
    #endif
    if( dirp == NULL ) {
        _ErrorStatus = _GRFONTFILENOTFOUND;
        return( -1 );   // No such file
    } else {
        for( ;; ) {
            #if !defined( __386__ )
              #define dirent dirent __far
            #endif
            direntp = readdir( dirp );
            #if !defined( __386__ )
              #undef dirent
            #endif
            if( direntp == NULL ) {
                break;
            }
            len = StrLen( direntp->d_name );
            if( len > 4 && StrCmp( direntp->d_name + len - 4, ".fon" ) == 0 ) {
                StrCpy( curr_file, font_path );
                len = strlen( curr_file );
                curr_file[ len ] = '/';
                StrCpy( curr_file + len + 1, direntp->d_name );
                if( !readfontfile( curr_file ) ) {
                    return( -2 );   // bad font file or out of memory
                }
            }
        }
        closedir( dirp );
    }
#else
    TinySetDTA( &fileinfo );
    rc = TinyFindFirst( curr_file, TIO_NORMAL );
    if( TINY_ERROR( rc ) ) {
        _ErrorStatus = _GRFONTFILENOTFOUND;
        return( -1 );   // No such file
    } else {
        p = font_path + StrLen( font_path ) - 1;
        while( p != font_path ) {
            if( *p == '\\' || *p == '/' ) {
                ++p;
                break;
            }
            --p;
        }
        len = p - font_path;    // length of path specified
        do {
            MemCpy( curr_file, font_path, len );
            strcpy( curr_file + len, fileinfo.name );
            if( !readfontfile( curr_file ) ) {
                return( -2 );   // bad font file or out of memory
            }
            rc = TinyFindNext();
        } while( TINY_OK( rc ) );
    }
#endif
    count = 0;
    for( curr = _FontList; curr != NULL; curr = curr->link ) {
        ++count;
    }
    return( count );
}

Entry( _REGISTERFONTS, _registerfonts ) // alternate entry-point


_WCRTLINK void _WCI86FAR _CGRAPH _unregisterfonts( void )
//========================================

{
    FONT_ENTRY _WCI86FAR     *curr;
    FONT_ENTRY _WCI86FAR     *next;

    _ErrorStatus = _GROK;
    if( _CurFont != &_8x8Font ) {
        Free( _CurFont->glyph_table );
        Free( _CurFont->bitmap_table );
        _CurFont = &_8x8Font;
    }
    for( curr = _FontList; curr != NULL; ) {
        next = curr->link;
        Free( curr );
        curr = next;
    }
    _FontList = NULL;
}

Entry( _UNREGISTERFONTS, _unregisterfonts ) // alternate entry-point


static short loadfont( FONT_ENTRY _WCI86FAR *curr, short height, short width )
//=======================================================================

{
    tiny_ret_t          rc;
    tiny_handle_t       handle;
    short               num;
    short               glyph_width;
    char                file_name[ _MAX_PATH ];

    StrCpy( file_name, curr->filename );        // copy into near memory
    rc = TinyOpen( file_name, TIO_READ );
    if( TINY_ERROR( rc ) ) {
        _ErrorStatus = _GRFONTFILENOTFOUND;
        return( -1 );
    }
    handle = TINY_INFO( rc );
    num = curr->lastchar - curr->firstchar + 2;   // extra 1
    glyph_width = GlyphWidth( curr );
    curr->glyph_table = Alloc( num * glyph_width );
    if( curr->glyph_table == NULL ) {
        _ErrorStatus = _GRINSUFFICIENTMEMORY;
        TinyClose( handle );
        return( -1 );
    }
    if( seek_and_read( handle, curr->glyph_offset,
                       curr->glyph_table, num * glyph_width ) == 0 ) {
        return( -1 );
    }
    curr->bitmap_table = Alloc( curr->bitmap_size );
    if( curr->bitmap_table == NULL ) {
        _ErrorStatus = _GRINSUFFICIENTMEMORY;
        Free( curr->glyph_table );
        curr->glyph_table = NULL;
        TinyClose( handle );
        return( -1 );
    }
    if( seek_and_read( handle, curr->start_offset + curr->bitmap_offset,
                       curr->bitmap_table, curr->bitmap_size ) == 0 ) {
        return( -1 );
    }
    TinyClose( handle );
    _CurFont = curr;
    _XVecScale = 1;
    _YVecScale = 1;
    if( curr->type == _STROKE ) {
        if( width != 0 ) {
            _XVecScale = width / (float) curr->avgwidth;
        }
        if( height != 0 ) {
            _YVecScale = height / (float) curr->height;
        }
    }
    return( 0 );    // success
}


static short error_func( short font_type, short height, char *facename,
/*====================*/ short width, short spacing, FONT_ENTRY _WCI86FAR *curr )

{
    short               error = 0;
    short               len;
//char buff[ 80 ];

    if( curr->type == _BITMAP ) {
        if( font_type == _STROKE ) {
            error += 2000;
        }
    } else {
        if( font_type == _BITMAP ) {
            error += 2000;
        }
    }
    len = StrLen( facename );
    if( height != 0 && curr->type == _BITMAP && len == 0 ) {
        if( curr->height > height ) {
            error += 1000 + 5 * ( curr->height - height );
        } else if( curr->height < height ) {            // lower heights
            error += 1000 + height - curr->height;      // are favoured.
        }
    }
    if( len != 0 ) {
        if( StrNICmp( curr->facename, facename, len ) != 0 ) {
            error += 500;
        }
    }
    if( width != 0 && curr->type == _BITMAP ) {
        if( curr->avgwidth > width ) {
            error += 100 + curr->avgwidth - width;
        } else if( curr->avgwidth < width ) {
            error += 100 + width - curr->avgwidth;
        }
    }
    if( curr->width == 0 ) {
        if( spacing == _FIXED ) {
            error += 10;
        }
    } else {
        if( spacing == _PROPORTIONAL ) {
            error += 10;
        }
    }
//StrCpy( buff, curr->facename );
//printf( "    current font (%d) facename = '%s', type = %d, height = %d, width = %d, spacing = %d\n", error, buff, curr->type, curr->height, curr->avgwidth, curr->width );
    return( error );
}


#else

short _IsStockFont()
/*==================
This function tells if the current font is a stock font.*/
{
#if defined( __OS2__ )
    return( _CurFnt == LCID_DEFAULT );
#else
    return( StockFont );
#endif
}


_WCRTLINK short _WCI86FAR _CGRAPH _registerfonts( char _WCI86FAR *font_path )
//=======================================================

{
// We don't load the fonts for windows
    _ErrorStatus = _GROK;
    font_path = font_path;
    return( 1 );
}

Entry( _REGISTERFONTS, _registerfonts ) // alternate entry-point


_WCRTLINK void _WCI86FAR _CGRAPH _unregisterfonts( void )
//========================================

{
}

Entry( _UNREGISTERFONTS, _unregisterfonts ) // alternate entry-point

#endif


static char _WCI86FAR *getnumber( char _WCI86FAR *str, short *num )
//=======================================================

{
    short               val;

    val = 0;
    while( *str >= '0' && *str <= '9' ) {
        val = val * 10 + ( *str - '0' );
        ++str;
    }
    *num = val;
    return( str );
}


#if defined( __OS2__ )
static PFONTMETRICS getfonts( WPI_PRES dc, PLONG fnts, char* facename )
/*=====================================================================
  This function is used to get all suitable the public fonts fontmetric. */
{
    PFONTMETRICS        afm = NULL;
    LONG                num = 200;

    *fnts = GpiQueryFonts( dc, QF_PUBLIC, facename, &num, sizeof( *afm ), NULL );
    afm = _MyAlloc( sizeof( *afm )*( *fnts ) );
    if( afm == NULL ){
        GpiQueryFonts( dc, QF_PUBLIC, facename, &num, sizeof( *afm ), afm );
    }
    return afm;
}
#endif


_WCRTLINK short _WCI86FAR _CGRAPH _setfont( char _WCI86FAR *opt )
//===========================================

{
    short               height;
    short               width;
    short               spacing;
    short               font_type;
    short               best_fit;
    char                option;
    char                *face;
    char                facename[ 32 ];
#if defined( __OS2__ )
    FATTRS              fat;
    PFONTMETRICS        afm;
    long                rc;
    long                i;
    long                numfnts;
    long                fntindex = -1;
    long                mindiff = LONG_MAX;
    long                diff;
#elif !defined( __WINDOWS__ )
    FONT_ENTRY _WCI86FAR     *curr;
    unsigned short      best_so_far = 65535;
    unsigned short      value;
    FONT_ENTRY _WCI86FAR     *best = NULL;
    short               font_num;
#endif

    _ErrorStatus = _GROK;
#if !defined( _DEFAULT_WINDOWS )
    if( _CurFont != &_8x8Font ) {   // free old defn
        Free( _CurFont->glyph_table );
        Free( _CurFont->bitmap_table );
        _CurFont = &_8x8Font;
    }
#endif
    if( opt == NULL ) {     // use default font
        return( 0 );
    }
#if !defined( _DEFAULT_WINDOWS )
    if( _FontList == NULL ) {
        return( -1 );       // no fonts have been registered
    }
#endif
    facename[ 0 ] = '\0';
    height = 0;
    width = 0;
    spacing = _UNDEFINED;
    font_type = _UNDEFINED;
    best_fit = FALSE;
#if !defined( _DEFAULT_WINDOWS )
    font_num = 0;
#endif
    while( *opt != '\0' ) {
        option = *opt;
        ++opt;
        switch( option ) {
        case 't':
        case 'T':
            face = facename;
            if( *opt != '\'' ) {
                return( -1 );
            }
            ++opt;
            while( *opt != '\'' ) {
                if( *opt == '\0' || face - facename >= sizeof( facename ) ) {
                    return( -1 );
                }
                *face = *opt;
                ++face;
                ++opt;
            }
            *face = '\0';
            ++opt;
            break;
        case 'h':
        case 'H':
            opt = getnumber( opt, &height );
            break;
        case 'w':
        case 'W':
            opt = getnumber( opt, &width );
            break;
        case 'f':
        case 'F':
#if defined( _DEFAULT_WINDOWS )
            spacing = FIXED_PITCH|FF_DONTCARE;
#else
            spacing = _FIXED;
#endif
            break;
        case 'p':
        case 'P':
#if defined( _DEFAULT_WINDOWS )
            spacing = VARIABLE_PITCH|FF_DONTCARE;
#else
            spacing = _PROPORTIONAL;
#endif
            break;
        case 'v':
        case 'V':
            font_type = _STROKE;
            break;
        case 'r':
        case 'R':
            font_type = _BITMAP;
            break;
        case 'b':
        case 'B':
            best_fit = TRUE;
            break;
        case 'n':
        case 'N':
#if defined( _DEFAULT_WINDOWS )
            return( -1 );
#else
            opt = getnumber( opt, &font_num );
            break;
#endif
        case ' ':   // blanks are allowed
            break;
        default:
            return( -1 );
        }
    }
#if defined( __WINDOWS__ )
    if( !_IsStockFont() ) {
        DeleteObject( _CurFnt );
    }
    _CurFnt = CreateFont( height, width, 0, 0, 400, 0, 0, 0,
                          ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS,
                          DEFAULT_QUALITY, spacing, facename );
    if( _CurFnt == NULL ) {
        _CurFnt = GetStockObject( SYSTEM_FONT );
        return( -1 );
    } else {
        StockFont = FALSE;
        return( 0 );
    }
#elif defined( __OS2__ )
    _CurFnt = LCID_DEFAULT;
    fat.usRecordLength = sizeof( fat );
    fat.fsSelection = 0;
    fat.lMatch = 0;
    fat.usCodePage = 0;
    fat.fsType = 0;
    fat.fsFontUse = FATTR_FONTUSE_OUTLINE;
    fat.lMaxBaselineExt = height;
    fat.lAveCharWidth = width;
    if( spacing & FIXED_PITCH ) {
        afm = getfonts( _Mem_dc, &numfnts, facename );
        if( afm ) {
            for( i=0; i < numfnts; i++ ) {
                if( ( afm[i].fsType & FM_TYPE_FIXED ) &&
                    ( afm[i].fsDefn & FM_DEFN_OUTLINE ) ){
                    if( ( afm[i].lMaxBaselineExt == height ) &&
                        ( afm[i].lAveCharWidth == width ) ) {
                        fntindex = i;
                        break;
                    } else {
                        diff = abs( afm[i].lMaxBaselineExt - height );
                        if( diff < mindiff ) {
                            fntindex = i;
                            mindiff = diff;
                        }
                    }
                }
            }
            if( fntindex != -1 ) {
                strcpy( fat.szFacename, afm[i].szFacename );
                fat.idRegistry = afm[i].idRegistry;
                fat.lMaxBaselineExt = afm[i].lMaxBaselineExt;
                fat.lAveCharWidth = afm[i].lAveCharWidth;
            }
        } else {
            return( -1 );
        }
    }
    if( GpiQueryCharSet( _Mem_dc ) == _STDFONTID ) {
        GpiSetCharSet( _Mem_dc, LCID_DEFAULT );
    }
    rc = GpiCreateLogFont( _Mem_dc, NULL, _STDFONTID, &fat );
    if( rc == FONT_MATCH ) {
        _CurFnt = _STDFONTID;
        return( 0 );
    } else {
        return( -1 );
    }
#else
    if( font_num != 0 ) {
        curr = _FontList;
        while( font_num > 1 ) {
            curr = curr->link;
            if( curr == NULL ) {
                return( -1 );
            }
            --font_num;
        }
        return( loadfont( curr, height, width ) );
    }
//printf( "looking for facename = '%s', type = %d, height = %d, width = %d, spacing = %d\n", facename, font_type, height, width, spacing );
    for( curr = _FontList; curr != NULL; curr = curr->link ) {
        value = error_func( font_type, height, facename, width, spacing, curr );
        if( value < best_so_far ) {
            best_so_far = value;
            best = curr;
        }
        if( best_so_far == 0 ) {
            return( loadfont( best, height, width ) );
        }
    }
    if( best_fit ) {
        return( loadfont( best, height, width ) );
    } else {
        return( -1 );
    }
#endif
}

Entry( _SETFONT, _setfont ) // alternate entry-point


#if !defined( _DEFAULT_WINDOWS )
static short _charwidth( short ch )
//================================

{
    short               width;
    short               glyph_width;
    short _WCI86FAR     *glyph;

    if( ch >= _CurFont->firstchar && ch <= _CurFont->lastchar ) {
        ch -= _CurFont->firstchar;
        glyph_width = GlyphWidth( _CurFont );
        glyph = (short _WCI86FAR *) ( _CurFont->glyph_table + ch * glyph_width );
        if( _CurFont->type == _BITMAP ) {
            return( *glyph );
        } else {
            if( _CurFont->width == 0 ) {    // proportional
                width = glyph[ 1 ];         // 2nd entry is the width
            } else {
                width = _CurFont->width;
            }
            return( width * _XVecScale );
        }
    } else {
        return( 0 );
    }
}
#endif


_WCRTLINK short _WCI86FAR _CGRAPH _getfontinfo( struct _fontinfo _WCI86FAR *font )
//============================================================

{
#if defined( _DEFAULT_WINDOWS )
    WPI_PRES            dc;
    HFONT               old_font;
    WPI_TEXTMETRIC      Font_info;
    int                 rc;

    dc = _Mem_dc;
    old_font = _MySelectFont( dc, _CurFnt );
    rc = _wpi_gettextmetrics( dc, &Font_info );
    if( rc == 0 ) {
        return ( -1 );
    }
    _wpi_gettextface( dc, 30, font->facename );
    _MyGetOldFont( dc, old_font );

    font->type = _STROKE;
    font->ascent = _wpi_metricascent( Font_info );
#if defined( __WINDOWS__ )
    if( Font_info.tmPitchAndFamily & 0x01 ) {
#else
    if( !( Font_info.fsType & FM_TYPE_FIXED ) ) {
#endif
        font->pixwidth = 0;
    } else {
        font->pixwidth = _wpi_metricmaxcharwidth( Font_info );
    }
    font->pixheight = _wpi_metricheight( Font_info );
    font->avgwidth = _wpi_metricavecharwidth( Font_info );
    StrCpy( font->filename, "\0" );
    return( 0 );
#else
    font->type = _CurFont->type;
    font->ascent = _CurFont->ascent;
    font->pixwidth = _CurFont->width;
    font->pixheight = _CurFont->height;
    font->avgwidth = _CurFont->avgwidth;
    StrCpy( font->filename, _CurFont->filename );
    StrCpy( font->facename, _CurFont->facename );
    return( 0 );
#endif
}

Entry( _GETFONTINFO, _getfontinfo ) // alternate entry-point


_WCRTLINK short _WCI86FAR _CGRAPH _getgtextextent( char _WCI86FAR *text )
//============================================================

/*  Calculates the width of 'text' in pixels.  */

{
    int                 width;
#if defined( _DEFAULT_WINDOWS )
    WPI_PRES            dc;
    HFONT               old_font;
    int                 height;

    dc = _Mem_dc;
    old_font  = _MySelectFont( dc, _CurFnt );
    _wpi_gettextextent( dc, text, StrLen( text ), &width, &height );
    height= height;
    _MyGetOldFont( dc, old_font );
#else
    width = 0;
    while( *text != '\0' ) {
        width += _charwidth( (unsigned char)*text );
        ++text;
    }
#endif
    return( width );
}

Entry( _GETGTEXTEXTENT, _getgtextextent ) // alternate entry-point


_WCRTLINK struct xycoord _WCI86FAR _CGRAPH _getgtextvector( void )
//=================================================

{
    struct xycoord      temp;

    temp.xcoord = _XVecDir;
    temp.ycoord = _YVecDir;
    return( temp );
}

Entry( _GETGTEXTVECTOR, _getgtextvector ) // alternate entry-point


_WCRTLINK struct xycoord _WCI86FAR _CGRAPH _setgtextvector( short x, short y )
//=============================================================

{
    struct xycoord      prev;

    prev = _getgtextvector();
    if( x != 0 || y != 0 ) {
        if( x > 0 ) {
            _XVecDir = 1;
        } else if( x == 0 ) {
            _XVecDir = 0;
        } else {
            _XVecDir = -1;
        }
        if( y > 0 ) {
            _YVecDir = 1;
        } else if( y == 0 ) {
            _YVecDir = 0;
        } else {
            _YVecDir = -1;
        }
    }
    return( prev );
}

Entry( _SETGTEXTVECTOR, _setgtextvector ) // alternate entry-point


#if defined( __WINDOWS__ )

static int YVec2Degs( short YDir )
//==============================
{
    switch( YDir ) {
        case 1  : return( 450 );
        case 0  : return( 0 );
        case -1 : return( -450 );
        default : return( 0 );
    }
}

#elif !defined( __OS2__ )

static void _outdot( short x, short y )
//=====================================

{
    gr_device _FARD     *dev_ptr;
    putdot_fn           *putdot;

    if( _L1OutCode( x, y ) == 0 ) {             /* check if inside viewport */
        dev_ptr = _CurrState->deviceptr;
        ( *dev_ptr->setup )( x, y, _CurrColor );
        putdot = dev_ptr->plot[ _PlotAct ];
        ( *putdot )( _Screen.mem, _Screen.colour, _Screen.mask );
    } else {
        _ErrorStatus = _GRNOOUTPUT;
    }
}


char _WCI86FAR *_getbitmap( short ch, short _WCI86FAR *width )
//=================================================

{
    long                offset;
    short _WCI86FAR     *glyph;

    ch -= _CurFont->firstchar;
    glyph = (short _WCI86FAR *) ( _CurFont->glyph_table + ch * GlyphWidth( _CurFont ) );
    *width = *glyph;
    ++glyph;
    if( _CurFont->version == 0x200 ) {
        offset = *glyph;
    } else {
        offset = *( (long _WCI86FAR *) glyph );
    }
    return( offset + _CurFont->bitmap_table - _CurFont->bitmap_offset );
}


static struct xycoord _outbitchar( short x0, short y0, short ch )
//==============================================================

{
    short               x, y;
    short               width;
    short               height;
    int                 mask;
    char _WCI86FAR      *bits;
    short               column;
    short               row;
    struct xycoord      pos;

    _StartDevice();
    height = _CurFont->height;
    bits = _getbitmap( ch, &width );
    pos.xcoord = x0 + _XVecDir * width;
    pos.ycoord = y0 - _YVecDir * width;
    while( width > 0 ) {
        for( row = 0; row < height; ++row ) {
            x = x0 + row * _YVecDir;
            y = y0 + row * _XVecDir;
            mask = 0x80;
            for( column = 0; column < width && mask != 0; ++column ) {
                if( *bits & mask ) {
                    _outdot( x, y );
                };
                x += _XVecDir;
                y -= _YVecDir;
                mask >>= 1;
            }
            ++bits;
        };
        width -= 8;
        x0 += 8 * _XVecDir;
        y0 -= 8 * _YVecDir;
    }
    _ResetDevice();
    return( pos );
}


static struct xycoord _outstrokechar( float x0, float y0, short ch )
//=================================================================

{
    signed char         x, y;
    float               x1, y1;
    short               length;
    short               width;
    long                offset;
    short               penup;
    signed char _WCI86FAR    *strokes;
    short _WCI86FAR          *glyph;
    struct xycoord      pos;

    ch -= _CurFont->firstchar;
    glyph = (short _WCI86FAR *) ( _CurFont->glyph_table + ch * GlyphWidth( _CurFont ) );
    offset = glyph[ 0 ];
    if( _CurFont->width == 0 ) {    // proportional
        width = glyph[ 1 ];
        length = glyph[ 2 ] - offset;
    } else {
        width = _CurFont->width;
        length = glyph[ 1 ] - offset;
    }
    strokes = (signed char _WCI86FAR *)_CurFont->bitmap_table + offset;
    x1 = x0 + _XVecDir * width * _XVecScale;
    pos.xcoord = x1 + roundoff( x1 );
    y1 = y0 - _YVecDir * width * _XVecScale;
    pos.ycoord =  y1 + roundoff( y1 );
    penup = TRUE;
    while( length != 0 ) {
        x = *strokes;
        ++strokes;
        --length;
        if( (unsigned char)x == 0x80 ) {
            penup = TRUE;
            continue;
        }
        y = *strokes;
        ++strokes;
        --length;
        x1 = x0 + _XVecDir * x * _XVecScale + _YVecDir * y * _YVecScale;
        y1 = y0 + _XVecDir * y * _YVecScale - _YVecDir * x * _XVecScale;
        if( penup ) {
            penup = FALSE;
        } else {
            _L1SLine( x0 + roundoff( x0 ), y0 + roundoff( y0 ),
                      x1 + roundoff( x1 ), y1 + roundoff( y1 ) );
        }
        x0 = x1;
        y0 = y1;
    }
    return( pos );
}
#endif


_WCRTLINK void _WCI86FAR _CGRAPH _outgtext( char _WCI86FAR *str )
//===========================================

{
#if defined( _DEFAULT_WINDOWS )
    WPI_PRES            dc;
    HFONT               old_font;
    HFONT               original_font;
    int                 width;
    int                 height;
    long                old_color;
    long                old_bkcolor;
    int                 x1, y1, x2, y2, outy;
    WPI_TEXTMETRIC      fmet;
  #if defined( __OS2__ )
    GRADIENTL           angle;
    GRADIENTL           Old_angle;
  #else
    char                facename[30];
    int                 escape;
  #endif
#else
    short               ch;
#endif
    struct xycoord      pos;

    if( !_GrProlog() ) {
        return;
    }
    pos.xcoord = _VtoPhysX( _CurrPos.xcoord );
    pos.ycoord = _VtoPhysY( _CurrPos.ycoord );
#if defined( _DEFAULT_WINDOWS )
    original_font = _CurFnt;
    if( _CurFnt == NULL ) {
  #if defined( __OS2__ )
        _CurFnt = LCID_DEFAULT;
  #else
        _CurFnt = GetStockObject( SYSTEM_FONT);
  #endif
    }
    dc = _Mem_dc;
#if defined( __WINDOWS__ )
    switch( _XVecDir ) {
        case 1  : escape = YVec2Degs( _YVecDir );
                  break;
        case 0  : escape = YVec2Degs( _YVecDir ) * 2;
                  break;
        case -1 : escape = 1800 - YVec2Degs( _YVecDir );
                  break;
    }
#else
    angle.x = _XVecDir;
    angle.y = _YVecDir;
#endif

    old_font = _MySelectFont( dc, _CurFnt );

#if defined( __WINDOWS__ )
    if( escape != 0 ) {
        // Create the window font for the current escape
        GetTextMetrics( dc, &fmet );
        GetTextFace( dc, 30, &facename );
        _CurFnt = CreateFont( fmet.tmHeight, fmet.tmAveCharWidth,
                                escape, 0, fmet.tmWeight, fmet.tmItalic,
                                fmet.tmUnderlined, fmet.tmStruckOut,
                                fmet.tmCharSet, OUT_DEFAULT_PRECIS,
                                CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                fmet.tmPitchAndFamily, facename );
        SelectObject( dc, _CurFnt );
    }
#else
    GpiSetCharMode( dc, CM_MODE3 );
    GpiQueryCharAngle( dc, &Old_angle);
    GpiSetCharAngle( dc, &angle );
#endif

    old_color = _wpi_settextcolor( dc, _Col2RGB( _CurrColor ) );
    old_bkcolor = _wpi_setbackcolour( dc, _Col2RGB( _CurrBkColor ) );
    x1 = pos.xcoord;
    y1 = _wpi_cvth_y( pos.ycoord, _GetPresHeight() );

#if defined( __OS2__ )
    _wpi_gettextmetrics( dc, &fmet );
    outy = y1 - _wpi_metricascent( fmet );
#else
    outy = y1;
#endif

    _wpi_textout( dc, x1, outy, str, StrLen( str ) );
    _wpi_gettextextent( dc, str, StrLen( str ), &width, &height );

    pos.xcoord += width;
    pos.ycoord += height;

// Clean up
    _wpi_settextcolor( dc, old_color );
    _wpi_setbackcolour( dc, old_bkcolor );
    _MyGetOldFont( dc, old_font );
#if defined( __WINDOWS__ )
    if( escape != 0 ) {
        DeleteObject( _CurFnt );
    }
#else
    GpiSetCharAngle( dc, &Old_angle );
#endif
    _CurFnt = original_font;

#else
    while( *str != '\0' ) {
        ch = (unsigned char)*str;
        if( ch >= _CurFont->firstchar && ch <= _CurFont->lastchar ) {
            if( _CurFont->type == _BITMAP ) {
                pos = _outbitchar( pos.xcoord, pos.ycoord, ch );
            } else {
                pos = _outstrokechar( pos.xcoord, pos.ycoord, ch );
            }
        }
        ++str;
    }
#endif
    _moveto( _GetLogX( pos.xcoord ), _GetLogY( pos.ycoord ) ); // update current position
    _CurrPos_w = _getwindowcoord( pos.xcoord, pos.ycoord );
    _GrEpilog();

#if defined( _DEFAULT_WINDOWS )
    x2 = pos.xcoord;
    y2 = _wpi_cvth_y( pos.ycoord, _GetPresHeight() );
#if defined( __OS2__ )
    y2 -= _wpi_metricascent( fmet );
#endif
    _MyInvalidate( x1, y1, x2, y2 );
    _RefreshWindow();
#endif
}

Entry( _OUTGTEXT, _outgtext ) // alternate entry-point
