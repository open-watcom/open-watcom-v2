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
* Description:  Alpha AXP specific definitions for #pragma aux.
*
****************************************************************************/


hw_reg_set DefaultParms[] = {
    HW_D( HW_EMPTY )
};

hw_reg_set DefaultVarParms[] = {
    HW_D( HW_EMPTY )
};

hw_reg_set DefaultLinkage[] = {
    HW_D( HW_R16 ),
    HW_D( HW_R17 ),
    HW_D( HW_R18 ),
    HW_D( HW_R19 ),
    HW_D( HW_R20 ),
    HW_D( HW_R21 ),
};

#define NC '\0'

char Registers[] = {
     0, 'v','0',NC,
     1, 't','0',NC,
     2, 't','1',NC,
     3, 't','2',NC,
     4, 't','3',NC,
     5, 't','4',NC,
     6, 't','5',NC,
     7, 't','6',NC,
     8, 't','7',NC,
     9, 's','0',NC,
    10, 's','1',NC,
    11, 's','2',NC,
    12, 's','3',NC, 
    13, 's','4',NC, 
    14, 's','5',NC, 
    15, 's','6',NC, 
    15, 'f','p',NC, 
    16, 'a','0',NC, 
    17, 'a','1',NC, 
    18, 'a','2',NC, 
    19, 'a','3',NC, 
    20, 'a','4',NC, 
    21, 'a','5',NC, 
    22, 't','8',NC, 
    23, 't','9',NC, 
    24, 't','1','0',NC,
    25, 't','1','1',NC,
    26, 'r','a',NC,
    27, 't','1','2',NC,
    27, 'p','v',NC,
    28, 'a','t',NC, 
    29, 'g','p',NC, 
    30, 's','p',NC, 
    31, 'z','e','r','o',NC,
    0,  NC
};

hw_reg_set RegBits[] = {
    HW_D( HW_R0 ),
    HW_D( HW_R1 ),
    HW_D( HW_R2 ),
    HW_D( HW_R3 ),
    HW_D( HW_R4 ),
    HW_D( HW_R5 ),
    HW_D( HW_R6 ),
    HW_D( HW_R7 ),
    HW_D( HW_R8 ),
    HW_D( HW_R9 ),
    HW_D( HW_R10 ),
    HW_D( HW_R11 ),
    HW_D( HW_R12 ),
    HW_D( HW_R13 ),
    HW_D( HW_R14 ),
    HW_D( HW_R15 ),
    HW_D( HW_R16 ),
    HW_D( HW_R17 ),
    HW_D( HW_R18 ),
    HW_D( HW_R19 ),
    HW_D( HW_R20 ),
    HW_D( HW_R21 ),
    HW_D( HW_R22 ),
    HW_D( HW_R23 ),
    HW_D( HW_R24 ),
    HW_D( HW_R25 ),
    HW_D( HW_R26 ),
    HW_D( HW_R27 ),
    HW_D( HW_R28 ),
    HW_D( HW_R29 ),
    HW_D( HW_R30 ),
    HW_D( HW_R31 ),
};
