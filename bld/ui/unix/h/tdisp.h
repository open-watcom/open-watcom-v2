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
* Description:  Terminal display attributes data declaration.
*
****************************************************************************/


#ifdef _AIX
    #define _HAS_NO_CHAR_BIT_FIELDS
#endif

#if defined( _HAS_NO_CHAR_BIT_FIELDS )
    #define _attr_blink( a ) (((a).blink_back_bold_fore >> 7) & 1)
    #define _attr_back( a )  (((a).blink_back_bold_fore >> 4) & 7)
    #define _attr_bold( a )  (((a).blink_back_bold_fore >> 3) & 1)
    #define _attr_fore( a )  ( (a).blink_back_bold_fore       & 7)
#else
    #define _attr_blink( a ) ((a).bits.blink)
    #define _attr_back( a )  ((a).bits.back)
    #define _attr_bold( a )  ((a).bits.bold)
    #define _attr_fore( a )  ((a).bits.fore)
#endif

typedef struct {
#if defined( _HAS_NO_CHAR_BIT_FIELDS )
    unsigned char   blink_back_bold_fore;
#elif defined( __BIG_ENDIAN__ )
    unsigned char   blink:1;
    unsigned char   back:3;
    unsigned char   bold:1;
    unsigned char   fore:3;
#else
    unsigned char   fore:3;
    unsigned char   bold:1;
    unsigned char   back:3;
    unsigned char   blink:1;
#endif
} attr_bits;
