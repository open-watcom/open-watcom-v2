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


pick( NO,         StubUnary   )
pick( NYI,        StubUnary   )
pick( BIN1,       Binary1     )
pick( BIN2,       Binary2     )
pick( BIN4,       Binary4     )
pick( BIN8,       Binary8     )
pick( UBIN1,      UBinary1    )
pick( UBIN2,      UBinary2    )
pick( UBIN4,      UBinary4    )
pick( NBIN1,      NBinary1    )
pick( NBIN2,      NBinary2    )
pick( NBIN4,      NBinary4    )
pick( UN1,        Un1         )
pick( UN2,        Un2         )
pick( UN4,        Un4         )
pick( UN8,        StubUnary   )
pick( MOV1,       Move1       )
pick( MOV2,       Move2       )
pick( MOV4,       Move4       )
pick( MOV8,       Move8       )
pick( MOVXX,      MoveXX      )
pick( CONV,       Conv        )
pick( FCONV,      Conv        )
pick( CALL,       Call        )
pick( CALLI,      CallI       )
pick( PUSH,       Push        )
pick( POP,        Pop         )
pick( LA2,        LoadAddr2   )
pick( LA4,        LoadAddr4   )
pick( LA8,        LoadAddr8   )
pick( CMP4,       Cmp4        )
pick( CMP8,       StubBinary  )
pick( TEST4,      Test4       )
pick( TEST8,      StubBinary  )
pick( SET4,       Set4        )
pick( SET8,       StubBinary  )
pick( ZAP,        Binary4     )
pick( EXT4,       Binary4     )
pick( FBINS,      FloatBinary )
pick( FBIND,      FloatBinary )
pick( MOVFS,      MoveF       )
pick( MOVFD,      MoveF       )
pick( CMPFS,      CmpF        )
pick( CMPFD,      CmpF        )
pick( RTN,        Rtn         )
pick( NEGF,       NegF        )
pick( PROM,       Promote     )
pick( VASTR,      VaStart     )
pick( MOD4,       Mod4        )
pick( DONOTHING,  DoNop       )
pick( BAD,        NULL        )
