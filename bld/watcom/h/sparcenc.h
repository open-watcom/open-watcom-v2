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
* Description:  Instruction encoding for 32-bit SPARC v8 architecture.
*
*
****************************************************************************/


typedef union {
    unsigned_32 full;
    struct {
        unsigned_32 disp        : 30;
        unsigned_32 opcode      : 2;
    }           call;
    struct {
        unsigned_32 rs2         : 5;
        unsigned_32 asi         : 8;
        unsigned_32 imm         : 1;
        unsigned_32 rs1         : 5;
        unsigned_32 opcode_3    : 6;
        unsigned_32 rd          : 5;
        unsigned_32 opcode      : 2;
    } op3;
    struct {
        unsigned_32 simm13      : 13;
        unsigned_32 imm         : 1;
        unsigned_32 rs1         : 5;
        unsigned_32 opcode_3    : 6;
        unsigned_32 rd          : 5;
        unsigned_32 opcode      : 2;
    } op3imm;
    struct {
        unsigned_32 rs2         : 5;
        unsigned_32 opf         : 9;
        unsigned_32 rs1         : 5;
        unsigned_32 opcode_3    : 6;
        unsigned_32 rd          : 5;
        unsigned_32 opcode      : 2;
    } op3opf;
    struct {
        unsigned_32 imm22       : 22;
        unsigned_32 op2         : 3;
        unsigned_32 rd          : 5;
        unsigned_32 opcode      : 2;
    } sethi;
    struct {
        unsigned_32 disp22      : 22;
        unsigned_32 op2         : 3;
        unsigned_32 cond        : 4;
        unsigned_32 anul        : 1;
        unsigned_32 opcode      : 2;
    } branch;
} sparc_ins;

typedef uint_32         opcode;
typedef uint_32         reg_index;

