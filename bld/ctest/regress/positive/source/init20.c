#include "fail.h"
#include <stdlib.h>
#include <string.h>

/* Initialization torture test. This is extracted (with modifications)
 * from the prag386 module of the C++ compiler. The primary purpose is
 * to ensure that the cfe doesn't keel over when parsing this mess.
 */

typedef unsigned hw_reg_part;
typedef hw_reg_part HWT;
typedef struct hw_reg_set {
     HWT _0;
} hw_reg_set;
 enum { HW_AH_0 = (HWT)0x0001U+(HWT)(0x0000U<<16)  } ;
 enum { HW_AL_0 = (HWT)0x0002U+(HWT)(0x0000U<<16)  } ;
 enum { HW_BH_0 = (HWT)0x0004U+(HWT)(0x0000U<<16)  } ;
 enum { HW_BL_0 = (HWT)0x0008U+(HWT)(0x0000U<<16)  } ;
 enum { HW_CH_0 = (HWT)0x0010U+(HWT)(0x0000U<<16)  } ;
 enum { HW_CL_0 = (HWT)0x0020U+(HWT)(0x0000U<<16)  } ;
 enum { HW_DH_0 = (HWT)0x0040U+(HWT)(0x0000U<<16)  } ;
 enum { HW_DL_0 = (HWT)0x0080U+(HWT)(0x0000U<<16)  } ;
 enum { HW_SI_0 = (HWT)0x0100U+(HWT)(0x0000U<<16)  } ;
 enum { HW_DI_0 = (HWT)0x0200U+(HWT)(0x0000U<<16)  } ;
 enum { HW_BP_0 = (HWT)0x0400U+(HWT)(0x0000U<<16)  } ;
 enum { HW_SP_0 = (HWT)0x0800U+(HWT)(0x0000U<<16)  } ;
 enum { HW_DS_0 = (HWT)0x1000U+(HWT)(0x0000U<<16)  } ;
 enum { HW_ES_0 = (HWT)0x2000U+(HWT)(0x0000U<<16)  } ;
 enum { HW_CS_0 = (HWT)0x4000U+(HWT)(0x0000U<<16)  } ;
 enum { HW_SS_0 = (HWT)0x8000U+(HWT)(0x0000U<<16)  } ;
 enum { HW_ST0_0 = (HWT)0x0000U+(HWT)(0x0001U<<16)  } ;
 enum { HW_ST1_0 = (HWT)0x0000U+(HWT)(0x0002U<<16)  } ;
 enum { HW_ST2_0 = (HWT)0x0000U+(HWT)(0x0004U<<16)  } ;
 enum { HW_ST3_0 = (HWT)0x0000U+(HWT)(0x0008U<<16)  } ;
 enum { HW_ST4_0 = (HWT)0x0000U+(HWT)(0x0010U<<16)  } ;
 enum { HW_ST5_0 = (HWT)0x0000U+(HWT)(0x0020U<<16)  } ;
 enum { HW_ST6_0 = (HWT)0x0000U+(HWT)(0x0040U<<16)  } ;
 enum { HW_ST7_0 = (HWT)0x0000U+(HWT)(0x0080U<<16)  } ;
 enum { HW_EAXH_0 = (HWT)0x0000U+(HWT)(0x0100U<<16)  } ;
 enum { HW_EBXH_0 = (HWT)0x0000U+(HWT)(0x0200U<<16)  } ;
 enum { HW_ECXH_0 = (HWT)0x0000U+(HWT)(0x0400U<<16)  } ;
 enum { HW_EDXH_0 = (HWT)0x0000U+(HWT)(0x0800U<<16)  } ;
 enum { HW_ESIH_0 = (HWT)0x0000U+(HWT)(0x1000U<<16)  } ;
 enum { HW_EDIH_0 = (HWT)0x0000U+(HWT)(0x2000U<<16)  } ;
 enum { HW_FS_0 = (HWT)0x0000U+(HWT)(0x4000U<<16)  } ;
 enum { HW_GS_0 = (HWT)0x0000U+(HWT)(0x8000U<<16)  } ;
 enum { HW_FULL_0 = (HWT)0xffffU+(HWT)(0xffffU<<16)  } ;
 enum { HW_UNUSED_0 = (HWT)0x0000U+(HWT)(0x0000U<<16)  } ;
 enum { HW_EMPTY_0 = (HWT)0x0000U+(HWT)(0x0000U<<16)  } ;
 enum { HW_AX_0 = (hw_reg_part)(HW_AL_0+HW_AH_0),
        HW_BX_0 = (hw_reg_part)(HW_BL_0+HW_BH_0),
    HW_CX_0 = (hw_reg_part)(HW_CL_0+HW_CH_0),
    HW_DX_0 = (hw_reg_part)(HW_DL_0+HW_DH_0),
    HW_EAX_0 = (hw_reg_part)(HW_EAXH_0+HW_AX_0),
    HW_EBX_0 = (hw_reg_part)(HW_EBXH_0+HW_BX_0),
    HW_ECX_0 = (hw_reg_part)(HW_ECXH_0+HW_CX_0),
    HW_EDX_0 = (hw_reg_part)(HW_EDXH_0+HW_DX_0),
    HW_ESI_0 = (hw_reg_part)(HW_ESIH_0+HW_SI_0),
    HW_EDI_0 = (hw_reg_part)(HW_EDIH_0+HW_DI_0),
    HW_ABCD_0 = (hw_reg_part)(HW_AX_0+HW_BX_0 +HW_CX_0+HW_DX_0),
    HW_SDSB_0 = (hw_reg_part)(HW_SI_0+HW_DI_0 +HW_SP_0+HW_BP_0),
        HW_SEGS_0 = (hw_reg_part)(HW_DS_0+HW_ES_0+HW_CS_0 +HW_SS_0+HW_FS_0+HW_GS_0),
    HW_IDX16_0 = (hw_reg_part)(HW_SI_0+HW_DI_0+HW_BX_0),
    HW_FLTS_0 = (hw_reg_part)(HW_ST0_0+HW_ST1_0 +HW_ST2_0+HW_ST3_0 +HW_ST4_0+HW_ST5_0+HW_ST6_0+HW_ST7_0),
    HW_FLTS_NOT_ST0_0 = (hw_reg_part)(HW_ST1_0 +HW_ST2_0+HW_ST3_0 +HW_ST4_0+HW_ST5_0+HW_ST6_0+HW_ST7_0),
    HW_32_0 = (hw_reg_part)(HW_EAXH_0+HW_EBXH_0 +HW_ECXH_0+HW_EDXH_0+HW_ESIH_0+HW_EDIH_0),
    HW_DX_AX_0 = (hw_reg_part)(HW_DX_0+HW_AX_0),
        HW_CX_BX_0 = (hw_reg_part)(HW_CX_0+HW_BX_0),
    HW_CX_DI_0 = (hw_reg_part)(HW_CX_0+HW_DI_0),
    HW_BX_SI_0 = (hw_reg_part)(HW_BX_0+HW_SI_0),
    HW_BX_DI_0 = (hw_reg_part)(HW_BX_0+HW_DI_0),
    HW_BP_SI_0 = (hw_reg_part)(HW_BP_0+HW_SI_0),
    HW_BP_DI_0 = (hw_reg_part)(HW_BP_0+HW_DI_0),
    HW_DS_BX_0 = (hw_reg_part)(HW_DS_0+HW_BX_0),
    HW_DS_SI_0 = (hw_reg_part)(HW_DS_0+HW_SI_0),
    HW_DS_ESI_0 = (hw_reg_part)(HW_DS_0+HW_ESI_0),
    HW_DS_DI_0 = (hw_reg_part)(HW_DS_0+HW_DI_0),
    HW_SS_BX_0 = (hw_reg_part)(HW_SS_0+HW_BX_0),
    HW_SS_SI_0 = (hw_reg_part)(HW_SS_0+HW_SI_0),
        HW_SS_DI_0 = (hw_reg_part)(HW_SS_0+HW_DI_0),
    HW_ES_BX_0 = (hw_reg_part)(HW_ES_0+HW_BX_0),
    HW_ES_SI_0 = (hw_reg_part)(HW_ES_0+HW_SI_0),
    HW_ES_DI_0 = (hw_reg_part)(HW_ES_0+HW_DI_0),
    HW_ES_EDI_0 = (hw_reg_part)(HW_ES_0+HW_EDI_0),
    HW_ECX_ESI_0 = (hw_reg_part)(HW_ECX_0+HW_ESI_0),
    HW_ECX_EDI_0 = (hw_reg_part)(HW_ECX_0+HW_EDI_0),
    HW_CS_EDI_0 = (hw_reg_part)(HW_CS_0+HW_EDI_0),
    HW_FS_GS_0 = (hw_reg_part)(HW_FS_0+HW_GS_0),
    HW_DS_GS_0 = (hw_reg_part)(HW_DS_0+HW_GS_0),
    HW_ECX_EBX_0 = (hw_reg_part)(HW_ECX_0+HW_EBX_0),
        HW_32_BP_SP_0= (hw_reg_part)(HW_32_0+HW_BP_0+HW_SP_0),
    HW_DS_ES_FS_GS_0 = (hw_reg_part)(HW_DS_0+HW_ES_0+ HW_FS_0+HW_GS_0),
    HW_DS_ES_SS_CS_0 = (hw_reg_part)(HW_DS_0+HW_ES_0+ HW_SS_0+HW_CS_0),
    HW_AL_BL_CL_DL_0 = (hw_reg_part)(HW_AL_0+HW_BL_0+ HW_CL_0+HW_DL_0),
    HW_AH_BH_CH_DH_0 = (hw_reg_part)(HW_AH_0+HW_BH_0+ HW_CH_0+HW_DH_0),
    HW_AX_BX_CX_DX_SI_DI_0 = (hw_reg_part)(HW_AX_0+HW_BX_0 +HW_CX_0+HW_DX_0+HW_SI_0+HW_DI_0), HW__COMPOUND_END_0
};
typedef unsigned long call_class;
typedef unsigned long byte_seq_len;
#pragma pack(1)
typedef struct byte_seq {
        byte_seq_len    length;
        char            data[];
} byte_seq;
#pragma pack()
struct inline_funcs {
    char            *name;
    byte_seq        *code;
    hw_reg_set      *parms;
    hw_reg_set      returns;
    hw_reg_set      save;
};
hw_reg_set DefaultParms[] = {
         {+ (HW_EAX_0+HW_EBX_0+HW_ECX_0+HW_EDX_0 )       }   ,
        { 0 } };
        char Registers[] = {
                'e','s', '\0' ,
                'd','s', '\0' ,
                'c','s', '\0' ,
                's','s', '\0' ,
                'a','x', '\0' ,
                'b','x', '\0' ,
                'c','x', '\0' ,
                'd','x', '\0' ,
                'a','l', '\0' ,
                'b','l', '\0' ,
                'c','l', '\0' ,
                'd','l', '\0' ,
                'a','h', '\0' ,
                'b','h', '\0' ,
                'c','h', '\0' ,
                'd','h', '\0' ,
                's','i', '\0' ,
                'd','i', '\0' ,
                'b','p', '\0' ,
                's','p', '\0' ,
                'f','s', '\0' ,
                'g','s', '\0' ,
                'e','a','x', '\0' ,
                'e','b','x', '\0' ,
                'e','c','x', '\0' ,
                'e','d','x', '\0' ,
                'e','s','i', '\0' ,
                'e','d','i', '\0' ,
                'e','b','p', '\0' ,
                'e','s','p', '\0' ,
                 '\0'  };
        hw_reg_set RegBits[] ={  {+ (HW_ES_0 )       } ,
                                 {+ (HW_DS_0 )       } ,
                                 {+ (HW_CS_0 )       } ,
                                 {+ (HW_SS_0 )       } ,
                                 {+ (HW_AX_0 )       } ,
                                 {+ (HW_BX_0 )       } ,
                                 {+ (HW_CX_0 )       } ,
                                 {+ (HW_DX_0 )       } ,
                                 {+ (HW_AL_0 )       } ,
                                 {+ (HW_BL_0 )       } ,
                                 {+ (HW_CL_0 )       } ,
                                 {+ (HW_DL_0 )       } ,
                                 {+ (HW_AH_0 )       } ,
                                 {+ (HW_BH_0 )       } ,
                                 {+ (HW_CH_0 )       } ,
                                 {+ (HW_DH_0 )       } ,
                                 {+ (HW_SI_0 )       } ,
                                 {+ (HW_DI_0 )       } ,
                                 {+ (HW_BP_0 )       } ,
                                 {+ (HW_SP_0 )       } ,
                                 {+ (HW_FS_0 )       } ,
                                 {+ (HW_GS_0 )       } ,
                                 {+ (HW_EAX_0 )       } ,
                                 {+ (HW_EBX_0 )       } ,
                                 {+ (HW_ECX_0 )       } ,
                                 {+ (HW_EDX_0 )       } ,
                                 {+ (HW_ESI_0 )       } ,
                                 {+ (HW_EDI_0 )       } ,
                                 {+ (HW_BP_0 )       } ,
                                 {+ (HW_SP_0 )       } ,
 };
