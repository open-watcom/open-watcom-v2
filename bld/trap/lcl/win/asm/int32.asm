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


;****************************************************************************
;***                                                                      ***
;*** INT32.ASM - handle 32-bit debug interrupts                           ***
;***                                                                      ***
;****************************************************************************
.386p
.387

DGROUP group _DATA

;*
;*** data defined here
;*
_DATA segment word public 'DATA' use16

InterruptStack  LABEL FWORD
_InterruptStackOff      dd 0
_InterruptStackSel      dw 0
_IDTSel                 dw 0
_DPL                    dw 0
_OurOwnInt              dw 0

public _DPL
public _IDTSel
public _OurOwnInt
public _InterruptStackSel
public _InterruptStackOff

;*
;*** save area for registers
;*
_SaveEAX        dd 0
_SaveEBX        dd 0
_SaveECX        dd 0
_SaveEDX        dd 0
_SaveEDI        dd 0
_SaveESI        dd 0
_SaveEFLAGS     dd 0
_SaveEBP        dd 0
_SaveEIP        dd 0
_SaveESP        dd 0
_SaveSS         dw 0
_SaveCS         dw 0
_SaveDS         dw 0
_SaveES         dw 0
_SaveFS         dw 0
_SaveGS         dw 0
_FaultNumber    dw 0
                dw 0
                dw 0
public _SaveEAX
public _SaveEBX
public _SaveECX
public _SaveEDX
public _SaveEDI
public _SaveESI
public _SaveEFLAGS
public _SaveEBP
public _SaveEIP
public _SaveESP
public _SaveSS
public _SaveCS
public _SaveDS
public _SaveES
public _SaveFS
public _SaveGS
public _FaultNumber

idt             LABEL FWORD
_idt            db 8 dup(0)

_DATA ends

;*
;*** the interrupt code lies here
;*
_TEXT segment word public 'CODE' use16

assume cs:_TEXT

;applds     dw      DGROUP

public ReflectInt1Int3_
ReflectInt1Int3_ PROC
;       push    gs
;       mov     gs,cs:[applds]
;       lss     esp,gs:[InterruptStack]
;       pop     gs
        db      0fh
        db      0ffh
ReflectInt1Int3_ ENDP

;
; InterruptCallback:
;
; this code is executed after any 32-bit fault occurs.  WDEBUG.386 traps
; the fault, copies all the registers to the SaveEAX et al above,
; and then sets the Windows VM to start execution here.  We flag
; that we got one of our special interrupts, and then do an int 3,
; which will be processed by a normal fault handler.
;
; once we return from the normal fault handler, we make call WDEBUG.386
; to load all registers from the save area (SaveEAX etc) and restart the
; Windows VM at the new location.
;
public InterruptCallback_
InterruptCallback_ PROC
        mov     ds:_OurOwnInt,1
        int     3

        mov     ax,0fa2ch               ; RestartFromInterrupt
        int     02fh
InterruptCallback_ ENDP

assume ds:DGROUP

;****************************************************************************
;***                                                                      ***
;*** GetIDTSel_ - get a selector pointing at the IDT                      ***
;***                                                                      ***
;****************************************************************************
GetIDTSel_  PROC
PUBLIC GetIDTSel_

        pusha                           ; paranoid register saving!

;*
;*** access interrupt descriptor table
;*
        db      66h
        sidt    idt                     ; get IDT entry

        mov     al,_idt+5               ; twiddle IDT entry
        mov     _idt+7,al               ;    to make it look like an
        mov     ax,_DPL                 ;       LDT entry (with r/w access)
        add     ax,092h                 ;
        mov     _idt+5,al               ;
        mov     _idt+6,000h

        xor     ax,ax                   ; now, get an LDT
        mov     cx,1                    ;    entry from
        int     31h                     ;       DPMI
        mov     _IDTSel,ax

        mov     bx,_IDTSel              ; set up the LDT entry
        mov     ax,ds                   ;    to be the IDT entry
        mov     es,ax                   ;       to give us access to
        mov     ax,0ch                  ;           the IDT
        xor     edi,edi
        mov     di,offset _idt
        int     31h

        popa

        ret

GetIDTSel_ ENDP

;****************************************************************************
;***                                                                      ***
;*** ReleaseIDTSel - get rid of selector for IDT                          ***
;***                                                                      ***
;****************************************************************************
ReleaseIDTSel_  PROC
PUBLIC ReleaseIDTSel_

        push    bx

        mov     ax,1                            ; free descriptor
        mov     bx,_IDTSel
        int     31h

        pop     bx

        ret
ReleaseIDTSel_ ENDP

_TEXT ends
        end
