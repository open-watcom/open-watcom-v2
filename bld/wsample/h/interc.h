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


/*
    located in INTRC.ASM
*/

extern void __interrupt (* FAR_PTR old_int13)();
extern void __interrupt (* FAR_PTR old_int21)();
extern void __interrupt (* FAR_PTR old_int28)();

extern void __interrupt (* FAR_PTR old_intx0)();
extern void __interrupt (* FAR_PTR old_intx1)();
extern void __interrupt (* FAR_PTR old_intx2)();
extern void __interrupt (* FAR_PTR old_intx3)();
extern void __interrupt (* FAR_PTR old_intx4)();
extern void __interrupt (* FAR_PTR old_intx5)();
extern void __interrupt (* FAR_PTR old_intx6)();
extern void __interrupt (* FAR_PTR old_intx7)();

extern seg_offset       __far SysCallerAddr;
extern unsigned char    __far SysCaught;
extern unsigned char    __far SysNoDOS;

extern void __interrupt int28_handler( union INTPACK r );
extern void __interrupt int21_handler( union INTPACK r );
extern void __interrupt int13_handler( union INTPACK r );
extern void __interrupt int03_handler( union INTPACK r );

extern void __interrupt intx0_handler( union INTPACK r );
extern void __interrupt intx1_handler( union INTPACK r );
extern void __interrupt intx2_handler( union INTPACK r );
extern void __interrupt intx3_handler( union INTPACK r );
extern void __interrupt intx4_handler( union INTPACK r );
extern void __interrupt intx5_handler( union INTPACK r );
extern void __interrupt intx6_handler( union INTPACK r );
extern void __interrupt intx7_handler( union INTPACK r );