hw_reg_set DefaultVarParms[] = {
        { 0 } };
hw_reg_set MSC_Save =  {+ (HW_ESI_0+HW_EDI_0+HW_BP_0 )       } ;
enum    asm_codes {
        push_es = 0x06,
        pop_es  = 0x07,
        sbb_rr  = 0x19,
        push_ds = 0x1e,
        pop_ds  = 0x1f,
        sub_rr  = 0x29,
        xor_rr  = 0x31,
        cmp_al  = 0x3c,
        inc_cx  = 0x41,
        inc_di  = 0x47,
        dec_cx  = 0x49,
        dec_si  = 0x4e,
        dec_di  = 0x4f,
        push_ax = 0x50,
        push_cx = 0x51,
        push_dx = 0x52,
        push_si = 0x56,
        push_di = 0x57,
        pop_ax  = 0x58,
        pop_cx  = 0x59,
        pop_dx  = 0x5a,
        pop_si  = 0x5e,
        pop_di  = 0x5f,
        size_prefix = 0x66,
        je      = 0x74,
        jne     = 0x75,
        jge     = 0x7d,
        xchg_cx_ax = 0x91,
        xchg_si_ax = 0x96,
        cbw     = 0x98,
        cwde    = 0x98,
        cwd     = 0x99,
        fwait   = 0x9b,
        movsb   = 0xa4,
        movsd   = 0xa5,
        cmpsb   = 0xa6,
        stosb   = 0xaa,
        stosd   = 0xab,
        lodsb   = 0xac,
        lodsd   = 0xad,
        scasb   = 0xae,
        jcxz    = 0xe3,
        in_al_dx= 0xec,
        in_ax_dx= 0xed,
        out_dx_al=0xee,
        out_dx_ax=0xef,
        rep     = 0xf3,
        repnz   = 0xf2,
        repz    = 0xf3,
        cli     = 0xfa,
        sti     = 0xfb,
        cld     = 0xfc,
        ax_ax   = 0xc0,
        di_cx   = 0xcf,
        ax_dx   = 0xd0,
        cx_cx   = 0xc9,
        si_si   = 0xf6,
        hide1   = 0xa8,
        cmc     = 0xf5,
};
static byte_seq C_strcpy = { 0x1A, {
    0x57,0x8A,0x06,0x88,0x07,0x3C,0x00,0x74,0x10,0x8A,
    0x46,0x01,0x83,0xC6,0x02,0x88,0x47,0x01,0x83,0xC7,
    0x02,0x3C,0x00,0x75,0xE8,0x5F
} };
static byte_seq S_strcpy = { 0x0A, {
    0x57,0xAC,0x88,0x07,0x47,0x3C,0x00,0x75,0xF8,0x5F
} };
static byte_seq Z_strcpy = { 0x08, {
    0x57,0xAC,0xAA,0x3C,0x00,0x75,0xFA,0x5F
} };
static byte_seq BD_strcpy = { 0x1C, {
    0x57,0x8A,0x06,0x26,0x88,0x07,0x3C,0x00,0x74,0x11,
    0x8A,0x46,0x01,0x83,0xC6,0x02,0x26,0x88,0x47,0x01,
    0x83,0xC7,0x02,0x3C,0x00,0x75,0xE6,0x5F
} };
static byte_seq DP_strcpy = { 0x2A, {
    0x1E,0x06,0x50,0x66,0x8E,0xC2,0x66,0x8E,0xD9,0x89,
    0xDE,0x89,0xC7,0x8A,0x06,0x26,0x88,0x07,0x3C,0x00,
    0x74,0x11,0x8A,0x46,0x01,0x83,0xC6,0x02,0x26,0x88,
    0x47,0x01,0x83,0xC7,0x02,0x3C,0x00,0x75,0xE6,0x58,
    0x07,0x1F
} };
static byte_seq C_strcat = { 0x26, {
    0x06,0x1E,0x07,0x57,0x29,0xC9,0x49,0xB0,0x00,0xF2,
    0xAE,0x4F,0x8A,0x06,0x88,0x07,0x3C,0x00,0x74,0x10,
    0x8A,0x46,0x01,0x83,0xC6,0x02,0x88,0x47,0x01,0x83,
    0xC7,0x02,0x3C,0x00,0x75,0xE8,0x5F,0x07
} };
static byte_seq FC_strcat = { 0x22, {
    0x57,0x29,0xC9,0x49,0xB0,0x00,0xF2,0xAE,0x4F,0x8A,
    0x06,0x88,0x07,0x3C,0x00,0x74,0x10,0x8A,0x46,0x01,
    0x83,0xC6,0x02,0x88,0x47,0x01,0x83,0xC7,0x02,0x3C,
    0x00,0x75,0xE8,0x5F
} };
static byte_seq S_strcat = { 0x11, {
    0x57,0x4F,0x47,0x80,0x3F,0x00,0x75,0xFA,0xAC,0x88,
    0x07,0x47,0x3C,0x00,0x75,0xF8,0x5F
} };
static byte_seq Z_strcat = { 0x12, {
    0x57,0x51,0x29,0xC9,0x49,0xB0,0x00,0xF2,0xAE,0x59,
    0x4F,0xAC,0xAA,0x3C,0x00,0x75,0xFA,0x5F
} };
static byte_seq BD_strcat = { 0x1C, {
    0x57,0x29,0xC9,0x49,0xB0,0x00,0xF2,0xAE,0x4F,0x8A,
    0x06,0xAA,0x3C,0x00,0x74,0x0B,0x8A,0x46,0x01,0x83,
    0xC6,0x02,0xAA,0x3C,0x00,0x75,0xEE,0x5F
} };
static byte_seq DP_strcat = { 0x2A, {
    0x1E,0x06,0x50,0x66,0x8E,0xC2,0x66,0x8E,0xD9,0x89,
    0xDE,0x89,0xC7,0x29,0xC9,0x49,0xB0,0x00,0xF2,0xAE,
    0x4F,0x8A,0x06,0xAA,0x3C,0x00,0x74,0x0B,0x8A,0x46,
    0x01,0x83,0xC6,0x02,0xAA,0x3C,0x00,0x75,0xEE,0x58,
    0x07,0x1F
} };
static byte_seq C_strchr = { 0x18, {
    0x8A,0x06,0x38,0xD0,0x74,0x12,0x3C,0x00,0x74,0x0C,
    0x46,0x8A,0x06,0x38,0xD0,0x74,0x07,0x46,0x3C,0x00,
    0x75,0xEA,0x29,0xF6
} };
static byte_seq S_strchr = { 0x0D, {
    0x8A,0x06,0x38,0xD0,0x74,0x07,0x46,0x3C,0x00,0x75,
    0xF5,0x29,0xF6
} };
static byte_seq BD_strchr = { 0x12, {
    0x66,0x8C,0xDA,0x8A,0x06,0x38,0xC8,0x74,0x09,0x46,
    0x3C,0x00,0x75,0xF5,0x29,0xF6,0x29,0xD2
} };
static byte_seq C_div = {
                8,
              { cwd,
                 0xf7,0xf9 ,
                0x89,0x06,
                0x89,0x56,0x04 }
        };
