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

                                                                         
/*    enum          opcode      fpopcode  */
pick( NO,           NULL,       NULL       )
pick( ADD1,         Add1,       Add1       )
pick( ADD2,         Add2,       Add2       )
pick( ADD4,         Add4,       Add4       )
pick( ADD8,         Add8,       Add8       )
pick( EADD,         AddExt,     AddExt     )
pick( ADDCP,        AddCP,      AddCP      )
pick( ADDPT,        AddPT,      AddPT      )
pick( SUB1,         Sub1,       Sub1       )
pick( SUB2,         Sub2,       Sub2       )
pick( SUB4,         Sub4,       Sub4       )
pick( SUB8,         Sub8,       Sub8       )
pick( ESUB,         SubExt,     SubExt     )
pick( SUBCP,        SubCP,      SubCP      )
pick( SUBPT,        SubPT,      SubPT      )
pick( LOG8,         Log8,       Log8       )
pick( SUPP8,        Rtn8,       Rtn8       )
pick( OR1,          Or1,        Or1        )
pick( OR2,          Or2,        Or2        )
pick( OR4,          Or4,        Or4        )
pick( AND1,         And1,       And1       )
pick( AND2,         And2,       And2       )
pick( AND4,         And4,       And4       )
pick( MUL1,         Mul1,       Mul1       )
pick( MUL2,         Mul2,       Mul2       )
pick( EMUL,         ExtMul,     ExtMul     )
pick( DIV1,         Div1,       Div1       )
pick( DIV2,         Div2,       Div2       )
pick( MOD1,         Mod1,       Mod1       )
pick( MOD2,         Mod2,       Mod2       )
pick( SHFT1,        Shft1,      Shft1      )
pick( SHFT2,        Shft2,      Shft2      )
pick( SHFT4,        Shft4,      Shft4      )
pick( TEST1,        TestOrCmp1, TestOrCmp1 )
pick( TEST2,        Test2,      Test2      )
pick( TEST4,        Test4,      Test4      )
pick( TEST8,        Test8,      Test8      )
pick( CMP1,         TestOrCmp1, TestOrCmp1 )
pick( CMP2C,        Cmp2,       Cmp2       )
pick( CMP2,         Cmp2,       Cmp2       )
pick( CMP4C,        Cmp4,       Cmp4       )
pick( CMP4,         Cmp4,       Cmp4       )
pick( CMP8,         Cmp8,       Cmp8       )
pick( CMPCP,        CmpCP,      CmpCP      )
pick( CMPPT,        CmpPT,      CmpPT      )
pick( CMPF,         CmpFS,      Cmp87      )
pick( CMPD,         CmpFD,      Cmp87      )
pick( CMPL,         CmpFD,      Cmp87      )
pick( CMPX,         CmpXX,      CmpXX      )
pick( MOV1,         Move1,      Move1      )
pick( MOV2,         Move2CC,    Move2CC    )
pick( MOV4,         Move4,      Move4      )
pick( MOV8,         Move8,      Move8      )
pick( MOVX,         MoveXX,     MoveXX     )
pick( LA4,          LoadA4,     LoadA4     )
pick( LA,           LoadA,      LoadA      )
pick( NOT1,         Not1,       Not1       )
pick( NOT2,         Not2,       Not2       )
pick( NOT4,         Not4,       Not4       )
pick( NOT8,         Not4,       Not8       )
pick( NEG1,         Neg1,       Neg1       )
pick( NEG2,         Neg2,       Neg2       )
pick( NEG4,         Neg4,       Neg4       )
pick( NEG8,         Neg8,       Neg8       )
pick( NEGF,         NegF,       Un87       )
pick( PUSH1,        Push1,      Push1      )
pick( PUSH2,        Push2,      Push2      )
pick( POP2,         Pop2,       Pop2       )
pick( PUSH4,        Push4,      Push4      )
pick( PUSH8,        Push8,      Push8      )
pick( CALL,         Call,       Call       )
pick( CALLI,        CallI,      CallI      )
pick( SJUMP,        SJump,      SJump      )
pick( DPARM,        Parm,       Parm       )
pick( CVT,          Cvt,        Cvt        )
pick( RTN4C,        Rtn4C,      Rtn4C      )
pick( RTN4,         Rtn4,       Rtn4       )
pick( RTN4FC,       Rtn4C,      Bin87      )
pick( RTN4F,        Rtn4,       Bin87      )
pick( RTN8,         Rtn8,       Bin87      )
pick( RTN10,        Rtn8,       Bin87      )
pick( DONOTHING,    DoNop,      DoNop      )
pick( PUSHX,        PushXX,     PushXX     )
pick( MOVFS,        Move4,      Move87S    )
pick( MOVFD,        Move8,      Move87D    )
pick( MOVFL,        Move8,      Move87D    )
pick( PSHFS,        Push4,      Push87S    )
pick( PSHFD,        Push8,      Push87D    )
pick( PSHFL,        Push8,      Push87D    )
pick( RTN8C,        Rtn8C,      Bin87      )
pick( RTN10C,       Rtn8C,      Bin87      )
pick( BITCP,        BitCP,      BitCP      )
pick( UFUNS,        RTCall,     Un87Func   )
pick( UFUND,        RTCall,     Un87Func   )
pick( UFUNL,        RTCall,     Un87Func   )
pick( BFUNS,        Rtn4,       Bin87Func  )
pick( BFUND,        Rtn8,       Bin87Func  )
pick( BFUNL,        Rtn8,       Bin87Func  )
pick( BAD,          NULL,       NULL       )
