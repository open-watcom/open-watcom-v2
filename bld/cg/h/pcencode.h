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


#include <jumps.h>
#define _Code   Format( OC_CODE );
#define _Next   EjectInst()
#define _Emit   Finalize()

extern void     EjectInst(void);
extern void     Finalize(void);
extern void     Format(oc_class);

#define M_PUSHATBP      0x76ff
#define M_PUSH          0x50
#define M_PUSHI         0x68
#define M_POP           0x58
#define M_MOVRR         0xc088
#define M_XORRR         0xc030
#define M_NEGR          0xd8f6
#define M_SUBRC         0xe880
#define M_ANDRC         0xe080
#define M_ADDRC         0xc080
#define M_SBB           0x1880
#define M_TESTRR        0xc084
#define M_NEG           0x18f6
#define M_NOT           0x10f6
#define M_SHR1          0x28d0
#define M_SHL1          0x20d0
#define M_LEA           0x8d
#define M_PUSHSEG       0x06
#define M_PUSHFS        0xA00F
#define M_PUSHGS        0xA80F
#define M_POPSEG        0x07
#define M_POPFS         0xA10F
#define M_POPGS         0xA90F
#define M_RCR1          0x18d0
#define M_RCL1          0x10d0
#define M_SEGOVER       0x26
#define M_SEGFS         0x64
#define M_SEGGS         0x65
#define M_OPND_SIZE     0x66
#define M_ADDR_SIZE     0x67
#define M_MOVSB         0xa4
#define M_MOVSW         0xa5
#define M_CMPSB         0xa6
#define M_CMPSW         0xa7
#define M_REPNE         0xf2
#define M_REPE          0xf3
#define M_CBW           0x98
#define M_CWD           0x99
#define M_LOOP          0xe2
#define M_MOVSX         0xbe
#define M_MOVZX         0xb6
#define M_SECONDARY     0x0f
#define M_386MUL        0xaf
#define M_SETCC         0x90

/* structures and shift positions for instructions*/

#define S_KEY_REG       0           /* bit position*/
#define S_KEY_SR        3                    /* found in segment override*/
#define S_RMR_MOD       6
#define S_RMR_SR        3
#define S_RMR_REG       3
#define S_RMR_RM        0
#define RMR_MOD_REG     0xc0       /* mask on RMR bit*/
#define B_RMR_MUL_SGN   8               /* signed multiply/divid*/
#define B_RMR_SHR_SAR   16
#define B_KEY_W 1             /* bit mask*/
#define B_KEY_AW        8
#define B_KEY_S 2
#define B_KEY_DS        1
#define B_KEY_FS        0x10
#define B_KEY_POPSEG    1
#define B_RMR_FMT_I8    0x28

#define MF_FS   0x00
#define MF_FD   0x04
#define MF_I2   0x06
#define MF_I4   0x02
#define MF_I8   0x06

#define KEY     0
#define RMR     1
#define DATAHI  2
#define DATALO  1

#define INSSIZE         128

typedef struct template {
        oc_header       oc;
        byte            data[ INSSIZE ];
} template;