static byte_seq S_memset = {
                8,
              { push_es,
                push_di,
                push_ds,
                pop_es,
                rep,
                stosb,
                pop_di,
                pop_es }
        };
static byte_seq FS_memset = {
                4,
              { push_di,
                rep,
                stosb,
                pop_di, }
        };
static byte_seq C_memset = {
                 13 +4,
              { push_es,
                push_ds,
                pop_es,
                 push_di, 0x88,0xc4, 0xd1,0xe9, rep, size_prefix,stosd, 0x11,0xc9, rep, stosb, pop_di ,
                pop_es }
        };
static byte_seq FC_memset = {
                 13 ,
              {  push_di, 0x88,0xc4, 0xd1,0xe9, rep, size_prefix,stosd, 0x11,0xc9, rep, stosb, pop_di , }
        };
static byte_seq Z_memset = {
                4,
              { push_di,
                rep,
                stosb,
                pop_di }
        };
static byte_seq BD_memset = {
                13,
              { push_di,
                 0x88,0xc4 ,
                 0xd1,0xe9 ,
                rep,
                 size_prefix,stosd ,
                 0x11,0xc9 ,
                rep,
                stosb,
                pop_di }
        };
static byte_seq DP_memset = {
                17,
              { push_es,
                push_di,
                 0x8e,0xc2 ,
                 0x88,0xc4 ,
                 0xd1,0xe9 ,
                rep,
                 size_prefix,stosd ,
                 0x11,0xc9 ,
                rep,
                stosb,
                pop_di,
                pop_es }
        };
