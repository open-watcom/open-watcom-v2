/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DWARF x86 register definitions and related data.
*
****************************************************************************/

/*      name    ci   start len */
DW_REG( eax,    CI_EAX, 0, 32 )
DW_REG( ecx,    CI_ECX, 0, 32 )
DW_REG( edx,    CI_EDX, 0, 32 )
DW_REG( ebx,    CI_EBX, 0, 32 )
DW_REG( esp,    CI_ESP, 0, 32 )
DW_REG( ebp,    CI_EBP, 0, 32 )
DW_REG( esi,    CI_ESI, 0, 32 )
DW_REG( edi,    CI_EDI, 0, 32 )
DW_REG( eip,    CI_EIP, 0, 32 )
DW_REG( eflags, CI_EFL, 0, 32 )
DW_REG( trapno, 0,      0, 0  )
DW_REG( st0,    CI_ST0, 0, 80 )
DW_REG( st1,    CI_ST1, 0, 80 )
DW_REG( st2,    CI_ST2, 0, 80 )
DW_REG( st3,    CI_ST3, 0, 80 )
DW_REG( st4,    CI_ST4, 0, 80 )
DW_REG( st5,    CI_ST5, 0, 80 )
DW_REG( st6,    CI_ST6, 0, 80 )
DW_REG( st7,    CI_ST7, 0, 80 )
DW_REG( al,     CI_EAX, 0, 8  )
DW_REG( ah,     CI_EAX, 8, 8  )
DW_REG( bl,     CI_EBX, 0, 8  )
DW_REG( bh,     CI_EBX, 8, 8  )
DW_REG( cl,     CI_ECX, 0, 8  )
DW_REG( ch,     CI_ECX, 8, 8  )
DW_REG( dl,     CI_EDX, 0, 8  )
DW_REG( dh,     CI_EDX, 8, 8  )
DW_REG( ax,     CI_EAX, 0, 16 )
DW_REG( bx,     CI_EBX, 0, 16 )
DW_REG( cx,     CI_ECX, 0, 16 )
DW_REG( dx,     CI_EDX, 0, 16 )
DW_REG( si,     CI_ESI, 0, 16 )
DW_REG( di,     CI_EDI, 0, 16 )
DW_REG( bp,     CI_EBP, 0, 16 )
DW_REG( sp,     CI_ESP, 0, 16 )
DW_REG( cs,     CI_CS,  0, 16 )
DW_REG( ss,     CI_SS,  0, 16 )
DW_REG( ds,     CI_DS,  0, 16 )
DW_REG( es,     CI_ES,  0, 16 )
DW_REG( fs,     CI_FS,  0, 16 )
DW_REG( gs,     CI_GS,  0, 16 )
