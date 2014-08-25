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


#ifndef MSDBG_H
#include "watcom.h"
#include "queue.h"
#include "objrec.h"

/*
    This file was built from information provided in

        "Microsoft Symbolic Debugging OMF document", v2.0 June 8, 1989.

    Any hope of understanding these definitions will require said document.
*/

#define MS_DDTYPES      "\x7" "$$TYPES"
#define MS_DDTYPES_LEN  8
#define MS_DDSYMBOLS    "\x9" "$$SYMBOLS"
#define MS_DDSYMBOLS_LEN 10
#define MS_DEBTYP       "\x6" "DEBTYP"
#define MS_DEBTYP_LEN   7
#define MS_DEBSYM       "\x6" "DEBSYM"
#define MS_DEBSYM_LEN   7

#define MS_FLAT         "\x4" "FLAT"
#define MS_FLAT_LEN     5

enum ms_start_leaves {
    MS_SL_BITFIELD          = 0x5c,
    MS_SL_NEWTYPE           = 0x5d,
    MS_SL_STRING            = 0x60,
    MS_SL_CONST             = 0x71,
    MS_SL_LABEL             = 0x72,
    MS_SL_PROCEDURE         = 0x75,
    MS_SL_PARAMETER         = 0x76,
    MS_SL_ARRAY             = 0x78,
    MS_SL_STRUCTURE         = 0x79,
    MS_SL_POINTER           = 0x7a,
    MS_SL_SCALAR            = 0x7b,
    MS_SL_LIST              = 0x7f,
    MS_SL_BARRAY            = 0x8c,
    MS_SL_FSTRING           = 0x8d,
    MS_SL_FARRIDX           = 0x8e,
    MS_SL_SKIP              = 0x90,
    MS_SL_BASED             = 0x91
};

enum ms_basic_component_leaves {
    MS_BCL_NIL              = 0x80,
    MS_BCL_STRING           = 0x82,
    MS_BCL_INDEX            = 0x83,
    MS_BCL_UINT_16          = 0x85,
    MS_BCL_UINT_32          = 0x86,
    MS_BCL_INT_8            = 0x88,
    MS_BCL_INT_16           = 0x89,
    MS_BCL_INT_32           = 0x8a,
    MS_BCL_BASEDSEG         = 0x92,
    MS_BCL_BASEDVAL         = 0x93,
    MS_BCL_BASEDSEGVAL      = 0x94,
    MS_BCL_BASEDADDR        = 0x97,
    MS_BCL_BASEDSEGADDR     = 0x98
};

enum ms_basic_type_leaves {
    MS_BTL_TAG              = 0x5a,
    MS_BTL_VARIANT          = 0x5b,
    MS_BTL_BOOLEAN          = 0x6c,
    MS_BTL_CHARACTER        = 0x6f,
    MS_BTL_INTEGER          = 0x70,
    MS_BTL_UNSIGNED_INT     = 0x7c,
    MS_BTL_SIGNED_INT       = 0x7d,
    MS_BTL_REAL             = 0x7e
};

enum ms_other_leaves {
    MS_OL_HUGE              = 0x5e,
    MS_OL_C_NEAR            = 0x63,
    MS_OL_C_FAR             = 0x64,
    MS_OL_PACKED            = 0x68,
    MS_OL_UNPACKED          = 0x69,
    MS_OL_PLM_FAR           = 0x73,
    MS_OL_PLM_NEAR          = 0x74,
    MS_OL_FASTCALL_NEAR     = 0x95,
    MS_OL_FASTCALL_FAR      = 0x96,
    MS_OL_INLINE            = 0x99
};

enum ms_reserved_types {
/*
    byte format is  x xx xxx xx
                    i md typ sz
*/
    MS_RT_I_FIELD           = 0x80,     /* 1 means interpret md typ sz */
                                        /* 0 means use special_types */

    MS_RT_MD_FIELD          = 0x60,
    MS_RT_MD_DIRECT         = 0x00,     /* 00 in md field       */
    MS_RT_MD_NEAR_PTR       = 0x20,     /* 01                   */
    MS_RT_MD_FAR_PTR        = 0x40,     /* 10                   */
    MS_RT_MD_HUGE_PTR       = 0x60,     /* 11                   */

    MS_RT_TYP_FIELD         = 0x1c,
    MS_RT_TYP_SIGNED        = 0x00,     /* 000 in typ field     */
    MS_RT_TYP_UNSIGNED      = 0x04,     /* 001                  */
    MS_RT_TYP_REAL          = 0x08,     /* 010                  */
    MS_RT_TYP_COMPLEX       = 0x0c,     /* 011                  */
    MS_RT_TYP_BOOLEAN       = 0x10,     /* 100                  */
    MS_RT_TYP_ASCII         = 0x14,     /* 101                  */
    MS_RT_TYP_CURRENCY      = 0x18,     /* 110                  */