static byte_seq C_stosw = {
                7,
              { push_es,
                 0x8e,0xc2 ,
                rep,
                 size_prefix,stosd ,
                pop_es }
        };
static byte_seq F_stosw = {
                3,
              { rep,
                 size_prefix,stosd , }
        };
static byte_seq BD_stosw = {
                3,
              { rep,
                 size_prefix,stosd , }
        };
static byte_seq C_stosd = {
                6,
              { push_es,
                 0x8e,0xc2 ,
                rep,
                stosd,
                pop_es }
        };
static byte_seq F_stosd = {
                2,
              { rep,
                stosd, }
        };
static byte_seq BD_stosd = {
                2,
              { rep,
                stosd, }
        };
static byte_seq C_stoswb = {
                8,
              { push_es,
                 0x8e,0xc2 ,
                rep,
                 size_prefix,stosd ,
                stosb,
                pop_es }
        };
static byte_seq F_stoswb = {
                4,
              { rep,
                 size_prefix,stosd ,
                stosb, }
        };
static byte_seq BD_stoswb = {
                4,
              { rep,
                 size_prefix,stosd ,
                stosb, }
        };
static byte_seq S_memcpy = {
                8,
              { push_es,
                push_di,
                push_ds,
                pop_es,
                rep,
                movsb,
                pop_di,
                pop_es }
        };
