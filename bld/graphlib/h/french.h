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


extern short            GetCodePage( void );

#pragma aux             GetCodePage = \
                            0xb8 0x01 0x66    /* mov ax,6601h */ \
                            0xcd 0x21         /* int 21 */ \
                            parm caller value [bx] modify [dx];


// List of supported multi-lingual characters. Not all codepages
// support all of these characters.

enum {
    a_AIGU,
    A_AIGU,
    e_AIGU,
    E_AIGU,
    i_AIGU,
    I_AIGU,
    o_AIGU,
    O_AIGU,
    u_AIGU,
    U_AIGU,
    y_AIGU,
    Y_AIGU,
    a_GRAV,
    A_GRAV,
    e_GRAV,
    E_GRAV,
    i_GRAV,
    I_GRAV,
    o_GRAV,
    O_GRAV,
    u_GRAV,
    U_GRAV,
    a_UMLT,
    A_UMLT,
    e_UMLT,
    E_UMLT,
    i_UMLT,
    I_UMLT,
    o_UMLT,
    O_UMLT,
    u_UMLT,
    U_UMLT,
    y_UMLT,
    a_CIRC,
    A_CIRC,
    e_CIRC,
    E_CIRC,
    i_CIRC,
    I_CIRC,
    o_CIRC,
    O_CIRC,
    u_CIRC,
    U_CIRC,
    c_CEDL,
    C_CEDL,
    L_ARRO,
    R_ARRO,
    b_BETA
};

#define NUM_LANG_CHARS  ( b_BETA + 1 )


// The regular character set ends at 0x7e.
// Special accent characters come after that point.

#define LC_AIGU         0x7f
#define UC_AIGU         0x80
#define LC_GRAVE        0x81
#define UC_GRAVE        0x82
#define LC_UMLAUT       0x83
#define UC_UMLAUT       0x84
#define LC_CIRC         0x85
#define UC_CIRC         0x86
#define CEDILLE         0x87
#define LFT_DBL_ARROW   0x88
#define RGT_DBL_ARROW   0x89
#define UNDOTTED_I      0x8a
#define BETA            0x8b
