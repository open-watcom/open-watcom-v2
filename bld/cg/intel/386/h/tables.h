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


typedef enum {
        NO,
        ADD1,
        ADD2,
        ADD4,
        ADD8,
        EADD,
        ADDCP,
        SUB1,
        SUB2,
        SUB4,
        SUB8,
        ESUB,
        SUBCP,
        BIN8,
        OR1,
        OR2,
        OR4,
        AND1,
        AND2,
        AND4,
        MUL1,
        MUL2,
        MUL4,
        DIV1,
        DIV2,
        DIV4,
        MOD1,
        MOD2,
        MOD4,
        SHFT1,
        SHFT2,
        SHFT4,
        TEST1,
        TEST2,
        TEST4,
        TEST8,
        CMP1,
        CMP2,
        CMP4,
        CMP8,
        CMPCP,
        CMPF,
        CMPD,
        CMPL,
        MOV1,
        MOV2,
        MOV4,
        MOVCP,
        MOV8,
        MOVX,
        LACP,
        LA,
        NOT1,
        NOT2,
        NOT4,
        NOT8,
        NEG1,
        NEG2,
        NEG4,
        NEG8,
        NEGF,
        PUSH1,
        PUSH2,
        POP2,
        PUSH4,
        POP4,
        PSHCP,
        PUSH8,
        CALL,
        CALLI,
        SJUMP,
        DPARM,
        CVT,
        RTN4C,
        RTN4,
        RTN4FC,
        RTN4F,
        RTN8C,
        RTN8,
        RTNI8,
        RTN10C,
        RTN10,
        DONOTHING,
        PUSHX,
        MOVFS,
        MOVFD,
        MOVFL,
        PSHFS,
        PSHFD,
        PSHFL,
        UFUNS,
        UFUND,
        UFUNL,
        BFUNS,
        BFUND,
        BFUNL,
        BAD,
        LAST
} table_def;
