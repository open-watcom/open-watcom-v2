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



#if !defined(pick)
#define pick( e, x0, x1, x2, x3, x4 )   X86T_##e,
#define defining_enums
enum {
#endif
    /* integers */
    pick( BYTE,         BYTE,           1, L1, L1, U1 )
    pick( WORD,         WORD,           1, L1, L1, U2 )
    pick( DWORD,        DWORD,          1, L3, L1, U4 )
    pick( QWORD,        QWORD,          1, LX, L1, U8 )
    pick( SBYTE,        NIL,            1, LX, LX, I1 )
    pick( SWORD,        NIL,            1, LX, LX, I2 )
    pick( SDWORD,       NIL,            1, LX, LX, I4 )
    pick( SQWORD,       NIL,            1, LX, LX, I8 )
    pick( CHAR,         CHAR,           0, L1, L1, I1 )
    pick( SHORT,        SHORT,          0, L1, L1, I2 )
    pick( LONG,         LONG,           0, L3, L1, I4 )
    pick( I64,          INT64,          0, LX, L1, I8 )
    pick( UCHAR,        UCHAR,          0, L1, L1, U1 )
    pick( USHORT,       USHORT,         0, L1, L1, U2 )
    pick( ULONG,        ULONG,          0, L3, L1, U4 )
    pick( U64,          UNSIGNED64,     0, LX, L1, U8 )
    pick( U128,         NIL,            0, LX, LX, U16 )
    pick( BIT,          NIL,            0, LX, LX, BIT )
    /* addresses */
    pick( N16_PTR,      N16_PTR,        1, LX, L1, N16 )
    pick( F16_PTR,      F16_PTR,        1, LX, L1, F16 )
    pick( N32_PTR,      N32_PTR,        1, LX, L3, N32 )
    pick( F32_PTR,      F32_PTR,        1, LX, L3, F32 )
    /* floats */
    pick( FLOAT,        FLOAT,          0, LX, L1, F4 )
    pick( DOUBLE,       DOUBLE,         0, LX, L1, F8 )
    pick( EXTENDED,     EXTENDED,       0, LX, L1, F10 )
    pick( HEXTENDED,    NIL,            1, LX, LX, F10 )
    pick( F10EMPTY,     NIL,            0, LX, LX, F10EMPTY )
    pick( F10SPECIAL,   NIL,            0, LX, LX, F10SPECIAL )
    /* customs */
    pick( IRET16,       NIL,            0, LX, LX, IRET16 )
    pick( IRET32,       NIL,            0, LX, LX, IRET32 )
    pick( POPA,         NIL,            0, LX, LX, POPA )
    pick( POPAD,        NIL,            0, LX, LX, POPAD )
    pick( UNKNOWN,      NIL,            0, LX, LX, BIT )
    pick( PC,           NIL,            0, LX, LX, BIT )
    pick( RC,           NIL,            0, LX, LX, BIT )
    pick( IC,           NIL,            0, LX, LX, BIT )
    pick( TAG,          NIL,            0, LX, LX, TAG )
    pick( FPPTR_REAL,   NIL,            0, LX, LX, FPPTR_REAL )
    pick( FPPTR_16,     NIL,            0, LX, LX, FPPTR_16 )
    pick( FPPTR_32,     NIL,            0, LX, LX, FPPTR_32 )
    pick( MMX_TITLE0,   NIL,            0, LX, LX, MMX_TITLE )
    pick( MMX_TITLE1,   NIL,            0, LX, LX, MMX_TITLE )
    pick( MMX_TITLE2,   NIL,            0, LX, LX, MMX_TITLE )
    pick( MMX_TITLE3,   NIL,            0, LX, LX, MMX_TITLE )
    pick( MMX_TITLE4,   NIL,            0, LX, LX, MMX_TITLE )
    pick( MMX_TITLE5,   NIL,            0, LX, LX, MMX_TITLE )
    pick( MMX_TITLE6,   NIL,            0, LX, LX, MMX_TITLE )
    pick( MMX_TITLE7,   NIL,            0, LX, LX, MMX_TITLE )
    pick( XMM_TITLE0,   NIL,            0, LX, LX, XMM_TITLE )
    pick( XMM_TITLE1,   NIL,            0, LX, LX, XMM_TITLE )
    pick( XMM_TITLE2,   NIL,            0, LX, LX, XMM_TITLE )
    pick( XMM_TITLE3,   NIL,            0, LX, LX, XMM_TITLE )
    pick( XMM_TITLE4,   NIL,            0, LX, LX, XMM_TITLE )
    pick( XMM_TITLE5,   NIL,            0, LX, LX, XMM_TITLE )
    pick( XMM_TITLE6,   NIL,            0, LX, LX, XMM_TITLE )
    pick( XMM_TITLE7,   NIL,            0, LX, LX, XMM_TITLE )
    pick( XMM_TITLE8,   NIL,            0, LX, LX, XMM_TITLE )
    pick( XMM_TITLE9,   NIL,            0, LX, LX, XMM_TITLE )
    pick( XMM_TITLE10,  NIL,            0, LX, LX, XMM_TITLE )
    pick( XMM_TITLE11,  NIL,            0, LX, LX, XMM_TITLE )
    pick( XMM_TITLE12,  NIL,            0, LX, LX, XMM_TITLE )
    pick( XMM_TITLE13,  NIL,            0, LX, LX, XMM_TITLE )
    pick( XMM_TITLE14,  NIL,            0, LX, LX, XMM_TITLE )
    pick( XMM_TITLE15,  NIL,            0, LX, LX, XMM_TITLE )

#if defined(defining_enums)
X86T_LAST_ONE };
#undef pick
#undef defining_enums
#endif
