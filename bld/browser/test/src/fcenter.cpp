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


char C15() { return 15; }
char C14() { return 14; }
char C13() { return 13; }

char CenteringReallyLongNames12() { return C15(); }
char CenteringReallyLongNames11() { return C14(); }
char CenteringReallyLongNames10() { return C13(); }

char CenteringReallyLongNames8() { return CenteringReallyLongNames12() +
                                          CenteringReallyLongNames12() +
                                          CenteringReallyLongNames12(); }

char C7() { return CenteringReallyLongNames8(); }

char CenteringReallyLongNames6() { return C7(); }
char CenteringReallyLongNames5() { return C7(); }
char CenteringReallyLongNames4() { return C7(); }

char C3() { return CenteringReallyLongNames6(); }
char C2() { return CenteringReallyLongNames5(); }
char C1() { return CenteringReallyLongNames4(); }
