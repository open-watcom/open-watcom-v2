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


#ifndef HWREG_INCLUDED
#define HWREG_INCLUDED

#include "cghwreg.h"

/*       Target dependent set of hardware registers available */

HW_DEFINE_SIMPLE( HW_AH,     0x0001U, 0x0000U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_AL,     0x0002U, 0x0000U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_BH,     0x0004U, 0x0000U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_BL,     0x0008U, 0x0000U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_CH,     0x0010U, 0x0000U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_CL,     0x0020U, 0x0000U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_DH,     0x0040U, 0x0000U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_DL,     0x0080U, 0x0000U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_SI,     0x0100U, 0x0000U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_DI,     0x0200U, 0x0000U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_BP,     0x0400U, 0x0000U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_SP,     0x0800U, 0x0000U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_DS,     0x1000U, 0x0000U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_ES,     0x2000U, 0x0000U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_CS,     0x4000U, 0x0000U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_SS,     0x8000U, 0x0000U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_ST0,    0x0000U, 0x0001U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_ST1,    0x0000U, 0x0002U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_ST2,    0x0000U, 0x0004U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_ST3,    0x0000U, 0x0008U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_ST4,    0x0000U, 0x0010U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_ST5,    0x0000U, 0x0020U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_ST6,    0x0000U, 0x0040U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_ST7,    0x0000U, 0x0080U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_EAXH,   0x0000U, 0x0100U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_EBXH,   0x0000U, 0x0200U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_ECXH,   0x0000U, 0x0400U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_EDXH,   0x0000U, 0x0800U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_ESIH,   0x0000U, 0x1000U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_EDIH,   0x0000U, 0x2000U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_FS,     0x0000U, 0x4000U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_GS,     0x0000U, 0x8000U, 0x0000U, 0x0000U );
HW_DEFINE_SIMPLE( HW_FULL,   0xffffU, 0xffffU, 0xffffU, 0xffffU );
HW_DEFINE_SIMPLE( HW_UNUSED, 0x0000U, 0x0000U, 0xffffU, 0xffffU );
HW_DEFINE_SIMPLE( HW_EMPTY,  0x0000U, 0x0000U, 0x0000U, 0x0000U );