    MS_RT_SZ_FIELD          = 0x03,
    MS_RT_SZ_00             = 0x00,     /* 00 in sz field       */
    MS_RT_SZ_01             = 0x01,     /* 01                   */
    MS_RT_SZ_10             = 0x02,     /* 10                   */
    MS_RT_SZ_11             = 0x03,     /* 11                   */
        /* for SIGNED, UNSIGNED, BOOLEAN, ASCII */
    MS_SZ_8BIT              = MS_RT_SZ_00,
    MS_SZ_16BIT             = MS_RT_SZ_01,
    MS_SZ_32BIT             = MS_RT_SZ_10,
        /* for REAL */
    MS_SZ_REAL_4            = MS_RT_SZ_00,
    MS_SZ_REAL_8            = MS_RT_SZ_01,
    MS_SZ_REAL_10           = MS_RT_SZ_10,
        /* for COMPLEX */
    MS_SZ_COMPLEX_8         = MS_RT_SZ_00,
    MS_SZ_COMPLEX_16        = MS_RT_SZ_01,
    MS_SZ_COMPLEX_20        = MS_RT_SZ_10,
        /* for CURRENCY */
    MS_SZ_CURRENCY_8        = MS_RT_SZ_10
};

enum ms_special_types {
    MS_T_NOTYPE             = 0x00,
    MS_T_ABS                = 0x01
};

enum ms_register_vars {
    MS_REG_AL               = 0x00,
    MS_REG_CL               = 0x01,
    MS_REG_DL               = 0x02,
    MS_REG_BL               = 0x03,
    MS_REG_AH               = 0x04,
    MS_REG_CH               = 0x05,
    MS_REG_DH               = 0x06,
    MS_REG_BH               = 0x07,

    MS_REG_AX               = 0x08,
    MS_REG_CX               = 0x09,
    MS_REG_DX               = 0x0a,
    MS_REG_BX               = 0x0b,
    MS_REG_SP               = 0x0c,
    MS_REG_BP               = 0x0d,
    MS_REG_SI               = 0x0e,
    MS_REG_DI               = 0x0f,

    MS_REG_EAX              = 0x10,
    MS_REG_ECX              = 0x11,
    MS_REG_EDX              = 0x12,
    MS_REG_EBX              = 0x13,
    MS_REG_ESP              = 0x14,
    MS_REG_EBP              = 0x15,
    MS_REG_ESI              = 0x16,
    MS_REG_EDI              = 0x17,

    MS_REG_ES               = 0x18,
    MS_REG_CS               = 0x19,
    MS_REG_SS               = 0x1a,
    MS_REG_DS               = 0x1b,
    MS_REG_FS               = 0x1c,
    MS_REG_GS               = 0x1d,

    MS_REG_DX_AX            = 0x20,
    MS_REG_ES_BX            = 0x21,
    MS_REG_IP               = 0x22,
    MS_REG_FLAGS            = 0x23,

    MS_REG_ST0              = 0x80,
    MS_REG_ST1              = 0x81,
    MS_REG_ST2              = 0x82,
    MS_REG_ST3              = 0x83,
    MS_REG_ST4              = 0x84,
    MS_REG_ST5              = 0x85,
    MS_REG_ST6              = 0x86,
    MS_REG_ST7              = 0x87
};

enum ms_symbols_records {
    MS_SYM_BLOCK_START      = 0x00,
    MS_SYM_PROCEDURE_START  = 0x01,
    MS_SYM_END_RECORD       = 0x02,
    MS_SYM_BP_RELATIVE      = 0x04,
    MS_SYM_LOCAL_SYM        = 0x05,
    MS_SYM_CODE_LABEL       = 0x0b,
    MS_SYM_WITH_START       = 0x0c,
    MS_SYM_REGISTER_SYM     = 0x0d,
    MS_SYM_CONSTANT_SYM     = 0x0e,
    MS_SYM_FORTRAN_ENTRY    = 0x0f,
    MS_SYM_SKIP_RECORD      = 0x10,
    MS_SYM_CHANGE_DEF_SEG   = 0x11,
    MS_SYM_TYPEDEF_SYM      = 0x12,

    MS_SYM_386_FLAG         = 0x80      /* indicates 386 style record */
};

enum ms_symbols_others {
    MS_SYM_NEAR             = 0x00,
    MS_SYM_FAR              = 0x04
};

extern int Can2MSMetaware;

extern void Can2MsInit( int metaware, int os2_specific );
extern void Can2MsFini( void );
extern int Can2MsOS2Flat( void );

/* these are only called by routines in can2ms0.c */
extern void Can2MsT( void );
extern void Can2MsS( void );
extern obj_rec *Can2MsRec( uint_8 command );
extern obj_rec *Can2MsSegdef( uint_8 *lname_data, uint_16 lname_len );

#define MSDBG_H 1
#endif
