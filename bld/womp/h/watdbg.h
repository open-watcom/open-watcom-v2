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


#ifndef WATDBG_H
#include "segment.h"
#include "objio.h"

#define WAT_MAJOR       1
#define WAT_MINOR       3

#define WAT_DDTYPES     "\x7" "$$TYPES"
#define WAT_DDSYMBOLS   "\x9" "$$SYMBOLS"
#define WAT_DEBTYP      "\x6" "DEBTYP"
#define WAT_DEBSYM      "\x6" "DEBSYM"

#define WAT_ZAP_DDTYPES "\x7" "$$TZAPS"
#define WAT_ZAP_DDSYMBOLS "\x9" "$$SZAPPYS"
#define WAT_ZAP_DEBTYP  "\x6" "ZAPTYP"
#define WAT_ZAP_DEBSYM  "\x6" "ZAPSYM"

enum wat_type_class_byte {
        /* TYPE_NAME general class */
    WAT_TYPE_NAME           = 0x10,
    WAT_TN_SCALAR           = 0x00,
    WAT_TN_SCOPE            = 0x01,
    WAT_TN_NAME             = 0x02,

        /* ARRAY general class */
    WAT_ARRAY               = 0x20,
    WAT_AY_BYTE_INDEX       = 0x00,
    WAT_AY_WORD_INDEX       = 0x01,
    WAT_AY_LONG_INDEX       = 0x02,
    WAT_AY_TYPE_INDEX       = 0x03,
    WAT_AY_DESC_INDEX       = 0x04,
    WAT_AY_DESC_INDEX_386   = 0x05,

        /* SUBRANGE general class */
    WAT_SUBRANGE            = 0x30,
    WAT_SR_BYTE_RANGE       = 0x00,
    WAT_SR_WORD_RANGE       = 0x01,
    WAT_SR_LONG_RANGE       = 0x02,

        /* POINTER general class */
        /* wat2can1 depends on the ordering of the _PT_* entries */
    WAT_POINTER             = 0x40,
    WAT_PT_NEAR             = 0x00,
    WAT_PT_FAR              = 0x01,
    WAT_PT_HUGE             = 0x02,
    WAT_PT_NEAR_DEREF       = 0x03,
    WAT_PT_FAR_DEREF        = 0x04,
    WAT_PT_HUGE_DEREF       = 0x05,
    WAT_PT_NEAR386          = 0x06,
    WAT_PT_FAR386           = 0x07,
    WAT_PT_NEAR386_DEREF    = 0x08,
    WAT_PT_FAR386_DEREF     = 0x09,

        /* ENUMERATED general class */
    WAT_ENUMERATED          = 0x50,
    WAT_EN_LIST             = 0x00,
    WAT_EN_CONST_BYTE       = 0x01,
    WAT_EN_CONST_WORD       = 0x02,
    WAT_EN_CONST_LONG       = 0x03,

        /* STRUCTURE general class */
    WAT_STRUCTURE           = 0x60,
    WAT_ST_LIST             = 0x00,
    WAT_ST_FIELD_BYTE       = 0x01,
    WAT_ST_FIELD_WORD       = 0x02,
    WAT_ST_FIELD_LONG       = 0x03,
    WAT_ST_BIT_BYTE         = 0x04,
    WAT_ST_BIT_WORD         = 0x05,
    WAT_ST_BIT_LONG         = 0x06,

        /* PROCEDURE general class */
    WAT_PROCEDURE           = 0x70,
    WAT_PR_NEAR             = 0x00,
    WAT_PR_FAR              = 0x01,
    WAT_PR_NEAR386          = 0x02,
    WAT_PR_FAR386           = 0x03,
    WAT_PR_EXT_PARMS        = 0x04,

        /* CHARACTER_BLOCK general class */
    WAT_CHARACTER_BLOCK     = 0x80,
    WAT_CB_CHAR_BYTE        = 0x00,
    WAT_CB_CHAR_WORD        = 0x01,
    WAT_CB_CHAR_LONG        = 0x02,
    WAT_CB_CHAR_IND         = 0x03,
    WAT_CB_CHAR_IND_386     = 0x04
};

