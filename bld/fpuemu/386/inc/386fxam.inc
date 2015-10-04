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


        modstart  386fxam

        xdefp   __fxam          ; examine stack top

;
;       these equates match the condition code bits in the status word
;
XAM_UNNORMAL    equ     00h
XAM_NAN         equ     01h
XAM_NORMAL      equ     04h
XAM_INFINITY    equ     05h
XAM_ZERO        equ     40h
XAM_EMPTY       equ     41h
XAM_DENORMAL    equ     44h

;
;       input:  AX - pointer to long double operand
;

        defp    __fxam
        push    EBX                     ; save EBX
        push    EDX                     ; save EDX
        mov     EBX,EAX                 ; get pointer to top of stack
        mov     AX,CS:tagtab[EDI].t_empty; get tag word mask for top of stack
        mov     EDX,EAX                 ; ...
        and     AX,DS:[EBP].tags        ; ...
        _guess                          ; guess: EMPTY
          cmp   AX,DX                   ; - quit if not empty
          _quif ne                      ; - ...
          mov   AL,XAM_EMPTY            ; - indicate empty
          mov   DH,0                    ; - set sign to +ve
        _admit                          ; guess: DENORMAL or ZERO
          not   EDX                     ; - set tag entry to VALID
          and   DS:[EBP].tags,DX        ; - ...
          mov   AX,8[EBX]               ; - get exponent and sign
          mov   EDX,EAX                 ; - DH = 0 for +ve, 02h for -ve
          and   DH,80h                  ; - isolate sign bit
          rol   DH,1                    ; - shift into position
          shl   DH,1                    ; - ...
          and   AX,7FFFh                ; - isolate exponent
          _quif ne                      ; - quit if not 0
          mov   EAX,4[EBX]              ; - check fraction to see if 0
          or    EAX,[EBX]               ; - ...
          mov   AL,XAM_DENORMAL         ; - assume DENORMAL number
          _if   e                       ; - if fraction is 0
            mov   AX,CS:tagtab[EDI].t_zero;- - get tag word mask for a zero
            or    DS:[EBP].tags,AX      ; - - tag number as a zero
            mov   AL,XAM_ZERO           ; - - indicate number is ZERO
          _endif                        ; - endif
        _admit                          ; guess: NAN or INFINITY
          cmp   AX,7FFFh                ; - quit if not NAN or INFINITY
          _quif ne                      ; - ...
          mov   AX,CS:tagtab[EDI].t_nan ; - get tag word mask for a nan
          or    DS:[EBP].tags,AX        ; - tag number as a nan
          cmp   dword ptr [EBX],0       ; - check fraction to see if 0
          _if   e                       ; - if low order word is zero
            mov   EAX,4[EBX]            ; - - get high order word of fraction
            cmp   EAX,80000000h         ; - - check for infinity
          _endif                        ; - endif
          mov   AL,XAM_NAN              ; - assume NAN number
          _if   e                       ; - if fraction is 8000 0000 0000 0000
            mov   AL,XAM_INFINITY       ; - - indicate number is INFINITY
          _endif                        ; - endif
        _admit                          ; admit: NORMAL or UNNORMAL
          test  byte ptr 7[EBX],80h     ; - check implied 1 bit
          mov   AL,XAM_NORMAL           ; - assume NORMAL number
          _if   e                       ; - if implied 1 bit is not on
            mov   AX,CS:tagtab[EDI].t_nan; - - get tag word mask for a nan
            or    DS:[EBP].tags,AX      ; - - tag number as a nan
            mov   AL,XAM_DENORMAL       ; - - indicate DENORMAL number
          _endif                        ; - endif
        _endguess                       ; endguess
        or      AL,DH                   ; merge in sign bit
        mov     DX,DS:[EBP].status      ; get status word
        and     DX,NOT SW_CC            ; turn off C0,C1,C2,C3
        or      DH,AL                   ; turn on new bits
        mov     DS:[EBP].status,DX      ; save in status word
        pop     EDX                     ; restore EDX
        pop     EBX                     ; restore EBX
        ret                             ; return
__fxam  endp

