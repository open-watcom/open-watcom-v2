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


char Diag1() { return 10; }
char Diag3() { return 10; }
char Diag5() { return 10; }
char Diag7() { return 10; }
char Diag9() { return 10; }
char Diag10() { return 10; }
char Diag12() { return 10; }
char Diag14() { return 10; }
char Diag16() { return 10; }
char Diag18() { return 10; }
char Diag20() { return 10; }

char Diag8() { return Diag9() + Diag10(); }
char Diag6() { return Diag7() + Diag8(); }
char Diag4() { return Diag5() + Diag6(); }
char Diag2() { return Diag3() + Diag4(); }
char Diag19() { return Diag1() + Diag2(); }

char Diag11() { return Diag10() + Diag12(); }
char Diag13() { return Diag11() + Diag14(); }
char Diag15() { return Diag13() + Diag16(); }
char Diag17() { return Diag15() + Diag18(); }
char Diag21() { return Diag17() + Diag20(); }

char Diag22() { return 22; }
char Diag23() { return Diag19() + Diag22(); }
char Diag24() { return Diag22() + Diag21(); }
