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


#include "gdefn.h"
#include "gbios.h"
#if defined( __386__ )
#include "extender.h"
#endif


#define _KANJI_SIZE         74

#if defined( __386__ )
    #define MemSet( s, c, len )         memset( s, c, len )
    #define MemCpy( dst, src, len )     memcpy( dst, src, len )
#else
    #define MemSet( s, c, len )         _fmemset( s, c, len )
    #define MemCpy( dst, src, len )     _fmemcpy( dst, src, len )
#endif


short _WCI86FAR _CGRAPH _getkanji( unsigned short code, char _WCI86FAR *image )
//===================================================================

{
    short               i, j, k;
    short               width;
    short               height;
    char                buffer[ _KANJI_SIZE ];

    if( code < 0x100 ) {
        if( code >= 0x20 && code <= 0x7f ) {
            code += 0x2900;
        } else if( code >= 0xa0 && code <= 0xdf ) {
            code = 0x2a00 + ( code - 0x80 );
        }
    }

#if defined( __386__ )
    if( _KanjiBuf.seg ) {
        if (_GRCGPort == 0x7C) {    // Check for normal or hires mode
            NECVideoInt( _BIOS_FONT_READ, _KanjiBuf.seg, 0, code );
            _fmemcpy( buffer, _KanjiBuf.buf, _KANJI_SIZE );
        } else {
            /* Note: Hires Font ints don't return a size header */
            NECHiresVideoInt( _BIOS_HIRES_FONT_READ, _KanjiBuf.seg, 0, code );
            _fmemcpy( &(buffer[2]), _KanjiBuf.buf, _KANJI_SIZE - 2 );
            buffer[ 0 ] = 3;
            if (height == 24 && _CharLen( code ) == 2) {
                buffer[ 1 ] = 3;
            } else {
                buffer[ 1 ] = 2;
            }
        }
    } else {
        _ErrorStatus = _GRERROR;
        return( 0 );
    }

#else
    if (_GRCGPort == 0x7C) {    // Check for normal or hires mode
        NECVideoInt( _BIOS_FONT_READ, FP_SEG( buffer ), FP_OFF( buffer ),
                                                                        code );
    } else {
        /* Note: Hires Font ints don't return a size header */
        NECHiresVideoInt( _BIOS_HIRES_FONT_READ, FP_SEG( &(buffer[2]) ),
                                                 FP_OFF( &(buffer[2]) ), code );
        buffer[ 0 ] = 3;
        if (height == 24 && _CharLen( code ) == 2) {
            buffer[ 1 ] = 3;
        } else {
            buffer[ 1 ] = 2;
        }
    }
#endif

    MemSet( image, 0, _KANJI_SIZE );
    height = 8 * buffer[ 0 ];
    width = 8 * buffer[ 1 ];
    image[ 0 ] = width;
    image[ 1 ] = height;


    if( width == 8 ) {
        MemCpy( &image[ 2 ], &buffer[ 2 ], height );

    } else {
        for( i = 2, j = 2; i < height + buffer[ 1 ]; i++, j += buffer[ 1 ] ) {
            for( k = 0; k < buffer[ 1 ]; k++ ) {
                image[ i + height * k ] = buffer[ j + k ];
            }
        }
    }

    return( 1 );
}


short _WCI86FAR _CGRAPH _setkanji( short code, char _WCI86FAR *image )
//==========================================================

{
    short               i, j, k;
    short               width;
    short               height;
    char                buffer[ _KANJI_SIZE ];

    if( code < 0x100 ) {
        if( code >= 0x20 && code <= 0x7f ) {
            code += 0x2900;
        } else if( code >= 0xa0 && code <= 0xdf ) {
            code = 0x2a00 + ( code - 0x80 );
        }
    }

    memset( buffer, 0, _KANJI_SIZE );
    width = image[ 0 ];
    height = image[ 1 ];
    buffer[ 0 ] = height / 8;
    buffer[ 1 ] = width / 8;

    if( width == 8 ) {
        MemCpy( &buffer[ 2 ], &image[ 2 ], height );
    } else {
        for( i = 2, j = 2; i < height + buffer[ 1 ]; i += buffer[ 1 ], j++ ) {
            for( k = 0; k < buffer[ 1 ]; k++ ) {
                buffer[ i + k ] = image[ j + k * height ];
            }
        }
    }

#if defined( __386__ )
    if( _KanjiBuf.seg ) {
        if (_GRCGPort == 0x7C) {    // Check for normal or hires mode
            _fmemcpy( _KanjiBuf.buf, buffer, _KANJI_SIZE );
            NECVideoInt( _BIOS_FONT_WRITE, _KanjiBuf.seg, 0, code );
        } else {
            /* Note: Hires Font ints don't return a size header */
            _fmemcpy( _KanjiBuf.buf, &(buffer[2]), _KANJI_SIZE - 2 );
            NECHiresVideoInt( _BIOS_HIRES_FONT_WRITE, _KanjiBuf.seg, 0, code );
        }
    } else {
        _ErrorStatus = _GRERROR;
        return( 0 );
    }

#else
    if (_GRCGPort == 0x7C) {    // Check for normal or hires mode
        NECVideoInt( _BIOS_FONT_WRITE, FP_SEG( buffer ), FP_OFF( buffer ),
                                                                        code );
    } else {
        /* Note: Hires Font ints don't return a size header */
        NECHiresVideoInt( _BIOS_HIRES_FONT_WRITE, FP_SEG( &(buffer[2]) ),
                                                 FP_OFF( &(buffer[2]) ), code );
    }
#endif

    return( 1 );
}


short _WCI86FAR _CGRAPH _kanjisize( short code )
//=========================================

{
    code = code;
    return( _KANJI_SIZE );
}
