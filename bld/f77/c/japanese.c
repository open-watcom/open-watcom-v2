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
* Description:  Japanese character set support
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "ftextvar.h"
#include "scan.h"

// Double-byte characters are represented as follows:
//
//    0x81 <= chr <= 0x9f --> 1st byte of 2-byte Japanese character
//    0xa0 <= chr <= 0xdf --> single-byte Hiragana
//    0xe0 <= chr <= 0xfc --> 1st byte of 2-byte Japanese character
//
// The second byte of 2-byte Japanese characters is in the range:
//
//    0x40 <= chr <= 0xfc, chr != 0x7f

#if !defined( __RT__ )

static const byte __FAR CharSet[] = {

//   00 NUL 01 SOH 02 STX 03 ETX 04 EOT 05 ENQ 06 ACK 07 BEL
     C_EL,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // NUL to BEL

//   08 BS  09 HT  0A LF  0B VT  0C FF  0D CR  0E SO  0F SI
     C_BC,  C_TC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // BS  to SI

//   10 DLE 11 DC1 12 DC2 13 DC3 14 DC4 15 NAK 16 SYN 17ETB
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // DLE to ETB

//   18 CAN 19 EM  1A SUB 1B ESC 1C FS  1D GS  1E RS  1FUS
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // CAN to US

//   20 SP  21 !   22 "   23 &   24 $   25 %   26 &   27'
     C_SP,  C_CM,  C_BC,  C_BC,  C_AL,  C_BC,  C_BC,  C_AP, // SP  to '

//   28 (   29 )   2A *   2B +   2C ,   2D -   2E .   2F /
     C_OP,  C_OP,  C_OP,  C_SG,  C_OP,  C_SG,  C_DP,  C_OP, // (   to /

//   30 0   31 1   32 2   33 3   34 4   35 5   36 6   37 7
     C_DI,  C_DI,  C_DI,  C_DI,  C_DI,  C_DI,  C_DI,  C_DI, // 0   to 7

//   38 8   39 9   3A :   3B ;   3C <   3D =   3E >   3F ?
     C_DI,  C_DI,  C_OP,  C_BC,  C_BC,  C_OP,  C_BC,  C_BC, // 8   to ?

//   40 @   41 A   42 B   43 C   44 D   45 E   46 F   47 G
     C_BC,  C_AL,  C_AL,  C_CS,  C_EX,  C_EX,  C_AL,  C_AL, // @   to G

//   48 H   49 I   4A J   4B K   4C L   4D M   4E N   4F O
     C_HL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_OL, // H   to O

//   50 P   51 Q   52 R   53 S   54 T   55 U   56 V   57 W
     C_AL,  C_EX,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL, // P   to W

//   58 X   59 Y   5A Z   5B [   5C \   5D ]   5E @   5F _
     C_HX,  C_AL,  C_AL,  C_BC,  C_BC,  C_BC,  C_BC, XC_AL, // X   to _

//   60 `   61 a   62 b   63 c   64 d   65 e   66 f   67 g
     C_BC, LC_AL, LC_AL, LC_CS, LC_EX, LC_EX, LC_AL, LC_AL, // `   to g

//   68 h   69 i   6A j   6B k   6C l   6D m   6E n   6F o
    LC_HL, LC_AL, LC_AL, LC_AL, LC_AL, LC_AL, LC_AL, LC_OL, // h   to o

//   70 p   71 q   72 r   73 s   74 t   75 u   76 v   77 w
    LC_AL, LC_EX, LC_AL, LC_AL, LC_AL, LC_AL, LC_AL, LC_AL, // p   to w

//   78 x   79 y   7A z   7B {   7C |   7D }   7E ~   7F DEL
    LC_HX, LC_AL, LC_AL, C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  // x   to DEL

//   80     81     82     83     84     85     86     87
    C_BC,  XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, // 0x80 to 0x87

//   88     89     8A     8B     8C     8D     8E     8F
    XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, // 0x88 to 0x8F

//   90     91     92     93     94     95     96     97
    XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, // 0x90 to 0x97

//   98     99     9A     9B     9C     9D     9E     9F
    XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, // 0x98 to 0x9F

//   A0     A1     A2     A3     A4     A5     A6     A7
    XC_BC, XC_BC, XC_BC, XC_BC, XC_BC, XC_BC, XC_AL, XC_AL, // 0xA0 to 0xA7

//   A8     A9     AA     AB     AC     AD     AE     AF
    XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, // 0xA8 to 0xAF

//   B0     B1     B2     B3     B4     B5     B6     B7
    XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, // 0xB0 to 0xB7

//   B8     B9     BA     BB     BC     BD     BE     BF
    XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, // 0xB8 to 0xBF

//   C0     C1     C2     C3     C4     C5     C6     C7
    XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, // 0xC0 to 0xC7

//   C8     C9     CA     CB     CC     CD     CE     CF
    XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, // 0xC8 to 0xCF

//   D0     D1     D2     D3     D4     D5     D6     D7
    XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, // 0xD0 to 0xD7

//   D8     D9     DA     DB     DC     DD     DE     DF
    XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, XC_AL, // 0xD8 to 0xDF

//   E0     E1     E2     E3     E4     E5     E6     E7
    XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, // 0xE0 to 0xE7

//   E8     E9     EA     EB     EC     ED     EE     EF
    XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, // 0xE8 to 0xEF

//   F0     F1     F2     F3     F4     F5     F6     F7
    XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, // 0xF0 to 0xF7

//   F8     F9     FA     FB     FC     FD     FE     FF
    XC_DB, XC_DB, XC_DB, XC_DB, XC_DB, C_BC,  C_BC,  C_BC   // 0xF8 to 0xFF
};

#endif

static  bool    IsDoubleByteBlank( char *ptr ) {
// Determine if character is a double-byte blank character.

    if( ( (unsigned char)*ptr == 0x81 ) && ( (unsigned char)*(ptr + 1) == 0x40 ) )
        return( TRUE );
    return( FALSE );
}


static  bool    IsDoubleByteChar( char ch ) {
// Determine if character is a double-byte character.

    if( ( 0x81 <= (unsigned char)ch ) && ( (unsigned char)ch <= 0x9f ) )
        return( TRUE );
    if( ( 0xe0 <= (unsigned char)ch ) && ( (unsigned char)ch <= 0xfc ) )
        return( TRUE );
    return( FALSE );
}


static  int     CharacterWidth( char PGM *ptr ) {
// Determine character width.

    unsigned char   ch;

    if( IsDoubleByteChar( *ptr ) ) {
        ch = (unsigned char)ptr[1];
        if( ( 0x40 <= ch ) && ( ch <= 0xfc ) ) {
            if( ch == 0x7f) return( 1 );
        }
        return( 2 );
    }
    return( 1 );
}


static  bool    IsForeign( char ch ) {
// Determine if character is a foreign character (i.e. non-ASCII).

    if( IsDoubleByteChar( ch ) ) return( TRUE );
    if( ( 0xa0 <= (unsigned char)ch ) && ( (unsigned char)ch <= 0xdf ) )
        return( TRUE );
    return( FALSE );
}


void    __UseJapaneseCharSet( void ) {
    CharSetInfo.extract_text = &ExtractText;
    CharSetInfo.is_double_byte_blank = &IsDoubleByteBlank;
    CharSetInfo.is_double_byte_char = &IsDoubleByteChar;
    CharSetInfo.character_width = &CharacterWidth;
    CharSetInfo.is_foreign = &IsForeign;
#if !defined( __RT__ )
    CharSetInfo.character_set = CharSet;
    CharSetInfo.initializer = "__init_japanese";
#endif
}