#define HW_DEFINE_COMPOUND( x ) \
enum {                                                                        \
                                                                              \
HW_AX_##x    = (hw_reg_part)(HW_AL_##x+HW_AH_##x),                       \
HW_BX_##x    = (hw_reg_part)(HW_BL_##x+HW_BH_##x),                       \
HW_CX_##x    = (hw_reg_part)(HW_CL_##x+HW_CH_##x),                       \
HW_DX_##x    = (hw_reg_part)(HW_DL_##x+HW_DH_##x),                       \
                                                                              \
                                                                              \
HW_EAX_##x   = (hw_reg_part)(HW_EAXH_##x+HW_AX_##x),                     \
HW_EBX_##x   = (hw_reg_part)(HW_EBXH_##x+HW_BX_##x),                     \
HW_ECX_##x   = (hw_reg_part)(HW_ECXH_##x+HW_CX_##x),                     \
HW_EDX_##x   = (hw_reg_part)(HW_EDXH_##x+HW_DX_##x),                     \
HW_ESI_##x   = (hw_reg_part)(HW_ESIH_##x+HW_SI_##x),                     \
HW_EDI_##x   = (hw_reg_part)(HW_EDIH_##x+HW_DI_##x),                     \
                                                                              \
HW_ABCD_##x  = (hw_reg_part)(HW_AX_##x+HW_BX_##x                         \
               +HW_CX_##x+HW_DX_##x),                                         \
                                                                              \
HW_SDSB_##x  = (hw_reg_part)(HW_SI_##x+HW_DI_##x                         \
               +HW_SP_##x+HW_BP_##x),                                         \
                                                                              \
HW_SEGS_##x  = (hw_reg_part)(HW_DS_##x+HW_ES_##x+HW_CS_##x               \
               +HW_SS_##x+HW_FS_##x+HW_GS_##x),                               \
                                                                              \
HW_IDX16_##x  = (hw_reg_part)(HW_SI_##x+HW_DI_##x+HW_BX_##x),            \
                                                                              \
HW_FLTS_##x  = (hw_reg_part)(HW_ST0_##x+HW_ST1_##x                       \
               +HW_ST2_##x+HW_ST3_##x                                         \
               +HW_ST4_##x+HW_ST5_##x+HW_ST6_##x+HW_ST7_##x),                 \
                                                                              \
HW_FLTS_NOT_ST0_##x  = (hw_reg_part)(HW_ST1_##x                          \
               +HW_ST2_##x+HW_ST3_##x                                         \
               +HW_ST4_##x+HW_ST5_##x+HW_ST6_##x+HW_ST7_##x),                 \
                                                                              \
HW_32_##x    = (hw_reg_part)(HW_EAXH_##x+HW_EBXH_##x                     \
               +HW_ECXH_##x+HW_EDXH_##x+HW_ESIH_##x+HW_EDIH_##x),             \
                                                                              \
HW_DX_AX_##x   = (hw_reg_part)(HW_DX_##x+HW_AX_##x),                     \
HW_CX_BX_##x   = (hw_reg_part)(HW_CX_##x+HW_BX_##x),                     \
HW_CX_DI_##x   = (hw_reg_part)(HW_CX_##x+HW_DI_##x),                     \
HW_BX_SI_##x   = (hw_reg_part)(HW_BX_##x+HW_SI_##x),                     \
HW_BX_DI_##x   = (hw_reg_part)(HW_BX_##x+HW_DI_##x),                     \
HW_BP_SI_##x   = (hw_reg_part)(HW_BP_##x+HW_SI_##x),                     \
HW_BP_DI_##x   = (hw_reg_part)(HW_BP_##x+HW_DI_##x),                     \
HW_DS_BX_##x   = (hw_reg_part)(HW_DS_##x+HW_BX_##x),                     \
HW_DS_SI_##x   = (hw_reg_part)(HW_DS_##x+HW_SI_##x),                     \
HW_DS_ESI_##x  = (hw_reg_part)(HW_DS_##x+HW_ESI_##x),                    \
HW_DS_DI_##x   = (hw_reg_part)(HW_DS_##x+HW_DI_##x),                     \
HW_SS_BX_##x   = (hw_reg_part)(HW_SS_##x+HW_BX_##x),                     \
HW_SS_SI_##x   = (hw_reg_part)(HW_SS_##x+HW_SI_##x),                     \
HW_SS_DI_##x   = (hw_reg_part)(HW_SS_##x+HW_DI_##x),                     \
HW_ES_BX_##x   = (hw_reg_part)(HW_ES_##x+HW_BX_##x),                     \
HW_ES_SI_##x   = (hw_reg_part)(HW_ES_##x+HW_SI_##x),                     \
HW_ES_DI_##x   = (hw_reg_part)(HW_ES_##x+HW_DI_##x),                     \
HW_ES_EDI_##x  = (hw_reg_part)(HW_ES_##x+HW_EDI_##x),                    \
HW_ECX_ESI_##x = (hw_reg_part)(HW_ECX_##x+HW_ESI_##x),                   \
HW_ECX_EDI_##x = (hw_reg_part)(HW_ECX_##x+HW_EDI_##x),                   \
HW_CS_EDI_##x  = (hw_reg_part)(HW_CS_##x+HW_EDI_##x),                    \
HW_FS_GS_##x   = (hw_reg_part)(HW_FS_##x+HW_GS_##x),                     \
HW_DS_GS_##x   = (hw_reg_part)(HW_DS_##x+HW_GS_##x),                     \
HW_ECX_EBX_##x = (hw_reg_part)(HW_ECX_##x+HW_EBX_##x),                   \
HW_32_BP_SP_##x= (hw_reg_part)(HW_32_##x+HW_BP_##x+HW_SP_##x),           \
HW_DS_ES_FS_GS_##x = (hw_reg_part)(HW_DS_##x+HW_ES_##x+                  \
                                        HW_FS_##x+HW_GS_##x),                 \
HW_DS_ES_SS_CS_##x = (hw_reg_part)(HW_DS_##x+HW_ES_##x+                  \
                                        HW_SS_##x+HW_CS_##x),                 \
HW_AL_BL_CL_DL_##x = (hw_reg_part)(HW_AL_##x+HW_BL_##x+                  \
                                        HW_CL_##x+HW_DL_##x),                 \
HW_AH_BH_CH_DH_##x = (hw_reg_part)(HW_AH_##x+HW_BH_##x+                  \
                                        HW_CH_##x+HW_DH_##x),                 \
HW_AX_BX_CX_DX_SI_DI_##x = (hw_reg_part)(HW_AX_##x+HW_BX_##x             \
                       +HW_CX_##x+HW_DX_##x+HW_SI_##x+HW_DI_##x),   \
                                                                              \
HW__COMPOUND_END_##x                                                          \
}

