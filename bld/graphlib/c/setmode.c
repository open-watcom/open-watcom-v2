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


#define MT_0        0x00000001                  /* bit masks for the modes  */
#define MT_1        0x00000002
#define MT_2        0x00000004
#define MT_3        0x00000008
#define MT_4        0x00000010
#define MT_5        0x00000020
#define MT_6        0x00000040
#define MT_7        0x00000080
#define MT_8        0x00000100
#define MT_9        0x00000200
#define MT_10       0x00000400
#define MT_11       0x00000800
#define MT_12       0x00001000
#define MT_13       0x00002000
#define MT_14       0x00004000
#define MT_15       0x00008000
#define MT_16       0x00010000
#define MT_17       0x00020000
#define MT_18       0x00040000
#define MT_19       0x00080000

#define CGA_MODES       MT_0 + MT_1 + MT_2 + MT_3 + MT_4 + MT_5 + MT_6
#define MONO_MODES      MT_7 + MT_15
#define JR_MODES        MT_8 + MT_9 + MT_10
#define EGA_MODES       MT_13 + MT_14 + MT_16
#define VGA_MODES       MT_17 + MT_18 + MT_19


static long ModeTable[ 18 ] = {
    0,                                              // no display in system
    MT_7,                                           // IBM MDPA
    CGA_MODES,                                      // IBM CGA
    CGA_MODES + EGA_MODES,                          // IBM EGA enhanced
    CGA_MODES + EGA_MODES - MT_16,                  // IBM EGA color
    MONO_MODES,                                     // IBM EGA mono
    CGA_MODES,                                      // IBM PGC
    CGA_MODES + MONO_MODES + EGA_MODES + VGA_MODES, // IBM VGA mono
    CGA_MODES + MONO_MODES + EGA_MODES + VGA_MODES, // IBM VGA color
    CGA_MODES + JR_MODES,                           // IBM PCjr CGA
    CGA_MODES + MT_17 + MT_19,                      // IBM MCGA color
    CGA_MODES + MT_17 + MT_19,                      // IBM MCGA analog mono
    CGA_MODES + MT_17 + MT_19,                      // IBM MCGA analog color
    MT_7+MT_11,                                     // HERCULES Mono
    MT_7+MT_11,                                     // HERCULES Mono Plus
    MT_7+MT_11,                                     // HERCULES InColor
    CGA_MODES + MONO_MODES + EGA_MODES + VGA_MODES, // SuperVGA (mono)
    CGA_MODES + MONO_MODES + EGA_MODES + VGA_MODES  // SuperVGA (colour)
};


short                   _NoClear = 0;       // allow user to control whether
                                            // SetMode clears the screen

short _ValidMode( short mode )
/*============================

    Check if desired mode is valid  */

{
    short               monitor;
    short               alternate;
    long                mode_test;

    mode &= 0x7F;                   // wipe regen bits
    monitor = _SysMonType();
    alternate = monitor >> 8;       // separate active/alternate adapters
    monitor &= 0xff;
    mode_test = 1L << mode;
    if( ModeTable[ monitor ] & mode_test ||       // check active
        ModeTable[ alternate ] & mode_test ) {    // check alternate
        return( TRUE );
    } else {
        return( FALSE );
    }
}


short _SetMode( short mode )
/*==========================

    This function sets the video mode on IBM PC family. */

{
    char far *          p;

    if( _ValidMode( mode ) ) {
        p = _BIOS_data( EQUIP_FLAGS );        // equipment flags
        *p &= 0xCF;                                 // remove previous settings
        if( mode == 7 || mode == 15 ) {
            *p |= 0x30;                                 // monochrome
        } else {
            *p |= 0x20;                                 // colour
        }
        if( _NoClear ) {
            mode |= 0x80;           // set high bit, screen won't be cleared
        }
        VideoInt( _BIOS_SET_MODE + mode, 0, 0, 0 );
    }
    return( GetVideoMode() );
}
