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


#define X1    m = 1
#define X2    m == -1*&*~var|(+y|x)
#define X3    1|&*2
#define X4    m == 1?x.i->z:crap
#define X5    m = 1||==1
#define X6    m & sizeof(int)%a+t-0==2
#define X7    sizeof int[100]
#define X8    sizeof a[0(t)][a]()

int i01 = &13 ;
int i02 = 13 & 23;
int i03 = 13 && 23;
int i04 = a->b;
int i05 = max(13, 23);
int i06 = (int) 13;
int i07 = 13 ? 23: 33;
int i08 = 13/23;
int i09 = *13;
int i00 = 13 == 23;
int i11 = !13;
int i12 = (13, 23);
int i13 = ((13=23), (33=43), 5);
int i14 = 13 >= 23;
int i15 = 13 > 23;
int i16 = 13[23];
int i17 = 13 <= 23;
int i18 = 13 << 23;
int i19 = 13 < 23;
int i20 = -13;
int i21 = 13 != 23;
int i22 = 13 | 23;
int i23 = 13 || 23 || 33;
int i24 = (13);
int i25 = 13 % 23 % 33;
int i26 = 13 + 23 + 33;
int i27 = 13 >> 23;
int i28 = sizeof i1;
int i29 = sizeof(int);
int i30 = "13" "23";
int i31 = ~13;
int i32 = 13 * 23 * 33;
int i33 = - 13 - 23;
int i34 = + 13 + 23;
int i35 = *13;
int i36 = 13 ^ 23;
#define M ;
#if 13
#elif 23
#else
#endif
#ifdef a
#ifndef a
