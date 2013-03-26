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


typedef enum {
    #define PICK( a, b, c, d, e, f, g ) a,
    #include "ppcfmt.inc"
    #undef PICK
} ppc_template;

enum {
    SRWI,
    CLRRWI,
}; // simplified rotate instructions

/* The following enum is for the BI field of the simplified branch mnemonics.
   Symbol   Value   Meaning
   lt       0       Less than
   gt       1       Greater than
   eq       2       Equal
   so       3       Summary overflow
   un       3       Unordered (after floating-point comparison) */
enum {
    BI_FIRST    = 0,
        BI_LT   = BI_FIRST,
        BI_GT   = 1,
        BI_EQ   = 2,
        BI_SO   = 3,
        BI_UN   = 3,
    BI_LAST     = BI_UN,
}; // BI bit offset

enum {
    B_DNZF      = 0,    // value is the encoding of the BO operand
    B_DZF       = 2,
    B_F         = 4,
    B_DNZT      = 8,
    B_DZT       = 10,
    B_T         = 12,
    B_DNZ       = 16,
    B_DZ        = 18,
    B_UNCOND    = 20,
}; // branch conditions

#define _BICC( x, y )   ( ( ( x ) << 8 ) | ( y ) )
#define _BICC_BO( x )   ( ( ( x ) & 0xff00 ) >> 8 )
#define _BICC_BI( x )   ( ( x ) & 0xff )
enum {
    BICC_LT     = _BICC( 12, BI_LT ),   // (BO operand, BI offset)
    BICC_NG     = _BICC(  4, BI_GT ),
    BICC_EQ     = _BICC( 12, BI_EQ ),
    BICC_NL     = _BICC(  4, BI_LT ),
    BICC_GT     = _BICC( 12, BI_GT ),
    BICC_NE     = _BICC(  4, BI_EQ ),
    BICC_SO     = _BICC( 12, BI_SO ),
    BICC_NS     = _BICC(  4, BI_SO ),
    BICC_UN     = _BICC( 12, BI_UN ),
    BICC_NU     = _BICC(  4, BI_UN ),
}; // bicc contains encoding for both BO and BI operands

enum {
    TO_LGT  = 1,
    TO_LLT  = 2,
    TO_EQ   = 4,
    TO_LGE  = 5,
    TO_LNL  = TO_LGE,
    TO_LLE  = 6,
    TO_LNG  = TO_LLE,
    TO_GT   = 8,
    TO_GE   = 12,
    TO_NL   = TO_GE,
    TO_LT   = 16,
    TO_LE   = 20,
    TO_NG   = TO_LE,
    TO_NE   = 24,
    TO_ANY  = 31,
}; // TO operand encodings

enum {
    XER     = 1,
    LR      = 8,
    CTR     = 9,
    DSISR   = 18,
    DAR     = 19,
    DEC     = 22,
    SDR1    = 25,
    SRR0    = 26,
    SRR1    = 27,
    SPRG    = 272,
    EAR     = 282,
    PVR     = 287,
    BATU    = 528,
    BATL    = 529,
}; // Special Purpose Register encodings

