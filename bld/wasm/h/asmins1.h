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


#ifdef _WASM_

    #include "asmops1.h"
    #ifndef   asm_op
        #include "asmops2.h"
    #endif

    struct asm_ins {
        unsigned short  token;                  /* T_ADD, etc */
        unsigned        allowed_prefix  : 2,    /* allowed prefix */
                        byte1_info      : 2,    /* flags for 1st byte */
                        rm_info         : 2;    /* info on r/m byte */
        enum asm_cpu    cpu;                    /* CPU type */
        unsigned long   opnd_type[2];           /* asm_opnds */
        unsigned char   opcode;                 /* opcode byte */
        unsigned char   rm_byte;                /* mod_rm_byte */
    };
#else
struct asm_ins {
        unsigned        token           : 10,   /* T_ADD, etc */

                        allowed_prefix  : 2,    /* allowed prefix */
                        byte1_info      : 2,    /* flags for 1st byte */
                        rm_info         : 2,    /* info on r/m byte */
                        cpu             : 8;    /* CPU type */
        unsigned long   opnd_type[2];           /* asm_opnds */
        unsigned char   opcode;                 /* opcode byte */
        unsigned char   rm_byte;                /* mod_rm_byte */
};
#endif

struct asm_code {
        signed short    prefix;         // prefix before instruction, e.g. lock
        // #ifdef _WASM_
        #if 0
            enum asm_token  mem_type;   /* byte / word / etc. NOT near/far */
        #else
            signed short    mem_type;   /* byte / word / etc. NOT near/far */
        #endif
        long            data[2];
        struct asm_ins  info;
        signed char     seg_prefix;
        signed char     extended_ins;
        signed char     adrsiz;
        signed char     opsiz;
        unsigned char   sib;
        unsigned char   use32;
        signed short    distance;       /* short / near / far / empty */
        unsigned        mem_type_fixed:1;
};

#define NO_PREFIX   0x00
#define LOCK        0x01
#define REPxx       0x02
#define FWAIT       0x03

#define PREFIX_ES       0x26
#define PREFIX_CS       0x2E
#define PREFIX_SS       0x36
#define PREFIX_DS       0x3E
#define PREFIX_FS       0x64
#define PREFIX_GS       0x65

#define F_0F        0x2
#define F_16        0x1
#define F_32        0x3

#define no_RM       0x1
#define no_WDS      0x2
#define R_in_OP     0x3

#define IREG    1       /* indicates INDEX register for an OP_REGISTER entry*/

/* Note on the byte_1_info
   10 ( + F_0F ) -> the first byte is 0x0F, follow by opcode and rm_byte
   01 ( + F_16 ) -> the first byte is OPSIZ prefix if in use32 segment
   11 ( + F_32 ) -> the first byte is OPSIZ prefix if in use16 segment
   00            -> the first byte is opcode, follow by rm_byte      */

/* Note on the rm_info:
   01 ( + no_RM   ) -> no rm_byte
   10 ( + no_WDS  ) -> has rm_byte, but w-bit, d-bit, s-bit of opcode are absent
   11 ( + R_in_OP ) -> no rm_byte, reg field is included in opcode
   00               -> has rm_byte with w-, d- and/or s-bit in opcode  */

/* NOTE: The order of table is IMPORTANT !! */
/* OP_A should put before OP_R16 & OP_R
   OP_R16   "     "    "    OP_R
   OP_I8    "     "    "    OP_I
   OP_M ( without extension ) should follow OP_M_x
   OP_R ( without extension ) should follow OP_Rx
   OP_I ( without extension ) should follow OP_Ix  */

