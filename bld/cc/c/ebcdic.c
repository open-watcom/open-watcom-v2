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


/*
%% CHRSET    : character set - table of character types
*/
#include "scan.h"
#include "ctokens.h"

/* The following table is EBCDIC dependent. */

charset_flags CharSet[]  = {                              /* EBCDIC */

/*  00 NUL 01 SOH 02 STX 03 ETX 04 EOT 05 HT  06 ACK 07 BEL */
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_WS,  C_BC,  C_BC, /* NUL to BEL */

/*  08 BS  09 HT  0A LF  0B VT  0C FF  0D CR  0E SO  0F SI  */
     C_BC,  C_WS,  C_BC,  C_WS,  C_WS,  C_WS,  C_BC,  C_BC, /* BS  to SI */

/*  10 DLE 11 DC1 12 DC2 13 DC3 14 DC4 15 NAK 16 SYN 17 ETB */
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, /* DLE to ETB */

/*  18 CAN 19 EM  1A SUB 1B ESC 1C FS  1D GS  1E RS  1F US  */
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, /* CAN to US */

/*  20 DLE 21 DC1 22 DC2 23 DC3 24 DC4 25 NAK 26 SYN 27 ETB */
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, /* DLE to ETB */

/*  28 CAN 29 EM  2A SUB 2B ESC 2C FS  2D GS  2E RS  2F US  */
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, /* CAN to US */

/*  30 DLE 31 DC1 32 DC2 33 DC3 34 DC4 35 NAK 36 SYN 37 ETB */
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, /* DLE to ETB */

/*  38 CAN 39 EM  3A SUB 3B ESC 3C FS  3D GS  3E RS  3F US  */
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, /* CAN to US */

/*  40 SP  41     42     43     44     45     46     47     */
     C_WS,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, /* DLE to ETB */

/*  48     49     4A     4B .   4C <   4D (   4E +   4F |   */
     C_BC,  C_BC,  C_BC,  C_D1,  C_D2,  C_D2,  C_D2,  C_D2,

/*  50 &   51     52     53     54     55     56     57     */
     C_D2,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,

/*  58     59     5A !   5B $   5C *   5D )   5E ;   5F ª   */
     C_BC,  C_BC,  C_D2,  C_BC,  C_D2,  C_D1,  C_D1,  C_D2,

/*  60 -   61 /   62     63     64     65     66     67  */
    C_D2,  C_D2,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  /* $60 to $67 */

/*  68     69     6A |   6B ,   6C %   6D _   6E >   6F ? */
    C_BC,  C_BC,  C_D2,  C_D1,  C_D2,  C_AL,  C_D2,  C_D1,  /* $88 to $8F */

/*  70     71     72     73     74     75     76     77  */
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  /* $70 to $77 */

/*  78     79*/
    C_BC,  C_BC,

/*  7A :*/
    C_D2,/* (: :) support MJC */
/*  7B #   7C @   7D '   7E =   7F " */
    C_D2,  C_BC,  C_D1,  C_D2,  C_D1,               /* $7B to $7F */


        C_BC,           /* 80 `  */
        C_LH | C_AL,    /* 81 a  */
        C_LH | C_AL,    /* 82 b  */
        C_LH | C_AL,    /* 83 c  */
        C_LH | C_AL,    /* 84 d  */
        C_LH | C_AL,    /* 85 e  */
        C_LH | C_AL,    /* 86 f  */
        C_AL,           /* 87 g  */

/*  88 h   89 i   8A     8B     8C     8D     8E     8F     */
     C_AL,  C_AL,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,

/*  90     91 j   92 k   93 l   94 m   95 n   96 o   97 p   */
     C_BC,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,

/*  98 q   99 r   9A     9B     9C     9D     9E     9F     */
     C_AL,  C_AL,  C_BC, C_BC,  C_BC,  C_BC,  C_BC,  C_BC,

/*  A0     A1 ~   A2 s   A3 t   A4 u   A5 v   A6 w   A7 x */
    C_BC,  C_D1,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,

/*  A8 y   A9 z   AA     AB     AC     AD [   AE     AF  */
    C_AL,  C_AL,  C_BC,  C_BC,  C_BC,  C_D1,  C_BC,  C_BC,  /* $A8 to $AF */

/*  B0     B1     B2     B3     B4     B5     B6     B7  */
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  /* $B0 to $B7 */

/*  B8     B9     BA     BB     BC     BD ]   BE     BF  */
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_D1,  C_BC,  C_BC,  /* $B8 to $BF */

        C_D1,           /* C0 {  */
        C_UH | C_AL,    /* C1 A  */
        C_UH | C_AL,    /* C2 B  */
        C_UH | C_AL,    /* C3 C  */
        C_UH | C_AL,    /* C4 D  */
        C_UH | C_AL,    /* C5 E  */
        C_UH | C_AL,    /* C6 F  */
        C_AL,           /* C7 G  */

/*  C8 H   C9 I   CA     CB     8C     CD     CE     CF     */
     C_AL,  C_AL,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,

/*  D0 }   D1 J   D2 K   D3 L   94 M   D5 N   D6 O   D7 P   */
     C_D1,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,

/*  D8 Q   D9 R   DA     DB     9C     DD     DE     DF     */
     C_AL,  C_AL,  C_BC, C_BC,  C_BC,  C_BC,  C_BC,  C_BC,

/*  E0 \   E1     E2 S   E3 T   a4 U   E5 V   E6 W   E7 X */
    C_BC,  C_BC,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,

/*  E8 Y   E9 Z   EA     EB     AC     ED     EE     EF  */
    C_AL,  C_AL,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  /* $E8 to $EF */

/*  F0     F1     F2     F3     F4     F5     F6     F7  */
    C_DI,  C_DI,  C_DI,  C_DI,  C_DI,  C_DI,  C_DI,  C_DI,  /* $F0 to $F7 */

/*  F8     F9     FA |   FB     FC     FD     FE     FF  */
    C_DI,  C_DI,  C_D2,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC   /* $F8 to $FF */
  };

