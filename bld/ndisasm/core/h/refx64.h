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
* Description:  Memory references for x86 CPUs.
*
****************************************************************************/


refpick( BYTE, "byte" )
refpick( WORD, "word" )
refpick( DWORD, "dword" )
refpick( QWORD, "qword" )
refpick( TBYTE, "tbyte" )
refpick( BCD, "tbyte" )
refpick( BYTE28, "" )
refpick( BYTE14, "" )
refpick( BYTE108, "" )
refpick( BYTE94, "" )
refpick( BYTE512, "" )
refpick( BYTEX, "" )
refpick( DWORDF, "dword" )
refpick( QWORDF, "qword" )
refpick( FARPTR48, "fword" )
refpick( FARPTR32, "dword" )
refpick( MEM1624, "fword" )  // 32 bits instead of 24 is still fetched
refpick( MEM1632, "fword" )
refpick( MM64, "" )
refpick( XMM16, "" )
refpick( XMM32, "" )
refpick( XMM64, "" )
refpick( XMM128, "" )
