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


#pragma aux ShiftStatus =   \
/* ShiftStatus() : int */   \
            "mov    ah,2"   \
            "int    16h"    \
        parm caller [ax];



#pragma aux kbhit =         \
/* kbhit() : char */        \
            "mov    ah,1"   \
            "int    16h"    \
            "jnz short L1"  \
            "xor    al,al"  \
            "jmp short L2"  \
        "L1: mov    al,1"   \
        "L2:"               \
        parm caller ;



#pragma aux ScnKey =        \
/* ScnKey() : int */        \
            "mov    ah,1"   \
            "int    16h"    \
        parm caller ;



#pragma aux GtKey =         \
            "xor    ah,ah"  \
            "int    16h"    \
        parm caller [ax];



extern int  ShiftStatus();
extern char kbhit();
extern int  ScnKey();
extern int  GtKey();
