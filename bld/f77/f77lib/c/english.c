/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  English character set support
*
****************************************************************************/

#include "ftnstd.h"
#include "csetinfo.h"
#include "charset.h"
#include "sbcsutil.h"

// Modified:    By:             Reason:
// --------     ---             -------
// 92/12/18     G. Coschi       initial implementation
// 93/12/14     Alex Brodsky    Added Q as legal exponent character


static  bool    IsDoubleByteBlank( const char *ptr )
// Determine if character is a double-byte blank character.
{
    /* unused parameters */ (void)ptr;

    return( false );
}


static  bool    IsDoubleByteChar( char ch )
// Determine if character is a double-byte character.
{
    /* unused parameters */ (void)ch;

    return( false );
}


static size_t   CharacterWidth( const char PGM *ptr )
// Determine character width.
{
    /* unused parameters */ (void)ptr;

    return( 1 );
}


static  bool    IsForeign( char ch )
// Determine if character is a foreign character (i.e. non-ASCII).
{
    /* unused parameters */ (void)ch;

    return( false );
}


void    __UseEnglishCharSet( void )
{
    CharSetInfo.extract_text = ExtractTextSBCS;
    CharSetInfo.is_double_byte_blank = IsDoubleByteBlank;
    CharSetInfo.is_double_byte_char = IsDoubleByteChar;
    CharSetInfo.character_width = CharacterWidth;
    CharSetInfo.is_foreign = IsForeign;
}
