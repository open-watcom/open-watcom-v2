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
* Description:  Default Unicode conversion table.
*
****************************************************************************/


/*
FROM: John Bennett @ GO

Code Page
437          UNICODE     IBM ID          Name
---          -------     ------          ---- */
unsigned short  UniCode[] = {
/* 0x00 */   0x0000,
/* 0x01 */   0x0001,
/* 0x02 */   0x0002,
/* 0x03 */   0x0003,
/* 0x04 */   0x0004,
/* 0x05 */   0x0005,
/* 0x06 */   0x0006,
/* 0x07 */   0x0007,
/* 0x08 */   0x0008,
/* 0x09 */   0x0009,
/* 0x0A */   0x000A,
/* 0x0B */   0x000B,
/* 0x0C */   0x000C,
/* 0x0D */   0x000D,
/* 0x0E */   0x000E,
/* 0x0F */   0x000F,
/* 0x10 */   0x0010,
/* 0x11 */   0x0011,
/* 0x12 */   0x0012,
/* 0x13 */   0x0013,
/* 0x14 */   0x0014,
/* 0x15 */   0x0015,
/* 0x16 */   0x0016,
/* 0x17 */   0x0017,
/* 0x18 */   0x0018,
/* 0x19 */   0x0019,
/* 0x1A */   0x001A,
/* 0x1B */   0x001B,
/* 0x1C */   0x001C,
/* 0x1D */   0x001D,
/* 0x1E */   0x001E,
/* 0x1F */   0x001F,
/* 0x20 */   0x0020,
/* 0x21 */   0x0021,
/* 0x22 */   0x0022,
/* 0x23 */   0x0023,
/* 0x24 */   0x0024,
/* 0x25 */   0x0025,
/* 0x26 */   0x0026,
/* 0x27 */   0x0027,
/* 0x28 */   0x0028,
/* 0x29 */   0x0029,
/* 0x2A */   0x002A,
/* 0x2B */   0x002B,
/* 0x2C */   0x002C,
/* 0x2D */   0x002D,
/* 0x2E */   0x002E,
/* 0x2F */   0x002F,
/* 0x30 */   0x0030,
/* 0x31 */   0x0031,
/* 0x32 */   0x0032,
/* 0x33 */   0x0033,
/* 0x34 */   0x0034,
/* 0x35 */   0x0035,
/* 0x36 */   0x0036,
/* 0x37 */   0x0037,
/* 0x38 */   0x0038,
/* 0x39 */   0x0039,
/* 0x3A */   0x003A,
/* 0x3B */   0x003B,
/* 0x3C */   0x003C,
/* 0x3D */   0x003D,
/* 0x3E */   0x003E,
/* 0x3F */   0x003F,
/* 0x40 */   0x0040,
/* 0x41 */   0x0041,
/* 0x42 */   0x0042,
/* 0x43 */   0x0043,
/* 0x44 */   0x0044,
/* 0x45 */   0x0045,
/* 0x46 */   0x0046,
/* 0x47 */   0x0047,
/* 0x48 */   0x0048,
/* 0x49 */   0x0049,
/* 0x4A */   0x004A,
/* 0x4B */   0x004B,
/* 0x4C */   0x004C,
/* 0x4D */   0x004D,
/* 0x4E */   0x004E,
/* 0x4F */   0x004F,
/* 0x50 */   0x0050,
/* 0x51 */   0x0051,
/* 0x52 */   0x0052,
/* 0x53 */   0x0053,
/* 0x54 */   0x0054,
/* 0x55 */   0x0055,
/* 0x56 */   0x0056,
/* 0x57 */   0x0057,
/* 0x58 */   0x0058,
/* 0x59 */   0x0059,
/* 0x5A */   0x005A,
/* 0x5B */   0x005B,
/* 0x5C */   0x005C,
/* 0x5D */   0x005D,
/* 0x5E */   0x005E,
/* 0x5F */   0x005F,
/* 0x60 */   0x0060,
/* 0x61 */   0x0061,
/* 0x62 */   0x0062,
/* 0x63 */   0x0063,
/* 0x64 */   0x0064,
/* 0x65 */   0x0065,
/* 0x66 */   0x0066,
/* 0x67 */   0x0067,
/* 0x68 */   0x0068,
/* 0x69 */   0x0069,
/* 0x6A */   0x006A,
/* 0x6B */   0x006B,
/* 0x6C */   0x006C,
/* 0x6D */   0x006D,
/* 0x6E */   0x006E,
/* 0x6F */   0x006F,
/* 0x70 */   0x0070,
/* 0x71 */   0x0071,
/* 0x72 */   0x0072,
/* 0x73 */   0x0073,
/* 0x74 */   0x0074,
/* 0x75 */   0x0075,
/* 0x76 */   0x0076,
/* 0x77 */   0x0077,
/* 0x78 */   0x0078,
/* 0x79 */   0x0079,
/* 0x7A */   0x007A,
/* 0x7B */   0x007B,
/* 0x7C */   0x007C,
/* 0x7D */   0x007D,
/* 0x7E */   0x007E,
/* 0x7F */   0x007F,
/* 0x80 */   0x00C7,  //LC420000        LATIN CAPITAL LETTER C CEDILLA
/* 0x81 */   0x00FC,  //LU170000        LATIN SMALL LETTER U DIAERESIS
/* 0x82 */   0x00E9,  //LE110000        LATIN SMALL LETTER E ACUTE
/* 0x83 */   0x00E2,  //LA150000        LATIN SMALL LETTER A CIRCUMFLEX
/* 0x84 */   0x00E4,  //LA170000        LATIN SMALL LETTER A DIAERESIS
/* 0x85 */   0x00E0,  //LA130000        LATIN SMALL LETTER A GRAVE
/* 0x86 */   0x00E5,  //LA270000        LATIN SMALL LETTER A RING
/* 0x87 */   0x00E7,  //LC410000        LATIN SMALL LETTER C CEDILLA
/* 0x88 */   0x00EA,  //LE150000        LATIN SMALL LETTER E CIRCUMFLEX
/* 0x89 */   0x00EB,  //LE170000        LATIN SMALL LETTER E DIAERESIS
/* 0x8A */   0x00E8,  //LE130000        LATIN SMALL LETTER E GRAVE
/* 0x8B */   0x00EF,  //LI170000        LATIN SMALL LETTER I DIAERESIS
/* 0x8C */   0x00EE,  //LI150000        LATIN SMALL LETTER I CIRCUMFLEX
/* 0x8D */   0x00EC,  //LI130000        LATIN SMALL LETTER I GRAVE
/* 0x8E */   0x00C4,  //LA180000        LATIN CAPITAL LETTER A DIAERESIS
/* 0x8F */   0x00C5,  //LA280000        LATIN CAPITAL LETTER A RING
/* 0x90 */   0x00C9,  //LE120000        LATIN CAPITAL LETTER E ACUTE
/* 0x91 */   0x00E6,  //LA510000        LATIN SMALL LETTER A E
/* 0x92 */   0x00C6,  //LA520000        LATIN CAPITAL LETTER A E
/* 0x93 */   0x00F4,  //LO150000        LATIN SMALL LETTER O CIRCUMFLEX
/* 0x94 */   0x00F6,  //LO170000        LATIN SMALL LETTER O DIAERESIS
/* 0x95 */   0x00F2,  //LO130000        LATIN SMALL LETTER O GRAVE
/* 0x96 */   0x00FB,  //LU150000        LATIN SMALL LETTER U CIRCUMFLEX
/* 0x97 */   0x00F9,  //LU130000        LATIN SMALL LETTER U GRAVE
/* 0x98 */   0x00FF,  //LY170000        LATIN SMALL LETTER Y DIAERESIS
/* 0x99 */   0x00D6,  //LO180000        LATIN CAPITAL LETTER O DIAERESIS
/* 0x9A */   0x00DC,  //LU180000        LATIN CAPITAL LETTER U DIAERESIS
/* 0x9B */   0x00A2,  //SC040000        CENT SIGN
/* 0x9C */   0x00A3,  //SC020000        POUND SIGN
/* 0x9D */   0x00A5,  //SC050000        YEN SIGN
/* 0x9E */   0x20A7,  //SC060000        PESETA SIGN
/* 0x9F */   0x0192,  //SC070000        LATIN SMALL LETTER SCRIPT F
/* 0xA0 */   0x00E1,  //LA110000        LATIN SMALL LETTER A ACUTE
/* 0xA1 */   0x00ED,  //LI110000        LATIN SMALL LETTER I ACUTE
/* 0xA2 */   0x00F3,  //LO110000        LATIN SMALL LETTER O ACUTE
/* 0xA3 */   0x00FA,  //LU110000        LATIN SMALL LETTER U ACUTE
/* 0xA4 */   0x00F1,  //LN190000        LATIN SMALL LETTER N TILDE
/* 0xA5 */   0x00D1,  //LN200000        LATIN CAPITAL LETTER N TILDE
/* 0xA6 */   0x00AA,  //SM210000        FEMININE ORDINAL INDICATOR
/* 0xA7 */   0x00BA,  //SM200000        MASCULINE ORDINAL INDICATOR
/* 0xA8 */   0x00BF,  //SP160000        INVERTED QUESTION MARK
/* 0xA9 */   0x2310,  //SM680000        REVERSED NOT SIGN
/* 0xAA */   0x00AC,  //SM660000        NOT SIGN
/* 0xAB */   0x00BD,  //NF010000        FRACTION ONE HALF
/* 0xAC */   0x00BC,  //NF040000        FRACTION ONE QUARTER
/* 0xAD */   0x00A1,  //SP030000        INVERTED EXCLAMATION MARK
/* 0xAE */   0x00AB,  //SP170000        LEFT POINTING GUILLEMET
/* 0xAF */   0x00BB,  //SP180000        RIGHT POINTING GUILLEMET
/* 0xB0 */   0x2591,  //SF140000        LIGHT SHADE
/* 0xB1 */   0x2592,  //SF150000        MEDIUM SHADE
/* 0xB2 */   0x2593,  //SF160000        DARK SHADE
/* 0xB3 */   0x2502,  //SF110000        FORMS LIGHT VERTICAL
/* 0xB4 */   0x2524,  //SF090000        FORMS LIGHT VERTICAL AND LEFT
/* 0xB5 */   0x2561,  //SF190000        FORMS VERTICAL SINGLE AND LEFT DOUBLE
/* 0xB6 */   0x2562,  //SF200000        FORMS VERTICAL DOUBLE AND LEFT SINGLE
/* 0xB7 */   0x2556,  //SF210000        FORMS DOWN DOUBLE AND LEFT SINGLE
/* 0xB8 */   0x2555,  //SF220000        FORMS DOWN SINGLE AND LEFT DOUBLE
/* 0xB9 */   0x2563,  //SF230000        FORMS DOUBLE VERTICAL AND LEFT
/* 0xBA */   0x2551,  //SF240000        FORMS DOUBLE VERTICAL
/* 0xBB */   0x2557,  //SF250000        FORMS DOUBLE DOWN AND LEFT
/* 0xBC */   0x255D,  //SF260000        FORMS DOUBLE UP AND LEFT
/* 0xBD */   0x255C,  //SF270000        FORMS UP DOUBLE AND LEFT SINGLE
/* 0xBE */   0x255B,  //SF280000        FORMS UP SINGLE AND LEFT DOUBLE
/* 0xBF */   0x2510,  //SF030000        FORMS LIGHT DOWN AND LEFT
/* 0xC0 */   0x2514,  //SF020000        FORMS LIGHT UP AND RIGHT
/* 0xC1 */   0x2534,  //SF070000        FORMS LIGHT UP AND HORIZONTAL
/* 0xC2 */   0x252C,  //SF060000        FORMS LIGHT DOWN AND HORIZONTAL
/* 0xC3 */   0x251C,  //SF080000        FORMS LIGHT VERTICAL AND RIGHT
/* 0xC4 */   0x2500,  //SF100000        FORMS LIGHT HORIZONTAL
/* 0xC5 */   0x253C,  //SF050000        FORMS LIGHT VERTICAL AND HORIZONTAL
/* 0xC6 */   0x255E,  //SF360000        FORMS VERTICAL SINGLE AND RIGHT DOUBLE
/* 0xC7 */   0x255F,  //SF370000        FORMS VERTICAL DOUBLE AND RIGHT SINGLE
/* 0xC8 */   0x255A,  //SF380000        FORMS DOUBLE UP AND RIGHT
/* 0xC9 */   0x2554,  //SF390000        FORMS DOUBLE DOWN AND RIGHT
/* 0xCA */   0x2569,  //SF400000        FORMS DOUBLE UP AND HORIZONTAL
/* 0xCB */   0x2566,  //SF410000        FORMS DOUBLE DOWN AND HORIZONTAL
/* 0xCC */   0x2560,  //SF420000        FORMS DOUBLE VERTICAL AND RIGHT
/* 0xCD */   0x2550,  //SF430000        FORMS DOUBLE HORIZONTAL
/* 0xCE */   0x256C,  //SF440000        FORMS DOUBLE VERTICAL AND HORIZONTAL
/* 0xCF */   0x2567,  //SF450000        FORMS UP SINGLE AND HORIZONTAL DOUBLE
/* 0xD0 */   0x2568,  //SF460000        FORMS UP DOUBLE AND HORIZONTAL SINGLE
/* 0xD1 */   0x2564,  //SF470000        FORMS DOWN SINGLE AND HORIZONTAL DOUBLE
/* 0xD2 */   0x2565,  //SF480000        FORMS DOWN DOUBLE AND HORIZONTAL SINGLE
/* 0xD3 */   0x2559,  //SF490000        FORMS UP DOUBLE AND RIGHT SINGLE
/* 0xD4 */   0x2558,  //SF500000        FORMS UP SINGLE AND RIGHT DOUBLE
/* 0xD5 */   0x2552,  //SF510000        FORMS DOWN SINGLE AND RIGHT DOUBLE
/* 0xD6 */   0x2553,  //SF520000        FORMS DOWN DOUBLE AND RIGHT SINGLE
/* 0xD7 */   0x256B,  //SF530000        FORMS VERTICAL DOUBLE AND HORIZONTAL SINGLE
/* 0xD8 */   0x256A,  //SF540000        FORMS VERTICAL SINGLE AND HORIZONTAL DOUBLE
/* 0xD9 */   0x2518,  //SF040000        FORMS LIGHT UP AND LEFT
/* 0xDA */   0x250C,  //SF010000        FORMS LIGHT DOWN AND RIGHT
/* 0xDB */   0x2588,  //SF610000        FULL BLOCK
/* 0xDC */   0x2584,  //SF570000        LOWER HALF BLOCK
/* 0xDD */   0x258C,  //SF580000        LEFT HALF BLOCK
/* 0xDE */   0x2590,  //SF590000        RIGHT HALF BLOCK
/* 0xDF */   0x2580,  //SF600000        UPPER HALF BLOCK
/* 0xE0 */   0x03B1,  //GA010000        GREEK SMALL LETTER ALPHA
/* 0xE1 */   0x00DF,  //LS610000        LATIN SMALL LETTER SHARP S
/* 0xE2 */   0x0393,  //GG020000        GREEK CAPITAL LETTER GAMMA
/* 0xE3 */   0x03C0,  //GP010000        GREEK SMALL LETTER PI
/* 0xE4 */   0x03A3,  //GS020000        GREEK CAPITAL LETTER SIGMA
/* 0xE5 */   0x03C3,  //GS010000        GREEK SMALL LETTER SIGMA
/* 0xE6 */   0x00B5,  //SM170000        MICRO SIGN
/* 0xE7 */   0x03C4,  //GT010000        GREEK SMALL LETTER TAU
/* 0xE8 */   0x03A6,  //GF020000        GREEK CAPITAL LETTER PHI
/* 0xE9 */   0x0398,  //GT620000        GREEK CAPITAL LETTER THETA
/* 0xEA */   0x03A9,  //GO320000        GREEK CAPITAL LETTER OMEGA
/* 0xEB */   0x03B4,  //GD010000        GREEK SMALL LETTER DELTA
/* 0xEC */   0x221E,  //SA450000        INFINITY
/* 0xED */   0x03C6,  //GF010000        GREEK SMALL LETTER PHI
/* 0xEE */   0x03B5,  //GE010000        GREEK SMALL LETTER EPSILON
/* 0xEF */   0x2229,  //SA380000        INTERSECTION
/* 0xF0 */   0x2261,  //SA480000        IDENTICAL TO
/* 0xF1 */   0x00B1,  //SA020000        PLUS-OR-MINUS SIGN
/* 0xF2 */   0x2265,  //SA530000        GREATER THAN OR EQUAL TO
/* 0xF3 */   0x2264,  //SA520000        LESS THAN OR EQUAL TO
/* 0xF4 */   0x2320,  //SS260000        TOP HALF INTEGRAL
/* 0xF5 */   0x2321,  //SS270000        BOTTOM HALF INTEGRAL
/* 0xF6 */   0x00F7,  //SA060000        DIVISION SIGN
/* 0xF7 */   0x2248,  //SA700000        ALMOST EQUAL TO
/* 0xF8 */   0x00B0,  //SM190000        DEGREE SIGN
/* 0xF9 */   0x2219,  //SA790000        DOT OPERATOR
/* 0xFA */   0x00B7,  //SD630000        MIDDLE DOT
/* 0xFB */   0x221A,  //SA800000        SQUARE ROOT
/* 0xFC */   0x207F,  //LN011000        SUPERSCRIPT LATIN SMALL LETTER N
/* 0xFD */   0x00B2,  //ND021000        SUPERSCRIPT DIGIT TWO
/* 0xFE */   0x25A0,  //SM470000        BLACK SQUARE
/* 0xFF */   0x00A0   //SP300000        NON-BREAKING SPACE
};