/*
    wat2can1 depends on the ordering of the _CLASS_* entries
*/
enum wat_scalar_type_byte {
    WAT_STB_SIZE_FIELD      = 0x0f,     /* size in bytes - 1            */
    WAT_STB_CLASS_FIELD     = 0x70,
    WAT_STB_CLASS_SHIFT     = 4,
    WAT_STB_CLASS_SINT      = 0x00,     /* signed int                   */
    WAT_STB_CLASS_UINT      = 0x10,     /* unsigned int                 */
    WAT_STB_CLASS_FLOAT     = 0x20,
    WAT_STB_CLASS_VOID      = 0x30,     /* void, size = 0               */
    WAT_STB_CLASS_COMPLEX   = 0x40
};

enum wat_symb_class_byte {
    WAT_VARIABLE            = 0x10,
    WAT_VAR_MODULE          = 0x00,
    WAT_VAR_LOCAL           = 0x01,
    WAT_VAR_MODULE_386      = 0x02,

    WAT_CODE                = 0x20,
    WAT_CODE_BLOCK          = 0x00,
    WAT_CODE_NEAR_RTN       = 0x01,
    WAT_CODE_FAR_RTN        = 0x02,
    WAT_CODE_BLOCK_386      = 0x03,
    WAT_CODE_NEAR_RTN_386   = 0x04,
    WAT_CODE_FAR_RTN_386    = 0x05,

    WAT_NEW_BASE            = 0x30,
    WAT_NB_ADD_PREV_SEG     = 0x00,
    WAT_NB_SET_BASE         = 0x01,
    WAT_NB_SET_BASE_386     = 0x02
};

enum wat_location_byte {
    WAT_BP_OFFSET           = 0x10,
    WAT_BP_BYTE             = 0x00,
    WAT_BP_WORD             = 0x01,
    WAT_BP_DWORD            = 0x02,

    WAT_MEMORY              = 0x20,
    WAT_MEM_ADDR_286        = 0x00,
    WAT_MEM_ADDR_386        = 0x01,

    WAT_MULTI_REG           = 0x30,

    WAT_REG                 = 0x40,

    WAT_IND_REG             = 0x50,
    WAT_IR_CALLOC_NEAR      = 0x00,
    WAT_IR_CALLOC_FAR       = 0x01,
    WAT_IR_RALLOC_NEAR      = 0x02,
    WAT_IR_RALLOC_FAR       = 0x03
};

/*
    wat2can1 depends on the ordering of this enum
*/
enum wat_registers {
    WAT_REG_AL,
    WAT_REG_AH,
    WAT_REG_BL,
    WAT_REG_BH,
    WAT_REG_CL,
    WAT_REG_CH,
    WAT_REG_DL,
    WAT_REG_DH,
    WAT_REG_AX,
    WAT_REG_BX,
    WAT_REG_CX,
    WAT_REG_DX,
    WAT_REG_SI,
    WAT_REG_DI,
    WAT_REG_BP,
    WAT_REG_SP,
    WAT_REG_CS,
    WAT_REG_SS,
    WAT_REG_DS,
    WAT_REG_ES,
    WAT_REG_ST0,
    WAT_REG_ST1,
    WAT_REG_ST2,
    WAT_REG_ST3,
    WAT_REG_ST4,
    WAT_REG_ST5,
    WAT_REG_ST6,
    WAT_REG_ST7,
    WAT_REG_EAX,
    WAT_REG_EBX,
    WAT_REG_ECX,
    WAT_REG_EDX,
    WAT_REG_ESI,
    WAT_REG_EDI,
    WAT_REG_EBP,
    WAT_REG_ESP,
    WAT_REG_FS,
    WAT_REG_GS
};

extern void Wat2CanInit( int c70 );
extern void Wat2CanFini( void );

/* this is only used by the code in wat2can0.c */
extern void Wat2CanTandS( seghdr *dd_types, seghdr *dd_Symbols );

#define WATDBG_H    1
#endif

