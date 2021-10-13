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
* Description:  8x8 font definition.
*
****************************************************************************/


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

extern char         _WCI86FAR _8x8Font[];
#pragma aux _8x8Font "_*"

extern FONT_ENTRY   _WCI86FAR _8x8FontDef;
#pragma aux _8x8FontDef "_*"
