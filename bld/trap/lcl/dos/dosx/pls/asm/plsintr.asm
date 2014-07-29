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


.386p

                name            phartrap

extrn           dbg_rdides      : near
extrn           dbg_wrides      : near

idtentry        struc
                loffs   dw      0
                select  dw      0
                wcount  db      0
                arights db      0
                hoffs   dw      0
idtentry        ends

intvars         macro   num
i&num&off       dd              0
i&num&seg       dw              0
                endm

saveintr        macro   num,reg                 ; modifies ds,ax,cl
            ifidn   <num>,<KB>
                mov     cl,&reg
            else
                mov     cl,&num
            endif
                mov     ax,2502H                ; get interrupt request
                int     21H
                jc      false
                mov     ds,cs:dataseg
                mov     ds:i&num&off,ebx
                mov     ds:i&num&seg,es
endm

setintr         macro   num,mode
            ifidn       <num>,<KB>
                mov     cl,&mode
            else
                mov     cl,&num
            endif
            ifidn       <mode>,<both>
                mov     ax,2506H                ; set pm interrupt
            else
                mov     ax,2504H                ; set pm interrupt
            endif
                mov     dx,cs                   ; set regs for set interrupt req
                mov     ds,dx
                lea     edx,int&num
                int     21H
        end&num&:
endm

restintr        macro   num,reg
            ifidn       <num>,<KB>
                mov     cl,&reg
            else
                mov     cl,&num
            endif
                mov     es,cs:dataseg           ; get addressability
                mov     ax,2504H                ; set pm interrupt
                lds     edx,fword ptr es:i&num&off
                cmp     word ptr es:i&num&seg,0
                je      no&num
                int     21h
                mov     word ptr es:i&num&seg,0
            no&num:
endm


_data segment word public 'data'
saveesp         dd              0
saveeip         dd              0
savecs          dd              0
savefl          dd              0
int5real        dd              0
                intvars 05H
                intvars 20H
                intvars 21H
                intvars 27H
                intvars KB
kbloffs         dw              0
kbhoffs         dw              0
kbselect        dw              0
kbarights       db              0,0
dataseg         dw              0
spawned         db              0
break_hit       db              0
extrn           _XVersion       : byte
extrn           _SavedByte      : byte
extrn           _AtEnd          : byte
extrn           _FakeBreak      : byte
extrn           _InitialSS      : word
extrn           _InitialCS      : word
sysregs dd      14 dup(0)       ; only need 12, but just in case
_data ends


dgroup group _data

assume  ds:DGROUP

_text segment byte public 'code'

int05H          proc    near
                push    ds                      ; save ds
                mov     ds,cs:dataseg           ; set it up
                mov     ds:break_hit,1          ; break hit!
                pop     ds                      ; restore ds
                iretd
int05H          endp

public          intKB
intKB           proc    near
                push    eax                     ; save some regs
                push    ds                      ; ...
                mov     ds,cs:dataseg           ; get addressability
                mov     eax,08H[esp]            ; remember the old return frame
                mov     saveeip,eax             ; ...
                mov     eax,0CH[esp]            ; ...
                mov     savecs,eax              ; ...
                mov     eax,10H[esp]            ; ...
                mov     savefl,eax              ; ...
                mov     saveesp,esp             ; remember esp
                add     saveesp,5*4             ; ... after we've old frame
                mov     break_hit,0             ; PrtScrn not hit
                pop     ds                      ; restore some regs
                pop     eax                     ; ...
                add     esp,0CH                 ; get rid of old return frame

                cmp     cs:_XVersion,3          ; if version >= 3
                jl      ver2                    ; - then
                pushfd                          ; invoke the old handler
                push    cs                      ; ...
                push    offset return           ; ...
                jmp     chip_bug_1              ; bug in the chip
