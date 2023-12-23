;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2021-2022 The Open Watcom Contributors. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  FPU interrupt handling support.
;*
;*****************************************************************************


;*
;* -------------------------------------------------------------------
;*
;* For the IBM PC, we use IRQ13 (math coprocessor exception)
;* Level     Function                               Vector Number
;* -----     --------                               -------------
;* NMI       Parity, Watchdog Timer,
;*           Arbitration time-out,
;*           Channel Check
;* IRQ 0     Timer                                  0x08
;* IRQ 1     Keyboard                               0x09
;* IRQ 2     Cascaded Interrupt Control             0x0A
;* IRQ 3     Serial Alternate                       0x0B
;* IRQ 4     Serial Primary                         0x0C
;* IRQ 5     Reserved                               0x0D
;* IRQ 6     Diskette                               0x0E
;* IRQ 7     Parallel Port                          0x0F
;* IRQ 8     Real Time Clock                        0x70
;* IRQ 9     Redirect Cascade                       0x71
;* IRQ 10    Reserved                               0x72
;* IRQ 11    Reserved                               0x73
;* IRQ 12    Mouse                                  0x74
;* IRQ 13    Math Coprocessor Exception             0x75
;* IRQ 14    Fixed Disk                             0x76
;* IRQ 15    Reserved                               0x77
;*
;* IRQ 8 through 15 are cascaded through IRQ 2
;*
;* IRQ 0 through  7 map to INT 0x08 through INT 0x0F
;* IRQ 8 through 15 map to INT 0x70 through INT 0x77
;*
;* -------------------------------------------------------------------
;*
;* For the NEC PC98, we would use IRQ 8 (math coprocessor exception)
;* Level     Function                               Vector Number
;* -----     --------                               -------------
;* IRQ  0     Timer                                 0x08
;* IRQ  1     Keyboard                              0x09
;* IRQ  2     CTRV                                  0x0A
;* IRQ  3     Expansion bus INT0                    0x0B
;* IRQ  4     Serial Primary (RS-232C ch0)          0x0C
;* IRQ  5     Expansion bus INT1                    0x0D
;* IRQ  6     Expansion bus INT2                    0x0E
;* IRQ  7     Slave                                 0x0F
;* IRQ  8     Math coprocessor (286, 386, ...)      0x10
;* IRQ  9     Expansion bus INT3 (HD, HDLC)         0x11
;* IRQ 10     Expansion bus INT41 (640KB FD)        0x12
;* IRQ 11     Expansion bus INT42 (1MB FD)          0x13
;* IRQ 12     Expansion bus INT5 (RS-232C ch1, ch2) 0x14
;* IRQ 13     Expansion bus INT6 (mouse)            0x15
;* IRQ 14     Reserved (math coprocessor in 70116)  0x16
;* IRQ 15     Reserved                              0x17
;*
;* IRQ 0 through  7 map to INT 0x08 through INT 0x0F
;* IRQ 8 through 15 map to INT 0x10 through INT 0x17
;*
;* If IRQ_NUM is less than 8, we have a problem in the code.
;*

;* FPU INT number */
IBM_FPE_INT equ     02h
NEC_FPE_INT equ     10h
;* FPU IRQ number */
IBM_IRQ_NUM equ     0Dh
NEC_IRQ_NUM equ     08h
;* Master PIC port number */
IBM_MST_PIC equ     20h
NEC_MST_PIC equ     00h
;* Slave PIC port number */
IBM_SLV_PIC equ     0A0h
NEC_SLV_PIC equ     08h

ifdef __WATCOM_PC98__
    FPE_INT equ     __FPE_int
    ifdef __386__
        IRQ_NUM equ __IRQ_num
        MST_PIC equ __MST_pic
        SLV_PIC equ __SLV_pic
    endif
else
    FPE_INT equ     IBM_FPE_INT
    ifdef __386__
        IRQ_NUM equ IBM_IRQ_NUM
        MST_PIC equ IBM_MST_PIC
        SLV_PIC equ IBM_SLV_PIC
    endif
endif

ifdef  __WATCOM_PC98__ )
extrn  "C",__FPE_int : byte
ifdef __386__)
extrn  "C",__IRQ_num : byte
extrn  "C",__MST_pic : byte
extrn  "C",__SLV_pic : byte
endif
endif
