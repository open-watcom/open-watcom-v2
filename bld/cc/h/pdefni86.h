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


#define NC '\0'

hw_reg_set DefaultParms[] = {
    HW_D_4( HW_AX, HW_BX, HW_CX, HW_DX ) /*+HW_ST1+HW_ST2+HW_ST3+HW_ST4*/,
    HW_D( HW_EMPTY )
};

hw_reg_set DefaultVarParms[] = {
    HW_D( HW_EMPTY )
};

#define REGS_MAP \
REG_PICK( "es",  0  ) \
REG_PICK( "ds",  1  ) \
REG_PICK( "cs",  2  ) \
REG_PICK( "ss",  3  ) \
REG_PICK( "ax",  4  ) \
REG_PICK( "bx",  5  ) \
REG_PICK( "cx",  6  ) \
REG_PICK( "dx",  7  ) \
REG_PICK( "al",  8  ) \
REG_PICK( "bl",  9  ) \
REG_PICK( "cl",  10 ) \
REG_PICK( "dl",  11 ) \
REG_PICK( "ah",  12 ) \
REG_PICK( "bh",  13 ) \
REG_PICK( "ch",  14 ) \
REG_PICK( "dh",  15 ) \
REG_PICK( "si",  16 ) \
REG_PICK( "di",  17 ) \
REG_PICK( "bp",  18 ) \
REG_PICK( "sp",  19 ) \
REG_PICK( "fs",  20 ) \
REG_PICK( "gs",  21 )

char Registers[] = {
    #define REG_PICK(t,r) t "\0"
    REGS_MAP
    "\0"
    #undef REG_PICK
};

unsigned char RegMap[] = {
    #define REG_PICK(t,r) r,
    REGS_MAP
    #undef REG_PICK
};


hw_reg_set RegBits[] = {
    HW_D( HW_ES ),
    HW_D( HW_DS ),
    HW_D( HW_CS ),
    HW_D( HW_SS ),
    HW_D( HW_AX ),
    HW_D( HW_BX ),
    HW_D( HW_CX ),
    HW_D( HW_DX ),
    HW_D( HW_AL ),
    HW_D( HW_BL ),
    HW_D( HW_CL ),
    HW_D( HW_DL ),
    HW_D( HW_AH ),
    HW_D( HW_BH ),
    HW_D( HW_CH ),
    HW_D( HW_DH ),
    HW_D( HW_SI ),
    HW_D( HW_DI ),
    HW_D( HW_BP ),
    HW_D( HW_SP ),
    HW_D( HW_FS ),
    HW_D( HW_GS ),
};

/*      INLINE FUNCTIONS */

aux_info        InlineInfo;


/*
  here come the code bursts ...
*/
/****************************************************************/
#include "codei86.gh"

#define DP_stosw_ret    DF_stosw_ret
#define DP_stosw_parms  DF_stosw_parms
#define DP_stosw_saves  DF_stosw_saves
#define DP_stosw        DF_stosw

#define C_stosw_ret     DF_stosw_ret
#define C_stosw_parms   DF_stosw_parms
#define C_stosw_saves   DF_stosw_saves
#define C_stosw         DF_stosw

#define C_stoswb_ret    DF_stoswb_ret
#define C_stoswb_parms  DF_stoswb_parms
#define C_stoswb_saves  DF_stoswb_saves
#define C_stoswb        DF_stoswb

#define DP_stoswb_ret   DF_stoswb_ret
#define DP_stoswb_parms DF_stoswb_parms
#define DP_stoswb_saves DF_stoswb_saves
#define DP_stoswb       DF_stoswb

#define C_8087_fabs_ret    HW_D( HW_EMPTY )
#define C_8087_fabs_parms  P_8087
#define C_8087_fabs_saves  HW_NotD( HW_EMPTY )

static struct STRUCT_byte_seq( 5 ) _8087_fabs = {
    5, true,
    {
        FLOATING_FIXUP_BYTE,     /* 8087 fixup indicator */
        FIX_FPP_NORMAL,          /* FP patch type */
        0x9b,                    /* fwait */
        0xd9, 0xe1               /* fabs  */
    }
};


/****************************************************************/

