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


#pragma pack(1);                /* make sure no structures are padded. */

#ifdef OVL_SMALL
typedef struct vector {               /* short overlay vector */
    unsigned_8  call_op;
    unsigned_16 ldr_addr;
    unsigned_16 sec_num;
    unsigned_8  jmp_op;
    unsigned_16 target;
} vector;
typedef vector * vector_ptr;

#else

#define CS_OVERRIDE 0x2E
#define INC_OPCODE 0x6FE

typedef struct {
    unsigned_8  cs_over;
    unsigned_16 inc_op;
    unsigned_16 tab_addr;
} incvector;

#define MOV_AX_AX   0xC089          /* opcode for MOV AX,AX */
#define TEST_OPCODE 0xA9            /* opcode for TEST AX,word */
                                    /* used for hiding the section number */
typedef struct {
    unsigned_16 big_nop;
    unsigned_8  test_op;
    unsigned_16 sec_num;
} mungedvector;

typedef struct {
    unsigned_8  call_op;
    unsigned_16 ldr_addr;
    unsigned_16 sec_num;
} plainvector;

typedef struct vector {               /* long overlay vector */
    union {
        plainvector     v;
        mungedvector    m;
        incvector       i;
    } u;
    unsigned_8          jmp_op;
    dos_addr            target;
} vector;

typedef vector far * vector_ptr;

#endif
