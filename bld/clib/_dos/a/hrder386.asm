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
;
include mdef.inc
include struct.inc
include extender.inc

        name    harderr

        extrn   __GETDS         : near
        extrn   "C",_Extender   : byte
_TEXT   segment word public 'CODE'
_TEXT   ends

_DATA   segment word public 'DATA'
        extrn   "C",_STACKLOW   : dword
_DATA   ends

_BSS    segment word public 'BSS'
_BSS    ends

DGROUP  group   _DATA,_BSS
        assume  cs:_TEXT, ds: DGROUP, ss: DGROUP, es:nothing

_DATA   segment
oldsp   dd      0
oldss   dw      0
int24ip dd      0
int24cs dw      0
oldstklow dd    0
_DATA   ends

_BSS    segment
temp_stacklow label dword
        db      1024 dup(?)
temp_stack label dword
_BSS    ends

_TEXT   segment

        defp    _harderr
        if __WASM__ ge 100
            xdefp       "C",_harderr
        else
            xdefp       <"C",_harderr>
        endif
;
;       void _harderr( void (far *fptr)() );
;
        push    EDX             ; save EDX
ifdef __STACK__
        mov     EAX,8[ESP]      ; get offset
        mov     EDX,12[ESP]     ; get segment
endif
        mov     int24ip,EAX     ; save address of users int 24 handler
        mov     int24cs,DX      ; ...
        push    DS              ; save DS
        mov     AX,CS           ; set DS=CS
        mov     DS,AX           ; ...
        mov     EDX,offset int24rtn ; point to int 24 interrupt handler
;
;       need to set interrupt so that we gain control in protect mode
;
        mov     AL,_Extender    ; get extender indicator
        _guess                  ; guess: pharlap
          cmp   AL,X_PHARLAP_V2 ; - ...
          _quif b               ; - quit if not pharlap
          cmp   AL,X_PHARLAP_V7 ; - ...
          _quif a               ; - quit if not pharlap
          push  ECX             ; - save ECX
          mov   CL,24H          ; - critical error interrupt number
          mov   AL,06H          ; - set interrupt to always gain control
          mov   AH,25H          ; - ... in protected mode
          int   21H             ; - ...
          pop   ECX             ; - restore ECX
        _admit                  ; assume: DOS/4GW or Ergo OS386
          mov   AL,24h          ; - critical error interrupt number
          mov   AH,25H          ; - set interrupt vector
          int   21h             ; - ...
        _endguess               ; endguess
        sub     EAX,EAX         ; set success
        pop     DS              ; restore DS
        pop     EDX             ; restore EDX
        ret                     ; return to caller
        endproc _harderr
        if __WASM__ ge 100
            xdefp       "C",_hardresume
        else
            xdefp       <"C",_hardresume>
        endif

        defp    int24rtn
        push    ES              ; save registers
        push    DS              ; ...
        push    EBP             ; ...
        push    EDI             ; ...
        push    ESI             ; ...
        push    EDX             ; ...
        push    ECX             ; ...
        push    EBX             ; ...
        push    EAX             ; ...
        mov     EDX,EAX         ; save EAX
        call    __GETDS         ; get DGROUP
        mov     AX,DS           ; ...
        mov     ES,AX           ; ...
        mov     oldsp,ESP       ; save the stack pointer
        mov     oldss,SS        ; save SS
        mov     ECX,_STACKLOW   ; save old stack low value
        mov     oldstklow,ECX   ; ...
        lea     ECX,DGROUP:temp_stacklow; setup stacklow variable
        mov     _STACKLOW,ECX   ; ...
        mov     SS,AX           ; point to stack in DGROUP
        lea     ESP,DGROUP:temp_stack;...
        sub     EBX,EBX         ; zero offset
        mov     BX,BP           ; get 16-bit real-mode segment
        shl     EBX,4           ; shift left 4
        and     ESI,0000FFFFh   ; mask off top bits     18-feb-94
        add     EBX,ESI         ; add in the offset of device header
        sub     ECX,ECX         ; zero ECX for selector to use
        mov     AL,_Extender    ; get extender indicator
        _guess                  ; guess: pharlap
          cmp   AL,X_PHARLAP_V2 ; - ...
          _quif b               ; - quit if not pharlap
          cmp   AL,X_PHARLAP_V7 ; - ...
          _quif a               ; - quit if not pharlap
          mov   CL,34H          ; - GDT entry for first 1 meg
        _admit                  ; guess: DOS/4GW
          cmp   AL,X_RATIONAL   ; - ...
          _quif ne              ; - quit if not DOS4GW
          mov   CX,DS           ; - just use DS
        _admit                  ; assume: Ergo OS386
          mov   CL,34H          ; - GDT entry for first 1 meg
        _endguess               ; endguess
        movzx   EAX,DX          ; get deverror
        movzx   EDX,DI          ; get error code
        push    ECX             ; push parms on stack for 3s libraries
        push    EBX             ; ...
        push    EDX             ; ...
        push    EAX             ; ...
        sub     EBP,EBP         ; zero EBP
        call    pword ptr int24ip; invoke user's int 24 handler
ifdef __STACK__
        push    EAX             ; push action code
        push    EAX             ; ...
endif


_hardresume:
;
;       AX - contains an action code
;       0 - IGNORE
;       1 - RETRY
;       2 - ABORT
;       3 - FAIL
;
ifdef __STACK__
        mov     EAX,4[ESP]      ; get action code
endif
        lss     ESP,pword ptr oldsp     ; restore SS:ESP
        mov     ECX,oldstklow   ; restore old stacklow variable
        mov     _STACKLOW,ECX   ; ...
        pop     EBX             ; skip over saved EAX
        pop     EBX             ; restore registers
        pop     ECX             ; ...
        pop     EDX             ; ...
        pop     ESI             ; ...
        pop     EDI             ; ...
        pop     EBP             ; ...
        pop     DS              ; ...
        pop     ES              ; ...
        iretd                   ; return from interrupt

        endproc int24rtn

        endmod
        end
