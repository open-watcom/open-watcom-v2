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


include mdef.inc
include struct.inc

dgroup  group           _data
        modstart        hook,word
        assume          ds:dgroup,ss:dgroup

        xdefp   __PRO
        xdefp   __EPI

;
;       Every time a C function is called, space is set aside on the
;       stack and __PRO is called.  __PRO uses this stack to maintain a
;       stack containing the current callgraph information.  If this is
;       the first call to __PRO and a special environment variable is set
;       to indicate the presence of the sampler, an int 3 is issued which
;       tells the sampler where to find the communications area containing
;       information on where to find the callgraph information.
;

        defp    __PRO
        push    BP                      ; save registers
        mov     BP,SP
        push    AX
if _MODEL and _BIG_DATA
        push    DS
        mov     AX,DGROUP
        mov     DS,AX                   ; point DS to DGROUP
endif
        mov     AH,1
        xchg    AH,byte ptr in_hook     ; turn sampler off
        push    BX                      ; save lots more registers
        push    DX
        push    SI
        push    DI
        mov     SI,SS:0[BP]             ; this is the bp of the caller
        mov     DI,SS:0[SI]             ; the saved bp - bit 0 set if far call
        mov     AX,SS:2[BP]
        mov     word ptr top_ip,AX      ; top_ip = ip in function
if _MODEL and _BIG_CODE
        mov     AX,SS:4[BP]
        mov     word ptr top_cs,AX      ; top_cs = cs in function
        lea     BX,SS:6[BP]
else
        mov     word ptr top_cs,CS      ; top_cs = cs
        lea     BX,SS:4[BP]
endif
        mov     DX,word ptr ___cgraph_top
        mov     AX,word ptr ___cgraph_top+2
        mov     word ptr SS:[BX],DX
        mov     word ptr SS:2[BX],AX    ; storage->ptr = cgraph_top
        test    DI,1                    ; if we had a far call
        _if     ne                      ; then
        mov     AX,SS:4[SI]             ;    grab CS from stack
        _else                           ; else
if _MODEL and _BIG_CODE
        mov     AX,SS:4[BP]             ;    use function's CS if big code
else
        mov     AX,CS                   ;    use CS otherwise
endif
        _endif                          ; endif
        mov     word ptr SS:4[BX],AX    ; storage->cs = caller's cs
        mov     AX,SS:2[SI]
        mov     word ptr SS:6[BX],AX    ; storage->ip = caller's ip
        inc     word ptr push_no        ; push_no++
        mov     word ptr ___cgraph_top,BX       ; cgraph_top = storage
        mov     word ptr ___cgraph_top+2,SS
        cmp     byte ptr first_time,0
        jne     first_pass              ; pass common area if first entry
end_pro:
        pop     DI
        pop     SI
        pop     DX
        pop     BX
        xor     AH,AH
        xchg    AH,byte ptr in_hook     ; start sample collection again
if _MODEL and _BIG_DATA
        pop     DS                      ; restore registers
endif
        pop     AX
        pop     BP
        ret
first_pass:
        push    CX                      ; save more registers

;
; now check if our special environment variable is set (ie. the sampler
; is present).  If it is, execute this code to pass the common region to
; the sampler
;

; **************** ADD CODE HERE! **************************

        mov     BX,offset DGROUP:in_hook
        mov     CX,DS                   ;     pass common area address in CX:BX
        xor     AX,AX                   ;     set DX:AX to zero, to distinguish
        xor     DX,DX                   ;                this int 3 from a mark
        int     3                       ;     call sampler with common info

;

        mov     byte ptr first_time,0   ; turn off first_time flag
        pop     CX
        jmp     short end_pro           ; and return like usual!
        endproc __PRO

;
;       Just before each function return, __EPI is called.  __EPI shrinks
;       the callgraph stack by one entry, updating the necessary pointers.
;       Control is returned to the program which de-allocates the no longer
;       used stack storage space and returns.
;

        defp    __EPI
        push    BP                      ; save registers
        mov     BP,SP
        push    AX
if _MODEL and _BIG_DATA
        push    DS
        mov     AX,DGROUP
        mov     DS,AX                   ; point DS to DGROUP
endif
        mov     AH,1
        xchg    AH,byte ptr in_hook     ; turn sampler off
        push    BX
        push    ES
        cmp     word ptr push_no,0      ; if push_no == 0
        _if     e                       ; then
        inc     word ptr pop_no         ;     pop_no++;
        _else                           ; else
        dec     word ptr push_no        ;     push_no--;
        _endif                          ; endif
        les     BX,dword ptr ___cgraph_top
        mov     AX,word ptr ES:4[BX]
        mov     word ptr top_cs,AX      ; top_cs = cgraph_top->cs
        mov     AX,word ptr ES:6[BX]
        mov     word ptr top_ip,AX      ; top_ip = cgraph_top->ip
        mov     AX,word ptr ES:[BX]
        mov     BX,word ptr ES:2[BX]
        mov     word ptr ___cgraph_top,AX
        mov     word ptr ___cgraph_top+2,BX; cgraph_top = cgraph_top->ptr
        pop     ES                      ; restore some registers
        pop     BX
        xor     AH,AH
        xchg    AH,byte ptr in_hook     ; start collecting samples again
if _MODEL and _BIG_DATA
        pop     DS                      ; restore the rest
endif
        pop     AX
        pop     BP
        ret
        endproc __EPI

        dataseg
storage label   byte
        dd      0               ; ptr to next storage
        dw      0               ; cs of calling address
        dw      0               ; ip of calling address

        db      0, 0, 0         ; alignment junk

; The following variables are shared with the sampler, by means
; of an int 3 on the first pass through __PRO.  On writing a
; sample, the sampler is responsible for resetting pop_no and
; push_no to 0.

in_hook db      0               ; flag: in __PRO/__EPI?
public          ___cgraph_top   ; this is exported for now (until setjump/
___cgraph_top label byte        ; longjump are handled properly)
        dd      0               ; ptr to top of callgraph stack
top_ip  dw      0               ; ip of most recently called function
top_cs  dw      0               ; cs of most recently called function
pop_no  dw      0               ; # of entries to pop from prev sample
push_no dw      0               ; # of entries to push from prev sample

first_time label byte
        db      1               ; flags first time through __PRO
ENV_VAR label   byte
        db      43h,20h,4dh,20h,53h,20h,00h     ; env var set when sampling
        enddata

        endmod

        end
