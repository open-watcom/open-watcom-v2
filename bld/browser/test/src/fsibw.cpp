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


char S13() { return 13; }
char S12() { return S13(); }
char S11() { return S12(); }

char S2() { return 1; }
char S3() { return 1; }
char S4() { return 1; }
char S5() { return 1; }
char S6() { return 1; }
char S7() { return 1; }
char S8() { return 1; }
char S9() { return 1; }
char S10() { return 1; }

char S1() { return S2() + S3() + S4() + S5() +
                   S6() + S7() + S8() + S9() + S10(); }

char AReallyExtremelyVeryLongNameThatCouldCauseSomeProblemsSomewhere()
{ return S1() + S2() + S3() + S4() + S5() + S6() + S7() + S8() + S9() + S10(); }
