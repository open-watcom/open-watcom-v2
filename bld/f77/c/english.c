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
* Description:  English character set support
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextvar.h"
#include "scan.h"

// Modified:    By:             Reason:
// --------     ---             -------
// 92/12/18     G. Coschi       initial implementation
// 93/12/14     Alex Brodsky    Added Q as legal exponent character

#if !defined( __RT__ )

#if _CSET == _EBCDIC

// EBCDIC character set:
// =====================

static const byte __FAR CharSet[] = {
//    00     01     02     03     04     05     06     07
     C_EL,  C_BC,  C_BC,  C_BC,  C_BC,  C_TC,  C_BC,  C_BC, // NUL to BEL
//    08     09     0A     0B     0C     0D     0E     0F
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // ?   to ?
//    10     11     12     13     14     15     16     17
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // ?   to ?
//    18     19     1A     1B     1C     1D     1E     1F
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // ?   to ?
//    20     21     22     23     24     25     26     27
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // ?   to ?
//    28     29     2A     2B     2C     2D     2E     2F
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // ?   to ?
//    30     31     32     33     34     35     36     37
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // ?   to ?
//    38     39     3A     3B     3C     3D     3E     3F
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // ?   to ?
//    40     41     42     43     44     45     46     47
     C_SP,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // SP  to ?
//    48     49     4A @   4B .   4C <   4D (   4E +   4F |
     C_BC,  C_BC,  C_BC,  C_DP,  C_BC,  C_OP,  C_SG,  C_BC, // ?   to |
//    50 &   51     52     53     54     55     56     57
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // &   to ?
//    58     59     5A !   5B 0x   5C *   5D )   5E ;   5F ~
     C_BC,  C_BC,  C_CM,  C_AL,  C_OP,  C_OP,  C_BC,  C_BC, // ?   to ~
//    60 -   61 /   62     63     64     65     66     67
     C_SG,  C_OP,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // -   to ?
//    68     69     6A |   6B ,   6C %   6D _   6E >   6F ?
     C_BC,  C_BC,  C_BC,  C_OP,  C_BC,  XC_AL, C_BC,  C_BC, // ?   to ?
//    70     71     72     73     74     75     76     77
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // ?   to ?
//    78     79 `   7A :   7B &   7C @   7D '   7E =   7F "
     C_BC,  C_BC,  C_OP,  C_BC,  C_BC,  C_AP,  C_OP,  C_BC, // ?   to "
//    80     81 a   82 b   83 c   84 d   85 e   86 f   87g
     C_BC,  LC_AL, LC_AL, LC_CS, LC_EX, LC_EX, LC_AL, LC_AL,// ?   to g
//    88 h   89 i   8A     8B     8C     8D     8E     8F
     LC_HL, LC_AL, C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // h   to ?
//    90     91 j   92  k  93 l   94 m   95 n   96 o   97 p
     C_BC,  LC_AL, LC_AL, LC_AL, LC_AL, LC_AL, LC_OL, LC_AL,// ?   to p
//    98 q   99 r   9A     9B     9C     9D     9E     9F
     LC_EX, LC_AL, C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // q   to ?
//    A0     A1 ~   A2 s   A3 t   A4 u   A5 v   A6 w   A7 x
     C_BC,  C_BC,  LC_AL, LC_AL, LC_AL, LC_AL, LC_AL, LC_HX,// ?   to x
//    A8 y   A9 z   AA     AB     AC     AD     AE     AF
     LC_AL, LC_AL, C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // y   to ?
//    B0     B1     B2     B3     B4     B5     B6     B7
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // ?   to ?
//    B8     B9     BA     BB     BC     BD     BE     BF
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // ?   to ?
//    C0 {   C1 A   C2 B   C3 C   C4 D   C5 E   C6 F   C7 G
     C_BC,  C_AL,  C_AL,  C_CS,  C_EX,  C_EX,  C_AL,  C_AL, // {   to G
//    C8 H   C9 I   CA     CB     CC     CD     CE     CF
     C_HL,  C_AL,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // H   to ?
//    D0 }   D1 J   D2 K   D3 L   D4 M   D5 N   D6 O   D7 P
     C_BC,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_OL,  C_AL, // }   to P
//    D8 Q   D9 R   DA     DB     DC     DD     DE     DF
     C_EX,  C_AL,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // Q   to ?
//    E0 \   E1     E2 S   E3 T   E4 U   E5 V   E6 W   E7X
     C_BC,  C_BC,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_HX, // \   to X
//    E8 Y   E9 Z   EA     EB     EC     ED     EE     EF
     C_AL,  C_AL,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // Y   to ?
//    F0 0   F1 1   F2 2   F3 3   F4 4   F5 5   F6 6   F7 7
     C_DI,  C_DI,  C_DI,  C_DI,  C_DI,  C_DI,  C_DI,  C_DI, // 0   to 7
//    F8 8   F9 9   FA |   FB     FC     FD     FE     FF
     C_DI,  C_DI,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC  // 8   to 'FF'
};

