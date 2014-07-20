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


;
; CORTNS       : i/o co-routines (common to READ and WRITE)
;

.386p
.8087

include ptypes.inc
include struct.inc
include mdef.inc
include xinit.inc

        xrefp   "C",__ReleaseIOSys

        dataseg

        xrefd   "C",IORslt,         word
        xrefd   __ASTACKSIZ,        dword
        xrefd   __ASTACKPTR,        dword

ifdef __MT__
        xrefd   "C",__SwitchStkLow, dword
endif

ALT_STACK_SIZE = 8*1024
NORMAL_STACK_SIZE = 4*1024

ifndef __NT__
ifndef __LINUX__
STACK   SEGMENT STACK BYTE 'STACK'
        db      ALT_STACK_SIZE dup(?)
ifdef __WINDOWS__
        db      NORMAL_STACK_SIZE dup(?) ; "wstart" doesn't define a stack
endif
        db      NORMAL_STACK_SIZE dup(?)
STACK   ENDS
endif
endif

_FRT_SaveESP dd      ?
RetAddr dd      ?
SaveReg dd      6 dup(?)

        public  _FRT_SaveESP

        enddata

        modstart cortns, byte


        xinit   __setstksize,DEF_PRIORITY

__setstksize proc near
        mov     dword ptr __ASTACKSIZ,ALT_STACK_SIZE
        ret
__setstksize endp


FRAME_SIZE      = (6+1)*4       ; define stack frame to discard when restoring
                                ; state (6 registers and return address)

        xdefp   SwitchToGen
        defp    SwitchToGen
        push    EBP                     ; only save EBP
        mov     EBP,_FRT_SaveESP             ; switch stacks
        mov     _FRT_SaveESP,ESP             ; ...
        mov     ESP,EBP                 ; ...
        pop     EBP                     ; ...
        pop     ESI                     ; ...
        pop     EDI                     ; ...
        pop     EDX                     ; ...
        pop     ECX                     ; ...
        pop     EBX                     ; ...
ifdef __MT__
        push    EAX                     ; switch stack low pointer
        call    dword ptr __SwitchStkLow; ...
        pop     EAX                     ; ...
endif
        ret                             ; return
        endproc SwitchToGen

        xdefp   SwitchToRT
        defp    SwitchToRT
        push    EBX                     ; save current registers
        push    ECX                     ; ...
        push    EDX                     ; ...
        push    EDI                     ; ...
        push    ESI                     ; ...
        push    EBP                     ; ...
        mov     EBP,_FRT_SaveESP             ; switch stacks
        mov     _FRT_SaveESP,ESP             ; ...
        mov     ESP,EBP                 ; ...
        pop     EBP                     ; only ebp saved
ifdef __MT__
        push    EAX                     ; switch stack low pointer
        call    dword ptr __SwitchStkLow; ...
        pop     EAX                     ; ...
endif
        ret                             ; return
        endproc SwitchToRT


        xdefp   RdWrCommon
        defp    RdWrCommon
        ; on entry EAX contains address of read or write routine
        mov     SaveReg,EBX             ; save state
        mov     SaveReg+4,ECX           ; (in case ERR= or END=)
        mov     SaveReg+8,EDX           ; ...
        mov     SaveReg+12,EDI          ; ...
        mov     SaveReg+16,ESI          ; ...
        mov     SaveReg+20,EBP          ; ...
        push    EAX                     ; save address of i/o routine
ifdef __MT__
        call    dword ptr __SwitchStkLow; switch stack low pointer
