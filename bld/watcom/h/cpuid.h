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


extern int cpu_id( void );

#pragma aux cpu_id = \
    "           .586            " \
    "           pushfd          " /* save flags register */ \
    "           mov eax,3       " /* assume 386 */ \
    "           pushfd          " \
    "           pop ebx         " \
    "           xor ebx,040000h " /* change AC bit */ \
    "           push ebx        " \
    "           popfd           " \
    "           pushfd          " \
    "           pop edx         " \
    "           xor ebx, edx    " \
    "           test ebx,040000h" /* check if AC bit changed */ \
    "           jne done        " \
    "           mov eax,4       " /* at least a 486 */ \
    "           pushfd          " \
    "           pop ebx         " \
    "           xor ebx,200000h " /* change ID bit */ \
    "           push ebx        " \
    "           popfd           " \
    "           pushfd          " \
    "           pop edx         " \
    "           xor ebx, edx    " \
    "           test ebx,200000h" /* check if ID bit changed */ \
    "           jne done        " \
    "           mov eax,1       " /* want version information */ \
    "           cpuid           " \
    "           shr eax,8       " /* get family bits into bottom of eax */ \
    "           and eax,0Fh     " \
    " done:     popfd           " \
    modify exact [ ebx edx eax ] value [eax];
