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
    PREF_CS             = 0x0001,
    PREF_DS             = 0x0002,
    PREF_ES             = 0x0004,
    PREF_FS             = 0x0008,
    PREF_GS             = 0x0010,
    PREF_SS             = 0x0020,
    PREF_LOCK           = 0x0040,
    PREF_REPE           = 0x0080,
    PREF_REPNE          = 0x0100,
    PREF_FWAIT          = 0x0200,
    PREF_OPND_SIZE      = 0x0400,
    PREF_ADDR_SIZE      = 0x0800,
    EMU_INTERRUPT       = 0x1000,       /* not a prefix */
    OPND_LONG           = 0x2000,       /* not a prefix */
    ADDR_LONG           = 0x4000,       /* not a prefix */
    FP_INS              = 0x8000        /* not a prefix */
} prefix;

#define PREF_xS (PREF_CS|PREF_DS|PREF_ES|PREF_SS|PREF_FS|PREF_GS)

typedef enum {
    ADDR_NONE,
    ADDR_REG,
    ADDR_CONST,
    ADDR_LABEL,
    ADDR_ABS,
    ADDR_BASE,
    ADDR_INDEX,
    ADDR_BASE_INDEX,
    ADDR_SEG_OFFSET,
    ADDR_DS_SI,
    ADDR_DS_ESI,
    ADDR_ES_DI,
    ADDR_ES_EDI,
    ADDR_DS_SI_ES_DI,
    ADDR_DS_ESI_ES_EDI,
    ADDR_WTK,
    ADDR_WTK_OPCODE
} address_mode;

#define mmpick( en, str ) MOD_##en,
typedef enum {
    #include "mmpick.h"
    MOD_END
} mem_modifier;
#undef mmpick

typedef struct {
    uint_32             disp;
    processor_reg       base;
    processor_reg       index;
    address_mode        mode;
    uint_8              size;
    uint_8              scale;
    uint_8              offset;                 /* offset in instruction */
} operand;

typedef struct {
    prefix              pref;
    uint_16             opcode;
    uint_8              ins_size;
    uint_8              num_oper;
    operand             op[ 3 ];
    mem_modifier        modifier;
    uint_8              mem_ref_op;             /* for VIDEO */
    processor_reg       seg_used;               /* for VIDEO */
    uint_8              mem_ref_size;           /* for VIDEO */
} instruction;

#define NULL_OP         ((uint_8)-1)
#define OP_1            0
#define OP_2            1
#define OP_3            2