endif
        mov     EAX,4[ESP]              ; ...
        mov     RetAddr,EAX             ; ...
        pop     EAX                     ; restore address of i/o routine
        push    EBX                     ; save CG registers
        push    ECX                     ; ...
        push    EDX                     ; ...
        push    EDI                     ; ...
        push    ESI                     ; ...
        push    EBP                     ; ...
        mov     _FRT_SaveESP,ESP             ; save stack pointer of executing code
        mov     ESP,__ASTACKPTR         ; get start of new stack
        docall  EAX                     ; start i/o operation
        test    EAX,EAX                 ; check if i/o error
        _if     e
          mov   EBP,_FRT_SaveESP             ; switch stacks
          mov   _FRT_SaveESP,ESP             ; ...
          mov   ESP,EBP                 ; ...
          pop   EBP                     ; restore CG registers
          pop   ESI                     ; ...
          pop   EDI                     ; ...
          pop   EDX                     ; ...
          pop   ECX                     ; ...
          pop   EBX                     ; ...
        _else
          mov   ESP,_FRT_SaveESP             ; switch stacks
          add   ESP,FRAME_SIZE          ; discard previous registers
          mov   EBX,SaveReg             ; restore state
          mov   ECX,SaveReg+4           ; (in case ERR= or END=)
          mov   EDX,SaveReg+8           ; ...
          mov   EDI,SaveReg+12          ; ...
          mov   ESI,SaveReg+16          ; ...
          mov   EBP,SaveReg+20          ; ...
          push  RetAddr                 ; setup return to generated code
        _endif
        push    EAX                     ; save EAX
ifdef __MT__
        call    dword ptr __SwitchStkLow; switch stack low pointer
endif
        call    __ReleaseIOSys          ; release i/o system
        pop     EAX                     ; restore EAX
        ret
        endproc RdWrCommon


        xdefp   "C",IOType
        defp    IOType                  ; return to generated code
        push    EBP                     ; only save EBP
        mov     EBP,_FRT_SaveESP             ; switch stacks
        mov     _FRT_SaveESP,ESP             ; ...
        mov     ESP,EBP                 ; ...
        pop     EBP                     ; ...
        pop     ESI                     ; ...
        pop     EDI                     ; ...
        pop     EDX                     ; ...
        pop     ECX                     ; ...
        pop     EBX                     ; ...
ifdef __MT__
        call    dword ptr __SwitchStkLow; switch stack low pointer
endif
        sub     EAX,EAX                 ; indicate i/o operation succeeded
        ret                             ; return
        endproc IOType


        xdefp   IOChar
        defn    IOChar
        push    [EAX]                   ; put SCB in IORslt
        pop     dword ptr IORslt        ; ...
        push    4[EAX]                  ; ...
        pop     dword ptr IORslt+4      ; ...
        mov     EAX,PT_CHAR             ; return CHARACTER*n type
        jmp     SwitchToRT              ; return to caller of IOType()
        endproc IOChar


        xdefp   IOStr
        defn    IOStr
        mov     dword ptr IORslt,EAX    ; put SCB in IORslt
        mov     dword ptr IORslt+4,EDX  ; ...
        mov     EAX,PT_CHAR             ; return CHARACTER*n type
        jmp     SwitchToRT              ; return to caller of IOType()
        endproc IOStr


        xdefp   IOArr
        defn    IOArr                   ; put array descriptor in IORslt
        mov     dword ptr IORslt,EAX    ; ... data pointer
        mov     dword ptr IORslt+4,EDX  ; ... number of elements
        mov     byte ptr IORslt+12,BL   ; ... type of array
        mov     EAX,PT_ARRAY            ; return ARRAY type
        jmp     SwitchToRT              ; return to caller of IOType()
        endproc IOArr


        xdefp   IOChArr
        defn    IOChArr                 ; put array descriptor in IORslt
        mov     dword ptr IORslt,EAX    ; ... data pointer
        mov     dword ptr IORslt+4,EDX  ; ... number of elements
        mov     dword ptr IORslt+8,EBX  ; ... element size
        mov     AL,PT_CHAR              ; ... type of array
        mov     byte ptr IORslt+12,AL   ; ...
        mov     EAX,PT_ARRAY            ; return ARRAY type
        jmp     SwitchToRT              ; return to caller of IOType()
        endproc IOChArr


        xdefp   RT@EndIO
        defp    RT@EndIO
        mov     EAX,PT_NOTYPE           ; return "no i/o items remaining"
        jmp     SwitchToRT              ; return to caller of IOType()
        endproc RT@EndIO

        endmod
        end
