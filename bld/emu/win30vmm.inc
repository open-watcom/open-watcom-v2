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


;******************************************************************************
; The following structures are pointed to by EBP when VxD routines are entered,
;   both for VxD control calls and traps(I/O traps, software INT traps, etc.).
;   The first structure as DWORD values, the second WORD values and the last
;   has BYTE values.
;
Client_Reg_Struc   struc
Client_EDI      dd      ?               ; Client's EDI
Client_ESI      dd      ?               ; Client's ESI
Client_EBP      dd      ?               ; Client's EBP
                dd      ?               ; ESP at pushall
Client_EBX      dd      ?               ; Client's EBX
Client_EDX      dd      ?               ; Client's EDX
Client_ECX      dd      ?               ; Client's ECX
Client_EAX      dd      ?               ; Client's EAX
Client_Error    dd      ?               ; Dword error code
Client_EIP      dd      ?               ; EIP
Client_CS       dw      ?               ; CS
                dw      ?               ;   (padding)
Client_EFlags   dd      ?               ; EFLAGS
Client_ESP      dd      ?               ; ESP
Client_SS       dw      ?               ; SS
                dw      ?               ;   (padding)
Client_ES       dw      ?               ; ES
                dw      ?               ;   (padding)
Client_DS       dw      ?               ; DS
                dw      ?               ;   (padding)
Client_FS       dw      ?               ; FS
                dw      ?               ;   (padding)
Client_GS       dw      ?               ; GS
                dw      ?               ;   (padding)
Client_Alt_EIP  dd      ?
Client_Alt_CS   dw      ?
                dw      ?
Client_Alt_EFlags  dd   ?
Client_Alt_ESP  dd      ?
Client_Alt_SS   dw      ?
                dw      ?
Client_Alt_ES   dw      ?
                dw      ?
Client_Alt_DS   dw      ?
                dw      ?
Client_Alt_FS   dw      ?
                dw      ?
Client_Alt_GS   dw      ?
                dw      ?
Client_Reg_Struc   ends

