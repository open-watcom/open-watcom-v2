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


#include "variety.h"
#include "hanzentb.h"


const unsigned short __HanZen1[] = {
    0x8140,       /* 20 = sp */
    0x8149,       /* 21 = ! */
    0x8168,       /* 22 = " */
    0x8194,       /* 23 = # */
    0x8190,       /* 24 = $ */
    0x8193,       /* 25 = % */
    0x8195,       /* 26 = & */
    0x8166,       /* 27 = ' */
    0x8169,       /* 28 = ( */
    0x816a,       /* 29 = ) */
    0x8196,       /* 2A = * */
    0x817b,       /* 2B = + */
    0x8143,       /* 2C = , */
    0x817c,       /* 2D = - */
    0x8144,       /* 2E = . */
    0x815e,       /* 2F = / */
    0x824f,       /* 30 = 0 */
    0x8250,       /* 31 = 1 */
    0x8251,       /* 32 = 2 */
    0x8252,       /* 33 = 3 */
    0x8253,       /* 34 = 4 */
    0x8254,       /* 35 = 5 */
    0x8255,       /* 36 = 6 */
    0x8256,       /* 37 = 7 */
    0x8257,       /* 38 = 8 */
    0x8258,       /* 39 = 9 */
    0x8146,       /* 3A = : */
    0x8147,       /* 3B = ; */
    0x8183,       /* 3C = < */
    0x8181,       /* 3D = = */
    0x8184,       /* 3E = > */
    0x8148,       /* 3F = ? */
    0x8197,       /* 40 = @ */
    0x8260,       /* 41 = A */
    0x8261,       /* 42 = B */
    0x8262,       /* 43 = C */
    0x8263,       /* 44 = D */
    0x8264,       /* 45 = E */
    0x8265,       /* 46 = F */
    0x8266,       /* 47 = G */
    0x8267,       /* 48 = H */
    0x8268,       /* 49 = I */
    0x8269,       /* 4A = J */
    0x826a,       /* 4B = K */
    0x826b,       /* 4C = L */
    0x826c,       /* 4D = M */
    0x826d,       /* 4E = N */
    0x826e,       /* 4F = O */
    0x826f,       /* 50 = P */
    0x8270,       /* 51 = Q */
    0x8271,       /* 52 = R */
    0x8272,       /* 53 = S */
    0x8273,       /* 54 = T */
    0x8274,       /* 55 = U */
    0x8275,       /* 56 = V */
    0x8276,       /* 57 = W */
    0x8277,       /* 58 = X */
    0x8278,       /* 59 = Y */
    0x8279,       /* 5A = Z */
    0x816d,       /* 5B = [ */
    0x818f,       /* 5C = \\ */
    0x816e,       /* 5D = ] */
    0x814f,       /* 5E = ^ */
    0x8151,       /* 5F = _ */
    0x8165,       /* 60 = ` */
    0x8281,       /* 61 = a */
    0x8282,       /* 62 = b */
    0x8283,       /* 63 = c */
    0x8284,       /* 64 = d */
    0x8285,       /* 65 = e */
    0x8286,       /* 66 = f */
    0x8287,       /* 67 = g */
    0x8288,       /* 68 = h */
    0x8289,       /* 69 = i */
    0x828a,       /* 6A = j */
    0x828b,       /* 6B = k */
    0x828c,       /* 6C = l */
    0x828d,       /* 6D = m */
    0x828e,       /* 6E = n */
    0x828f,       /* 6F = o */
    0x8290,       /* 70 = p */
    0x8291,       /* 71 = q */
    0x8292,       /* 72 = r */
    0x8293,       /* 73 = s */
    0x8294,       /* 74 = t */
    0x8295,       /* 75 = u */
    0x8296,       /* 76 = v */
    0x8297,       /* 77 = w */
    0x8298,       /* 78 = x */
    0x8299,       /* 79 = y */
    0x829a,       /* 7A = z */
    0x816f,       /* 7B = { */
    0x8162,       /* 7C = | */
    0x8170,       /* 7D = } */
    0x8150        /* 7E = ~ */
};


const unsigned short __HanZen2[] = {
    0x8142,
    0x8175,
    0x8176,
    0x8141,
    0x8145,
    0x8392,
    0x8340,
    0x8342,
    0x8344,
    0x8346,
    0x8348,
    0x8383,
    0x8385,
    0x8387,
    0x8362,
    0x815B,
    0x8341,
    0x8343,
    0x8345,
    0x8347,
    0x8349,
    0x834A,
    0x834C,
    0x834E,
    0x8350,
    0x8352,
    0x8354,
    0x8356,
    0x8358,
    0x835A,
    0x835C,
    0x835E,
    0x8360,
    0x8363,
    0x8365,
    0x8367,
    0x8369,
    0x836A,
    0x836B,
    0x836C,
    0x836D,
    0x836E,
    0x8371,
    0x8374,
    0x8377,
    0x837A,
    0x837D,
    0x837E,
    0x8380,
    0x8381,
    0x8382,
    0x8384,
    0x8386,
    0x8388,
    0x8389,
    0x838A,
    0x838B,
    0x838C,
    0x838D,
    0x838F,
    0x8393,
    0x814A,
    0x814B
};
