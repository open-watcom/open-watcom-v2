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


#ifndef TURBODBG_H
#define TURBODBG_H  1
#include <watcom.h>
#include "cansymb.h"
#include "objrec.h"

/*
    In order to understand most of this file refer to the document named

        "Turbo Object File Comment Records".
*/

enum turbo_coment_classes {
    TD_CMT_IDENTIFICATION   = 0x00,
    TD_CMT_EXTDEF           = 0xe0,
    TD_CMT_PUBDEF           = 0xe1,
    TD_CMT_STRUCT_DEFN      = 0xe2,
    TD_CMT_TYPE_DEFN        = 0xe3,
    TD_CMT_ENUM_DEFN        = 0xe4,
    TD_CMT_BEGIN_SCOPE      = 0xe5,
    TD_CMT_LOCAL_DEFN       = 0xe6,
    TD_CMT_END_SCOPE        = 0xe7,
    TD_CMT_SELECT_SOURCE    = 0xe8,
    TD_CMT_DEPENDANCY_FILE  = 0xe9,
    TD_CMT_COMPILE_PARMS    = 0xea,
    TD_CMT_EXTDEF_MATCHED   = 0xeb,
    TD_CMT_PUBDEF_MATCHED   = 0xec,
    TD_CMT_BEGIN_SCOPE_LARGE= 0xf5,
    TD_CMT_LOCAL_DEFN_LARGE = 0xf6,
    TD_CMT_LARGE_END_SCOPE  = 0xf7,
    TD_CMT_ATTR             = 0x80  /* attribute field for each COMENT rec */
};

enum turbo_compile_parms {
        /* first byte */
    TD_CP_LANG_UNSPECIFIED  = 0,
    TD_CP_LANG_C            = 1,
    TD_CP_LANG_PASCAL       = 2,
    TD_CP_LANG_BASIC        = 3,
    TD_CP_LANG_ASSEMBLY     = 4,

        /* second byte */
    TD_CP_UNDERBARS_USED    = 0x08,
    TD_CP_MEMORY_SHIFT      = 0,
    TD_CP_MEMORY_FIELD      = 0x07,
    TD_CP_MEMORY_TINY       = 0 << TD_CP_MEMORY_SHIFT,
    TD_CP_MEMORY_SMALL      = 1 << TD_CP_MEMORY_SHIFT,
    TD_CP_MEMORY_MEDIUM     = 2 << TD_CP_MEMORY_SHIFT,
    TD_CP_MEMORY_COMPACT    = 3 << TD_CP_MEMORY_SHIFT,
    TD_CP_MEMORY_LARGE      = 4 << TD_CP_MEMORY_SHIFT,
    TD_CP_MEMORY_HUGE       = 5 << TD_CP_MEMORY_SHIFT,
    TD_CP_MEMORY_386_SMALL  = 6 << TD_CP_MEMORY_SHIFT,
    TD_CP_MEMORY_386_MEDIUM = 7 << TD_CP_MEMORY_SHIFT,
    TD_CP_MEMORY_386_COMPACT= 8 << TD_CP_MEMORY_SHIFT,
    TD_CP_MEMORY_386_LARGE  = 9 << TD_CP_MEMORY_SHIFT
};

enum turbo_publics {
    TD_PUB_VALID_BP         = 0x08,
    TD_PUB_RET_ADDR_SHIFT   = 4,
    TD_PUB_RET_ADDR_FIELD   = 0xf0
};

enum turbo_struct_defn {
    TD_STRUCT_BITS_SHIFT    = 0,
    TD_STRUCT_BITS_FIELD    = 0x3f,
    TD_STRUCT_NEW_OFFSET    = 0x40,
    TD_STRUCT_LAST_MEMBER   = 0x80
};

enum turbo_predefined_types {
    TD_TYPE_VOID            = 0x01,
    TD_TYPE_SIGNED_8        = 0x02,
    TD_TYPE_SIGNED_16       = 0x04,
    TD_TYPE_SIGNED_32       = 0x06,
    TD_TYPE_UNSIGNED_8      = 0x08,
    TD_TYPE_UNSIGNED_16     = 0x0a,
    TD_TYPE_UNSIGNED_32     = 0x0c,
    TD_TYPE_REAL_32         = 0x0e,
    TD_TYPE_REAL_64         = 0x0f,
    TD_TYPE_REAL_80         = 0x10,
    TD_TYPE_REAL_48         = 0x11,     /* pascal 6-byte real */
    TD_TYPE_BOOLEAN         = 0x12,
    TD_TYPE_LAST_PREDEFN    = 0x17
};

