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


#include "plusplus.h"
#include "scan.h"
#include "ctokens.h"
#include "weights.gh"

#define C_XW    ( C_WS | C_EX )

/* The following table is ASCII dependent. */


char CharSet[LCHR_MAX]  = {                              /* ASCII */

/*  00 NUL 01 SOH 02 STX 03 ETX 04 EOT 05 ENQ 06 ACK 07 BEL */
     C_EX,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, /* NUL to BEL */

/*  08 BS  09 HT  0A LF  0B VT  0C FF  0D CR  0E SO  0F SI  */
     C_BC,  C_XW,  C_EX,  C_WS,  C_WS,  C_WS,  C_BC,  C_BC, /* BS  to SI */

/*  10 DLE 11 DC1 12 DC2 13 DC3 14 DC4 15 NAK 16 SYN 17 ETB */
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, /* DLE to ETB */

/*  18 CAN 19 EM  1A SUB 1B ESC 1C FS  1D GS  1E RS  1F US  */
     C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC, /* CAN to US */

        C_WS,           /* 20 SP */
        C___,           /* 21 !  */
        C___,           /* 22 "  */
        C___,           /* 23 #  */
        C_BC,           /* 24 $  */
        C___,           /* 25 %  */
        C___,           /* 26 &  */
        C___,           /* 27 '  */

        C___,           /* 28 (  */
        C___,           /* 29 )  */
        C___,           /* 2A *  */
        C___,           /* 2B +  */
        C___,           /* 2C ,  */
        C___,           /* 2D -  */
        C___,           /* 2E .  */
        C___,           /* 2F /  */

        C_DI,           /* 30 0  */
        C_DI,           /* 31 1  */
        C_DI,           /* 32 2  */
        C_DI,           /* 33 3  */
        C_DI,           /* 34 4  */
        C_DI,           /* 35 5  */
        C_DI,           /* 36 6  */
        C_DI,           /* 37 7  */

        C_DI,           /* 38 8  */
        C_DI,           /* 39 9  */
        C___,           /* 3A :  */
        C___,           /* 3B ;  */
        C___,           /* 3C <  */
        C___,           /* 3D =  */
        C___,           /* 3E >  */
        C_EX,           /* 3F ?  */

        C_BC,           /* 40 @  */
        C_HX | C_AL,    /* 41 A  */
        C_HX | C_AL,    /* 42 B  */
        C_HX | C_AL,    /* 43 C  */
        C_HX | C_AL,    /* 44 D  */
        C_HX | C_AL,    /* 45 E  */
        C_HX | C_AL,    /* 46 F  */
        C_AL,           /* 47 G  */

/*  48 H   49 I   4A J   4B K   4C L   4D M   4E N   4F O   */
     C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL, /* H   to O */

/*  50 P   51 Q   52 R   53 S   54 T   55 U   56 V   57 W   */
     C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL, /* P   to W */

/*  58 X   59 Y   5A Z   5B [   5C \   5D ]   5E ^   5F _   */
     C_AL,  C_AL,  C_AL,  C___,  C_EX,  C___,  C___,  C_AL, /* X   to _ */

        C_BC,           /* 60 `  */
        C_HX | C_AL,    /* 61 a  */
        C_HX | C_AL,    /* 62 b  */
        C_HX | C_AL,    /* 63 c  */
        C_HX | C_AL,    /* 64 d  */
        C_HX | C_AL,    /* 65 e  */
        C_HX | C_AL,    /* 66 f  */
        C_AL,           /* 67 g  */

/*  68 h   69 i   6A j   6B k   6C l   6D m   6E n   6F o   */
     C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL, /* h   to o */

/*  70 p   71 q   72 r   73 s   74 t   75 u   76 v   77 w   */
     C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL, /* p   to w */

/*  78 x   79 y   7A z   7B {   7C |   7D }   7E ~   7F DEL */
     C_AL,  C_AL,  C_AL, C___,  C___,  C___,  C___,  C_BC,  /* x   to DEL */

/*  80     81     82     83     84     85     86     87  */
    C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  /* $80 to $87 */

/*  88     89     8A     8B     8C     8D     8E     8F  */
    C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  /* $88 to $8F */

/*  90     91     92     93     94     95     96     97  */
    C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  /* $90 to $97 */

/*  98     99     9A     9B     9C     9D     9E     9F  */
    C_AL,  C_AL,  C_AL,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  /* $98 to $9F */

/*  A0     A1     A2     A3     A4     A5     A6     A7  */
    C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  C_AL,  /* $A0 to $A7 */

/*  A8     A9     AA     AB     AC     AD     AE     AF  */
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  /* $A8 to $AF */

/*  B0     B1     B2     B3     B4     B5     B6     B7  */
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  /* $B0 to $B7 */

/*  B8     B9     BA     BB     BC     BD     BE     BF  */
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  /* $B8 to $BF */

/*  C0     C1     C2     C3     C4     C5     C6     C7  */
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  /* $C0 to $C7 */

/*  C8     C9     CA     CB     CC     CD     CE     CF  */
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  /* $C8 to $CF */

/*  D0     D1     D2     D3     D4     D5     D6     D7  */
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  /* $D0 to $D7 */

/*  D8     D9     DA     DB     DC     DD     DE     DF  */
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  /* $D8 to $DF */

/*  E0     E1     E2     E3     E4     E5     E6     E7  */
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  /* $E0 to $E7 */

/*  E8     E9     EA     EB     EC     ED     EE     EF  */
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  /* $E8 to $EF */

/*  F0     F1     F2     F3     F4     F5     F6     F7  */
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  /* $F0 to $F7 */

/*  F8     F9     FA     FB     FC     FD     FE     FF  */
    C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  C_BC,  /* $F8 to $FF */
/*  LCHR_EOF */
    C___
/*  ... LCHR_MAX no bits set for chars */
  };

