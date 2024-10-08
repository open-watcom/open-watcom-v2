;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2024      The Open Watcom Contributors. All Rights Reserved.
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
;*               DESCRIBE IT HERE!
;*
;*****************************************************************************


    name roman

ifdef __386__
    _DATA segment word public 'DATA'
else
    RomanFont_DATA segment para 'FONT_DATA'
endif

    public __RomanFont
__RomanFont label byte

dw    0,  26,  45,  64, 143, 203, 297, 308, 345, 382, 396, 405, 419, 423
dw  433, 437, 513, 531, 616, 705, 726, 798, 890, 946,1065,1157,1178,1203
dw 1209,1218,1224,1284,1391,1420,1503,1564,1619,1656,1689,1763,1807,1826
dw 1862,1906,1929,1978,2012,2096,2148,2270,2352,2417,2444,2485,2509,2548
dw 2582,2615,2642,2661,2665,2684,2697,2706,2717,2790,2850,2903,2968,3026
dw 3065,3178,3226,3256,3301,3345,3364,3441,3489,3557,3622,3682,3722,3783
dw 3811,3859,3883,3922,3956,3994,4021,4071,4075,4125,4170,4178,4186,4194
dw 4202,4223,4244,4254,4260,4274,4287,4300,4319,4426

db   16,  28,  15,  26,  16,  14,  17,  26,  16,  28, -99,  16,  26,  16
db   20, -99,  16,   9,  15,   8,  16,   7,  17,   8,  16,   9
db   12,  28,  11,  21, -99,  13,  28,  11,  21, -99,  20,  28,  19,  21
db  -99,  21,  28,  19,  21
db   17,  28,  10,   0, -99,  23,  28,  16,   0, -99,  10,  17,  24,  17
db  -99,   9,  11,  23,  11
db   14,  32,  14,   3, -99,  18,  32,  18,   3, -99,  22,  25,  21,  24
db   22,  23,  23,  24,  23,  25,  21,  27,  18,  28,  14,  28,  11,  27
db    9,  25,   9,  23,  10,  21,  11,  20,  13,  19,  19,  17,  21,  16
db   23,  14, -99,   9,  23,  11,  21,  13,  20,  19,  18,  21,  17,  22
db   16,  23,  14,  23,  10,  21,   8,  18,   7,  14,   7,  11,   8,   9
db   10,   9,  11,  10,  12,  11,  11,  10,  10
db   25,  28,   7,   7, -99,  12,  28,  14,  26,  14,  24,  13,  22,  11
db   21,   9,  21,   7,  23,   7,  25,   8,  27,  10,  28,  12,  28,  14
db   27,  17,  26,  20,  26,  23,  27,  25,  28, -99,  21,  14,  19,  13
db   18,  11,  18,   9,  20,   7,  22,   7,  24,   8,  25,  10,  25,  12
db   23,  14,  21,  14
db   25,  20,  24,  19,  25,  18,  26,  19,  26,  20,  25,  21,  24,  21
db   23,  20,  22,  18,  20,  13,  18,  10,  16,   8,  14,   7,  11,   7
db    8,   8,   7,  10,   7,  13,   8,  15,  14,  19,  16,  21,  17,  23
db   17,  25,  16,  27,  14,  28,  12,  27,  11,  25,  11,  23,  12,  20
db   14,  17,  19,  10,  21,   8,  24,   7,  25,   7,  26,   8,  26,   9
db  -99,  11,   7,   9,   8,   8,  10,   8,  13,   9,  15,  11,  17, -99
db   11,  23,  12,  21,  20,  10,  22,   8,  24,   7
db   18,  28,  13,  22, -99,  18,  28,  19,  27,  13,  22
db   20,  32,  18,  30,  16,  27,  14,  23,  13,  18,  13,  14,  14,   9
db   16,   5,  18,   2,  20,   0, -99,  18,  30,  16,  26,  15,  23,  14
db   18,  14,  14,  15,   9,  16,   6,  18,   2
db   12,  32,  14,  30,  16,  27,  18,  23,  19,  18,  19,  14,  18,   9
db   16,   5,  14,   2,  12,   0, -99,  14,  30,  16,  26,  17,  23,  18
db   18,  18,  14,  17,   9,  16,   6,  14,   2
db   16,  28,  16,  16, -99,  11,  25,  21,  19, -99,  21,  25,  11,  19
db   16,  25,  16,   7, -99,   7,  16,  25,  16
db   16,   7,  15,   8,  16,   9,  17,   8,  17,   6,  16,   4,  15,   3
db    7,  16,  25,  16
db   16,   9,  15,   8,  16,   7,  17,   8,  16,   9
db   25,  32,   7,   0
db   15,  28,  12,  27,  10,  24,   9,  19,   9,  16,  10,  11,  12,   8
db   15,   7,  17,   7,  20,   8,  22,  11,  23,  16,  23,  19,  22,  24
db   20,  27,  17,  28,  15,  28, -99,  15,  28,  13,  27,  12,  26,  11
db   24,  10,  19,  10,  16,  11,  11,  12,   9,  13,   8,  15,   7, -99
db   17,   7,  19,   8,  20,   9,  21,  11,  22,  16,  22,  19,  21,  24
db   20,  26,  19,  27,  17,  28
db   12,  24,  14,  25,  17,  28,  17,   7, -99,  16,  27,  16,   7, -99
db   12,   7,  21,   7
db   10,  24,  11,  23,  10,  22,   9,  23,   9,  24,  10,  26,  11,  27
db   14,  28,  18,  28,  21,  27,  22,  26,  23,  24,  23,  22,  22,  20
db   19,  18,  14,  16,  12,  15,  10,  13,   9,  10,   9,   7, -99,  18
db   28,  20,  27,  21,  26,  22,  24,  22,  22,  21,  20,  18,  18,  14
db   16, -99,   9,   9,  10,  10,  12,  10,  17,   8,  20,   8,  22,   9
db   23,  10, -99,  12,  10,  17,   7,  21,   7,  22,   8,  23,  10,  23
db   12
db   10,  24,  11,  23,  10,  22,   9,  23,   9,  24,  10,  26,  11,  27
db   14,  28,  18,  28,  21,  27,  22,  25,  22,  22,  21,  20,  18,  19
db   15,  19, -99,  18,  28,  20,  27,  21,  25,  21,  22,  20,  20,  18
db   19, -99,  18,  19,  20,  18,  22,  16,  23,  14,  23,  11,  22,   9
db   21,   8,  18,   7,  14,   7,  11,   8,  10,   9,   9,  11,   9,  12
db   10,  13,  11,  12,  10,  11, -99,  21,  17,  22,  14,  22,  11,  21
db    9,  20,   8,  18,   7
db   18,  26,  18,   7, -99,  19,  28,  19,   7, -99,  19,  28,   8,  13
db   24,  13, -99,  15,   7,  22,   7
db   11,  28,   9,  18, -99,   9,  18,  11,  20,  14,  21,  17,  21,  20
db   20,  22,  18,  23,  15,  23,  13,  22,  10,  20,   8,  17,   7,  14
db    7,  11,   8,  10,   9,   9,  11,   9,  12,  10,  13,  11,  12,  10
db   11, -99,  17,  21,  19,  20,  21,  18,  22,  15,  22,  13,  21,  10
db   19,   8,  17,   7, -99,  11,  28,  21,  28, -99,  11,  27,  16,  27
db   21,  28
db   21,  25,  20,  24,  21,  23,  22,  24,  22,  25,  21,  27,  19,  28
db   16,  28,  13,  27,  11,  25,  10,  23,   9,  19,   9,  13,  10,  10
db   12,   8,  15,   7,  17,   7,  20,   8,  22,  10,  23,  13,  23,  14
db   22,  17,  20,  19,  17,  20,  16,  20,  13,  19,  11,  17,  10,  14
db  -99,  16,  28,  14,  27,  12,  25,  11,  23,  10,  19,  10,  13,  11
db   10,  13,   8,  15,   7, -99,  17,   7,  19,   8,  21,  10,  22,  13
db   22,  14,  21,  17,  19,  19,  17,  20
db    9,  28,   9,  22, -99,   9,  24,  10,  26,  12,  28,  14,  28,  19
db   25,  21,  25,  22,  26,  23,  28, -99,  10,  26,  12,  27,  14,  27
db   19,  25, -99,  23,  28,  23,  25,  22,  22,  18,  17,  17,  15,  16
db   12,  16,   7, -99,  22,  22,  17,  17,  16,  15,  15,  12,  15,   7
db   14,  28,  11,  27,  10,  25,  10,  22,  11,  20,  14,  19,  18,  19
db   21,  20,  22,  22,  22,  25,  21,  27,  18,  28,  14,  28, -99,  14
db   28,  12,  27,  11,  25,  11,  22,  12,  20,  14,  19, -99,  18,  19
db   20,  20,  21,  22,  21,  25,  20,  27,  18,  28, -99,  14,  19,  11
db   18,  10,  17,   9,  15,   9,  11,  10,   9,  11,   8,  14,   7,  18
db    7,  21,   8,  22,   9,  23,  11,  23,  15,  22,  17,  21,  18,  18
db   19, -99,  14,  19,  12,  18,  11,  17,  10,  15,  10,  11,  11,   9
db   12,   8,  14,   7, -99,  18,   7,  20,   8,  21,   9,  22,  11,  22
db   15,  21,  17,  20,  18,  18,  19
db   22,  21,  21,  18,  19,  16,  16,  15,  15,  15,  12,  16,  10,  18
db    9,  21,   9,  22,  10,  25,  12,  27,  15,  28,  17,  28,  20,  27
db   22,  25,  23,  22,  23,  16,  22,  12,  21,  10,  19,   8,  16,   7
db   13,   7,  11,   8,  10,  10,  10,  11,  11,  12,  12,  11,  11,  10
db  -99,  15,  15,  13,  16,  11,  18,  10,  21,  10,  22,  11,  25,  13
db   27,  15,  28, -99,  17,  28,  19,  27,  21,  25,  22,  22,  22,  16
db   21,  12,  20,  10,  18,   8,  16,   7
db   16,  21,  15,  20,  16,  19,  17,  20,  16,  21, -99,  16,   9,  15
db    8,  16,   7,  17,   8,  16,   9
db   16,  21,  15,  20,  16,  19,  17,  20,  16,  21, -99,  16,   7,  15
db    8,  16,   9,  17,   8,  17,   6,  16,   4,  15,   3
db   24,  25,   8,  16,  24,   7
db    7,  19,  25,  19, -99,   7,  13,  25,  13
db    8,  25,  24,  16,   8,   7
db   11,  24,  12,  23,  11,  22,  10,  23,  10,  24,  11,  26,  12,  27
db   14,  28,  17,  28,  20,  27,  21,  26,  22,  24,  22,  22,  21,  20
db   20,  19,  16,  17,  16,  14, -99,  17,  28,  19,  27,  20,  26,  21
db   24,  21,  22,  20,  20,  18,  18, -99,  16,   9,  15,   8,  16,   7
db   17,   8,  16,   9
db   21,  20,  20,  22,  18,  23,  15,  23,  13,  22,  12,  21,  11,  18
db   11,  15,  12,  13,  14,  12,  17,  12,  19,  13,  20,  15, -99,  15
db   23,  13,  21,  12,  18,  12,  15,  13,  13,  14,  12, -99,  21,  23
db   20,  15,  20,  13,  22,  12,  24,  12,  26,  14,  27,  17,  27,  19
db   26,  22,  25,  24,  23,  26,  21,  27,  18,  28,  15,  28,  12,  27
db   10,  26,   8,  24,   7,  22,   6,  19,   6,  16,   7,  13,   8,  11
db   10,   9,  12,   8,  15,   7,  18,   7,  21,   8,  23,   9,  24,  10
db  -99,  22,  23,  21,  15,  21,  13,  22,  12
db   16,  28,   9,   7, -99,  16,  28,  23,   7, -99,  16,  25,  22,   7
db  -99,  11,  13,  20,  13, -99,   7,   7,  13,   7, -99,  19,   7,  25
db    7
db   10,  28,  10,   7, -99,  11,  28,  11,   7, -99,   7,  28,  19,  28
db   22,  27,  23,  26,  24,  24,  24,  22,  23,  20,  22,  19,  19,  18
db  -99,  19,  28,  21,  27,  22,  26,  23,  24,  23,  22,  22,  20,  21
db   19,  19,  18, -99,  11,  18,  19,  18,  22,  17,  23,  16,  24,  14
db   24,  11,  23,   9,  22,   8,  19,   7,   7,   7, -99,  19,  18,  21
db   17,  22,  16,  23,  14,  23,  11,  22,   9,  21,   8,  19,   7
db   22,  25,  23,  22,  23,  28,  22,  25,  20,  27,  17,  28,  15,  28
db   12,  27,  10,  25,   9,  23,   8,  20,   8,  15,   9,  12,  10,  10
db   12,   8,  15,   7,  17,   7,  20,   8,  22,  10,  23,  12, -99,  15
db   28,  13,  27,  11,  25,  10,  23,   9,  20,   9,  15,  10,  12,  11
db   10,  13,   8,  15,   7
db   10,  28,  10,   7, -99,  11,  28,  11,   7, -99,   7,  28,  17,  28
db   20,  27,  22,  25,  23,  23,  24,  20,  24,  15,  23,  12,  22,  10
db   20,   8,  17,   7,   7,   7, -99,  17,  28,  19,  27,  21,  25,  22
db   23,  23,  20,  23,  15,  22,  12,  21,  10,  19,   8,  17,   7
db   10,  28,  10,   7, -99,  11,  28,  11,   7, -99,  17,  22,  17,  14
db  -99,   7,  28,  23,  28,  23,  22,  22,  28, -99,  11,  18,  17,  18
db  -99,   7,   7,  23,   7,  23,  13,  22,   7
db   10,  28,  10,   7, -99,  11,  28,  11,   7, -99,  17,  22,  17,  14
db  -99,   7,  28,  23,  28,  23,  22,  22,  28, -99,  11,  18,  17,  18
db  -99,   7,   7,  14,   7
db   22,  25,  23,  22,  23,  28,  22,  25,  20,  27,  17,  28,  15,  28
db   12,  27,  10,  25,   9,  23,   8,  20,   8,  15,   9,  12,  10,  10
db   12,   8,  15,   7,  17,   7,  20,   8,  22,  10, -99,  15,  28,  13
db   27,  11,  25,  10,  23,   9,  20,   9,  15,  10,  12,  11,  10,  13
db    8,  15,   7, -99,  22,  15,  22,   7, -99,  23,  15,  23,   7, -99
db   19,  15,  26,  15
db    9,  28,   9,   7, -99,  10,  28,  10,   7, -99,  22,  28,  22,   7
db  -99,  23,  28,  23,   7, -99,   6,  28,  13,  28, -99,  19,  28,  26
db   28, -99,  10,  18,  22,  18, -99,   6,   7,  13,   7, -99,  19,   7
db   26,   7
db   16,  28,  16,   7, -99,  17,  28,  17,   7, -99,  13,  28,  20,  28
db  -99,  13,   7,  20,   7
db   19,  28,  19,  11,  18,   8,  16,   7,  14,   7,  12,   8,  11,  10
db   11,  12,  12,  13,  13,  12,  12,  11, -99,  18,  28,  18,  11,  17
db    8,  16,   7, -99,  15,  28,  22,  28
db    9,  28,   9,   7, -99,  10,  28,  10,   7, -99,  23,  28,  10,  15
db  -99,  15,  19,  23,   7, -99,  14,  19,  22,   7, -99,   6,  28,  13
db   28, -99,  19,  28,  25,  28, -99,   6,   7,  13,   7, -99,  19,   7
db   25,   7
db   12,  28,  12,   7, -99,  13,  28,  13,   7, -99,   9,  28,  16,  28
db  -99,   9,   7,  24,   7,  24,  13,  23,   7
db    9,  28,   9,   7, -99,  10,  28,  16,  10, -99,   9,  28,  16,   7
db  -99,  23,  28,  16,   7, -99,  23,  28,  23,   7, -99,  24,  28,  24
db    7, -99,   6,  28,  10,  28, -99,  23,  28,  27,  28, -99,   6,   7
db   12,   7, -99,  20,   7,  27,   7
db   10,  28,  10,   7, -99,  11,  28,  23,   9, -99,  11,  26,  23,   7
db  -99,  23,  28,  23,   7, -99,   7,  28,  11,  28, -99,  20,  28,  26
db   28, -99,   7,   7,  13,   7
db   15,  28,  12,  27,  10,  25,   9,  23,   8,  19,   8,  16,   9,  12
db   10,  10,  12,   8,  15,   7,  17,   7,  20,   8,  22,  10,  23,  12
db   24,  16,  24,  19,  23,  23,  22,  25,  20,  27,  17,  28,  15,  28
db  -99,  15,  28,  13,  27,  11,  25,  10,  23,   9,  19,   9,  16,  10
db   12,  11,  10,  13,   8,  15,   7, -99,  17,   7,  19,   8,  21,  10
db   22,  12,  23,  16,  23,  19,  22,  23,  21,  25,  19,  27,  17,  28
db   10,  28,  10,   7, -99,  11,  28,  11,   7, -99,   7,  28,  19,  28
db   22,  27,  23,  26,  24,  24,  24,  21,  23,  19,  22,  18,  19,  17
db   11,  17, -99,  19,  28,  21,  27,  22,  26,  23,  24,  23,  21,  22
db   19,  21,  18,  19,  17, -99,   7,   7,  14,   7
db   15,  28,  12,  27,  10,  25,   9,  23,   8,  19,   8,  16,   9,  12
db   10,  10,  12,   8,  15,   7,  17,   7,  20,   8,  22,  10,  23,  12
db   24,  16,  24,  19,  23,  23,  22,  25,  20,  27,  17,  28,  15,  28
db  -99,  15,  28,  13,  27,  11,  25,  10,  23,   9,  19,   9,  16,  10
db   12,  11,  10,  13,   8,  15,   7, -99,  17,   7,  19,   8,  21,  10
db   22,  12,  23,  16,  23,  19,  22,  23,  21,  25,  19,  27,  17,  28
db  -99,  12,   9,  12,  10,  13,  12,  15,  13,  16,  13,  18,  12,  19
db   10,  20,   3,  21,   2,  23,   2,  24,   4,  24,   5, -99,  19,  10
db   20,   6,  21,   4,  22,   3,  23,   3,  24,   4
db   10,  28,  10,   7, -99,  11,  28,  11,   7, -99,   7,  28,  19,  28
db   22,  27,  23,  26,  24,  24,  24,  22,  23,  20,  22,  19,  19,  18
db   11,  18, -99,  19,  28,  21,  27,  22,  26,  23,  24,  23,  22,  22
db   20,  21,  19,  19,  18, -99,   7,   7,  14,   7, -99,  16,  18,  18
db   17,  19,  16,  22,   9,  23,   8,  24,   8,  25,   9, -99,  18,  17
db   19,  15,  21,   8,  22,   7,  24,   7,  25,   9,  25,  10
db   22,  25,  23,  28,  23,  22,  22,  25,  20,  27,  17,  28,  14,  28
db   11,  27,   9,  25,   9,  23,  10,  21,  11,  20,  13,  19,  19,  17
db   21,  16,  23,  14, -99,   9,  23,  11,  21,  13,  20,  19,  18,  21
db   17,  22,  16,  23,  14,  23,  10,  21,   8,  18,   7,  15,   7,  12
db    8,  10,  10,   9,  13,   9,   7,  10,  10
db   16,  28,  16,   7, -99,  17,  28,  17,   7, -99,  10,  28,   9,  22
db    9,  28,  24,  28,  24,  22,  23,  28, -99,  13,   7,  20,   7
db    9,  28,   9,  13,  10,  10,  12,   8,  15,   7,  17,   7,  20,   8
db   22,  10,  23,  13,  23,  28, -99,  10,  28,  10,  13,  11,  10,  13
db    8,  15,   7, -99,   6,  28,  13,  28, -99,  20,  28,  26,  28
db    9,  28,  16,   7, -99,  10,  28,  16,  10, -99,  23,  28,  16,   7
db  -99,   7,  28,  13,  28, -99,  19,  28,  25,  28
db    8,  28,  12,   7, -99,   9,  28,  12,  12, -99,  16,  28,  12,   7
db  -99,  16,  28,  20,   7, -99,  17,  28,  20,  12, -99,  24,  28,  20
db    7, -99,   5,  28,  12,  28, -99,  21,  28,  27,  28
db    9,  28,  22,   7, -99,  10,  28,  23,   7, -99,  23,  28,   9,   7
db  -99,   7,  28,  13,  28, -99,  19,  28,  25,  28, -99,   7,   7,  13
db    7, -99,  19,   7,  25,   7
db    9,  28,  16,  17,  16,   7, -99,  10,  28,  17,  17,  17,   7, -99
db   24,  28,  17,  17, -99,   7,  28,  13,  28, -99,  20,  28,  26,  28
db  -99,  13,   7,  20,   7
db   22,  28,   9,   7, -99,  23,  28,  10,   7, -99,  10,  28,   9,  22
db    9,  28,  23,  28, -99,   9,   7,  23,   7,  23,  13,  22,   7
db   13,  32,  13,   0, -99,  14,  32,  14,   0, -99,  13,  32,  20,  32
db  -99,  13,   0,  20,   0
db    2,  30,  30,   2
db   18,  32,  18,   0, -99,  19,  32,  19,   0, -99,  12,  32,  19,  32
db  -99,  12,   0,  19,   0
db    8,  14,  16,  19,  24,  14, -99,   8,  14,  16,  18,  24,  14
db    1,   7,  31,   7, -99,   1,   6,  31,   6
db   14,  28,  19,  22, -99,  14,  28,  13,  27,  19,  22
db   12,  19,  12,  18,  11,  18,  11,  19,  12,  20,  14,  21,  18,  21
db   20,  20,  21,  19,  22,  17,  22,  10,  23,   8,  24,   7, -99,  21
db   19,  21,  10,  22,   8,  24,   7,  25,   7, -99,  21,  17,  20,  16
db   14,  15,  11,  14,  10,  12,  10,  10,  11,   8,  14,   7,  17,   7
db   19,   8,  21,  10, -99,  14,  15,  12,  14,  11,  12,  11,  10,  12
db    8,  14,   7
db   10,  28,  10,   7, -99,  11,  28,  11,   7, -99,  11,  18,  13,  20
db   15,  21,  17,  21,  20,  20,  22,  18,  23,  15,  23,  13,  22,  10
db   20,   8,  17,   7,  15,   7,  13,   8,  11,  10, -99,  17,  21,  19
db   20,  21,  18,  22,  15,  22,  13,  21,  10,  19,   8,  17,   7, -99
db    7,  28,  11,  28
db   21,  18,  20,  17,  21,  16,  22,  17,  22,  18,  20,  20,  18,  21
db   15,  21,  12,  20,  10,  18,   9,  15,   9,  13,  10,  10,  12,   8
db   15,   7,  17,   7,  20,   8,  22,  10, -99,  15,  21,  13,  20,  11
db   18,  10,  15,  10,  13,  11,  10,  13,   8,  15,   7
db   21,  28,  21,   7, -99,  22,  28,  22,   7, -99,  21,  18,  19,  20
db   17,  21,  15,  21,  12,  20,  10,  18,   9,  15,   9,  13,  10,  10
db   12,   8,  15,   7,  17,   7,  19,   8,  21,  10, -99,  15,  21,  13
db   20,  11,  18,  10,  15,  10,  13,  11,  10,  13,   8,  15,   7, -99
db   18,  28,  22,  28, -99,  21,   7,  25,   7
db   10,  15,  22,  15,  22,  17,  21,  19,  20,  20,  18,  21,  15,  21
db   12,  20,  10,  18,   9,  15,   9,  13,  10,  10,  12,   8,  15,   7
db   17,   7,  20,   8,  22,  10, -99,  21,  15,  21,  18,  20,  20, -99
db   15,  21,  13,  20,  11,  18,  10,  15,  10,  13,  11,  10,  13,   8
db   15,   7
db   19,  27,  18,  26,  19,  25,  20,  26,  20,  27,  19,  28,  17,  28
db   15,  27,  14,  25,  14,   7, -99,  17,  28,  16,  27,  15,  25,  15
db    7, -99,  11,  21,  19,  21, -99,  11,   7,  18,   7
db   15,  21,  13,  20,  12,  19,  11,  17,  11,  15,  12,  13,  13,  12
db   15,  11,  17,  11,  19,  12,  20,  13,  21,  15,  21,  17,  20,  19
db   19,  20,  17,  21,  15,  21, -99,  13,  20,  12,  18,  12,  14,  13
db   12, -99,  19,  12,  20,  14,  20,  18,  19,  20, -99,  20,  19,  21
db   20,  23,  21,  23,  20,  21,  20, -99,  12,  13,  11,  12,  10,  10
db   10,   9,  11,   7,  14,   6,  19,   6,  22,   5,  23,   4, -99,  10
db    9,  11,   8,  14,   7,  19,   7,  22,   6,  23,   4,  23,   3,  22
db    1,  19,   0,  13,   0,  10,   1,   9,   3,   9,   4,  10,   6,  13
db    7
db   10,  28,  10,   7, -99,  11,  28,  11,   7, -99,  11,  18,  13,  20
db   16,  21,  18,  21,  21,  20,  22,  18,  22,   7, -99,  18,  21,  20
db   20,  21,  18,  21,   7, -99,   7,  28,  11,  28, -99,   7,   7,  14
db    7, -99,  18,   7,  25,   7
db   16,  28,  15,  27,  16,  26,  17,  27,  16,  28, -99,  16,  21,  16
db    7, -99,  17,  21,  17,   7, -99,  13,  21,  17,  21, -99,  13,   7
db   20,   7
db   17,  28,  16,  27,  17,  26,  18,  27,  17,  28, -99,  18,  21,  18
db    3,  17,   1,  15,   0,  13,   0,  12,   1,  12,   2,  13,   3,  14
db    2,  13,   1, -99,  17,  21,  17,   3,  16,   1,  15,   0, -99,  14
db   21,  18,  21
db   10,  28,  10,   7, -99,  11,  28,  11,   7, -99,  21,  21,  11,  11
db  -99,  16,  15,  22,   7, -99,  15,  15,  21,   7, -99,   7,  28,  11
db   28, -99,  18,  21,  24,  21, -99,   7,   7,  14,   7, -99,  18,   7
db   24,   7
db   16,  28,  16,   7, -99,  17,  28,  17,   7, -99,  13,  28,  17,  28
db  -99,  13,   7,  20,   7
db    5,  21,   5,   7, -99,   6,  21,   6,   7, -99,   6,  18,   8,  20
db   11,  21,  13,  21,  16,  20,  17,  18,  17,   7, -99,  13,  21,  15
db   20,  16,  18,  16,   7, -99,  17,  18,  19,  20,  22,  21,  24,  21
db   27,  20,  28,  18,  28,   7, -99,  24,  21,  26,  20,  27,  18,  27
db    7, -99,   2,  21,   6,  21, -99,   2,   7,   9,   7, -99,  13,   7
db   20,   7, -99,  24,   7,  31,   7
db   10,  21,  10,   7, -99,  11,  21,  11,   7, -99,  11,  18,  13,  20
db   16,  21,  18,  21,  21,  20,  22,  18,  22,   7, -99,  18,  21,  20
db   20,  21,  18,  21,   7, -99,   7,  21,  11,  21, -99,   7,   7,  14
db    7, -99,  18,   7,  25,   7
db   15,  21,  12,  20,  10,  18,   9,  15,   9,  13,  10,  10,  12,   8
db   15,   7,  17,   7,  20,   8,  22,  10,  23,  13,  23,  15,  22,  18
db   20,  20,  17,  21,  15,  21, -99,  15,  21,  13,  20,  11,  18,  10
db   15,  10,  13,  11,  10,  13,   8,  15,   7, -99,  17,   7,  19,   8
db   21,  10,  22,  13,  22,  15,  21,  18,  19,  20,  17,  21
db   10,  21,  10,   0, -99,  11,  21,  11,   0, -99,  11,  18,  13,  20
db   15,  21,  17,  21,  20,  20,  22,  18,  23,  15,  23,  13,  22,  10
db   20,   8,  17,   7,  15,   7,  13,   8,  11,  10, -99,  17,  21,  19
db   20,  21,  18,  22,  15,  22,  13,  21,  10,  19,   8,  17,   7, -99
db    7,  21,  11,  21, -99,   7,   0,  14,   0
db   21,  21,  21,   0, -99,  22,  21,  22,   0, -99,  21,  18,  19,  20
db   17,  21,  15,  21,  12,  20,  10,  18,   9,  15,   9,  13,  10,  10
db   12,   8,  15,   7,  17,   7,  19,   8,  21,  10, -99,  15,  21,  13
db   20,  11,  18,  10,  15,  10,  13,  11,  10,  13,   8,  15,   7, -99
db   18,   0,  25,   0
db   12,  21,  12,   7, -99,  13,  21,  13,   7, -99,  13,  15,  14,  18
db   16,  20,  18,  21,  21,  21,  22,  20,  22,  19,  21,  18,  20,  19
db   21,  20, -99,   9,  21,  13,  21, -99,   9,   7,  16,   7
db   21,  19,  22,  21,  22,  17,  21,  19,  20,  20,  18,  21,  14,  21
db   12,  20,  11,  19,  11,  17,  12,  16,  14,  15,  19,  13,  21,  12
db   22,  11, -99,  11,  18,  12,  17,  14,  16,  19,  14,  21,  13,  22
db   12,  22,   9,  21,   8,  19,   7,  15,   7,  13,   8,  12,   9,  11
db   11,  11,   7,  12,   9
db   14,  28,  14,  11,  15,   8,  17,   7,  19,   7,  21,   8,  22,  10
db  -99,  15,  28,  15,  11,  16,   8,  17,   7, -99,  11,  21,  19,  21
db   10,  21,  10,  10,  11,   8,  14,   7,  16,   7,  19,   8,  21,  10
db  -99,  11,  21,  11,  10,  12,   8,  14,   7, -99,  21,  21,  21,   7
db  -99,  22,  21,  22,   7, -99,   7,  21,  11,  21, -99,  18,  21,  22
db   21, -99,  21,   7,  25,   7
db   10,  21,  16,   7, -99,  11,  21,  16,   9, -99,  22,  21,  16,   7
db  -99,   8,  21,  14,  21, -99,  18,  21,  24,  21
db    8,  21,  12,   7, -99,   9,  21,  12,  10, -99,  16,  21,  12,   7
db  -99,  16,  21,  20,   7, -99,  17,  21,  20,  10, -99,  24,  21,  20
db    7, -99,   5,  21,  12,  21, -99,  21,  21,  27,  21
db   10,  21,  21,   7, -99,  11,  21,  22,   7, -99,  22,  21,  10,   7
db  -99,   8,  21,  14,  21, -99,  18,  21,  24,  21, -99,   8,   7,  14
db    7, -99,  18,   7,  24,   7
db   10,  21,  16,   7, -99,  11,  21,  16,   9, -99,  22,  21,  16,   7
db   14,   3,  12,   1,  10,   0,   9,   0,   8,   1,   9,   2,  10,   1
db  -99,   8,  21,  14,  21, -99,  18,  21,  24,  21
db   21,  21,  10,   7, -99,  22,  21,  11,   7, -99,  11,  21,  10,  17
db   10,  21,  22,  21, -99,  10,   7,  22,   7,  22,  11,  21,   7
db   18,  32,  15,  29,  14,  26,  14,  24,  15,  21,  18,  18, -99,  16
db   30,  15,  27,  15,  23,  16,  20, -99,  18,  18,  15,  16,  18,  14
db  -99,  18,  14,  15,  11,  14,   8,  14,   6,  15,   3,  18,   0, -99
db   16,  12,  15,   9,  15,   5,  16,   2
db   16,  32,  16,   0
db   14,  32,  17,  29,  18,  26,  18,  24,  17,  21,  14,  18, -99,  16
db   30,  17,  27,  17,  23,  16,  20, -99,  14,  18,  17,  16,  14,  14
db  -99,  14,  14,  17,  11,  18,   8,  18,   6,  17,   3,  14,   0, -99
db   16,  12,  17,   9,  17,   5,  16,   2
db    7,  13,   7,  15,   8,  18,  10,  19,  12,  19,  14,  18,  18,  15
db   20,  14,  22,  14,  24,  15,  25,  17, -99,   7,  15,   8,  17,  10
db   18,  12,  18,  14,  17,  18,  14,  20,  13,  22,  13,  24,  14,  25
db   17,  25,  19
db   13,  24,  18,  27,  19,  26,  13,  24
db   12,  30,  19,  32,  20,  32,  12,  30
db   19,  24,  14,  27,  13,  26,  19,  24
db   20,  30,  13,  32,  12,  32,  20,  30
db   12,  26,  13,  25,  12,  24,  11,  25,  12,  26, -99,  20,  26,  21
db   25,  20,  24,  19,  25,  20,  26
db   12,  31,  13,  30,  12,  29,  11,  30,  12,  31, -99,  20,  31,  21
db   30,  20,  29,  19,  30,  20,  31
db   11,  24,  16,  26,  21,  24,  16,  27,  11,  24
db   11,  30,  16,  32,  21,  30
db   18,   3,  17,   4,  18,   5,  19,   4,  18,   3,  15,   2,  13,   3
db   24,  23,  12,  16,  24,   9, -99,  18,  23,   6,  16,  18,   9
db    8,  23,  20,  16,   8,   9, -99,  14,  23,  26,  16,  14,   9
db   16,  21,  16,   7, -99,  17,  21,  17,   7, -99,  13,  21,  17,  21
db  -99,  13,   7,  20,   7
db   18,  28,  15,  27,  13,  25,  11,  21,  10,  18,   9,  14,   8,   8
db    7,   0, -99,  18,  28,  16,  27,  14,  25,  12,  21,  11,  18,  10
db   14,   9,   8,   8,   0, -99,  18,  28,  20,  28,  22,  27,  23,  26
db   23,  23,  22,  21,  21,  20,  18,  19,  14,  19, -99,  20,  28,  22
db   26,  22,  23,  21,  21,  20,  20,  18,  19, -99,  14,  19,  18,  18
db   20,  16,  21,  14,  21,  11,  20,   9,  19,   8,  16,   7,  14,   7
db   12,   8,  11,   9,  10,  12, -99,  14,  19,  17,  18,  19,  16,  20
db   14,  20,  11,  19,   9,  18,   8,  16,   7

ifdef __386__
    _DATA ends
else
    RomanFont_DATA ends
endif

    end