static byte_seq FS_memcpy = {
                4,
              { push_di,
                rep,
                movsb,
                pop_di, }
        };
static byte_seq C_memcpy = {
                 16 +6,
              { push_es,
                 0x8c,0xd8 ,
                 0x8e,0xc0 ,
                 push_di, 0x89,0xc8, 0xc1,0xe9,0x02, rep, movsd, 0x8a,0xc8, 0x80,0xe1,3, rep, movsb, pop_di ,
                pop_es }
        };
static byte_seq FC_memcpy = {
                 16 ,
              {  push_di, 0x89,0xc8, 0xc1,0xe9,0x02, rep, movsd, 0x8a,0xc8, 0x80,0xe1,3, rep, movsb, pop_di  }
        };
static byte_seq Z_memcpy = {
                4,
              { push_di,
                rep,
                movsb,
                pop_di }
        };
static byte_seq BD_memcpy = {
                 16 ,
              {  push_di, 0x89,0xc8, 0xc1,0xe9,0x02, rep, movsd, 0x8a,0xc8, 0x80,0xe1,3, rep, movsb, pop_di  }
        };
static byte_seq DP_memcpy = {
                25,
              { push_ds,
                push_es,
                push_di,
                xchg_cx_ax,
                 0x8e,0xd8 ,
                 0x8e,0xc2 ,
                 0x89,0xc8 ,
                 0xc1,0xe9,0x02 ,
                rep,
                movsd,
                 0x8a,0xc8 ,
                 0x80,0xe1 ,3,
                rep,
                movsb,
                pop_ax,
                pop_es,
                pop_ds }
        };