HW_ITER( HW_DEFINE_COMPOUND );

#if !defined( BY_C_FRONT_END ) && !defined( BY_CPP_FRONT_END ) \
  && !defined( BY_FORTRAN_FRONT_END )
HW_DEFINE_GLOBAL_CONST( HW_EMPTY );
HW_DEFINE_GLOBAL_CONST( HW_DS );
HW_DEFINE_GLOBAL_CONST( HW_DS_ESI );
HW_DEFINE_GLOBAL_CONST( HW_DS_SI );
HW_DEFINE_GLOBAL_CONST( HW_DS_DI );
HW_DEFINE_GLOBAL_CONST( HW_DS_BX );
HW_DEFINE_GLOBAL_CONST( HW_SS );
HW_DEFINE_GLOBAL_CONST( HW_SS_SI );
HW_DEFINE_GLOBAL_CONST( HW_SS_DI );
HW_DEFINE_GLOBAL_CONST( HW_SS_BX );
HW_DEFINE_GLOBAL_CONST( HW_ES );
HW_DEFINE_GLOBAL_CONST( HW_ES_SI );
HW_DEFINE_GLOBAL_CONST( HW_ES_EDI );
HW_DEFINE_GLOBAL_CONST( HW_ES_DI );
HW_DEFINE_GLOBAL_CONST( HW_ES_BX );
HW_DEFINE_GLOBAL_CONST( HW_FS );
HW_DEFINE_GLOBAL_CONST( HW_GS );
HW_DEFINE_GLOBAL_CONST( HW_CS );
HW_DEFINE_GLOBAL_CONST( HW_SP );
HW_DEFINE_GLOBAL_CONST( HW_BP );
HW_DEFINE_GLOBAL_CONST( HW_EAX );
HW_DEFINE_GLOBAL_CONST( HW_AX );
HW_DEFINE_GLOBAL_CONST( HW_AL );
HW_DEFINE_GLOBAL_CONST( HW_AH );
HW_DEFINE_GLOBAL_CONST( HW_ECX );
HW_DEFINE_GLOBAL_CONST( HW_CX );
HW_DEFINE_GLOBAL_CONST( HW_CL );
HW_DEFINE_GLOBAL_CONST( HW_CH );
HW_DEFINE_GLOBAL_CONST( HW_EDX );
HW_DEFINE_GLOBAL_CONST( HW_DX );
HW_DEFINE_GLOBAL_CONST( HW_DL );
HW_DEFINE_GLOBAL_CONST( HW_DH );
HW_DEFINE_GLOBAL_CONST( HW_EBX );
HW_DEFINE_GLOBAL_CONST( HW_BX );
HW_DEFINE_GLOBAL_CONST( HW_BL );
HW_DEFINE_GLOBAL_CONST( HW_BH );
HW_DEFINE_GLOBAL_CONST( HW_ESI );
HW_DEFINE_GLOBAL_CONST( HW_BP_DI );
HW_DEFINE_GLOBAL_CONST( HW_CX_DI );
HW_DEFINE_GLOBAL_CONST( HW_SI );
HW_DEFINE_GLOBAL_CONST( HW_EDI );
HW_DEFINE_GLOBAL_CONST( HW_BP_SI );
HW_DEFINE_GLOBAL_CONST( HW_DI );
HW_DEFINE_GLOBAL_CONST( HW_ST0 );
HW_DEFINE_GLOBAL_CONST( HW_ST1 );
HW_DEFINE_GLOBAL_CONST( HW_ST2 );
HW_DEFINE_GLOBAL_CONST( HW_ST3 );
HW_DEFINE_GLOBAL_CONST( HW_ST4 );
HW_DEFINE_GLOBAL_CONST( HW_ST5 );
HW_DEFINE_GLOBAL_CONST( HW_ST6 );
HW_DEFINE_GLOBAL_CONST( HW_ST7 );
#endif

#define MAX_POSSIBLE_REG        8

#endif

#define _AL     HW_AL
#define _BL     HW_BL
#define _CL     HW_CL
#define _DL     HW_DL
