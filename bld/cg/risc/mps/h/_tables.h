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
* Description:  List of instruction reduction tables. See mipstable.c.
*
****************************************************************************/


pick( NO,         StubUnary       )
pick( NYI,        StubUnary       )
pick( BIN1,       Binary1         )
pick( BIN2,       Binary2         )
pick( BIN4,       Binary4         )
pick( BIN8,       Binary8         )
pick( BINU1,      BinaryUC1       )
pick( BINU2,      BinaryUC2       )
pick( BINU4,      BinaryUC4       )
pick( BINU8,      Binary8         )
pick( BINN1,      BinaryNI1       )
pick( BINN2,      BinaryNI2       )
pick( BINN4,      BinaryNI4       )
pick( BINN8,      Binary8         )
pick( UN1,        Un1             )
pick( UN2,        Un2             )
pick( UN4,        Un4             )
pick( UN8,        Un8             )
pick( NEG8,       Neg8            )
pick( MOV1,       Move1           )
pick( MOV2,       Move2           )
pick( MOV4,       Move4           )
pick( MOV8,       Move8           )
pick( MOVXX,      MoveXX          )
pick( CONV,       Conv            )
pick( FCONV,      Conv            )
pick( CALL,       Call            )
pick( CALLI,      CallI           )
pick( PUSH,       Push            )
pick( POP,        Pop             )
pick( LA2,        LoadAddr2       )
pick( LA4,        LoadAddr4       )
pick( LA8,        LoadAddr8       )
#if 0   // Maybe for MIPS64?
pick( CMP4,       StubBinary      )
pick( CMP8,       Cmp8            )
#else
pick( CMP4,       Cmp4            )
pick( CMP8,       Cmp8            )
#endif
pick( TEST4,      Test4           )
pick( TEST8,      Test4           )
pick( SET4,       Set4            )
pick( SET8,       Set4            )
pick( ZAP,        Binary4         )
pick( EXT4,       Binary4         )
pick( LDQU,       LoadUnaligned   )
pick( STQU,       StoreUnaligned  )
pick( FBINS,      FloatBinary     )
pick( FBIND,      FloatBinary     )
pick( MOVFS,      MoveF           )
pick( MOVFD,      MoveF           )
pick( CMPFS,      CmpF            )
pick( CMPFD,      CmpF            )
pick( RTN,        Rtn             )
pick( NEGF,       NegF            )
pick( PROM,       Promote         )
pick( PROM8,      Promote8        )
pick( BFUNS,      Rtn             )
pick( BFUND,      Rtn             )
pick( BFUNL,      Rtn             )
pick( UFUNS,      UnaryRtn        )
pick( UFUND,      UnaryRtn        )
pick( UFUNL,      UnaryRtn        )
pick( STK4,       Alloca4         )
pick( DONOTHING,  DoNop           )
pick( BAD,        NULL            )