static hw_reg_set P_AX_CX[] = {
    HW_D( HW_AX ),
    HW_D( HW_CX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_AX_DX[] = {
    HW_D( HW_AX ),
    HW_D( HW_DX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DI_SI[] = {
    HW_D( HW_DI ),
    HW_D( HW_SI ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DI_SI_CX[] = {
    HW_D( HW_DI ),
    HW_D( HW_SI ),
    HW_D( HW_CX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_ESDI_AX_CX[] = {
    HW_D_2( HW_ES, HW_DI ),
    HW_D( HW_AX ),
    HW_D( HW_CX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_SI_DI_CX[] = {
    HW_D( HW_SI ),
    HW_D( HW_DI ),
    HW_D( HW_CX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DI_AL_CX[] = {
    HW_D( HW_DI ),
    HW_D( HW_AL ),
    HW_D( HW_CX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_ESDI_SIAX[] = {
    HW_D_2( HW_ES, HW_DI ),
    HW_D_2( HW_SI, HW_AX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_ESDI_DSSI[] = {
    HW_D_2( HW_ES, HW_DI ),
    HW_D_2( HW_DS, HW_SI ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_ESDI_CXSI_AX[] = {
    HW_D_2( HW_ES, HW_DI ),
    HW_D_2( HW_CX, HW_SI ),
    HW_D( HW_AX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_ESDI_DSSI_CX[] = {
    HW_D_2( HW_ES, HW_DI ),
    HW_D_2( HW_DS, HW_SI ),
    HW_D( HW_CX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_ESDI_AL_CX[] = {
    HW_D_2( HW_ES, HW_DI ),
    HW_D( HW_AL ),
    HW_D( HW_CX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_SI_DI[] = {
    HW_D( HW_SI ),
    HW_D( HW_DI ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_SIAX_ESDI[] = {
    HW_D_2( HW_SI, HW_AX ),
    HW_D_2( HW_ES, HW_DI ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DSSI_ESDI[] = {
    HW_D_2( HW_DS, HW_SI ),
    HW_D_2( HW_ES, HW_DI ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_CXSI_ESDI_AX[] = {
    HW_D_2( HW_CX, HW_SI ),
    HW_D_2( HW_ES, HW_DI ),
    HW_D( HW_AX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DSSI_ESDI_CX[] = {
    HW_D_2( HW_DS, HW_SI ),
    HW_D_2( HW_ES, HW_DI ),
    HW_D( HW_CX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DXSI_CL[] = {
    HW_D_2( HW_DX, HW_SI ),
    HW_D( HW_CL ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DSSI_CL[] = {
    HW_D_2( HW_DS, HW_SI ),
    HW_D( HW_CL ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_SI_DL[] = {
    HW_D( HW_SI ),
    HW_D( HW_DL ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DI[] = {
    HW_D( HW_DI ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_ESDI[] = {
    HW_D_2( HW_ES, HW_DI ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_AX[] = {
    HW_D( HW_AX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DX[] = {
    HW_D( HW_DX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DX_AL[] = {
    HW_D( HW_DX ),
    HW_D( HW_AL ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DX_AX[] = {
    HW_D( HW_DX ),
    HW_D( HW_AX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_AXDX[] = {
    HW_D_2( HW_AX, HW_DX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_AX_SI_ES_DI_CX[] = {
    HW_D( HW_AX ),
    HW_D( HW_SI ),
    HW_D( HW_ES ),
    HW_D( HW_DI ),
    HW_D( HW_CX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_AXBXCXDX[] = {
    HW_D( HW_ABCD ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_8087[] = {
    HW_D( HW_FLTS ),
    HW_D( HW_EMPTY )
};


const inline_funcs SInline_Functions[] = {
    { "strcpy", (byte_seq *)&S_strcpy, S_strcpy_parms, S_strcpy_ret, S_strcpy_saves },
    { "strcmp", (byte_seq *)&S_strcmp, S_strcmp_parms, S_strcmp_ret, S_strcmp_saves },
    { "strcat", (byte_seq *)&S_strcat, S_strcat_parms, S_strcat_ret, S_strcat_saves },
    { "memset", (byte_seq *)&S_memset, S_memset_parms, S_memset_ret, S_memset_saves },
    { "memcpy", (byte_seq *)&S_memcpy, S_memcpy_parms, S_memcpy_ret, S_memcpy_saves },
    {"_fstrcpy",(byte_seq *)&ZP_strcpy, ZP_strcpy_parms, ZP_strcpy_ret, ZP_strcpy_saves},
    { NULL }
};

const inline_funcs Inline_Functions[] = {
    { "strlen", (byte_seq *)&C_strlen, C_strlen_parms, C_strlen_ret, C_strlen_saves },
    { "strcpy", (byte_seq *)&C_strcpy, C_strcpy_parms, C_strcpy_ret, C_strcpy_saves },
    { "strcmp", (byte_seq *)&C_strcmp, C_strcmp_parms, C_strcmp_ret, C_strcmp_saves },
    { "strcat", (byte_seq *)&C_strcat, C_strcat_parms, C_strcat_ret, C_strcat_saves },
    { "strchr", (byte_seq *)&C_strchr, C_strchr_parms, C_strchr_ret, C_strchr_saves },
    { "memcpy", (byte_seq *)&C_memcpy, C_memcpy_parms, C_memcpy_ret, C_memcpy_saves },
    { "memcmp", (byte_seq *)&C_memcmp, C_memcmp_parms, C_memcmp_ret, C_memcmp_saves },
    { "memset", (byte_seq *)&C_memset, C_memset_parms, C_memset_ret, C_memset_saves },
    { "memchr", (byte_seq *)&C_memchr, C_memchr_parms, C_memchr_ret, C_memchr_saves },
    { ".stosw", (byte_seq *)&C_stosw,  C_stosw_parms,  C_stosw_ret, C_stosw_saves   },
    { ".stoswb",(byte_seq *)&C_stoswb, C_stoswb_parms, C_stoswb_ret, C_stoswb_saves },
    {"_fstrcpy",(byte_seq *)&DP_strcpy, DP_strcpy_parms, DP_strcpy_ret, DP_strcpy_saves},
    { NULL }
};

const inline_funcs ZF_Data_Functions[] = {
    { "strcpy", (byte_seq *)&ZF_strcpy, ZF_strcpy_parms, ZF_strcpy_ret, ZF_strcpy_saves },
    { "strcmp", (byte_seq *)&ZF_strcmp, ZF_strcmp_parms, ZF_strcmp_ret, ZF_strcmp_saves },
    { "strcat", (byte_seq *)&ZF_strcat, ZF_strcat_parms, ZF_strcat_ret, ZF_strcat_saves },
    { "memset", (byte_seq *)&ZF_memset, ZF_memset_parms, ZF_memset_ret, ZF_memset_saves },
    { "memcpy", (byte_seq *)&ZF_memcpy, ZF_memcpy_parms, ZF_memcpy_ret, ZF_memcpy_saves },
    { NULL }
};


const inline_funcs ZP_Data_Functions[] = {
    { "strcpy", (byte_seq *)&ZP_strcpy, ZP_strcpy_parms, ZP_strcpy_ret, ZP_strcpy_saves },
    { "strcmp", (byte_seq *)&ZP_strcmp, ZP_strcmp_parms, ZP_strcmp_ret, ZP_strcmp_saves },
    { "strcat", (byte_seq *)&ZP_strcat, ZP_strcat_parms, ZP_strcat_ret, ZP_strcat_saves },
    { "memset", (byte_seq *)&ZP_memset, ZP_memset_parms, ZP_memset_ret, ZP_memset_saves },
    { "memcpy", (byte_seq *)&ZP_memcpy, ZP_memcpy_parms, ZP_memcpy_ret, ZP_memcpy_saves },
    {"_fstrcpy",(byte_seq *)&ZP_strcpy, ZP_strcpy_parms, ZP_strcpy_ret, ZP_strcpy_saves },
    { NULL }
};

const inline_funcs DF_Data_Functions[] = {
    { "strlen", (byte_seq *)&DF_strlen, DF_strlen_parms, DF_strlen_ret, DF_strlen_saves },
    { "strcpy", (byte_seq *)&DF_strcpy, DF_strcpy_parms, DF_strcpy_ret, DF_strcpy_saves },
    { "strcmp", (byte_seq *)&DF_strcmp, DF_strcmp_parms, DF_strcmp_ret, DF_strcmp_saves },
    { "strcat", (byte_seq *)&DF_strcat, DF_strcat_parms, DF_strcat_ret, DF_strcat_saves },
    { "strchr", (byte_seq *)&DF_strchr, DF_strchr_parms, DF_strchr_ret, DF_strchr_saves },
    { "memcpy", (byte_seq *)&DF_memcpy, DF_memcpy_parms, DF_memcpy_ret, DF_memcpy_saves },
    { "memcmp", (byte_seq *)&DF_memcmp, DF_memcmp_parms, DF_memcmp_ret, DF_memcmp_saves },
    { "memset", (byte_seq *)&DF_memset, DF_memset_parms, DF_memset_ret, DF_memset_saves },
    { "memchr", (byte_seq *)&DF_memchr, DF_memchr_parms, DF_memchr_ret, DF_memchr_saves },
    { ".stosw", (byte_seq *)&DF_stosw,  DF_stosw_parms,  DF_stosw_ret,  DF_stosw_saves  },
    { ".stoswb",(byte_seq *)&DF_stoswb, DF_stoswb_parms, DF_stoswb_ret, DF_stoswb_saves },
    { NULL }
 };

const inline_funcs DP_Data_Functions[] = {
    { "strlen", (byte_seq *)&DP_strlen, DP_strlen_parms, DP_strlen_ret, DP_strlen_saves },
    { "strcpy", (byte_seq *)&DP_strcpy, DP_strcpy_parms, DP_strcpy_ret, DP_strcpy_saves },
    { "strcmp", (byte_seq *)&DP_strcmp, DP_strcmp_parms, DP_strcmp_ret, DP_strcmp_saves },
    { "strcat", (byte_seq *)&DP_strcat, DP_strcat_parms, DP_strcat_ret, DP_strcat_saves },
    { "strchr", (byte_seq *)&DP_strchr, DP_strchr_parms, DP_strchr_ret, DP_strchr_saves },
    { "memcpy", (byte_seq *)&DP_memcpy, DP_memcpy_parms, DP_memcpy_ret, DP_memcpy_saves },
    { "memcmp", (byte_seq *)&DP_memcmp, DP_memcmp_parms, DP_memcmp_ret, DP_memcmp_saves },
    { "memset", (byte_seq *)&DP_memset, DP_memset_parms, DP_memset_ret, DP_memset_saves },
    { "memchr", (byte_seq *)&DP_memchr, DP_memchr_parms, DP_memchr_ret, DP_memchr_saves },
    { ".stosw", (byte_seq *)&DP_stosw,  DP_stosw_parms,  DP_stosw_ret,  DP_stosw_saves  },
    { ".stoswb",(byte_seq *)&DP_stoswb, DP_stoswb_parms, DP_stoswb_ret, DP_stoswb_saves },
    { NULL }
 };

const inline_funcs _8087_Functions[] = {
    { "fabs", (byte_seq *)&_8087_fabs, C_8087_fabs_parms, C_8087_fabs_ret, C_8087_fabs_saves  },
    { NULL }
};


/* memory model independent functions */

const inline_funcs Common_Functions[] = {
    { "abs",     (byte_seq *)&C_abs,     C_abs_parms,     C_abs_ret,     C_abs_saves     },
    { "labs",    (byte_seq *)&C_labs,    C_labs_parms,    C_labs_ret,    C_labs_saves    },
    { "fabs",    (byte_seq *)&C_fabs,    C_fabs_parms,    C_fabs_ret,    C_fabs_saves    },
    { "inp",     (byte_seq *)&C_inp,     C_inp_parms,     C_inp_ret,     C_inp_saves     },
    { "inpw",    (byte_seq *)&C_inpw,    C_inpw_parms,    C_inpw_ret,    C_inpw_saves    },
    { "outp",    (byte_seq *)&C_outp,    C_outp_parms,    C_outp_ret,    C_outp_saves    },
    { "outpw",   (byte_seq *)&C_outpw,   C_outpw_parms,   C_outpw_ret,   C_outpw_saves   },
    { "movedata",(byte_seq *)&C_movedata,C_movedata_parms,C_movedata_ret,C_movedata_saves},
    { "_enable", (byte_seq *)&C_enable,  C_enable_parms,  C_enable_ret,  C_enable_saves  },
    { "_disable",(byte_seq *)&C_disable, C_disable_parms, C_disable_ret, C_disable_saves },
    { "_rotl",   (byte_seq *)&C_rotl,    C_rotl_parms,    C_rotl_ret,    C_rotl_saves    },
    { "_rotr",   (byte_seq *)&C_rotr,    C_rotr_parms,    C_rotr_ret,    C_rotr_saves    },
    { "div",     (byte_seq *)&C_div,     C_div_parms,     C_div_ret,     C_div_saves     },
    { "_fmemcpy",(byte_seq *)&DP_memcpy, DP_memcpy_parms, DP_memcpy_ret, DP_memcpy_saves },
    { "_fmemcmp",(byte_seq *)&DP_memcmp, DP_memcmp_parms, DP_memcmp_ret, DP_memcmp_saves },
    { "_fmemset",(byte_seq *)&DP_memset, DP_memset_parms, DP_memset_ret, DP_memset_saves },
    { "_fmemchr",(byte_seq *)&DP_memchr, DP_memchr_parms, DP_memchr_ret, DP_memchr_saves },
    { "_fstrcpy",(byte_seq *)&DP_strcpy, DP_strcpy_parms, DP_strcpy_ret, DP_strcpy_saves },
    { "_fstrcat",(byte_seq *)&DP_strcat, DP_strcat_parms, DP_strcat_ret, DP_strcat_saves },
    { "_fstrcmp",(byte_seq *)&DP_strcmp, DP_strcmp_parms, DP_strcmp_ret, DP_strcmp_saves },
    { "_fstrlen",(byte_seq *)&DP_strlen, DP_strlen_parms, DP_strlen_ret, DP_strlen_saves },
    { ".min",    (byte_seq *)&C_min,     C_min_parms,     C_min_ret,     C_min_saves     },
    { ".max",    (byte_seq *)&C_max,     C_max_parms,     C_max_ret,     C_max_saves     },
    { NULL }
};