chip_bug_1:     jmp     fword ptr cs:iKBoff     ; ...
ver2:
                pushfd                          ; push flags
                jmp     chip_bug_2              ; BUG IN THE CHIP!!! (don't ask)
chip_bug_2:     call    fword ptr cs:kbloffs    ; chain to old handler

return:         mov     esp,cs:saveesp          ; old handler might not pop FL!
                push    ds                      ; save regs again
                push    eax                     ; ...
                push    ebx                     ; ...
                mov     ds,cs:dataseg           ; get addressability
                cmp     break_hit,0             ; if break wasn't hit
                je      alldone                 ; - return
                cmp     _FakeBreak,0            ; if planted BreakPoint already
                jne     alldone                 ; - return
                cmp     _XVersion,3             ; if version < 3
                jge     ver3                    ; - then
                mov     eax,savecs              ; - fish out return CS value
                mov     ebx,saveeip             ; - fish out return EIP value
                jmp     endver                  ; else
ver3:           mov     eax,10H[esp]            ; - fish out return CS value
                mov     ebx,0CH[esp]            ; - fish out return EIP value
endver:                                         ; endif
                cmp     ax,_InitialCS           ; if not in user's data space
                jne     alldone                 ; - return
                mov     es,_InitialSS           ; writable segment to app's code
                mov     al,es:[ebx]             ; get byte of next instruction
                mov     _SavedByte,al           ; ... save it
                mov     _FakeBreak,1            ; indicate we have fake break
                mov     byte ptr es:[ebx],0CCH  ; ... set break point
alldone:        mov     break_hit,0             ; reset break hit

                pop     ebx                     ; restore regs
                pop     eax                     ; restore regs
                pop     ds                      ; ...

                push    cs:savefl               ; restore original stack frame
                push    cs:savecs               ; ...
                push    cs:saveeip              ; ...
                jmp     chip_bug_3              ;
chip_bug_3:     iretd                           ; return to caller
intKB           endp

public          int20H
int20H          proc    near                    ; terminate request
                push    ds                      ; save ds
                mov     ds,cs:dataseg           ; set it up
                dec     byte ptr spawned        ; if no spawned subprocess
                pop     ds                      ; restore ds
                js      fakebreak               ; - fake up a break point
                jmp     fword ptr cs:i20Hoff    ; chain to old handler
int20H          endp

public          int21H
int21H          proc    near                    ; interrupt 21 handler
                cmp     ah,4cH                  ; if terminate request
                je      terminate               ; - then
                cmp     ah,0                    ; if terminate
                je      terminate               ; - then
                cmp     ax,25c3H                ; if terminate
                je      spawn                   ; -
                cmp     ah,4bh                  ; if spawn request
                jne     chain21                 ; - then
spawn:          push    ds                      ; save ds
                mov     ds,cs:dataseg           ; set it up
                inc     byte ptr spawned        ; - keep track of it
                pop     ds                      ; restore ds
chain21:        jmp     fword ptr cs:i21Hoff    ; chain to old handler
terminate:      push    ds                      ; save ds
                mov     ds,cs:dataseg           ; set it up
                dec     byte ptr spawned        ; - if no spawned a subprocess
                pop     ds                      ; restore ds
                js      fakebreak               ; - - fake a break point
                jmp     short chain21           ; chain to old handler
int21H          endp                            ; whew!

public          int27H
int27H          proc    near                    ; terminate request
                push    ds                      ; save ds
                mov     ds,cs:dataseg           ; set it up
                dec     byte ptr spawned        ; if no spawned subprocess
                pop     ds                      ; restore ds
                js      fakebreak               ; - fake up a break point
                jmp     fword ptr cs:i27Hoff    ; chain to old handler
int27H          endp

public          fakebreak
fakebreak       proc    near
                push    eax                     ; save a register
                push    ebx                     ; and another
                push    esi                     ; and another
                push    ds                      ; ...
                push    es                      ; ...
                mov     ds,cs:dataseg           ; get addressability
                xor     esi,esi                 ; eax = 12 if >= version 3
                cmp     byte ptr ds:_XVersion,3 ; if version 3 of extender
                jl      less3                   ; ...
                mov     esi,0cH                 ; ...
less3:          mov     ebx,14h[esi+esp]        ; fish stacked return address
                mov     es,18h[esi+esp]         ; ...
                dec     ebx                     ; back it up one
                dec     ebx                     ; back it up one
                mov     al,es:[ebx]             ; get last byte of instruction
                mov     ds:_SavedByte,al        ; save the last byte of instr
                mov     ds:_FakeBreak,1         ; indicate we have fake break
                mov     ds:_AtEnd,1             ; indicate we are terminated
                mov     es,ds:_InitialSS        ; ...
                mov     byte ptr es:[ebx],0CCH  ; ... with a break point instr
                mov     14h[esi+esp],ebx        ; save return address
                pop     es                      ; restore regs
                pop     ds                      ; ...
                pop     esi                     ; ...
                pop     ebx                     ; ...
                pop     eax                     ; ...
                iretd                           ; return to bkpt instruction
fakebreak       endp                            ; double whew!

public          GrabPrtScrn_
GrabPrtScrn_    proc    near
                push    ecx                     ; save registers
                push    ebx                     ; ...
                push    edx                     ; ...
                push    es                      ; ...
                push    ds                      ; ...

                mov     dataseg,ds              ; stash ds for addressability

                saveintr 05H                    ; save protected mode int 5

                mov     ax,2503h                ; save old int5 real mode
                mov     cl,5                    ; ...
                int     21h                     ; ...
                mov     int5real,ebx            ; ...

                setintr 05H,both

                pop     ds                      ; restore registers
                pop     es                      ; ...
                pop     edx                     ; ...
                pop     ebx                     ; ...
                pop     ecx                     ; ...
                ret
GrabPrtScrn_    endp

public          RelePrtScrn_
RelePrtScrn_    proc    near
                push    ecx                     ; save registers
                push    ebx                     ; ...
                push    edx                     ; ...
                push    es                      ; ...
                push    ds                      ; ...

                mov     ebx,int5real            ; restore real mode int 5
                mov     cl,5                    ; ...
                mov     ax,2505h                ; ...
                int     21h                     ; ...

                restintr 05H                    ; restore protected int 5

                pop     ds                      ; restore registers
                pop     es                      ; ...
                pop     edx                     ; ...
                pop     ebx                     ; ...
                pop     ecx                     ; ...
                ret
RelePrtScrn_    endp


public          GrabVects_
GrabVects_      proc near
                push    ecx                     ; save registers
                push    ebx                     ; ...
                push    edx                     ; ...
                push    es                      ; ...
                push    ds                      ; ...

                mov     ax,250cH                ; get hw interrupt vectors
                int     21H                     ; ...
                add     al,1                    ; this is keyboard interrupt
                cmp     _XVersion,3             ; if version >= 3
                jl      grab_idt                ; - then
                push    eax                     ; - remember kb interrupt num
                saveintr KB,al                  ; - save the keyboard handler
                pop     eax                     ; - set new keyboard handler
                setintr KB,al                   ; - ...
                jmp     done_idt                ; else
grab_idt:       movzx   ebx,al                  ; - point to IDT entry
                shl     ebx,3                   ; - ...
                mov     ax,50h                  ; - ...
                mov     es,ax                   ; - ...
                cli                             ; - interrupts off
                mov     ax,es:[ebx].hoffs       ; - save IDT entry
                mov     kbhoffs,ax              ; - ...
                mov     ax,es:[ebx].loffs       ; - ...
                mov     kbloffs,ax              ; - ...
                mov     ax,es:[ebx].select      ; - ...
                mov     kbselect,ax             ; - ...
                mov     al,es:[ebx].arights     ; - ...
                mov     kbarights,al            ; - ...
                lea     eax,intKB               ; - set new keyboard handler
                mov     es:[ebx].loffs,ax       ; - ...
                shr     eax,16                  ; - ...
                mov     es:[ebx].hoffs,ax       ; - ...
                mov     es:[ebx].select,cs      ; - ...
                mov     es:[ebx].arights,08EH   ; - ...
                sti                             ; - interrupts back on
done_idt:                                       ; endif

                saveintr 20H
                setintr 20H
                saveintr 21H
                setintr 21H
                saveintr 27H
                setintr 27H

                mov     eax,1                   ; return true
                jmp     epi                     ; ...
false:          xor     eax,eax                 ; return false
epi:            pop     ds                      ; restore registers
                pop     es                      ; ...
                pop     edx                     ; ...
                pop     ebx                     ; ...
                pop     ecx                     ; ...
                ret                             ; return to caller
GrabVects_      endp

public          ReleVects_
ReleVects_      proc near
                push    ecx                     ; save registers
                push    edx                     ; ...
                push    ds                      ; ...
                push    es                      ; ...

                cmp     _XVersion,3             ; if version >= 3
                jl      fix_idt                 ; - then
                mov     ax,250cH                ; - get hw interrupt vectors
                int     21H                     ; - ...
                add     al,1                    ; - this is keyboard interrupt
                restintr KB,al                  ; - restore it
                jmp     fixed_idt               ; else
fix_idt:        cmp     byte ptr kbarights,0    ; - if we have taken it over
                je      fixed_idt               ; - - then

                mov     ax,250cH                ; - - get hw interrupt vectors
                int     21H                     ; - - ...
                add     al,1                    ; - - this is keyboard interrupt
                movzx   ebx,al                  ; - - ...
                shl     ebx,3                   ; - - ...
                mov     ax,50h                  ; - - point to IDT
                mov     es,ax                   ; - - ...

                cli                             ; - ...
                mov     ax,kbhoffs              ; - ...
                mov     es:[ebx].hoffs,ax       ; - ...
                mov     ax,kbloffs              ; - ...
                mov     es:[ebx].loffs,ax       ; - save offset
                mov     ax,kbselect             ; - ...
                mov     es:[ebx].select,ax      ; - save offset
                mov     al,kbarights            ; - ...
                mov     es:[ebx].arights,al     ; - ...
                sti                             ; - ...
                                                ; - endif
fixed_idt:                                      ; endif
                restintr 20H
                restintr 21H
                restintr 27H

                pop     es                      ; restore registers
                pop     ds                      ; ...
                pop     edx                     ; ...
                pop     ecx                     ; ...
                ret                             ; return to caller
ReleVects_      endp

public          SetPSP_
SetPSP_         proc near
                push    ebx
                mov     bx,ax
                mov     ah,50H
                int     21H
                pop     ebx
                ret
SetPSP_         endp

public          SetMSW_
SetMSW_         proc near
                push    edx             ; save regs
                push    ecx             ; ...
                push    ebx             ; ...
                mov     ecx,eax         ; save new CR0 value
                and     ecx, 6          ; isolate EM, MP bits
                mov     ax,cs           ; get CPL
                and     ax,3            ; ...
                je      ring_0          ; if ring_0, do the stuff ourselves
                sub     esp,48          ; allocate buffer space
                mov     edx,esp         ; point at buffer
                xor     ebx,ebx         ; read system regs
                mov     ax,2535H        ; ...
                int     21H             ; ...
                and     byte ptr [edx], 0f9h; turn off EM, MP bits
                or      [edx],ecx       ; set new CR0 value (EM, MP bits)
                inc     ebx             ; write system regs
                int     21H             ; ...
                add     esp,48          ; dealloc buffer space
done:
                pop     ebx
                pop     ecx
                pop     edx
                ret
ring_0:
                mov     eax,cr0         ; get current
                and     eax,NOT 6       ; turn off EM, MP bits
                or      ecx,eax         ; merge two CR0's together
                mov     cr0,ecx         ; retore EM, MP bits to original
                jmp     short done
SetMSW_         endp

_text           ends

                end