static byte_seq C_memcmp = {
                 11 +6,
              { push_es,
                 0x8c,0xd8 ,
                 0x8e,0xc0 ,
                 xor_rr, ax_ax, repz, cmpsb, je, 5, sbb_rr, ax_ax, 0x83,0xd8,0xff ,
                pop_es }
        };
static byte_seq FC_memcmp = {
                 11 ,
              {  xor_rr, ax_ax, repz, cmpsb, je, 5, sbb_rr, ax_ax, 0x83,0xd8,0xff  }
        };
static byte_seq BD_memcmp = {
                11,
              {  0x09,0xc9 ,
                repz,
                cmpsb,
                je,  5,
                sbb_rr, cx_cx,
                 0x83,0xd9,0xff  }
        };
static byte_seq DP_memcmp = {
                20,
              { push_ds,
                push_es,
                xchg_cx_ax,
                 0x8e,0xda ,
                 0x8e,0xc0 ,
                 0x09,0xc9 ,
                repz,
                cmpsb,
                je,  5,
                sbb_rr, cx_cx,
                 0x83,0xd9,0xff ,
                pop_es,
                pop_ds }
        };
static byte_seq C_memchr = {
                17,
              { push_es,
                jcxz, 11,
                 0x8c,0xda ,
                 0x8e,0xc2 ,
                repnz,
                scasb,
                jne, 3,
                dec_di,
                 size_prefix,0xa9 ,
                 0x89,0xcf ,
                pop_es }
        };
static byte_seq FC_memchr = {
                11,
              { jcxz, 7,
                repnz,
                scasb,
                jne, 3,
                dec_di,
                 size_prefix,0xa9 ,
                 0x89,0xcf , }
        };
static byte_seq BD_memchr = {
                13,
              { jcxz, 9,
                repnz,
                scasb,
                jne, 5,
                dec_di,
                 0x8c,0xc1 ,
                 size_prefix,0xa9 ,
                 0x89,0xcf  }
        };
static byte_seq DP_memchr = {
                17,
              { push_es,
                jcxz, 11,
                 0x8e,0xc2 ,
                repnz,
                scasb,
                jne, 5,
                dec_di,
                 0x8c,0xc1 ,
                 size_prefix,0xa9 ,
                 0x89,0xcf ,
                pop_es }
        };
static byte_seq S_strcmp = {
                16,
              { lodsb,
                 0x8a,0x27 ,
                 0x28,0xe0 ,
                jne, 5,
                inc_di,
                 0x38,0xe0 ,
                jne, -12,
                size_prefix,
                 0x18,0xe4 ,
                cwde }
        };
static byte_seq C_strcmp = {
                31,
              { push_es,
                 0x8c,0xd8 ,
                 0x8e,0xc0 ,
                cmpsb,
                jne, 17,
                dec_di,
                dec_si,
                 0x29,0xc9,0x49 ,
                xor_rr, ax_ax,
                repnz,
                scasb,
                 0xf7,0xd1 ,
                sub_rr, di_cx,
                repz,
                cmpsb,
                je,  5,
                sbb_rr, ax_ax,
                 0x83,0xd8,0xff ,
                pop_es }
        };
static byte_seq F_strcmp = {
                25,
              { cmpsb,
                jne, 17,
                dec_di,
                dec_si,
                 0x29,0xc9,0x49 ,
                xor_rr, ax_ax,
                repnz,
                scasb,
                 0xf7,0xd1 ,
                sub_rr, di_cx,
                repz,
                cmpsb,
                je,  5,
                sbb_rr, ax_ax,
                 0x83,0xd8,0xff  }
        };
static byte_seq Z_strcmp = {
                17,
              { lodsb,
                 0x26,0x8a,0x27 ,
                 0x28,0xe0 ,
                jne, 5,
                inc_di,
                 0x38,0xe0 ,
                jne, -13,
                size_prefix,
                 0x18,0xe4 ,
                cwde }
        };
static byte_seq BD_strcmp = {
                25,
              { cmpsb,
                jne, 17,
                dec_di,
                dec_si,
                 0x29,0xc9,0x49 ,
                xor_rr, ax_ax,
                repnz,
                scasb,
                 0xf7,0xd1 ,
                sub_rr, di_cx,
                repz,
                cmpsb,
                je,  5,
                sbb_rr, cx_cx,
                 0x83,0xd9,0xff  }
        };
static byte_seq DP_strcmp = {
                34,
              { push_ds,
                push_es,
                xchg_si_ax,
                 0x8e,0xd8 ,
                 0x8e,0xc1 ,
                cmpsb,
                jne, 17,
                dec_di,
                dec_si,
                xor_rr, ax_ax,
                 0x89,0xc1 ,
                dec_cx,
                repnz,
                scasb,
                 0xf7,0xd1 ,
                sub_rr, di_cx,
                repz,
                cmpsb,
                je,  5,
                sbb_rr, cx_cx,
                 0x83,0xd9,0xff ,
                pop_es,
                pop_ds }
        };