char TokValue[]  = {                             /* ASCII */

        0,              /* 20 SP */
        T_EXCLAMATION,  /* 21 !  */
        0,              /* 22 "  */
        T_SHARP,        /* 23 #  */
        0,              /* 24 $  */
        T_PERCENT,      /* 25 %  */
        T_AND,          /* 26 &  */
        0,              /* 27 '  */
        T_LEFT_PAREN,   /* 28 (  */
        T_RIGHT_PAREN,  /* 29 )  */
        T_TIMES,        /* 2A *  */
        T_PLUS,         /* 2B +  */
        T_COMMA,        /* 2C ,  */
        T_MINUS,        /* 2D -  */
        T_DOT,          /* 2E .  */
        T_DIVIDE,       /* 2F /  */
        W_0,            /* 30 0  */
        W_1,            /* 31 1  */
        W_2,            /* 32 2  */
        W_3,            /* 33 3  */
        W_4,            /* 34 4  */
        W_5,            /* 35 5  */
        W_6,            /* 36 6  */
        W_7,            /* 37 7  */
        W_8,            /* 38 8  */
        W_9,            /* 39 9  */
        T_COLON,        /* 3A :  */
        T_SEMI_COLON,   /* 3B ;  */
        T_LT,           /* 3C <  */
        T_EQUAL,        /* 3D =  */
        T_GT,           /* 3E >  */
        T_QUESTION,     /* 3F ?  */
        T_ATSIGN,       /* 40 @  */

/* character weights used for hashing function */
        W_A,    /* A */
        W_B,    /* B */
        W_C,    /* C */
        W_D,    /* D */
        W_E,    /* E */
        W_F,    /* F */
        W_G,    /* G */
        W_H,    /* H */
        W_I,    /* I */
        W_J,    /* J */
        W_K,    /* K */
        W_L,    /* L */
        W_M,    /* M */
        W_N,    /* N */
        W_O,    /* O */
        W_P,    /* P */
        W_Q,    /* Q */
        W_R,    /* R */
        W_S,    /* S */
        W_T,    /* T */
        W_U,    /* U */
        W_V,    /* V */
        W_W,    /* W */
        W_X,    /* X */
        W_Y,    /* Y */
        W_Z,    /* Z */

        T_LEFT_BRACKET, /* 5B [  */
        0,              /* 5C \  */
        T_RIGHT_BRACKET,/* 5D ]  */
        T_XOR,          /* 5E ^  */
        W__,            /* 5F _  */
        0,              /* 60 `  */

/* character weights used for hashing function */
        W_a,    /* a */
        W_b,    /* b */
        W_c,    /* c */
        W_d,    /* d */
        W_e,    /* e */
        W_f,    /* f */
        W_g,    /* g */
        W_h,    /* h */
        W_i,    /* i */
        W_j,    /* j */
        W_k,    /* k */
        W_l,    /* l */
        W_m,    /* m */
        W_n,    /* n */
        W_o,    /* o */
        W_p,    /* p */
        W_q,    /* q */
        W_r,    /* r */
        W_s,    /* s */
        W_t,    /* t */
        W_u,    /* u */
        W_v,    /* v */
        W_w,    /* w */
        W_x,    /* x */
        W_y,    /* y */
        W_z,    /* z */

        T_LEFT_BRACE,   /* 7B {  */
        T_OR,           /* 7C |  */
        T_RIGHT_BRACE,  /* 7D }  */
        T_TILDE,        /* 7E ~  */
        0,              /* 7F DEL*/
        0, 0, 0, 0, 0, 0, 0, 0, /* 80-87 */
        0, 0, 0, 0, 0, 0, 0, 0, /* 88-8F */
        0, 0, 0, 0, 0, 0, 0, 0, /* 90-97 */
        0, 0, 0, 0, 0, 0, 0, 0, /* 98-9F */
        0, 0, 0, 0, 0, 0, 0, 0  /* A0-A7 */
    };