char TokValue[]  = {                             /* EBCDIC */

        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 00 - 0f */
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 10 - 1f */
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 20 - 2f */
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 30 - 3f */

        0,              /* 40 SP */
        0,              /* 41   */
        0,              /* 42   */
        0,              /* 43   */
        0,              /* 44   */
        0,              /* 45   */
        0,              /* 46   */
        0,              /* 47   */
        0,              /* 48   */
        0,              /* 49   */
        0,              /* 4A   */
        T_DOT,          /* 4B .  */
EQ|DUP| T_LT,           /* 4C <  */
        T_LEFT_PAREN,   /* 4D (  */
EQ|DUP| T_PLUS,         /* 4E +  */
EQ|DUP| T_OR,           /* 4F |  */

EQ|DUP| T_AND,          /* 50 &  */
        0,              /* 51   */
        0,              /* 52   */
        0,              /* 53   */
        0,              /* 54   */
        0,              /* 55   */
        0,              /* 56   */
        0,              /* 57   */
        0,              /* 58   */
        0,              /* 59   */
EQ|     T_EXCLAMATION,  /* 5A !  */
        0,              /* 5B $  */
EQ|     T_TIMES,        /* 5C *  */
        T_RIGHT_PAREN,  /* 5D )  */
        T_SEMI_COLON,   /* 5E ;  */
EQ|     T_XOR,          /* 5F ª  */

EQ|DUP| T_MINUS,        /* 60 -  */
EQ|     T_DIV,          /* 61 /  */
        0,              /* 62   */
        0,              /* 63   */
        0,              /* 64   */
        0,              /* 65   */
        0,              /* 66   */
        0,              /* 67   */
        0,              /* 68   */
        0,              /* 69   */
EQ|DUP| T_OR,           /* 6A |  */
        T_COMMA,        /* 6B ,  */
EQ|     T_PERCENT,      /* 6C %  */
        W__,            /* 6D _  */
EQ|DUP| T_GT,           /* 6E >  */
        T_QUESTION,     /* 6F ?  */
        0,              /* 70   */
        0,              /* 71   */
        0,              /* 72   */
        0,              /* 73   */
        0,              /* 74   */
        0,              /* 75   */
        0,              /* 76   */
        0,              /* 77   */
        0,              /* 78   */
        0,              /* 79   */
#ifdef C_PLUS_PLUS
   DUP| T_COLON,        /* 7A :  */
#else
        T_COLON,        /* 7A :  */
#endif
   DUP| T_SHARP,        /* 7B #  */
        0,              /* 7C @  */
        0,              /* 7D '  */
EQ|     T_EQUAL,        /* 7E =  */
        0,              /* 7F "  */

        0,              /* 80    */
/* character weights used for hashing function */

        W_A,            /* a */
        W_B,            /* b */
        W_C,            /* c */
        W_D,            /* d */
        W_E,            /* e */
        W_F,            /* f */
        W_G,            /* g */
        W_H,            /* h */
        W_I,            /* i */
        0,              /* 8A   */
        0,              /* 8B   */
        0,              /* 8C   */
        0,              /* 8D   */
        0,              /* 8E   */
        0,              /* 8F   */
        0,              /* 90   */
        W_J,            /* j */
        W_K,            /* k */
        W_L,            /* l */
        W_M,            /* m */
        W_N,            /* n */
        W_O,            /* o */
        W_P,            /* p */
        W_Q,            /* q */
        W_R,            /* r */
        0,              /* 9A   */
        0,              /* 9B   */
        0,              /* 9C   */
        0,              /* 9D   */
        0,              /* 9E   */
        0,              /* 9F   */
        0,              /* A0   */
        T_TILDE,        /* A1 ~  */
        W_S,            /* s */
        W_T,            /* t */
        W_U,            /* u */
        W_V,            /* v */
        W_W,            /* w */
        W_X,            /* x */
        W_Y,            /* y */
        W_Z,            /* z */
        0,              /* AA   */
        0,              /* AB   */
        0,              /* AC   */
        T_LEFT_BRACKET, /* AD [  */
        0,              /* AE   */
        0,              /* AF   */
        0,              /* B0   */
        0,              /* B1   */
        0,              /* B2   */
        0,              /* B3   */
        0,              /* B4   */
        0,              /* B5   */
        0,              /* B6   */
        0,              /* B7   */
        0,              /* B8   */
        0,              /* B9   */
        0,              /* BA   */
        0,              /* BB   */
        0,              /* BC   */
        T_RIGHT_BRACKET,/* BD ]  */
        0,              /* BE   */
        0,              /* BF   */
        T_LEFT_BRACE,   /* C0 {  */

/* character weights used for hashing function */

        W_A,            /* A */
        W_B,            /* B */
        W_C,            /* C */
        W_D,            /* D */
        W_E,            /* E */
        W_F,            /* F */
        W_G,            /* G */
        W_H,            /* H */
        W_I,            /* I */
        0,              /* CA   */
        0,              /* CB   */
        0,              /* CC   */
        0,              /* CD   */
        0,              /* CE   */
        0,              /* CF   */
        T_RIGHT_BRACE,  /* D0 }  */
        W_J,            /* J */
        W_K,            /* K */
        W_L,            /* L */
        W_M,            /* M */
        W_N,            /* N */
        W_O,            /* O */
        W_P,            /* P */
        W_Q,            /* Q */
        W_R,            /* R */
        0,              /* DA   */
        0,              /* DB   */
        0,              /* DC   */
        0,              /* DD   */
        0,              /* DE   */
        0,              /* DF   */
        0,              /* E0   */
        0,              /* E1   */
        W_S,            /* S */
        W_T,            /* T */
        W_U,            /* U */
        W_V,            /* V */
        W_W,            /* W */
        W_X,            /* X */
        W_Y,            /* Y */
        W_Z,            /* Z */
        0,              /* EA   */
        0,              /* EB   */
        0,              /* EC   */
        0,              /* ED   */
        0,              /* EE   */
        0,              /* EF   */


        26,             /* F0 0  */
        25,             /* F1 1  */
        24,             /* F2 2  */
        23,             /* F3 F  */
        22,             /* F4 4  */
        21,             /* F5 5  */
        20,             /* F6 6  */
        19,             /* F7 7  */
        18,             /* F8 8  */
        17,             /* F9 9  */
DUP|    T_OR,           /* FA |  */
        0,              /* FB    */
        0,              /* FC    */
        0,              /* FD    */
        0,              /* FE    */
        0               /* FF    */
    };