#else

// ASCII character set:
// ====================

static const byte __FAR CharSet[] = {

//   00 NUL 01 SOH 02 STX 03 ETX 04 EOT 05 ENQ 06 ACK 07 BEL
     C_EL,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // NUL to BEL

//   08 BS  09 HT  0A LF  0B VT  0C FF  0D CR  0E SO  0F SI
     C_BC,  C_TC,  C_BC,  C_BC,  C_SP,  C_BC,  C_BC,  C_BC, // BS  to SI

//   10 DLE 11 DC1 12 DC2 13 DC3 14 DC4 15 NAK 16 SYN 17ETB
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // DLE to ETB

//   18 CAN 19 EM  1A SUB 1B ESC 1C FS  1D GS  1E RS  1FUS
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, // CAN to US

//   20 SP  21 !   22 "   23 &   24 $   25 %   26 &   27'
     C_SP,  C_CM,  C_BC,  C_BC,  C_AL,  C_OP,  C_BC,  C_AP, // SP  to '

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
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  // 0x80 to $87

//   88     89     8A     8B     8C     8D     8E     8F
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  // 0x88 to $8F

//   90     91     92     93     94     95     96     97
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  // 0x90 to $97

//   98     99     9A     9B     9C     9D     9E     9F
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  // 0x98 to $9F

//   A0     A1     A2     A3     A4     A5     A6     A7
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  // 0xA0 to $A7

//   A8     A9     AA     AB     AC     AD     AE     AF
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  // 0xA8 to $AF

//   B0     B1     B2     B3     B4     B5     B6     B7
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  // 0xB0 to $B7

//   B8     B9     BA     BB     BC     BD     BE     BF
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  // 0xB8 to $BF

//   C0     C1     C2     C3     C4     C5     C6     C7
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  // 0xC0 to $C7

//   C8     C9     CA     CB     CC     CD     CE     CF
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  // 0xC8 to $CF

//   D0     D1     D2     D3     D4     D5     D6     D7
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  // 0xD0 to $D7

//   D8     D9     DA     DB     DC     DD     DE     DF
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  // 0xD8 to $DF

//   E0     E1     E2     E3     E4     E5     E6     E7
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  // 0xE0 to $E7

//   E8     E9     EA     EB     EC     ED     EE     EF
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  // 0xE8 to $EF

//   F0     F1     F2     F3     F4     F5     F6     F7
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  // 0xF0 to $F7

//   F8     F9     FA     FB     FC     FD     FE     FF
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC   // 0xF8 to $FF
};

#endif

#endif

static  int     ExtractText( char *string, int len ) {
// Given a string of text, extract as much text as possible up to a maximum
// of "len" bytes so that we don't split double-byte characters.

    string = string;
    return( len );
}


static  bool    IsDoubleByteBlank( char *ptr ) {
// Determine if character is a double-byte blank character.

    ptr = ptr;
    return( FALSE );
}


static  bool    IsDoubleByteChar( char ch ) {
// Determine if character is a double-byte character.

    ch = ch;
    return( FALSE );
}


static  int     CharacterWidth( char PGM *ptr ) {
// Determine character width.

    ptr = ptr;
    return( 1 );
}


static  bool    IsForeign( char ch ) {
// Determine if character is a foreign character (i.e. non-ASCII).

    ch = ch;
    return( FALSE );
}


void    __UseEnglishCharSet( void ) {
    CharSetInfo.extract_text = &ExtractText;
    CharSetInfo.is_double_byte_blank = &IsDoubleByteBlank;
    CharSetInfo.is_double_byte_char = &IsDoubleByteChar;
    CharSetInfo.character_width = &CharacterWidth;
    CharSetInfo.is_foreign = &IsForeign;
#if !defined( __RT__ )
    CharSetInfo.character_set = CharSet;
    CharSetInfo.initializer = "__init_english";
#endif
}
