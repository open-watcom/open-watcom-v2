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


RG( RL_,       RL_,       RL_,       RL_,        RL_,      RG_ ),
RG( RL_DX_AX,  RL_CX_BX,  RL_DX_AX,  RL_,        RL_,      RG_4CRTN ),
RG( RL_8,      RL_8,      RL_8,      RL_,        RL_,      RG_8 ),
RG( RL_STI,    RL_STI,    RL_STI,    RL_,        RL_,      RG_8087  ),
RG( RL_STI,    RL_,       RL_8,      RL_,        RL_,      RG_8087_8  ),
RG( RL_STI,    RL_STI,    RL_STI,    RL_AX,      RL_,      RG_8087_ZAP_ACC  ),
RG( RL_STI,    RL_,       RL_DOUBLE, RL_,        RL_,      RG_8087_DBL  ),
RG( RL_8,      RL_,       RL_STI,    RL_,        RL_,      RG_8_8087  ),
RG( RL_8,      RL_8,      RL_8,      RL_,        RL_WORD,  RG_8_NEED_WORD ),
RG( RL_BYTE,   RL_BYTE,   RL_BYTE,   RL_,        RL_,      RG_BYTE ),
RG( RL_AL,     RL_AL,     RL_AL,     RL_,        RL_,      RG_BYTE_ACC ),
RG( RL_AX,     RL_BYTE,   RL_AL,     RL_AH,      RL_,      RG_BYTE_DIV ),
RG( RL_AX,     RL_BYTE,   RL_AL,     RL_AH_CL,   RL_CL,    RG_BYTE_DIVPOW2 ),
RG( RL_AL,     RL_,       RL_AX,     RL_,        RL_,      RG_BYTE_EXT ),
RG( RL_AX,     RL_BYTE,   RL_AH,     RL_AL,      RL_,      RG_BYTE_MOD ),
RG( RL_AL,     RL_BYTE,   RL_AX,     RL_,        RL_,      RG_BYTE_MUL ),
RG( RL_BYTE,   RL_BYTE,   RL_BYTE,   RL_,        RL_BYTE,  RG_BYTE_NEED ),
RG( RL_BYTE,   RL_BYTE,   RL_BYTE,   RL_,        RL_CL,    RG_BYTE_NEED_CL ),
RG( RL_BYTE,   RL_CL,     RL_BYTE,   RL_,        RL_,      RG_BYTE_SHIFT ),
RG( RL_DOUBLE, RL_,       RL_STI,    RL_,        RL_,      RG_DBL_8087  ),
RG( RL_DOUBLE, RL_,       RL_LOWBYTE,RL_,        RL_,      RG_DBL_BYTE ),
RG( RL_DOUBLE, RL_DOUBLE, RL_DOUBLE, RL_,        RL_CX,    RG_DBL_NEED_CX ),
RG( RL_DP,     RL_DP,     RL_DP,     RL_,        RL_,      RG_DBL_OR_PTR ),
RG( RL_DP,     RL_DP,     RL_DP,     RL_,        RL_DP,    RG_DBL_OR_PTR_NEED ),
RG( RL_DOUBLE, RL_CX,     RL_DOUBLE, RL_CX,      RL_,      RG_DBL_SHIFT ),
RG( RL_DOUBLE, RL_DOUBLE, RL_DOUBLE, RL_,        RL_,      RG_DOUBLE ),
RG( RL_DOUBLE, RL_DOUBLE, RL_DOUBLE, RL_,        RL_DOUBLE,RG_DOUBLE_NEED ),
RG( RL_INDEX,  RL_,       RL_WORD,   RL_,        RL_,      RG_LEA ),
RG( RL_SEG,    RL_,       RL_WORD,   RL_,        RL_,      RG_SEG_WORD ),
RG( RL_ST0,    RL_,       RL_STI,    RL_,        RL_,      RG_ST0_STI  ),
RG( RL_STI,    RL_,       RL_ST0,    RL_,        RL_,      RG_STI_ST0  ),
RG( RL_WORD,   RL_WORD,   RL_WORD,   RL_,        RL_,      RG_WORD ),
RG( RL_AX,     RL_AX,     RL_AX,     RL_,        RL_,      RG_WORD_ACC ),
RG( RL_WORD,   RL_,       RL_DOUBLE, RL_,        RL_,      RG_WORD_DBL ),
RG( RL_DX_AX,  RL_WORD,   RL_AX,     RL_DX,      RL_,      RG_WORD_DIV ),
RG( RL_DX_AX,  RL_WORD,   RL_AX,     RL_DX_CL,   RL_CL,    RG_WORD_DIVPOW2 ),
RG( RL_AX,     RL_,       RL_DX_AX,  RL_,        RL_,      RG_WORD_EXT ),
RG( RL_DX_AX,  RL_WORD,   RL_DX,     RL_AX,      RL_,      RG_WORD_MOD ),
RG( RL_AX,     RL_WORD,   RL_DX_AX,  RL_,        RL_,      RG_WORD_MUL ),
RG( RL_WORD,   RL_WORD,   RL_WORD,   RL_,        RL_WORD,  RG_WORD_NEED ),
RG( RL_WORD,   RL_WORD,   RL_WORD,   RL_,        RL_CL,    RG_WORD_NEED_CL ),
RG( RL_WORD,   RL_WORD,   RL_,       RL_,        RL_,      RG_WORD_OP ),
RG( RL_WORD,   RL_,       RL_SEG,    RL_,        RL_,      RG_WORD_SEG ),
RG( RL_WORD,   RL_CL,     RL_WORD,   RL_,        RL_,      RG_WORD_SHIFT ),
RG( RL_TWOBYTE,RL_TWOBYTE,RL_TWOBYTE,RL_,        RL_,      RG_TWOBYTE ),
RG( RL_WS,     RL_WS,     RL_WS,     RL_,        RL_,      RG_ANYWORD ),
RG( RL_WS,     RL_WS,     RL_WS,     RL_,        RL_WORD,  RG_ANYWORD_NEED ),
RG( RL_SEG,    RL_SEG,    RL_SEG,    RL_,        RL_,      RG_SEG_SEG ),
RG( RL_BYTE,   RL_CL,     RL_BYTE,   RL_,        RL_BYTE,  RG_BYTE_SHIFT_NEED ),
RG( RL_WORD,   RL_CL,     RL_WORD,   RL_,        RL_WORD,  RG_WORD_SHIFT_NEED ),
RG( RL_DOUBLE, RL_CX,     RL_DOUBLE, RL_CX,      RL_CX,    RG_DBL_SHIFT_NEED ),
RG( RL_STI,    RL_STI,    RL_STI,    RL_,        RL_WORD,  RG_8087_NEED ),
#define MAX_RG  27