enum turbo_type_ids {
    TD_ID_VOID              = 0,
    TD_ID_LSTR,
    TD_ID_DSTR,
    TD_ID_PSTR,
    TD_ID_SCHAR,
    TD_ID_SINT,
    TD_ID_SLONG,
    TD_ID_SQUAD,
    TD_ID_UCHAR,
    TD_ID_UINT,
    TD_ID_ULONG,
    TD_ID_UQUAD,
    TD_ID_PCHAR,
    TD_ID_FLOAT,
    TD_ID_TPREAL,
    TD_ID_DOUBLE,
    TD_ID_LDOUBLE,
    TD_ID_BCD4,
    TD_ID_BCD8,
    TD_ID_BCD10,
    TD_ID_BCDCOB,
    TD_ID_NEAR,
    TD_ID_FAR,
    TD_ID_SEG,
    TD_ID_NEAR386,
    TD_ID_FAR386,
    TD_ID_CARRAY,
    TD_ID_VLARRAY,
    TD_ID_PARRAY,
    TD_ID_ADESC,
    TD_ID_STRUCT,
    TD_ID_UNION,
    TD_ID_VLSTRUCT,
    TD_ID_VLUNION,
    TD_ID_ENUM,
    TD_ID_FUNCTION,
    TD_ID_LABEL,
    TD_ID_SET,
    TD_ID_TFILE,
    TD_ID_BFILE,
    TD_ID_BOOL,
    TD_ID_PENUM,
    TD_ID_RPWORD,
    TD_ID_RTBYTE,

    TD_ID_POINTER_FLAG      = 0x80       /* set to indicate a pointer type */
};

enum td_pointer_info {
    TD_PTR_NEAR_NO_SEG      = 0x00,
    TD_PTR_NEAR_ES          = 0x01,
    TD_PTR_NEAR_CS          = 0x02,
    TD_PTR_NEAR_SS          = 0x03,
    TD_PTR_NEAR_DS          = 0x04,
    TD_PTR_NEAR_FS          = 0x05,
    TD_PTR_NEAR_GS          = 0x06,

    TD_PTR_FAR_NORMAL       = 0x00,
    TD_PTR_FAR_HUGE         = 0x01
};

enum td_function_info {
    TD_FCN_C_NEAR           = 0x00,
    TD_FCN_PASCAL_NEAR      = 0x01,
    TD_FCN_C_FAR            = 0x04,
    TD_FCN_PASCAL_FAR       = 0x05,
    TD_FCN_INTERRUPT        = 0x07,
    TD_FCN_VAR_ARGS_FLAG    = 0x80
};

enum td_local_defn {
    TD_LOCAL_STATIC         = 0x00,
    TD_LOCAL_ABSOLUTE       = 0x01,
    TD_LOCAL_AUTO           = 0x02,
    TD_LOCAL_PASVAR         = 0x03,
    TD_LOCAL_REGISTER       = 0x04,
    TD_LOCAL_TYPEDEF        = 0x06,
    TD_LOCAL_TAG            = 0x07,
    TD_LOCAL_PARM_FLAG      = 0x08      /* OR this in to indicate it's a parm*/
};

enum td_register_ids {
    TD_REG_AX               = 0x00,
    TD_REG_CX,
    TD_REG_DX,
    TD_REG_BX,
    TD_REG_SP,
    TD_REG_BP,
    TD_REG_SI,
    TD_REG_DI,
    TD_REG_AL,
    TD_REG_CL,
    TD_REG_DL,
    TD_REG_BL,
    TD_REG_AH,
    TD_REG_CH,
    TD_REG_DH,
    TD_REG_BH,
    TD_REG_ES,
    TD_REG_CS,
    TD_REG_SS,
    TD_REG_DS,
    TD_REG_FS,
    TD_REG_GS,
    TD_REG_EAX              = 0x18,
    TD_REG_ECX,
    TD_REG_EDX,
    TD_REG_EBX,
    TD_REG_ESP,
    TD_REG_EBP,
    TD_REG_ESI,
    TD_REG_EDI,

    /* the following are extensions Borland added for Watcom compatibility */
    TD_REG_ST0              = 0x20,
    TD_REG_ST1,
    TD_REG_ST2,
    TD_REG_ST3,
    TD_REG_ST4,
    TD_REG_ST5,
    TD_REG_ST6,
    TD_REG_ST7,

    TD_REG_MULTI            = 0x40,
    TD_REG_MULTI_LO_SHIFT   = 0,
    TD_REG_MULTI_HI_SHIFT   = 3,
    TD_REG_MULTI_AX         = 0,
    TD_REG_MULTI_CX,
    TD_REG_MULTI_DX,
    TD_REG_MULTI_BX,
    TD_REG_MULTI_SI,
    TD_REG_MULTI_DI,
    TD_REG_MULTI_ES,
    TD_REG_MULTI_DS,
    TD_REG_MULTI_AXBXCXDX   = TD_REG_MULTI /* AX:AX pair */
};

extern void Can2TDInit( void );
extern void Can2TDFini( void );

/*
    These are internal routines for can2td?.c only!
*/
extern void Can2TDT( void );
extern void Can2TDS( void );
extern void Can2TDStatic( void );
extern void Can2TDTypeDef( void );
extern obj_rec *Can2TDNewRec( uint_8 cmt_class, uint_16 size );
extern void Can2TDEndRec( obj_rec *coment );
extern uint_8 Can2TDBPOffset( symb_handle symb );
#define TD_HAS_A_PUBDEF (0x80000000UL)

#endif
