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
* Description:  double-byte character processing utilities
*
****************************************************************************/


#include "ftnstd.h"
#include "csetinfo.h"
#include "charset.h"
#include "dbcsutil.h"


size_t  ExtractTextDBCS( const char *string, size_t len )
// Given a string of text, extract as much text as possible up to a maximum
// of "len" bytes so that we don't split double-byte characters.
{
    size_t      str_len;
    size_t      chr_len;

    str_len = 0;
    for( ;; ) {
        chr_len = CharSetInfo.character_width( string + str_len );
        if( str_len + chr_len > len )
            break;
        str_len += chr_len;
    }
    return( str_len );
}