static byte_seq C_strlen = {
                 10 +6,
              { push_es,
                 0x8c,0xd8 ,
                 0x8e,0xc0 ,
                 0x29,0xc9,0x49, xor_rr, ax_ax, repnz, scasb, 0xf7,0xd1, dec_cx ,
                pop_es }
        };
static byte_seq FC_strlen = {
                 10 ,
              {  0x29,0xc9,0x49, xor_rr, ax_ax, repnz, scasb, 0xf7,0xd1, dec_cx  }
        };
static byte_seq BD_strlen = {
                10,
              {  0x29,0xc9,0x49 ,
                xor_rr, ax_ax,
                repnz,
                scasb,
                 0xf7,0xd1 ,
                dec_cx }
        };
static byte_seq DP_strlen = {
                14,
              { push_es,
                 0x8e,0xc1 ,
                xor_rr, ax_ax,
                 0x89,0xc1 ,
                dec_cx,
                repnz,
                scasb,
                 0xf7,0xd1 ,
                dec_cx,
                pop_es }
        };
static byte_seq C_abs = {
                5,
              { cwd,
                xor_rr, ax_dx,
                sub_rr, ax_dx }
        };
static byte_seq C_labs = {
                5,
              { cwd,
                xor_rr, ax_dx,
                sub_rr, ax_dx }
        };
static byte_seq C_inp = {
                3,
              { sub_rr,ax_ax,
                in_al_dx }
        };
static byte_seq C_inpw = {
                4,
              { sub_rr,ax_ax,
                size_prefix,
                in_ax_dx }
        };
static byte_seq C_inpd = {
                1,
              { in_ax_dx }
        };
static byte_seq C_outp = {
                1,
              { out_dx_al }
        };
static byte_seq C_outpw = {
                2,
              { size_prefix,
                out_dx_ax }
        };
static byte_seq C_outpd = {
                1,
              { out_dx_ax }
        };
static byte_seq C_movedata = {
                22,
              { push_ds,
                push_es,
                 0x8e,0xd8 ,
                 0x8e,0xc2 ,
                 0x89,0xc8 ,
                 0xc1,0xe9,0x02 ,
                rep,
                movsd,
                 0x8a,0xc8 ,
                 0x80,0xe1 ,3,
                rep,
                movsb,
                pop_es,
                pop_ds }
        };
static byte_seq C_enable = {
                2,
              { sti,
                cld }
        };
static byte_seq C_disable = {
                1,
              { cli }
        };
static byte_seq C_rotl = {
                2,
              { 0xd3,0xc0 }
        };
static byte_seq C_rotr = {
                2,
              { 0xd3,0xc8 }
        };
static byte_seq _8087_fabs = {
                2,
              {  0xd9,0xe1  }
        };
static byte_seq C_fabs = {
                4,
              {  0xd1,0xe2 ,
                 0xd1,0xea  }
        };
static byte_seq C_min = {
                8,
              {  0x29,0xd0 ,
                 0x19,0xc9 ,
                 0x21,0xc8 ,
                 0x01,0xd0  }
        };
static byte_seq C_max = {
                9,
              {  0x29,0xd0 ,
                cmc,
                 0x19,0xc9 ,
                 0x21,0xc8 ,
                 0x01,0xd0  }
        };
static hw_reg_set P_AX_CX[]        = {
     {+ (HW_EAX_0 )       } ,
     {+ (HW_ECX_0 )       } ,
     {+ (HW_EMPTY_0 )       }  };
static hw_reg_set P_DI_SI[]        = {
     {+ (HW_EDI_0 )       } ,
     {+ (HW_ESI_0 )       } ,
     {+ (HW_EMPTY_0 )       }  };
