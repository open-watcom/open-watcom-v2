;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
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
;* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
;*               DESCRIBE IT HERE!
;*
;*****************************************************************************


; control word fields
; exception masks (nyi)
CW_IM           =       0001H
CW_DM           =       0002H
CW_ZM           =       0004H
CW_OM           =       0008H
CW_UM           =       0010H
CW_PM           =       0020H
; precision control (nyi)
CW_PC           =       0300H
  PC_EXTENDED   =       0300H
  PC_DOUBLE     =       0200H
  PC_SINGLE     =       0000H
; rounding control (nyi)
CW_RC           =       0C00H
  RC_ZERO       =       0C00H
  RC_UP         =       0800H
  RC_DOWN       =       0400H
  RC_NEAREST    =       0000H
; infinity control (must be 1 for 80387)
CW_IC           =       1000H
CW_INIT = CW_IM+CW_DM+CW_ZM+CW_OM+CW_UM+CW_PM+PC_EXTENDED+RC_NEAREST

; status word fields
; exception bits (sticky) (nyi)
SW_IE           =       0001H
SW_DE           =       0002H
SW_ZE           =       0004H
SW_OE           =       0008H
SW_UE           =       0010H
SW_PE           =       0020H

; stack flag - set if we underflow or overflow the 8087 stack (nyi)
SW_SF           =       0040H
; error summary - set when we produce an unmasked exception (nyi)
SW_ES           =       0080H
; condition codes - (nyi) except for fcom(p(p)) instructions
SW_C0           =       0100H
SW_C1           =       0200H
SW_C2           =       0400H
SW_C3           =       4000H
SW_CC           =      04700H

; top of stack pointer
SW_ST           =       3800H
SW_ST_SHIFT     =       11
; set when busy (never)
SW_B            =       8000H
SW_INIT = 0
TAG_INIT= 0FFFFH