static hw_reg_set P_DI_SI_CX[]     = {
     {+ (HW_EDI_0 )       } ,
     {+ (HW_ESI_0 )       } ,
     {+ (HW_ECX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_SI_DI_CX[]     = {
     {+ (HW_ESI_0 )       } ,
     {+ (HW_EDI_0 )       } ,
     {+ (HW_ECX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_DI_AX_CX[]     = {
     {+ (HW_EDI_0 )       } ,
     {+ (HW_AX_0 )       } ,
     {+ (HW_ECX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_EDI_EAX_ECX[]     = {
     {+ (HW_EDI_0 )       } ,
     {+ (HW_EAX_0 )       } ,
     {+ (HW_ECX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_DI_AL_CX[]     = {
     {+ (HW_EDI_0 )       } ,
     {+ (HW_AL_0 )       } ,
     {+ (HW_ECX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_ESDI_DSSI[]    = {
     {+ (HW_ES_0+HW_EDI_0 )       } ,
     {+ (HW_DS_0+HW_ESI_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_DXEDI_CXESI_EAX[]= {
     {+ (HW_DX_0+HW_EDI_0 )       } ,
     {+ (HW_CX_0+HW_ESI_0 )       } ,
     {+ (HW_EAX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_DXESI_CXEDI_EAX[]= {
     {+ (HW_DX_0+HW_ESI_0 )       } ,
     {+ (HW_CX_0+HW_EDI_0 )       } ,
     {+ (HW_EAX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_DXEAX_CXEBX[]  = {
     {+ (HW_DX_0+HW_EAX_0 )       } ,
     {+ (HW_CX_0+HW_EBX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_ESDI_DSSI_CX[] = {
     {+ (HW_ES_0+HW_EDI_0 )       } ,
     {+ (HW_DS_0+HW_ESI_0 )       } ,
     {+ (HW_ECX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_ESDI_AL_CX[]   = {
     {+ (HW_ES_0+HW_EDI_0 )       } ,
     {+ (HW_AL_0 )       } ,
     {+ (HW_ECX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_DXEDI_AL_ECX[] = {
     {+ (HW_DX_0+HW_EDI_0 )       } ,
     {+ (HW_AL_0 )       } ,
     {+ (HW_ECX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_ESDI_AX_CX[]   = {
     {+ (HW_ES_0+HW_EDI_0 )       } ,
     {+ (HW_AX_0 )       } ,
     {+ (HW_ECX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_ESEDI_EAX_ECX[]   = {
     {+ (HW_ES_0+HW_EDI_0 )       } ,
     {+ (HW_EAX_0 )       } ,
     {+ (HW_ECX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_DXEDI_AX_CX[] = {
     {+ (HW_DX_0+HW_EDI_0 )       } ,
     {+ (HW_AX_0 )       } ,
     {+ (HW_ECX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_DXEDI_EAX_ECX[] = {
     {+ (HW_DX_0+HW_EDI_0 )       } ,
     {+ (HW_EAX_0 )       } ,
     {+ (HW_ECX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_SI_DI[]        = {
     {+ (HW_ESI_0 )       } ,
     {+ (HW_EDI_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_DSSI_ESDI[]    = {
     {+ (HW_DS_0+HW_ESI_0 )       } ,
     {+ (HW_ES_0+HW_EDI_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_SIEAX_CXEDI[]  = {
     {+ (HW_SI_0+HW_EAX_0 )       } ,
     {+ (HW_CX_0+HW_EDI_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_DSSI_ESDI_CX[] = {
     {+ (HW_DS_0+HW_ESI_0 )       } ,
     {+ (HW_ES_0+HW_EDI_0 )       } ,
     {+ (HW_ECX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_DSSI_CL[]      = {
     {+ (HW_DS_0+HW_ESI_0 )       } ,
     {+ (HW_CL_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_SI_DL[]        = {
     {+ (HW_ESI_0 )       } ,
     {+ (HW_DL_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_DI[]           = {
     {+ (HW_EDI_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_ESDI[]         = {
     {+ (HW_ES_0+HW_EDI_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_CXEDI[]        = {
     {+ (HW_CX_0+HW_EDI_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_AX[]           = {
     {+ (HW_EAX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_DX[]           = {
     {+ (HW_EDX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_DX_AL[]        = {
     {+ (HW_EDX_0 )       } ,
     {+ (HW_AL_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_DX_AX[]        = {
     {+ (HW_EDX_0 )       } ,
     {+ (HW_EAX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_AX_DX[]        = {
     {+ (HW_EAX_0 )       } ,
     {+ (HW_EDX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_AX_SI_DX_DI_CX[]= {
     {+ (HW_EAX_0 )       } ,
     {+ (HW_ESI_0 )       } ,
     {+ (HW_EDX_0 )       } ,
     {+ (HW_EDI_0 )       } ,
     {+ (HW_ECX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_EDXEAX[]       = {
     {+ (HW_EDX_0+HW_EAX_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static hw_reg_set P_8087[]         = {
     {+ (HW_FLTS_0 )       } ,
     {+ (HW_EMPTY_0 )       }
};
static byte_seq fs_push[] =
{   15
,   0x64,0xff,0x35,0x00,0x00,0x00,0x00
,   0x8f,0x00
,   0x64,0xa3,0x00,0x00,0x00,0x00
};
static byte_seq fs_pop[] =
{   6
,   0x64,0xa3,0x00,0x00,0x00,0x00
};
struct  inline_funcs Fs_Functions[] =
{   { ".FsPush", fs_push,  P_AX,  {+ (HW_EMPTY_0 ) }, {~ (HW_EMPTY_0 ) }  }
,   { ".FsPop",  fs_pop,   P_AX,  {+ (HW_EMPTY_0 ) }, {~ (HW_EAX_0 )   }  }
,   { 0 }
};

int main( void )
{
    if( sizeof( Fs_Functions ) / sizeof( Fs_Functions[0] ) != 3 ) fail( __LINE__ );
    _PASS;
}
